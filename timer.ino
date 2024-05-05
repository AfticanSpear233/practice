#include <Arduino.h>
#include <TM1637Display.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <osapi.h>
#include <stdio.h>
static WiFiClient client;
Ticker discounter,flash;
#define Num "1"
#define discribtion "timer in ***"
// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define UP  13
#define Dow 12
#define Lef 14
#define Rht 4
#define mid 5
#define Rang 15
// The amount of time (in milliseconds) between tests
#define UNDELAY   500
//设置时间界面的动画优化

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display(CLK, DIO);
void discount();
void disFlash();
//0开始模式，1设置模式，2计时模式，3暂停模式
// enum WorkMod{
//   initialmod,
//   seting,
//   counting,
//   stop
// }WK;
class Mtimer
{
  public:
    int WorkMod=0;
    int rate=1;//设置时间的倍数 x1x10x60x600(s)
    int ratelist[4]={1,10,60,600};
    int counttime=0;
    //0开始模式，1设置模式，2计时模式，3暂停模式,4响铃模式
    void timer();
    void update();
    void excuse(bool *opt);
    void rang();
    

    
}timers;

void Mtimer::timer()
{
  pinMode(UP,INPUT);
  pinMode(Dow,INPUT);
  pinMode(Lef,INPUT);
  pinMode(Rht,INPUT);
  pinMode(mid,INPUT);
}

void Mtimer::update()
{
  bool optreation[5];
  optreation[0]=digitalRead(UP);
  optreation[1]=digitalRead(Dow);
  optreation[2]=digitalRead(Lef);
  optreation[3]=digitalRead(Rht);
  optreation[4]=digitalRead(mid);

  
  
  this->excuse(optreation);
}

void Mtimer::excuse(bool *opt)
{
  switch(this->WorkMod)
  {
    case 0:
      if (opt[4]==0)
      {
        this->WorkMod=1;
      }
    break;
    case 1:
      
      //加倍率
      if (opt[2]==0)
      {
        if (this->rate>1)
        {
          this->rate--;
        }
      }
      //减倍率
      if (opt[3]==0)
      {
        if(this->rate<4)
        {
          this->rate++;
        }
      }
      //减时间
      if (opt[0]==0)
      {
        if(this->counttime<6000)
        {
          this->counttime+=this->ratelist[rate-1];
          
        }
      }
      if(this->counttime>6000)
          {
            this->counttime=6000;
          }
      //加时间
      if (opt[1]==0)
      {
        if(this->counttime>0)
        {
          this->counttime-=this->ratelist[rate-1];
          
        }
      }
      if(this->counttime <0)
          {
            this->counttime=0;
          }
      //切换模式
      if(opt[4]==0)
      {
        flash.detach();
        if(client.connected())
        {
          
          char info[2048];
          memset(info,NULL,2048);
          sprintf(info,"%s,%s,%d,1,%d",Num,discribtion,this->counttime,this->counttime);
          client.print(info);
          Serial.println(info);
        }
        this->WorkMod=2;
        this->rate=1;
      }

    break;
    case 2:
      if(opt[4]==0)
      {
        this->WorkMod=3;
        char info[2048];
        memset(info,NULL,2048);
        sprintf(info,"%s,%s,%d,2,%d",Num,discribtion,this->counttime,this->counttime);
        client.print(info);
        discounter.detach();
      }
      if(this->counttime<=0)
      {
        this->WorkMod=4;
        
        this->counttime=0;
      }
    break;
    case 3:
      if(opt[4]==0)
      {
        this->WorkMod=2;
        char info[2048];
        memset(info,NULL,2048);
        sprintf(info,"%s,%s,%d,1,%d",Num,discribtion,this->counttime,this->counttime);
        client.print(info);
        discounter.attach(1,discount);
      }
      if(opt[0]==0)
      {
        this->WorkMod=0;
        this->counttime=0;
        char info[2048];
        memset(info,NULL,2048);
        sprintf(info,"%s,%s,%d,0,%d",Num,discribtion,this->counttime,this->counttime);
        client.print(info);
      }
    break;
    case 4:
    char info[2048];
    memset(info,NULL,2048);
    sprintf(info,"%s,%s,%d,3,%d",Num,discribtion,this->counttime,this->counttime);
    client.print(info);
    this->rang();
    if(opt[4]==0)
    {
      this->WorkMod=0;
    }
    break;
    
  }
}

void Mtimer::rang()
{
  display.clear();
  digitalWrite(Rang,HIGH);
  
  //display.setBrightness(1, false); // Turn on
  //delay(200);
  //display.setBrightness(1, true); // Turn on
}

void discount()
{
  timers.counttime--;
  if(timers.counttime<=0)
  {
    timers.WorkMod=4;
    timers.counttime=0;
    discounter.detach();
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin("","");//请输入ssid和password
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  if (!client.connect("192.168.179.1", 5010))//连接的IP地址和port
  {
    Serial.println("Connection to host failed, timer will work in offline");
  }
  
  //timers.timer();
  pinMode(UP,INPUT_PULLUP);
  pinMode(Dow,INPUT_PULLUP);
  pinMode(Lef,INPUT_PULLUP);
  pinMode(Rht,INPUT_PULLUP);
  pinMode(mid,INPUT_PULLUP);
  pinMode(Rang,OUTPUT);
  display.setBrightness(1, true); // Turn on
}



void disFlash()
{
  display.clear();
}

void loop()
{
  //display.encodeDigit(sec);
  timers.update();
  int min=timers.counttime/60;
  int sec=timers.counttime%60;
  digitalWrite(Rang,LOW);
  if(timers.WorkMod==1 && !flash.active())
  {
    flash.attach_ms(800,disFlash);
  }
  display.showNumberDec(min*100+sec,true);
  //display.setBrightness(1, true); // Turn on
  if(timers.WorkMod==2 && !discounter.active())
  {
    discounter.attach(1,discount);
  }
  delay(150);
}
