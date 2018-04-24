#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H
#include "stm32f10x.h"

// 串口等待时间
#define UART_WAIT_TIME                     0xfffff

/********************************************** 引脚定义 *************************************************/
#define BEL_USART                             USART3
#define BEL_USART_CLK                         RCC_APB1Periph_USART3
#define BEL_USART_BAUDRATE                    115200

#define BEL_USART_RX_GPIO_PORT                GPIOB
#define BEL_USART_RX_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define BEL_USART_RX_PIN                      GPIO_Pin_11
//#define UART_1_RX_AF                       GPIO_AF_USART1
//#define UART_1_RX_SOURCE                   GPIO_PinSource11

#define BEL_USART_TX_GPIO_PORT                GPIOB
#define BEL_USART_TX_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define BEL_USART_TX_PIN                      GPIO_Pin_10
//#define UART_1_TX_AF                       GPIO_AF_USART1
//#define UART_1_TX_SOURCE                   GPIO_PinSource10



//中断
#define BLE_USART_IRQHandler USART3_IRQHandler
#define BLE_USART_IRQn			 USART3_IRQn

/********************************************** 用户函数 *************************************************/
u16 Usart_RecvOrder(USART_TypeDef* pUSARTx);
void Usart_SendUserId(USART_TypeDef* pUSARTx, u16 user_id);

void Usart_SendRFCard_ADD_Success(USART_TypeDef* pUSARTx, u16 user_id);
void Usart_SendRFCard_ADD_Error(USART_TypeDef* pUSARTx, u16 errorcode);
void Usart_SendRFCard_DEL_Success(USART_TypeDef* pUSARTx);
void Usart_SendRFCard_DEL_Error(USART_TypeDef* pUSARTx);

void Usart_SendFinger_ADD_Success(USART_TypeDef* pUSARTx, u16 seq, u16 user_id);
void Usart_SendFinger_ADD_Error(USART_TypeDef* pUSARTx, u16 seq, u16 errorcode);
void Usart_SendFinger_DEL_Success(USART_TypeDef* pUSARTx);
void Usart_SendFinger_DEL_Error(USART_TypeDef* pUSARTx);

void Usart_SendPassword_ADD_Success(USART_TypeDef* pUSARTx, u16 user_id);
void Usart_SendPassword_ADD_Error(USART_TypeDef* pUSARTx, u16 errorcode);
void Usart_SendPassword_DEL_Success(USART_TypeDef* pUSARTx);
void Usart_SendPassword_DEL_Error(USART_TypeDef* pUSARTx);

void Usart_SendOpenDoor_Success(USART_TypeDef* pUSARTx);



/********************************************** 底层函数 *************************************************/
void pUsart_SendByte( USART_TypeDef* pUSARTx, u8* ch );
void pUsart_SentMessage( USART_TypeDef* pUSARTx, u16* message);
uint16_t Usart_SendByte( USART_TypeDef* pUSARTx, u8 ch );
uint16_t Usart_RecvByte( USART_TypeDef* pUSARTx);
void debug_usart_init(void);
void Init_BLE_MAC(void);
void Cogradient_Time(void);

#endif
