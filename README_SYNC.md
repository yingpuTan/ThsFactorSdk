 # ThsFactorSdk 同步接口使用说明

## 概述

本同步接口是对原有异步回调接口的封装，提供了同步调用的方式，简化了使用流程。同步接口会阻塞调用线程直到收到响应或超时。所有接口定义都统一在 `ThsFactorSdk.h` 头文件中。

## 主要特性

- **同步调用**：调用线程会等待直到收到响应或超时
- **超时控制**：每个接口都支持自定义超时时间
- **时间范围查询**：查询接口支持指定时间范围，格式为YYYYMMDDHHmmss
- **推送回调**：支持注册推送回调函数，接收订阅后的实时数据
- **向后兼容**：保留原有的异步接口，不影响现有代码
- **线程安全**：支持多线程并发调用
- **自动清理**：自动清理过期的等待请求

## 接口说明

### 初始化接口

```cpp
// 初始化同步管理器（可选注册推送回调）
int InitSyncManager(OnPushCb pushCallback = nullptr);

// 清理同步管理器
int CleanupSyncManager();
```

### 同步接口

```cpp
// 同步登录
SyncResponse LoginSync(const LoginParam* param, int timeout_ms = 5000);

// 同步登出
SyncResponse LogoutSync(int timeout_ms = 5000);

// 同步查询（支持时间范围）
SyncResponse QuerySync(const char* type, const char* begin, const char* end, int timeout_ms = 10000);
// begin: 开始时间，格式YYYYMMDDHHmmss，如：20240101143000
// end: 结束时间，格式YYYYMMDDHHmmss，如：20240131235959

// 同步订阅
SyncResponse SubscribeSync(const char* type, int timeout_ms = 5000);

// 同步取消订阅
SyncResponse UnSubscribeSync(const char* type, int timeout_ms = 5000);
```

### 响应结构体

```cpp
typedef struct {
    int code;              // 返回码，0表示成功
    const char* message;   // 错误信息
    const char* data;      // 响应数据（JSON格式）
} SyncResponse;
```

## 使用示例

### 基本使用流程

```cpp
#include "ThsFactorSdk.h"
#include <iostream>

// 推送回调函数
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    
    std::string push_data(push, len);
    std::cout << "收到推送: " << push_data << std::endl;
}

int main() {
    // 1. 初始化同步管理器，注册推送回调
    if (InitSyncManager(OnPushData) != 0) {
        std::cout << "同步管理器初始化失败" << std::endl;
        return -1;
    }

    // 2. 准备登录参数
    LoginParam param;
    param.ip = "121.52.252.12";
    param.port = 9999;
    param.account = "your_account";
    param.password = "your_password";

    // 3. 同步登录
    SyncResponse login_response = LoginSync(&param, 10000);
    if (login_response.code != 0) {
        std::cout << "登录失败: " << login_response.message << std::endl;
        CleanupSyncManager();
        return -1;
    }

    // 4. 同步查询
    SyncResponse query_response = QuerySync("hxfnews", "20240101000000", "20240131235959", 15000);
    if (query_response.code == 0) {
        std::cout << "查询成功: " << query_response.data << std::endl;
    }

    // 5. 同步订阅
    SyncResponse subscribe_response = SubscribeSync("hxfnews", 10000);
    if (subscribe_response.code == 0) {
        std::cout << "订阅成功" << std::endl;
    }

    // 6. 同步取消订阅
    SyncResponse unsubscribe_response = UnSubscribeSync("hxfnews", 10000);
    if (unsubscribe_response.code == 0) {
        std::cout << "取消订阅成功" << std::endl;
    }

    // 7. 同步登出
    SyncResponse logout_response = LogoutSync(10000);
    if (logout_response.code == 0) {
        std::cout << "登出成功" << std::endl;
    }

    // 8. 清理同步管理器
    CleanupSyncManager();

    return 0;
}
```

### 推送回调使用

```cpp
// 定义推送回调函数
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    
    std::string push_data(push, len);
    std::cout << "收到推送数据: " << push_data << std::endl;
    
    // 可以在这里解析JSON数据
    // 例如使用rapidjson解析推送内容
}

// 在初始化时注册推送回调
InitSyncManager(OnPushData);

// 订阅后，推送数据会通过回调函数自动接收
SyncResponse subscribe_response = SubscribeSync("hxfnews", 10000);
if (subscribe_response.code == 0) {
    std::cout << "订阅成功，等待推送数据..." << std::endl;
    // 推送数据会通过OnPushData回调函数接收
}
```

### 错误处理

```cpp
SyncResponse response = QuerySync("hxfnews", "20240101000000", "20240131235959", 10000);
switch (response.code) {
    case 0:
        std::cout << "操作成功" << std::endl;
        break;
    case -1:
        std::cout << "请求超时" << std::endl;
        break;
    default:
        std::cout << "操作失败: " << response.message << std::endl;
        break;
}
```

## 编译说明

### 使用CMake编译（推荐）

#### 方法一：直接使用LHSession.dll（推荐）
```bash
# Windows
build_sync_dll.bat

# Linux
mkdir build_sync_dll
cd build_sync_dll
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64 -DCMAKE_PROJECT_NAME=ThsFactorSdkSync
cmake --build . --config Release
```

编译完成后，可执行文件位于 `build_sync_dll/bin/Release/` 目录下。

**注意**：
- 使用此方法需要确保 `LHSession.dll` 在 `dll/` 目录中
- 同步接口代码直接编译到可执行文件中，无需额外的DLL

#### 方法二：编译完整库
```bash
mkdir build
cd build
cmake -f ../CMakeLists_sync.txt ..
make
```

编译完成后，可执行文件位于 `build/bin/` 目录下。

### 手动编译

```bash
# 编译示例程序（直接链接LHSession.dll）
g++ -Iinclude demo/main_sync.cpp src/ThsFactorSdkSync.cpp -o demo_sync
```

## 注意事项

1. **初始化顺序**：必须先调用 `InitSyncManager()` 才能使用同步接口
2. **推送回调**：如需接收推送数据，在初始化时注册推送回调函数
3. **超时设置**：根据网络环境和业务需求合理设置超时时间
4. **时间格式**：查询接口的时间参数必须使用YYYYMMDDHHmmss格式
5. **时间范围**：确保开始时间早于结束时间，且时间格式正确
6. **线程安全**：同步接口支持多线程并发调用
7. **资源清理**：程序结束时调用 `CleanupSyncManager()` 清理资源
8. **错误处理**：始终检查返回码，处理可能的错误情况
9. **推送处理**：推送数据通过注册的回调函数异步接收

## 与异步接口的区别

| 特性 | 异步接口 | 同步接口 |
|------|----------|----------|
| 调用方式 | 非阻塞 | 阻塞 |
| 响应处理 | 回调函数 | 直接返回 |
| 超时控制 | 无 | 可配置 |
| 使用复杂度 | 较高 | 较低 |
| 性能 | 更好 | 稍差 |

## 性能建议

1. **合理设置超时时间**：避免过长的等待时间
2. **批量操作**：对于多个请求，考虑使用异步接口
3. **错误重试**：对于网络错误，实现重试机制
4. **连接复用**：避免频繁的登录登出操作

## 故障排除

### 常见问题

1. **初始化失败**
   - 检查LHSession.dll是否正确加载
   - 确认DLL文件路径正确
   - 确保dll目录中存在LHSession.dll

2. **请求超时**
   - 检查网络连接
   - 增加超时时间
   - 确认服务器状态

3. **返回码异常**
   - 检查登录状态
   - 确认请求参数正确
   - 检查时间格式是否正确（YYYYMMDDHHmmss）
   - 确认开始时间早于结束时间
   - 查看服务器日志

4. **DLL加载失败**
   - 确保LHSession.dll在可执行文件同目录或系统PATH中
   - 检查DLL依赖项是否完整
   - 使用Dependency Walker等工具检查DLL依赖

### 调试技巧

1. 启用详细日志输出
2. 使用网络抓包工具分析请求
3. 检查服务器响应格式
4. 验证UUID生成和匹配逻辑