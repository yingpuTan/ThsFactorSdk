#pragma once
#include <string>
#include <memory>

// ʱ���ʽ˵����
// ����ʱ�������ʹ��YYYYMMDDHHmmss��ʽ
// ���磺20240101143000 ��ʾ 2024��1��1��14ʱ30��00��
// ��ʼʱ�䣺��ѯ����ʼʱ���
// ����ʱ�䣺��ѯ�Ľ���ʱ���

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

// ��¼�����ṹ��
#pragma pack(push, 1)
typedef struct {
    const char* ip;        	// ��������ַ
    int port;             	// �������˿�
    const char* account;  	// �˻�
    const char* password; 	// ����
} LoginParam;
#pragma pack(pop)

// ͬ����Ӧ����ṹ��
#pragma pack(push, 1)
typedef struct {
    int code;              // �����룬0��ʾ�ɹ�
    std::string message;   // ������Ϣ
    std::string data;      // ��Ӧ����
} SyncResponse;
#pragma pack(pop)

extern "C"
{
	/// <summary>
	/// ע��ص�
	/// </summary>
	/// <param name="cbLogin">��¼�ص�</param>
	/// <param name="cbQuery">��ѯ�ص�</param>
	/// <param name="cbSubscribe">���Ļص�</param>
	/// <param name="cbUnSubscribe">ȡ�����Ļص�</param>
	/// <param name="cbPush">���ͻص�</param>
	/// <param name="cbSession">session�ص�</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int RegisterCallback(OnLoginCb cbLogin, OnQueryCb cbQuery, OnSubscribeCb cbSubscribe, OnUnSubscribeCb cbUnSubscribe, OnPushCb cbPush, OnSessionCb cbSession);

	/// <summary>
	/// �����¼
	/// </summary>
	/// <param name="param">��¼�����ṹ��ָ��</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Login(const LoginParam* param);

	/// <summary>
	/// ����ǳ�
	/// </summary>
	/// <param name="handle">handle</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Logout();

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="type">��������</param>
	/// <param name="begin">��ʼʱ�䣬��ʽΪYYYYMMDDHHmmss</param>
	/// <param name="end">����ʱ�䣬��ʽΪYYYYMMDDHHmmss</param>
	/// <param name="uuid">����Ψһ��ʶ��ʽΪxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Query(const char* type, const char* begin, const char* end, const char* uuid);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="handle">handle</param>
	/// <param name="type">��������</param>
	/// <param name="uuid">����Ψһ��ʶ��ʽΪxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int Subscribe(const char* type, const char* uuid);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="handle">handle</param>
	/// <param name="type">��������</param>
	/// <param name="uuid">����Ψһ��ʶ��ʽΪxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</param>
	/// <returns>0���ɹ�</returns>
	COSMOS_THSFACTOR_SDK_EXPORT int UnSubscribe(const char* type, const char* uuid);

	// ========== ͬ���ӿڶ��� ==========
	
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
}
