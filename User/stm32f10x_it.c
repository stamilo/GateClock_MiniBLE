/**
	******************************************************************************
	* @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
	* @author  MCD Application Team
	* @version V3.2.0
	* @date    03/01/2010
	* @brief   Main Interrupt Service Routines.
	*          This file provides template for all exceptions handler and
	*          peripherals interrupt service routine.
	******************************************************************************
	* @copy
	*
	* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
	* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
	* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
	* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
	* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
	* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
	*
	* <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
	*/

/* Includes ------------------------------------------------------------------*/

/** @addtogroup STM32F10x_StdPeriph_Template
	* @{
	*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
	* @brief   This function handles NMI exception.
	* @param  None
	* @retval None
	*/
#include "stm32f10x_it.h"

extern uint8_t WAKEUP_FLAG;
uint8_t WAKEUP_SOURCE = 0;	// 0��ʾָ�ƻ��ѣ�1��ʾ��������

extern u8 USART_Recv_Flag;
extern u8 USART_RecvBuf[USART_RECVBUF_LENGTH];
extern u8 USART1_RecvBuf_Length;

// qs808�ж�
void QS808_INT_EXT_IRQHandler(void) {
	// ����ʱ���ѣ���Ҫ������������
	if(WAKEUP_FLAG) {
		SystemInit();
		WAKEUP_FLAG=0;
		WAKEUP_SOURCE = 0;
		VCC_Adc_Init();
		Power_ctrl_on();
		BLE_WakeUp();
	}
	EXTI_ClearITPendingBit(QS808_INT_EXT_LINE);
}

// �����ж�
void TSM12_INT_EXT_IRQHandler(void) {
}

// �����ж�
void BLE_INT_EXT_IRQHandler(void)
{
	EXTI_ClearITPendingBit(BLE_INT_EXT_LINE);
}

void NMI_Handler(void)
{
}

/**
	* @brief  This function handles Hard Fault exception.
	* @param  None
	* @retval None
	*/
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles Memory Manage exception.
	* @param  None
	* @retval None
	*/
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles Bus Fault exception.
	* @param  None
	* @retval None
	*/
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles Usage Fault exception.
	* @param  None
	* @retval None
	*/
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
	* @brief  This function handles SVCall exception.
	* @param  None
	* @retval None
	*/
void SVC_Handler(void)
{
}

/**
	* @brief  This function handles Debug Monitor exception.
	* @param  None
	* @retval None
	*/
void DebugMon_Handler(void)
{
}

/**
	* @brief  This function handles PendSVC exception.
	* @param  None
	* @retval None
	*/
void PendSV_Handler(void)
{
}

/**
	* @brief  This function handles SysTick Handler.
	* @param  None
	* @retval None
	*/

void SysTick_Handler(void)
{
	// TimingDelay_Decrement();
}

// ���ݵ���flag ��flagΪ1����flash�е����ݴӴ��ڵ���
uint8_t debug_data_read_flag = 0;

// ����1�жϺ����������յ������ⳤ�ȵ����ݴ洢�� u8 USART_RecvBuf[12] ��
void BLE_USART_IRQHandler(void){
	// ���������� warrning
	u8 clear = clear;

	// ������յ���һ�ֽ�����
	if (USART_GetITStatus(BEL_USART,USART_IT_RXNE) != RESET) {
		// �洢һ�ֽ�����
		USART_RecvBuf[USART1_RecvBuf_Length++] = BEL_USART->DR;
	}

	// ������յ���һ֡����
	else if (USART_GetITStatus(BEL_USART,USART_IT_IDLE) != RESET) {
		// �ȶ�ȡ�������ݼĴ��� DR
		clear = BEL_USART->DR;
		// �ٶ�ȡ�������ݼĴ��� SR����ô���Ϳ������֡�жϱ�־λ
		clear = BEL_USART->SR;
		// ��ǽ��յ���һ֡����
		USART_Recv_Flag = 1;
	}
}
//void USART1_IRQHandler(void){

//}

extern QS808_Rec_Buf_type QS808_Rec_Buf;
uint8_t cnt=0;
void QS808_USART_RX_ISR(void) {
	uint8_t ucTemp;
	cnt++;
	if (USART_GetITStatus(QS808_USART,USART_IT_RXNE)!=RESET) {
		ucTemp = USART_ReceiveData( QS808_USART );

		// �հ�����ʱ�յ���ͷ���봦��
		if(QS808_Rec_Buf.Rec_state == idle){
			// �յ��˰�ͷ
			if(ucTemp == 0xaa){
				QS808_Rec_Buf.Rec_state = busy;
				QS808_Rec_Buf.Rec_Buf[QS808_Rec_Buf.Rec_point]=ucTemp;
				QS808_Rec_Buf.Rec_point++;
			}
		}
		// �հ�busyʱ �ձ�����ݰ�
		else{
			QS808_Rec_Buf.Rec_Buf[QS808_Rec_Buf.Rec_point]=ucTemp;
			QS808_Rec_Buf.Rec_point++;
		}
	}
}
