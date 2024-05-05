#include<time.h>
#include<Windows.h>
#include<iostream>
#include"pch.h"

#include"timerclass.h"

//#include"MFCApplication2.h"

void timer::update()
{
	switch (this->workmode)
	{
	case 0:
		break;
	case 1:

		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}
	while (this->lasttime>=0)
	{
		//CMFCApplication2App theApp;
		
		Sleep(1000);
		this->lasttime--;
	}
	SendMessage(handle, 2020, 0, 0);
	this->rang();
}

void timer::rang()
{
	printf_s( "rang!!\n");
	MessageBox(NULL, L"1233", L"133", MB_OK);
}

timer::timer()
{

}

timer::~timer()
{
}