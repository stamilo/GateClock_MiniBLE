#ifndef __TIMER_H
#define __TIMER_H
#include "my_board.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"



void TIM3_Int_Init(u16 arr,u16 psc);

#endif
