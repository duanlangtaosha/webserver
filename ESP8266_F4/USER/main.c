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


int main(void)
{ 
 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usmart_dev.init(84); 	//初始化USMART			
	LED_Init();					  //初始化LED  
	
	TIM3_Int_Init(1000,839);	//定时器3频率为100hz

 	while(1)
	{	

	}
}
