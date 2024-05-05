#include<iostream>
#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")
#include"pch.h"
#include"timerclass.h"

#include<thread>
#include<string.h>
#include<vector>

#pragma warning(disable:4996)
using namespace std;

std::thread Cthread[1024];
//extern std::vector<timer> timers;
timer timers[1024];
int arrptr = 0;

void initialization();
void clientrecv(int recv_len, SOCKET s_accept, char* recv_buf,HWND handle);
int serverinital(HWND handle) {
	//���峤�ȱ���
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//���巢�ͻ������ͽ��ܻ�����
	char recv_buf[1024];
	//���������׽��֣����������׽���
	SOCKET s_server;

	//����˵�ַ�ͻ��˵�ַ
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//���������Ϣ
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5010);
	//�����׽���
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("�׽��ְ�ʧ�ܣ�\n");
		WSACleanup();
	}
	else {
		printf("�׽��ְ󶨳ɹ���\n");
	}
	//�����׽���Ϊ����״̬
	if (listen(s_server, SOMAXCONN) < 0) {
		printf("���ü���״̬ʧ�ܣ�\n");
		WSACleanup();
	}
	else {
		printf("���ü���״̬�ɹ���\n");
	}
	printf("��������ڼ������ӣ����Ժ�....\n");
	std::thread Cthread[256];
	int ptr = 0;
	while (true)
	{
		//������������
		SOCKET s_accept;
		len = sizeof(SOCKADDR);
		s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
		if (s_accept == SOCKET_ERROR) {
			printf("����ʧ�ܣ�\n");
			WSACleanup();
			return 0;
		}
		printf("%d�����ӽ�����׼����������\n", ptr);
		Cthread[ptr] = std::thread(clientrecv, recv_len, s_accept, recv_buf,handle);
		Cthread[ptr].detach();
		ptr++;

	}

	//�ر��׽���
	closesocket(s_server);

	//�ͷ�DLL��Դ
	WSACleanup();
	return 0;
}
void initialization() {
	//��ʼ���׽��ֿ�
	WORD w_req = MAKEWORD(2, 2);//�汾��
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		printf("��ʼ���׽��ֿ�ʧ�ܣ�\n");
	}
	else {
		printf("��ʼ���׽��ֿ�ɹ���\n");
	}
	//���汾��
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		printf("�׽��ֿ�汾�Ų�����\n");
		WSACleanup();
	}
	else {
		printf("�׽��ֿ�汾��ȷ��\n");
	}
	//������˵�ַ��Ϣ

}
void clientrecv(int recv_len, SOCKET s_accept, char* recv_buf,HWND handle)
{
	//��������
	//������Ϣ����
	//ddd               ,ccc     ,dddd     ,xx      ,xxxx(һ�㲻��Ҫ)
	//(int)�����豸���,�豸����  �趨ʱ��  �豸״̬ ʣ��ʱ��
	
	std::vector<string>infos;
	int ptr = 0;
	memset(recv_buf, NULL, 1024);
	recv_len = recv(s_accept, recv_buf, 100, 0);

	if (recv_len < 0) {
		printf("����ʧ�ܣ�");
	}
	else {
		
		char* p;
		char* info = strtok_s(recv_buf, ",",&p);
		while (info)
		{
			infos.push_back(info);
			
			info = strtok_s(NULL, ",", &p);
			
		}
	}
	switch (timers[atoi(infos[0].c_str())].workmode)
	{
	case -1:
		timers[atoi(infos[0].c_str())].munber = atoi(infos[0].c_str());
		timers[atoi(infos[0].c_str())].decribetion = (char*)(infos[1].c_str());
		timers[atoi(infos[0].c_str())].settingtime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].devicestatu = (char*)infos[3].c_str();
		timers[atoi(infos[0].c_str())].lasttime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		timers[atoi(infos[0].c_str())].handle = handle;
		break;
	case 0:
		timers[atoi(infos[0].c_str())].settingtime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].devicestatu = (char*)infos[3].c_str();
		timers[atoi(infos[0].c_str())].lasttime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		break;
	case 1:
		timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		break;
	case 2:
		//timers[atoi(infos[0].c_str())].settingtime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].devicestatu = (char*)infos[3].c_str();
		//timers[atoi(infos[0].c_str())].lasttime = atoi(infos[2].c_str());
		timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		break;
	case 3:
		timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		break;
	default:
		break;
	}
	if (timers[atoi(infos[0].c_str())].munber ==-1 )
	{
		
	}
	else
	{
		if (timers[atoi(infos[0].c_str())].workmode == 2)
		{
			
		}
		if (timers[atoi(infos[0].c_str())].workmode==0)
		{
			
		}
		else
		{
			timers[atoi(infos[0].c_str())].workmode = atoi(infos[3].c_str());
		}
	}
	
	SendMessage(handle, 2020,0,0);
	
	//timers.push_back(Mtimer);
	closesocket(s_accept);
	return;
}