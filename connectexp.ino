#include <ESP8266WiFi.h>
static WiFiClient espClient;
#include <AliyunIoTSDK.h>
#define PRODUCT_KEY ""  //阿里云产品PRODUCT_KEY
#define DEVICE_NAME ""  //阿里云产品名
#define DEVICE_SECRET "" //阿里云产品DEVICE_SECRET
#define REGION_ID ""  //阿里云节点
#define SSID ""  //开发板链接的wifi名
#define PASSWORD //wifi密码

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(SSID,PASSWORD);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);
  AliyunIoTSDK::bindData("switcher", powerCallback);
  //"switcher"是产品功能的变量名在阿里云平台设置

}

void loop() {
  // put your main code here, to run repeatedly:
  AliyunIoTSDK::loop();
  
  delay(1000);
}

void powerCallback(JsonVariant p)
{
    int PowerSwitch = p["switcher"];
    
    if (PowerSwitch == 1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    } 
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
}
