/*
  【DUTY BOX】：
	slave2 地址 0x02
	从机2完成流水灯
	P2显示数据
	P17为指示灯
	中断x调用串口接收数据
	【DEBUG NOTE】:
	1、保证中断里没有循环，可以尽快退出，将其他操作放入主程序，使用【FLAG】作为中断运行过标志
	2、已修正“循环不能打断”问题，放弃循环，在主程序加入死循环，以【FLAG】为条件，间接实现循环
	【UPDATE LOG】：
	21/12/9 slave2.c替换为slave2_2.c，解决循环无法被打断的问题，及瀑布流水灯bug
*/
#include "8A8K.h"
#include "delay.h"
#include "intrins.h"
#include <math.h>
#define rxINTtestled P17
#define LEDPLAY P2
unsigned char dataa,led,flow,flag=0;
unsigned char seg[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

//接收测试数码管显示
void datatest()
{
	  unsigned char ledcycle;
	  dataa = SBUF;
		switch(dataa)
		{
			case(0x01): //循环右移
					led = 0x01;
					for(ledcycle=0;ledcycle<7;ledcycle++)
					{
							led = _cror_(led,1);
							LEDPLAY = led ;
							Delay500ms();
					}
					break;
			case(0x02)://循环左移
					led = 0x03;
					for(ledcycle=0;ledcycle<7;ledcycle++)
					{
							led = _crol_(led,1);
							LEDPLAY = led ;
							Delay500ms();
					}
					break;
			case(0x03)://1100 1100
					LEDPLAY = 0xcc;
					Delay500ms();
					LEDPLAY = 0X33;
				  Delay500ms();
					break;
			case(0x04):  //1010 1010
					LEDPLAY = 0x55;
					Delay500ms();
					LEDPLAY = 0XAA;
				  Delay500ms();
			    break;
			case(0x05): //瀑布
					led = 0xff;
					for(flow=0;flow<8;flow++)
					{
						led = 0xff - (pow(2,flow)-1);
						LEDPLAY = led;
						Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();
						Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();
						Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();
					}
					break;
			case(0x06):
					LEDPLAY = 0x00;
					Delay500ms();
					Delay500ms();
					break;
			default:LEDPLAY = 0Xff;Delay500ms();
		}
	//	P2 = dataa;
}
//串口一中断：与地址相匹配的数据来后进入中断
void UART1()interrupt 4 using 1
//void UART1() __interrupt (4) __using (1) //串口1中断服务函数//触发条件：地址数据，且地址相匹配（此时已将RI置1）
{
    ES = 0; //关闭串口1中断
    RI = 0; //不清零的话能否继续接收
	//-------返回响应
	  SBUF = SADDR+0x01 ;SM2 = 0;//接收数据
	  while(!TI);
	  TI = 0;
	//--------
		
    rxINTtestled = 1; //表示已准备好接受数据
		Delay10ms();

    while(!RI); //接收完数据
		rxINTtestled = 0;
    RI = 0; //清除接收标志位
    ES = 1; //重新开启串口1中断
		SM2 = 1;//接收地址
		flag = 1;
	  ///datatest(); //内有死循环，未打开串口，无法再次进入【已用flag更正】
}

//slave串口设置
void UartInit(void)		//9600bps@24.000MHz
{
	  P3M1 = 0X01;     //设置为高阻
	  P3M0 = 0X02;
    P_SW1 = 0X00;    //端口映射3.0 3.1
    SCON = 0xF0;		//串口控制：串口1，9位数据,可变波特率  1111 0000 允许串口接收数据，筛选地址帧,RB8=1为地址帧，地址信息可以进入SBUF并使RI为1，进而在中断服务程序中再进行地址号比较；
		PCON &= 0x7F;		//电源管理：波特率不倍速 PCON=0X00 SMOD=0
    AUXR &= 0xFE;		//选择波特率发生器：定时器1
		AUXR |= 0x40;		//定时器1时钟：Fosc,即1T T1x12=1
		TMOD &= 0x0F;		//定时器1模式配置：定时，16位自动重装方式
		TL1 = 0x8F;		//设定定时低字节: 
		TH1 = 0xFD;		//设定定时高字节:  64911 65536-(SYSCLK/4/baud)
		ET1 = 0;		//定时器1中断：禁止
    ES = 1;         //串口1中断: 开启
	  IP |= 0X10;
	  IPH |=0X10; 
    TR1 = 1;        //启动定时器1
    EA = 1;         //中断 总开关
		TR1 = 1;		//启动定时器1
}

void main()
{
		P2M0=0X00;
		P2M1=0X00;
    UartInit();
    SADDR = 0X02;
    SADEN = 0X0F;
	  rxINTtestled = 0;
    while (1)
		{
				if( flag == 1)
					  datatest();
		} 
}