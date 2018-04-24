#include "./gatelock/gatelock.h"

void Gate_Init(void) {
	// IO ��ʼ��
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(L9LL0S_A_GPIO_CLK|L9LL0S_B_GPIO_CLK, ENABLE);	//����Ӧ�� IO ʱ��
	GPIO_InitStructure.GPIO_Pin = L9LL0S_A_PIN;				//ѡ����Ҫ���õ� IO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�Ϊ50Mhz
	GPIO_Init(L9LL0S_A_GPIO_PORT, &GPIO_InitStructure);		//��ʼ��
	GPIO_InitStructure.GPIO_Pin = L9LL0S_B_PIN;
	GPIO_Init(L9LL0S_B_GPIO_PORT, &GPIO_InitStructure);
}

void Gate_Unlock(void) {
	// �������򿪡�
	motor_anti_clockwise();

	delay_ms(500);


	motor_hold_on();
	// if(!SPEAK_flag) SPEAK_OPEN_THE_DOOR();

	// ��2s����ʱ��
	// ֮������ôд������Ϊ�üĴ�����ʽ��ʱ��24λ��ʱ�Ĵ�����д����������2^24����72M����ʱ����ſ�����ʱ1.5s,�ʲ���дdelay_ms(10000)
	delay_ms(1000);delay_ms(1000);

	// �������رա�
	motor_clockwise();
	delay_ms(500);
	motor_hold_on();
}
