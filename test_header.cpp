#include "ThsFactorSdk.h"
#include <iostream>

int main() {
    // 测试异步接口类型定义
    OnLoginCb loginCallback = nullptr;
    OnQueryCb queryCallback = nullptr;
    
    // 测试同步接口类型定义
    SyncResponse response;
    response.code = 0;
    response.message = "测试成功";
    response.data = "测试数据";
    
    // 测试登录参数结构体
    LoginParam param;
    param.ip = "127.0.0.1";
    param.port = 9999;
    param.account = "test";
    param.password = "test";
    
    // 测试查询接口参数
    const char* type = "hxfnews";
    const char* begin = "20240101000000";
    const char* end = "20240131235959";
    
    // 测试推送回调类型
    OnPushCb pushCallback = nullptr;
    
    std::cout << "头文件合并测试成功！" << std::endl;
    std::cout << "异步接口类型定义: OK" << std::endl;
    std::cout << "同步接口类型定义: OK" << std::endl;
    std::cout << "结构体定义: OK" << std::endl;
    std::cout << "查询接口参数: OK" << std::endl;
    std::cout << "推送回调类型: OK" << std::endl;
    
    return 0;
} 