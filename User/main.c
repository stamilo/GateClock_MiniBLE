#include "./my_board.h"

extern u8 USART_Recv_Flag;
extern u8 USART_RecvBuf[USART_RECVBUF_LENGTH];
extern u8 USART1_RecvBuf_Length;
extern u8 WAKEUP_SOURCE;
extern QS808_Rec_Buf_type QS808_Rec_Buf;
extern _calendar_obj calendar;
extern u8 BLE_MAC[17];      //蓝牙mac地址

float Battery_quantity;
u8 SLEEP_MAX = 30;


int main(void) {
    delay_init();			// 【系统时钟】初始化
    RTC_Init();				// 【RTC时钟】初始化
    debug_usart_init();		// 【串口】初始化
    BLE_init();				// 【蓝牙】初始化
    Init_BLE_MAC();			// 【蓝牙MAC】地址初始化
    QS808_Init();			// 【指纹采集头】初始化
    UT588C_init();			// 【语音芯片】初始化
    Gate_Init();			// 【门锁机械控制】初始化
    // Erase_History();        // 删除所有历史记录
	History_Pos_Init();     // 历史记录指针位置初始化


    // TIM3_Int_Init(99,7199);	// 定时器3，测试函数运行时间用,10K频率计数,每10ms一个中断
    // power_ctrl_init();		// 【电源控制】初始化
    // Power_ctrl_on();		// 打开电源控制
    // delay_ms(100);
    // VCC_Adc_Init();			// 【ADC】通道初始化
    // RTC_Set(2018, 4, 20, 9, 13, 0); // 52100366
	// QS808_CMD_DEL_ALL();	// 删除指纹头中的全部指纹数据

    u16 temp_cmdid, temp_userid, temp_return, sleep_count=0;

    // 主循环
    while(1) {
        // 睡眠计数++
        // sleep_count++;
        // // 判断是否该进入睡眠模式
        // if (sleep_count>=SLEEP_MAX && QS808_Rec_Buf.Trans_state==reset) {
        //     sleep_count = 0;
        //     QS808_Rec_Buf_refresh();
        //     delay_ms(500);
        //     PWR_Standby_Mode();
        //     // 如果是指纹头唤醒
        //     if(WAKEUP_SOURCE==0) {
        //         // 如果检测到有手指按下，就开始检测指纹正确性，准备开门
        //         if (QS808_CMD_FINGER_DETECT()==ERR_FINGER_DETECT) {
        //             sleep_count = SLEEP_MAX;
        //             temp_return = Confirm_Finger(&temp_userid);
        //             if (temp_return==ERROR_CODE_SUCCESS) {
        //                 SPEAK_OPEN_THE_DOOR();
        //                 Gate_Unlock();
        //                 Write_History(MY_USER_FINGER, temp_userid);
        //             }
        //             else {
        //                 SPEAK_OPEN_THE_DOOR_FAIL();
        //                 delay_ms(800);	// 验证失败的显示时间
        //             }
        //         }
        //     }
        // }

        // 如果接收到了数据传入，说明手机端发来了信息，可能要进行信息录入或者一键开锁
        if ( Usart_RecvOrder(USART1)==SYS_RECV_ORDER ) {
            // 根据 temp_cmdid 来进行分支判断
            temp_cmdid = RecvBuf2Cmdid();
            switch( temp_cmdid ) {
                /************************* 接收到【更新时间】指令 *************************/
                case CMDID_SET_TIME:
                    sleep_count = 0;
                    SPEAK_DUDUDU();
                    Cogradient_Time();
                    break;

                /************************* 接收到【添加指纹】指令 *************************/
                case CMDID_ADD_FINGER:
                    sleep_count = 0;
                    // SPEAK_DUDUDU();
                    if(Add_Finger(&temp_userid) == ERROR_CODE_SUCCESS)
                        Usart_SendFinger_ADD_Success(USART3, 3, temp_userid);
                    // 延时一段时间，防止直接进入指纹解锁步骤
                    delay_ms(1000);
                    break;

                /************************* 接收到【删除指纹】指令 *************************/
                case CMDID_DEL_FINGER:
                    sleep_count = 0;
                    SPEAK_DUDUDU();
                    temp_userid = RecvBuf2Userid();
                    if (Delete_Finger(temp_userid) == ERROR_CODE_SUCCESS)
                        Usart_SendFinger_DEL_Success(USART3);
                    else
                        Usart_SendFinger_DEL_Error(USART3);
                    break;

                /************************* 接收到【一键开锁】指令 *************************/
                case CMDID_OPEN_DOOR:
                    sleep_count = 0;
                    Usart_SendOpenDoor_Success(USART3);
                    Gate_Unlock();
                    SPEAK_OPEN_THE_DOOR();
                    // 一般开锁都是成功的，也没有办法检测开锁是否成功。。所以直接返回开锁成功就是了
                    break;

                /*********************** 接收到【查看历史记录】指令 ************************/
                case CMDID_CHECK_HISTIRY:
                    sleep_count = 0;
                    SPEAK_DUDUDU();
                    SendHistory2Phone();
                    break;

                /************************************************************************/
                default:
                    break;
            }
        }

        // 如果没有接收到手机端发来的信息，那就时刻准备开锁
        else {
            // 如果检测到有手指按下，就开始检测指纹正确性，准备开门
            if (QS808_CMD_FINGER_DETECT()==ERR_FINGER_DETECT) {
                sleep_count = 0;
                temp_return = Confirm_Finger(&temp_userid);
                if (temp_return==ERROR_CODE_SUCCESS) {
                    SPEAK_OPEN_THE_DOOR();
                    Gate_Unlock();
                    Write_History(MY_USER_FINGER, temp_userid);
                }
                else {
                    SPEAK_OPEN_THE_DOOR_FAIL();
                }
                // 防止短时间内再次进入指纹检测
                delay_ms(800);
            }
        }
    }
}
