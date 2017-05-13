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
 

#define NUM_CONFIG_CGI_URIS	2  /* \brief CGI��URI���� */
#define NUM_CONFIG_SSI_TAGS	4  /* \brief SSI��TAG���� */

//����LED��BEEP��CGI handler
const char* M1_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* M2_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

static const char *ppcTAGs[]=  //SSI��Tag
{
	"t", /**< \brief ADCֵ */
	"w", /**< \brief �¶�ֵ */
	"h", /**< \brief ʱ�� */
	"y"  /**< \brief ���� */
};

/** \brief cgi���� */
static const tCGI ppcURLs[] = 
{
	{"/M1.cgi", M1_CGI_Handler},
	{"/M2.cgi", M2_CGI_Handler}
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


//SSIHandler����Ҫ�õ��Ĵ���ADC�ĺ���
void ADC_Handler(char *pcInsert)
{ 
	char Digit1=0, Digit2=0, Digit3=0, Digit4=0; 
    uint32_t ADCVal = 0;        

    //��ȡADC��ֵ
	ADCVal = T_Get_Adc_Average(1,10); //��ȡADC1_CH1�ĵ�ѹֵ
		
    //ת��Ϊ��ѹ ADCVval * 0.8mv
    ADCVal = (uint32_t)(ADCVal * 0.8);  
     
    Digit1= ADCVal/1000;
    Digit2= (ADCVal-(Digit1*1000))/100 ;
    Digit3= (ADCVal-((Digit1*1000)+(Digit2*100)))/10;
    Digit4= ADCVal -((Digit1*1000)+(Digit2*100)+ (Digit3*10));
        
    //׼����ӵ�html�е�����
    *pcInsert       = (char)(Digit1+0x30);
    *(pcInsert + 1) = (char)(Digit2+0x30);
    *(pcInsert + 2) = (char)(Digit3+0x30);
    *(pcInsert + 3) = (char)(Digit4+0x30);
}

//SSIHandler����Ҫ�õ��Ĵ����ڲ��¶ȴ������ĺ���
void Temperate_Handler(char *pcInsert)
{
	char Digit1=0, Digit2=0, Digit3=0, Digit4=0,Digit5=0; 
	short Temperate = 0;
		
	//��ȡ�ڲ��¶�ֵ 
	Temperate = Get_Temprate();//����100��
	
	
	Digit1 = Temperate / 10000;
	Digit2 = (((short)Temperate) % 10000)/1000;
    Digit3 = (((short)Temperate) % 1000)/100 ;
    Digit4 = (((short)Temperate) % 100)/10;
    Digit5 = ((short)Temperate) % 10;
	//��ӵ�html�е�����
	*pcInsert 	  = (char)(Digit1+0x30);
	*(pcInsert+1) = (char)(Digit2+0x30);
	*(pcInsert+2) =	(char)(Digit3+0x30);
	*(pcInsert+3) = '.';
	*(pcInsert+4) = (char)(Digit4+0x30);
	*(pcInsert+5) = (char)(Digit5+0x30);
}

//SSIHandler����Ҫ�õ��Ĵ���RTCʱ��ĺ���
void RTCTime_Handler(char *pcInsert)
{
	u8 hour,min,sec;
	
	hour = calendar.hour;
	min  = calendar.min;
	sec  = calendar.sec;
	*pcInsert = 	(char)((hour/10) + 0x30);
	*(pcInsert+1) = (char)((hour%10) + 0x30);
	*(pcInsert+2) = ':';
	*(pcInsert+3) = (char)((min/10) + 0x30);
	*(pcInsert+4) = (char)((min%10) + 0x30);
	*(pcInsert+5) = ':';
	*(pcInsert+6) = (char)((sec/10) + 0x30);
	*(pcInsert+7) = (char)((sec%10) + 0x30);
}

//SSIHandler����Ҫ�õ��Ĵ���RTC���ڵĺ���
void RTCdate_Handler(char *pcInsert)
{
	u16 year,month,date,week;

	year  = calendar.w_year;
	month = calendar.w_month;
	date  = calendar.w_date;
	week  = calendar.week;
	*pcInsert = '2';
	*(pcInsert+1) = '0';
	*(pcInsert+2) = (char)(((year%100)/10) + 0x30);
	*(pcInsert+3) = (char)((year%10) + 0x30);
	*(pcInsert+4) = '-';
	*(pcInsert+5) = (char)((month/10) + 0x30);
	*(pcInsert+6) = (char)((month%10) + 0x30);
	*(pcInsert+7) = '-';
	*(pcInsert+8) = (char)((date/10) + 0x30);
	*(pcInsert+9) = (char)((date%10) + 0x30);
	*(pcInsert+10) = ' ';
	*(pcInsert+11) = 'w';
	*(pcInsert+12) = 'e';
	*(pcInsert+13) = 'e';
	*(pcInsert+14) = 'k';
	*(pcInsert+15) = ':';
	*(pcInsert+16) = (char)(week + 0x30);
	
}
//SSI��Handler���
static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
	switch(iIndex)
	{
		case 0: 
				ADC_Handler(pcInsert);
				break;
		case 1:
				Temperate_Handler(pcInsert);
				break;
		case 2:
				RTCTime_Handler(pcInsert);
				break;
		case 3:
				RTCdate_Handler(pcInsert);
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
		  
			if (strcmp(pcValue[i], "M1ON") == 0) { /* �ı�LED1״̬ */
				LED1 = 0; /* ��LED1 */
			} else if (strcmp(pcValue[i],"M1OFF") == 0) {
				LED1 = 1; /* �ر�LED1 */
			}
		 }
//					else  if (strcmp(pcParam[i] , "M2") == 0) {  /* ������"led" ���ڿ���LED1�Ƶ� */
//		  
//			if (strcmp(pcValue[i], "M2ON") == 0) { /* �ı�LED1״̬ */
//				LED1 = 0; /* ��LED1 */
//			} else if (strcmp(pcValue[i],"M2OFF") == 0) {
//				LED1 = 1; /* �ر�LED1 */
//			}
//		 }
		}
	 }
//	if (LED1 == 0) {			
//		return "/M1_ON_M2_OFF.shtml";	/* LED1�� */
//	} else {
//		return "/M1_OFF_M2_OFF.shtml";	/* LED1�� */
//	}		
	 	if ((LED1 == 0) && (LED2 == 0)) {			
		return "/M1_ON_M2_ON.shtml";	/* LED1�� */
	} else if ((LED1 == 1) && (LED2 == 0)){
		return "/M1_OFF_M2_ON.shtml";	/* LED1�� */
	}	else if ((LED1 == 0) && (LED2 == 1)){
		return "/M1_ON_M2_OFF.shtml";	/* LED1�� */
	}	else {
		return "/M1_OFF_M2_OFF.shtml";	/* LED1�� */
	}		
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
		  
			if (strcmp(pcValue[i], "M2ON") == 0) { /* �ı�LED1״̬ */
				LED2 = 0; /* ��LED1 */
			} else {
				LED2 = 1; /* �ر�LED1 */
			}
		 }
		}
	 }
	if ((LED1 == 0) && (LED2 == 0)) {			
		return "/M1_ON_M2_ON.shtml";	/* LED1�� */
	} else if ((LED1 == 1) && (LED2 == 0)){
		return "/M1_OFF_M2_ON.shtml";	/* LED1�� */
	}	else if ((LED1 == 0) && (LED2 == 1)){
		return "/M1_ON_M2_OFF.shtml";	/* LED1�� */
	}	else {
		return "/M1_OFF_M2_OFF.shtml";	/* LED1�� */
	}				
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







