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

// 生成标准UUID字符串（xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx）
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

// 辅助函数：检查回调参数有效性
inline bool IsValidCallbackParam(const char* result, int len, const char* tag) {
    if (result == nullptr || len <= 0) {
        printf("%s回调参数异常\n", tag);
        return false;
    }
    return true;
}

// 登录回调函数
void OnLogin(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "登录")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("登录回调JSON解析失败\n");
        return;
    }
    // 判断是否登录成功
    if (dom.HasMember("result")) {
        printf("登录成功\n");
    } else if(dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "未知错误";
        printf("登录失败：error：%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("登录回调数据格式异常\n");
    }
}

// 查询回调函数
void OnQuery(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "查询")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("查询回调JSON解析失败\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("查询成功 uuid：%s\n", strUUid.c_str());
        if (dom["result"].HasMember("data") && dom["result"]["data"].IsArray()) {
            const rapidjson::Value& array = dom["result"]["data"];
            for (rapidjson::Value::ConstValueIterator it = array.Begin(); it != array.End(); ++it) {
                const rapidjson::Value& data = *it;
                const char* type = data.HasMember("type") && data["type"].IsString() ? data["type"].GetString() : "";
                const char* d = data.HasMember("data") && data["data"].IsString() ? data["data"].GetString() : "";
                printf("type：%s  data:%s\n", type, d);
            }
        }
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "未知错误";
        printf("查询失败：error：%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("查询回调数据格式异常\n");
    }
}

// 订阅回调函数
void OnSubscribe(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "订阅")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("订阅回调JSON解析失败\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("订阅成功 uuid：%s\n", strUUid.c_str());
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("message") && dom["error"]["message"].IsString() ? dom["error"]["message"].GetString() : "未知错误";
        printf("订阅失败：error：%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("订阅回调数据格式异常\n");
    }
}

// 取消订阅回调函数
void OnUnSubscribe(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "取消订阅")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("取消订阅回调JSON解析失败\n");
        return;
    }
    if (dom.HasMember("result")) {
        std::string strUUid = dom.HasMember("id") && dom["id"].IsString() ? dom["id"].GetString() : "";
        printf("取消订阅成功 uuid：%s\n", strUUid.c_str());
    } else if (dom.HasMember("error") && dom["error"].IsObject()) {
        int nCode = dom["error"].HasMember("code") && dom["error"]["code"].IsInt() ? dom["error"]["code"].GetInt() : -1;
        std::string strMsg = dom["error"].HasMember("msg") && dom["error"]["msg"].IsString() ? dom["error"]["msg"].GetString() : "未知错误";
        printf("取消订阅失败：error：%d, msg:%s\n", nCode, strMsg.c_str());
    } else {
        printf("取消订阅回调数据格式异常\n");
    }
}

// 推送回调函数
void OnPush(const char* result, int len)
{
    if (!IsValidCallbackParam(result, len, "推送")) return;
    rapidjson::Document dom;
    if (dom.Parse(result, len).HasParseError()) {
        printf("推送回调JSON解析失败\n");
        return;
    }
    const char* type = dom.HasMember("type") && dom["type"].IsString() ? dom["type"].GetString() : "";
    const char* data = dom.HasMember("data") && dom["data"].IsString() ? dom["data"].GetString() : "";
    printf("收到推送 type：%s  data:%s\n", type, data);
}

// Session状态回调函数
void OnSession(const char* result, int len)
{
    // 解析Session返回的 JSON 字符串
    rapidjson::Document dom;
    if (result == nullptr || len <= 0) {
        printf("Session回调参数异常\n");
        return;
    }
    if (dom.Parse(result, len).HasParseError()) {
        printf("Session回调JSON解析失败\n");
        return;
    }
    // 获取事件类型
    std::string strEvent = dom.HasMember("event") && dom["event"].IsString() ? dom["event"].GetString() : "";
    std::string strCode, strMsg;
    // 获取附加信息
    if (dom.HasMember("msg") && dom["msg"].IsObject())
    {
        strCode = dom["msg"].HasMember("code") && dom["msg"]["code"].IsString() ? dom["msg"]["code"].GetString() : "";
        strMsg = dom["msg"].HasMember("msg") && dom["msg"]["msg"].IsString() ? dom["msg"]["msg"].GetString() : "";
    }
    // 根据事件类型输出不同的状态
    if (strEvent == "connecting")
        printf("session状态：连接中 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_success")
        printf("session状态：连接成功 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_fail")
        printf("session状态：连接失败 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "disconnnect")
        printf("session状态：断开连接 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "connect_success")
        printf("session状态：连接成功中 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "reconnect")
        printf("session状态：重连中 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "logining")
        printf("session状态：登录中 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "login_success")
    {
        //第一次登录成功和断线重连成功都会返回该事件，断线后原先的订阅将会失效，需要重新订阅
        printf("session状态：登录成功 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
        // 生成唯一请求ID，发起查询
        std::string strUUid = GenerateUuid();
        Query("hxfnews", strUUid.c_str());
        // 生成唯一请求ID，发起订阅
        strUUid = GenerateUuid();
        Subscribe("hxfnews", strUUid.c_str());
        // 生成唯一请求ID，发起取消订阅
        strUUid = GenerateUuid();
        UnSubscribe("hxfnews", strUUid.c_str());
    }
    else if (strEvent == "login_fail")
        printf("session状态：登录失败 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else if (strEvent == "kick_user")
        printf("session状态：后台剔除下线 code:%s msg:%s\n", strCode.c_str(), strMsg.c_str());
    else
        printf("session回调未知事件：%s\n", strEvent.c_str());
}

// 主程序入口
int main(int argc, char* argv[])
{
    // 注册各类回调
    int handle = RegisterCallback(OnLogin, OnQuery, OnSubscribe, OnUnSubscribe, OnPush, OnSession);
    if (handle != 0) {
        printf("CreateSignalApi 创建失败！\n");
        return -1;
    }
    // 发起登录请求
    LoginParam param;
    param.ip = "121.52.252.12";
    param.port = 9999;
    param.account = "xxx";
    param.password = "xxx";
    int ret = Login(&param);
    if (ret != 0) {
        printf("Login 登录接口调用失败，返回值：%d\n", ret);
        return -2;
    }
    // 消息循环，保持程序运行
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