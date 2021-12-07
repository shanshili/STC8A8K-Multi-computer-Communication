#include "8A8K.H"
#include "LCD1602.h"
#define testled2 P04
	 
void delay(uint n)//延时函数
{
		uint i;
		uchar j;
				for(i=n;i>0;i++)//！！延时没写对，导致的不显示
					for(j=0;j<2;j++);
}

//读写判断函数
void rwjudge()
{
	uchar stateword;
	LCD_DB=0xFF;
	LCD_RS=0;
  LCD_RW=1;
	do
	{
			LCD_E=1;
			stateword=LCD_DB;//第七位携带使能数据，sta7=1,禁止读写；sta7=0,允许读写
			LCD_E=0;//满足条件表示sta=1禁止，则一直读取状态字,直到=0
  }
	while(stateword & 0x80);//与1都为a，与0都为0，我想判断第七位，则第七位为1则整体>0，第七位为0，则整体=0  
}

//初始化函数
void LCD_init(void)
{
	rwjudge();
	LCD_w_command(0x38);//设置格剩（即功能设定指令 DL=1数据总线8位 N=2显示两行 F=0 5*7点阵）� 8位 2行 5*7
	LCD_w_command(0x0c);//整体显示、关光标、不闪烁(显示开关控制命令)
	LCD_w_command(0x06);//（输入模式设置  ，增量不移位
	LCD_w_command(0x01);//清除屏幕
	delay(1);
}

//写指令
void LCD_w_command(uchar dat)
{	
	rwjudge();
	LCD_RS=0;
	LCD_RW=0;
	LCD_DB=dat;
	LCD_E=1;
	 //dat顺序不知道有无影响，答：会有影响
	LCD_E=0;
}

//写数据
void LCD_w_data(uchar dat)
{	
	rwjudge();
	LCD_RS=1;
	LCD_RW=0;
	LCD_DB=dat;
	LCD_E=1;
	LCD_E=0;
}

//显示一个字符
void LCD_dischar(uchar x,uchar y,uchar dat)
{//y=0第二行；y=1第一行   //x是前面空的格数
		uchar address;
		if(y==1)
			address=0x80+x;
		else 
			address=0xc0+x;///有可能是区分两行
		LCD_w_command(address);
		LCD_w_data(dat);
}

//写一个字符串
void LCD_disstr(uchar x, uchar y, uchar *str)
{   //y=1第二行；y=0第一行。x是前面空的格数
	uchar addr;
	addr = y*0x40+x;
	LCD_w_command(addr | 0x80);
	while(*str != '\0')
	{
		LCD_w_data(*str++);
	}
}




