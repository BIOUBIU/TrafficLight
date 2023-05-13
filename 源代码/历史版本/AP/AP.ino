/********************************************************************************************************
  Anti-occlusion Traffic Lights System based on ESP8266
  基于ESP8266的防公交车遮挡交通信号灯系统
  *******************************************************************************************************
  *这是本项目的AP端程序代码。
  
  Author/作者: 杨昊峥
  Version/版本：v1.0

  使用的非Arduino官方库和非ESP8266库:
  1.LedControl库：https://github.com/wayoda/LedControl

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

#include <LedControl.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define LIGHT_DIRECTION 0   //模拟显示的红绿灯的方向，可填入0或1
#define SECONDS4TURN_LEFT 10    //左转绿灯秒数
#define SECONDS4GO_STRAIGHT 35  //直行绿灯秒数
#define SECONDS4YELLOW_LIGHT 3  //黄灯秒数

const char* ssid = "TLECROSSROAD1";  //AP的SSID
const char* password = "12345678";     //AP的密码
unsigned int localPort = 4210;         //UDP通信端口
WiFiUDP Udp;

const int DIN = 13;   //DIN-->SPI MOSI，GPIO13（D7）
const int CLK = 14;   //CLK-->SPI CLK，GPIO14（D5）
const int CS = 12;    //CS -->GPIO12（D6）
/*******************************************************************************************************
  关于引脚方面的一些补充说明：
      *某些资料称NodeMCU的HSPI CS，即GPIO15无上拉电阻，而看了下ledcontrol库中会对CS写高电平，虽然只有一个从机正
常不会有影响，但为保险起见不使用。不过文档中对CS脚的注释写到"This one is driven LOW for chip selection"，故如
确有需要，可以尝试使用GPIO15。
      *NodeMCU另有一组SPI口，由SDIO通信兼容SPI模式。其中 SPI 通常专门用于从片外Flash 读取 CPU 程序代码。而 HSPI
则用于用户 SPI 设备的通信操作。
      *另外，MAX7219点阵屏是5V供电，而NodeMCU的供电引脚都是3.3v，需要外接电源。但我使用的Lolin v3开发板将两个备
用引脚之一作为VUSB脚，可直接输出USB电源，故只要给USB5v供电，就可用来驱动点阵屏。但注意，在连接电脑的最高500mAUSB
口时有可能因电流过大损坏USB或引起电脑供电不稳定。(血的教训)
*******************************************************************************************************/

LedControl LC = LedControl(DIN,CLK,CS,4);   //创建库对象，引脚模式等已由库文件代劳

/***********************************************************************************************************************
    *以下是手搓的字库，LEDControl库假定屏幕类别为FC-16，而我使用的是IC-Station的点阵屏，故字库0-7反了过来。
其按列排序，顺序是P方向->G方向
    *另附几种常见点阵屏的接线方式，请按自己的屏幕使用字库。
     p  A  B  C  D  E  F  G        7  6  5  4  3  2  1  0        G  F  E  D  C  B  A  p        G  F  E  D  C  B  A  p
    ------------------------      ------------------------      ------------------------      ------------------------
  0 |o  o  o  o  o  o  o  o|    p |o  o  o  o  o  o  o  o|    0 |o  o  o  o  o  o  o  o|    7 |o  o  o  o  o  o  o  o|
  1 |o  o  o  o  o  o  o  o|    A |o  o  o  o  o  o  o  o|    1 |o  o  o  o  o  o  o  o|    6 |o  o  o  o  o  o  o  o|
  2 |o  o  o  o  o  o  o  o|    B |o  o  o  o  o  o  o  o|    2 |o  o  o  o  o  o  o  o|    5 |o  o  o  o  o  o  o  o|
  3 |o  o              o  o|    C |o  o              o  o|    3 |o  o              o  o|    4 |o  o              o  o|
  4 |o  o    FC-16     o  o|    D |o  o   Generic    o  o|    4 |o  o   Parola     o  o|    3 |o  o  IC-Station  o  o|
  5 |o  o              o  o|    E |o  o              o  o|    5 |o  o              o  o|    2 |o  o              o  o|
  6 |o  o  o  o  o  o  o  o|    F |o  o  o  o  o  o  o  o|    6 |o  o  o  o  o  o  o  o|    1 |o  o  o  o  o  o  o  o|
  7 |o  o  o  o  o  o  o  o|    G |o  o  o  o  o  o  o  o|    7 |o  o  o  o  o  o  o  o|    0 |o  o  o  o  o  o  o  o|
    ------------------------      ------------------------      ------------------------      ------------------------
************************************************************************************************************************/
//数字0~9字库，大小5*8，每个十六进制数为一字节，控制一列LED
const byte number[10][5] = {
  {0x3E, 0x45, 0x49, 0x51, 0x3E},   // 0;
  {0x00, 0x40, 0x7F, 0x42, 0x00},   // 1;
  {0x46, 0x49, 0x51, 0x61, 0x42},   // 2;
  {0x31, 0x4B, 0x45, 0x41, 0x21},   // 3;
  {0x10, 0x7F, 0x12, 0x14, 0x18},   // 4;
  {0x39, 0x45, 0x45, 0x45, 0x27},   // 5;
  {0x30, 0x49, 0x49, 0x4A, 0x3C},   // 6;
  {0x03, 0x05, 0x09, 0x71, 0x03},   // 7;
  {0x36, 0x49, 0x49, 0x49, 0x36},   // 8;
  {0x1E, 0x29, 0x49, 0x49, 0x06}    // 9;
};
//一些图标，大小6*8
const byte StraightSign[2][6] = {
  {0x14, 0x12, 0x7F, 0x02, 0x04, 0x00},   //绿灯
  {0x15, 0x12, 0x7F, 0x0A, 0x14, 0x20}    //红灯
};
const byte TurnLeftSign[2][6] = {
  {0x08, 0x08, 0x08, 0x2A, 0x1C, 0x08},   //绿灯
  {0x09, 0x0A, 0x0C, 0x2A, 0x1C, 0x28}    //红灯
};
const byte Empty[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   //用来实现模拟黄灯时的闪动效果

void LEDinit()       //初始化LED
{
    for(int i=0; i<4; i++)
    {
      LC.shutdown(i,false);  //关闭led省电
      LC.setIntensity(i,0);  //设置led亮度
      LC.clearDisplay(i);    //清屏
    }
}

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
    //delay(1000);
    Serial.begin(9600);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);  //设置softAP
    delay(1000);
    Udp.begin(localPort); //开启UDP服务，监听localPort
    LEDinit();
    lightDataInit();
}

//********************************************************************************************************************
//*以下为显示函数，注释中的“列”与“行”【均以库文件索引为准，不是实际屏幕的列与行】
//p.s.大佬名言：“向函数传入一个状态参数来更改其业务是十分丑陋且愚蠢的。”咱不差这点内存非传啥参数呢，故写了几个看起来很像的函数（逃

void displayLeftNum(int tensPlace, int onesPlace) 
{   //从右向左打印
    const int tensEndPos_in0 = 1;   //十位数字在第0块点阵的第1列结束(第0列开始)
    const int tensStartPos_in1 = 5; //十位数字在第1块点阵的第5列开始
    const int tensEndPos_in1 = 7;   //十位数字在第1块点阵的第7列结束
    const int onesEndPos_in1 = 4;   //个位数字在第1块点阵的第4列结束(第0列开始),个位数不跨点阵显示故只有一个常量
    for(int i=0; i<=onesEndPos_in1; i++)
    {
      LC.setColumn(1,i,number[onesPlace][i]);
    }
    for(int i=tensStartPos_in1; i<=tensEndPos_in1; i++)
    {
      LC.setColumn(1,i,number[tensPlace][i-tensStartPos_in1]);
    }
    for(int i=0; i<=tensEndPos_in0; i++)
    {
      LC.setColumn(0,i,number[tensPlace][i+(tensEndPos_in1 - tensStartPos_in1 + 1)]);
    }
}

void displayRightNum(int tensPlace, int onesPlace) 
{
    const int tensEndPos_in2 = 1;   //十位数字在第2块点阵的第1列结束(第0列开始)
    const int tensStartPos_in3 = 5; //十位数字在第3块点阵的第5列开始
    const int tensEndPos_in3 = 7;   //十位数字在第3块点阵的第7列结束
    const int onesEndPos_in3 = 4;   //个位数字在第3块点阵的第4列结束(第0列开始),个位数不跨点阵显示故只有一个常量
    for(int i=0; i<=onesEndPos_in3; i++)
    {
      LC.setColumn(3,i,number[onesPlace][i]);
    }
    for(int i=tensStartPos_in3; i<=tensEndPos_in3; i++)
    {
      LC.setColumn(3,i,number[tensPlace][i-tensStartPos_in3]);
    }
    for(int i=0; i<=tensEndPos_in2; i++)
    {
      LC.setColumn(2,i,number[tensPlace][i+(tensEndPos_in3 - tensStartPos_in3 + 1)]);
    }
}

void displayLeftGreen() 
{
    const int signStartPos = 2;
    const int signEndPos = 7;
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(0,i,TurnLeftSign[0][i-signStartPos]);
    }
}

void displayLeftRed() 
{
    const int signStartPos = 2;
    const int signEndPos = 7;
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(0,i,TurnLeftSign[1][i-signStartPos]);
    }
}

const unsigned long flashInterval = 350;

void displayLeftYellow() 
{  //使用闪动的方式模拟黄灯(单色LEDの痛)
    const int signStartPos = 2;
    const int signEndPos = 7;
    //const unsigned long flashInterval = 350;
    //unsigned long startTime = millis();
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(0,i,Empty[i-signStartPos]);
    }
    //while(millis()-startTime < flashInterval);
    delay(400);
    //startTime = millis();
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(0,i,TurnLeftSign[0][i-signStartPos]);
    }
    //while(millis()-startTime < flashInterval);
    delay(400);
}

void displayRightGreen() 
{
    const int signStartPos = 2;
    const int signEndPos = 7;
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(2,i,StraightSign[0][i-signStartPos]);
    }
}

void displayRightRed() 
{
    const int signStartPos = 2;
    const int signEndPos = 7;
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(2,i,StraightSign[1][i-signStartPos]);
    }
}

void displayRightYellow() 
{  //使用闪动的方式模拟黄灯(单色LEDの痛)
    const int signStartPos = 2;
    const int signEndPos = 7;
    //const unsigned long flashInterval = 350;
    //unsigned long startTime = millis();
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(2,i,Empty[i-signStartPos]);
    }
    //while(millis()-startTime < flashInterval)
    //{
    //}
    delay(400);
    //startTime = millis();
    for(int i=signStartPos; i<=signEndPos; i++)
    {
      LC.setColumn(2,i,StraightSign[0][i-signStartPos]);
    }
    //while(millis()-startTime < flashInterval)
    //{
    //}
    delay(400);
}

int dir = LIGHT_DIRECTION;

void display()  //调用前面几个小的显示函数
{
  switch(dir)
  {
    case 0:
    {
      displayLeftNum(dir0L.num / 10, dir0L.num % 10);
      displayRightNum(dir0R.num / 10, dir0R.num % 10);
      if(dir0L.status == 0)
      {
        displayLeftGreen();
      }
      if(dir0L.status == 2)
      {
        displayLeftRed();
      }
      if(dir0R.status == 0)
      {
        displayRightGreen();
      }
      if(dir0R.status == 2)
      {
        displayRightRed();
      }
      if(dir0L.status == 1)   //因为黄灯涉及时延，故一定要放到最后判断，以下同理
      {
        displayLeftYellow();
        //displayLeftGreen();
      }
      if(dir0R.status == 1)
      {
        displayRightYellow();
      }
      break;
    }
    case 1:
    {
      displayLeftNum(dir1L.num / 10, dir1L.num % 10);
      displayRightNum(dir1R.num / 10, dir1R.num % 10);
      if(dir1L.status == 0)
      {
        displayLeftGreen();
      }
      if(dir1L.status == 2)
      {
        displayLeftRed();
      }
      if(dir1R.status == 0)
      {
        displayRightGreen();
      }
      if(dir1R.status == 2)
      {
        displayRightRed();
      }
      if(dir1L.status == 1)
      {
        displayLeftYellow();
      }
      if(dir1R.status == 1)
      {
        displayRightYellow();
      }
      break;
    }
  }
}
//***********************************************************************************************

char sendBuffer[13];    //发送数据包，格式：12位数字 e.g.{[0]99[1]99}{[1]99[0]99}第一组大括号表示方向0，第二组表示方向1，方括号内表示灯状态

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
    Udp.write(sendBuffer);
    Udp.endPacket();
    Udp.beginPacket("192.168.4.3", localPort);
    Udp.write(sendBuffer);
    Udp.endPacket();
    Udp.beginPacket("192.168.4.4", localPort);
    Udp.write(sendBuffer);
    Udp.endPacket();
/*    Udp.beginPacket("192.168.4.5", localPort);
    Udp.write(sendBuffer);
    Udp.endPacket();*/
}

//**********************************************************************************************
//以下函数负责数据的改变
void LCycles0()
{
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
    changeVal();
    writeBuffer();
    delay(1000 - (millis() - previousTime));
}

///【增加通信时延补偿，如达到rssi阈值才发包（结合走向），发送间隔随rssi值动态调整等】