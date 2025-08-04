#include "ThsFactorSdk.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include <iostream>
#include <chrono>
#include <thread>

// 推送回调函数 - 解析JSON数据
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    
    std::string push_data(push, len);
    std::cout << "\n=== 收到推送数据 ===" << std::endl;
    std::cout << "原始数据: " << push_data << std::endl;
    
    // 尝试解析JSON数据
    rapidjson::Document doc;
    if (!doc.Parse(push_data.c_str()).HasParseError()) {
        if (doc.HasMember("type") && doc["type"].IsString()) {
            std::cout << "数据类型: " << doc["type"].GetString() << std::endl;
        }
        if (doc.HasMember("data") && doc["data"].IsString()) {
            std::cout << "数据内容: " << doc["data"].GetString() << std::endl;
        }
        if (doc.HasMember("timestamp") && doc["timestamp"].IsString()) {
            std::cout << "时间戳: " << doc["timestamp"].GetString() << std::endl;
        }
    } else {
        std::cout << "JSON解析失败，显示原始数据" << std::endl;
    }
    std::cout << "=====================" << std::endl;
}

// 辅助函数：打印同步响应结果
void PrintSyncResponse(const SyncResponse& response, const std::string& operation) {
    std::cout << "=== " << operation << " 结果 ===" << std::endl;
    std::cout << "返回码: " << response.code << std::endl;
    std::cout << "消息: " << response.message << std::endl;
    std::cout << "数据: " << response.data << std::endl;
    std::cout << "================================" << std::endl;
}

int main() {
    // 初始化同步管理器，注册推送回调
    int ret = InitSyncManager(OnPushData);
    if (ret != 0) {
        std::cout << "同步管理器初始化失败！" << std::endl;
        return -1;
    }
    std::cout << "同步管理器初始化成功，推送回调已注册" << std::endl;

    // 发起同步登录请求
    LoginParam param;
    param.ip = "121.52.252.12";
    param.port = 9999;
    param.account = "xxx";
    param.password = "xxx";
    
    std::cout << "开始同步登录..." << std::endl;
    SyncResponse login_response = LoginSync(&param, 10000);
    PrintSyncResponse(login_response, "登录");

    if (login_response.code != 0) {
        std::cout << "登录失败，程序退出" << std::endl;
        CleanupSyncManager();
        return -1;
    }

    // 等待一段时间确保登录完成
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 发起同步订阅请求
    std::cout << "开始同步订阅..." << std::endl;
    SyncResponse subscribe_response = SubscribeSync("hxfnews", 10000);
    PrintSyncResponse(subscribe_response, "订阅");

    if (subscribe_response.code == 0) {
        std::cout << "订阅成功！等待推送数据..." << std::endl;
        std::cout << "推送数据将通过OnPushData回调函数接收" << std::endl;
        
        // 保持程序运行一段时间，接收推送数据
        std::cout << "程序将运行30秒接收推送数据..." << std::endl;
        for (int i = 0; i < 30; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "等待推送数据... " << (30 - i) << "秒" << std::endl;
        }
    }

    // 发起同步取消订阅请求
    std::cout << "开始同步取消订阅..." << std::endl;
    SyncResponse unsubscribe_response = UnSubscribeSync("hxfnews", 10000);
    PrintSyncResponse(unsubscribe_response, "取消订阅");

    // 发起同步登出请求
    std::cout << "开始同步登出..." << std::endl;
    SyncResponse logout_response = LogoutSync(10000);
    PrintSyncResponse(logout_response, "登出");

    // 清理同步管理器
    CleanupSyncManager();
    std::cout << "程序执行完成" << std::endl;

    return 0;
} 