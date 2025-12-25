#include "ThsFactorSdkSync.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <string>
#include <fstream>
#include <map>

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

// 简单的INI配置文件读取类
class ConfigReader {
private:
    std::map<std::string, std::map<std::string, std::string>> config;

    // 去除字符串首尾空白
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    // 从文件加载配置
    bool LoadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "无法打开配置文件: " << filename << std::endl;
            return false;
        }

        std::string current_section;
        std::string line;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // 跳过空行和注释
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // 检查是否是节（section）
            if (line[0] == '[' && line.back() == ']') {
                current_section = trim(line.substr(1, line.length() - 2));
                continue;
            }
            
            // 解析键值对
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                if (!current_section.empty() && !key.empty()) {
                    config[current_section][key] = value;
                }
            }
        }
        
        file.close();
        return true;
    }

    // 获取字符串值
    std::string GetString(const std::string& section, const std::string& key, const std::string& default_value = "") {
        auto sec_it = config.find(section);
        if (sec_it != config.end()) {
            auto key_it = sec_it->second.find(key);
            if (key_it != sec_it->second.end()) {
                return key_it->second;
            }
        }
        return default_value;
    }

    // 获取整数值
    int GetInt(const std::string& section, const std::string& key, int default_value = 0) {
        std::string value = GetString(section, key, "");
        if (value.empty()) return default_value;
        try {
            return std::stoi(value);
        } catch (...) {
            return default_value;
        }
    }
};

// 主程序入口
int main(int argc, char* argv[])
{
    // 读取配置文件
    ConfigReader config;
    std::string config_file = "init.ini";
    if (argc > 1) {
        config_file = argv[1];
    }
    
    if (!config.LoadFromFile(config_file)) {
        std::cout << "配置文件加载失败，请确保 " << config_file << " 文件存在！" << std::endl;
        std::cout << "按回车退出程序..." << std::endl;
        std::string pause;
        std::getline(std::cin, pause);
        return -1;
    }
    std::cout << "配置文件加载成功: " << config_file << std::endl;

    // 初始化同步管理器，注册推送回调
    int ret = InitSyncManager(OnPushData);
    if (ret != 0) {
        std::cout << "同步管理器初始化失败！" << std::endl;
        return -1;
    }
    std::cout << "同步管理器初始化成功，推送回调已注册" << std::endl;

    // 从配置文件读取登录参数
    std::string ip = config.GetString("login", "ip", "real-factor.forfunds.cn");
    int port = config.GetInt("login", "port", 7001);
    std::string account = config.GetString("login", "account", "test");
    std::string password = config.GetString("login", "password", "123456");
    int login_timeout = config.GetInt("login", "login_timeout", 30000);
    
    LoginParam param;
    param.ip = ip.c_str();
    param.port = port;
    param.account = account.c_str();
    param.password = password.c_str();
    
    std::cout << "登录参数: IP=" << ip << ", Port=" << port << ", Account=" << account << std::endl;

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
            std::string type = config.GetString("query", "type", "");
            std::string data = config.GetString("query", "data", "");
            std::string stime = config.GetString("query", "stime", "");
            std::string etime = config.GetString("query", "etime", "");
            int timeout = config.GetInt("query", "timeout", 10000);
            
            if (type.empty() || data.empty() || stime.empty() || etime.empty()) {
                std::cout << "查询参数不完整，请检查配置文件中的 [query] 节" << std::endl;
                continue;
            }
            
            std::cout << "查询参数: type=" << type << ", data=" << data 
                      << ", stime=" << stime << ", etime=" << etime << std::endl;

            start_time = std::chrono::steady_clock::now();
            SyncResponse query_response = QuerySync(type.c_str(), data.c_str(), stime.c_str(), etime.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(query_response, "查询");
            std::cout << "查询耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&query_response);
        } else if (choice == "2") {
            std::string type = config.GetString("subscribe", "type", "");
            int timeout = config.GetInt("subscribe", "timeout", 5000);
            
            if (type.empty()) {
                std::cout << "订阅类型为空，请检查配置文件中的 [subscribe] 节" << std::endl;
                continue;
            }
            
            std::cout << "订阅参数: type=" << type << std::endl;

            start_time = std::chrono::steady_clock::now();
            SyncResponse subscribe_response = SubscribeSync(type.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(subscribe_response, "订阅");
            std::cout << "订阅耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&subscribe_response);
        } else if (choice == "3") {
            std::string type = config.GetString("unsubscribe", "type", "");
            int timeout = config.GetInt("unsubscribe", "timeout", 5000);
            
            if (type.empty()) {
                std::cout << "取消订阅类型为空，请检查配置文件中的 [unsubscribe] 节" << std::endl;
                continue;
            }
            
            std::cout << "取消订阅参数: type=" << type << std::endl;

            start_time = std::chrono::steady_clock::now();
            SyncResponse unsubscribe_response = UnSubscribeSync(type.c_str(), timeout);
            end_time = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            PrintSyncResponse(unsubscribe_response, "取消订阅");
            std::cout << "取消订阅耗时: " << duration.count() << "ms" << std::endl;
            CleanupSyncResponse(&unsubscribe_response);
        } else if (choice == "4") {
            int timeout = config.GetInt("logout", "timeout", 5000);
            std::cout << "登出参数: timeout=" << timeout << "ms" << std::endl;

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