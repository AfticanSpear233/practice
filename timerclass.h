//#include"pch.h"
class timer
{
public:
	timer();
	int munber = -1;
	int lasttime = -1;
	int settingtime = -1;
	int workmode = -1;//0����ģʽ��1����ģʽ��2��ͣģʽ��3����ģʽ
	int listsite = -1;//�б�λ�ã���ֹ����ʱ��ɾ
	char* decribetion;
	char* devicestatu;
	HWND handle;
	void update();
	void rang();
	~timer();

private:

};


extern timer timers[1024];