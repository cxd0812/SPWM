#include <stc15w.h>
#include <intrins.h>
#include <stdio.h>
#include <string.h>
#include "STC_PWM.h"
//#include	"T_SineTable.h"
#include "AD.h" 




#define CCP_S0 0x10                 //P_SW1.4
#define CCP_S1 0x20                 //P_SW1.5
//��Ƭ���ڲ��Ĵ���
#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1


sbit SP485_ON=P3^4;   //1: ����    0:����
sbit Drive_SD=P0^4;   //1: �ر�    0:ʹ��


#define UART_BUF_LEN	32
#define UART_BUF_MASK	(UART_BUF_LEN - 1)
u8 Uart3_Rec[UART_BUF_LEN]={0};
u8 uartRcvLen = 0;
u8 uartReadLen = 0;




#define __debug   
/************************************
  ��ʱ�ӳ���
  ��ʱʱ��(xms*1)ms
 *************************************/
void delayms(u16 xms)
{
    u16 x,y;
    for(x=xms;x>0;x--)
        for(y=550;y>0;y--);
}
void Uart1Init(void)		//9600bps@30.000MHz
{
    SCON = 0x50;		//8λ����,�ɱ䲨����
    AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
    AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
    TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
    TL1 = 0xF3;		//�趨��ʱ��ֵ
    TH1 = 0xFC;		//�趨��ʱ��ֵ
    ET1 = 0;		//��ֹ��ʱ��1�ж�
    TR1 = 1;		//������ʱ��1
}

void Uart3Init(void)		//9600bps@30.000MHz
{
    S3CON = 0x10;		//8λ����,�ɱ䲨����
    S3CON |= 0x40;		//����3ѡ��ʱ��3Ϊ�����ʷ�����
    T4T3M |= 0x02;		//��ʱ��3ʱ��ΪFosc,��1T
    T3L = 0xF3;		//�趨��ʱ��ֵ
    T3H = 0xFC;		//�趨��ʱ��ֵ
    T4T3M |= 0x08;		//������ʱ��3
    IE2 = 0x08;                 //ʹ�ܴ���3�ж�
    EA = 1;
}
void Timer0Init(void)		//10����@30.000MHz
{
    AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0;		//���ö�ʱ��ģʽ
    TL0 = 0x58;		//���ö�ʱ��ֵ
    TH0 = 0x9E;		//���ö�ʱ��ֵ
    TF0 = 0;		//���TF0��־
    TR0 = 1;		//��ʱ��0��ʼ��ʱ
    ET0=1;
}


void SendData(u8 dat)
{
    SBUF = dat;             //Send data to UART buffer
    while(!TI);
    TI=0;
}

/*----------------------------
  ���ڷ����ַ���
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
    P3M1 = 0x00; // ���� Ϊ��chuģʽ
    P0M0 = 0xff;
    P0M1 = 0x01; // ���� Ϊ��chuģʽ  P00 P05 shuru  
    P4M0 = 0xff;
    P4M1 = 0x00; // ���� Ϊ��chuģʽ
    P1M0 = 0xff;
    P1M1 = 0x03; // ���� Ϊ��chuģʽ

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
            Drive_SD=1; //�ر�IR2110оƬ
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
	
	if(9 == len) //���յ�һ������
	{
		freq=buf[2]; //��ȡƵ��
		volage=buf[7]<<8|buf[8]; //��ȡƵ��
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
		Drive_SD=1; //�ر�IR2110оƬ
	}
}
void main(void)					
{
    IO_init();
    Uart1Init();
    Uart3Init();
    Timer0Init();
    InitADC();        //Init ADC  
    SP485_ON=0;//ʹ��485����
    Drive_SD=1; //�ر�IR2110оƬ
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
  UART3 �жϷ������
  -----------------------------*/
void Uart3() interrupt 17 using 1
{
    if (S3CON & S3RI)
    {
        S3CON &= ~S3RI;         //���S3RIλ
        Uart3_Rec[uartRcvLen] = S3BUF;             //P0��ʾ�������� 
				uartRcvLen ++;

    }
    if (S3CON & S3TI)  //�����жϼ��
    {
        S3CON &= ~S3TI;         //���S3TIλ
    }   

}
