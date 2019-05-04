#ifndef STC_PWM_H_ 
#define STC_PWM_H_


typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;


#define SPWM_FREQ_50HZ		0x50
#define SPWM_FREQ_60HZ		0x60

void STC_PWM_Init(u16 freq);
void STC_PWM_SetVolage(u16 freq,u16 vol);
void Updata_Plv(u8 plv);
u16 Get_voltage(void);
void STC_PWM_Timer(void);



#endif