#ifndef UT588C_H
#define UT588C_H
#include "stm32f10x.h"

//TG1
#define TG1_PORT GPIOB
#define TG1_GPIO_CLK      RCC_APB2Periph_GPIOB
#define TG1_PIN GPIO_Pin_7
//TG2
#define TG2_PORT GPIOB
#define TG2_GPIO_CLK      RCC_APB2Periph_GPIOB
#define TG2_PIN GPIO_Pin_6

#define	TG1 PBout(7)
#define	TG2 PBout(6)

// 开门提示音效
#define SPEAK_OPEN_THE_DOOR()		UT588C_speak(4)
// 开门失败提示音效
#define SPEAK_OPEN_THE_DOOR_FAIL()	UT588C_speak(10)
// 操作成功提示音
#define SPEAK_OPT_SUCCESS() 		UT588C_speak(4)
// 操作失败提示音
#define SPEAK_OPT_FAIL() 			UT588C_speak(10)
// 按键音效
#define SPEAK_DUDUDU() 				UT588C_speak(1)
// 请按手指
#define SPEAK_PRESS_FIGER()			UT588C_speak(1)
// 请按手指，刷卡，或输入密码
#define SPEAK_CARD_FIGER_OR_PW()	UT588C_speak(1)
// 请先录入一个管理员
#define SPEAK_NO_ADMIN_NOW() 		UT588C_speak(1)


// 不知名函数
#define SPEAK_VERIFY_ADMIN() 		UT588C_speak(1)
#define SPEAK_PRESS_FIGER_AG() 		UT588C_speak(1)
#define SPEAK_NUMBER_INPUT()		UT588C_speak(1)



void UT588C_init(void);
void UT588C_speak(uint8_t count);


#endif
