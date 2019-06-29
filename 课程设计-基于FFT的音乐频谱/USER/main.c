#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"
#include "adc.h"
#include "fft.h"
#include "math.h"
#include "timer.h"

#define units 0.015 //y�������λ

int time_flag=0;



void OLED_display(int data[]) //��ͼ����
{
	int x,y;
	OLED_Clear();//����
	for(x=0;x<128;x+=2)//
	{
		for(y=0;y<data[x/2]*units;y++)
		{
			OLED_DrawPoint(128-x,y,1);
			OLED_DrawPoint(128-x+1,y,1);
		}
	}
	OLED_Refresh_Gram();//������ʾ
}

int main(void)
 {	
	int i;
	u16 adcx;
	float temp;//��ѹֵ
	int flag=0;//ѭ��������־λ
	int data[128]={0};
	struct compx s[FFT_N];
	delay_init();	     //��ʱ��ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	TIM3_Int_Init(9,7199);
	uart_init(9600);	   
 	Adc_Init();
  OLED_Init();			//��ʼ��OLED   
	OLED_Refresh_Gram();//������ʾ��OLED	 
	while(1) 
	{	
			adcx=Get_Adc_Average(ADC_Channel_1,5);
			temp=adcx*(3.3/4096);
			//adcx=temp;
			//temp-=adcx;
			temp*=100;
		while(flag<128&&time_flag==1)
		{
			s[flag].real = temp;
      s[flag].imag = 0;
			flag++;
			time_flag=0;
		}
		if(flag>=128)
		{
			flag=0;
			FFT(s);
			for(i=0;i<64;i++)
			{
				data[i]=sqrt(s[i].real * s[i].real + s[i].imag * s[i].imag);
				//printf("%d\t",data[i]);
			}
			OLED_display(data);
		}

	}	 
}
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		time_flag=1;
		}
}

