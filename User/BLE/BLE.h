#ifndef __BLE_H
#define __BLE_H
#include "stm32f10x.h"

// NRST �ܽ�
#define BLE_NRST_PORT				GPIOB
#define BLE_NRST_PORT_CLK			RCC_APB2Periph_GPIOB
#define BLE_NRST_PIN				GPIO_Pin_12
// PWRC �ܽ�
#define BLE_PWRC_PORT				GPIOB
#define BLE_PWRC_PORT_CLK			RCC_APB2Periph_GPIOB
#define BLE_PWRC_PIN				GPIO_Pin_13
// INT �ܽ�
#define BLE_INT_PORT				GPIOA
#define BLE_INT_PORT_CLK			RCC_APB2Periph_GPIOA
#define BLE_INT_PIN					GPIO_Pin_10

#define BLE_INT_EXT_LINE			EXTI_Line10
#define BLE_INT_PIN_SOURCE_PORT		GPIO_PortSourceGPIOA
#define BLE_INT_PIN_SOURCE_PIN		GPIO_PinSource10
#define BLE_INT_EXT_LINE			EXTI_Line10

#define AT_MODE_ON()				BLE_PWRC_LOW()
#define AT_MODE_OFF()				BLE_PWRC_HIGH()
#define BLE_RST_ON()				GPIO_ResetBits(BLE_NRST_PORT,BLE_NRST_PIN)
#define BLE_RST_OFF()				GPIO_SetBits(BLE_NRST_PORT,BLE_NRST_PIN)
#define BLE_PWRC_LOW()				GPIO_ResetBits(BLE_PWRC_PORT,BLE_PWRC_PIN)
#define BLE_PWRC_HIGH()				GPIO_SetBits(BLE_PWRC_PORT,BLE_PWRC_PIN)
#define BLE_INT_EXT_IRQHandler		EXTI9_5_IRQHandler
#define BLE_INT_IRQn				EXTI9_5_IRQn



#define cmdId_DataToServ			0x0001
// 0xFECF 0001 000E 0001 0000 0000
#define cmdId_DataToServAck			0x1001
#define cmdId_DataToLock			0x0002
#define cmdId_DataToLockAck			0x1002
#define cmdId_AddNotifToLock		0x0003
#define cmdId_AddNotifToLockAck		0x1003
#define cmdId_DelNotifToLock		0x0004
#define cmdId_DelNotifToLockAck		0x1004
#define cmdId_pushToServ			0x2001
#define cmdId_pushToLock			0x2002


#define errorCode0   0x0000   // success
#define errorCode1   0x0001   // �������ݰ�ʧ��
#define errorCode2   0x0002   // ����¼��   ģʽʧ��
#define errorCode3   0x0003   // ¼��ģʽ�³�ʱ��cmdId_pushToServ(0x2001)ʹ��
#define errorCode4   0x0004   // ¼��ģʽ����öָ�Ʋ�ƥ�䣬cmdId_pushToServ(0x2001)ʹ��
#define errorCode5   0x0005   // ָ��ɾ��ʧ�ܣ�cmdId_DataToLockAck(0x1002)ʹ��
#define errorCode6   0x0006   // ����ɾ��ģʽʧ�ܣ�cmdId_DelNotifToLockAck(0x1004)ʹ��
#define errorCode7   0x0007   // ¼���һ����ָ���ֻ��˵���˳�¼��
#define errorCode8   0x0008   // ����ɾ��ģʽ����ѡ��ɾ��ָ�ƽ����˳�
#define errorCode9   0x0500   // �ɼ���ʱ





void BLE_init(void);
void BLE_USART_SendStr(char* str);
void BLE_USART_CMD(char* cmd);
void BLE_Sleep(void);
void BLE_WakeUp(void);

#endif
