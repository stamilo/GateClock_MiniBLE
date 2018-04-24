#include "./usart/debug_usart.h"
#include "./data/data_def.h"		// Ϊ��ʹ�� Userid2Ascii ת���û�ID���Լ�һЩ�궨��
#include "./Delay/delay.h"
#include <stdio.h>					// Ϊ��ʹ�� fgetc �����ض��� printf


static volatile uint32_t  UARTTimeout = UART_WAIT_TIME;	// ���ڽ��յȴ�ʱ��
u8 USART_RecvBuf[USART_RECVBUF_LENGTH];	// ���ڽ������ݻ����
u8 USART1_RecvBuf_Length=0;				// ���ڽ������ݵ��ܳ���
u8 USART_Recv_Flag=0;					// ���ڽ��յ����ݱ�־λ
u8 BLE_MAC[17];


/********************************************** �û����� *************************************************/

// �����ֻ�������
// result:	�Ƿ��յ�����Ч�İ�ͷ����
u16 Usart_RecvOrder(USART_TypeDef* pUSARTx) {
	// ������ڽ��յ�����
	if(USART_Recv_Flag==1) {
		// ��ձ�־λ
		USART_Recv_Flag = 0;

		// ���RecBuf���ݳ���
		USART1_RecvBuf_Length = 0;

		return SYS_RECV_ORDER;
	}
	return SYS_NO_ORDER;
}

// ���͡��û�ID��
// pUSARTx:		���ں�
// user_id:		ʮ�����û�id
// notes:		12=0x303132 ��ʽ
void Usart_SendUserId(USART_TypeDef* pUSARTx, u16 user_id) {
	u8 temp[3];
	Userid2Ascii(user_id,temp);

	for (u8 i=0; i<3; i++){
		pUsart_SendByte(pUSARTx, temp+i);
	}
}

// ���͡�ָ�ơ���ӳɹ����ݰ�
// pUSARTx:		���ں�
// seq:			�ڼ��βɼ��ɹ�
void Usart_SendFinger_ADD_Success(USART_TypeDef* pUSARTx, u16 seq, u16 user_id) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	if (seq<3)
		temp.m_totalLength = 12;
	else
		temp.m_totalLength = 15;
	temp.m_cmdId = CMDID_ADD_FINGER;
	temp.m_seq = seq;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}

	// �����û�ID
	if (seq==3)
		Usart_SendUserId(pUSARTx, user_id);
}

// ���͡�ָ�ơ����ʧ�����ݰ�
// pUSARTx:		���ں�
// seq:			�ڼ��βɼ�ʧ��
// errorcode:	����ԭ��
void Usart_SendFinger_ADD_Error(USART_TypeDef* pUSARTx, u16 seq, u16 errorcode) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_ADD_FINGER;
	temp.m_seq = seq;
	temp.m_errorCode = errorcode;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡�ָ�ơ�ɾ���ɹ����ݰ�
// pUSARTx:		���ں�
void Usart_SendFinger_DEL_Success(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_FINGER;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡�ָ�ơ�ɾ��ʧ�����ݰ�
// pUSARTx:		���ں�
void Usart_SendFinger_DEL_Error(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_FINGER;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_ERROR;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡���Ƶ����¼��ɹ����ݰ�
// pUSARTx:		���ں�
// user_id:		ʮ�����û�id
void Usart_SendRFCard_ADD_Success(USART_TypeDef* pUSARTx, u16 user_id) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 15;
	temp.m_cmdId = CMDID_ADD_RFCARD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++) {
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}

	// �����û� ID
	Usart_SendUserId(pUSARTx, user_id);
}

// ���͡���Ƶ����¼��ʧ�����ݰ�
// pUSARTx:		���ں�
void Usart_SendRFCard_ADD_Error(USART_TypeDef* pUSARTx, u16 errorcode) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_ADD_RFCARD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = errorcode;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡���Ƶ����ɾ���ɹ����ݰ�
// pUSARTx:		���ں�
void Usart_SendRFCard_DEL_Success(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_RFCARD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡���Ƶ����ɾ��ʧ�����ݰ�
// pUSARTx:		���ں�
void Usart_SendRFCard_DEL_Error(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_RFCARD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_ERROR;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡����롿¼��ɹ����ݰ�
// pUSARTx:		���ں�
// user_id:		ʮ�����û�id
void Usart_SendPassword_ADD_Success(USART_TypeDef* pUSARTx, u16 user_id) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 15;
	temp.m_cmdId = CMDID_ADD_PASSWORD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++) {
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}

	// �����û� ID
	Usart_SendUserId(pUSARTx, user_id);
}

// ���͡����롿¼��ʧ�����ݰ�
// pUSARTx:		���ں�
void Usart_SendPassword_ADD_Error(USART_TypeDef* pUSARTx, u16 errorcode) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_ADD_PASSWORD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = errorcode;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡���Ƶ����ɾ���ɹ����ݰ�
// pUSARTx:		���ں�
void Usart_SendPassword_DEL_Success(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_PASSWORD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡���Ƶ����ɾ��ʧ�����ݰ�
// pUSARTx:		���ں�
void Usart_SendPassword_DEL_Error(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_DEL_PASSWORD;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_ERROR;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ���͡�һ���������ɹ����ݰ�
// pUSARTx:		���ں�
void Usart_SendOpenDoor_Success(USART_TypeDef* pUSARTx) {
	// �����ͷ����
	BleDataHead temp;
	temp.m_magicCode = MAGICCODE;
	temp.m_version = VERSION;
	temp.m_totalLength = 12;
	temp.m_cmdId = CMDID_OPEN_DOOR;
	temp.m_seq = 0x0000;
	temp.m_errorCode = ERROR_CODE_SUCCESS;

	// ���Ͱ�ͷ����
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(pUSARTx, (u16*)&temp+i);
	}
}

// ��������оƬ���ص����ݣ��趨 mac ��ַ
void Init_BLE_MAC(void) {

	delay_ms(100);

	// ���ͻ�ȡ mac ��ַ����
	printf("AT+MAC\r\n");

	// ������һ�㴦�����ݵ�ʱ��
	delay_ms(10);

	// ������ڽ��յ�����
	if(USART_Recv_Flag==1) {
		// ��ձ�־λ
		USART_Recv_Flag = 0;

		// ���RecBuf���ݳ���
		USART1_RecvBuf_Length = 0;
	}

	// ��ȡ mac ��ַ
	// +MAC=112233445566**  **��ʾĩβ���ϵĻس�0D�ͻ���0A
	// �յ������ݵĸ�ʽΪ 2B 4D 41 43 3D 33 43 41 35 30 39 30 41 31 46 41 46 0D 0A
	//					 +	M  A  C  =  3  C  A  5  0  9  0  A  1  F  A  F
	u8 j=5;
	for (u8 i=0; i<17; i++) {
		if ( i==2 || i==5 || i==8 || i==11 || i==14) {
			BLE_MAC[i] = 0x3A;	// 0x3A �� ��ð��
		}
		else {
			BLE_MAC[i] = USART_RecvBuf[j];
			j++;
		}
	}

}

// �����ֻ��˷�����ʱ��ͬ����Ϣ�����°��ӵ�ϵͳʱ��
void Cogradient_Time(void) {
	u8 mon=0,day=0,hour=0,min=0,sec=0;
	u16 year=0;

	// ������ݰ��е�ʱ����Ϣ
	year = year + (u16)(USART_RecvBuf[12]-0x30)*1000;
	year = year + (u16)(USART_RecvBuf[13]-0x30)*100;
	year = year + (u16)(USART_RecvBuf[14]-0x30)*10;
	year = year + (u16)(USART_RecvBuf[15]-0x30);
	mon = mon + (USART_RecvBuf[16]-0x30)*10;
	mon = mon + (USART_RecvBuf[17]-0x30);
	day = day + (USART_RecvBuf[18]-0x30)*10;
	day = day + (USART_RecvBuf[19]-0x30);
	// hour = hour + (USART_RecvBuf[20]-0x30)*10;
	// hour = hour + (USART_RecvBuf[21]-0x30);
	// min = min + (USART_RecvBuf[22]-0x30)*10;
	// min = min + (USART_RecvBuf[23]-0x30);
	// sec = sec + (USART_RecvBuf[24]-0x30)*10;
	// sec = sec + (USART_RecvBuf[25]-0x30);

	// ��ȡ��һ�����ݣ��ֻ���ֻ��һ���Է������ 20 Bytes
	delay_ms(1000);	// ��ʱһ��ʱ��ȴ��°�����
	if ( Usart_RecvOrder(USART1)==SYS_RECV_ORDER ) {
		hour = hour + (USART_RecvBuf[12]-0x30)*10;
		hour = hour + (USART_RecvBuf[13]-0x30);
		min = min + (USART_RecvBuf[14]-0x30)*10;
		min = min + (USART_RecvBuf[15]-0x30);
		sec = sec + (USART_RecvBuf[16]-0x30)*10;
		sec = sec + (USART_RecvBuf[17]-0x30);
	}

	// ����ϵͳʱ��
	RTC_Set(year, mon, day, hour, min, sec);
}





/********************************************** �ײ㺯�� *************************************************/

// �ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f){
	// ����һ���ֽ����ݵ�����
	USART_SendData(USART3, (uint8_t) ch);

	// �ȴ��������
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

	return (ch);
}

// �ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
//int fgetc(FILE *f){
//	// �ȴ�������������
//	while (USART_GetFlagStatus(UART_1, USART_FLAG_RXNE) == RESET);
//	return (int)USART_ReceiveData(UART_1);
//}

// ����һ���ֽڣ�ʹ�õ�ַ����
void pUsart_SendByte( USART_TypeDef * pUSARTx, u8* ch ){
	// ����һ���ֽ����ݵ�USART1
	USART_SendData(pUSARTx,*ch);
	// ����
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// һ���Է��������ֽڣ�Ҳ����һ����Ϣ��,ʹ�õ�ַ����
void pUsart_SentMessage( USART_TypeDef * pUSARTx, u16 * message){
	// pUSARTx:		���ں�
	// message:		��Ҫ���͵�16λ��Ϣ
	u8 temp_h, temp_l;

	// ȡ���߰�λ
	temp_h = ((*message)&0XFF00)>>8;
	// ȡ���Ͱ�λ
	temp_l = (*message)&0XFF;
	// ���͸߰�λ
	USART_SendData(pUSARTx,temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	// ���͵Ͱ�λ
	USART_SendData(pUSARTx,temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// ����һ���ֽ�
uint16_t Usart_SendByte( USART_TypeDef * pUSARTx, u8 ch ){
	UARTTimeout = UART_WAIT_TIME;
	// ����һ���ֽ����ݵ�USART1
	USART_SendData(pUSARTx,ch);

	// �ȴ��������
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET){
		if(UARTTimeout-- == 0) return 0xFFFF;
	}
	return 0;
}

// ����һ���ֽڣ��ر�ע�⣬���ڼĴ���ֻ�е�9λ��Ч����������
uint16_t Usart_RecvByte( USART_TypeDef * pUSARTx){

	// ���ر�ע�⡿������ط�һ��Ҫ��FFFFF��ô������ݣ�������1000��ôС��
	UARTTimeout = UART_WAIT_TIME;
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE) == RESET){
		if(UARTTimeout-- == 0) return 0xFFFF;
	}
	return USART_ReceiveData(pUSARTx)&0x00FF;
}

// ���ô����ж����ȼ�
static void NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Ƕ�������жϿ�������ѡ�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* ����USARTΪ�ж�Դ */
	NVIC_InitStructure.NVIC_IRQChannel = BLE_USART_IRQn;
	/* �������ȼ�Ϊ1 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	/* �����ȼ�Ϊ1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	/* ʹ���ж� */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* ��ʼ������NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

// ���ڳ�ʼ������
void debug_usart_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd( BEL_USART_RX_GPIO_CLK|BEL_USART_TX_GPIO_CLK, ENABLE);

	// ʧ�� UART ʱ��
	USART_DeInit(BEL_USART);

	// ���� Rx ����Ϊ���ù���
	GPIO_InitStructure.GPIO_Pin = BEL_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEL_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	// ���� Tx ����Ϊ���ù���
	GPIO_InitStructure.GPIO_Pin = BEL_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		// ���ÿ�©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEL_USART_TX_GPIO_PORT, &GPIO_InitStructure);

	// ʹ�� UART ʱ��
	RCC_APB1PeriphClockCmd(BEL_USART_CLK, ENABLE);

	// ���ô���ģʽ
	USART_InitStructure.USART_BaudRate = BEL_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(BEL_USART, &USART_InitStructure);

	// Ƕ�������жϿ�����NVIC����
	NVIC_Configuration();

	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(BEL_USART, USART_IT_RXNE, ENABLE);

	// ����������֡�жϡ�
	USART_ITConfig(BEL_USART, USART_IT_IDLE, ENABLE);

	// ʹ�ܴ���
	USART_Cmd(BEL_USART, ENABLE);

}
