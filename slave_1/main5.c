/*
  【INFORMAION】
	串口1：9位数据,可变波特率
	定时器1:16位自动重载 
	串口1选择定时器1为波特率发生器
	定时器0：16位自动重载 ———>占空比发生
  【DUTY BOX】：
	slave1 地址 0x01
	从机1完成脉冲波形发生，频率、占空比可以设置；
	P2显示数据
	P00为指示灯
	中断x调用串口接收数据
	显示：
		输入频率
		输入占空比
	frequency frequency
	duty_cycle dutycycle
	【DEBUG NOTE】:
	1、从机串口通信接收存在漏洞
	【UPDATE LOG】：
	*21/12/9* 更新为main2.c:收发方式修正,配合从机main4版本使用
	*21/12/12*更新为main4.c:收发方式修正为可接收长度和多个数据，且接收停止连接位
						配合主机main4_2版本使用
	*21/12/13*接收时，只接收一个数，不再接受数据长度
						更新为math5.c
*/

#include "8A8K.h"
#include "delay.h"
#include "FrequencyDutycycle.H"
#include <math.h>
#define rxINTtestled P00
#define segdisplay P2
#define pulse_out P10
unsigned char dataa,flag=0,length=0,i,j,highlevel=0xff,freq_H,freq_L;
unsigned char seg[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
unsigned int frequency,duty_cycle,freq,dutyc,s,table[]=0,high_level_us,count;
double T_us,duty100;

//定时器0中断
void timer0() interrupt 1 
{
	++highlevel;
	if(highlevel==0)
	{
		  pulse_out=1;
	}
	if((highlevel>=1)&&(highlevel<(100/duty_cycle)))
	{
	    pulse_out=0;
	}
	if(highlevel==(100/duty_cycle))
	{
		  pulse_out=1;
		  highlevel=0x0;
	}
}

//串口一中断：与地址相匹配的数据来后进入中断
void UART1()interrupt 4 using 1
//void UART1() __interrupt (4) __using (1) //串口1中断服务函数//触发条件：地址数据，且地址相匹配（此时已将RI置1）
{
    ES = 0; //关闭串口1中断
    RI = 0; //不清零的话能否继续接收？？
	//-------返回响应
	  SBUF = SADDR+0x01 ;
	  while(!TI);
	  TI = 0;
	//--------
    SM2 = 0;//接收数据   
    rxINTtestled = 1; //表示已准备好接受数据
	//-------Receive the length
	  while(!RI);
	  RI = 0;
	  freq_H = SBUF;
	  SBUF = freq_H;
	  while(!TI);
	  TI = 0;
	  while(!RI);
	  RI = 0;
	  freq_L = SBUF;
	  SBUF = freq_L;
	  while(!TI);
	  TI = 0;
		while(!RI);
	  RI = 0;
	  duty_cycle = SBUF;
	  SBUF = duty_cycle;
	  while(!TI);
	  TI = 0;
	//Receive datas
	  while(!RI);
	  RI = 0;
		if( SBUF == 0XFF)
		{
				SM2 = 1;//断开连接
		}	
    ES = 1; //重新开启串口1中断
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
    TR1 = 1;        //启动定时器1
    EA = 1;         //中断 总开关
//----定时器0
	  TMOD &= 0XF8;  //定时器0模式配置：定时，16位自动重装方式
	  ET0 = 0;
	  
}

void main()
{
		P2M0=0X00;
		P2M1=0X00;
    UartInit();
    SADDR = 0X01;
    SADEN = 0X0F;
	  rxINTtestled = 0;
    while (1)
		{   
			//显示
			  if((frequency!=0)&&(duty_cycle!=0))
				{
						for(j=0;j<5;j++)
						{
							  freq = ((unsigned int)((unsigned int)((unsigned int)frequency)/pow(10,4-j))%10);
								segdisplay = seg[freq];
        				rxINTtestled = 0;
								Delay500ms();		  
						}
						Delay500ms();
						P2 = 0XFF;
						Delay500ms();
						for(j=0;j<3;i++)
						{
						    dutyc = ((unsigned int)((unsigned int)((unsigned int)frequency)/pow(10,2-j))%10);
							  segdisplay = seg[dutyc];
        				rxINTtestled = 0;
								Delay500ms();	
						}
				//脉冲
              //脉冲初始化           	
            T_us = (double)(1000000/frequency);
						duty100 = (double)((double)duty_cycle*0.01);
						high_level_us = (unsigned int)(T_us * duty100); 
						count = 65536-(unsigned int)(high_level_us*2);
						TH0 = count/256;
						TL0 = count%256;
						TR0 = 1;
				}
		} 
}