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

// ͬ����Ӧ����ṹ��
#pragma pack(push, 1)
typedef struct {
    int code;              // �����룬0��ʾ�ɹ�
    const char* message;   // ������Ϣ
    const char* data;      // ��Ӧ����
    void* data_owner;      // ���������ߣ������ڴ����
} SyncResponse;
#pragma pack(pop)

// ========== ͬ���ӿڶ��� ==========
extern "C" {
    /// <summary>
    /// ��ʼ��ͬ���ӿڹ�����
    /// </summary>
    /// <param name="pushCallback">���ͻص���������ѡ����</param>
    /// <returns>0���ɹ�</returns>
    COSMOS_THSFACTOR_SDK_EXPORT int InitSyncManager(OnPushCb pushCallback = nullptr);

    /// <summary>
    /// ����ͬ���ӿڹ�����
    /// </summary>
    /// <returns>0���ɹ�</returns>
    COSMOS_THSFACTOR_SDK_EXPORT int CleanupSyncManager();

    /// <summary>
    /// ͬ����¼�ӿ�
    /// </summary>
    /// <param name="param">��¼�����ṹ��ָ��</param>
    /// <param name="timeout_ms">��ʱʱ�䣨���룩��Ĭ��5000ms</param>
    /// <returns>ͬ����Ӧ���</returns>
    COSMOS_THSFACTOR_SDK_EXPORT SyncResponse LoginSync(const LoginParam* param, int timeout_ms = 5000);

    /// <summary>
    /// ͬ���ǳ��ӿ�
    /// </summary>
    /// <param name="timeout_ms">��ʱʱ�䣨���룩��Ĭ��5000ms</param>
    /// <returns>ͬ����Ӧ���</returns>
    COSMOS_THSFACTOR_SDK_EXPORT SyncResponse LogoutSync(int timeout_ms = 5000);

    /// <summary>
    /// ͬ����ѯ�ӿ�
    /// </summary>
    /// <param name="type">��������</param>
    /// <param name="begin">��ʼʱ�䣬��ʽΪYYYYMMDDHHmmss</param>
    /// <param name="end">����ʱ�䣬��ʽΪYYYYMMDDHHmmss</param>
    /// <param name="timeout_ms">��ʱʱ�䣨���룩��Ĭ��10000ms</param>
    /// <returns>ͬ����Ӧ���</returns>
    COSMOS_THSFACTOR_SDK_EXPORT SyncResponse QuerySync(const char* type, const char* begin, const char* end, int timeout_ms = 10000);

    /// <summary>
    /// ͬ�����Ľӿ�
    /// </summary>
    /// <param name="type">��������</param>
    /// <param name="timeout_ms">��ʱʱ�䣨���룩��Ĭ��5000ms</param>
    /// <returns>ͬ����Ӧ���</returns>
    COSMOS_THSFACTOR_SDK_EXPORT SyncResponse SubscribeSync(const char* type, int timeout_ms = 5000);

    /// <summary>
    /// ͬ��ȡ�����Ľӿ�
    /// </summary>
    /// <param name="type">��������</param>
    /// <param name="timeout_ms">��ʱʱ�䣨���룩��Ĭ��5000ms</param>
    /// <returns>ͬ����Ӧ���</returns>
    COSMOS_THSFACTOR_SDK_EXPORT SyncResponse UnSubscribeSync(const char* type, int timeout_ms = 5000);

    /// <summary>
    /// ����ͬ����Ӧ����
    /// </summary>
    /// <param name="response">Ҫ�������Ӧ����</param>
    COSMOS_THSFACTOR_SDK_EXPORT void CleanupSyncResponse(SyncResponse* response); 
}