#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "lwip_comm.h"
#include "led.h"
#include "tsensor.h"
#include "rtc.h"
#include "lcd.h"
#include "string.h"
#include "stdlib.h"  
 

#define NUM_CONFIG_CGI_URIS	6  /* \brief CGI的URI数量 */
#define NUM_CONFIG_SSI_TAGS	6  /* \brief SSI的TAG数量 */

//控制LED和BEEP的CGI handler
const char* M1_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M2_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M3_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M4_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M5_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* MALL_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

static const char *ppcTAGs[]=  //SSI的Tag
{
	"a", /**< \brief M1 */
	"b", /**< \brief M2 */
	"c", /**< \brief M3 */
	"d", /**< \brief M4 */
	"e", /**< \brief M5 */
	"f"  /**< \brief MALL */
};

/** \brief cgi程序 */
static const tCGI ppcURLs[] = 
{
	{"/M1.cgi", M1_CGI_Handler},
	{"/M2.cgi", M2_CGI_Handler},
	{"/M3.cgi", M3_CGI_Handler},
	{"/M4.cgi", M4_CGI_Handler},
	{"/M5.cgi", M5_CGI_Handler},
	{"/MALL.cgi", MALL_CGI_Handler}

};


/*
* \brief 当web客户端请求浏览器的时候,使用此函数被CGI handler调用
*
* \param[in] pcToFind   :
* \param[in] pcParam    :
* \param[in] iNumParams :
*
* \retval  正确 返回字符串对比成功的循环值
*          错误 -1
*/
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for (iLoop = 0; iLoop < iNumParams; iLoop ++ ) {
		if(strcmp(pcToFind,pcParam[iLoop]) == 0) {
		
			return (iLoop); //返回iLOOP
		}
	}
	return (-1);
}

void m1_sta_Handler(char *pcInsert)
{

	/* LED1 */
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) == 1) {

		*pcInsert =       'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	}

}

void m2_sta_Handler(char *pcInsert)
{

	/* LED2 */
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 1) {

		*pcInsert =       'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	}
}

void m3_sta_Handler(char *pcInsert)
{

	/* LED3 */
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_6) == 1) {

		*pcInsert =       'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	}
}

void m4_sta_Handler(char *pcInsert)
{

	/* LED4 */
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1) == 1) {

		*pcInsert =       'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	}
}

void m5_sta_Handler(char *pcInsert)
{

	/* LED5 */
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) == 1) {

		*pcInsert =       'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	}
}

void mall_sta_Handler(char *pcInsert)
{

	/* ALL */
	if (   (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) == 0)
		  && (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 0)
      && (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_6) == 0)
      && (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1) == 0)
      && (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) == 0) )	{

		*pcInsert =       'O';
		*(pcInsert + 1) = 'N';
		*(pcInsert + 2) = ' ';
	} else {

		*pcInsert       = 'O';
		*(pcInsert + 1) = 'F';
		*(pcInsert + 2) = 'F';
	}
}
//SSI的Handler句柄
static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
	switch(iIndex)
	{
		case 0: 

	     m1_sta_Handler(pcInsert);
				break;
		case 1:

			 m2_sta_Handler(pcInsert);
				break;
		case 2:

		   m3_sta_Handler(pcInsert);
				break;
		case 3:
			 m4_sta_Handler(pcInsert);
				break;
		case 4:
			 m5_sta_Handler(pcInsert);
			break;
		case 5:
			mall_sta_Handler(pcInsert);
			break;
	}
	return strlen(pcInsert);
}

//CGI LED控制句柄

/*
*	\brief LED控制句柄
*
* \param[in] iIndex  : 
* \param[in] pcParam : 
* \param[in] pcValue : 
*
* \return 返回1个网页的名字
*/
const char* M1_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("M1", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
		LED1 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "M1") == 0) {  /* 检查参数"led" 属于控制LED1灯的 */
		  
			if (strcmp(pcValue[i], "M1_ON") == 0) { /* 改变LED1状态 */
				LED1 = 0; /* 打开LED1 */
			} else if (strcmp(pcValue[i],"M1_OFF") == 0) {
				LED1 = 1; /* 关闭LED1 */
			}
		 }
		}
	 } 
	 return "/index.shtml";
}

const char* M2_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("M2", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
		LED2 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "M2") == 0) {  /* 检查参数"led" 属于控制LED1灯的 */
		  
			if (strcmp(pcValue[i], "M2_ON") == 0) { /* 改变LED1状态 */
				LED2 = 0; /* 打开LED1 */
			} else if (strcmp(pcValue[i],"M2_OFF") == 0) {
				LED2 = 1; /* 关闭LED1 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}

const char* M3_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("M3", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
		LED3 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "M3") == 0) {  /* 检查参数"led" 属于控制LED3灯的 */
		  
			if (strcmp(pcValue[i], "M3_ON") == 0) { /* 改变LED1状态 */
				LED3 = 0; /* 打开LED3 */
			} else if (strcmp(pcValue[i],"M3_OFF") == 0) {
				LED3 = 1; /* 关闭LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* M4_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("M4", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
		LED4 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "M4") == 0) {  /* 检查参数"led" 属于控制LED3灯的 */
		  
			if (strcmp(pcValue[i], "M4_ON") == 0) { /* 改变LED1状态 */
				LED4 = 0; /* 打开LED3 */
			} else if (strcmp(pcValue[i],"M4_OFF") == 0) {
				LED4 = 1; /* 关闭LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* M5_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("M5", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
		LED5 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "M5") == 0) {  /* 检查参数"led" 属于控制LED3灯的 */
		  
			if (strcmp(pcValue[i], "M5_ON") == 0) { /* 改变LED1状态 */
				LED5 = 0; /* 打开LED3 */
			} else if (strcmp(pcValue[i],"M5_OFF") == 0) {
				LED5 = 1; /* 关闭LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* MALL_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* 注意根据自己的GET的参数的多少来选择i值范围 */
	iIndex = FindCGIParameter("MALL", pcParam, iNumParams);  /* 找到led的索引号 */
	
	/* 只有一个CGI句柄 iIndex = 0 */
	if (iIndex != -1) {
		
//		LED5 = 1;  /* 关闭LED1灯 */
		for (i=0; i < iNumParams; i++) { /* 检查CGI参数 */
		
		  if (strcmp(pcParam[i] , "MALL") == 0) {  /* 检查参数"led" 属于控制LED3灯的 */
		  
			if (strcmp(pcValue[i], "MALL_ON") == 0) { /* 改变LED1状态 */
				LED1 = 0;
				LED2 = 0;
				LED3 = 0;
				LED4 = 0;
				LED5 = 0; /* 打开LED5 */
			} else if (strcmp(pcValue[i],"MALL_OFF") == 0) {
				LED1 = 1;
				LED2 = 1;
				LED3 = 1;
				LED4 = 1;
				LED5 = 1; /* 关闭LED5 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}

void httpd_ssi_init(void)
{  
	//配置SSI句柄
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//CGI句柄初始化
void httpd_cgi_init(void)
{ 
	//配置CGI句柄
	http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}







