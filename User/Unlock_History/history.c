#include "./Unlock_History/history.h"
#include "./rtc/rtc.h"
#include "./usart/debug_usart.h"
#include "./data/data_def.h"


//历史记录指针的位置
u16 History_Pos;

// 历史记录指针位置初始化
void History_Pos_Init(void) {
    STMFLASH_Read(HISTORY_POS_ADDR, &History_Pos, 1);
    // 如果是第一次全擦除烧录，就直接把 History_Pos = 0
    u16 temp=0;
    if (History_Pos==0xFFFF) {
        STMFLASH_Write(HISTORY_POS_ADDR, &temp, 1);
    }
}

// 更新历史记录指针位置
void History_Pos_Update(void) {
    STMFLASH_Write(HISTORY_POS_ADDR, &History_Pos, 1);
}

// 向 flash 中写入一条历史记录信息
// type:    所属的解锁类型，如密码（单独500个），或者指纹和射频卡（指纹和射频卡用的一套id，两个加起来一共500个）
// id:      编号
void Write_History(u16 type, u16 id) {

    // 赋值结构体
    STRUCT_HISTORY history;
    history.m_Flag = 0x6666;
    history.m_Type = type;
    history.m_Id = id;
    history.m_Year = calendar.w_year;
    history.m_MonthAndDay = calendar.w_month;   history.m_MonthAndDay <<= 8;
    history.m_MonthAndDay = history.m_MonthAndDay | calendar.w_date;
    history.m_HourAndMin = calendar.hour;       history.m_HourAndMin <<= 8;
    history.m_HourAndMin = history.m_HourAndMin | calendar.min;

    // 写入 flash
    STMFLASH_Write(HISTORY_ADDR_START +History_Pos*HISTORY_ONE_LENGTH, (u16*)&history, HISTORY_ONE_LENGTH/2);
    History_Pos = (History_Pos==(HISTORY_NUM-1)) ? 0:(History_Pos+1);
    History_Pos_Update();

}

// 清除所有的历史记录
void Erase_History(void) {
    u16 temp = 0xFFFF;
    for (u8 i=0; i<HISTORY_NUM; i++) {
        STMFLASH_Write(HISTORY_ADDR_START +i*HISTORY_ONE_LENGTH, &temp, 1);
    }
    History_Pos = 0;
    History_Pos_Update();
}

// 向手机端发送所有历史记录（最多发50条）
void SendHistory2Phone(void) {
    u8 len;
    u16 temp16;
	BleDataHead tempBLE;
    STRUCT_HISTORY tempHIS;

    /*********************** 先发一包长度信息（历史记录的数量） **********************/
    // 首先就是需要知道现在一共存了多少条历史记录，如果历史记录指针的位置没有数据，那就表明历史记录还未存满
    STMFLASH_Read(HISTORY_ADDR_START +History_Pos*HISTORY_ONE_LENGTH, &temp16, 1);
    if (temp16 == 0xFFFF) {
        if (History_Pos>=10)    tempBLE.m_totalLength = 14;    // 这个包的长度为14,因为历史记录是两位数
        else                    tempBLE.m_totalLength = 13;    // 这个包的长度为13,因为历史记录是一位数
        temp16 = (u16)(History_Pos/10 +0x30);
        temp16 <<= 8;
        temp16 = temp16 | (History_Pos%10+0x30);
    }
    else {
        tempBLE.m_totalLength = 14;    // 这个包的长度为14,因为历史记录是两位数
        temp16 = 0x3530;            // 历史记录有50条
    }
	tempBLE.m_magicCode = MAGICCODE;
	tempBLE.m_version = VERSION;
	tempBLE.m_cmdId = CMDID_CHECK_HISTIRY;
	tempBLE.m_seq = 0x0000;
	tempBLE.m_errorCode = 0x0000;
	// 发送包头
	for (u8 i=0; i<6; i++){
		pUsart_SentMessage(USART1, (u16*)&tempBLE+i);
	}
    // 发送包体，一共支持 50 条历史记录的查询
    pUsart_SentMessage(USART1, &temp16);



    delay_ms(1000);


    /************************* 再发len包历史记录的具体信息 ************************/
    // 获得历史记录的条数len(0~50条)
    len = temp16&0x00FF -0x30;
    len += ((temp16>>=8)-0x30) *10;

    // 如果len满50 那就先从(pos-1)递减发到0 再从49递减发到pos
    if (len==50) {
        // 先从(pos-1)递减发到0
        for (u16 i=History_Pos; i>0; i--) {
            // 读取历史记录至tempHIS临时结构体
            STMFLASH_Read(HISTORY_ADDR_START +(i-1)*HISTORY_ONE_LENGTH, (u16*)&tempHIS, HISTORY_ONE_LENGTH/2);
            // 发送包头
            tempBLE.m_magicCode = MAGICCODE;
            tempBLE.m_version = VERSION;
            tempBLE.m_totalLength = 27;    // 12+15
            tempBLE.m_cmdId = CMDID_CHECK_HISTIRY;
            tempBLE.m_seq = tempHIS.m_Type;  // 0001表示密码开门 0002表示指纹开门 0003表示门卡开门
            tempBLE.m_errorCode = 0x0000;
            for (u8 i=0; i<6; i++) {
                pUsart_SentMessage(USART1, (u16*)&tempBLE+i);
            }
            // 发送包体
            SendOneHistoryStruct(USART1, tempHIS);


            delay_ms(200);


        }
        // 再从49递减发到pos
        for (u16 i=50; i>History_Pos; i--) {
            // 读取历史记录至tempHIS临时结构体
            STMFLASH_Read(HISTORY_ADDR_START +(i-1)*HISTORY_ONE_LENGTH, (u16*)&tempHIS, HISTORY_ONE_LENGTH/2);
            // 发送包头
            tempBLE.m_magicCode = MAGICCODE;
            tempBLE.m_version = VERSION;
            tempBLE.m_totalLength = 27;     // 12+15
            tempBLE.m_cmdId = CMDID_CHECK_HISTIRY;
            tempBLE.m_seq = tempHIS.m_Type; // 0001表示密码开门 0002表示指纹开门 0003表示门卡开门
            tempBLE.m_errorCode = 0x0000;
            for (u8 i=0; i<6; i++) {
                pUsart_SentMessage(USART1, (u16*)&tempBLE+i);
            }
            // 发送包体
            SendOneHistoryStruct(USART1, tempHIS);


            delay_ms(200);


        }
    }
    // 如果len未满50 那就直接从0位置发到(pos-1)就行
    else {
        for (u16 i=0; i<History_Pos; i++) {
            // 读取历史记录至tempHIS临时结构体
            STMFLASH_Read(HISTORY_ADDR_START +i*HISTORY_ONE_LENGTH, (u16*)&tempHIS, HISTORY_ONE_LENGTH/2);
            // 发送包头
            tempBLE.m_magicCode = MAGICCODE;
            tempBLE.m_version = VERSION;
            tempBLE.m_totalLength = 27;     // 12+15
            tempBLE.m_cmdId = CMDID_CHECK_HISTIRY;
            tempBLE.m_seq = tempHIS.m_Type; // 0001表示密码开门 0002表示指纹开门 0003表示门卡开门
            tempBLE.m_errorCode = 0x0000;
            for (u8 i=0; i<6; i++) {
                pUsart_SentMessage(USART1, (u16*)&tempBLE+i);
            }
            // 发送包体
            SendOneHistoryStruct(USART1, tempHIS);


            delay_ms(200);


        }
    }
}

// 发送单条历史记录包体
void SendOneHistoryStruct(USART_TypeDef* pUSARTx, STRUCT_HISTORY HIS) {
    u16 temp16,shi,ge;

    // 发送包体的年份信息 如：2018 0x32303138
    temp16 = HIS.m_Year/1000 +0x30;                     temp16 <<= 8;
    temp16 = temp16 | (HIS.m_Year/100%10 +0x30);
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送2018中的20 0x3230
    temp16 = HIS.m_Year/10%10 +0x30;                    temp16 <<= 8;
    temp16 = temp16 | (HIS.m_Year%10 +0x30);
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送2018中的18 0x3138

    // 发送包体的月份和日期信息 如：0228 2月28号 0x30323238
    temp16 = HIS.m_MonthAndDay & 0XFF00;
    temp16 >>= 8;
    shi = temp16/10 +0x30;
    ge = temp16%10 +0x30;
    temp16 = shi;
    temp16 <<= 8;
    temp16 |= ge;
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送0228中的02 0x3032
    temp16 = HIS.m_MonthAndDay & 0X00FF;
    shi = temp16/10 +0x30;
    ge = temp16%10 +0x30;
    temp16 = shi;
    temp16 <<= 8;
    temp16 |= ge;
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送0228中的28 0x3238

    // 发送包体的小时和分钟信息 如：2359 23:59分 0x32333539
    temp16 = HIS.m_HourAndMin & 0XFF00;
    temp16 >>= 8;
    shi = temp16/10 +0x30;
    ge = temp16%10 +0x30;
    temp16 = shi;
    temp16 <<= 8;
    temp16 |= ge;
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送2359中的23 0x3233
    temp16 = HIS.m_HourAndMin & 0X00FF;
    shi = temp16/10 +0x30;
    ge = temp16%10 +0x30;
    temp16 = shi;
    temp16 <<= 8;
    temp16 |= ge;
    pUsart_SentMessage(pUSARTx, &temp16);    // 发送2359中的28 0x3539

    // 发送包体中的id信息 如：001 0x303031
    Usart_SendUserId(pUSARTx, HIS.m_Id);
}
