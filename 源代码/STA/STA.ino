/********************************************************************************************************
  Anti-occlusion Traffic Lights System based on ESP8266
  基于ESP8266的防公交车遮挡交通信号灯系统
  *******************************************************************************************************
  *这是本项目的STA端程序代码。
  
  Author/作者: 杨昊峥
  Version/版本：v0.1

  使用的非Arduino官方库和非ESP8266库:
  1.u8g2库
  2.ESPAsyncUDP

  Copyright (C) 2023 杨昊峥

  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
  General Public License as published by the Free Software Foundation, either version 3 of the License, 
  or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
  License for more details.

  You should have received a copy of the GNU General Public License along with this program. If not, 
  see <https://www.gnu.org/licenses/>.
********************************************************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "ESPAsyncUDP.h"
#include <cstring>

#define SCL 5
#define SDA 4

#define MAX_CROSSROADS_NUM 3

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/SCL, /*data=*/SDA, /*reset=*/U8X8_PIN_NONE);   

ESP8266WiFiMulti WiFiMulti;

AsyncUDP udp;
unsigned long localUdpPort = 4210;

void APList()
{
  //以下存入线路中所有路口的AP的SSID和密码
  WiFiMulti.addAP("TLECROSSROAD1", "12345678"); 
  WiFiMulti.addAP("TLECROSSROAD2", "87654321"); 
  WiFiMulti.addAP("TLECROSSROAD3", "13572468"); 
}

int dirList[MAX_CROSSROADS_NUM] = {0, 1, 1}; //预先存入所有路口的方向信息

int dir = 0;  //车辆当前所处方向

char RXPacket[13];
void onPacketCallBack(AsyncUDPPacket packet)  //收包后的回调函数；
{
  memcpy(RXPacket, packet.data(), sizeof(RXPacket));
  for(int i = 0; i <= 12; i++)/////////////////////////////////////////////////
  {
    Serial.print(RXPacket[i]);
  }
  Serial.println("");
}

void judgeDir()
{
  if(!strcmp(WiFi.SSID().c_str(),"TLECROSSROAD1"))
  {
    dir = dirList[0];
  }
  if(!strcmp(WiFi.SSID().c_str(),"TLECROSSROAD2"))
  {
    dir = dirList[1];
  }
  if(!strcmp(WiFi.SSID().c_str(),"TLECROSSROAD3"))
  {
    dir = dirList[2];
  }
  //以此类推
}

void connectWiFi()
{
  while(WiFiMulti.run() != WL_CONNECTED)
  {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 15, "Searching for");
    u8g2.drawStr(0, 30, " AP...");
    u8g2.sendBuffer();
  }
  u8g2.clear();
  u8g2.drawStr(0, 15, "Connected to:");
  u8g2.drawStr(0, 30, WiFi.SSID().c_str());
  u8g2.sendBuffer();
  delay(1000);
  u8g2.clear();
  u8g2.drawStr(0, 15, "Setting up UDP");
  u8g2.drawStr(0, 30, "listening...");
  u8g2.sendBuffer();
  delay(500);
  while(!udp.listen(localUdpPort))
  {
  }
  udp.onPacket(onPacketCallBack);
  judgeDir();
  u8g2.clear();
  u8g2.setCursor(0, 15);
  u8g2.print("direction:");
  u8g2.setCursor(0, 30);
  u8g2.print(dir);
  u8g2.sendBuffer();
  delay(1000);
  u8g2.setFont(u8g2_font_inr24_mn);
}

void enterReconn()
{
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.clear();
  u8g2.drawStr(0, 15, "AP disconnected.");
  u8g2.drawStr(0, 30, "Entering");
  u8g2.drawStr(0, 45, "connect mode...");
  u8g2.sendBuffer();
  delay(1000);
  connectWiFi();
}

void setup()
{
  Serial.begin(9600);/////////////////////////////////////////////////////////////////////////
  u8g2.begin();
  u8g2.enableUTF8Print(); // enable UTF8 support for the Arduino print() function
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  APList();
  connectWiFi();
}

void LeftGreen2Buffer()
{
  u8g2.drawTriangle(30, 16, 42, 4, 42, 28);
  u8g2.drawBox(42, 10, 18, 12);
}

void LeftRed2Buffer()
{
  u8g2.drawTriangle(30, 16, 42, 4, 42, 28);
  u8g2.drawBox(42, 10, 18, 12);
  u8g2.drawLine(30, 4, 60, 28);
  u8g2.drawLine(30, 5, 59, 28);
  u8g2.drawLine(31, 4, 60, 27);  
}

void LeftYellow2Buffer()
{
  Serial.println("Start draw Lyellow");  ///////////////////////////////////
  u8g2.drawLine(30, 16, 42, 4);
  u8g2.drawLine(42, 4, 42, 10);
  u8g2.drawLine(42, 10, 60, 10);
  u8g2.drawLine(60, 10, 60, 22);
  u8g2.drawLine(60, 22, 42, 22);
  u8g2.drawLine(42, 22, 42, 28);
  u8g2.drawLine(42, 28, 30, 16);
  Serial.println("End draw Lyellow");  ///////////////////////////////////
}

void RightGreen2Buffer()
{
  u8g2.drawTriangle(45, 36, 35, 46, 55, 46);
  u8g2.drawBox(40, 46, 10, 15);
}

void RightRed2Buffer()
{
  u8g2.drawTriangle(45, 36, 35, 46, 55, 46);
  u8g2.drawBox(40, 46, 10, 15);
  u8g2.drawLine(30, 36, 60, 60);
  u8g2.drawLine(30, 37, 59, 60);
  u8g2.drawLine(31, 36, 60, 59);
}

void RightYellow2Buffer()
{
  Serial.println("Start draw Ryellow"); /////////////////////////////////////
  u8g2.drawLine(45, 36, 55, 46);
  u8g2.drawLine(55, 46, 50, 46);
  u8g2.drawLine(50, 46, 50, 61);
  u8g2.drawLine(50, 61, 40, 61);
  u8g2.drawLine(40, 61, 40, 46);
  u8g2.drawLine(40, 46, 35, 46);
  u8g2.drawLine(35, 46, 45, 36);
  Serial.println("End draw Ryellow"); ////////////////////////////////////
}

void LeftNum2Buffer0()
{
  u8g2.setCursor(68, 28);
  u8g2.print(RXPacket[1]);
  u8g2.setCursor(88, 28);
  u8g2.print(RXPacket[2]);  
}

void LeftNum2Buffer1()
{
  u8g2.setCursor(68, 28);
  u8g2.print(RXPacket[7]);
  u8g2.setCursor(88, 28);
  u8g2.print(RXPacket[8]);
}

void RightNum2Buffer0()
{
  u8g2.setCursor(68, 60);
  u8g2.print(RXPacket[4]);
  u8g2.setCursor(88, 60);
  u8g2.print(RXPacket[5]);  
}

void RightNum2Buffer1()
{
  u8g2.setCursor(68, 60);
  u8g2.print(RXPacket[10]);
  u8g2.setCursor(88, 60);
  u8g2.print(RXPacket[11]);  
}

void printPacket()
{
  u8g2.clearBuffer();
  switch(dir)
  {
    case 0:
    {
      LeftNum2Buffer0();
      RightNum2Buffer0();
      if(RXPacket[0] == '0')
      {
        LeftGreen2Buffer();
      }
      if(RXPacket[0] == '1')
      {
        LeftYellow2Buffer();
      }
      if(RXPacket[0] == '2')
      {
        LeftRed2Buffer();
      }
      if(RXPacket[3] == '0')
      {
        RightGreen2Buffer();        
      }
      if(RXPacket[3] == '1')
      {
        RightYellow2Buffer();
      }
      if(RXPacket[3] == '2')
      {
        RightRed2Buffer();
      }
      break;
    }
    case 1:
    {
      LeftNum2Buffer1();
      RightNum2Buffer1();
      if(RXPacket[6] == '0')
      {
        LeftGreen2Buffer();
      }
      if(RXPacket[6] == '1')
      {
        LeftYellow2Buffer();
      }
      if(RXPacket[6] == '2')
      {
        LeftRed2Buffer();
      }
      if(RXPacket[9] == '0')
      {
        RightGreen2Buffer();        
      }
      if(RXPacket[9] == '1')
      {
        RightYellow2Buffer();
      }
      if(RXPacket[9] == '2')
      {
        RightRed2Buffer();
      }
      break;
    }
  }
  u8g2.sendBuffer();
}

void loop()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    enterReconn();
  }
  printPacket();
}
