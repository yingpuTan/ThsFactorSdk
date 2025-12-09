#include "ThsFactorSdkSync.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <string>

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

// 基础输入工具，允许空值返回默认
std::string PromptString(const std::string& prompt, const std::string& default_value = "") {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    if (input.empty()) return default_value;
    return input;
}

int PromptInt(const std::string& prompt, int default_value) {
    while (true) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        if (input.empty()) return default_value;
        try {
            return std::stoi(input);
        } catch (...) {
            std::cout << "请输入有效的数字。" << std::endl;
        }
    }
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

    // 登录参数交互式输入
    LoginParam param;
    std::cout << "请输入登录参数（回车使用默认值）" << std::endl;
    std::string ip = PromptString("IP [默认real-factor.forfunds.cn]: ", "real-factor.forfunds.cn");
    param.ip = ip.c_str();
    param.port = PromptInt("端口 [默认7001]: ", 7001);
    std::string account = PromptString("账号 [默认test]: ", "test");
    std::string password = PromptString("密码 [默认123456]: ", "123456");
    param.account = account.c_str();
    param.password = password.c_str();
    int login_timeout = PromptInt("登录超时(ms) [默认30000]: ", 30000);

    std::cout << "开始同步登录..." << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    SyncResponse login_response = LoginSync(&param, login_timeout);
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    PrintSyncResponse(login_response, "登录");
    std::cout << "登录耗时: " << duration.count() << "ms" << std::endl;

    if (login_response.code != 0) {
        std::cout << "登录失败，按回车退出程序..." << std::endl;
        std::string pause;
        std::getline(std::cin, pause);
        CleanupSyncResponse(&login_response);
        CleanupSyncManager();
        return -1;
    }

    CleanupSyncResponse(&login_response);

    bool running = true;
    while (running) {
        std::cout << "\n请选择操作：" << std::endl;
        std::cout << "1. 查询" << std::endl;
        std::cout << "2. 订阅" << std::endl;
        std::cout << "3. 取消订阅" << std::endl;
        std::cout << "4. 登出" << std::endl;
        std::cout << "0. 退出程序" << std::endl;
        std::cout << "请输入选项: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::string type = PromptString("查询类型(type): ");
            std::string data = PromptString("查询数据(data，建议JSON): ");
            std::string stime = PromptString("开始时间(stime, 格式yyyymmddHHMMSS): ");
            std::string etime = PromptString("结束时间(etime, 格式yyyymmddHHMMSS): ");
            int timeout = PromptInt("超时(ms) [默认10000]: ", 10000);

            start_time = std::chrono::steady_clock::now();
            SyncResponse query_response = QuerySync(type.c_str(), data.c_str(), stime.c_str(), etime.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(query_response, "查询");
            std::cout << "查询耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&query_response);
        } else if (choice == "2") {
            std::string type = PromptString("订阅类型(type): ");
            int timeout = PromptInt("超时(ms) [默认5000]: ", 5000);

            start_time = std::chrono::steady_clock::now();
            SyncResponse subscribe_response = SubscribeSync(type.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(subscribe_response, "订阅");
            std::cout << "订阅耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&subscribe_response);
        } else if (choice == "3") {
            std::string type = PromptString("取消订阅类型(type): ");
            int timeout = PromptInt("超时(ms) [默认5000]: ", 5000);

            start_time = std::chrono::steady_clock::now();
            SyncResponse unsubscribe_response = UnSubscribeSync(type.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(unsubscribe_response, "取消订阅");
            std::cout << "取消订阅耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&unsubscribe_response);
        } else if (choice == "4") {
            int timeout = PromptInt("登出超时(ms) [默认5000]: ", 5000);

            start_time = std::chrono::steady_clock::now();
            SyncResponse logout_response = LogoutSync(timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(logout_response, "登出");
            std::cout << "登出耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&logout_response);
        } else if (choice == "0") {
            running = false;
        } else {
            std::cout << "无效选项，请重新选择。" << std::endl;
        }
    }

    CleanupSyncManager();
    std::cout << "程序执行完成" << std::endl;

    return 0;
}