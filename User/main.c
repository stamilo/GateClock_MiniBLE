#include "./my_board.h"

extern u8 USART_Recv_Flag;
extern u8 USART_RecvBuf[USART_RECVBUF_LENGTH];
extern u8 USART1_RecvBuf_Length;
extern u8 WAKEUP_SOURCE;
extern QS808_Rec_Buf_type QS808_Rec_Buf;
extern _calendar_obj calendar;
extern u8 BLE_MAC[17];      //����mac��ַ

float Battery_quantity;
u8 SLEEP_MAX = 30;


int main(void) {
    delay_init();			// ��ϵͳʱ�ӡ���ʼ��
    RTC_Init();				// ��RTCʱ�ӡ���ʼ��
    debug_usart_init();		// �����ڡ���ʼ��
    BLE_init();				// ����������ʼ��
    Init_BLE_MAC();			// ������MAC����ַ��ʼ��
    QS808_Init();			// ��ָ�Ʋɼ�ͷ����ʼ��
    UT588C_init();			// ������оƬ����ʼ��
    Gate_Init();			// ��������е���ơ���ʼ��
    // Erase_History();        // ɾ��������ʷ��¼
	History_Pos_Init();     // ��ʷ��¼ָ��λ�ó�ʼ��


    // TIM3_Int_Init(99,7199);	// ��ʱ��3�����Ժ�������ʱ����,10KƵ�ʼ���,ÿ10msһ���ж�
    // power_ctrl_init();		// ����Դ���ơ���ʼ��
    // Power_ctrl_on();		// �򿪵�Դ����
    // delay_ms(100);
    // VCC_Adc_Init();			// ��ADC��ͨ����ʼ��
    // RTC_Set(2018, 4, 20, 9, 13, 0); // 52100366
	// QS808_CMD_DEL_ALL();	// ɾ��ָ��ͷ�е�ȫ��ָ������

    u16 temp_cmdid, temp_userid, temp_return, sleep_count=0;

    // ��ѭ��
    while(1) {
        // ˯�߼���++
        // sleep_count++;
        // // �ж��Ƿ�ý���˯��ģʽ
        // if (sleep_count>=SLEEP_MAX && QS808_Rec_Buf.Trans_state==reset) {
        //     sleep_count = 0;
        //     QS808_Rec_Buf_refresh();
        //     delay_ms(500);
        //     PWR_Standby_Mode();
        //     // �����ָ��ͷ����
        //     if(WAKEUP_SOURCE==0) {
        //         // �����⵽����ָ���£��Ϳ�ʼ���ָ����ȷ�ԣ�׼������
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
        //                 delay_ms(800);	// ��֤ʧ�ܵ���ʾʱ��
        //             }
        //         }
        //     }
        // }

        // ������յ������ݴ��룬˵���ֻ��˷�������Ϣ������Ҫ������Ϣ¼�����һ������
        if ( Usart_RecvOrder(USART1)==SYS_RECV_ORDER ) {
            // ���� temp_cmdid �����з�֧�ж�
            temp_cmdid = RecvBuf2Cmdid();
            switch( temp_cmdid ) {
                /************************* ���յ�������ʱ�䡿ָ�� *************************/
                case CMDID_SET_TIME:
                    sleep_count = 0;
                    SPEAK_DUDUDU();
                    Cogradient_Time();
                    break;

                /************************* ���յ������ָ�ơ�ָ�� *************************/
                case CMDID_ADD_FINGER:
                    sleep_count = 0;
                    // SPEAK_DUDUDU();
                    if(Add_Finger(&temp_userid) == ERROR_CODE_SUCCESS)
                        Usart_SendFinger_ADD_Success(USART3, 3, temp_userid);
                    // ��ʱһ��ʱ�䣬��ֱֹ�ӽ���ָ�ƽ�������
                    delay_ms(1000);
                    break;

                /************************* ���յ���ɾ��ָ�ơ�ָ�� *************************/
                case CMDID_DEL_FINGER:
                    sleep_count = 0;
                    SPEAK_DUDUDU();
                    temp_userid = RecvBuf2Userid();
                    if (Delete_Finger(temp_userid) == ERROR_CODE_SUCCESS)
                        Usart_SendFinger_DEL_Success(USART3);
                    else
                        Usart_SendFinger_DEL_Error(USART3);
                    break;

                /************************* ���յ���һ��������ָ�� *************************/
                case CMDID_OPEN_DOOR:
                    sleep_count = 0;
                    Usart_SendOpenDoor_Success(USART3);
                    Gate_Unlock();
                    SPEAK_OPEN_THE_DOOR();
                    // һ�㿪�����ǳɹ��ģ�Ҳû�а취��⿪���Ƿ�ɹ���������ֱ�ӷ��ؿ����ɹ�������
                    break;

                /*********************** ���յ����鿴��ʷ��¼��ָ�� ************************/
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

        // ���û�н��յ��ֻ��˷�������Ϣ���Ǿ�ʱ��׼������
        else {
            // �����⵽����ָ���£��Ϳ�ʼ���ָ����ȷ�ԣ�׼������
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
                // ��ֹ��ʱ�����ٴν���ָ�Ƽ��
                delay_ms(800);
            }
        }
    }
}
