# ThsFactorSdk 使用说明

## 目录结构及作用说明

- `include/`：头文件目录，包含 SDK 的接口声明（如 ThsFactorSdk.h）以及 rapidjson 解析库。
- `dll/`：动态链接库目录，包含 SDK 运行所需的所有 DLL 文件。
- `lib/`：静态库或导入库目录，包含部分依赖的 .lib 文件（如 LHSession.lib）。
- `demo/`：示例代码目录，包含 main.cpp 演示如何集成和调用 SDK。
- `CMakeLists.txt`：CMake 构建脚本，用于项目编译配置。

---

## 简介

ThsFactorSdk 是一个用于连接服务器、登录、查询、订阅、推送等功能的 C++ 动态库。通过注册回调和调用接口函数，可以实现与服务器的数据交互。

---

## 目录

- [环境准备](#环境准备)
- [头文件与库文件](#头文件与库文件)
- [主要数据结构](#主要数据结构)
- [接口说明](#接口说明)
- [回调说明](#回调说明)
- [使用示例](#使用示例)
- [常见问题与建议](#常见问题与建议)

---

## 环境准备

- 支持平台：Windows（需确保相关 DLL 在可执行文件同目录下）
- 依赖库：rapidjson（已包含在 include 目录下）

---

## 头文件与库文件

- 头文件：`include/ThsFactorSdk.h`
- 库文件：`dll/` 目录下的相关 DLL 文件（如 `LHSession.dll` 等）

---

## 主要数据结构

### 登录参数结构体

```cpp
typedef struct {
    const char* ip;         // 服务器地址
    int         port;       // 服务器端口
    const char* account;    // 账户
    const char* password;   // 密码
} LoginParam;
```

---

## 接口说明

### 1. 注册回调

```cpp
int RegisterCallback(
    OnLoginCb cbLogin,
    OnQueryCb cbQuery,
    OnSubscribeCb cbSubscribe,
    OnUnSubscribeCb cbUnSubscribe,
    OnPushCb cbPush,
    OnSessionCb cbSession
);
```
- 注册各类回调函数，返回 0 表示成功。

### 2. 登录

```cpp
int Login(const LoginParam* param);
```
- 发起登录请求，返回 0 表示成功。

### 3. 登出

```cpp
int Logout();
```
- 发起登出请求，返回 0 表示成功。

### 4. 查询

```cpp
int Query(const char* type, const char* uuid);
```
- 发送查询请求，type 为请求类型，uuid 为唯一标识。

### 5. 订阅

```cpp
int Subscribe(const char* type, const char* uuid);
```
- 发送订阅请求。

### 6. 取消订阅

```cpp
int UnSubscribe(const char* type, const char* uuid);
```
- 发送取消订阅请求。

---

## 回调说明

所有回调函数类型如下：

```cpp
typedef void (*OnLoginCb)(const char* result, int len);
typedef void (*OnQueryCb)(const char* result, int len);
typedef void (*OnSubscribeCb)(const char* result, int len);
typedef void (*OnUnSubscribeCb)(const char* result, int len);
typedef void (*OnPushCb)(const char* push, int len);
typedef void (*OnSessionCb)(const char* session, int len);
```

回调参数为 JSON 字符串，需用 rapidjson 解析。

---

## 使用示例

### 1. 注册回调并登录

```cpp
// 定义回调函数
void OnLogin(const char* result, int len) { /* ... */ }
void OnQuery(const char* result, int len) { /* ... */ }
void OnSubscribe(const char* result, int len) { /* ... */ }
void OnUnSubscribe(const char* result, int len) { /* ... */ }
void OnPush(const char* result, int len) { /* ... */ }
void OnSession(const char* result, int len) { /* ... */ }

int main() {
    // 注册回调
    int handle = RegisterCallback(OnLogin, OnQuery, OnSubscribe, OnUnSubscribe, OnPush, OnSession);
    if (handle != 0) {
        printf("CreateSignalApi 创建失败！\n");
        return -1;
    }

    // 登录参数
    LoginParam param;
    param.ip = "10.0.53.100";
    param.port = 9999;
    param.account = "your_account";
    param.password = "your_password";

    // 发起登录
    int ret = Login(&param);
    if (ret != 0) {
        printf("Login 登录接口调用失败，返回值：%d\n", ret);
        return -2;
    }

    // 消息循环（Windows）
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) break;
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return 0;
}
```

---

## 常见问题与建议

- **回调参数检查**：务必判断 result 是否为 nullptr，len 是否大于 0。
- **JSON 解析**：建议使用 rapidjson 解析回调数据，并做好字段存在性和类型判断。
- **多线程安全**：如在多线程环境下使用，请确保线程安全。
- **DLL 依赖**：运行时需保证所有 DLL 文件在可执行文件同目录下。

---

如需更详细的接口说明或遇到具体问题，请参考 demo/main.cpp 或联系开发者支持。 