/************************先定义接口
P0----DB0~DB7
P2.7------RS
P2.6------RW
P2.5------E
*******************/
#define LCD_DB  P2
//sbit LCD_RS=P2^7;
//sbit LCD_RW=P2^6;
//sbit LCD_E=P2^5;///两种都可行
#define LCD_RS P37
#define LCD_RW P36
#define LCD_E P35
/************定义函数************/
#define uchar unsigned char
#define uint unsigned int
void LCD_init(void);
void LCD_w_command(uchar dat);
void LCD_w_data(uchar dat);
void LCD_dischar(uchar x,uchar y, uchar dat);
void LCD_disstr(uchar x, uchar y, uchar *str);
void rwjudge();
void delay(uint n);