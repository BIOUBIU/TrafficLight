/********************************************************************************************************
  Anti-occlusion Traffic Lights System based on ESP32\8266
  基于ESP32\8266的防公交车遮挡交通信号灯系统
  *******************************************************************************************************
  *这是本项目的AP端程序代码。
  *本代码适用于ESP32。
  
  Author/作者: 杨昊峥
  Version/版本：v2.0-RGB

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


#include <WiFi.h>
#include <WiFiUdp.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

#define LIGHT_DIRECTION 0  //模拟显示的红绿灯的方向，可填入0或1
#define SECONDS4TURN_LEFT 10    //左转绿灯秒数
#define SECONDS4GO_STRAIGHT 35  //直行绿灯秒数
#define SECONDS4YELLOW_LIGHT 3  //黄灯秒数

const char* ssid = "TLECROSSROAD1";  //AP的SSID
const char* password = "12345678";     //AP的密码
unsigned int localPort = 4210;         //UDP通信端口

WiFiUDP Udp;

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myYELLOW = dma_display->color565(255, 255, 0);

struct numAndStatus   //灯的数据
{
    int num;
    int status; //0为绿灯，1为黄灯，2为红灯
}dir0L, dir0R, dir1L, dir1R;

void lightDataInit()
{
  dir0L.num = SECONDS4GO_STRAIGHT + SECONDS4YELLOW_LIGHT;
  dir0L.status = 2;
  dir0R.num = SECONDS4GO_STRAIGHT;
  dir0R.status = 0;
  dir1L.num = SECONDS4GO_STRAIGHT * 2 + SECONDS4YELLOW_LIGHT * 3 + SECONDS4TURN_LEFT;
  dir1L.status = 2;
  dir1R.num = SECONDS4GO_STRAIGHT + SECONDS4YELLOW_LIGHT * 2 + SECONDS4TURN_LEFT;
  dir1R.status = 2;
}

void setup() 
{
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6047;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(100); //0-255
  dma_display->clearScreen();
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves


  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);  //设置softAP
  delay(1000);
  Udp.begin(localPort); //开启UDP服务，监听localPort
  lightDataInit();
  Serial.begin(115200);
}

void drawLSign(uint16_t color)
{
  dma_display->fillTriangle(8, 9, 14, 3, 14, 15, color);
  dma_display->fillTriangle(15, 2, 15, 6, 17, 4, color);
  dma_display->fillTriangle(15, 12, 15, 16, 17, 14, color);
  dma_display->fillRect(15, 8, 9, 3, color);
}

void drawRSign(uint16_t color)
{
  dma_display->fillCircle(48, 10, 8, color);
}

void clearLSign()
{
  dma_display->fillRect(8, 2, 16, 16, myBLACK);
}

void clearRSign()
{
  dma_display->fillRect(40, 2, 16, 16, myBLACK);
}

void draw0LNum(uint16_t color)
{
  dma_display->setCursor(11, 20);
  dma_display->setTextColor(color);
  dma_display->print(dir0L.num);
}

void draw0RNum(uint16_t color)
{
  dma_display->setCursor(43, 20);
  dma_display->setTextColor(color);
  dma_display->print(dir0R.num);
}

void draw1LNum(uint16_t color)
{
  dma_display->setCursor(11, 20);
  dma_display->setTextColor(color);
  dma_display->print(dir1L.num);
}

void draw1RNum(uint16_t color)
{
  dma_display->setCursor(43, 20);
  dma_display->setTextColor(color);
  dma_display->print(dir1R.num);
}

void clearLNum()
{
  dma_display->fillRect(11, 20, 12, 8, myBLACK);
}

void clearRNum()
{
  dma_display->fillRect(43, 20, 18, 8, myBLACK);
}

int dir = LIGHT_DIRECTION;

void display()
{
  /*switch(dir)
  {
    case 0:
    {*/
    if(dir == 0)
    {
      switch(dir0L.status)
      {
        case 2:
        {
          clearLSign(); 
          //delay(20);
          drawLSign(myRED); 
          clearLNum();
          draw0LNum(myRED);
          break;
        }
        case 0:
        {
          clearLSign(); //清除左边箭头的显示（即在指定区域填充一个(0,0,0)的矩形）
          drawLSign(myGREEN); //绘制左边的箭头，颜色为myGREEN（但是看起来是蓝色）
          clearLNum();  //清除左边的数字（也是填充黑矩形）
          draw0LNum(myGREEN); //绘制左边数字
          break;
        }
        case 1:
        {
          clearLSign();
          drawLSign(myYELLOW);
          clearLNum();
          draw0LNum(myYELLOW);
          break;
        }
      }
      switch(dir0R.status)
      {
        case 2:
        {
          clearRSign();
          drawRSign(myRED);
          clearRNum();
          draw0RNum(myRED);
          break;
        }
        case 0:
        {
          clearRSign();
          drawRSign(myGREEN);
          clearRNum();
          draw0RNum(myGREEN);
          break;
        }
        case 1:
        {
          clearRSign();
          drawRSign(myYELLOW);
          clearRNum();
          draw0RNum(myYELLOW);
          break;
        }
      }
    }
    /*case 1:
    {*/
    else
    {
      switch(dir1L.status)
      {
        case 2:
        {
          clearLSign();
          drawLSign(myRED);
          clearLNum();
          draw1LNum(myRED);
          break;
        }
        case 0:
        {
          clearLSign();
          drawLSign(myGREEN);
          clearLNum();
          draw1LNum(myGREEN);
          break;
        }
        case 1:
        {
          clearLSign();
          drawLSign(myYELLOW);
          clearLNum();
          draw1LNum(myYELLOW);
          break;
        }
      }
      switch(dir1R.status)
      {
        case 2:
        {
          clearRSign();
          drawRSign(myRED);
          clearRNum();
          draw1RNum(myRED);
          break;
        }
        case 0:
        {
          clearRSign();
          drawRSign(myGREEN);
          clearRNum();
          draw1RNum(myGREEN);
          break;
        }
        case 1:
        {
          clearRSign();
          drawRSign(myYELLOW);
          clearRNum();
          draw1RNum(myYELLOW);
          break;
        }
      }
    }
  /*}*/
}


uint8_t sendBuffer[13];    //发送数据包，格式：12位数字 e.g.{[0]99[1]99}{[1]99[0]99}第一组大括号表示方向0，第二组表示方向1，方括号内表示灯状态

void writeBuffer()    //将数据写入缓冲区
{
  sendBuffer[0] = dir0L.status + 48;
  sendBuffer[1] = dir0L.num / 10 + 48;
  sendBuffer[2] = dir0L.num % 10 + 48;
  sendBuffer[3] = dir0R.status + 48;
  sendBuffer[4] = dir0R.num / 10 + 48;
  sendBuffer[5] = dir0R.num % 10 + 48;
  sendBuffer[6] = dir1L.status + 48;
  sendBuffer[7] = dir1L.num / 10 + 48;
  sendBuffer[8] = dir1L.num % 10 + 48;
  sendBuffer[9] = dir1R.status + 48;
  sendBuffer[10] = dir1R.num / 10 + 48;
  sendBuffer[11] = dir1R.num % 10 + 48;
}

void sendPacket()   //局域网内广播UDP包     //【注意】尝试加入一个判断连接数量决定发包次数的功能///////////////////
{
    Udp.beginPacket("192.168.4.2", localPort);
    Udp.write(sendBuffer,13);
    Udp.endPacket();
    Udp.beginPacket("192.168.4.3", localPort);
    Udp.write(sendBuffer,13);
    Udp.endPacket();
    Udp.beginPacket("192.168.4.4", localPort);
    Udp.write(sendBuffer,13);
    Udp.endPacket();
/*    Udp.beginPacket("192.168.4.5", localPort);
    Udp.write(sendBuffer);
    Udp.endPacket();*/
}

//**********************************************************************************************
//以下函数负责数据的改变


void LCycles0()
{
  //previous0L = dir0L.status;
  switch(dir0L.status)//【写错啦！！！！！！】光改形参辣！！！！！！
  {
    case 0:
    {
      dir0L.num--;
      if(dir0L.num == 0)
      {
        dir0L.num = SECONDS4YELLOW_LIGHT;
        dir0L.status = 1;
      }
      break;
    }
    case 1:
    {
      dir0L.num--;
      if(dir0L.num == 0)
      {
        dir0L.num = SECONDS4GO_STRAIGHT * 2 + SECONDS4YELLOW_LIGHT * 3 + SECONDS4TURN_LEFT;
        dir0L.status = 2;
      }
      break;
    }
    case 2:
    {
      dir0L.num--;
      if(dir0L.num == 0)
      {
        dir0L.num = SECONDS4TURN_LEFT;
        dir0L.status = 0;
      }
      break;
    }
  }
}

void LCycles1 ()
{
  //previous1L = dir1L.status;
  switch(dir1L.status)
  {
    case 0:
    {
      dir1L.num--;
      if(dir1L.num == 0)
      {
        dir1L.num = SECONDS4YELLOW_LIGHT;
        dir1L.status = 1;
      }
      break;
    }
    case 1:
    {
      dir1L.num--;
      if(dir1L.num == 0)
      {
        dir1L.num = SECONDS4GO_STRAIGHT * 2 + SECONDS4YELLOW_LIGHT * 3 + SECONDS4TURN_LEFT;
        dir1L.status = 2;
      }
      break;
    }
    case 2:
    {
      dir1L.num--;
      if(dir1L.num == 0)
      {
        dir1L.num = SECONDS4TURN_LEFT;
        dir1L.status = 0;
      }
      break;
    }
  }
}

void RCycles0 ()   //右转的循环流程
{
  //previous0R = dir0R.status;
  switch(dir0R.status)
  {
    case 0:
    {
      dir0R.num--;
      if(dir0R.num == 0)
      {
        dir0R.num = SECONDS4YELLOW_LIGHT;
        dir0R.status = 1;
      }
      break;
    }
    case 1:
    {
      dir0R.num--;
      if(dir0R.num == 0)
      {
        dir0R.num = SECONDS4GO_STRAIGHT + SECONDS4YELLOW_LIGHT * 3 + SECONDS4TURN_LEFT * 2;
        dir0R.status = 2;
      }
      break;
    }
    case 2:
    {
      dir0R.num--;
      if(dir0R.num == 0)
      {
        dir0R.num = SECONDS4GO_STRAIGHT;
        dir0R.status = 0;
      }
      break;
    }
  }
}

void RCycles1 ()   //右转的循环流程
{
  //previous1R = dir1R.status;
  switch(dir1R.status)
  {
    case 0:
    {
      dir1R.num--;
      if(dir1R.num == 0)
      {
        dir1R.num = SECONDS4YELLOW_LIGHT;
        dir1R.status = 1;
      }
      break;
    }
    case 1:
    {
      dir1R.num--;
      if(dir1R.num == 0)
      {
        dir1R.num = SECONDS4GO_STRAIGHT + SECONDS4YELLOW_LIGHT * 3 + SECONDS4TURN_LEFT * 2;
        dir1R.status = 2;
      }
      break;
    }
    case 2:
    {
      dir1R.num--;
      if(dir1R.num == 0)
      {
        dir1R.num = SECONDS4GO_STRAIGHT;
        dir1R.status = 0;
      }
      break;
    }
  }
}

void changeVal()  //又是一个调用前面几个小函数的函数
{
  LCycles0();
  LCycles1();
  RCycles0();
  RCycles1();
}

unsigned long previousTime = millis();

void loop() 
{
    previousTime = millis();
    sendPacket();
    display();
    delay(20);
    changeVal();
    writeBuffer();
    delay(1000 - (millis() - previousTime));
}

///【增加通信时延补偿，如达到rssi阈值才发包（结合走向），发送间隔随rssi值动态调整等】