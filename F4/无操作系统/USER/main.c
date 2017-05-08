#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "malloc.h"
#include "enc28j60.h" 	 
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "timer.h"


//ALIENTEK ̽����STM32F407������ ʵ��14
//USMART����ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


//LED״̬���ú���
void led_set(u8 sta)
{
	LED1=sta;
} 
//�����������ò��Ժ���
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
}  


void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==1)
	{
		sprintf((char*)buf,"MAC    :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
//		LCD_ShowString(30,130,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
//		LCD_ShowString(30,150,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
//		LCD_ShowString(30,170,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
//		LCD_ShowString(30,190,210,16,16,buf); 
		printf("%s\r\n", buf);
	}
	else 
	{
		sprintf((char*)buf,"MAC      :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
//		LCD_ShowString(30,130,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
//		LCD_ShowString(30,150,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
//		LCD_ShowString(30,170,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
//		LCD_ShowString(30,190,210,16,16,buf); 
		printf("%s\r\n", buf);
	}	
}


int main(void)
{ 
 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usmart_dev.init(84); 	//��ʼ��USMART			
	LED_Init();					  //��ʼ��LED  
//  while(1) 
//	{		 	  
//		LED0=!LED0;					 
//		delay_ms(500);	
//	}
	
	
	TIM3_Int_Init(1000,839);	//��ʱ��3Ƶ��Ϊ100hz
//	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

	while(lwip_comm_init()) //lwip��ʼ��
	{
		printf("LWIP Init Falied!\r\n");
		delay_ms(1200);  
		printf("Retrying...\r\n");
	}
	printf("LWIP Init Success!\r\n");
	printf("DHCP IP configing...\r\n");
#if LWIP_DHCP   //ʹ��DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
	}
#endif
	show_address(lwipdev.dhcpstatus);	//��ʾ��ַ��Ϣ
 	while(1)
	{	
		static uint32_t i = 0;
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
		i++;
		if(i==50000)
		{
			LED0=~LED0;
			i=0;
		}
	}
}
