#ifndef __DATA_DEF_H
#define __DATA_DEF_H
#include "stm32f10x.h"

// 用户的数据结构(包括指纹和射频卡)
typedef struct{
    u16 m_USER_Number;	    // 编号，【000~499】
    u16 m_USER_Type;	    // 类型，【0x0001.指纹】【0x0002.射频卡】
    u32 m_USER_Data;	    // 数据，【将m_USER_Type的数据存储在这里】
}MY_USER;

// 密码的数据结构
typedef struct{
    u16 m_Password_ID;		// 编号，【000~499】
    u16 m_Type;				// 密码类型用，来判断准入时间，【0x0001 无时间限制】【0x0002 时间段】【0x0003 保姆】
    u32 m_Password;			// 密码数值
    u16 m_HourAndMin_Start;	// 准入开始时间(时分)(2359)
    u16 m_HourAndMin_End;	// 准入结束时间(时分)(2359)
    // 下面的已经废弃
    u16 m_Year_Start;		// 准入开始时间(年)(2018)
    u16 m_MonthAndDay_Start;// 准入开始时间(月日)(1111)
    u16 m_Year_End;			// 准入结束时间(年)(2018)
    u16 m_MonthAndDay_End;	// 准入结束时间(月日)(1111)
}STRUCT_PASSWORD;

// 历史记录的数据结构
typedef struct{
    u16 m_Flag;			    // 用来表示这块区域是否已经被使用了（0x6666表示已经使用了）
    u16 m_Type;             // 这条历史记录属于密码输入还是指纹和射频卡输入
    u16 m_Id;               // 这条历史记录的编号
    u16 m_Year;             // 这条历史记录的年份
    u16 m_MonthAndDay;      // 这条历史记录的月和日期
    u16 m_HourAndMin;       // 这条历史记录的小时和分钟
}STRUCT_HISTORY;

// BLE 与 手机通信数据包
typedef struct{
    u16 m_magicCode;	    // 0xFECF
    u16 m_version;		    // 0x0001
    u16 m_totalLength;	    // 12+
    u16 m_cmdId;		    // 单片机主要就看这个信息,来判断这条指令需要单片机做什么
    u16 m_seq;			    // None
    u16 m_errorCode;
}BleDataHead;


/********************************************** 用户参数 *************************************************/
#define MY_USER_LENGTH			8			// 一个用户数据结构的大小，通过 sizeof 得到长度位8个字节
#define MY_USER_MAX_NUM			512		    // 最多可以存储 500 个用户结构体（也就是指纹和射频卡一共可以存500个）
#define MY_USER_PASSWORD		0x0001		// 表示这个数据是密码
#define MY_USER_FINGER			0x0002		// 表示这个数据是指纹
#define MY_USER_RFCARD			0x0003		// 表示这个数据是射频卡
#define MY_PASSWORD_LENGTH		32			// 一个密码数据结构的大小，实际的大小为20字节，为了能整除1024，故32
#define MY_PASSWORD_MAX_NUM		96			// 最多可以存储 96 个密码
#define MY_PASSWORD_FREE		0x0001		// 表示这个密码没有时间限制
#define MY_PASSWORD_SECTION		0x0002		// 表示这个密码的时间限制为【时间段】
#define MY_PASSWORD_NURSE		0x0003		// 表示这个密码的时间限制位【保姆】


/******************************************** 蓝牙通信参数 ************************************************/
#define USART_RECVBUF_LENGTH	42			// 串口缓存池大小，12包头+6密码+2*12准入时间=12+6+24=42，这是最大情况
#define ERROR_CODE_SUCCESS		0x0000   	// 执行成功
#define ERROR_CODE_TIMEOUT		0x0500   	// 采集超时
#define ERROR_CODE_ERROR		0x0800   	// 异常
#define ERROR_CODE_DUPLICATION	0x0400   	// 信息重复
#define ERROR_CODE_FINGERBAD	0x0300   	// 指纹质量差

#define ERROR_CODE_SUCCESS_SHA	0x0001   	// SHA1开门执行成功


/******************************************** 数据包头参数 ************************************************/
#define MAGICCODE				0xFECF
#define VERSION					0x0001


/*********************************************** cmdID ***************************************************/
#define SYS_NO_ORDER			0x3333		// 表示没有接收到有效的包头
#define SYS_RECV_ORDER			0x6666		// 表示接收到了有效的包头
#define CMDID_ADD_FINGER		0x0301		// 添加指纹命令
#define CMDID_DEL_FINGER		0x0802		// 添加指纹命令
#define CMDID_ADD_RFCARD		0x0302		// 添加射频卡命令
#define CMDID_DEL_RFCARD		0x0803		// 删除射频卡命令
#define CMDID_ADD_PASSWORD		0x0104		// 添加密码命令
#define CMDID_DEL_PASSWORD		0x0804		// 删除密码命令
#define CMDID_OPEN_DOOR			0x0106		// 一键开锁命令
#define CMDID_SET_TIME			0x0201		// 同步时间命令
#define CMDID_CHECK_HISTIRY		0x0102		// 查看历史记录命令


/********************************************* 历史记录 ***************************************************/
#define HISTORY_ONE_LENGTH		12			// 一条历史记录占12个字节,也就是6个u16
#define HISTORY_NUM				50			// 一共可存储50条历史记录


/********************************************** 存储地址 *************************************************/
// 最好400一条,从flash的页首地址开始写,这样稳定,不会出bug
#define HISTORY_ADDR_START		0x0800DC00	// 历史记录存储首地址
#define HISTORY_POS_ADDR		0x0800DE80	// 存储历史记录指针的位置
#define PASSWORD_ADDR_START		0x0800E000	// 存储手机端发来的密码的编号的首地址,密码一共可以存储96条,共3072B
#define MY_USER_ADDR_START		0x0800EC00	// 用户结构体存储首地址,k指纹+射频卡一共可以存储512条,共4096B
#define MY_USER_ADDR_END		0x0800FFFF	// STM32 Flash 末地址




// 修改这些 记得在初始化函数加上数量初始化（这里都没有被用到，未来要删除这部分代码）
#define alluser_amount_addr		0x800fc00	// 保存所有用户个数
#define admin_amount_addr 		0x800fc02	// 保存管理员个数
#define family_amount_addr 		0x800fc04	// 家人用户
#define babysitter_amount_addr 	0x800fc06	// 保姆用户
#define speak_flag_addr 		0x800fc08	// 音频flag
#define UNLOCK_TIME_addr 		0x800fd00	// 这个结构体存在这个地址
#define UNLOCK_TIME_length 		0x04		// 结构体长度




u16 Ascii2Userid(u8* userid_ascii);
u16 RecvBuf2Cmdid(void);
u16 RecvBuf2Userid(void);
u16 Compare_2Buf(u8* a, u8* b, u8 len);
void Userid2Ascii(u16 userid, u8* userid_ascii);
void RecvBuf2TimeUnlock_SECTION(u16* result);
void Int2Char8(int newPassword, u8* result);



#endif
