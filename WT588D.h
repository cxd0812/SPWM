#ifndef __WT588D_H 
#define __WT588D_H

#define uchar unsigned char  //?????? ???	????0~255
#define uint  unsigned int	 //????? ???	????0~65535
#include <intrins.h>


sbit  VRST = P4^7; 
sbit  VBUSY= P1^1;	
sbit  VSDA = P1^3; 
sbit  VCS  = P1^2;  
sbit  VSCL = P1^5;

void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<955;j++);
}
				   


/***********************?????**************************/
void  delay_us (unsigned int us)
{

	while(us--)
	{
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	}
}

/***********************???????************************/
void Send_threelines(unsigned char addr,uchar dat) 
{
   unsigned char i;
	VRST=0;
	delay_1ms(5);
	VRST=1;
	delay_1ms(20); /* ????20ms*/
	VCS=0;
	delay_1ms(5);  /* ????5ms */
    for(i=0;i<8;i++)
	{
		VSCL=0;
		if(addr&0x01)
		{
			VSDA=1;
		}
		else 
			VSDA=0;
		addr>>=1;
		delay_us(150); /* 150us */
		VSCL=1;
		delay_us(150); /* 150us */
	}
	VCS=1;
	delay_1ms(30);
	if(dat == 1)
		while(VBUSY == 0);	  //???
}

#endif


 