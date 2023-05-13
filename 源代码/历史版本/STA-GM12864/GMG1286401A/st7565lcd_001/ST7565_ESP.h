/***************************
 *RST     =D4=GPIO2
 *SPI_SCL =D5=GPIO14----SPI.h库内定义
 *RS=CD   =D6=GPIO12
 *SPI_SDA =D7=GPIO13----SPI.h库内定义
 *CS      =D8=GPIO15
 ***************************/

#ifndef __ST7565_ESP_h__
#define __ST7565_ESP_h__

#include "SPI.h"

//与sbit cs=p1^0类似的端口定义,端口号可以是GPIO_n也可以是其板上的引脚号Dn
static const uint8_t CS  = D8;  // GPIO15-LCD选定
static const uint8_t RST = D4;  // GPIO2-LCD复位
static const uint8_t RS  = D6;  // GPIO12-指令/数据切换

void wr_lcd(unsigned char q,unsigned char date_com);
void Initialize();
void SetPage(unsigned char Page);
void SetColumn(unsigned char Column);
void Clr_Scr();

#endif
