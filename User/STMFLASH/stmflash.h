#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#define STM32_FLASH_SIZE 64             // ��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_BASE 0x08000000     // FLASH��ʼ��ַ
#define STM32_FLASH_END  0x0800FFFF     // STM32 FLASH �Ľ�����ַ


/********************************************** �û����� *************************************************/
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		// ��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		// ��ָ����ַ��ʼ����ָ�����ȵ�����


/********************************************** �ײ㺯�� *************************************************/
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);


#endif
