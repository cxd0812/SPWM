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

 
void InitADC();
 

/*----------------------------
Get ADC result
----------------------------*/
u16 GetADCResult(u8 ch);
 

/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC();
 

/*----------------------------
Software delay function
----------------------------*/
void Delay(u16 n);
