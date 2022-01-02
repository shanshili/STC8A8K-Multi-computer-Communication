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
	显示发送的数据（转换为10进制）
	按键输入要发送的数据
	通过主机设置从机地址，设置从机的频率、占空比、流水灯花样。
	主机通过键盘设置，LCD12864显示。
	【NOTE】:
	开发板限制，无法使用LCD显示
*/
#include "8A8K.h"
#include "KEYBOARD.H"
#include "DELAY.H"
#include <INTRINS.H>
//#include "lcd1602.h"
#define TXendtestLED P00
#define TXendtestLEDADDR P02
#define TXendtestLEDDATA P03
#define uchar unsigned char //1byte
#define uin32 unsigned int  //4byte
sbit rs485re = P3^7;
sbit rs485DE = P3^5;
unsigned int keyvalue = 0;

	
unsigned char seg[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void UartInit(void) //9600bps@24.000MHz
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
}
/*
void SEND_data(uchar *Buff)//发送数据函数
{
    uchar i;
    uchar lenth;
    uchar check;
    lenth=strlen(Buff); //计算数据长度
    check=lenth;

    TI=0; 
    TB8=0; //发送数据帧
    SBUF=lenth;//发送数据长度
    while(!TI);
    TI=0;

    for(i=0;i<lenth;i++) //发送数据
    {
        check=check^Buff[i];//^异或运算符
        TB8=0;
        SBUF=Buff[i];
        while(!TI);
        TI=0;
    }

    TB8=0; //发送校验字节?
    SBUF=check;
    while(!TI);
    TI=0;
}

void ADDR_data(unsigned addr)//向指定从机地址发送数据
{
    while(temp!=addr) //主机等待从机返回其地址作为应答信号
    {
        TI=0; //发送从机地址
        TB8=1; //反复发送地址帧
        SBUF=addr;
        while(!TI);
        TI=0;
        RI=0;//反复接收，并比较是否为地址
        while(!RI);
        temp=SBUF;
        RI=0;
    }
    temp=_ERR_; //?
    while(temp!=_SUCC_)//主机等待从机数据接收成功信号
    {
        SEND_data(Buff);
        RI=0;
        while(!RI);
        temp=SBUF;
        RI=0;
    }
}
*/

void TXdata(uchar addr, uchar *str)
{
    TB8 = 1;
	  TB8 = 1;
    SBUF = addr;
//    LCD_dischar(6, 0, addr);
    while (!TI); //finish sending put 1 //问题：不确定对方是否接受到
    TI = 0;
		TXendtestLED = 1; //已发送
		while(SBUF != (addr+0x01)); //接收到从机响应
		Delay500ms();
		TXendtestLED = 0;//从机已接受
		Delay500ms();
    TB8 = 0;
	  TB8 = 0;
    while (*str != '\0')
    {
        SBUF = (*str);
//      LCD_disstr(7, 0, str);
        while (!TI);
        TI = 0;
        str++;
    }
    TXendtestLED = 1;  //已发送完
//  LCD_disstr(0, 1, "finished");
    Delay500ms();
}
void main()
{
    uin32 addr;
    uchar str[] = {0};
		/*
    uin32 keyvalue = 0; //可在其他模块调用的全局变量的初始化//debug:该变量为多处调用的全局变量，注意不要再次声明和复制
		P2M1 = 0X00;
		P2M0 = 0X00; //因为要读取状态字 ，所以只能是弱上拉：准双向				
		P3M1 = 0X00;
		P3M0 = 0Xe0;
    LCD_init();
    LCD_disstr(0, 1,"opening");
		*/

    Delay500ms();
    UartInit();
		rs485re = 0;
		rs485DE = 1;
    while (1)
    {
        TXendtestLED = 0;
        TXendtestLEDADDR = 0;
        TXendtestLEDDATA = 0;
//      LCD_disstr(0, 1, "address");
        Keyboardmain(); //写成中断，随时可输入地址和数据
        addr = keyvalue;
		  	P2=seg[addr];
        TXendtestLEDADDR = 1;
//      sLCD_disstr(0, 1, "data");
        Keyboardmain();
        str[0] = keyvalue;
			  P2=seg[*str];
        TXendtestLEDDATA = 1;
        Delay500ms();
        TXdata(addr, str); //直接调用函数名//查询法
    }
}