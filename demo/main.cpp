#include "ThsFactorSdk.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <random>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif

// ���ɱ�׼UUID�ַ�����xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx��
std::string GenerateUuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
    uint32_t data[4];
    for (int i = 0; i < 4; ++i) data[i] = dis(gen);
    std::stringstream ss;
    ss << std::hex << std::setfill('0')
       << std::setw(8) << (data[0]) << "-"
       << std::setw(4) << ((data[1] >> 16) & 0xFFFF) << "-"
       << std::setw(4) << (data[1] & 0xFFFF) << "-"
       << std::setw(4) << ((data[2] >> 16) & 0xFFFF) << "-"
       << std::setw(4) << (data[2] & 0xFFFF)
       << std::setw(8) << data[3];
    return ss.str();
}

// �������������ص�������Ч��
inline bool IsValidCallbackParam(const char* result, int len, const char* tag) {
    if (result == nullptr || len <= 0) {
        printf("%s�ص������쳣\n", tag);
        return false;
    }
    return true;
}

// ��¼�ص�����
void OnLogin(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "��¼")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("��¼�ص�JSON����ʧ��\n");
        return;
    }
    // �ж��Ƿ��¼�ɹ�
    if (dom.HasMember("result")) {
        printf("��¼�ɹ�\n");
    } else if(dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "δ֪����";
        printf("��¼ʧ�ܣ�error��%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("��¼�ص����ݸ�ʽ�쳣\n");
    }
}

// ��ѯ�ص�����
void OnQuery(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "��ѯ")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("��ѯ�ص�JSON����ʧ��\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("��ѯ�ɹ� uuid��%s\n", strUUid.c_str());
        if (dom["result"].HasMember("data") && dom["result"]["data"].IsArray()) {
            const rapidjson::Value& array = dom["result"]["data"];
            for (rapidjson::Value::ConstValueIterator it = array.Begin(); it != array.End(); ++it) {
                const rapidjson::Value& data = *it;
                const char* type = data.HasMember("type") && data["type"].IsString() ? data["type"].GetString() : "";
                const char* d = data.HasMember("data") && data["data"].IsString() ? data["data"].GetString() : "";
                printf("type��%s  data:%s\n", type, d);
            }
        }
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "δ֪����";
        printf("��ѯʧ�ܣ�error��%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("��ѯ�ص����ݸ�ʽ�쳣\n");
    }
}

// ���Ļص�����
void OnSubscribe(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "����")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("���Ļص�JSON����ʧ��\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("���ĳɹ� uuid��%s\n", strUUid.c_str());
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "δ֪����";
        printf("����ʧ�ܣ�error��%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("���Ļص����ݸ�ʽ�쳣\n");
    }
}

// ȡ�����Ļص�����
void OnUnSubscribe(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "ȡ������")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("ȡ�����Ļص�JSON����ʧ��\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("ȡ�����ĳɹ� uuid��%s\n", strUUid.c_str());
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("msg") && dom["error"]["msg"].IsString() ? dom["error"]["msg"].GetString() : "δ֪����";
        printf("ȡ������ʧ�ܣ�error��%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("ȡ�����Ļص����ݸ�ʽ�쳣\n");
    }
}

// ���ͻص�����
void OnPush(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "����")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("���ͻص�JSON����ʧ��\n");
        return;
    }
    const char* type = dom.HasMember("type") && dom["type"].IsString() ? dom["type"].GetString() : "";
    const char* data = dom.HasMember("data") && dom["data"].IsString() ? dom["data"].GetString() : "";
    printf("�յ����� type��%s  data:%s\n", type, data);
}

// Session״̬�ص�����
void OnSession(const char* result, int len)
{
    // ����Session���ص� JSON �ַ���
    rapidjson::Document dom;
    if (result == nullptr || len <= 0) {
        printf("Session�ص������쳣\n");
        return;
    }
    if (dom.Parse(result, len).HasParseError()) {
        printf("Session�ص�JSON����ʧ��\n");
        return;
    }
    // ��ȡ�¼�����
    std::string strEvent = dom.HasMember("event") && dom["event"].IsString() ? dom["event"].GetString() : "";
    std::string strCode, strMsg;
    // ��ȡ������Ϣ
    if (dom.HasMember("msg") && dom["msg"].IsObject())
    {
        strCode = dom["msg"].HasMember("code") && dom["msg"]["code"].IsString() ? dom["msg"]["code"].GetString() : "";
        strMsg = dom["msg"].HasMember("msg") && dom["msg"]["msg"].IsString() ? dom["msg"]["msg"].GetString() : "";
    }
    // �����¼����������ͬ��״̬
    if (strEvent == "connecting")
        printf("session״̬�������� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_success")
        printf("session״̬�����ӳɹ� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_fail")
        printf("session״̬������ʧ�� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "disconnnect")
        printf("session״̬���Ͽ����� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_success")
        printf("session״̬�����ӳɹ��� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "reconnect")
        printf("session״̬�������� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "logining")
        printf("session״̬����¼�� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "login_success")
    {
        //��һ�ε�¼�ɹ��Ͷ��������ɹ����᷵�ظ��¼������ߺ�ԭ�ȵĶ��Ľ���ʧЧ����Ҫ���¶���
        printf("session״̬����¼�ɹ� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
        // ����Ψһ����ID�������ѯ
        std::string strUUid = GenerateUuid();
        Query("hxfnews", strUUid.c_str());
        // ����Ψһ����ID��������
        strUUid = GenerateUuid();
        Subscribe("hxfnews", strUUid.c_str());
        // ����Ψһ����ID������ȡ������
        strUUid = GenerateUuid();
        UnSubscribe("hxfnews", strUUid.c_str());
    }
    else if (strEvent == "login_fail")
        printf("session״̬����¼ʧ�� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "kick_user")
        printf("session״̬����̨�޳����� code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else
        printf("session�ص�δ֪�¼���%s\n", strEvent.c_str());
}

// ���������
int main(int argc, char* argv[])
{
    // ע�����ص�
    int handle = RegisterCallback(OnLogin, OnQuery, OnSubscribe, OnUnSubscribe, OnPush, OnSession);
    if (handle != 0) {
        printf("CreateSignalApi ����ʧ�ܣ�\n");
        return -1;
    }
    // �����¼����
    LoginParam param;
    param.ip = "121.52.252.12";
    param.port = 9999;
    param.account = "xxx";
    param.password = "xxx";
    int ret = Login(&param);
    if (ret != 0) {
        printf("Login ��¼�ӿڵ���ʧ�ܣ�����ֵ��%d\n", ret);
        return -2;
    }
    // ��Ϣѭ�������ֳ�������
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_QUIT)
        {
            break;
        }
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return 0;
}