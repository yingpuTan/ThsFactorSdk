#include "ThsFactorSdk.h"
#include "ThsFactorSdkSync.h"
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <memory>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <random>

// 同步管理器类
class SyncManager {
private:
    struct PendingRequest {
        std::condition_variable cv;
        std::mutex mutex;
        SyncResponse response;
        bool completed;
        std::chrono::steady_clock::time_point timestamp;
        
        // 存储响应数据的字符串，确保数据生命周期
        std::string response_data;
        std::string error_message;
        
        PendingRequest() : completed(false) {}
    };

    std::map<std::string, std::shared_ptr<PendingRequest>> pending_requests;
    std::mutex requests_mutex;
    std::atomic<bool> initialized{false};
    OnPushCb user_push_callback{nullptr};
    
    // 异步回调函数
    static void OnLoginCallback(const char* result, int len);
    static void OnQueryCallback(const char* result, int len);
    static void OnSubscribeCallback(const char* result, int len);
    static void OnUnSubscribeCallback(const char* result, int len);
    static void OnPushCallback(const char* push, int len);
    static void OnSessionCallback(const char* session, int len);

    // 辅助函数
    static std::string ExtractUuidFromJson(const std::string& json_str);
    static bool ParseJsonResponse(const std::string& json_str, SyncResponse& response, PendingRequest* request = nullptr);

public:
    static SyncManager& GetInstance() {
        static SyncManager instance;
        return instance;
    }

    bool Initialize(OnPushCb pushCallback = nullptr) {
        if (initialized.load()) return true;
        
        // 保存用户推送回调
        user_push_callback = pushCallback;
        
        // 注册异步回调
        int ret = RegisterCallback(
            OnLoginCallback,
            OnQueryCallback, 
            OnSubscribeCallback,
            OnUnSubscribeCallback,
            OnPushCallback,
            OnSessionCallback
        );
        
        if (ret == 0) {
            initialized.store(true);
            
            // 启动定期清理线程
            std::thread([this]() {
                while (initialized.load()) {
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                    CleanupExpiredRequests();
                }
            }).detach();
            
            return true;
        }
        return false;
    }

    void Cleanup() {
        std::lock_guard<std::mutex> lock(requests_mutex);
        pending_requests.clear();
        initialized.store(false);
        user_push_callback = nullptr;
    }

    std::string GenerateUuid() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
        
        uint32_t data[4];
        for (int i = 0; i < 4; ++i) data[i] = dis(gen);
        
        std::stringstream ss;
        ss << std::hex << std::setfill('0')
           << std::setw(8) << data[0] << "-"
           << std::setw(4) << ((data[1] >> 16) & 0xFFFF) << "-"
           << std::setw(4) << (data[1] & 0xFFFF) << "-"
           << std::setw(4) << ((data[2] >> 16) & 0xFFFF) << "-"
           << std::setw(4) << (data[2] & 0xFFFF)
           << std::setw(8) << data[3];
        return ss.str();
    }

    SyncResponse WaitForResponse(const std::string& uuid, int timeout_ms) {
        std::shared_ptr<PendingRequest> request;
        
        {
            std::lock_guard<std::mutex> lock(requests_mutex);
            request = std::make_shared<PendingRequest>();
            request->timestamp = std::chrono::steady_clock::now();
            pending_requests[uuid] = request;
        }

        std::unique_lock<std::mutex> lock(request->mutex);
        bool success = request->cv.wait_for(lock, 
                                          std::chrono::milliseconds(timeout_ms),
                                          [request] { return request->completed; });

        // 清理请求
        {
            std::lock_guard<std::mutex> lock(requests_mutex);
            pending_requests.erase(uuid);
        }

        if (!success) {
            SyncResponse timeout_response;
            timeout_response.code = -1;
            timeout_response.message = "请求超时";
            timeout_response.data = "";
            timeout_response.data_owner = nullptr;
            return timeout_response;
        }

        // 创建完整的副本，避免野指针
        SyncResponse result = request->response;
        
        // 确保data指针指向有效的内存
        if (result.data && strlen(result.data) > 0) {
            // 将数据复制到独立的存储中
            char* data_copy = new char[request->response_data.length() + 1];
            strcpy(data_copy, request->response_data.c_str());
            result.data = data_copy;
            result.data_owner = data_copy;  // 设置数据所有者
        } else {
            result.data_owner = nullptr;
        }
        
        return result;
    }

    void HandleResponse(const std::string& uuid, const SyncResponse& response) {
        std::lock_guard<std::mutex> lock(requests_mutex);
        auto it = pending_requests.find(uuid);
        if (it != pending_requests.end()) {
            auto request = it->second;
            std::lock_guard<std::mutex> req_lock(request->mutex);
            request->response = response;
            request->completed = true;
            request->cv.notify_one();
        }
    }

    void CleanupExpiredRequests() {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(requests_mutex);
        
        auto it = pending_requests.begin();
        while (it != pending_requests.end()) {
            auto request = it->second;
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - request->timestamp).count();
            
            if (duration > 30000) { // 30秒超时清理
                it = pending_requests.erase(it);
            } else {
                ++it;
            }
        }
    }
};

// 从JSON字符串中提取UUID
std::string SyncManager::ExtractUuidFromJson(const std::string& json_str) {
    // 简单的UUID提取逻辑
    // 查找 "id": "uuid" 模式
    size_t pos = json_str.find("\"id\":\"");
    if (pos != std::string::npos) {
        pos += 6; // 跳过 "id":"
        size_t end = json_str.find("\"", pos);
        if (end != std::string::npos) {
            return json_str.substr(pos, end - pos);
        }
    }
    
    // 查找 "uuid": "uuid" 模式
    pos = json_str.find("\"uuid\":\"");
    if (pos != std::string::npos) {
        pos += 8; // 跳过 "uuid":"
        size_t end = json_str.find("\"", pos);
        if (end != std::string::npos) {
            return json_str.substr(pos, end - pos);
        }
    }
    
    return "";
}

// 解析JSON响应
bool SyncManager::ParseJsonResponse(const std::string& json_str, SyncResponse& response, PendingRequest* request) {
    // 简单的JSON解析逻辑
    // 检查是否包含错误信息
    if (json_str.find("\"error\"") != std::string::npos) {
        response.code = -1;
        response.message = "服务器返回错误";
        
        // 尝试提取错误信息
        size_t pos = json_str.find("\"message\":\"");
        if (pos != std::string::npos) {
            pos += 11; // 跳过 "message":"
            size_t end = json_str.find("\"", pos);
            if (end != std::string::npos) {
                std::string error_msg = json_str.substr(pos, end - pos);
                if (request) {
                    request->error_message = error_msg;
                    response.message = request->error_message.c_str();
                } else {
                    response.message = "服务器返回错误";
                }
            } else {
                response.message = "服务器返回错误";
            }
        }
    } else if (json_str.find("\"result\"") != std::string::npos) {
        response.code = 0;
        response.message = "操作成功";
    } else {
        response.code = 0;
        response.message = "操作完成";
    }
    
    // 存储响应数据到request中，确保数据生命周期
    if (request) {
        request->response_data = json_str;
        response.data = request->response_data.c_str();
    } else {
        response.data = "";
    }
    return true;
}

// 异步回调实现
void SyncManager::OnLoginCallback(const char* result, int len) {
    if (!result || len <= 0) return;
    
    std::string response_str(result, len);
    std::string uuid = ExtractUuidFromJson(response_str);
    
    if (!uuid.empty()) {
        // 获取对应的request对象
        auto& instance = GetInstance();
        std::shared_ptr<PendingRequest> request;
        
        {
            std::lock_guard<std::mutex> lock(instance.requests_mutex);
            auto it = instance.pending_requests.find(uuid);
            if (it != instance.pending_requests.end()) {
                request = it->second;
            }
        }
        
        if (request) {
            // 直接操作request，避免再次锁定mutex
            std::lock_guard<std::mutex> req_lock(request->mutex);
            
            // 先存储数据到request中
            request->response_data = response_str;
            request->error_message = "";
            
            // 然后解析响应
            ParseJsonResponse(response_str, request->response, request.get());
            
            request->completed = true;
            request->cv.notify_one();
        }
    }
}

void SyncManager::OnQueryCallback(const char* result, int len) {
    if (!result || len <= 0) return;
    
    std::string response_str(result, len);
    std::string uuid = ExtractUuidFromJson(response_str);
    
    if (!uuid.empty()) {
        // 获取对应的request对象
        auto& instance = GetInstance();
        std::shared_ptr<PendingRequest> request;
        
        {
            std::lock_guard<std::mutex> lock(instance.requests_mutex);
            auto it = instance.pending_requests.find(uuid);
            if (it != instance.pending_requests.end()) {
                request = it->second;
            }
        }
        
        if (request) {
            // 直接操作request，避免再次锁定mutex
            std::lock_guard<std::mutex> req_lock(request->mutex);
            
            // 先存储数据到request中
            request->response_data = response_str;
            request->error_message = "";
            
            // 然后解析响应
            ParseJsonResponse(response_str, request->response, request.get());
            
            request->completed = true;
            request->cv.notify_one();
        }
    }
}

void SyncManager::OnSubscribeCallback(const char* result, int len) {
    if (!result || len <= 0) return;
    
    std::string response_str(result, len);
    std::string uuid = ExtractUuidFromJson(response_str);
    
    if (!uuid.empty()) {
        // 获取对应的request对象
        auto& instance = GetInstance();
        std::shared_ptr<PendingRequest> request;
        
        {
            std::lock_guard<std::mutex> lock(instance.requests_mutex);
            auto it = instance.pending_requests.find(uuid);
            if (it != instance.pending_requests.end()) {
                request = it->second;
            }
        }
        
        if (request) {
            // 直接操作request，避免再次锁定mutex
            std::lock_guard<std::mutex> req_lock(request->mutex);
            
            // 先存储数据到request中
            request->response_data = response_str;
            request->error_message = "";
            
            // 然后解析响应
            ParseJsonResponse(response_str, request->response, request.get());
            
            request->completed = true;
            request->cv.notify_one();
        }
    }
}

void SyncManager::OnUnSubscribeCallback(const char* result, int len) {
    if (!result || len <= 0) return;
    
    std::string response_str(result, len);
    std::string uuid = ExtractUuidFromJson(response_str);
    
    if (!uuid.empty()) {
        // 获取对应的request对象
        auto& instance = GetInstance();
        std::shared_ptr<PendingRequest> request;
        
        {
            std::lock_guard<std::mutex> lock(instance.requests_mutex);
            auto it = instance.pending_requests.find(uuid);
            if (it != instance.pending_requests.end()) {
                request = it->second;
            }
        }
        
        if (request) {
            // 直接操作request，避免再次锁定mutex
            std::lock_guard<std::mutex> req_lock(request->mutex);
            
            // 先存储数据到request中
            request->response_data = response_str;
            request->error_message = "";
            
            // 然后解析响应
            ParseJsonResponse(response_str, request->response, request.get());
            
            request->completed = true;
            request->cv.notify_one();
        }
    }
}

void SyncManager::OnPushCallback(const char* push, int len) {
    // 调用用户的推送回调函数
    if (GetInstance().user_push_callback && push && len > 0) {
        GetInstance().user_push_callback(push, len);
    }
}

void SyncManager::OnSessionCallback(const char* session, int len) {
    // Session状态回调，通常不需要同步处理
    // 可以在这里添加session状态变化的处理逻辑
    if (session && len > 0) {
        std::string session_data(session, len);
        // TODO: 添加session状态处理逻辑
    }
}

// 同步接口实现
extern "C" {

int InitSyncManager(OnPushCb pushCallback) {
    return SyncManager::GetInstance().Initialize(pushCallback) ? 0 : -1;
}

int CleanupSyncManager() {
    SyncManager::GetInstance().Cleanup();
    return 0;
}

SyncResponse LoginSync(const LoginParam* param, int timeout_ms) {
    if (!param) {
        SyncResponse response;
        response.code = -1;
        response.message = "登录参数为空";
        response.data = "";
        return response;
    }
    
    if (!SyncManager::GetInstance().Initialize()) {
        SyncResponse response;
        response.code = -1;
        response.message = "同步管理器初始化失败";
        response.data = "";
        return response;
    }
    
    std::string uuid = SyncManager::GetInstance().GenerateUuid();
    int ret = Login(param);
    
    if (ret != 0) {
        SyncResponse response;
        response.code = ret;
        response.message = "登录请求发送失败";
        response.data = "";
        return response;
    }
    
    return SyncManager::GetInstance().WaitForResponse(uuid, timeout_ms);
}

SyncResponse LogoutSync(int timeout_ms) {
    if (!SyncManager::GetInstance().Initialize()) {
        SyncResponse response;
        response.code = -1;
        response.message = "同步管理器初始化失败";
        response.data = "";
        return response;
    }
    
    std::string uuid = SyncManager::GetInstance().GenerateUuid();
    int ret = Logout();
    
    if (ret != 0) {
        SyncResponse response;
        response.code = ret;
        response.message = "登出请求发送失败";
        response.data = "";
        return response;
    }
    
    return SyncManager::GetInstance().WaitForResponse(uuid, timeout_ms);
}

SyncResponse QuerySync(const char* type, const char* begin, const char* end, int timeout_ms) {
    if (!type) {
        SyncResponse response;
        response.code = -1;
        response.message = "查询类型为空";
        response.data = "";
        return response;
    }
    
    if (!begin || !end) {
        SyncResponse response;
        response.code = -1;
        response.message = "时间参数为空";
        response.data = "";
        return response;
    }
    
    // 验证时间格式 (YYYYMMDDHHmmss)
    std::string begin_str(begin);
    std::string end_str(end);
    if (begin_str.length() != 14 || end_str.length() != 14) {
        SyncResponse response;
        response.code = -1;
        response.message = "时间格式错误，应为YYYYMMDDHHmmss格式";
        response.data = "";
        return response;
    }
    
    // 验证时间范围
    if (begin_str >= end_str) {
        SyncResponse response;
        response.code = -1;
        response.message = "开始时间必须早于结束时间";
        response.data = "";
        return response;
    }
    
    if (!SyncManager::GetInstance().Initialize()) {
        SyncResponse response;
        response.code = -1;
        response.message = "同步管理器初始化失败";
        response.data = "";
        return response;
    }
    
    std::string uuid = SyncManager::GetInstance().GenerateUuid();
    int ret = Query(type, begin, end, uuid.c_str());
    
    if (ret != 0) {
        SyncResponse response;
        response.code = ret;
        response.message = "查询请求发送失败";
        response.data = "";
        return response;
    }
    
    return SyncManager::GetInstance().WaitForResponse(uuid, timeout_ms);
}

SyncResponse SubscribeSync(const char* type, int timeout_ms) {
    if (!type) {
        SyncResponse response;
        response.code = -1;
        response.message = "订阅类型为空";
        response.data = "";
        return response;
    }
    
    if (!SyncManager::GetInstance().Initialize()) {
        SyncResponse response;
        response.code = -1;
        response.message = "同步管理器初始化失败";
        response.data = "";
        return response;
    }
    
    std::string uuid = SyncManager::GetInstance().GenerateUuid();
    int ret = Subscribe(type, uuid.c_str());
    
    if (ret != 0) {
        SyncResponse response;
        response.code = ret;
        response.message = "订阅请求发送失败";
        response.data = "";
        return response;
    }
    
    return SyncManager::GetInstance().WaitForResponse(uuid, timeout_ms);
}

SyncResponse UnSubscribeSync(const char* type, int timeout_ms) {
    if (!type) {
        SyncResponse response;
        response.code = -1;
        response.message = "取消订阅类型为空";
        response.data = "";
        response.data_owner = nullptr;
        return response;
    }
    
    if (!SyncManager::GetInstance().Initialize()) {
        SyncResponse response;
        response.code = -1;
        response.message = "同步管理器初始化失败";
        response.data = "";
        response.data_owner = nullptr;
        return response;
    }
    
    std::string uuid = SyncManager::GetInstance().GenerateUuid();
    int ret = UnSubscribe(type, uuid.c_str());
    
    if (ret != 0) {
        SyncResponse response;
        response.code = ret;
        response.message = "取消订阅请求发送失败";
        response.data = "";
        response.data_owner = nullptr;
        return response;
    }
    
    return SyncManager::GetInstance().WaitForResponse(uuid, timeout_ms);
}

void CleanupSyncResponse(SyncResponse* response) {
    if (response && response->data_owner) {
        delete[] static_cast<char*>(response->data_owner);
        response->data_owner = nullptr;
        response->data = nullptr;
    }
}

}