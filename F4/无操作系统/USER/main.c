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


//ALIENTEK 探索者STM32F407开发板 实验14
//USMART调试实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


//LED状态设置函数
void led_set(u8 sta)
{
	LED1=sta;
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
}  


void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==1)
	{
		sprintf((char*)buf,"MAC    :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//打印MAC地址
//		LCD_ShowString(30,130,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
//		LCD_ShowString(30,150,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
//		LCD_ShowString(30,170,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
//		LCD_ShowString(30,190,210,16,16,buf); 
		printf("%s\r\n", buf);
	}
	else 
	{
		sprintf((char*)buf,"MAC      :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//打印MAC地址
//		LCD_ShowString(30,130,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
//		LCD_ShowString(30,150,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
//		LCD_ShowString(30,170,210,16,16,buf); 
		printf("%s\r\n", buf);
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
//		LCD_ShowString(30,190,210,16,16,buf); 
		printf("%s\r\n", buf);
	}	
}


int main(void)
{ 
 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usmart_dev.init(84); 	//初始化USMART			
	LED_Init();					  //初始化LED  
//  while(1) 
//	{		 	  
//		LED0=!LED0;					 
//		delay_ms(500);	
//	}
	
	
	TIM3_Int_Init(1000,839);	//定时器3频率为100hz
//	my_mem_init(SRAMIN);		//初始化内部内存池

	while(lwip_comm_init()) //lwip初始化
	{
		printf("LWIP Init Falied!\r\n");
		delay_ms(1200);  
		printf("Retrying...\r\n");
	}
	printf("LWIP Init Success!\r\n");
	printf("DHCP IP configing...\r\n");
#if LWIP_DHCP   //使用DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	}
#endif
	show_address(lwipdev.dhcpstatus);	//显示地址信息
 	while(1)
	{	
		static uint32_t i = 0;
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
		i++;
		if(i==50000)
		{
			LED0=~LED0;
			i=0;
		}
	}
}
