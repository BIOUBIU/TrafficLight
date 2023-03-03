
//代码原创：wannenggong
#include "my_time.h"
#include "TEXT.h"
#include "wifi_time.h"
extern int8_t time_dat[];

void my_time_init()  
{
  setTime(time_dat[2],time_dat[1],time_dat[0],time_dat[4],time_dat[5],time_dat[6]);//初始化时间(格式为：时、分、秒、日、月、年)
  digitalClockDisplay();//上电显示初始时间2022-04-15-周五 11:59:00
  wifi_connect();//获取网络时间更新time_dat[]数据
  setTime(time_dat[2],time_dat[1],time_dat[0],time_dat[4],time_dat[5],time_dat[6]);//校准显示时间
}

void digitalClockDisplay()
{
  int8_t mo,da,wk,hh,hi,mm,ss;
  int32_t yy;  
  char xingqi[][4]= {"日","一","二","三","四","五","六"};//数组用于将set_week()函数换算结果的数字转换成汉字显示
  yy=year();
  mo=month();  
  da=day(); 
  hh=hour();
  mm=minute();
  ss=second();  
  wk=set_week(yy,mo,da);//换算星期几
//时钟运行为24小时制、时钟显示为12小时制  
//显示时间的转换
  if(hh>0&&hh<=12)
  {
    hi=hh;     //午前
  }
  if(hh>12)
  {
    hi=hh%12;  //午后
  }
  if(hh==0)
  {
    hi=12;     //零点显示为12点  
  }
//0:01:00和12:01:00校时  
  if(((hh==0)&&(mm==1)&&(ss==0))||((hh==12)&&(mm==1)&&(ss==0)))
  {
    my_time_init();
  }
  PutGB8_16(0,0,0,mo/10);
  PutGB8_16(0,0,1,mo%10);//显示月
  PutHZ16_16(0,1,"月");//月
  PutGB8_16(0,0,4,da/10);
  PutGB8_16(0,0,5,da%10);//显示日
  PutHZ16_16(0,3,"日");//日
  
  PutGB16_32(0,1,2,hi/10);
  PutGB16_32(0,1,3,hi%10);
  PutGB16_32(0,1,5,mm/10);
  PutGB16_32(0,1,6,mm%10);
  PutHZ16_16(0,6,"周");
  PutHZ16_16(0,7,xingqi[wk]);
 
  if(ss%2==0)
  PutGB16_32(0,1,4,10);//显示秒点儿
  else
  PutGB16_32(0,1,4,11);//显示空格
}
