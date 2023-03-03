#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "ESPAsyncUDP.h"
#include <cstring>

#define SCL 5
#define SDA 4

const char* ssid = "delayTest";
const char* password = "12345678";
//unsigned int localPort = 4210;

unsigned long pingTime;
unsigned long delayTime;

AsyncUDP udp;
Ticker ticker;
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/SCL, /*data=*/SDA, /*reset=*/U8X8_PIN_NONE);   

IPAddress remote(192,168,4,1);

void onPacketCallBack(AsyncUDPPacket packet)
{
  delayTime = millis() - pingTime;
  //Serial.println("RX!");
 u8g2.clearBuffer();
  u8g2.setCursor(0,15);
  u8g2.print(delayTime);
  u8g2.setCursor(0,30);
  u8g2.print(WiFi.RSSI());
  u8g2.sendBuffer();
}

void ping()
{
  Serial.println("ping!");
  pingTime = millis();
  udp.writeTo((const unsigned char*)"ElPsyCongroo", 13, remote, 4210);
}

void setup()
{
//  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print(); // enable UTF8 support for the Arduino print() function
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);
  while (!udp.listen(4210)) //等待udp监听设置成功
  {
  }
  udp.onPacket(onPacketCallBack);
  ticker.attach(6, ping);
}

void loop() 
{
/*  u8g2.firstPage();
  do
  {
    u8g2.setCursor(0,15);
    u8g2.print(delayTime);
    u8g2.setCursor(0,30);
    u8g2.print(WiFi.RSSI());
  } while (u8g2.nextPage());
*/
}
