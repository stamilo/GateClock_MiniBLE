#ifndef __HISTORY_H
#define __HISTORY_H
#include "stm32f10x.h"
#include "./data/data_def.h"

void Write_History(u16 type, u16 id);
void SendHistory2Phone(void);
void SendOneHistoryStruct(USART_TypeDef* pUSARTx, STRUCT_HISTORY HIS);
void Erase_History(void);
void History_Pos_Init(void);

#endif
