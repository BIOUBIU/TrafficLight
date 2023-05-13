

#include "ST7565_ESP.h"
//#include "TEXT.h"
#include "my_time.h"

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600); //初始化串口波特率
  digitalWrite(CS, HIGH); // 禁用从设备HIGH为禁用
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV8);//设置SPI的时钟为八分之一
  pinMode(CS,OUTPUT);   //设置CS引脚为输出模式
  pinMode(RST,OUTPUT);  //设置RST引脚为输出模式
  pinMode(RS,OUTPUT);   //设置RS引脚为输出模式
  Initialize();
  Clr_Scr();
  my_time_init();
}

void loop() {

  digitalClockDisplay(); 

  delay(1000);
}
