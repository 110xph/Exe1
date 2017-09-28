#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "can.h"

int main(void)
{
//  u8 mode = 1;			//CAN_MODE_X中设定
	u8 msg[LEN];
	u8 tmp_msg[LEN];
	u8 i = 0;
	u16 times = 0;
	u16 len = 0;
	u8 t = 0;
	u16 sendLen = 5;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	delay_init(168);
	uart_init(115200);
	
	can_init();	
	
	//can口接收到数据，然后发送给串口
	can_add_callback(0x12, &can_rcv_msg);
		
	for(i = 0;i < 5;i++)
	{
		msg[i] = i + 1;
	}
	msg[0] = 'n';
	msg[1] = 'i';
	msg[2] = 'h';
	msg[3] = 'a';
	msg[4] = 'o';
	
//	USART_RX_STA = 0;
	printf("\r\n");
	printf("Test_can\r\n");
	
	USART_RX_STA = 0;
	
	while(1)
	{					
			if(USART_RX_STA&0x8000)
			{
				//关中断
				 CANX->IER &= ~CAN_IT_FMP0;

				len = recvBuf[0] - '0';
				printf("\r\nReceived data:");
				for(t = 1;t <= len;t++)
				{
//						printf("%c", recvBuf[t]);
						UART5->DR = recvBuf[t];
						printf("%c",recvBuf[t]);
//						while((UART5->SR&0x40) == 0);
				}
				printf("\r\n");
				
				USART_RX_STA = 0;
				//开中断
				 CANX->IER |= CAN_IT_FMP0;

			}
			else
			{
				if(times % 100 == 0)
				{			
					//can口发送数据
					if(sendLen <= 8)
					{
							if(can_send_msg(0x11, msg, sendLen))
							{
									printf("\r\nSuccess to send!\r\n");
									msg[sendLen] = 'a';
									sendLen++;
							}
							else
							{
									printf("\r\nFailure to send...\r\n");
							}
					}
					else
					{						
							if(can_send_msg(0x11, msg, 8))
							{
									printf("\r\nSuccess to send!\r\n");
							}
							else
							{
									printf("\r\nFailure to send...\r\n");
							}
							
							for(t = 0;t < 8;t++)
							{
								tmp_msg[t] = 'a';
							}
							
							t = sendLen - 8;

							while(t > 0)
							{
								if(t >= 8)
								{
									if(can_send_msg(0x11, tmp_msg, 8))
									{
										printf("\r\nSuccess to send!\r\n");
									}
									else
									{
										printf("\r\nFailure to send...\r\n");
									}
								}
								else
								{
									if(can_send_msg(0x11, tmp_msg, t))
									{
										printf("\r\nSuccess to send!\r\n");
									}
									else
									{
										printf("\r\nFailure to send...\r\n");
									}
								}
								if(t >= 8)
									t = t - 8;
								else
									t = 0;
							}
							sendLen++;
					}
				}
				times++;
				
				delay_ms(10);
			}
	}

		return 0;
}