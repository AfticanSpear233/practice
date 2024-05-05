#include<iostream>
#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")
#include"pch.h"
#include"timerclass.h"

#include<thread>
#include<string.h>
#include<vector>
//此段文件主要来源于csdn中Cche1的开源代码，在此基础上添加了多线程连接多个客户端的功能
//原代码网址:https://blog.csdn.net/qq_27923041/article/details/83857964
#pragma warning(disable:4996)
using namespace std;

std::thread Cthread[1024];
//extern std::vector<timer> timers;
timer timers[1024];
int arrptr = 0;

void initialization();
void clientrecv(int recv_len, SOCKET s_accept, char* recv_buf,HWND handle);
int serverinital(HWND handle) {
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义发送缓冲区和接受缓冲区
	char recv_buf[1024];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;

	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5010);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("套接字绑定失败！\n");
		WSACleanup();
	}
	else {
		printf("套接字绑定成功！\n");
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		printf("设置监听状态失败！\n");
		WSACleanup();
	}
	else {
		printf("设置监听状态成功！\n");
	}
	printf("服务端正在监听连接，请稍候....\n");
	std::thread Cthread[256];
	int ptr = 0;
	while (true)
	{
		//接受连接请求
		SOCKET s_accept;
		len = sizeof(SOCKADDR);
		s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
		if (s_accept == SOCKET_ERROR) {
			printf("连接失败！\n");
			WSACleanup();
			return 0;
		}
		printf("%d号连接建立，准备接受数据\n", ptr);
		Cthread[ptr] = std::thread(clientrecv, recv_len, s_accept, recv_buf,handle);
		Cthread[ptr].detach();
		ptr++;

	}

	//关闭套接字
	closesocket(s_server);

	//释放DLL资源
	WSACleanup();
	return 0;
}
void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		printf("初始化套接字库失败！\n");
	}
	else {
		printf("初始化套接字库成功！\n");
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		printf("套接字库版本号不符！\n");
		WSACleanup();
	}
	else {
		printf("套接字库版本正确！\n");
	}
	//填充服务端地址信息

}
void clientrecv(int recv_len, SOCKET s_accept, char* recv_buf,HWND handle)
{
	//接收数据
	//定义消息内容
	//ddd               ,ccc     ,dddd     ,xx      ,xxxx(一般不需要)
	//(int)自身设备编号,设备描述  设定时间  设备状态 剩余时间
	
	std::vector<string>infos;
	int ptr = 0;
	memset(recv_buf, NULL, 1024);
	recv_len = recv(s_accept, recv_buf, 100, 0);

	if (recv_len < 0) {
		printf("接受失败！");
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
