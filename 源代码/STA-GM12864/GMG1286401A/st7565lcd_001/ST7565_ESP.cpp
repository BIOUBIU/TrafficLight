
#include "ST7565_ESP.h"



//====命令或数据按时序并行写入函数(80时序)====================
void wr_lcd(unsigned char q,unsigned char date_com)
{
    digitalWrite(CS, LOW);  //启用从设备
    if(q)
    {
      digitalWrite(RS,HIGH);   //发送数据
    }
    else
    {
      digitalWrite(RS,LOW);  //发送指令
    }
    SPI.transfer(date_com);
    digitalWrite(CS, HIGH); // 发送完毕后再次禁用从设备
}
//====LCD初始化函数==============================================
//设置为屏幕横向放置、屏幕接线连带在下侧
void Initialize()
{
    digitalWrite(RST,LOW);
    delay(5);//delayMicroseconds(20);
    digitalWrite(RST, HIGH);
    wr_lcd(0,0xae);//OFF DISPLAY
    wr_lcd(0,0xa0);//水平显示方向设置  正/反==0xa1/0xa0
    wr_lcd(0,0xa6);//显示反白 黑字/白字=0xa6/0xa7
    wr_lcd(0,0xc8);//垂直显示方向设置  正/反==0xc0/0xc8
    wr_lcd(0,0x2f); //16POWER CONTROL 选择内部电源模式必选2F
    wr_lcd(0,0x24);//17RESISTER RATIO 电压值.（对比度）粗调 2x(x=0-7)、5V==0x24
    wr_lcd(0,0x81); //18VOLUM MODE SET 对比度细调指令，必须连续输入下面的值指令（双字节指令）
    wr_lcd(0,0x15);//RESISTER RATIO 对比度细调值。必须跟随上一句指令 ，细调 0~3F  0x2a
    wr_lcd(0,0xaf);//ON DISPLAY 开LCD
    delay(10);
}
//====页地址映射函数==============================================
//页地址对应字符写入的H地址
//行显示位置分页指令函数参见命令3--页地址设置(Page Address Set)
//VTM88870B屏(ST7565)的行排列方式自上而下为32-->63-->0-->31
//此屏以8点(8行)为一页、自上而下分为8页
//对应页地址排列为4、5、6、7、0、1、2、3
//为了便于8*8点阵的定位、代码中加入了行号Page重新映射的算法
//行号数据最大为8所以仅处理数据的低4位即可
//----------------------------------------------------------------
void SetPage(unsigned char Page)
{
    if(Page<4)
        Page+=4;    //0、1、2、3行映射到4、5、6、7页
    else if(Page>3)
        Page-=4;    //4、5、6、7行映射到0、1、2、3页
    Page=Page & 0x0f; //去掉前4位
    Page=Page | 0xB0; //将行地址与起始页(0x0b)1011XXXX组合
    wr_lcd(0,Page);   //送行地址命令
}
//====列地址高/低四位数据拆分写入函数===================
//列地址对应字符写入的Y地址
//列地址为4位的数据需要分两次写入
//8位列地址数据需要拆分为两个低4位有效的8位数据
//高、低位数据由数据的bit4位区分、bit7-bit5无效
//bit4置1代表高位数据
//bit4置0代表低位数据
//此屏幕列起始地址不是0而是1
void SetColumn(unsigned char Column)
{
    unsigned char temp;
    temp=Column;
    temp=temp>>4;
    temp=temp & 0x0f;
    temp=temp | 0x10;  //高4位
    wr_lcd(0,temp);

    Column=Column & 0x0f;
    Column=Column | 0x00;
    wr_lcd(0,Column);
}
//====清除屏显数据函数(满屏写0)============================
void Clr_Scr()
{
    unsigned char seg;
    unsigned char page;
    for(page=0xb0; page<0xb8; page++) //写页地址共 8 页 0xb0-0xb7
    {
        wr_lcd(0,page);//写入页地址
        for(seg=1; seg<129; seg++) //写 128 列(此屏幕起始列为1)
        {
            SetColumn(seg);//写入列地址
            wr_lcd(1,0x00);//写入0清除所有数据
        }
    }
}
