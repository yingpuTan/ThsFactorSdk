#pragma once
#include <string>
#include <memory>

// 时间格式说明：
// 所有时间参数都使用YYYYMMDDHHmmss格式
// 例如：20240101143000 表示 2024年1月1日14时30分00秒
// 开始时间：查询的起始时间点
// 结束时间：查询的结束时间点

#if defined(_WIN32) || defined(_WIN64)
#ifdef COSMOS_THSFACTOR_SDK_DLL_EXPORT
#define COSMOS_THSFACTOR_SDK_EXPORT __declspec(dllexport)
#else
#define COSMOS_THSFACTOR_SDK_EXPORT __declspec(dllimport)
#endif
#else
#define COSMOS_THSFACTOR_SDK_EXPORT __attribute__((visibility("default")))
#endif

typedef void (*OnLoginCb)(const char* result, int len);
typedef void (*OnQueryCb)(const char* result, int len);
typedef void (*OnSubscribeCb)(const char* result, int len);
typedef void (*OnUnSubscribeCb)(const char* result, int len);
typedef void (*OnPushCb)(const char* push, int len);
typedef void (*OnSessionCb)(const char* session, int len);

// 登录参数结构体
#pragma pack(push, 1)
typedef struct {
    const char* ip;        	// 服务器地址
    int port;             	// 服务器端口
    const char* account;  	// 账户
    const char* password; 	// 密码
} LoginParam;
#pragma pack(pop)

// 同步响应结果结构体
#pragma pack(push, 1)
typedef struct {
    int code;              // 返回码，0表示成功
    std::string message;   // 错误信息
    std::string data;      // 响应数据
} SyncResponse;
#pragma pack(pop)

extern "C"
{
	/// <summary>
	/// 注册回调
	/// </summary>
	/// <param name="cbLogin">登录回调</param>
	/// <param name="cbQuery">查询回调</param>
	/// <param name="cbSubscribe">订阅回调</param>
	/// <param name="cbUnSubscribe">取消订阅回调</param>
	/// <param name="cbPush">推送回调</param>
	/// <param name="cbSession">session回调</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int RegisterCallback(OnLoginCb cbLogin, OnQueryCb cbQuery, OnSubscribeCb cbSubscribe, OnUnSubscribeCb cbUnSubscribe, OnPushCb cbPush, OnSessionCb cbSession);

	/// <summary>
	/// 发起登录
	/// </summary>
	/// <param name="param">登录参数结构体指针</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Login(const LoginParam* param);

	/// <summary>
	/// 发起登出
	/// </summary>
	/// <param name="handle">handle</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Logout();

	/// <summary>
	/// 发送请求
	/// </summary>
	/// <param name="type">请求类型</param>
	/// <param name="begin">开始时间，格式为YYYYMMDDHHmmss</param>
	/// <param name="end">结束时间，格式为YYYYMMDDHHmmss</param>
	/// <param name="uuid">请求唯一标识格式为xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Query(const char* type, const char* begin, const char* end, const char* uuid);

	/// <summary>
	/// 发送请求
	/// </summary>
	/// <param name="handle">handle</param>
	/// <param name="type">请求类型</param>
	/// <param name="uuid">请求唯一标识格式为xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Subscribe(const char* type, const char* uuid);

	/// <summary>
	/// 发送请求
	/// </summary>
	/// <param name="handle">handle</param>
	/// <param name="type">请求类型</param>
	/// <param name="uuid">请求唯一标识格式为xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int UnSubscribe(const char* type, const char* uuid);

	// ========== 同步接口定义 ==========
	
	/// <summary>
	/// 初始化同步接口管理器
	/// </summary>
	/// <param name="pushCallback">推送回调函数，可选参数</param>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int InitSyncManager(OnPushCb pushCallback = nullptr);

	/// <summary>
	/// 清理同步接口管理器
	/// </summary>
	/// <returns>0：成功</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int CleanupSyncManager();

	/// <summary>
	/// 同步登录接口
	/// </summary>
	/// <param name="param">登录参数结构体指针</param>
	/// <param name="timeout_ms">超时时间（毫秒），默认5000ms</param>
	/// <returns>同步响应结果</returns>
	COSMOS_THSFACTOR_SDK_EXPORT SyncResponse LoginSync(const LoginParam* param, int timeout_ms = 5000);

	/// <summary>
	/// 同步登出接口
	/// </summary>
	/// <param name="timeout_ms">超时时间（毫秒），默认5000ms</param>
	/// <returns>同步响应结果</returns>
	COSMOS_THSFACTOR_SDK_EXPORT SyncResponse LogoutSync(int timeout_ms = 5000);

	/// <summary>
	/// 同步查询接口
	/// </summary>
	/// <param name="type">请求类型</param>
	/// <param name="begin">开始时间，格式为YYYYMMDDHHmmss</param>
	/// <param name="end">结束时间，格式为YYYYMMDDHHmmss</param>
	/// <param name="timeout_ms">超时时间（毫秒），默认10000ms</param>
	/// <returns>同步响应结果</returns>
	COSMOS_THSFACTOR_SDK_EXPORT SyncResponse QuerySync(const char* type, const char* begin, const char* end, int timeout_ms = 10000);

	/// <summary>
	/// 同步订阅接口
	/// </summary>
	/// <param name="type">请求类型</param>
	/// <param name="timeout_ms">超时时间（毫秒），默认5000ms</param>
	/// <returns>同步响应结果</returns>
	COSMOS_THSFACTOR_SDK_EXPORT SyncResponse SubscribeSync(const char* type, int timeout_ms = 5000);

	/// <summary>
	/// 同步取消订阅接口
	/// </summary>
	/// <param name="type">请求类型</param>
	/// <param name="timeout_ms">超时时间（毫秒），默认5000ms</param>
	/// <returns>同步响应结果</returns>
	COSMOS_THSFACTOR_SDK_EXPORT SyncResponse UnSubscribeSync(const char* type, int timeout_ms = 5000);
}
