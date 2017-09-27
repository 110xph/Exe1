#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"

void TIM4_PWM_Init(u32 arr,u32 psc);

int main(void)
{
	  u16 len = 0;					//
		u16 pwmVal = 100;			//duty ratio
		u16 f = 2000;					//Frequency
	  u8 t = 0;
		u16 tmp = 0;
//		u16 limit = 0;
		u16 flag = 0;
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		delay_init(168);
		uart_init(115200);

		TIM4_PWM_Init(500-1,84-1);											//arr = 1000000/f
		TIM_SetCompare4(TIM4,pwmVal);
	
		printf("   \n");
	  printf("\r\n1--edit Frequency  2--edit Duty Cycle\r\n");

		USART_RX_STA = 0;
		while(1)
		{
						
			if(USART_RX_STA&0x8000)
			{
				if(flag == 1)
				{// wait for frequency
						printf("\r\n1\r\n");

							len=USART_RX_STA&0x3fff;
							f = 0;
							for(t=0;t<len;t++)
							{
								UART5->DR=USART_RX_BUF[t];
								f = f * 10 + (USART_RX_BUF[t] - '0');
								while((UART5->SR&0X40)==0);
							}
							printf("\r\nYour setting:%d\r\n", f);
							//limit
							if(50 <= f && f <= 20000)
							{
								//edit TIM4
								tmp = TIM4->ARR;
								TIM4->ARR = 1000000/f;
								TIM4->CCR4 = (TIM4->CCR4*TIM4->ARR)/tmp;
								
								flag = 0;
								USART_RX_STA=0;
							}
							else
							{
								printf("\r\nInValid frequency!\r\n");
							}
				}
				else if(flag == 2)
				{// wait for Duty Ratio

						printf("\r\n2\r\n");
				
						len=USART_RX_STA&0x3fff;
						pwmVal = 0;
						for(t=0;t<len;t++)
						{
							UART5->DR=USART_RX_BUF[t];
							pwmVal = pwmVal * 10 + (USART_RX_BUF[t] - '0');
							while((UART5->SR&0X40)==0);
						}

						printf("\r\nYour setting:%d\r\n", pwmVal);

						if(10< pwmVal && pwmVal < 90)
						{
						//edit TIM4
							TIM4->CCR4 = (1-pwmVal*0.01)*TIM4->ARR;												
						}
						else
						{
							printf("\r\nInValid Duty Ratio!\r\n");
						}
						flag = 0;
						USART_RX_STA=0;
				}								
				else
				{
					flag = USART_RX_BUF[0] - '0';
					while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
		
					if(flag != 1 && flag != 2)
					{
						printf("\r\nInvalid Value!\r\n");					
						flag = 0;
					}
					else if(flag == 1)
					{
						printf("\r\nThe Frequency:\r\n");					
					}
					else if(flag == 2)
					{
						printf("\r\nThe Duty Ratio:\r\n");
					}
				
					USART_RX_STA=0;	
				}
			}			
		}
	
	return 0;
}


void TIM4_PWM_Init(u32 arr,u32 psc) 
{           
	GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;   

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//AF9(TIM4)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 	
  GPIO_Init(GPIOB,&GPIO_InitStructure); 						 //initiate AF9

  TIM_TimeBaseStructure.TIM_Prescaler=psc; 	
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	
  TIM_TimeBaseStructure.TIM_Period=arr;							 	
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;	
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);		//initiate TIM14

	//TIM4 channel 4
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);					//initiate TIM4 OC1

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4,ENABLE);

	TIM_Cmd(TIM4, ENABLE);														//enable TIM4
}