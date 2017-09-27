#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "can.h"


int main(void)
{
//  u8 mode = 1;			//CAN_MODE_X中设定
	u8 msg[LEN];
	u8 i = 0;
	u16 times = 0;
	u16 len = 0;
	u8 t = 0;
	
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
	msg[0] = 'a';
	msg[1] = 'b';
	msg[2] = 'c';
	msg[3] = 'd';
	msg[4] = 'e';
	
//	USART_RX_STA = 0;
	printf("\r\n");
	printf("Test_can\r\n");
	
	USART_RX_STA = 0;
	while(1)
	{					
			if(USART_RX_STA&0x8000)
			{
				len = recvBuf[0] - '0';
				printf("\r\nReceived data:");
				for(t = 1;t <= len;t++)
				{
//						printf("%c", recvBuf[t]);
						UART5->DR = recvBuf[t];
//						while((UART5->SR&0x40) == 0);
				}
				printf("\r\n");
				
				USART_RX_STA = 0;
			}
			else
			{
				if(times % 100 == 0)
				{
				//can口发送数据
						if(can_send_msg(0x12, msg, 5))
						{
								printf("\r\nSuccess to send!\r\n");
						}
						else
						{
								printf("\r\nFailure to send...\r\n");
						}
				}
				times++;
				
				delay_ms(10);
			}
			
	}

		return 0;
}
