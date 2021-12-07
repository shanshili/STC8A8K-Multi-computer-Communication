#include "DELAY.H"
#include "INTRINS.h"

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;
//    _nop_();
//	_nop_();
	i = 1;		   
	j = 234;
	k = 113;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay500ms()		//@24MHz
{
	unsigned char i, j, k;
//	_nop_();
//	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
