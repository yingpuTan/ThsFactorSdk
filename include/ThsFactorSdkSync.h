#pragma once

#include "ThsFactorSdk.h"

#if defined(_WIN32) || defined(_WIN64)
#ifdef COSMOS_THSFACTOR_SDK_DLL_EXPORT
#define COSMOS_THSFACTOR_SDK_EXPORT __declspec(dllexport)
#else
#define COSMOS_THSFACTOR_SDK_EXPORT __declspec(dllimport)
#endif
#else
#define COSMOS_THSFACTOR_SDK_EXPORT __attribute__((visibility("default")))
#endif

// 同步响应结果结构体
#pragma pack(push, 1)
typedef struct {
    int code;              // 返回码，0表示成功
    const char* message;   // 错误信息
    const char* data;      // 响应数据
    void* data_owner;      // 数据所有者，用于内存管理
} SyncResponse;
#pragma pack(pop)

// ========== 同步接口定义 ==========
extern "C" {
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

    /// <summary>
    /// 清理同步响应数据
    /// </summary>
    /// <param name="response">要清理的响应对象</param>
    COSMOS_THSFACTOR_SDK_EXPORT void CleanupSyncResponse(SyncResponse* response); 
}