
// MFCApplication2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"

#include<thread>

//#include"timerclass.h"
#include<vector>

#pragma warning(disable:4996)
std::thread Server,Windowpaint,upd,MSB[1024];
void update(HWND handle);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DeviceLIST, DeviceList);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CMFCApplication2Dlg::OnBnClickedCancel)
	ON_MESSAGE(paintList, &CMFCApplication2Dlg::Paintlist)
	
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//----------------------------------------------------------------------------------------------------------------------------------------
	
	Server = std::thread(serverinital,GetSafeHwnd());
	Server.detach();
	upd = std::thread(update, GetSafeHwnd());
	upd.detach();
	//Windowpaint = std::thread(paintList);
	//Windowpaint.detach();
	/*Windowpaint = std::thread(paintList);
	Windowpaint.detach();*/
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication2Dlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	DestroyWindow();
	
	//std::vector<timer> timers;
	//DeviceList.Insert();
	CDialogEx::OnCancel();
}


afx_msg LRESULT CMFCApplication2Dlg::Paintlist(WPARAM wParam, LPARAM lParam)
{
	DeviceList.ResetContent();
	for (int i = 0; i < 1024; i++)
	{
		if (timers[i].munber != -1)
		{
			char recv_buf[1024];
			//sprintf(recv_buf, "%d号,%d模式,%d已设定时间,%d:%d", timers[i].munber,timers[i].workmode, timers[i].settingtime, timers[i].lasttime/60, timers[i].lasttime % 60);
			sprintf(recv_buf, "%d号,%d模式,%d:%d|", timers[i].munber, timers[i].workmode, timers[i].lasttime / 60, timers[i].lasttime % 60);
			int rate = ((double)timers[i].lasttime / (double)timers[i].settingtime)/0.05;
			printf("%s\n", recv_buf);
			USES_CONVERSION;
			CString Onshow = A2T(recv_buf);
			for (int i = 0; i < 20; i++)
			{
				if (i < rate)
				{
					Onshow.Append(L"-", Onshow.GetLength());
					//recv_buf[i + strlen(recv_buf)] = '-';
				}
				else
				{
					Onshow.Append(L" ", Onshow.GetLength());
				}

			}
			Onshow.Append(L"|", Onshow.GetLength());
			//Onshow.Format(L"%s", recv_buf);
			DeviceList.AddString(Onshow);
		}
	}
	
	return 0;
}

void update(HWND handle)
{
	while (true)
	{
		Sleep(1000);
		for (int i = 0; i < 1024;i++) 
		{
			if (timers[i].munber!=-1)
			{
				switch (timers[i].workmode)
				{
				case 0:
					SendMessage(handle, 2021, 0, timers[i].munber);
					break;
				case 1:
					if (timers[i].lasttime <= 1)
					{
						timers[i].workmode = 3;
					}
					timers[i].lasttime--;
					break;
				case 2:
					break;
				case 3:
					//SendMessage(handle, 2021, 1, timers[i].munber);
					//timers->rang();
					//HWND hwnd;
					timers[i].workmode = 0;
					MSB[i] = std::thread(&timer::rang,timers[i]);
					MSB[i].detach();
					//MessageBox(NULL,L"123",L"123", MB_OK);
					break;
				default:
					break;
				}
			}
			
		}
		
		SendMessage(handle, 2020,0,0);

	}
}
