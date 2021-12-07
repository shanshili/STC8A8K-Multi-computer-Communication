#include "KEYBOARD.H"
#include "DELAY.H"
#include "8A8K.h"
//#include "lcd1602.h"

#define keyboard P1
#define pressdtestled P01
/*   ——————————————————
     | 0   1   2   3
     | 4   5   6   7 
	 | 8   9   10  11
	 | 12  13  14  15
 	 ——————————————————
*/
/*
P1 矩阵键盘  c1-p1.7   r4-p1.0
keyvalue 为返回按键值
uint 为个位
decade 为十位
pressdtestledtestled press down测试灯
*/

unsigned int Check_Keydown()
{
	unsigned int KeyValue=0;
	keyboard=0xf0;
	if(keyboard!=0xf0)//如果按键按下
	{
		Delay10ms();//延时10ms消抖
		if(keyboard!=0xf0)//按键确实按下
		{	
			//判断按键所在列，以所在列的第一行的按键键值赋给KeyValue
			keyboard=0Xf0;
			switch(keyboard)
			{
				case(0Xe0):	KeyValue=12;break; //第一列按下
				case(0Xd0):	KeyValue=13;break; //第二列按下
				case(0Xb0): KeyValue=14;break; //第三列按下
				case(0X70): KeyValue=15;break; //第四列按下
			}
			//判断按键所在行
			keyboard=0X0f;
			Delay10ms();//---------赋值后不能立即判断，可能还没有拉低就进入switch
			switch(keyboard)
			{
				case(0X07):	KeyValue-=12;break;  //第一行按下
				case(0X0b):	KeyValue-=8;break;  //第二行按下
				case(0X0d): KeyValue-=4;break;  //第三行按下
				case(0X0e):	KeyValue-=0;break;  //第四行按下
				default: KeyValue+=80;
			}
			while(keyboard!=0x0f); //按键松手后退出//-------必要！！！
			return KeyValue;
		}
		else  //否则认为是信号干扰导致
		{
			return 0;  //认为没有按键按下
		}
	}
	return 0;  //如果没有按键按下返回零
}

void Keyboardmain()
{
	unsigned int unit=0,decade=0;
	while(1)
	{
		pressdtestled = 0;
		keyvalue=Check_Keydown(); 
		if (keyvalue!=0)//按下非零的按键才触发testled
		{
			pressdtestled = 1;
//		LCD_dischar(0, 0, keyvalue);
			Delay500ms();
			break;
		}
		unit=keyvalue%10; //个位
		decade=keyvalue/10; //十位
	}
}