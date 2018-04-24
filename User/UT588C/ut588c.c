#include "./my_board.h"

void UT588C_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// RCC_APB2PeriphClockCmd(OLED_SCL_GPIO_CLK|OLED_SDA_GPIO_CLK, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	GPIO_InitStructure.GPIO_Pin = TG1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TG1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TG2_PIN;
	GPIO_Init(TG2_PORT, &GPIO_InitStructure);

	TG1=0;
	TG2=0;

	Power_ctrl_off();
	delay_ms(100);
	Power_ctrl_on();
}


void UT588C_speak(uint8_t count)
{
	int i;
	////��λ
	TG2=0;
	TG1=0;
	delay_us(100);
	TG2=1;
	delay_us(100);
	TG2=0;
	delay_us(100);
	TG2=1;
	delay_us(100);//TG2����оƬ��֮������
	TG2=0;
	for(i=1;i<=count;i++)	//������200us��ѭ�����ξͲ��ŵڼ��γ���
	{
		TG1=1;
		delay_us(100);
		TG1=0;
		delay_us(100);
	}
}
