#ifndef __QS808_DRIVE
#define __QS808_DRIVE
#include "stm32f10x.h"
#include "./qs808/qs808_usart.h"
#include "./usart/debug_usart.h"
#include "./Delay/delay.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "my_board.h"
#include "stm32f10x_dma.h"
typedef struct//qs808 ���ջ������ṹ��ԭ��
{
	enum Trans_state_enum {reset,set} Trans_state;//����flag����qs808�������ݺ����set
	enum Rec_state_enum {idle,busy} Rec_state;//����flag����ʼ���ջذ������busy
	uint8_t Rec_point;//������ָ��
	uint8_t Rec_Buf[30];//������
}QS808_Rec_Buf_type;

typedef struct
{	uint16_t HEAD;
	uint8_t SID1;//����ͺ궨���е�SID�ظ�
	uint8_t DID1;
	uint16_t RCM;
	uint16_t LEN;
	uint16_t RET;
	uint8_t DATA[14];
	uint16_t CKS;
} QS808_Pack_struct;


//ָ����2B��������Ҫ�ȷ���8λ���ٷ���8λ����������ĺ궨�����⽫�ߵͷ���д�����Ա����ʱ
//�����ٿ��Ǹߵ�λ��ֱ�Ӱ��ȸߺ�͵�˳��
#define QS808_HEAD 						0x55AA
#define SID 							0x00
#define DID 							0x00
//����
#define	CMD_TEST_CONNECTION				0x0100		//���Ӳ���
#define CMD_FINGER_DETECT 				0x2100		//�����ָ
#define CMD_GET_IMAGE					0x2000      //�ɼ�ͼ��
#define CMD_GENERATE					0x6000		//��������
#define CMD_MERGE						0x6100		//�ϳ�ģ���������
#define	CMD_STORE_CHAR					0x4000		//ģ�����
#define	CMD_SEARCH						0x6300		//����ָ�ƿ�
#define CMD_GET_EMPTY_ID 				0x4500		//��ȡָ�ƿ������ע����׸���ַ
#define CMD_DEL_CHAR 		 			0x4400		//ɾ��ָ����Χ��ָ�ƿ�
#define CMD_GET_ENROLL_COUNT 			0x4800      //��ȡָ�Ƹ���
#define CMD_STANDBY 					0x0C00   	//�͹���

//������
#define ERR_SUCCESS						0x00		//û�д���
#define ERR_FP_NOT_DETECTED 			0x28		//�ɼ�����û����ָ
#define ERR_INVALID_BUFFER_ID			0x26		//buffer ID ����ȷ
#define ERR_BAD_QUALITY					0x19		//ָ����������
#define	ERR_GEN_COUNT					0x25		//ָ�ƺϳɸ�����Ч
#define ERR_MERGE_FAIL					0x1A		//templete�ϳ�ʧ��
#define ERR_INVALID_TMPL_NO				0x1D		//templete�����Ч
#define ERR_DUPLICATION_ID				0x18		//ָ���Ѿ�ע��
#define	ERR_ALL_TMPL_EMPTY				0x14		//��Χ��û��ָ�ƣ���֤ʱ��
#define ERR_INVALID_PARAM				0x22		//ʹ���˲���ȷ�Ĳ���
#define ERR_TMPL_EMPTY					0x12		//��Χ��û��ָ�ƣ����ʱ��
#define	ERR_IDENTIFY					0x11		//ָ�ƿ����޴���ָ
//�Զ����������
#define QS808_DATA_ERR            		0xf0
#define QS808_UNKNOW_ERR          		0xf1
#define QS808_WAIT_TIMEOUT				0xf2		//�ȴ�����ָ�Ƴ�ʱ
#define ERR_DEL_FAIL					0xf3		//ɾ��ʧ��
#define ERR_NO_FINGER_DETECT			0xf4		//û��ָ��
#define ERR_FINGER_DETECT				0xf5		//��ָ��
#define ERR_USART_REC_ERR    			0xf6
//����ԭ��
uint16_t QS808_send_byte(uint8_t byte);
uint16_t QS808_send_half_word(uint16_t half_word);
uint8_t QS808_RecByte(void);
void QS808_Init(void);
uint8_t QS808_TEST_CONNECTION(void);
uint8_t QS808_GET_IMAGE(void);
uint8_t QS808_CMD_GENERATE(uint16_t ram_id);
uint8_t QS808_CMD_MERGE(void);
uint8_t QS808_CMD_STORE_CHAR(uint16_t templete_num);
uint8_t QS808_CMD_GET_EMPTY_ID(uint8_t * ID);
uint8_t	QS808_CMD_DEL_ALL(void);
uint8_t	QS808_CMD_SEARCH(uint8_t * index);
uint8_t	QS808_CMD_DEL_NUM(void);
//���涼��ָ��ĺ�����ʵ��ʹ����������ĺ���
uint8_t QS808_Wait(uint32_t time);//����ʱ������ָ�ƶ�ȡ
uint8_t QS808_Login(uint8_t * ID);//¼��ָ�� �ɼ�3��ͼ�� �ϳ� �洢
uint8_t QS808_SEARCH(uint8_t * index);//ָ����֤
uint8_t QS808_CMD_FINGER_DETECT(void);//����Ƿ�����ָ
uint8_t	QS808_CMD_DEL_NUM2(uint8_t index);//����ָ��idɾ��ָ��
//void QS808_CMD_FINGER_DETECT_dma(uint32_t memaddr);
uint16_t QS808_CMD_GET_ENROLL_COUNT(void);
uint8_t QS808_STANDBY(void);
void QS808_Reset(void);
uint8_t QS808_Unpack(QS808_Pack_struct * qs808_rec_bag);
void QS808_Detect_Finger(void);
uint8_t QS808_Detect_Finger_Unpack(void);
void QS808_Rec_Buf_refresh(void);
#endif
