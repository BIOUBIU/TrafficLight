#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"
#include <cstring>

const char* ssid = "delayTest";
const char* password = "12345678";
unsigned int localPort = 4210;

AsyncUDP udp;
IPAddress remote(192,168,4,2);

void onPacketCallBack(AsyncUDPPacket packet)
{
  udp.writeTo((const unsigned char*)"ElPsyCongroo", 13, remote, 4210);
}

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);  //设置softAP
  while (!udp.listen(4210)) //等待udp监听设置成功
  {
  }
  udp.onPacket(onPacketCallBack);
}

void loop() {


}
