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
 

#define NUM_CONFIG_CGI_URIS	6  /* \brief CGI��URI���� */
#define NUM_CONFIG_SSI_TAGS	6  /* \brief SSI��TAG���� */

//����LED��BEEP��CGI handler
const char* M1_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M2_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M3_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M4_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M5_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* MALL_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

static const char *ppcTAGs[]=  //SSI��Tag
{
	"a", /**< \brief M1 */
	"b", /**< \brief M2 */
	"c", /**< \brief M3 */
	"d", /**< \brief M4 */
	"e", /**< \brief M5 */
	"f"  /**< \brief MALL */
};

/** \brief cgi���� */
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
* \brief ��web�ͻ��������������ʱ��,ʹ�ô˺�����CGI handler����
*
* \param[in] pcToFind   :
* \param[in] pcParam    :
* \param[in] iNumParams :
*
* \retval  ��ȷ �����ַ����Աȳɹ���ѭ��ֵ
*          ���� -1
*/
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for (iLoop = 0; iLoop < iNumParams; iLoop ++ ) {
		if(strcmp(pcToFind,pcParam[iLoop]) == 0) {
		
			return (iLoop); //����iLOOP
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
//SSI��Handler���
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

//CGI LED���ƾ��

/*
*	\brief LED���ƾ��
*
* \param[in] iIndex  : 
* \param[in] pcParam : 
* \param[in] pcValue : 
*
* \return ����1����ҳ������
*/
const char* M1_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("M1", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
		LED1 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "M1") == 0) {  /* ������"led" ���ڿ���LED1�Ƶ� */
		  
			if (strcmp(pcValue[i], "M1_ON") == 0) { /* �ı�LED1״̬ */
				LED1 = 0; /* ��LED1 */
			} else if (strcmp(pcValue[i],"M1_OFF") == 0) {
				LED1 = 1; /* �ر�LED1 */
			}
		 }
		}
	 } 
	 return "/index.shtml";
}

const char* M2_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("M2", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
		LED2 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "M2") == 0) {  /* ������"led" ���ڿ���LED1�Ƶ� */
		  
			if (strcmp(pcValue[i], "M2_ON") == 0) { /* �ı�LED1״̬ */
				LED2 = 0; /* ��LED1 */
			} else if (strcmp(pcValue[i],"M2_OFF") == 0) {
				LED2 = 1; /* �ر�LED1 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}

const char* M3_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("M3", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
		LED3 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "M3") == 0) {  /* ������"led" ���ڿ���LED3�Ƶ� */
		  
			if (strcmp(pcValue[i], "M3_ON") == 0) { /* �ı�LED1״̬ */
				LED3 = 0; /* ��LED3 */
			} else if (strcmp(pcValue[i],"M3_OFF") == 0) {
				LED3 = 1; /* �ر�LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* M4_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("M4", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
		LED4 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "M4") == 0) {  /* ������"led" ���ڿ���LED3�Ƶ� */
		  
			if (strcmp(pcValue[i], "M4_ON") == 0) { /* �ı�LED1״̬ */
				LED4 = 0; /* ��LED3 */
			} else if (strcmp(pcValue[i],"M4_OFF") == 0) {
				LED4 = 1; /* �ر�LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* M5_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("M5", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
		LED5 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "M5") == 0) {  /* ������"led" ���ڿ���LED3�Ƶ� */
		  
			if (strcmp(pcValue[i], "M5_ON") == 0) { /* �ı�LED1״̬ */
				LED5 = 0; /* ��LED3 */
			} else if (strcmp(pcValue[i],"M5_OFF") == 0) {
				LED5 = 1; /* �ر�LED3 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}


const char* MALL_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i = 0;  /* ע������Լ���GET�Ĳ����Ķ�����ѡ��iֵ��Χ */
	iIndex = FindCGIParameter("MALL", pcParam, iNumParams);  /* �ҵ�led�������� */
	
	/* ֻ��һ��CGI��� iIndex = 0 */
	if (iIndex != -1) {
		
//		LED5 = 1;  /* �ر�LED1�� */
		for (i=0; i < iNumParams; i++) { /* ���CGI���� */
		
		  if (strcmp(pcParam[i] , "MALL") == 0) {  /* ������"led" ���ڿ���LED3�Ƶ� */
		  
			if (strcmp(pcValue[i], "MALL_ON") == 0) { /* �ı�LED1״̬ */
				LED1 = 0;
				LED2 = 0;
				LED3 = 0;
				LED4 = 0;
				LED5 = 0; /* ��LED5 */
			} else if (strcmp(pcValue[i],"MALL_OFF") == 0) {
				LED1 = 1;
				LED2 = 1;
				LED3 = 1;
				LED4 = 1;
				LED5 = 1; /* �ر�LED5 */
			}
		 }
		}
	 }
	 return "/index.shtml";
}

void httpd_ssi_init(void)
{  
	//����SSI���
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//CGI�����ʼ��
void httpd_cgi_init(void)
{ 
	//����CGI���
	http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}







