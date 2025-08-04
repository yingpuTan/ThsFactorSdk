#include "ThsFactorSdkSync.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

// ���ͻص�����
void OnPushData(const char* push, int len) {
    if (!push || len <= 0) return;
    std::cout << "�յ��������ݣ�����: " << len << std::endl;
    std::cout << "��������: " << push << std::endl;
}

// ������������ӡͬ����Ӧ���
void PrintSyncResponse(const SyncResponse& response, const std::string& operation) {
    std::cout << "=== " << operation << "��� ===" << std::endl;
    std::cout << "������: " << response.code << std::endl;
    std::cout << "������Ϣ: " << (response.message ? response.message : "��") << std::endl;
    
    if (response.data && strlen(response.data) > 0) {
        std::cout << "��Ӧ���ݳ���: " << strlen(response.data) << std::endl;
        std::cout << "��Ӧ����: " << response.data << std::endl;
    } else {
        std::cout << "��Ӧ����: ��" << std::endl;
    }
    std::cout << "==================" << std::endl;
}

// ���������
int main(int argc, char* argv[])
{
    // ��ʼ��ͬ����������ע�����ͻص�
    int ret = InitSyncManager(OnPushData);
    if (ret != 0) {
        std::cout << "ͬ����������ʼ��ʧ�ܣ�" << std::endl;
        return -1;
    }
    std::cout << "ͬ����������ʼ���ɹ������ͻص���ע��" << std::endl;

    // ����ͬ����¼����
    LoginParam param;
    param.ip = "121.52.252.12";
    param.port = 9999;
    param.account = "test_account";
    param.password = "test_password";
    
    std::cout << "��ʼͬ����¼..." << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    SyncResponse login_response = LoginSync(&param, 5000); // 5�볬ʱ
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(login_response, "��¼");
    std::cout << "��¼��ʱ: " << duration.count() << "ms" << std::endl;

    if (login_response.code != 0) {
        std::cout << "��¼ʧ�ܣ������˳�" << std::endl;
        CleanupSyncResponse(&login_response);
        CleanupSyncManager();
        return -1;
    }
    
    // �����¼��Ӧ����
    CleanupSyncResponse(&login_response);

    // ����ͬ����ѯ����
    std::cout << "��ʼͬ����ѯ..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse query_response = QuerySync("test_type", "20240101000000", "20240131235959", 10000); // 10�볬ʱ
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(query_response, "��ѯ");
    std::cout << "��ѯ��ʱ: " << duration.count() << "ms" << std::endl;
    
    // �����ѯ��Ӧ����
    CleanupSyncResponse(&query_response);

    // ����ͬ����������
    std::cout << "��ʼͬ������..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse subscribe_response = SubscribeSync("test_type", 5000); // 5�볬ʱ
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(subscribe_response, "����");
    std::cout << "���ĺ�ʱ: " << duration.count() << "ms" << std::endl;
    
    // ��������Ӧ����
    CleanupSyncResponse(&subscribe_response);

    // �ȴ�һ��ʱ�������������
    std::cout << "�ȴ���������..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // ����ͬ��ȡ����������
    std::cout << "��ʼͬ��ȡ������..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse unsubscribe_response = UnSubscribeSync("test_type", 5000); // 5�볬ʱ
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(unsubscribe_response, "ȡ������");
    std::cout << "ȡ�����ĺ�ʱ: " << duration.count() << "ms" << std::endl;
    
    // ����ȡ��������Ӧ����
    CleanupSyncResponse(&unsubscribe_response);

    // ����ͬ���ǳ�����
    std::cout << "��ʼͬ���ǳ�..." << std::endl;
    start_time = std::chrono::steady_clock::now();
    SyncResponse logout_response = LogoutSync(5000); // 5�볬ʱ
    end_time = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    PrintSyncResponse(logout_response, "�ǳ�");
    std::cout << "�ǳ���ʱ: " << duration.count() << "ms" << std::endl;
    
    // ����ǳ���Ӧ����
    CleanupSyncResponse(&logout_response);

    // ����ͬ��������
    CleanupSyncManager();
    std::cout << "����ִ�����" << std::endl;

    return 0;
}