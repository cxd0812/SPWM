#include <stc15w.h>
#include "STC_PWM.h"
#include "AD.h"
#include <math.h>


#define STC_PWM_FREQ	150000
#define STC_PWM_VUPTIME	2
#define  PWM_DeadZone  5   /*死区时钟数,6~24之间*/ 



sfr PIN_SW2 =   0xba;
sbit Drive_SD=P0^4;   //1: 关闭    0:使能

static u16 stcPwmTime = 0;
static u16 stcPwm_Curv = 0;
static u16 stmPwm_Tarv = 0;
static u16 stmPwm_Freq = 0;

u8 Value_50=151;  //50hz?? 
u8 Value_60=126;  //60hz?? 

void SendData(u8 dat);


u16   T_SinTable[ ]={
1200,  1238,  1275,  1313, 1350,  1388, 1425,  1462,  1498,  1535,  1571, 
 1606, 1641, 1676,  1711,  1744,  1778,  1810,  1843,  1874,  1905,  1935, 
 1964, 1993,  2021,  2048,  2074,  2100,  2124,  2148,  2170, 
 2192, 2213,  2232,  2251,  2269,  2285,  2301,  2315,  2329, 
 2341, 2352,  2362,  2371,  2379,  2385,  2390,  2395,  2398, 
 2399, 2400,  2399,  2398,  2395,  2391,  2385,  2379,  2371, 
 2363, 2353,  2342,  2330,  2316,  2302,  2287,  2270,  2252, 
 2234, 2214,  2194,  2172,  2149,  2126,  2102,  2076,  2050, 
 2023, 1995,  1967,  1937,  1907,  1876,  1845,  1813,  1780, 
 1747, 1713,  1679,  1644,  1609,  1573,  1537,  1501,  1464, 
 1428, 1390,  1353,  1316,  1278,  1241,  1203,  1165,  1128, 
 1090, 1053,  1015,  978,  941,  905,  868,  832,  797, 
 761, 726,  692,  658,  625,  592,  560,  528,  497, 
 467, 438,  409,  381,  354,  328,  302,  278,  254, 
 231, 210,  189,  169,  150,  133,  116,  100,  86, 
 72, 60,  49,  39,  30,  22,  15,  10,  6, 
 3, 1,  0,  0,  2,  5,  9,  14,  20, 
 28, 37,  46,  57,  69,  83,  97,  112,  129, 
 146, 165,  184,  205,  226,  249,  272,  297,  322, 
 348, 375,  403,  431,  460,  490,  521,  553,  585, 
 617, 650,  684,  718,  753,  788,  824,  860,  896, 
 933, 970,  1007,  1044,  1081,  1119,  1157,  
};

//先进先出滤波算法 
#define FILTER_N 12
int filter_buf[FILTER_N + 1];
int Filter() 
{
    int i;
    int filter_sum = 0;
    filter_buf[FILTER_N] = (int)(GetADCResult(0));
    for(i = 0; i < FILTER_N; i++) 
	{
        filter_buf[i] = filter_buf[i + 1]; // 所有数据左移，低位仍掉
        filter_sum += filter_buf[i];
    }
    return (int)(filter_sum / FILTER_N);
}
static u16 stmPwm_InVol = 0;
void STC_PWM_Init(u16 freq)
{
	u8 i;
	u16 temp;
	if(SPWM_FREQ_50HZ == freq)
	{
		temp = 3000;
	}
	else if(SPWM_FREQ_60HZ == freq)
	{
		temp = 2500;
	}
	else
	{
		return ;
	}

	
	for(i=0;i<100;i++)
	{
		stmPwm_InVol = Get_voltage();
	}
	if(stmPwm_InVol < 40)
	{
		Value_50=3000/stmPwm_InVol*1.414;
		Value_60=2500/stmPwm_InVol*1.414;
	}
	
	

	
	PIN_SW2 |= 0x80;                //浣胯兘璁块棶XSFR
	PWMCFG = 0x00;                  //閰嶇疆PWM鐨勮緭鍑哄垵濮嬬數骞充负浣庣數骞�

	PWMCKS = 0x00;                  //閫夋嫨PWM鐨勬椂閽熶负Fosc/1
	PWMC = temp;
	
	/*PWM3 閰嶇疆*/	
	PWM3T1 = 0;                //璁剧疆PWM3绗�1娆″弽杞殑PWM璁℃暟
	PWM3T2 = 100;                //璁剧疆PWM3绗�2娆″弽杞殑PWM璁℃暟
	PWM3CR = 0x00;                  //閫夋嫨PWM3杈撳嚭鍒癙2.1,涓嶄娇鑳絇WM3涓柇
	PWMCR |= 0x02;                   //浣胯兘PWM淇″彿杈撳嚭
	PWMCFG &= ~0x02;                 //閰嶇疆PWM鐨勮緭鍑哄垵濮嬬數骞充负浣庣數骞�
	P21=0;



    PWM4T1 = 0;                //璁剧疆PWM4绗�1娆″弽杞殑PWM璁℃暟
    PWM4T2 = 100+PWM_DeadZone;                //璁剧疆PWM4绗�2娆″弽杞殑PWM璁℃暟
    PWM4CR = 0x00;                  //閫夋嫨PWM4杈撳嚭鍒癙2.2,涓嶄娇鑳絇WM4涓柇
    PWMCR |= 0x04;                  //浣胯兘PWM淇″彿杈撳嚭	/*PWM4 閰嶇疆*/
    PWMCFG|= 0x04;                 //閰嶇疆PWM鐨勮緭鍑哄垵濮嬬數骞充负楂樼數骞�
    P22=1;



    PWM6T1 = 0;
    PWM6T2 = 100;

    PWM6CR = 0x00;
    PWMCR |= 0x10;
    PWMCFG &=~0x10;
    P16=0;				



    PWM7T1 = 0;
    PWM7T2 = 100+PWM_DeadZone;

    PWM7CR = 0x00;
    PWMCR |= 0x20;
    PWMCFG|= 0x20;
    P17=1;

    PWMCR |= 0x40;
    PWMCR |= 0x80;

    //		PWMFDCR |=0x28;  //浣胯兘pwm澶栭儴寮傚父妫�娴�
    //		PWMFDCR |=0x08;  //浣胯兘pwm澶栭儴寮傚父妫�娴嬩腑鏂�
    //		PWMFDCR |=0x02;  //浣胯兘pwm澶栭儴寮傚父妫�娴�  璁剧疆妫�娴嬪彛涓篜24  楂樼數骞宠Е鍙�

    PIN_SW2 &= ~0x80;

    EA = 1;
}



//鑾峰彇涓嶅悓鐐规暟鐨勬寮︽尝鏁版嵁
//point: 涓�鍛ㄦ湡鍐呯殑鍙栨牱鐐规暟
//maxnum: 涓�鍛ㄦ湡鍐呭搴擠A杈撳嚭鏈�澶у��
//freq 棰戠巼 
//v 閲囬泦鍒扮殑鐢靛帇鍊�

void GetSinTab(u16 point,u16 maxnum,u16 freq)
{
	u16 i=0; 
	float x,y;   //??
	float jiao;//?? ???
	
	jiao=360.000/point; 
	
	if(freq==SPWM_FREQ_50HZ)
	 {
		maxnum=maxnum*Value_50+1;//1.414*107.14+1;
		if(maxnum>3000)
			maxnum=3000;
		for(i=0;i<point;i++)
		{
			y=jiao*i;    //?????
			x=y*0.01744; //?????  ??=??*(p/180)

			T_SinTable[i]=1500+(maxnum/2-6)*sin(x)+0.5;//+0.5 ?????????????
			SendData(T_SinTable[i]);
		}
	}
	else if(freq==SPWM_FREQ_60HZ)
	{
		maxnum=maxnum*Value_60+1;//89.28*1.414+1;
		if(maxnum>2500)
			maxnum=2500;
		for(i=0;i<point;i++)
		{
			y=jiao*i;    //?????
			x=y*0.01744; //?????  ??=??*(p/180)
			T_SinTable[i]=1250+(maxnum/2-6)*sin(x)+0.5;//+0.5 ?????????????
		}		
	}
}	

u16 Get_voltage(void)
{
    u16  ad_temp=0;
    ad_temp=Filter();
    ad_temp=ad_temp*54;
    return (ad_temp/1000);
}

//void Updata_Plv(u8 plv)
//{
//    PIN_SW2 |= 0x80;                //使能访问XSFR
//    if(plv==0x50)
//        PWMC = 3000;                   //设置PWM周期
//    else if(plv==0x60)
//        PWMC = 2500;                   //设置PWM周期	
//    PIN_SW2 &= ~0x80;
//}
void STC_PWM_Task(u16 *v_current,u16 v_target,u16 freq)
{
	if((*v_current)!=v_target)
	{
		if((*v_current)>v_target)
		{
			(*v_current)--;
		}
		else
		{
			(*v_current)++;
		}
		GetSinTab(200,*v_current,freq);
		Drive_SD=0;
	}
}

void STC_PWM_SetVolage(u16 freq,u16 vol)
{
	u16 temp;
	if(SPWM_FREQ_50HZ == freq)
	{
		temp = 3000;
	}
	else if(SPWM_FREQ_60HZ == freq)
	{
		temp = 2500;
	}
	else
	{
		return ;
	}
	PIN_SW2 |= 0x80;                //使能访问XSFR
	
	PWMC = temp;
	PIN_SW2 &= ~0x80;
	stmPwm_Tarv=vol;
	stmPwm_Freq = freq;
}

void STC_PWM_Timer(void)
{
	if(0 == stcPwmTime)
	{
		stcPwmTime = STC_PWM_VUPTIME;
		/*SendData(0x55);
		SendData(stcPwm_Curv);
		SendData(stcPwm_Curv>>8);
		SendData(stcPwm_Curv>>16);
		SendData(stcPwm_Curv>>24);*/
		STC_PWM_Task(&stcPwm_Curv,stmPwm_Tarv,stmPwm_Freq);
	}
	stcPwmTime--;
}
static u16 stcPwm_P = 0;
void pwm_isr() interrupt 22 using 1
{
    u16 j=0;
    if (PWMIF & CBIF)
    {
		
        PWMIF &= ~CBIF; //娓呴櫎鏍囧織
        PIN_SW2 |= 0x80;  //浣胯兘璁块棶XSFR
		
        j=T_SinTable[stcPwm_P];
//		SendData(j);
        PWM3T2=j;
        PWM6T2=j;
        j+=PWM_DeadZone;
        PWM4T2=j;
        PWM7T2=j;

        PIN_SW2 &= ~0x80;
        if(stcPwm_P++>200)
		{
            stcPwm_P=0;
		}
    }
}
void pwmError_isr() interrupt 23 using 1  //鐭矾淇濇姢
{

    PWMFDCR &= ~FDIF; //娓呴櫎鏍囧織
    // Drive_SD=1; //鍏抽棴IR2110鑺墖
}
