#include "my_board.h"
#include "vcc_adc.h"
// ADCͨ����ʼ��
void VCC_Adc_Init(void) {
	// io��ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	VCC_ADC_GPIO_CLK, ENABLE );

	GPIO_InitStructure.GPIO_Pin = VCC_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VCC_ADC_GPIO_PORT, &GPIO_InitStructure);
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, ENABLE );	// ʹ��GPIOA,ADC1ͨ��ʱ��

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);	// ��Ƶ����

	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;		// ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	// ˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);		// ����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

	ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����

	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

	ADC_ResetCalibration(ADC1);	//����ָ����ADC1�ĸ�λ�Ĵ���

	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�

	ADC_StartCalibration(ADC1);

	while(ADC_GetCalibrationStatus(ADC1));	// ��ȡָ��ADC1��У׼����,����״̬��ȴ�
}

void VCC_Adc_Sleep(void) {
	ADC_Cmd(ADC1, DISABLE);

	ADC_TempSensorVrefintCmd(DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, DISABLE );
}

u16 VCC_Get_Adc(u8 ch) {
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��3,��һ��ת��,����ʱ��Ϊ239.5����
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

// ��ȡͨ��ch��ת��ֵ
// ȡtimes��,Ȼ��ƽ��
u16 VCC_Get_Adc_Average(u8 ch,u8 times) {
	u32 temp_val=0;
	u8 t;
	for (t=0;t<times;t++) {
		temp_val+=VCC_Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}

// ��ȡ��ص���
float Get_Battery(void) {
	u32 adcx,adcx2;
	float charge;
	adcx=VCC_Get_Adc_Average(ADC_Channel_Vrefint,20);	//��ȡͨ��16,20��ȡƽ��
	adcx2 = VCC_Get_Adc_Average(ADC_Channel_0,20); // �ⲿ��ѹ
	charge = 1.2/adcx*adcx2 * 2;
	charge = 1.5799f*charge - 1.8433f;//����ֵ����ֵ��һ�ι�ϵ��MATLAB��ϵõ���

	return charge;
}

// �͵�������
u16 Battery_Alarm(void) {
	
	return 0;
}
