
#include "ST7565_ESP.h"
#include "TEXT.h"
#include "shuzi.h"

//====写8*16数字函数====================================
//以下所有显示行(H)排列顺序为自上而下2301
//xh=1为调时状态、相应字符有下划线显示，xh=0为运行显示没有下划线
void PutGB8_16(uint8_t xh,uint8_t H,uint8_t W,uint8_t shu_zi)
{
    uint8_t i,j=0,k;

    for(k=0; k<2; k++)
    {
        SetColumn(W*8);
        SetPage(H*2+k);
        j=k*8;
        if((xh==1)&&(k==1))
        {
            for(i=0; i<8; i++)
            {
                wr_lcd(1,shuzi_16[shu_zi][i+j]|0x80);
            }
        }
        else
        {
            for(i=0; i<8; i++)
            {
                wr_lcd(1,shuzi_16[shu_zi][i+j]);
            }
        }
    }

}
//====写16*32数字函数====================================
//H=32有效值为H=0、H=1
void PutGB16_32(uint8_t xh,uint8_t H,uint8_t W,uint8_t shu_zi)
{
    uint8_t i,k;

    for(k=0; k<4; k++)
    {
        SetColumn(W*16);//根据字宽调整
        SetPage(H*2+k);//此项不变
        if(xh==1)
        {
            for(i=0; i<16; i++)//由字宽确定
            {
                wr_lcd(1,~shuzi_32[shu_zi][i+k*16]);//xh=1反白显示
            }
        }
        else
        {
            for(i=0; i<16; i++)
            {
                wr_lcd(1,shuzi_32[shu_zi][i+k*16]);//xh=0正常显示
            }
        }
    }
}

//====写16*16汉字函数====================================
//以下所有显示行(H)排列顺序为自上而下2301
void PutHZ16_16(uint8_t H,uint8_t W,char* c)
{
    uint8_t i,j=0,k;
    char date[32];
//读出一个汉字的点阵数据
    for(k=0; k<12; k++)  //自建汉字库中的个数，循环查询内码
    {
        if((GB_16[k].Index[0]==c[0])&&(GB_16[k].Index[1]==c[1])&&(GB_16[k].Index[2]==c[2]))//用于utf8编码三个字节对应一个汉字
        {
            for(i=0; i<32; i++)
            {
                date[i]=GB_16[k].Msk[i];
            }
        }
    }
//写出一个汉字
    for(k=0; k<2; k++)
    {
        SetColumn(W*16);
        SetPage(H*2+k);
        j=k*16;

        for(i=0; i<16; i++)
        {
            wr_lcd(1,date[i+j]);
        }
    }
}
