#include "./BLE/BLE.h"
#include "./Delay/delay.h"
#include <string.h>					// Ϊ��ʹ�� strcat ����
#include <stdio.h>					// Ϊ��ʹ�� printf ����

uint8_t BLE_FRAME_REC_FLAG=0;
char BLE_FRAME[100];
int BLE_FRAME_INDEX=0;

void BLE_init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( BLE_NRST_PORT_CLK|BLE_PWRC_PORT_CLK|BLE_INT_PORT_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;


	GPIO_InitStructure.GPIO_Pin = BLE_NRST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLE_NRST_PORT, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = BLE_PWRC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLE_NRST_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BLE_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLE_INT_PORT, &GPIO_InitStructure);



	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
 	GPIO_EXTILineConfig(BLE_INT_PIN_SOURCE_PORT,BLE_INT_PIN_SOURCE_PIN);

	// �ⲿ�ж�
	EXTI_InitStructure.EXTI_Line = BLE_INT_EXT_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// �����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = BLE_INT_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x05;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



	// AT_MODE_ON();
	// BLE_RST_OFF();
	// printf("AT+RST\r\n");
	// delay_ms(100);
	// printf("AT+STARTEN0\r\n");
	// delay_ms(100);
	// printf("AT+BAUD8\r\n");
	// delay_ms(100);
	// printf("AT+START\r\n");




	// ������λ
	BLE_RST_OFF();

	// �ر� AT ����
	AT_MODE_OFF();
}

void BLE_USART_SendStr(char* str){
	printf("%s",str);
}

void BLE_USART_CMD(char* cmd) {
	// Ԥ����20���ȣ�����strcat֮�󲻻����
	char BLE_CMD[50]="AT+";
	char enter[] = "\r\n";
	char* str_pt = BLE_CMD;
	str_pt = strcat(str_pt,cmd);
	str_pt = strcat(str_pt,enter);
	BLE_USART_SendStr(str_pt);
}

// ������������ģʽ
void BLE_Sleep(void) {
	// printf("AT+DISC\r\n");
	// delay_ms(2000);
	printf("AT+SLEEP1\r\n");
}

// �������빤��ģʽ
void BLE_WakeUp(void) {
	// �½��ػ���
	AT_MODE_OFF();
	AT_MODE_ON();

	// �ر�AT_MODE
	delay_ms(100);
	AT_MODE_OFF();

	// ������λ
	BLE_RST_OFF();
}
