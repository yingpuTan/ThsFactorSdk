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


}
