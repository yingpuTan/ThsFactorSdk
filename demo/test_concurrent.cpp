#include "ThsFactorSdk.h"
#include "ThsFactorSdkSync.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

std::atomic<int> completed_requests{0};
std::atomic<int> total_requests{0};

// 推送回调函数
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    std::cout << "收到推送数据，长度: " << len << std::endl;
}

// 测试函数
void TestConcurrentRequests(int thread_id) {
    std::cout << "线程 " << thread_id << " 开始测试" << std::endl;
    
    // 模拟多个并发请求
    for (int i = 0; i < 3; ++i) {
        total_requests++;
        
        // 创建登录参数
        LoginParam param;
        param.ip = "121.52.252.12";
        param.port = 9999;
        param.account = "test_account";
        param.password = "test_password";
        
        // 发起登录请求
        SyncResponse login_response = LoginSync(&param, 5000);
        
        std::cout << "线程 " << thread_id << " 请求 " << i << " 登录结果: " 
                  << login_response.code << " - " << login_response.message << std::endl;
        
        // 检查响应数据是否正确
        if (login_response.data && strlen(login_response.data) > 0) {
            std::cout << "线程 " << thread_id << " 请求 " << i << " 数据长度: " 
                      << strlen(login_response.data) << std::endl;
        }
        
        completed_requests++;
        
        // 短暂延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::cout << "开始并发测试..." << std::endl;
    
    // 初始化同步管理器
    if (InitSyncManager(OnPushData) != 0) {
        std::cout << "同步管理器初始化失败！" << std::endl;
        return -1;
    }
    
    std::cout << "同步管理器初始化成功" << std::endl;
    
    // 创建多个线程进行并发测试
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(TestConcurrentRequests, i);
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "并发测试完成！" << std::endl;
    std::cout << "总请求数: " << total_requests.load() << std::endl;
    std::cout << "完成请求数: " << completed_requests.load() << std::endl;
    
    // 清理
    CleanupSyncManager();
    
    return 0;
} 