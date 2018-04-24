#ifndef __DATA_DEF_H
#define __DATA_DEF_H
#include "stm32f10x.h"

// �û������ݽṹ(����ָ�ƺ���Ƶ��)
typedef struct{
    u16 m_USER_Number;	    // ��ţ���000~499��
    u16 m_USER_Type;	    // ���ͣ���0x0001.ָ�ơ���0x0002.��Ƶ����
    u32 m_USER_Data;	    // ���ݣ�����m_USER_Type�����ݴ洢�����
}MY_USER;

// ��������ݽṹ
typedef struct{
    u16 m_Password_ID;		// ��ţ���000~499��
    u16 m_Type;				// ���������ã����ж�׼��ʱ�䣬��0x0001 ��ʱ�����ơ���0x0002 ʱ��Ρ���0x0003 ��ķ��
    u32 m_Password;			// ������ֵ
    u16 m_HourAndMin_Start;	// ׼�뿪ʼʱ��(ʱ��)(2359)
    u16 m_HourAndMin_End;	// ׼�����ʱ��(ʱ��)(2359)
    // ������Ѿ�����
    u16 m_Year_Start;		// ׼�뿪ʼʱ��(��)(2018)
    u16 m_MonthAndDay_Start;// ׼�뿪ʼʱ��(����)(1111)
    u16 m_Year_End;			// ׼�����ʱ��(��)(2018)
    u16 m_MonthAndDay_End;	// ׼�����ʱ��(����)(1111)
}STRUCT_PASSWORD;

// ��ʷ��¼�����ݽṹ
typedef struct{
    u16 m_Flag;			    // ������ʾ��������Ƿ��Ѿ���ʹ���ˣ�0x6666��ʾ�Ѿ�ʹ���ˣ�
    u16 m_Type;             // ������ʷ��¼�����������뻹��ָ�ƺ���Ƶ������
    u16 m_Id;               // ������ʷ��¼�ı��
    u16 m_Year;             // ������ʷ��¼�����
    u16 m_MonthAndDay;      // ������ʷ��¼���º�����
    u16 m_HourAndMin;       // ������ʷ��¼��Сʱ�ͷ���
}STRUCT_HISTORY;

// BLE �� �ֻ�ͨ�����ݰ�
typedef struct{
    u16 m_magicCode;	    // 0xFECF
    u16 m_version;		    // 0x0001
    u16 m_totalLength;	    // 12+
    u16 m_cmdId;		    // ��Ƭ����Ҫ�Ϳ������Ϣ,���ж�����ָ����Ҫ��Ƭ����ʲô
    u16 m_seq;			    // None
    u16 m_errorCode;
}BleDataHead;


/********************************************** �û����� *************************************************/
#define MY_USER_LENGTH			8			// һ���û����ݽṹ�Ĵ�С��ͨ�� sizeof �õ�����λ8���ֽ�
#define MY_USER_MAX_NUM			512		    // �����Դ洢 500 ���û��ṹ�壨Ҳ����ָ�ƺ���Ƶ��һ�����Դ�500����
#define MY_USER_PASSWORD		0x0001		// ��ʾ�������������
#define MY_USER_FINGER			0x0002		// ��ʾ���������ָ��
#define MY_USER_RFCARD			0x0003		// ��ʾ�����������Ƶ��
#define MY_PASSWORD_LENGTH		32			// һ���������ݽṹ�Ĵ�С��ʵ�ʵĴ�СΪ20�ֽڣ�Ϊ��������1024����32
#define MY_PASSWORD_MAX_NUM		96			// �����Դ洢 96 ������
#define MY_PASSWORD_FREE		0x0001		// ��ʾ�������û��ʱ������
#define MY_PASSWORD_SECTION		0x0002		// ��ʾ��������ʱ������Ϊ��ʱ��Ρ�
#define MY_PASSWORD_NURSE		0x0003		// ��ʾ��������ʱ������λ����ķ��


/******************************************** ����ͨ�Ų��� ************************************************/
#define USART_RECVBUF_LENGTH	42			// ���ڻ���ش�С��12��ͷ+6����+2*12׼��ʱ��=12+6+24=42������������
#define ERROR_CODE_SUCCESS		0x0000   	// ִ�гɹ�
#define ERROR_CODE_TIMEOUT		0x0500   	// �ɼ���ʱ
#define ERROR_CODE_ERROR		0x0800   	// �쳣
#define ERROR_CODE_DUPLICATION	0x0400   	// ��Ϣ�ظ�
#define ERROR_CODE_FINGERBAD	0x0300   	// ָ��������

#define ERROR_CODE_SUCCESS_SHA	0x0001   	// SHA1����ִ�гɹ�


/******************************************** ���ݰ�ͷ���� ************************************************/
#define MAGICCODE				0xFECF
#define VERSION					0x0001


/*********************************************** cmdID ***************************************************/
#define SYS_NO_ORDER			0x3333		// ��ʾû�н��յ���Ч�İ�ͷ
#define SYS_RECV_ORDER			0x6666		// ��ʾ���յ�����Ч�İ�ͷ
#define CMDID_ADD_FINGER		0x0301		// ���ָ������
#define CMDID_DEL_FINGER		0x0802		// ���ָ������
#define CMDID_ADD_RFCARD		0x0302		// �����Ƶ������
#define CMDID_DEL_RFCARD		0x0803		// ɾ����Ƶ������
#define CMDID_ADD_PASSWORD		0x0104		// �����������
#define CMDID_DEL_PASSWORD		0x0804		// ɾ����������
#define CMDID_OPEN_DOOR			0x0106		// һ����������
#define CMDID_SET_TIME			0x0201		// ͬ��ʱ������
#define CMDID_CHECK_HISTIRY		0x0102		// �鿴��ʷ��¼����


/********************************************* ��ʷ��¼ ***************************************************/
#define HISTORY_ONE_LENGTH		12			// һ����ʷ��¼ռ12���ֽ�,Ҳ����6��u16
#define HISTORY_NUM				50			// һ���ɴ洢50����ʷ��¼


/********************************************** �洢��ַ *************************************************/
// ���400һ��,��flash��ҳ�׵�ַ��ʼд,�����ȶ�,�����bug
#define HISTORY_ADDR_START		0x0800DC00	// ��ʷ��¼�洢�׵�ַ
#define HISTORY_POS_ADDR		0x0800DE80	// �洢��ʷ��¼ָ���λ��
#define PASSWORD_ADDR_START		0x0800E000	// �洢�ֻ��˷���������ı�ŵ��׵�ַ,����һ�����Դ洢96��,��3072B
#define MY_USER_ADDR_START		0x0800EC00	// �û��ṹ��洢�׵�ַ,kָ��+��Ƶ��һ�����Դ洢512��,��4096B
#define MY_USER_ADDR_END		0x0800FFFF	// STM32 Flash ĩ��ַ




// �޸���Щ �ǵ��ڳ�ʼ����������������ʼ�������ﶼû�б��õ���δ��Ҫɾ���ⲿ�ִ��룩
#define alluser_amount_addr		0x800fc00	// ���������û�����
#define admin_amount_addr 		0x800fc02	// �������Ա����
#define family_amount_addr 		0x800fc04	// �����û�
#define babysitter_amount_addr 	0x800fc06	// ��ķ�û�
#define speak_flag_addr 		0x800fc08	// ��Ƶflag
#define UNLOCK_TIME_addr 		0x800fd00	// ����ṹ����������ַ
#define UNLOCK_TIME_length 		0x04		// �ṹ�峤��




u16 Ascii2Userid(u8* userid_ascii);
u16 RecvBuf2Cmdid(void);
u16 RecvBuf2Userid(void);
u16 Compare_2Buf(u8* a, u8* b, u8 len);
void Userid2Ascii(u16 userid, u8* userid_ascii);
void RecvBuf2TimeUnlock_SECTION(u16* result);
void Int2Char8(int newPassword, u8* result);



#endif
