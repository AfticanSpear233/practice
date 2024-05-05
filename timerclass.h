//#include"pch.h"
class timer
{
public:
	timer();
	int munber = -1;
	int lasttime = -1;
	int settingtime = -1;
	int workmode = -1;//0空闲模式，1工作模式，2暂停模式，3响铃模式
	int listsite = -1;//列表位置，防止操作时误删
	char* decribetion;
	char* devicestatu;
	HWND handle;
	void update();
	void rang();
	~timer();

private:

};


extern timer timers[1024];