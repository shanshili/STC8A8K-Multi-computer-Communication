/*
  【INFORMATION】：
	mainframe 
	串口1：9位数据,可变波特率
	定时器1:16位自动重载 
	串口1选择定时器1为波特率发生器
	定时器0：KEY 4 中断
	定时器2：KEY 8 中断
	UART中断：判断从机选中响应，提示发送数据
	波特率：9600
	RxD P3.0
	TxD P3.1
	------------------------------
	TB8=0 send datas
	TB8=1 send address
	【DUTY BOX】:
	通过主机设置从机地址，设置从机的频率、占空比、流水灯花样。
	主机通过键盘设置，LCD12864显示。
  KEY 1 SLAVE1  
				first input frequency renge  99999HZ~30.5HZ(31HZ)    5bit
				second input duty cycle  renge 100~0 3bit
	KEY 2 SLAVE2
	KEY 15 COMFIRM
	ans1 frequency
	ans2 dutycycle
	【NOTE】:
	开发板限制，无法使用LCD显示
	*21/12/9* 使用定时器中断触发外部中断；使用串口中断触发数据发送
	          中断触发：TI/RI，即发送完、接收完都可进入中断程序
					  更新为main2.c
	*21/12/11* 中断内不宜调用函数的问题,将串口发送函数放置到主函数，但是这样的话中断触发困难，
						更新为main3.c
	*21/12/12* 既然使用循环查询的方法，事实上也没必要用中断了，删除中断，重新写协议，
						更新为main4.c
						更新main4_2.c,采用模式一、二，和数据校验字
	*21/12/13*更新为main5.c,将占空比、频率合为2个/1个数据发送，流水灯只允许发送一个数据，修改数据传输函数
	          将frequency分为高低位发送，因为一次只能发八位
						整理参数类型及命名
	【IDEA】
	终止函数放到下一次选地址之前
*/
#include "8A8K.h"
#include "KEYBOARD.H"
#include "DELAY.H"
#include <INTRINS.H>
#include <string.h>
#include <math.h>
#define TXendtestLED P02
#define TXendtestLEDADDR P03
#define TXendtestLEDDATA P04
#define mode P05
#define uchar unsigned char //1byte
#define uin32 unsigned int  //4byte
#define segdisplay P2
unsigned int datalength,i,TXans;
unsigned char seg[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90},table[10] = {0},j,slaveaddr,transans,test;
unsigned int keyvalue = 16;
float frequency=0;
double Duty_Cycle=0;

//End of communication function
void End_Connect()
{
		SBUF = 0xff;
		while(!TI);
		TI = 0;
	  while(SBUF != 0XFF);
	  RI = 0;
}

//地址传输函数
void TXaddr(uchar TXaddrdata)
{
    TB8 = 1;
	  TB8 = 1;
	  SBUF = TXaddrdata;
	  while(!TI);
	  TI = 0;
	  while(SBUF != (TXaddrdata+0x01)); //接收到从机响应
	  RI = 0;
		Delay10ms();
		TXendtestLED = 1;//从机已接受
		Delay500ms();
    TB8 = 0;
	  TB8 = 0;
}

//数据传输函数
void TXdata()
{   
	  unsigned char TXfreq_H,TXfreq_L;
	  if( slaveaddr == 1 )
		{    
			  TXfreq_H = frequency/256;
			  TXfreq_L = (unsigned char)Duty_Cycle%256;
				SBUF = TXfreq_H;
				while(!TI);
				TI = 0;
			  while(!RI);
				while(SBUF != TXfreq_H);
				RI = 0;
			  SBUF = TXfreq_L;
				while(!TI);
				TI = 0;
			  while(!RI);
				while(SBUF != TXfreq_L);
				RI = 0;
				SBUF = Duty_Cycle;
				while(!TI);
				TI = 0;
				while(!RI);
				while(SBUF != Duty_Cycle);
				RI = 0;
		}
		if( slaveaddr == 2 )
		{
				SBUF = transans;
				while(!TI);
				TI = 0;
				while(SBUF != transans);
				RI = 0;
		}
    TXendtestLED = 1;  //已发送完
		End_Connect();
}

//数据处理函数
void datatrans(uchar addr,uchar *table,unsigned int tablelength)
{
	  Duty_Cycle = 0;
	  frequency = 0;
		if ( addr == 2 )
		{
				transans = table[0];
		}
		if ( addr == 1 )
		{   
/*				for(i=(tablelength-1);i>=(tablelength-3);i--)
				{
				    Duty_Cycle = Duty_Cycle+table[i]* pow(10,(tablelength-i-1));
				}	
				for(i=(tablelength-4);i>=0;i--)
				{
						frequency = frequency + table[i]*pow(10,(tablelength-4-i));
				}
				ans = (unsigned int)((unsigned int)(frequency*100) +  Duty_Cycle);
*/
				for(i=5;i>2;i--)
				{
				    Duty_Cycle = Duty_Cycle+((double)table[i]*(pow(10,((double)tablelength-i-1))));
				}	
				
				//TEST
        for(j=0;j<3;j++)
				{
//				    test = ((unsigned int)((unsigned int)frequency/pow(10,5-j))%10);
					   test = (unsigned char)(Duty_Cycle/pow(10,(double)(2-j)))%10; 	[[[[[[[[烧录
						segdisplay = seg[test];
//					  segdisplay = seg[table[j]];
						Delay500ms();		
						Delay500ms();						
				}
				for(i=2;i>=0;i--)
				{
						frequency = (float)(frequency + (double)table[i]*pow(10,((double)tablelength-4-i)));
				}
		}
}

//模式函数
void Mode(uchar salvenum) //按下多个数字，按确认跳出循环
/*
mode Input frequency and duty cycle respectively
mode2 Input the flow light number
*/
{
		mode = 1;
	  slaveaddr = salvenum;
		TXaddr(salvenum);	  
		TXendtestLEDADDR = 1;
		datalength = 0;
		while(1)
		{
				Keyboardmain();//内有限循环等待按下
				if( keyvalue == 15 )
				{break; }
				table[datalength]=keyvalue;
				datalength++;					   
		}
		  datatrans(salvenum,table,datalength);
		  TXdata();
		mode = 0;			  
}

//串口、定时器初始化
void Uart_Timer_Init(void) //9600bps@24.000MHz
{ 
		P3M1 = 0X00;
		P3M0 = 0X03;
    P_SW1 = 0X00; //端口映射 3.0 3.1
    SCON = 0xD0;  //串口控制：串口1，9位数据,可变波特率  1101 0000 允许串口接收数据，处于地址帧筛选被禁止状态，接收到的信息直接进入SBUF，使RI=1
    PCON &= 0x7F; //电源管理：波特率不倍速 PCON=0X00 SMOD=0
    AUXR &= 0xFE; //选择波特率发生器：定时器1
    AUXR |= 0x40; //定时器1时钟：Fosc,即1T T1x12=1
    TMOD &= 0x0F; //定时器1模式配置：定时，16位自动重装方式
    TL1 = 0x8F;   //设定定时低字节:
    TH1 = 0xFD;   //设定定时高字节:  64911 65536-(SYSCLK/4/baud)
    ET1 = 0;      //定时器1中断：禁止
    TR1 = 1;      //启动定时器1
	  RI = 0;
}

void main()
{
    Uart_Timer_Init();
    while (1)
    {
				TXendtestLED = 0;
        TXendtestLEDADDR = 0;
        TXendtestLEDDATA = 0;
				mode = 0;
        Keyboardmain(); //mode selection
			  segdisplay = seg[keyvalue];
			  Delay10ms();
			  if( keyvalue == 1 )//Enter mode 1 and connect slave machine 1
				{ Mode(1);  }
				if( keyvalue == 2 )//Enter mode 2 and connect slave machine 2
				{ Mode(2);  }
		}        
}