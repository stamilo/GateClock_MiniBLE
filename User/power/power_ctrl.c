#include "./power/power_ctrl.h"
#include "stm32f10x_pwr.h"

// �����Դ����	0��ʾ��������������ʱ�������λ1
uint8_t WAKEUP_FLAG=0;

void power_ctrl_init(void)
{
	int i;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	POWER_CTRL_GPIO_CLK, ENABLE );

	GPIO_InitStructure.GPIO_Pin = POWER_CTRL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER_CTRL_GPIO_PORT, &GPIO_InitStructure);
	Power_ctrl_off();
	for (i=0;i<999;i++);
}

// �������ģʽ
void PWR_Standby_Mode(void) {
	uint8_t cnt = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	// ԭ���İ����� GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	// û�õ�������
	// ���ڵİ����� GPIO_Pin_4 �����˵�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	// û�õ�������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	delay_ms(1000);		// ��ֹ����ͨ�Ž���ʱ��������
	BLE_Sleep();		// ������������
	VCC_Adc_Sleep();	// ��ADC������
	Power_ctrl_off();	// ����Ƶled���ϵ�

	// qs808����
	while((QS808_STANDBY() != ERR_SUCCESS) && (cnt < 5)) {
		QS808_Rec_Buf_refresh();
		delay_ms(10);
		cnt++;
	}
	WAKEUP_FLAG = 1;

	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
}

// �Ӵ���ģʽ�л���
void Wkup_init(void) {
	int i;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	// PA0���Žӵ���TTP229��SDO����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);

	for(i=0;i<999;i++);
}
