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
	KEY 1 slave1
	KEY 2 slave2
	KEY 4 input address 
	KEY 8 input datas  （每次发送数据都需先按下8）
	【NOTE】:
	开发板限制，无法使用LCD显示
	*21/12/9* 使用定时器中断触发外部中断；使用串口中断触发数据发送
	          中断触发：TI/RI，即发送完、接收完都可进入中断程序
					  更新为main2.c
	*21/12/11* 中断内不宜调用函数的问题,将串口发送函数放置到主函数，但是这样的话中断触发困难，
						更新为main3.c
	*21/12/12* 既然使用循环查询的方法，事实上也没必要用中断了，删除中断，重新写协议，
						更新为main4.c
						
	【problem】
	1、每次发送数都需要点击发送数据
	2、选中某从机后无法关闭
	
	采用模式一模式二？
	模式一，先输入对应的数据，存储进去
	然后一连贯发送，默认模式一是从机1
	模式二是流水灯
*/
#include "8A8K.h"
#include "KEYBOARD.H"
#include "DELAY.H"
#include <INTRINS.H>
#define TXendtestLED P02
#define TXendtestLEDADDR P03
#define TXendtestLEDDATA P04
#define timeLED0 P05
#define timeLED2 P06
#define uchar unsigned char //1byte
#define uin32 unsigned int  //4byte
unsigned int keyvalue = 0;
uin32 addr;
uchar str[] = {0};
unsigned char seg[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

//地址传输函数
void TXaddr(uchar addr)
{
	  TXendtestLED = 1;
		TB8 = 1;
	  TB8 = 1;
	  SBUF = addr;
	  while(!TI);
	  TI = 0;
//		Delay500ms();//检测是否发送完【已修正，此延时会影响接收】
	  while(SBUF != (addr+0x01)); //接收到从机响应
		Delay10ms();
		TXendtestLED = 0;//从机已接受
		Delay500ms();
    TB8 = 0;
	  TB8 = 0;
}

//数据传输函数
void TXdata(uchar *str)
{
    while (*str != '\0')
    {
        SBUF = (*str);
        while (!TI);
        TI = 0;
        str++;
    }
    TXendtestLED = 1;  //已发送完
    Delay500ms();
}
//定时器2中断：输入数据
void timer2() interrupt 12 
{
		AUXR &= 0Xef;//关闭定时器

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
				timeLED0 = 0;
				timeLED2 = 0;
        Keyboardmain(); //按下按键4、8打开定时器，即进入中断
			  P2 = seg[keyvalue];
			  Delay500ms();
			  if( keyvalue == 4 )
				{
						timeLED0 = 1;
					  Keyboardmain();//内有无限循环等待按下
						addr = keyvalue;
						P2=seg[addr];
						TXaddr(addr);	  
						TXendtestLEDADDR = 1;
						timeLED0 = 0;
				}
		    if( keyvalue == 8 )	
				{	  
						timeLED2 = 1;	
    				Keyboardmain();
						str[0] = keyvalue;
						P2=seg[*str];
						TXdata(str);
						TXendtestLEDDATA = 1;
						timeLED2 = 0;	
				}        
    }
}