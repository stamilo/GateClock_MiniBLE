#include  "./qs808/qs808_drive.h"


extern unsigned char HZ16x16[];
extern unsigned char F8X16[];

// ע�⣺����Duplication Check ����ע������ָ��ʱ���᷵�ش�����ERR_DUPLICATION_ID���Ӷ�����ע��
// ���ָ�ƿ��СΪ200��1~200���������ڴ���ָ�ƿ���ز���ʱ�����ó���200

QS808_Rec_Buf_type QS808_Rec_Buf ={
	.Trans_state = reset,
	.Rec_state = idle,
	.Rec_point = 0,
	.Rec_Buf = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
extern uint32_t ms10_cnt;

//���ո�λ�����յ��ذ��󣬷���flag��λ������״̬��λ��������ָ�븴λ
void QS808_Rec_Buf_refresh(void) {
	QS808_Rec_Buf.Rec_state = idle;
	QS808_Rec_Buf.Rec_point = 0x00;
	QS808_Rec_Buf.Trans_state = reset;
}
// ����qs808���ݰ�
// ���ô˺����󣬶�ʱ3ms�����û�����ݣ���ֱ�ӷ��ؿ�
// ����0��ʾ�հ���ȷ ����0xff ��ʾ�հ�����
uint8_t QS808_Unpack(QS808_Pack_struct * qs808_rec_bag) {
	int i=0;
	uint16_t check=0;
	uint16_t check2=0;
	int timeout_cnt=0x500000;	// ���1.5s

	// �ȴ�qs808��ʼ����
	while(QS808_Rec_Buf.Rec_state != busy) {
		timeout_cnt--;
		if(timeout_cnt<=0) {
			QS808_Rec_Buf_refresh();
			return 0xff;
		}
	}
	// �ȴ�3ms  26*8/115200=1.8ms���Է������һ֡
	delay_ms(3);
	QS808_Rec_Buf_refresh();
	for(i=0;i<24;i++){
		check += QS808_Rec_Buf.Rec_Buf[i];
	}
	// У���
	check2 = (QS808_Rec_Buf.Rec_Buf[25]<<8)|QS808_Rec_Buf.Rec_Buf[24];
	if(check != check2)	return 0xff;

	qs808_rec_bag->HEAD = (QS808_Rec_Buf.Rec_Buf[1]<<8)|QS808_Rec_Buf.Rec_Buf[0];
	qs808_rec_bag->SID1 = (QS808_Rec_Buf.Rec_Buf[2]);
	qs808_rec_bag->DID1 = QS808_Rec_Buf.Rec_Buf[3];
	qs808_rec_bag->RCM = (QS808_Rec_Buf.Rec_Buf[5]<<8)|QS808_Rec_Buf.Rec_Buf[4];
	qs808_rec_bag->LEN = (QS808_Rec_Buf.Rec_Buf[7]<<8)|QS808_Rec_Buf.Rec_Buf[6];
	qs808_rec_bag->RET = (QS808_Rec_Buf.Rec_Buf[9]<<8)|QS808_Rec_Buf.Rec_Buf[8];
	for(i=0;i<14;i++)
		qs808_rec_bag->DATA[i] = QS808_Rec_Buf.Rec_Buf[i+10];
	qs808_rec_bag->CKS = check;
	return 0x00;
}

uint16_t QS808_send_byte(uint8_t byte) {
	return Usart_SendByte(QS808_USART,byte);
}

uint16_t QS808_send_half_word(uint16_t half_word) {
	if(Usart_SendByte(QS808_USART,half_word>>8))
		return 0xFFFF;
	if(Usart_SendByte(QS808_USART,half_word & 0x00ff))
		return 0xFFFF;
	return 0x0000;
}

uint8_t QS808_RecByte(void) {
	return Usart_RecvByte(QS808_USART);
}

void QS808_Reset(void) {
	int i;
	QS808_NRST_High();
	for(i=0;i<1000;i++){}
	QS808_NRST_Low();
	for(i=0;i<1000;i++){}
	QS808_NRST_High();
	delay_ms(100);
}

void QS808_Init(void) {
	QS808_UART_Init();
	// ��λ����
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	QS808_NRST_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = QS808_NRST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(QS808_NRST_GPIO_PORT, &GPIO_InitStructure);
	// �ȸ�λQS808��д��������������QS808������Ӧʱ��

	// �ж�����
	GPIO_InitStructure.GPIO_Pin = QS808_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;
	GPIO_Init(QS808_INT_GPIO_PORT, &GPIO_InitStructure);

	// �жϳ�ʼ��
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
 	GPIO_EXTILineConfig(QS808_INT_PIN_SOURCE_PORT,QS808_INT_PIN_SOURCE_PIN);
	EXTI_InitStructure.EXTI_Line=QS808_INT_EXT_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = QS808_INT_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	QS808_Reset();
}

uint8_t QS808_TEST_CONNECTION(void) {
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);			// ��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_TEST_CONNECTION);	// CMD
	QS808_send_half_word(0x0000);				// LEN
	for (i=0;i<16;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x0001);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)	// ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

uint16_t QS808_CMD_GET_ENROLL_COUNT(void) {
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);			// ��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_GET_ENROLL_COUNT);	// CMD
	QS808_send_half_word(0x0400);				// LEN
	QS808_send_half_word(0x0100);				// start address
	QS808_send_half_word(0xF401);				// end address
	for (i=0;i<12;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x4102);//sum check
	QS808_Rec_Buf.Trans_state = set;
	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)	// ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else
		return ((qs808_rec_bag.DATA[1]<<8)|(qs808_rec_bag.DATA[0]));
}

// ˯��״̬�»�����������
uint8_t QS808_STANDBY(void) {
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_STANDBY);//CMD
	QS808_send_half_word(0x0000);//LEN
	for (i=0;i<16;i++)//data
	{
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x0B01);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

// ָ��ͷ�ɼ�ͼ��
uint8_t QS808_GET_IMAGE(void)
{
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_GET_IMAGE);//CMD
	QS808_send_half_word(0x0000);//LEN
	for (i=0;i<16;i++)//data
	{
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x1f01);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RET == ERR_FP_NOT_DETECTED)
		return ERR_FP_NOT_DETECTED;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

uint8_t QS808_CMD_GENERATE(uint16_t ram_id)
{
	QS808_Rec_Buf_refresh();
	//ram_idΪ16λ��ע����Ҫ����д������Ҫ����������ram2����ram_idΪ0x0200
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_GENERATE);//CMD
	QS808_send_half_word(0x0200);//LEN
	QS808_send_half_word(ram_id);//data
	for (i=0;i<14;i++)//data
	{
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x6101+ram_id);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_BUFFER_ID)
		return ERR_INVALID_BUFFER_ID;
	else if (qs808_rec_bag.RET == ERR_BAD_QUALITY)
		return ERR_BAD_QUALITY;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

uint8_t QS808_CMD_MERGE(void)
{
	QS808_Rec_Buf_refresh();
	//ģ��ϳɿɵ��ϳɸ�����Ŀ��ram������̶�Ϊ3��ָ�ƺϳ�һ��ģ�壬�浽ram0
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_MERGE);//CMD
	QS808_send_half_word(0x0300);//LEN
	QS808_send_half_word(0x0000);//�ϳɵ�ram0����
	QS808_send_byte(0x03);//3��ָ�ƺϳ�һ��ģ��
	for (i=0;i<13;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x6601);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_BUFFER_ID)
		return ERR_INVALID_BUFFER_ID;
	else if (qs808_rec_bag.RET == ERR_GEN_COUNT)
		return ERR_GEN_COUNT;
	else if (qs808_rec_bag.RET == ERR_MERGE_FAIL)
		return ERR_MERGE_FAIL;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

// ��⵱ǰ����ָ�ư���
uint8_t QS808_CMD_FINGER_DETECT(void) {
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_FINGER_DETECT);
	QS808_send_half_word(0x0000);
	for (i=0;i<16;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x2001);
	QS808_Rec_Buf.Trans_state = set;
	QS808_Pack_struct qs808_rec_bag;
	// ͨ��ʧ��
	if (QS808_Unpack(&qs808_rec_bag) == 0xff)	return QS808_DATA_ERR;
	else if (qs808_rec_bag.RCM == 0xff)			return QS808_DATA_ERR;
	else if (qs808_rec_bag.DATA[0]==0)			return ERR_NO_FINGER_DETECT;
	else if (qs808_rec_bag.DATA[0]==1)			return ERR_FINGER_DETECT;

	return QS808_UNKNOW_ERR;
}

uint8_t QS808_CMD_STORE_CHAR(uint16_t templete_num)
{
	//�������Ϊģ��ı�ţ�����Ҫ����ram�ţ�ֱ����ram0
	//ģ���Ÿߵ�λ��Ҫ����������浽��0x0011����ţ���ôӦ�������0x1100
	//templete_num���ô���200
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_STORE_CHAR);//CMD
	QS808_send_half_word(0x0400);//LEN
	QS808_send_half_word(templete_num);//templete_num
	QS808_send_half_word(0x0000);//ramid

	for (i=0;i<12;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x4301+templete_num);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_TMPL_NO)
		return ERR_INVALID_TMPL_NO;
	else if (qs808_rec_bag.RET == ERR_INVALID_BUFFER_ID)
		return ERR_INVALID_BUFFER_ID;
	else if (qs808_rec_bag.RET == ERR_DUPLICATION_ID)
		return ERR_DUPLICATION_ID;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

// ��ȡָ����ŷ�Χ�ڿ�ע����׸����
uint8_t QS808_CMD_GET_EMPTY_ID(uint8_t * ID) {
	QS808_Rec_Buf_refresh();
	int i;
	*ID = 0x00;
	QS808_send_half_word(QS808_HEAD);		//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_GET_EMPTY_ID);	//CMD
	QS808_send_half_word(0x0400);			//LEN
	QS808_send_half_word(0x0100);			//��ʼ��ַ
	QS808_send_half_word(0xc800);			//������ַ
	for (i=0;i<12;i++) {
		QS808_send_byte(0x00);
	}
	// sum check
	QS808_send_half_word(0x1102);
	QS808_Rec_Buf.Trans_state = set;
	QS808_Pack_struct qs808_rec_bag;
	// ͨ��ʧ��
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_PARAM)
		return ERR_INVALID_PARAM;
	else if (qs808_rec_bag.RET == ERR_SUCCESS) {
		*ID = qs808_rec_bag.DATA[0];
		return ERR_SUCCESS;
	}
	return QS808_UNKNOW_ERR;
}

// ���ָ��ͷ�е�����ָ����Ϣ��һ��200��ָ����Ϣ
uint8_t	QS808_CMD_DEL_ALL(void) {
	//���1~200��ָ��
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);	// ��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_DEL_CHAR);	// CMD
	QS808_send_half_word(0x0400);		// LEN
	QS808_send_half_word(0x0100);		// ��ʼ��ַ
	QS808_send_half_word(0xc800);		// ������ַ
	for (i=0;i<12;i++) {
		QS808_send_byte(0x00);
	}
	// sum check
	QS808_send_half_word(0x1002);
	QS808_Rec_Buf.Trans_state = set;
	QS808_Pack_struct qs808_rec_bag;

	// ͨ��ʧ��
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_PARAM)
		return ERR_INVALID_PARAM;
	else if (qs808_rec_bag.RET == ERR_TMPL_EMPTY)
		return ERR_TMPL_EMPTY;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)
		return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

// ʵ�鷢�֣����ģ����ָ�Ʊ���պ󣬲����᷵��ERR_ALL_TMPL_EMPTY�������
uint8_t	QS808_CMD_SEARCH(uint8_t * index) {
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_SEARCH);//CMD
	QS808_send_half_word(0x0600);//LEN
 	QS808_send_half_word(0x0000);//ָ��ģ����ram0��
	QS808_send_half_word(0x0100);//��ʼ��ַ 1
	QS808_send_half_word(0xc800);//������ַ 200
	for (i=0;i<10;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x3102);//sum check
	QS808_Rec_Buf.Trans_state = set;

	QS808_Pack_struct qs808_rec_bag;
	// ͨ��ʧ��
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_BUFFER_ID)
		return ERR_INVALID_BUFFER_ID;
	else if (qs808_rec_bag.RET == ERR_ALL_TMPL_EMPTY)
		return ERR_ALL_TMPL_EMPTY;
	else if (qs808_rec_bag.RET == ERR_IDENTIFY)
		return ERR_IDENTIFY;
	else if (qs808_rec_bag.RET == ERR_SUCCESS) {
		*index = qs808_rec_bag.DATA[0];
		return ERR_SUCCESS;
	}
	return QS808_UNKNOW_ERR;
}
extern uint32_t ms10_cnt;

// �ȴ�ָ�����뺯������ʱ����QS808_WAIT_TIMEOUT�����򷵻�0x00
// time���Ժ���Ϊ��λ�ĳ�ʱʱ��
// ���Է��֣�û����ָ����ʱ��ָ��ģ����500ms�ĵȴ������Բ���Ҫ����Ϊ��ӵȴ�ʱ��
uint8_t QS808_Wait(uint32_t time) {
	uint8_t temp;
	uint32_t cnt = time/180;//ɨ��Ȧ��
	while (cnt>0) {
		temp = QS808_GET_IMAGE();
		if (temp == ERR_FP_NOT_DETECTED) {
			cnt--;
			continue;
		}
		else if (temp == ERR_SUCCESS) return ERR_SUCCESS;
		else return QS808_DATA_ERR;
	}
	return QS808_WAIT_TIMEOUT;
}

// ����ֵ��
// 1.QS808_WAIT_TIMEOUT
// 2.ERR_BAD_QUALITY
// 3.ERR_MERGE_FAIL
// 4.ERR_DUPLICATION_ID
// 5.ERR_SUCCESS
// ������u8ָ��,�������ָ�Ƶ�ID,ID��1��ʼ
uint8_t QS808_Login(uint8_t* ID) {
	uint8_t temp;
	int i=0;

	// �ɼ�3��ָ����Ϣ
	for (i=0;i<3;i++){
		// �ɼ�ָ��
		temp = QS808_Wait(10000);
		if(temp == QS808_WAIT_TIMEOUT){
			return QS808_WAIT_TIMEOUT;
		}
		// ��������
		temp = QS808_CMD_GENERATE((uint16_t)(i<<8));
		if ((temp == QS808_DATA_ERR) || (temp == QS808_UNKNOW_ERR) || (temp == ERR_INVALID_BUFFER_ID))
			return QS808_DATA_ERR;
		if (temp == ERR_BAD_QUALITY)
			return ERR_BAD_QUALITY;
		delay_ms(1000);
	}

	// �ϲ�����
	temp = QS808_CMD_MERGE();

	// ���ݲ�����
	if ((temp == QS808_DATA_ERR) || (temp == QS808_UNKNOW_ERR) || (temp == ERR_INVALID_BUFFER_ID) || (temp == ERR_GEN_COUNT))
		return QS808_DATA_ERR;

	// �ϲ�����ʧ��
	if (temp == ERR_MERGE_FAIL)
		return ERR_MERGE_FAIL;

	// ��ȡ��ע���ID
	temp = QS808_CMD_GET_EMPTY_ID(ID);

	// ���ݲ�����
	if ((temp == QS808_DATA_ERR) || (temp == QS808_UNKNOW_ERR) || (temp == ERR_INVALID_PARAM))
		return QS808_DATA_ERR;

	// ���
	temp = QS808_CMD_STORE_CHAR((uint16_t)(*ID<<8));

	// ���ݲ�����
	if ((temp == QS808_DATA_ERR) || (temp == QS808_UNKNOW_ERR) || (temp == ERR_INVALID_TMPL_NO) || (temp == ERR_INVALID_BUFFER_ID))
		return QS808_DATA_ERR;

	// ָ���Ѵ���
	if (temp == ERR_DUPLICATION_ID)	return ERR_DUPLICATION_ID;

	// �ܹ��ܵ�������൱�ڳɹ���
	return ERR_SUCCESS;
}

uint8_t QS808_SEARCH(uint8_t* index) {
	uint8_t temp;
	// �ɼ�ָ��
	temp = QS808_Wait(1000);
	if(temp == QS808_WAIT_TIMEOUT)			return QS808_WAIT_TIMEOUT;

	// ����������ram0
	temp = QS808_CMD_GENERATE((uint16_t)(0x0000));
	if((temp==QS808_DATA_ERR) || (temp==QS808_UNKNOW_ERR) || (temp==ERR_INVALID_BUFFER_ID))	return QS808_DATA_ERR;
	else if (temp == ERR_BAD_QUALITY)		return ERR_BAD_QUALITY;

	// ����ָ��
	temp = QS808_CMD_SEARCH(index);
	if((temp==QS808_DATA_ERR) || (temp==QS808_UNKNOW_ERR) || (temp==ERR_INVALID_BUFFER_ID)) return QS808_DATA_ERR;
	else if (temp == ERR_ALL_TMPL_EMPTY)	return ERR_ALL_TMPL_EMPTY;
	else if(temp == ERR_IDENTIFY)			return ERR_IDENTIFY;
	else									return ERR_SUCCESS;
}

// ��չ̶���ŵ�ָ��
uint8_t	QS808_CMD_DEL_NUM(void) {
	QS808_Rec_Buf_refresh();
	uint8_t index;
	uint8_t temp;
	uint16_t sum;
	int i;
	temp = QS808_SEARCH(&index);//�ȵõ�ָ�Ʊ��
	if(temp == ERR_SUCCESS) {
		sum = 0x0147+index+index;
		QS808_send_half_word(QS808_HEAD);//��ͷ
		QS808_send_byte(SID);
		QS808_send_byte(DID);
		QS808_send_half_word(CMD_DEL_CHAR);//CMD
		QS808_send_half_word(0x0400);//LEN
		QS808_send_half_word((uint16_t) (index<<8));//��ʼ��ַ
		QS808_send_half_word((uint16_t) (index<<8));//������ַ
		for (i=0;i<12;i++)//data
		{
			QS808_send_byte(0x00);
		}
		QS808_send_byte(sum);//sum check
		QS808_send_byte(sum>>8);
		QS808_Rec_Buf.Trans_state = set;

		QS808_Pack_struct qs808_rec_bag;
		if(QS808_Unpack(&qs808_rec_bag) == 0xff)//ͨ��ʧ��
			return QS808_DATA_ERR;
		else if(qs808_rec_bag.RCM == 0xff)
			return QS808_DATA_ERR;
		else if (qs808_rec_bag.RET == ERR_INVALID_PARAM)
		{
			return ERR_INVALID_PARAM;
		}
		else if (qs808_rec_bag.RET == ERR_TMPL_EMPTY)
		{
			// printf("ɾ��ʧ��\n");
			return ERR_TMPL_EMPTY;
		}
		else if (qs808_rec_bag.RET == ERR_SUCCESS)
		{
			// printf("ɾ���ɹ�\n");
			return ERR_SUCCESS;
		}
		return QS808_UNKNOW_ERR;
	}
	else {
		// printf("ɾ��ʧ��\n");
		return ERR_DEL_FAIL;
	}
}

// ��QS808_CMD_DEL_NUM���������� QS808_CMD_DEL_NUM �ǰ���ָ Ȼ��ɾ����QS808_CMD_DEL_NUM2�Ǹ���ָ��IDɾ��ָ��
uint8_t	QS808_CMD_DEL_NUM2(uint8_t index) {
	// ��չ̶���ŵ�ָ��
	uint16_t sum;
	int i;
	QS808_Rec_Buf_refresh();
	sum = 0x0147+index+index;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_DEL_CHAR);//CMD
	QS808_send_half_word(0x0400);//LEN
	QS808_send_half_word((uint16_t) (index<<8));//��ʼ��ַ
	QS808_send_half_word((uint16_t) (index<<8));//������ַ
	for (i=0;i<12;i++) {
		QS808_send_byte(0x00);
	}

	// sum checks
	QS808_send_byte(sum);
	QS808_send_byte(sum>>8);
	QS808_Rec_Buf.Trans_state = set;
	QS808_Pack_struct qs808_rec_bag;

	// ͨ��ʧ��
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)			return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)					return QS808_DATA_ERR;
	else if (qs808_rec_bag.RET == ERR_INVALID_PARAM)	return ERR_INVALID_PARAM;
	else if (qs808_rec_bag.RET == ERR_TMPL_EMPTY)		return ERR_TMPL_EMPTY;
	else if (qs808_rec_bag.RET == ERR_SUCCESS)			return ERR_SUCCESS;
	return QS808_UNKNOW_ERR;
}

// ����Ѱ����ָָ��,ֻ���ͣ�������,��������
void QS808_Detect_Finger(void)
{
	QS808_Rec_Buf_refresh();
	int i;
	QS808_send_half_word(QS808_HEAD);//��ͷ
	QS808_send_byte(SID);
	QS808_send_byte(DID);
	QS808_send_half_word(CMD_FINGER_DETECT);//CMD
	QS808_send_half_word(0x0000);//LEN
	for (i=0;i<16;i++) {
		QS808_send_byte(0x00);
	}
	QS808_send_half_word(0x2001);//sum check
	QS808_Rec_Buf.Trans_state = set;

}

// ����Ѱ����ָ�������������
uint8_t QS808_Detect_Finger_Unpack(void) {
	QS808_Pack_struct qs808_rec_bag;

	// ͨ��ʧ��
	if(QS808_Unpack(&qs808_rec_bag) == 0xff)
		return QS808_DATA_ERR;
	else if(qs808_rec_bag.RCM == 0xff)
		return QS808_DATA_ERR;
	else if (qs808_rec_bag.DATA[0]==0)
		return ERR_FINGER_DETECT;
	else if(qs808_rec_bag.DATA[0]==1)
		return ERR_FINGER_DETECT;
	return QS808_UNKNOW_ERR;
}
