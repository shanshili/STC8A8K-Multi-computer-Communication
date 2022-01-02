#include "8A8Ksdcc.h"
#include <string.h>

#define addr 0x01   //从机1的地址
#define _SUCC_ 0x0f //数据传送成功
#define _ERR_ 0xf0  //数据传送失败
unsigned char aa=0xff;//主机与从机之间通信标志
unsigned char Buff[20];//数据缓冲区

//串口初始化函数
void UartInit(void)		//9600bps@24.000MHz
{
	PCON &= 0x7F;		//波特率不倍速  SMOD=0
	SCON = 0xD0;		//9位数据,可变波特率  1101 0000 允许串口接收数据，处于地址帧筛选被禁止状态，接收到的信息直接进入SBUF，使RI=1
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位16位自动重装方式
	TL1 = 0x8F;		//设定定时初值
	TH1 = 0xFD;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
    TI=0;
    RI=0;
}
//接收数据函数
unsigned char RECE_data(unsigned char *Buff)
{
    unsigned char i,temp;
    unsigned char lenth;
    unsigned char check;

    RI=0; //接收数据长度
    while(!RI);
    if(RB8==1) //若接收到地址帧，则返回0xfe
    return 0xfe;
    lenth=SBUF;
    RI=0;

    check=lenth;
    for(i=0;i<lenth;i++) //接收数据
    {
        while(!RI);
        if(RB8==1) //若接收到地址帧，则返回0xfe
        return 0xfe;
        Buff[i]=SBUF;
        check=check^(Buff[i]);
        RI=0;
    }

    while(!RI); //接收校验字节
    if(RB8==1) //若接收到地址帧，则返回0xfe
    return 0xfe;
    temp=SBUF;
    RI=0;

    check=temp^check; //将从主机接收到的校验码与自己计算的校验码比对
    if(check!=0) //校验码不一致，表明数据接收错误，向主机发送错误信号，函数返回0xff
    {
        TI=0;
        TB8=0;
        SBUF=_ERR_;
        while(!TI);
        TI=0;
        return 0xff;
    }
    TI=0; //校验码一致，表明数据接收正确，向主机发送成功信号，函数返回0x00
    TB8=0;
    SBUF=_SUCC_;
    while(!TI);
    TI=0;
    return 0;
}


void main()
{
    UartInit();
    while(1)
    {
        SM2=1; //接收地址帧
        while(aa!=addr) //从机等待主机请求自己的地址
        {
            RI=0;
            while(!RI);
            aa=SBUF;
            RI=0;
        }

        TI=0; //一旦被请求，从机返回自己的地址作为应答，等待接收数据
        TB8=0;
        SBUF=addr;
        while(!TI);
        TI=0;

        SM2=0; //接收数据帧
        aa=0xff; //从机接收数据，并将数据保存到数据缓冲区
        while(aa==0xff)
        {
            aa=RECE_data(Buff);
        }
        if(aa==0xfe)
        continue;
        P1=Buff[1]; //查看接收到的数据
    }
}