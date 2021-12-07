#ifndef _KEYBOARD_H
#define _KEYBOARD_H
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
testled press down测试灯
*/

unsigned int Check_Keydown();
void Keyboardmain();
extern unsigned int keyvalue;
/*
如果你需要一个全局变量，在这个模块被其他地方引用的时候也能奏效的话，
那么你应该在xxx.c中定义，并在xxx.h声明即可。
无需在其他文件中声明和定义这个变量。
e.g. 
extern unsigned int keyvalue;   //in xxx.h
unsigned int keyvalue = 0;//in xxx.c
*/

#endif