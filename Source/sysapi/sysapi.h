#ifndef __SYSAPI_H__
#define __SYSAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DRIVER_API_BASE_ADDR 0x00004034

#define DRIVER_API_START_ADDR (*((unsigned int *)DRIVER_API_BASE_ADDR))


typedef unsigned char INT8U;
typedef unsigned short INT16U;
typedef unsigned int INT32U;
typedef char INT8S;
typedef short INT16S;
typedef int INT32S;
typedef unsigned int OS_STK;

/* -------------------------------------API接口------------------------------------- */
#define LEVEL0		0
#define LEVEL1		1
#define LEVEL2		2
#define LEVEL3		3
#define LEVEL4		4
#define LEVEL5		5
#define LEVEL6		6
#define LEVEL7		7
#define LEVEL8		8
#define LEVEL9		9

#define LEVEL_GPS	(LEVEL9 + 1)	
#define LEVEL_GPRS	(LEVEL9 + 2)
#define LEVEL_CAN	(LEVEL9 + 3)

#define CAN_Id_Standard             ((unsigned int)0x00000000)  /*!< Standard Id */
#define CAN_Id_Extended             ((unsigned int)0x00000004)  /*!< Extended Id */

#define CAN_RTR_Data                ((unsigned int)0x00000000)  /*!< Data frame */
#define CAN_RTR_Remote              ((unsigned int)0x00000002)  /*!< Remote frame */


#define CAN_ID_STD                  CAN_Id_Standard           
#define CAN_ID_EXT                  CAN_Id_Extended
#define CAN_RTR_DATA                CAN_RTR_Data         
#define CAN_RTR_REMOTE              CAN_RTR_Remote


typedef struct tagOS_SEM
{
	void *pName;		//名称
	INT8U sem_cnt;	//数量
	INT32U timeout;	//超时时间
}OS_SEM;


/** 
  * @brief  CAN Tx message structure definition  
  */

typedef struct
{
  unsigned int StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  unsigned int ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  unsigned char IDE;     /*!< Specifies the type of identifier for the message that 
                        will be transmitted. This parameter can be a value 
                        of @ref CAN_identifier_type */

  unsigned char RTR;     /*!< Specifies the type of frame for the message that will 
                        be transmitted. This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  unsigned char DLC;     /*!< Specifies the length of the frame that will be 
                        transmitted. This parameter can be a value between 
                        0 to 8 */

  unsigned char Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
                        to 0xFF. */
} CanTxMsg;

/** 
  * @brief  CAN Rx message structure definition  
  */

typedef struct
{
  unsigned int StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  unsigned int ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  unsigned char IDE;     /*!< Specifies the type of identifier for the message that 
                        will be received. This parameter can be a value of 
                        @ref CAN_identifier_type */

  unsigned char RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  unsigned char DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

  unsigned char Data[8]; /*!< Contains the data to be received. It ranges from 0 to 
                        0xFF. */

  unsigned char FMI;     /*!< Specifies the index of the filter the message stored in 
                        the mailbox passes through. This parameter can be a 
                        value between 0 to 0xFF */
} CanRxMsg;

#define MAX_STAR_NUM 32
/* GPS天线 */
typedef enum _GpsAntStateE
{
	E_GPS_ANT_NORMAL = 0,				/* GPS天线正常 */
	E_GPS_ANT_SHORT,					/* GPS天线短路 */
	E_GPS_ANT_OPEN,						/* GPS天线开路 */
} GpsAntStateE;


#define MAX_LINK_CNT 6

typedef enum tagNetStatus
{
	NO_SIM = 0,
	SIM_READY,
	REGNET,
	REGNETOK,
	GETIP,
	GETIPOK,
	CONNECT_SERVER,
	CONNECT_SERVER_ING,
	CONNECT_SERVER_OK,
	CONNECT_SERVER_CONNECTED,

}NetStatus;

typedef enum tagWAKEUP_WAY
{
	WAKEUP_BY_CAN = 0x01,
	WAKEUP_BY_ACC = 0x02,

}WAKEUP_WAY;

typedef struct tagNineAxisValue
{
	//3轴加速度计
	INT32U acce_x;
	INT32U acce_y;
	INT32U acce_z;
	//3轴陀螺仪
	INT32U tly_x;
	INT32U tly_y;
	INT32U tly_z;

	//3轴磁力计
	INT32U clj_x;
	INT32U clj_y;
	INT32U clj_z;
}NineAxisValue;

#define EXTERNAL_STORAGE_RSV		0
#define EXTERNAL_STORAGE_SD			1
#define EXTERNAL_STORAGE_USB		2
typedef struct tagDRVFILE
{
	void *filename;
	void *pbuf;
	INT32U buflen;
	INT32U attri;			//0表示覆盖写 1表示追加
	INT32U seek_offset;		//对于写文件 该成员不用管
	INT32U want_read_len;	//对于写文件 该成员不用管
}DRVFILE;

typedef enum tagHOOK_MSG_TYPE
{
	HOOK_MSG_ONE_MS = 0,							// 1毫秒
	HOOK_MSG_100US, 								// 100us
	HOOK_MSG_10US, 									// 10us

	HOOK_MSG_CAN0_DATA_ARRIVE, 						// can0 msg arrive
	HOOK_MSG_CAN1_DATA_ARRIVE,						// can1 msg arrive
	HOOK_MSG_CAN2_DATA_ARRIVE,						// can2 msg arrive
	HOOK_MSG_CAN3_DATA_ARRIVE,						// can3 msg arrive


	HOOK_MSG_MAX,
}HOOK_MSG_TYPE;

typedef enum tagCALLBACK_MSG_TYPE
{
	CB_MSG_REBOOT_OK = 0,							//重启完成
	CB_MSG_OTA_SUCCESS,								//OTA成功
	CB_MSG_OTA_FALSE,								//OTA失败

	CB_MSG_KEY_MENU_DOWN,							//菜单键按下
	CB_MSG_KEY_OK_DOWN,								//确认键按下
	CB_MSG_KEY_CANCLE_DOWN,							//取消键按下
	CB_MSG_KEY_UP_DOWN,								//上翻键按下
	CB_MSG_KEY_DOWN_DOWN,							//下翻键按下

	CB_MSG_KEY_PRINT_CHANGAN,						//长按打印键

	CB_MSG_IC_CARD_INSERT,							//IC卡插入
	CB_MSG_IC_CARD_TAKEOUT,							//IC卡拔出

	CB_MSG_MAIN_POWER_OFF,							//主电源掉电

	CB_MSG_SOS_TRIGGER,								//紧急报警触发

	CB_MSG_SPEED_PULSE_ARRIVE,						//速度脉冲到达消息

	CB_MSG_SEC_INT_ARRIVE,							//秒中断到达

	CB_MSG_CEFAN_HAPPEN,							//发生侧翻
	CB_MSG_CEFAN_JIECHU,							//侧翻解除

	CB_MSG_PENGZHUANG_HAPPEN,						//发生碰撞
	CB_MSG_NINE_AXIS_VALUE,							//九轴加速度计

	CB_MSG_GPS_JS_OK,								//GPS校时成功

	CB_MSG_RING_IS_COMMING,							//电话来了
	CB_MSG_RING_IS_BEIGUADUAN,						//被挂断

	CB_MSG_NETWORK_JS_OK,							//网络校时完成
	CB_MSG_ZK_FILE_DOWNLOAD_OK,						//字库文件下载完成
	
	CB_MSG_KEY_OK_CHANGAN,							//长按确认键

	CB_MSG_READ_RECORD_FILE_FINISH,					//读取录音文件完成

	CB_MSG_CONNECT_SERVER_OK,
	CB_MSG_SERVER_DISCONNECT,
	CB_MSG_DBG_UART_DATA_ARRIVE,
	CB_MSG_SERVER_DATA_ARRIVE,	
	CB_MSG_LINUX_CMD_ACK_ARRIVE,
	CB_MSG_CACHE_DATA_ARRIVE,
	CB_MSG_EASY_CHANGE_PARAM_ARRIVE,
	CB_MSG_FILE_DATA_ARRIVE,
	CB_MSG_USER_MPU_MSG_ARRIVE,
	CB_MSG_SMS_ARRIVE,								//短信到达

	CB_MSG_MAX
}CALLBACK_MSG_TYPE;

typedef enum tagS_MSG_TYPE
{
	S_MSG_DIAL = 0,									//拨打电话
	S_MSG_SEND_SMS,									//发送短信
	S_MSG_GET_CPU_TYPE,								//获取CPU型号
	S_MSG_GET_MDM_TYPE,								//获取MODEM型号
	S_MSG_GET_GPS_TYPE,								//获取GPS模块型号

	S_MSG_JIETING_RING,								//接听来电
	S_MSG_GUADUAN_RING,								//挂断来电

	S_MSG_SET_SOUND_VOLUME,							//设置音量

	S_MSG_TRIGGER_BOHAOANJIAN_SOUND,				//拨号按键音
	S_MSG_TRIGGER_TONE,								//触发不同频率的声调

	S_MSG_GET_NETWORK_NAME,							//网络名称
	S_MSG_GPS_POWER_CTRL,							//GPS模块电源控制
	S_MSG_GPRS_POWER_CTRL,							//GPRS模块电源控制
	S_MSG_SCREEN_POWER_CTRL,						//屏幕背光电源控制
	S_MSG_PRINTER_POWER_CTRL,						//打印机电源控制
	S_MSG_CPU_INT_CTRL,								//CPU中断开关控制
	S_MSG_OIL_CTRL,									//油路控制
	S_MSG_ELEC_CTRL,								//电路控制
	S_MSG_DOOR_CTRL,								//门控制

	S_MSG_LOUDSPEAKER_POWER_CTRL,					//喇叭控制
	S_MSG_LISTEN_DIAL,								//监听

	//外部存储器相关的接口 外部存储器包括U盘 SD卡
	S_MSG_EXTERNAL_STORAGE_TOTAL_CAP,				//总容量
	S_MSG_EXTERNAL_STORAGE_USED, 					//已用空间
	S_MSG_EXTERNAL_STORAGE_FREE, 					//剩余空间

	S_MSG_EXTERNAL_STORAGE_WRITE_FILE, 				//写文件
	S_MSG_EXTERNAL_STORAGE_READ_FILE,				//读文件
	S_MSG_EXTERNAL_STORAGE_DEL_FILE,				//删除文件

	S_MSG_EXTERNAL_STORAGE_RSV1,					//保留接口
	S_MSG_EXTERNAL_STORAGE_RSV2,					//保留接口
	S_MSG_EXTERNAL_STORAGE_RSV3,					//保留接口
	S_MSG_EXTERNAL_STORAGE_RSV4,					//保留接口

	S_MSG_BEEP_CTRL,								//蜂鸣器控制
	S_MSG_CPU_CLOCK,								//cpu clock

	S_MSG_START_RECORD_INSIDE,						//开始录音
	S_MSG_STOP_RECORD_INSIDE,						//停止录音
	S_MSG_PLAY_RECORD_INSIDE,						//播放录音
	S_MSG_DEL_RECORD_FILE_INSIDE,					//删除内部录音文件
	S_MSG_READ_RECORD_FILE_INSIDE,					//读取内部录音文件
	S_MSG_IS_READ_RECORD_FILE_FINISH_INSIDE,		//当前读取的录音文件是否读取完成

	S_MSG_SYS_LED_CTRL,								//sys  led ctrl
	S_MSG_GPS_LED_CTRL,
	S_MSG_CAN_LED_CTRL,
	S_MSG_SEND_TO_SERVER,							//SEND TO SERVER

	S_MSG_GET_RTC_TIME,
	S_MSG_GET_LON,
	S_MSG_GET_LAT,
	S_MSG_GET_VIN,

	S_MSG_SEND_PARAM_TO_MPU,
	S_MSG_EXECUTE_DRV_CMD,

	S_MSG_GET_ICCID,
	S_MSG_GET_IMEI,
	S_MSG_GET_GPS_ANT_STATUS,
	S_MSG_GET_GPS_STAR_NUM,
	S_MSG_GET_SIM_CSQ,

	S_MSG_STORE_CAN_DATA,
	S_MSG_GET_MAIN_VOL,
	S_MSG_GET_BAT_VOL,
	S_MSG_GET_ACC_STATUS,

	S_MSG_GET_MPU_SOFTWARE_VERSION,		//MPU软件版本
	S_MSG_GET_GPS_SPEED,				//GPS速度
	S_MSG_GET_GAODU,					//高度
	S_MSG_GET_ANGLE,					//方向角

	S_MSG_GET_SERVER_ADDR,
	S_MSG_GET_SERVER_PORT,

	S_MSG_READ_PARAM,
	S_MSG_WRITE_PARAM,

	S_MSG_GET_GSENSOR_CHIP_STATUS,
	S_MSG_GET_FLASH_CHIP_STATUS,
	S_MSG_GET_EMMC_CHIP_STATUS,
	S_MSG_GET_JIAMI_CHIP_STATUS,

	S_MSG_ENTER_SLEEP_MODE,

	S_MSG_GET_GPS_MODULE_STATUS,
	S_MSG_GET_PUBLIC_KEY,
	S_MSG_STORE_CACHE_DATA,
	S_MSG_GET_CACHE_DATA,

	S_MSG_OUT_IO_CTRL,
	S_MSG_GET_JIAMI_CHIP_ID,			//获取加密芯片ID
	S_MSG_SET_JIAMI_CHIP_ID,			//设置加密芯片ID

	S_MSG_SET_SERVER_ADDR,
	S_MSG_SET_SERVER_PORT,
	S_MSG_SET_SERVER_IP_PORT,
	S_MSG_GET_BEIAN_MSG_BODY_WITH_SIGN,
	S_MSG_GET_POWER_SUPPLY_MODE,		//供电模式 0 主电源  1 电池供电
	S_MSG_DBG_DISP, 					//调试显示
	S_MSG_GET_GPS_LOCAL_STATUS,			//gps定位状态

	S_MSG_WRITE_EASY_CHANGE_PARAM,		//写易变化的参数
	S_MSG_SAVE_MCU_LOG,					//写MCU log
	S_MSG_GET_FILE_DATA,				//获取文件数据
	S_MSG_SET_STORE_CACHE_DATA_FLAG,	//设置存储CACHE数据的开关
	S_MSG_GET_HARDWARE_VERSION, 		//获取硬件版本号
	S_MSG_GET_WIFI_MODULE_STATUS,		//WIFI模块状态
	S_MSG_GET_CAN0_STATUS,				//can0 是否有数据	
	S_MSG_GET_CAN1_STATUS,				//can1 是否有数据
	S_MSG_GET_CAN2_STATUS,				//can2 是否有数据
	S_MSG_GET_GPS,
	S_MSG_GET_MPU_STATUS,
	S_MSG_REBOOT_TO_MPU,				//reboot pmu
	S_MSG_GET_TBOX_STATUS,
	S_MSG_MAX,
}S_MSG_TYPE;

typedef int (*_DBG_Print)(int iLevel,void *str,...);
typedef void (*_DBG_PrintBuf)(int iLevel,void *tip,void *pdat,unsigned int len);
typedef void (*_Set_DBG_Level)(int iLevel);
typedef int (*_Get_DBG_Level)(void);
typedef void (*_rsv)(void);

typedef int (*_os_create_thread)(INT8U thread_id,void (*thread_proc)(void),OS_STK *sp,OS_STK *bottom);
typedef void (*_os_sleep)(unsigned int t);
typedef int (*_os_create_sem)(OS_SEM *pSem,void *pName,INT8U cnt);
typedef int (*_os_sem_pend)(OS_SEM *pSem,INT32U timeout);
typedef int (*_os_sem_post)(OS_SEM *pSem);
typedef void * (*_os_malloc)(INT32U size);
typedef int (*_os_free)(void *ptr);

typedef void (*_can_init)(unsigned char channel,unsigned int bps);
typedef int (*_can_send_data)(unsigned char channel,CanTxMsg* CanData);


typedef void* (*_send_msg)(S_MSG_TYPE msgtype,void *param,void *lparam,void *wparam);
typedef void (*_install_hook_msg)(HOOK_MSG_TYPE msgtype,void *);
typedef void (*_reg_callback_msg)(CALLBACK_MSG_TYPE msgtype,void *);
typedef unsigned long (*_GetRunTime)(void);

#define __lffunc__(typ,addr) ((typ)(*((unsigned int *)(addr))))


#define DBG_Print								__lffunc__(_DBG_Print,DRIVER_API_START_ADDR)


/**
函数功能:以16进制形式将缓冲区的内容打印出来
入口参数:
	iLevel:打印等级
	tip:提示语
	pdat:要打印的缓冲区指针
	len:要打印的数据长度
出口参数:
	无
返回值:
	无
例如:
	假定 buf[3] = {0x11,0x22,0x33};
	DBG_PrintBuf(LEVEL0,"Read Hex:\r\n",buf,3);
	执行结果:
	Read Hex:
	11 22 33
*/
extern void DBG_PrintBuf(int iLevel,void *tip,void *pdat,unsigned int len);


/**
函数功能:设置打印等级
入口参数:
	iLevel:打印等级
出口参数:
	无
返回值:
	无
例如:
*/
extern void Set_DBG_Level(int iLevel);


/**
函数功能:获取当前打印等级
入口参数:
	无
出口参数:
	无
返回值:
	当前打印等级
例如:
*/
extern int Get_DBG_Level(void);


/**
函数功能:创建一个任务
入口参数:
	thread_id:线程ID
	thread_proc:线程处理函数
	sp:该线程的堆栈栈顶
	bottom:该线程的栈底
出口参数:
	无
返回值:
	0成功  非0失败
例如:
*/
extern int os_create_thread(INT8U thread_id,void (*thread_proc)(void),OS_STK *sp,OS_STK *bottom);

/**
函数功能:延时函数
入口参数:
	t:延时时长 ms
出口参数:
	无
返回值:
	无
例如:
*/
extern void os_sleep(unsigned int t);

/**
函数功能:创建信号量
入口参数:
	pSem:信号量指针
	pName:信号量名称
	cnt:信号量初始值
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_create_sem(&sem1,"test",1);
*/
extern int os_create_sem(OS_SEM *pSem,void *pName,INT8U cnt);

/**
函数功能:等待信号量
入口参数:
	pSem:信号量指针
	timeout:超时时间  0表示死等
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_sem_pend(&sem1,0);
*/
extern int os_sem_pend(OS_SEM *pSem,INT32U timeout);

/**
函数功能:释放信号量
入口参数:
	pSem:信号量指针
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_sem_post(&sem1);
*/
extern int os_sem_post(OS_SEM *pSem);

extern void *os_malloc(INT32U size);

extern int os_free(void *ptr);


/**
函数功能:can初始化
入口参数:
	channel:第几路CAN  0,1,2等
	bps:CAN速率  125 250 500 1000等
出口参数:
	无
返回值:
	无
例如:
*/
extern void can_init(unsigned char channel,unsigned int bps);


/**
函数功能:发送CAN消息
入口参数:
	channel:第几路CAN
	CanData:CanTxMsg结构的指针
出口参数:
	无
返回值:
	0表示发送成功 非零表示发送失败
例如:
*/
extern int can_send_data(unsigned char channel,CanTxMsg* CanData);


/**
函数功能:发送消息给驱动
入口参数:
	msgtype:消息类型
	lparam:参数1
	wparam:参数2
出口参数:
	无
返回值:根据不同的消息类型 返回值视情况而定

说明:例如
	拨打电话:
	send_msg(S_MSG_DIAL,"13902910201",0);

	发送短信
	send_msg(S_MSG_SEND_SMS,"13902910201","hello world");

	//获取CPU型号
	char *cpu_type = send_msg(S_MSG_GET_CPU_TYPE,0,0);
	DBG_Print(0,"CPU:%s\r\n",cpu_type);
*/
extern void *send_msg(S_MSG_TYPE msgtype,void *param,void *lparam,void *wparam);

//安装消息钩子函数
extern void install_hook_msg(HOOK_MSG_TYPE msgtype,void *pf);

//注册回调消息
extern void reg_callback_msg(CALLBACK_MSG_TYPE msgtype,void *pf);

/**
函数功能:获取系统运行时间 秒为单位
入口参数:
	无
出口参数:
	无
返回值:
	系统运行时间
例如:
*/
extern unsigned long GetRunTime(void);



#ifdef __cplusplus
}
#endif

#endif




