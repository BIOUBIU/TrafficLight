/********************金逸晨**************************
*****************GM12864-01A   SPI LCD  FOR Arduino*************

*****BY:GU
*********ST7567
本模块自带字库型号：金逸晨定制字库V1.0  字库存储为水平扫描方式从上到下，不足一字节补足

12864模块，由于其数据8位为一个显示基本单位，推荐建立128*64/8显存，
以整屏数据刷新的方式杜绝显示过程中的覆盖现象

通过cs选择屏，CS2选择字库
本模块自带背光限流电阻，使用BL 引脚调整背光亮度，推荐使用pwm信号控制亮度，也可直接高电平

********************************************************/

#define SPI_SCK_0  digitalWrite(2,LOW)   //字库sck与屏复用            
#define SPI_SCK_1  digitalWrite(2,HIGH)
#define SPI_SDA_0  digitalWrite(3,LOW)    //字库SDA与屏复用         
#define SPI_SDA_1  digitalWrite(3,HIGH) 
#define SPI_FSO    digitalRead(4)     //字库输出
#define SPI_RST_0  digitalWrite(5,LOW)               
#define SPI_RST_1  digitalWrite(5,HIGH)
#define SPI_DC_0  digitalWrite(6,LOW)               
#define SPI_DC_1  digitalWrite(6,HIGH)
#define SPI_CS_0  digitalWrite(7,LOW)               
#define SPI_CS_1  digitalWrite(7,HIGH)
#define SPI_CS2_0  digitalWrite(8,LOW)               
#define SPI_CS2_1  digitalWrite(8,HIGH)
#define BL_0  digitalWrite(9,LOW)               
#define BL_1  digitalWrite(9,HIGH)


//#define LCD_DIS_CHINA_ROT  180  //选择屏幕旋转参数，不旋转0，，旋转180度：180
#define LCD_DIS_CHINA_ROT  0

#define LCD_COLUMN_NUMBER 128
#define LCD_LINE_NUMBER 64

  #if (LCD_DIS_CHINA_ROT == 180) 
  #define LCD_COLUMN_OFFSET 4         //旋转180度需要加4个偏移量
  #else 
  #define LCD_COLUMN_OFFSET 0
  #endif

#define LCD_PAGE_NUMBER (LCD_LINE_NUMBER/8)
#define LCD_COMMAND_NUMBER  13


//指令表
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

//字库基地址
#define CHAR6_12_ADD     0X1000L 
#define CHAR7_14_ADD     0X1600LL
#define CHAR8_16_ADD     0X1D00L 
#define CHAR9_18_ADD     0X2500L
#define CHAR10_20_ADD     0X3700L
#define CHAR11_22_ADD     0X4B00L 
#define CHAR12_24_ADD     0X6100L
#define CHAR13_26_ADD     0X7900L

#define CHINA12_12_ADD     0X9300L 
#define CHINA14_14_ADD     0X39300L 
#define CHINA16_16_ADD     0X71300L 
#define CHINA18_18_ADD     0XB1300L 
#define CHINA20_20_ADD     0X11D300L 
#define CHINA22_22_ADD     0X195300L
#define CHINA24_24_ADD     0X219300L
#define CHINA26_26_ADD     0X2A9300L 
#define END_ADD             0X379300L


#define TRUE             1
#define FALSE           0

typedef enum        // 不同字体选择
{
     SONG_STYLE12,SONG_STYLE14,SONG_STYLE16,SONG_STYLE18,SONG_STYLE20,SONG_STYLE22,SONG_STYLE24,SONG_STYLE26
}type_of_font;
type_of_font TYPE_OF_STYLE;

typedef enum        // 不同字体选择
{
     NORMAL,REVERSE
}type_of_dis;
type_of_dis TYPE_OF_DIS;

struct                   //显示字符参数传递结构体
{                
    unsigned char  CHAR_WIDE;           //英文字体宽度       
    unsigned char  CHAR_HIGH;           //英文字体高度
    unsigned char  WORD_WIDE;           //汉字宽度
    unsigned char  WORD_HIGH;           //汉字高度
    unsigned  int CHAR_DATA_SIZE;     //英文一个字符总数据大小  字节
    unsigned  int WORD_DATA_SIZE;     //汉字一个字符总数据大小  字节
    type_of_dis TYPE_OF_DIS;              //字符颜色
    unsigned long   BASE_WORD_ADD;           //汉字字库基地址
    unsigned long   BASE_CHAR_ADD;           //英文字库基地址

} DIS_CHAR_MODE ;



 unsigned char FONT_BUFFER[104];         //字库缓存，支持最大26*26汉字，即4*26字节           
const unsigned char  *point;
unsigned char ACK=0;
unsigned char READ_BACK[32];                    ////一个汉字最大返回32个字节
unsigned char CHAR_DATA[32];

const unsigned char DIS_CHINA[] = 
{
  0x47,0x42,0x32,0x33,0x31,0x32,0xd7,0xd6,0xbf,0xe2,0xb2,0xe2,0xca,0xd4,0x00,
};

const unsigned char  LCD_init_cmd[LCD_COMMAND_NUMBER]=
{
       0xe2, //软复位

  0x2c, //升压步聚1
  
  0x2e, //升压步聚2
  
  0x2f, //升压步聚3
  
  0x24, //粗调对比度，可设置范围0x20～0x27
  0x81, //微调对比度
  0x22, //微调对比度的值，可设置范围0x00～0x3f
  0xa2, //1/9 偏压比（bias）
  #if (LCD_DIS_CHINA_ROT == 180) 
  0xA1, //列扫描顺序：从左到右
  0xc0, //行扫描顺序：反序
  #else 
  0xA0, //列扫描顺序：从左到右
  0xc8, //行扫描顺序：反序
  #endif
  0xa6, //正显
  0x40, //起始行：第一行开始
  0xaf, //开显示
};

const unsigned char  picture_tab[]={
/*------------------------------------------------------------------------------
;  列行式，低位在前，阴码
;  宽×高（像素）: 128×64
------------------------------------------------------------------------------*/
    0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x20,0x44,0x08,0x00,0x18,0x48,0x28,0xC8,0x08,0x28,0x48,0x18,0x00,
0x40,0x40,0xFC,0x40,0x40,0xFC,0x00,0x00,0xF8,0x00,0x00,0xFC,0x00,0x40,0x40,0xA0,
0x90,0x88,0x84,0x88,0x90,0x20,0x40,0x40,0x00,0x00,0x40,0x44,0xD8,0x20,0xF0,0xAC,
0xA8,0xE8,0xB8,0xA8,0xE0,0x00,0x00,0x00,0xC0,0x7C,0x54,0x54,0x54,0x54,0x54,0x54,
0x7C,0x40,0x40,0x00,0x00,0xF0,0x90,0x90,0x90,0xFC,0x90,0x90,0x90,0xF0,0x00,0x00,
0x00,0x80,0x88,0x88,0x88,0x88,0x88,0xE8,0xA8,0x98,0x8C,0x88,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x04,0x3E,0x01,0x10,0x11,0x09,0x05,0x3F,0x05,0x09,0x11,0x11,0x00,
0x08,0x18,0x0F,0x24,0x14,0x0F,0x00,0x00,0x0F,0x00,0x00,0x3F,0x00,0x20,0x22,0x2A,
0x32,0x22,0x3F,0x22,0x32,0x2A,0x22,0x20,0x00,0x00,0x20,0x10,0x0F,0x10,0x28,0x24,
0x23,0x20,0x2F,0x28,0x2A,0x2C,0x00,0x30,0x0F,0x04,0x3D,0x25,0x15,0x15,0x0D,0x15,
0x2D,0x24,0x24,0x00,0x00,0x07,0x04,0x04,0x04,0x1F,0x24,0x24,0x24,0x27,0x20,0x38,
0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x81,0x41,0x21,0x21,0x61,0x01,0x01,0x21,0xE1,0xE1,0x01,0xE1,0xE1,
0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x21,0xE1,0x21,0x21,0x21,0x61,0x01,0x01,
0x21,0x21,0xE1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0x10,0x20,0x24,0x1C,0x04,0x00,0x20,0x3F,0x01,0x3E,0x01,0x3F,
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x22,0x22,0x27,0x30,0x00,0x00,
0x20,0x20,0x3F,0x20,0x20,0x00,0x00,0x1E,0x25,0x25,0x25,0x16,0x00,0x00,0x1E,0x21,
0x21,0x21,0x13,0x00,0x01,0x01,0x1F,0x21,0x21,0x00,0x00,0x00,0x21,0x3F,0x22,0x21,
0x01,0x00,0x00,0x1E,0x21,0x21,0x21,0x1E,0x00,0x21,0x3F,0x22,0x01,0x01,0x3E,0x20,
0x00,0x21,0x21,0x3F,0x20,0x20,0x00,0x00,0x1E,0x21,0x21,0x21,0x13,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xF0,0x08,0x04,0x04,0x04,0x0C,0x00,0xF0,0x08,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xFC,0x04,0x00,
0x00,0x00,0x00,0x00,0x0C,0x04,0xFC,0x04,0x0C,0x00,0x04,0xFC,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x81,0x82,0x84,0x84,0x84,0x82,0x80,0x81,0x82,0x84,0x84,0x82,0x81,
0x80,0x80,0x86,0x86,0x80,0x80,0x80,0x80,0x80,0x85,0x83,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x84,0x87,0x84,0x84,
0x84,0x86,0x80,0x80,0x80,0x84,0x87,0x84,0x80,0x80,0x84,0x87,0x84,0x84,0x82,0x81,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF

};

void  IO_init(void )
{
pinMode(2,OUTPUT);//设置数字脚为输出
pinMode(3,OUTPUT);//设置数字脚为输出
pinMode(4,INPUT);//设置数字脚为输出
pinMode(5,OUTPUT);//设置数字脚为输出
pinMode(6,OUTPUT);//设置数字脚为输出
pinMode(7,OUTPUT);//设置数字脚为输出
pinMode(8,OUTPUT);//设置数字脚为输出
pinMode(9,OUTPUT);//设置数字脚为输出
//pinMode(10,OUTPUT);//设置数字脚为输出
//pinMode(11,OUTPUT);//设置数字脚为输出
//pinMode(12,OUTPUT);//设置数字脚为输出
//pinMode(A0,INPUT);//设置数字脚为输入

}
void delay_us(unsigned int _us_time)
{       
  unsigned char x=0;
  for(;_us_time>0;_us_time--)
  {
    x++;
  }
}
/*************SPI配置函数*******************
SCL空闲时低电平，第一个上升沿采样
模拟SPI
******************************************/

/**************************SPI模块发送函数************************************************

 *************************************************************************/
void SPI_SendByte(unsigned  char byte)        //向SPI接口写一个8位数据
{ 
  unsigned char counter;   
  for(counter=0;counter<8;counter++)
  { 
    SPI_SCK_0;    
    if((byte&0x80)==0)
    {
      SPI_SDA_0;
    }
    else SPI_SDA_1;
    byte=byte<<1; 
    SPI_SCK_1;      
  }
  
  SPI_SCK_0;
}

void LCD_send_cmd(unsigned char o_command)
  {
    SPI_DC_0;
    SPI_CS_0;
    SPI_SendByte(o_command);
    SPI_CS_1;
   
    //SPI_DC_1;
  }
  //向液晶屏写一个8位数据
void LCD_send_data(unsigned  char o_data)
  { 
    SPI_DC_1;
    SPI_CS_0;
    SPI_SendByte(o_data);
    SPI_CS_1;
    
   }

unsigned char ReadByte(void)        //从字库中读数据函数
{
  unsigned char i;
  unsigned char dat=0;
  
  SPI_SCK_1;
  for(i=0;i<8;i++)
  {
    SPI_SCK_0;
    dat=dat<<1;
    if(SPI_FSO)
      dat=dat|0x01;
    else 
      dat&=0xfe;
    SPI_SCK_1 ;   
  } 
  
  return dat;
}

void Column_set(unsigned char column)
  {
  column+=LCD_COLUMN_OFFSET;
    LCD_send_cmd(0x10|(column>>4));    //设置列地址高位
    LCD_send_cmd(0x00|(column&0x0f));   //设置列地址低位  
       
  }
void Page_set(unsigned char page)
  {
    LCD_send_cmd(0xb0+(page&0X07));
  }
void LCD_clear(void)
  {
    unsigned char page,column;
    for(page=0;page<LCD_PAGE_NUMBER;page++)             //page loop
      { 
          Page_set(page);
          Column_set(0);    
          for(column=0;column<LCD_COLUMN_NUMBER;column++) //column loop
            {
              LCD_send_data(0x00);
            }
      }
  }
void LCD_full(void)
  {
    unsigned char page,column;
    for(page=0;page<LCD_PAGE_NUMBER;page++)             //page loop
      { 
        Page_set(page);
        Column_set(0);    
  for(column=0;column<LCD_COLUMN_NUMBER;column++) //column loop
          {
            LCD_send_data(0xff);
          }
      }
  }
void LCD_init(void)
  {
    unsigned char i;
    for(i=0;i<LCD_COMMAND_NUMBER;i++)
      {
        LCD_send_cmd(LCD_init_cmd[i]);
      }
  }

void Picture_display(const unsigned char *ptr_pic)
  {
    unsigned char page,column;
    for(page=0;page<(LCD_LINE_NUMBER/8);page++)        //page loop
      { 
  Page_set(page);
  Column_set(0);    
  for(column=0;column<LCD_COLUMN_NUMBER;column++) //column loop
          {
            LCD_send_data(*ptr_pic++);
          }
      }
  }
void Picture_ReverseDisplay(const unsigned char *ptr_pic)
{
    unsigned char page,column,data;
    for(page=0;page<(LCD_LINE_NUMBER/8);page++)        //page loop
      { 
  Page_set(page);
  Column_set(0);    
  for(column=0;column<LCD_COLUMN_NUMBER;column++) //column loop
          {
            data=*ptr_pic++;
            data=~data;
            LCD_send_data(data);
          }
      }
  }
/*******************************************字库调用函数******************/
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(unsigned char *pBuffer, unsigned long ReadAddr, unsigned  int NumByteToRead)
{
  unsigned  int i;
    unsigned char counter,redata=0;
  SPI_CS2_0;             //使能器件
  SPI_SendByte(W25X_ReadData);//发送读取命令       
  SPI_SendByte((unsigned char)(ReadAddr >> 16));    //发送24bit地址
    SPI_SendByte((unsigned char)(ReadAddr >> 8));    //发送24bit地址
    SPI_SendByte((unsigned char)(ReadAddr));    //发送24bit地址
  for (i = 0; i < NumByteToRead; i++)         //循环读数
  {           
      for(counter=0;counter<8;counter++)
      { 
            SPI_SCK_0;    
            SPI_SDA_1;
            redata <<=1;
            if(SPI_FSO)
            {
               redata |=0x01; 
            } 
            SPI_SCK_1;      
      }
      
        SPI_SCK_0;
        pBuffer[i] = redata;
  }
  SPI_CS2_1;
}
unsigned char CHECK_FALSH(void)
{
    unsigned int x=0;
    const unsigned char string[] = "JYC-4MbByte-FONT-FLASH";
    W25QXX_Read(FONT_BUFFER, 0X000000, 22);
    for(x=0;x<22;x++)
    {
       if(FONT_BUFFER[x] != string[x]) 
       {
           return(FALSE);
       } 
    }
    return(TRUE);
}
void SET_FONT_STYLE (type_of_font TYPE_CHAR,type_of_dis DIS_TYPE)  
{
  if(DIS_TYPE != NORMAL)
  {
    DIS_CHAR_MODE.TYPE_OF_DIS = REVERSE;
  }
  else
  {
    DIS_CHAR_MODE.TYPE_OF_DIS = NORMAL;
  }
    switch (TYPE_CHAR)
    {
        case SONG_STYLE12: 
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR6_12_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA12_12_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 1*12;
                DIS_CHAR_MODE.CHAR_HIGH = 12;
                DIS_CHAR_MODE.CHAR_WIDE = 6;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 2*12;
                DIS_CHAR_MODE.WORD_HIGH = 12;
                DIS_CHAR_MODE.WORD_WIDE = 12;
        break;
        
        case SONG_STYLE14:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR7_14_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA14_14_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 1*14;
                DIS_CHAR_MODE.CHAR_HIGH = 14;
                DIS_CHAR_MODE.CHAR_WIDE = 7;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 2*14;
                DIS_CHAR_MODE.WORD_HIGH = 14;
                DIS_CHAR_MODE.WORD_WIDE = 14;
            break;
        case SONG_STYLE16:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR8_16_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA16_16_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 1*16;
                DIS_CHAR_MODE.CHAR_HIGH = 16;
                DIS_CHAR_MODE.CHAR_WIDE = 8;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 2*16;
                DIS_CHAR_MODE.WORD_HIGH = 16;
                DIS_CHAR_MODE.WORD_WIDE = 16;
            break;
        case SONG_STYLE18:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR9_18_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA18_18_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 2*18;
                DIS_CHAR_MODE.CHAR_HIGH = 18;
                DIS_CHAR_MODE.CHAR_WIDE = 9;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 3*18;
                DIS_CHAR_MODE.WORD_HIGH = 18;
                DIS_CHAR_MODE.WORD_WIDE = 18;
            break;
        case SONG_STYLE20:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR10_20_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA20_20_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 2*20;
                DIS_CHAR_MODE.CHAR_HIGH = 20;
                DIS_CHAR_MODE.CHAR_WIDE = 10;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 3*20;
                DIS_CHAR_MODE.WORD_HIGH = 20;
                DIS_CHAR_MODE.WORD_WIDE = 20;
            break;
        case SONG_STYLE22:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR11_22_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA22_22_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 2*22;
                DIS_CHAR_MODE.CHAR_HIGH = 22;
                DIS_CHAR_MODE.CHAR_WIDE = 11;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 3*22;
                DIS_CHAR_MODE.WORD_HIGH = 22;
                DIS_CHAR_MODE.WORD_WIDE = 22;
            break;
        case SONG_STYLE24:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR12_24_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA24_24_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 2*24;
                DIS_CHAR_MODE.CHAR_HIGH = 24;
                DIS_CHAR_MODE.CHAR_WIDE = 12;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 3*24;
                DIS_CHAR_MODE.WORD_HIGH = 24;
                DIS_CHAR_MODE.WORD_WIDE = 24;
            break;
        case SONG_STYLE26:
                DIS_CHAR_MODE.BASE_CHAR_ADD = CHAR13_26_ADD;
                DIS_CHAR_MODE.BASE_WORD_ADD = CHINA26_26_ADD;
                DIS_CHAR_MODE.CHAR_DATA_SIZE = 2*26;
                DIS_CHAR_MODE.CHAR_HIGH = 26;
                DIS_CHAR_MODE.CHAR_WIDE = 13;
                DIS_CHAR_MODE.WORD_DATA_SIZE = 4*26;
                DIS_CHAR_MODE.WORD_HIGH = 26;
                DIS_CHAR_MODE.WORD_WIDE = 26;
            break;
    }
    
}
 
void DIS_CHINESE(unsigned  int x_start,unsigned  int y_start,unsigned char *string)      //显示字符串，支持中英混显/GB2312编码
{
    unsigned char CACHE=0,CACHE_FONT[4*32];
    unsigned long Address;
    unsigned char  WORD_CODE_MSB,WORD_CODE_LSB;
    unsigned int x=0,y=0,z=0,m,n,f;
    unsigned int ADD_X_START = x_start, \
                 ADD_Y_START = y_start,  \
                 ADD_X_END = x_start + DIS_CHAR_MODE.WORD_WIDE-1,  \
                 ADD_Y_END = y_start + DIS_CHAR_MODE.WORD_HIGH;
    

/*******************************************/
   while(*string!='\0')
    { 
        WORD_CODE_MSB =(unsigned char)  *string++;
        WORD_CODE_LSB = (unsigned char) *string++;
        for(n=0;n<(4*26);n++)            
        {    
      FONT_BUFFER[n] = 0; //  清空字库缓存
    }
        if((WORD_CODE_MSB>=0xA1) &&  (WORD_CODE_LSB >=0xA1))            //GB2312编码范围,序列号1410  为：啊 后续为：阿埃挨暗   
        {
            Address = (WORD_CODE_MSB - 0xA1) * 94 ;
            Address = (Address + (WORD_CODE_LSB - 0xA1));
            Address =Address *(DIS_CHAR_MODE.WORD_DATA_SIZE);
            Address =Address + DIS_CHAR_MODE.BASE_WORD_ADD ;
            W25QXX_Read(FONT_BUFFER, Address, (DIS_CHAR_MODE.WORD_DATA_SIZE));            
            ADD_X_END = ADD_X_START + DIS_CHAR_MODE.WORD_WIDE-1;
      
/*******************************地址设定************************************///横向显示

            if((ADD_X_END>(LCD_COLUMN_NUMBER-1)) )          //超出x地址范围,转到下一页
            {                                           
                 ADD_Y_START = ADD_Y_START+DIS_CHAR_MODE.WORD_HIGH/8;
                 ADD_X_START = 0;//((ADD_X_END % TFT_COLUMN_NUMBER) / DIS_CHAR_MODE.WORD_WIDE ) *DIS_CHAR_MODE.WORD_WIDE + DIS_CHAR_MODE.WORD_WIDE;
                 ADD_X_END = DIS_CHAR_MODE.WORD_WIDE-1;
         ADD_Y_END = ADD_Y_START + DIS_CHAR_MODE.WORD_HIGH/8;
                 if (ADD_Y_END > LCD_PAGE_NUMBER )       //超出Y范围
                 {
                    ADD_Y_START = 0;                          //移动到第一行
                        
                 }
            }

/************************************************显示*************************************************************/              
            
      x = DIS_CHAR_MODE.WORD_DATA_SIZE / DIS_CHAR_MODE.WORD_HIGH;
      f = DIS_CHAR_MODE.WORD_DATA_SIZE / DIS_CHAR_MODE.WORD_WIDE *8;
            for (m=0;m<f ;m++)      //按行处理显示数据
            {   
                    for(n=0;n<x;n++)            //  取完一行数
                    {    
            CACHE = FONT_BUFFER[n + x*m];
            for(z=0;z<8;z++)
            { 
              y = m/8 * DIS_CHAR_MODE.WORD_WIDE+ n*8+z;
              CACHE_FONT[y]=((CACHE_FONT[y]>>1)&0X7F)    \
              | (CACHE&0X80);
              CACHE <<=1;
            }                            
                    }
            }
      x = DIS_CHAR_MODE.WORD_DATA_SIZE / DIS_CHAR_MODE.WORD_WIDE;
      for(m=0;m<x;m++)
      {
        Column_set(ADD_X_START);
        Page_set(ADD_Y_START+m);
        for(n=0;n<DIS_CHAR_MODE.WORD_WIDE;n++)  
        {
          if(DIS_CHAR_MODE.TYPE_OF_DIS == NORMAL)
          {
            LCD_send_data(CACHE_FONT[m * DIS_CHAR_MODE.WORD_WIDE+ n]); 
          }
          else
          {
            LCD_send_data(~CACHE_FONT[m * DIS_CHAR_MODE.WORD_WIDE+ n]);
          }
        }
      }
      
      ADD_X_START += DIS_CHAR_MODE.WORD_WIDE-1;     
        }
        else    //英文范围
        {           

            Address = (WORD_CODE_MSB ) * DIS_CHAR_MODE.CHAR_DATA_SIZE + DIS_CHAR_MODE.BASE_CHAR_ADD; 
            string--;
            ADD_X_END = ADD_X_START + DIS_CHAR_MODE.CHAR_WIDE-1;             
            W25QXX_Read(FONT_BUFFER, Address, (DIS_CHAR_MODE.CHAR_DATA_SIZE)); 

 /*******************************地址设定************************************/
                
             //判断地址是否合法
                if((ADD_X_END>(LCD_COLUMN_NUMBER-1)) )          //超出x地址范围,转到下一行
                {                                           
                    ADD_Y_START = ADD_Y_START+ DIS_CHAR_MODE.CHAR_HIGH/8;
                    ADD_X_START = 0;//((ADD_X_END % TFT_COLUMN_NUMBER) / DIS_CHAR_MODE.WORD_WIDE ) *DIS_CHAR_MODE.WORD_WIDE + DIS_CHAR_MODE.WORD_WIDE;
                    ADD_X_END = DIS_CHAR_MODE.CHAR_WIDE-1;            
                    ADD_Y_END = ADD_Y_START + DIS_CHAR_MODE.CHAR_HIGH/8;
                    if (ADD_Y_END > LCD_PAGE_NUMBER )       //超出Y范围
           {
            ADD_Y_START = 0;                          //移动到第一行
              
           }
                    
                }
           
///*************************************************************************************************************/      
      x = DIS_CHAR_MODE.CHAR_DATA_SIZE / DIS_CHAR_MODE.CHAR_HIGH;
      f = DIS_CHAR_MODE.CHAR_DATA_SIZE / DIS_CHAR_MODE.CHAR_WIDE *8;
            for (m=0;m<f ;m++)      //按行处理显示数据
            {   
                    for(n=0;n<x;n++)            //  取完一行数
                    {    
            CACHE = FONT_BUFFER[n + x*m];
            for(z=0;z<8;z++)
            { 
              y = m/8 * DIS_CHAR_MODE.CHAR_WIDE+ n*8+z;
              CACHE_FONT[y]=((CACHE_FONT[y]>>1)&0X7F)    \
              | (CACHE&0X80);
              CACHE <<=1;
            }                            
                    }
            }
      x = DIS_CHAR_MODE.CHAR_DATA_SIZE / DIS_CHAR_MODE.CHAR_WIDE;
      for(m=0;m<x;m++)
      {
        Column_set(ADD_X_START);
        Page_set(ADD_Y_START+m);
        for(n=0;n<DIS_CHAR_MODE.CHAR_WIDE;n++)  
        {
          if(DIS_CHAR_MODE.TYPE_OF_DIS == NORMAL)
          {
            LCD_send_data(CACHE_FONT[m * DIS_CHAR_MODE.CHAR_WIDE+ n]) ;
          }
          else
          {
            LCD_send_data(~CACHE_FONT[m * DIS_CHAR_MODE.CHAR_WIDE+ n]) ;
          }
          
        }
      }
            ADD_X_START = ADD_X_END;        
        }
    }            
}
void  setup(){
point=&picture_tab[0];
IO_init();
//Serial.begin(9600);
SPI_SCK_0; 
SPI_RST_0;
BL_1;
delay(30);
SPI_RST_1;
delay(30);
LCD_init();
LCD_full();
delay(500);
LCD_clear();
}

void  loop(){
Picture_display(point);
    delay(1000);
    
    Picture_ReverseDisplay(point);
    delay(1000);
    LCD_clear();
    SET_FONT_STYLE (SONG_STYLE12,NORMAL);//先设置显示样式    
    DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
        SET_FONT_STYLE (SONG_STYLE14,REVERSE);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
        SET_FONT_STYLE (SONG_STYLE16,NORMAL);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
        SET_FONT_STYLE (SONG_STYLE18,REVERSE);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
        SET_FONT_STYLE (SONG_STYLE20,NORMAL);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
        SET_FONT_STYLE (SONG_STYLE22,REVERSE);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
    SET_FONT_STYLE (SONG_STYLE24,NORMAL);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
    SET_FONT_STYLE (SONG_STYLE26,REVERSE);//先设置显示样式
  DIS_CHINESE(0,0,DIS_CHINA);
    delay(1000);LCD_clear();
}
