# 异步接口 vs 同步接口对比

## 概述

本文档对比了ThsFactorSdk的异步回调接口和新增的同步接口，帮助开发者选择合适的接口类型。

## 接口对比

### 异步接口（原有）

```cpp
// 回调函数定义
typedef void (*OnLoginCb)(const char* result, int len);
typedef void (*OnQueryCb)(const char* result, int len);
typedef void (*OnSubscribeCb)(const char* result, int len);
typedef void (*OnUnSubscribeCb)(const char* result, int len);

// 异步接口
int RegisterCallback(OnLoginCb cbLogin, OnQueryCb cbQuery, ...);
int Login(const LoginParam* param);
int Query(const char* type, const char* uuid);
int Subscribe(const char* type, const char* uuid);
int UnSubscribe(const char* type, const char* uuid);
```

### 同步接口（新增）

```cpp
// 响应结构体
typedef struct {
    int code;              // 返回码，0表示成功
    const char* message;   // 错误信息
    const char* data;      // 响应数据
    void* data_owner;      // 数据所有者，用于内存管理
} SyncResponse;
```

// 同步接口
int InitSyncManager(OnPushCb pushCallback = nullptr);
int CleanupSyncManager();
SyncResponse LoginSync(const LoginParam* param, int timeout_ms = 5000);
SyncResponse QuerySync(const char* type, const char* begin, const char* end, int timeout_ms = 10000);
SyncResponse SubscribeSync(const char* type, int timeout_ms = 5000);
SyncResponse UnSubscribeSync(const char* type, int timeout_ms = 5000);
void CleanupSyncResponse(SyncResponse* response);
```
```

## 使用方式对比

### 异步接口使用示例

```cpp
// 1. 定义回调函数
void OnLogin(const char* result, int len) {
    // 处理登录响应
    printf("登录响应: %.*s\n", len, result);
}

void OnQuery(const char* result, int len) {
    // 处理查询响应
    printf("查询响应: %.*s\n", len, result);
}

// 2. 注册回调
RegisterCallback(OnLogin, OnQuery, OnSubscribe, OnUnSubscribe, OnPush, OnSession);

// 3. 发起请求
LoginParam param = {...};
Login(&param);  // 非阻塞，立即返回

Query("hxfnews", "uuid-123");  // 非阻塞，立即返回

// 4. 响应在回调函数中处理
```

### 同步接口使用示例

```cpp
// 1. 初始化同步管理器（可选注册推送回调）
InitSyncManager(OnPushData);

// 2. 发起同步请求
LoginParam param = {...};
SyncResponse login_response = LoginSync(&param, 10000);  // 阻塞等待响应

if (login_response.code == 0) {
    printf("登录成功: %s\n", login_response.data);
    
    // 继续其他操作
    SyncResponse query_response = QuerySync("hxfnews", "20240101000000", "20240131235959", 15000);
    if (query_response.code == 0) {
        printf("查询成功: %s\n", query_response.data);
    }
    
    // 清理查询响应数据
    CleanupSyncResponse(&query_response);
}

// 3. 清理登录响应数据
CleanupSyncResponse(&login_response);

// 4. 清理同步管理器
CleanupSyncManager();
```

## 特性对比

| 特性 | 异步接口 | 同步接口 |
|------|----------|----------|
| **调用方式** | 非阻塞 | 阻塞 |
| **响应处理** | 回调函数 | 直接返回 |
| **超时控制** | 无 | 可配置 |
| **错误处理** | 在回调中处理 | 通过返回码处理 |
| **使用复杂度** | 较高 | 较低 |
| **性能** | 更好 | 稍差 |
| **线程安全** | 需要额外处理 | 内置支持 |
| **调试难度** | 较难 | 较易 |
| **代码可读性** | 较差 | 较好 |
| **内存管理** | 自动 | 手动清理 |
| **多线程支持** | 需要额外处理 | 内置支持 |
| **推送回调** | 支持 | 支持 |

## 适用场景

### 异步接口适用场景

1. **高性能要求**：需要处理大量并发请求
2. **实时性要求**：需要立即响应其他事件
3. **复杂业务逻辑**：需要处理多种异步事件
4. **已有异步架构**：系统已经采用异步模式
5. **推送数据处理**：需要处理实时推送数据

### 同步接口适用场景

1. **简单业务逻辑**：只需要简单的请求-响应模式
2. **快速原型开发**：需要快速验证功能
3. **调试和测试**：需要清晰的执行流程
4. **教学和演示**：需要易于理解的代码
5. **脚本化应用**：需要顺序执行的业务逻辑
6. **多线程应用**：需要线程安全的并发调用
7. **内存敏感应用**：需要精确控制内存使用

## 性能对比

### 异步接口优势

- **高并发**：可以同时处理多个请求
- **低延迟**：不会阻塞主线程
- **资源效率**：更好的CPU和内存利用率
- **实时响应**：可以立即响应其他事件

### 同步接口优势

- **简单直观**：代码逻辑清晰
- **易于调试**：可以单步调试
- **错误处理**：错误处理更直接
- **学习成本**：更容易理解和使用
- **线程安全**：内置多线程支持
- **内存控制**：精确的内存管理
- **推送支持**：支持推送回调功能

## 迁移建议

### 从异步迁移到同步

1. **评估需求**：确认是否真的需要同步接口
2. **逐步迁移**：可以先迁移部分功能
3. **保持兼容**：保留异步接口作为备选
4. **测试验证**：充分测试迁移后的功能

### 从同步迁移到异步

1. **重构回调**：将同步逻辑拆分为回调函数
2. **状态管理**：添加状态管理机制
3. **错误处理**：在回调中处理错误
4. **并发控制**：添加必要的并发控制

## 最佳实践

### 异步接口最佳实践

1. **合理设计回调**：避免回调地狱
2. **错误处理**：在每个回调中处理错误
3. **状态管理**：使用状态机管理复杂流程
4. **资源清理**：及时清理资源避免内存泄漏

### 同步接口最佳实践

1. **合理设置超时**：避免过长的等待时间
2. **错误检查**：始终检查返回码
3. **资源管理**：确保正确初始化和清理
4. **异常处理**：添加适当的异常处理
5. **内存管理**：必须调用 `CleanupSyncResponse()` 清理响应数据
6. **多线程使用**：每个线程独立初始化和清理
7. **推送回调**：合理使用推送回调功能

## 总结

- **异步接口**适合高性能、高并发的场景
- **同步接口**适合简单、直观的业务逻辑，特别是多线程应用
- 两种接口可以并存，根据具体需求选择
- 建议在开发初期使用同步接口，性能要求高时再考虑异步接口
- 同步接口提供了更好的内存控制和线程安全支持
- 两种接口都支持推送回调功能，满足实时数据需求

## 多线程使用对比

### 异步接口多线程使用

```cpp
// 异步接口需要额外的线程安全处理
std::mutex callback_mutex;
std::map<std::string, std::function<void(const char*, int)>> callbacks;

void OnLogin(const char* result, int len) {
    std::lock_guard<std::mutex> lock(callback_mutex);
    // 处理登录响应，需要额外的线程安全处理
    printf("登录响应: %.*s\n", len, result);
}

// 需要手动管理线程安全
RegisterCallback(OnLogin, OnQuery, OnSubscribe, OnUnSubscribe, OnPush, OnSession);
```

### 同步接口多线程使用

```cpp
// 同步接口内置线程安全支持
void TestThread(int thread_id) {
    // 每个线程独立初始化
    InitSyncManager(nullptr);
    
    LoginParam param = {...};
    SyncResponse login_response = LoginSync(&param, 5000);
    
    if (login_response.code == 0) {
        printf("线程 %d 登录成功\n", thread_id);
    }
    
    // 清理响应数据
    CleanupSyncResponse(&login_response);
    
    // 清理同步管理器
    CleanupSyncManager();
}

// 多线程并发调用
std::vector<std::thread> threads;
for (int i = 0; i < 5; ++i) {
    threads.emplace_back(TestThread, i);
}
for (auto& thread : threads) {
    thread.join();
}
```

### 多线程特性对比

| 特性 | 异步接口 | 同步接口 |
|------|----------|----------|
| **线程安全** | 需要手动处理 | 内置支持 |
| **数据隔离** | 需要额外设计 | 自动隔离 |
| **内存管理** | 自动 | 手动但精确 |
| **使用复杂度** | 较高 | 较低 |
| **调试难度** | 较难 | 较易 |