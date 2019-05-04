#include <stc15w.h>
#include <intrins.h>
#include <stdio.h>
#include <string.h>
#include "STC_PWM.h"
//#include	"T_SineTable.h"
#include "AD.h" 




#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5
//单片机内部寄存器
#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1


sbit SP485_ON=P3^4;   //1: 发送    0:接收
sbit Drive_SD=P0^4;   //1: 关闭    0:使能


#define UART_BUF_LEN	32
#define UART_BUF_MASK	(UART_BUF_LEN - 1)
u8 Uart3_Rec[UART_BUF_LEN]={0};
u8 uartRcvLen = 0;
u8 uartReadLen = 0;




#define __debug   
/************************************
  延时子程序
  延时时间(xms*1)ms
 *************************************/
void delayms(u16 xms)
{
    u16 x,y;
    for(x=xms;x>0;x--)
        for(y=550;y>0;y--);
}
void Uart1Init(void)		//9600bps@30.000MHz
{
    SCON = 0x50;		//8位数据,可变波特率
    AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
    AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
    TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
    TL1 = 0xF3;		//设定定时初值
    TH1 = 0xFC;		//设定定时初值
    ET1 = 0;		//禁止定时器1中断
    TR1 = 1;		//启动定时器1
}

void Uart3Init(void)		//9600bps@30.000MHz
{
    S3CON = 0x10;		//8位数据,可变波特率
    S3CON |= 0x40;		//串口3选择定时器3为波特率发生器
    T4T3M |= 0x02;		//定时器3时钟为Fosc,即1T
    T3L = 0xF3;		//设定定时初值
    T3H = 0xFC;		//设定定时初值
    T4T3M |= 0x08;		//启动定时器3
    IE2 = 0x08;                 //使能串口3中断
    EA = 1;
}
void Timer0Init(void)		//10毫秒@30.000MHz
{
    AUXR &= 0x7F;		//定时器时钟12T模式
    TMOD &= 0xF0;		//设置定时器模式
    TL0 = 0x58;		//设置定时初值
    TH0 = 0x9E;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0=1;
}


void SendData(u8 dat)
{
    SBUF = dat;             //Send data to UART buffer
    while(!TI);
    TI=0;
}

/*----------------------------
  串口发送字符串
  ----------------------------*/
void SendString(char *s)
{
    while (*s)              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr
    }
} 
//void 
void IO_init()
{
    P3M0 = 0xff;
    P3M1 = 0x00; // 配置 为输chu模式
    P0M0 = 0xff;
    P0M1 = 0x01; // 配置 为输chu模式  P00 P05 shuru  
    P4M0 = 0xff;
    P4M1 = 0x00; // 配置 为输chu模式
    P1M0 = 0xff;
    P1M1 = 0x03; // 配置 为输chu模式

    P2M0 = 0x00;
    P2M1 = 0x00;
}	



#define PROTECT_TIME   10
u16 ptotectTimer = 200;
void CurrentProtectTask(void)
{
	if(!ptotectTimer)
	{
		ptotectTimer = PROTECT_TIME;
		if( GetADCResult(1)>710)
		{
            Drive_SD=1; //关闭IR2110芯片
		}
	}
}


u8 UartGetBuf(u8 *p)
{
	u8 len = 0;
	while(uartRcvLen != uartReadLen)
	{
		p[len] = Uart3_Rec[uartReadLen];
		len++;
		uartReadLen++;
		delayms(20);
	}
	return len;	
}
void UartCmdProcess(void)
{
	u8 buf[32];
  u8 temp[50];
	u16 freq;
	u16 volage;
	u8 len;
	len = UartGetBuf(buf);
	
	if(9 == len) //接收到一组数据
	{
		freq=buf[2]; //获取频率
		volage=buf[7]<<8|buf[8]; //获取频率
		volage=volage/10;
		if(volage>28)
			volage=28;
		if(volage<1)
			volage=1;
					
		sprintf((char*)temp,"plv:%bu hz\r\n",freq);
		SendString(temp);
		sprintf((char*)temp,"Vol:%d\r\n",volage);
		SendString(temp);
		STC_PWM_SetVolage(freq,volage);
		Drive_SD=1; //关闭IR2110芯片
	}
}
void main(void)					
{
    IO_init();
    Uart1Init();
    Uart3Init();
    Timer0Init();
    InitADC();        //Init ADC  
    SP485_ON=0;//使能485接收
    Drive_SD=1; //关闭IR2110芯片
    STC_PWM_Init(SPWM_FREQ_50HZ);
	STC_PWM_SetVolage(SPWM_FREQ_50HZ,20);
    while(1)						
    { 
		//UartCmdProcess();
    }		
}
//  d Timer0using using using using using using using 
void Timer0() interrupt 1 using 1  //10ms
{
	P35=~P35;
	//SendString("d");
	CurrentProtectTask();
	STC_PWM_Timer();
}

/*----------------------------
  UART3 中断服务程序
  -----------------------------*/
void Uart3() interrupt 17 using 1
{
    if (S3CON & S3RI)
    {
        S3CON &= ~S3RI;         //清除S3RI位
        Uart3_Rec[uartRcvLen] = S3BUF;             //P0显示串口数据 
				uartRcvLen ++;

    }
    if (S3CON & S3TI)  //发送中断检测
    {
        S3CON &= ~S3TI;         //清除S3TI位
    }   

}
