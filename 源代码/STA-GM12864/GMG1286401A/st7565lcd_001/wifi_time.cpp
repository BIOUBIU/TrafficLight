/**
 * 代码原创：wannenggong
 * 代码仅用于演示
 * 未经同意请勿转载
 * 使用代码请注明出处
**/
#include "wifi_time.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>

#define URL "http://quan.suning.com/getSysTime.do"//苏宁的时间api
int8_t time_dat[7]={0,59,11,5,15,4,2022};//秒、分、时、周、日、月、年

unsigned char error_count = 0; //失败次数
String acquire;

//----星期与日期换算函数----------------------------
/*
 *应用基姆拉尔森计算公式换算 
 *wk=(d + 2*m + 3*(m + 1)/5 + y + y/4 - y/100 + y/400 + 1) % 7; 
 *wk的取值范围是0~6，0代表星期日，1~6星期一到星期六
 *公式的输入格式为：20xx年xx月xx日
 */
unsigned char set_week(unsigned int ye,unsigned char mo,unsigned char da)
{
    unsigned char wek=0;
    //ye=ye+2000;  //2000-2099
    if(mo==1||mo==2)  //月份为1、2月时、month=13、14并且年值-1
    {
        mo+=12;
        ye--;
    }
    wek=(da+2*mo+3*(mo+1)/5+ye+ye/4-ye/100+ye/400+1)%7;
    return wek;
}
//-----------------------------------------------
//网络时间获取与DS3231时间寄存器更新函数
void get_time() 
{
  
  int httpcode;//状态码
    HTTPClient httpclient;
    httpclient.setTimeout(1000);//超时时间
    while (1) 
    {
      httpclient.begin(URL);//开始连接
      httpcode = httpclient.GET();//获取状态码
      if (httpcode == 200)  //如果成功
      {
      delay(100);
      //获取网络时间
      /**报文格式：{"sysTime2":"2021-08-17 20:17:48","sysTime1":"20210817201748"}**/
      acquire = httpclient.getString();

      //拆分提取年、月、日、时、分、秒的char数据
      //分别提取十位、个位的数值并组合两位数
      //组合后的数据转换成ascⅡ数值
      time_dat[6] =(acquire.charAt(15)-0x30)*10+(acquire.charAt(16)-0x30);//年
      time_dat[5] =(acquire.charAt(18)-0x30)*10+(acquire.charAt(19)-0x30);//月
      time_dat[4] =(acquire.charAt(21)-0x30)*10+(acquire.charAt(22)-0x30);//日
      time_dat[3]=0x03;//星期
      time_dat[2] =(acquire.charAt(24)-0x30)*10+(acquire.charAt(25)-0x30);//时
      time_dat[1] =(acquire.charAt(27)-0x30)*10+(acquire.charAt(28)-0x30);//分
      time_dat[0] =(acquire.charAt(30)-0x30)*10+(acquire.charAt(31)-0x30);//秒
     
      delay(1000);
      httpclient.end();//断开连接（一定要断开，否则会出问题）
      WiFi.forceSleepBegin();//开始wifi睡眠      
      break;
      } 
      else 
      {     
      httpclient.end();//断开连接
      delay(100);
      error_count++;
        if(error_count>50)
        {
        error_count=0;
        }
      }
    }
}
//--------------------------------------------------------
//wifi连接函数
void wifi_connect()
{
    unsigned char f=0;//每次调用此函数时标志置0
    delay(10);
    WiFiManager wifiManager;//建立一个对象
    if(WiFi.status() == WL_CONNECTED)//wifi连接成功时会由autoConnect()返回标志
    {
      f=1;
    }
    if(f!=1)//如果wifi连接不成功则标志维持初始0状态不变
    {  
      wifiManager.autoConnect("esp8266","esp@wifi");//建立无线AP
      if(WiFi.status() == WL_CONNECTED)//判断连接状态、如果成功则置位标志
      f=1;
    }
    if(f==1)
    { 
      get_time();
    }
}
