/*------------------------------------------------------------------*/
/* --- STC MCU Limited ---------------------------------------------*/
/* --- STC12C5Axx Series MCU A/D Conversion Demo -------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966----------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

 
#include <stc15w.h>
#include "STC_PWM.h"
#include "AD.h" 
#include <intrins.h>
 

/*Declare SFR associated with the ADC */
//sfr ADC_CONTR   =   0xBC;           //ADC control register
//sfr ADC_RES     =   0xBD;           //ADC high 8-bit result register
//sfr ADC_LOW2    =   0xBE;           //ADC low 2-bit result register
//sfr P1ASF       =   0x9D;           //P1 secondary function control register

/*Define ADC operation const for ADC_CONTR*/
#define ADC_POWER   0x80            //ADC power control bit
#define ADC_FLAG    0x10            //ADC complete flag
#define ADC_START   0x08            //ADC start control bit
#define ADC_SPEEDLL 0x00            //420 clocks
#define ADC_SPEEDL  0x20            //280 clocks
#define ADC_SPEEDH  0x40            //140 clocks
#define ADC_SPEEDHH 0x60            //70 clocks

extern unsigned int  Vol;  //输出电压  默认输出最大值
 

/*----------------------------
Get ADC result
----------------------------*/
u16 GetADCResult(u8 ch)
{
	u16 ADC_Result;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_();                        //Must wait before inquiry
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC
    ADC_Result=ADC_RES;
	  ADC_Result=ADC_Result<<2|ADC_RESL;
    return ADC_Result;                 //Return ADC result
}  

 

/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC()
{
    P1ASF = 0x03;                   //Open 8 channels ADC function  0,1通道
    ADC_RES = 0;                    //Clear previous result
 
	   ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | 0x01;
    Delay(2);  
  
}

 

/*----------------------------
Software delay function
----------------------------*/
void Delay(u16 n)
{
    u16 x;

    while (n--)
    {
        x = 5000;
        while (x--);
    }
}

