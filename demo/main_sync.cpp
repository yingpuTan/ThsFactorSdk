#include "ThsFactorSdkSync.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

// 推送回调函数
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    std::cout << "收到推送数据，长度: " << len << std::endl;
    std::cout << "推送内容: " << push << std::endl;
}

// 辅助函数：打印同步响应结果
void PrintSyncResponse(const SyncResponse& response, const std::string& operation) {
    std::cout << "=== " << operation << "结果 ===" << std::endl;
    std::cout << "返回码: " << response.code << std::endl;
    std::cout << "错误信息: " << (response.message ? response.message : "无") << std::endl;
    
    if (response.data && strlen(response.data) > 0) {
        std::cout << "响应数据长度: " << strlen(response.data) << std::endl;
        std::cout << "响应数据: " << response.data << std::endl;
    } else {
        std::cout << "响应数据: 无" << std::endl;
    }
    std::cout << "==================" << std::endl;
}

// 主程序入口
int main(int argc, char* argv[])
{
    // 初始化同步管理器，注册推送回调
    int ret = InitSyncManager(OnPushData);
    if (ret != 0) {
        std::cout << "同步管理器初始化失败！" << std::endl;
        return -1;
    }
    std::cout << "同步管理器初始化成功，推送回调已注册" << std::endl;

    // 发起同步登录请求
    LoginParam param;
    param.ip = "10.0.53.100";
    param.port = 9999;
    param.account = "tanyingpu1";
    param.password = "qwe13579";
    
    std::cout << "开始同步登录..." << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    SyncResponse login_response = LoginSync(&param, 30000); // 5秒超时
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(login_response, "登录");
    std::cout << "登录耗时: " << duration.count() << "ms" << std::endl;

    if (login_response.code != 0) {
        std::cout << "登录失败，程序退出" << std::endl;
        CleanupSyncResponse(&login_response);
        CleanupSyncManager();
        return -1;
    }
    
    // 清理登录响应数据
    CleanupSyncResponse(&login_response);

    // 发起同步查询请求
    std::cout << "开始同步查询..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse query_response = QuerySync("test_type", "20240101000000", "20240131235959", 10000); // 10秒超时
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(query_response, "查询");
    std::cout << "查询耗时: " << duration.count() << "ms" << std::endl;
    
    // 清理查询响应数据
    CleanupSyncResponse(&query_response);

    // 发起同步订阅请求
    std::cout << "开始同步订阅..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse subscribe_response = SubscribeSync("test_type", 5000); // 5秒超时
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(subscribe_response, "订阅");
    std::cout << "订阅耗时: " << duration.count() << "ms" << std::endl;
    
    // 清理订阅响应数据
    CleanupSyncResponse(&subscribe_response);

    // 等待一段时间接收推送数据
    std::cout << "等待推送数据..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 发起同步取消订阅请求
    std::cout << "开始同步取消订阅..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse unsubscribe_response = UnSubscribeSync("test_type", 5000); // 5秒超时
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(unsubscribe_response, "取消订阅");
    std::cout << "取消订阅耗时: " << duration.count() << "ms" << std::endl;
    
    // 清理取消订阅响应数据
    CleanupSyncResponse(&unsubscribe_response);

    // 发起同步登出请求
    std::cout << "开始同步登出..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse logout_response = LogoutSync(5000); // 5秒超时
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(logout_response, "登出");
    std::cout << "登出耗时: " << duration.count() << "ms" << std::endl;
    
    // 清理登出响应数据
    CleanupSyncResponse(&logout_response);

    // 清理同步管理器
    CleanupSyncManager();
    std::cout << "程序执行完成" << std::endl;

    return 0;
}