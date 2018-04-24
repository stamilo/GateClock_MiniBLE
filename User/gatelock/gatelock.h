#ifndef	__GATELOCK_H
#define	__GATELOCK_H

#include "stm32f10x.h"
#include "sys.h"
#include "./Delay/delay.h"

#define L9LL0S_A_GPIO_PORT                GPIOA
#define L9LL0S_A_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define L9LL0S_A_PIN                      GPIO_Pin_12
#define L9LL0S_B_GPIO_PORT                GPIOA
#define L9LL0S_B_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define L9LL0S_B_PIN                      GPIO_Pin_11


#define motor_clockwise()       GPIO_SetBits(L9LL0S_A_GPIO_PORT,L9LL0S_A_PIN);      GPIO_ResetBits(L9LL0S_B_GPIO_PORT,L9LL0S_B_PIN);
#define motor_anti_clockwise()  GPIO_SetBits(L9LL0S_B_GPIO_PORT,L9LL0S_B_PIN);      GPIO_ResetBits(L9LL0S_A_GPIO_PORT,L9LL0S_A_PIN);
#define motor_hold_on()         GPIO_ResetBits(L9LL0S_A_GPIO_PORT,L9LL0S_A_PIN);    GPIO_ResetBits(L9LL0S_B_GPIO_PORT,L9LL0S_B_PIN);

void Gate_Init(void);			//��������
void Gate_Unlock(void);			//���� IO ��ʼ��


#endif
