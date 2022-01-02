/*
  【DUTY BOX】：
	slave1 地址 0x01
	从机1完成脉冲波形发生，频率、占空比可以设置；
	P2显示数据
	P00为指示灯
	中断x调用串口接收数据
	显示：
		输入频率
		输入占空比
	【DEBUG NOTE】:
	1、从机串口通信接收存在漏洞
	【UPDATE LOG】：
	*21/12/9* 更新为main2.c:收发方式修正,配合从机main4版本使用
*/

#include "8A8K.h"
#include "delay.h"
#include "FrequencyDutycycle.H"
#define rxINTtestled P00
unsigned char dataa,flag=0,addr=0x01;
unsigned char seg[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
unsigned char InpuDatSto[12]=0,InpDatNum=0;

//输入数据存储
void InputDataStore(unsigned char indat)
{
	  InpuDatSto[InpDatNum]=indat;
		InpDatNum++;
}

//接收测试数码管显示
void datatest()
{
    dataa = SBUF;
		P2 = seg[dataa];
	  InputDataStore(dataa);
	  Delay500ms();
	  Delay500ms();
	  Delay500ms(); 
}

//串口一中断：接收完毕即进入中断
void UART1() interrupt 4 using 1
{
    ES = 0; //关闭串口1中断
    if( RB8 == 1 )//为地址帧，判断，返回响应
		{
			  if( SBUF == 0X01 )
				{													
						SBUF = addr+0x01 ;
						while(!TI);
						TI = 0;
					  SM2 = 0;//开始接收数据 
						rxINTtestled = 1; //表示已准备好接受数据
				}
		}
	  if( RB8 == 0 )//为数据帧
		{
				while(!RI); //确认接收完数据
				rxINTtestled = 0;
				datatest();
		}
		RI = 0; //清除接收标志位
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
		TR1 = 1;		//启动定时器1
}

void main()
{
		P2M0=0X00;
		P2M1=0X00;
    UartInit();
//    SADDR = 0X01;
//    SADEN = 0X0F;
	  rxINTtestled = 0;
    while (1); 
}