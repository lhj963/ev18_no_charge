#ifndef __SYSPARAM_H__
#define __SYSPARAM_H__

#ifdef __cplusplus
extern "C" {
#endif



#define INIT_STORE_FLASH_FLAG     0x0AC2

#define THE_BEIAN_SUCCESS_FLAG 0x55

#define DBG_BPS_DEFAULT  	115200
#define MODEM_BPS_DEFAULT  	115200
#define GPS_BPS_DEFAULT  	9600
#define WIFI_BPS_DEFAULT  	115200
#define BT_BPS_DEFAULT  	115200
#define SENSOR_BPS_DEFAULT  115200
#define PERI_BPS_DEFAULT  	115200

#define REPORT_INV_DEFAULT6  1
#define REPORT_INV_DEFAULT5  15


#define APN_DEFAULT         				"CMNET"
#define APN_USERNAME_DEFAULT				"ROOT"
#define APN_PASSWORD_DEFAULT				"ROOT"



#define FTP_ADDR_DEFAULT 					"59.110.50.55"
#define FTP_PORT_DEFAULT 					21
#define FTP_USER_NAME_DEFAULT 				"gjbbgl"
#define FTP_PASSWORD_DEFAULT 				"fengyun@2019"
#define FTP_DOWNLOAD_FILE_NAME_DEFAULT 		"LF.BIN"


#define FTP_AUTO_UPDATE_CFG_FILE_DEFAULT    "CFG.LF"



//中车平台
#define	CN6_SERVER_ADDR_DEFAULT				"218.75.212.59"
#define	CN6_SERVER_PORT_DEFAULT				15001

//东风新能源平台
#define	CN6_SERVER_ADDR_DEFAULT1			"47.97.240.171"
#define	CN6_SERVER_PORT_DEFAULT1			0					//4002

//中车协议的第三链路 218.75.212.62 10002
#define	CN6_SERVER_ADDR_DEFAULT2			"218.75.212.62"
#define	CN6_SERVER_PORT_DEFAULT2			0


#define	CN5_SERVER_ADDR_DEFAULT				"server.natappfree.cc"
#define	CN5_SERVER_PORT_DEFAULT				0

//发送方的邮件服务器地址
#define	EMAIL_SERVER_ADDR_DEFAULT			"smtp.139.com"
#define	EMAIL_SERVER_PORT_DEFAULT			0

//发送方账号密码
#define EMAIL_SEND_USER_DEFAULT				"13902910201@139.com"
#define EMAIL_SEND_PSW_DEFAULT				"imxzq123"

//主电源低压报警阈值
#define MAIN_POWER_THRESHOLD_DEFAULT		9500

//CAN总线无数据进入休眠模式
#define M_CAN_NO_DATA_OUTTIME  		(5)					/* 2min的超时时间 */


#define MAX_VIN_LEN  17
#define MAX_SN_LEN   32

/* TCP多路connect ID,后续需要添加的ID，在这里添加，最大支持11个ID */
typedef enum _TcpConnectIdE
{
	E_TCP_CONN_CN6_ID = 0,						/* 第1路 */
	E_TCP_CONN_CN6_ID_2 = 1,					/* 第2路 */
	E_TCP_CONN_BJ5_ID = 2,						/* 第3路 */

	E_TCP_CONN_BK_ID = 3,						/* 第4路 */
	E_TCP_CONN_MAX_ID = 4,
} TcpConnectIdE;


/* socket连接方式 */
typedef enum _TcpSocketTypeE
{
	E_TCP_SOCKET_TCP,							/* TCP方式 */
	E_TCP_SOCKET_UDP,							/* UDP方式 */
} TcpSocketTypeE;


/* 协议方式 */
typedef enum _ProtocolTypeE
{
	E_PROTO_BJ5_TYPE = 0,					/* 京5协议 */
	E_PROTO_CN6_TYPE,						/* 国六协议 */
	E_PROTO_SHH_TYPE,						/* 上海重型柴油车 */
	E_PROTO_CN56_TYPE,						/* 国5-6协议，以国6为标准，不存在备案，防拆等逻辑，流水号在消息类型前面 */
	E_PROTO_HZ_TYPE,						/* 杭州地标协议 */
	E_PROTO_GB17691_TYPE = 5,				/* GB17691协议，以国6为标准，不存在备案，防拆等逻辑，流水号在第一组消息类型和消息体之间 */
	E_PROTO_JAC_CN6_TYPE,					/* 江淮国6协议，以国6标准，流水号在消息类型前面，但是每一组都有流水号 */
	E_PROTO_HJ_CN6_TYPE,					/* 江淮新的国6协议HJ，每一组数据中都有采集时间(发送时间 + 流水号 + 消息内容(消息标志 + 采集时间 + 消息内容)) + 签名信息  */
	E_PROTO_YT_BLG_CN6_TYPE = 8,			/* 宇通，北理工平台协议对接，每一组数据结构t(流水号 + 信息标志 + 消息体)  */
	E_PROTO_BACK_DOOR_TYPE, 				/* 远程管理协议  */
	E_PROTO_GDXC_HZ_TYPE,					/* 广东旭诚杭标，每一组数据中包含(1个obd数据，10个数据流，10个拓展) */

	E_PROTO_NJYWK_TANGSHA_J5_TYPE = 11,		/* 南京依维柯，唐山京五协议，每一组数据中都有采集时间(发送时间 + 流水号 + 消息内容(消息标志 + 采集时间 + 消息内容)) */
} ProtocolTypeE;


typedef enum tagEngineType
{
	EngineType_Compatible = 0,	//兼容所有的发动机数据
	EngineType_D78,				//D78
	EngineType_EV18,			//EV18

	EngineType_ZD30,
	EngineType_M9T,
	EngineType_YC,
	EngineType_QC,
	EngineType_CC,				//朝柴
	EngineType_WC,
	EngineType_KMS,

	EngineType_DFAC,
	EngineType_E11K,
	EngineType_EM26,
	EngineType_LXC,

	EngineType_MAX,
}EngineType;

typedef struct tagDate
{
	INT8U year;
	INT8U month;
	INT8U day;
	INT8U hour;
	INT8U min;
	INT8U sec;
}Date;

typedef enum tagUpdateResult
{
	UPDATE_OK = 0,
	UPDATE_FALSE
}UpdateResult;

typedef enum tagUpdateStatus
{
	UPDATE_STATUS_NO = 0x12,			//未升级
	UPDATE_STATUS_START = 0x34,			//启动
	UPDATE_STATUS_FILE_OK = 0x56,		//升级文件校验通过
	UPDATE_STATUS_UPDATE_OK = 0x78,		//升级完成 BOOT检测到UPDATE_STATUS_FILE_OK 这个状态 
										//就会升级APP 然后将状态置为UPDATE_STATUS_UPDATE_OK
	UPDATE_STATUS_UPDATE_FAIL = 0x90,	//升级失败
}UpdateStatus;

typedef struct tagUpdateInfo
{
	UpdateStatus eUpdateStatus;						//升级状态
	INT8U ucUpdateWay;						//升级方式(xmodem FTP TCP等方式)
	INT32U ulStartAddr;						//在FLASH中存放的起始地址
	INT32U ulEndAddr;						//结束地址
	INT32U ulTotalBytes;					//总大小
	INT16U usTotalPackCnt;						//总包数
	Date stStartTime;				//起始时间
	INT32U ulStartTime;				//起始时间
	Date stEndTime;					//结束时间
	INT32U ulEndTime;				//结束时间
	INT32U ulTakeTimes;				//FTP耗时多少
	INT8S szReportPhone[20];
	INT8S szServerAddr[32];
	INT16U usServerPort;
	INT8S szUpdateFileName[32];
}UpdateInfo;



#define MAX_SVR_LINK_CNT 8

typedef struct tagSysParam
{
	INT16U usInitFlag;			//参数初始化标志
	
	INT8S szServerAddr[MAX_SVR_LINK_CNT][32];		//服务器地址
	INT16U ausPort[MAX_SVR_LINK_CNT];				//服务器端口
	INT8U ucSocketType[MAX_SVR_LINK_CNT];							//SOCKET类型 TCP UDP
	ProtocolTypeE aeProtocolTypeE[MAX_SVR_LINK_CNT];

	INT8S szApn[32];
	INT8S szApnUsername[32];
	INT8S szApnPassword[32];

	unsigned int aeCanBaudRate[3];			//CAN1和CAN2 CAN3的波特率

	//车辆相关信息
	INT8U ucCN6_Ver;
	INT8U ucCN6_Way;

	INT8S szVIN[MAX_VIN_LEN + 1];
	INT8S szSN[MAX_SN_LEN + 1];

	INT8U BCDPhoneNo[6];				//BCD[6]终端手机号
	INT8U ucEncryptionMethod;			//加密方式

	INT16U usProviceID;					//省域ID
	INT16U usCityID;					//市域ID
	INT8U aucManufacturerID[5];			//制造商ID
	INT8U aucTerminalModel[8];			//终端型号 位数不足八位补空格
	INT8U aucTerminalID[7];				//终端ID
	INT8U ucLicensePlateColor;			//车牌颜色
	INT8S szLicensePlate[32];			//车牌号

	INT8S aucAuthenticationCode[41];	//鉴权码

	/* 主电源低压报警阈值 */
	INT16U usMainPowerThreshold;

	/* 超速报警阈值 */
	INT8U ucLimitSpeed;

	//EngineType stEngineType;		//发动机模式
	unsigned char ucCanChannel;
	
	INT8U sleepBaseAccCanFlag;					/* 休眠判断基于ACC - 0, 还是CAN - 1 */
	INT8U canNoDataOuttime;
	INT8U chipId[16+1];				/* 芯片ID */
	INT32U drive_cycle_cnt;			//驾驶循环数
	INT8U beian_flag;		//备案成功标志
	INT32U speed_canid;		//CAN速度ID
	INT32U milage_canid;	//CAN里程ID
	INT32U can_bag_id;
}SysParam;


//extern SysParam s_stUserParam;
//extern SysParam s_stUserParam;
//extern SysParam s_stYZUserParam;


extern INT8U get_can_test_mode(void);
extern void set_can_test_mode(INT8U val);
extern EngineType get_ryc_engine_type(void);
extern void set_ryc_engine_type(EngineType val);

extern void set_main_power_off_flag(unsigned char val);
extern unsigned char get_main_power_off_flag(void);
extern unsigned int get_can_list_tip(void *pout,unsigned int outlen);
extern void add_canid_in_list(unsigned int canid);
extern unsigned char is_in_catch_canid_list(unsigned int canid);

extern void set_dst_net_error_accumulate_time_tmp(INT32U val);
extern INT32U get_dst_net_error_accumulate_time_tmp(void);

extern unsigned char get_electric_car_class(void);
extern void set_electric_car_class(unsigned char val);

extern void *get_test_vin(void);
extern void set_test_vin(void *ptr,unsigned char len);


extern void set_rtc_time(unsigned char rtc_time[6]);
extern unsigned char *get_rtc_time_ptr(void);
extern unsigned int get_ms_mod(void);
extern void inc_ms_mod(void);

extern unsigned char get_collect_can_sw(void);
extern void set_collect_can_sw(unsigned char val);
extern unsigned char get_recv_send_can_flag(void);
extern void set_recv_send_can_flag(unsigned char val);

extern unsigned int get_can_rx_fps(unsigned char channel);
extern void stat_can_rx_fps(unsigned char channel);
extern void calc_can_rx_fps(unsigned char channel);

extern void set_net_link_status(unsigned char channel,unsigned char val);
extern unsigned char get_net_link_status(unsigned char channel);
extern unsigned char get_auto_vin_ok_flag(void);
extern void set_auto_vin_ok_flag(unsigned char val);
extern unsigned char get_can_data_arrive_flag(unsigned char channel);
extern void set_can_data_arrive_flag(unsigned char channel,unsigned char val);
extern unsigned char get_can_bps_match_ok_flag(unsigned char channel);
extern void set_can_bps_match_ok_flag(unsigned char channel,unsigned char val);


extern INT8U get_send_next_canfile_bag_flag(void);
extern void set_send_next_canfile_bag_flag(INT8U val);
extern INT8U get_five_hundred_ms_flag(void);
extern void set_five_hundred_ms_flag(INT8U val);

extern INT8U get_export_can_flag(void);
extern void set_export_can_flag(INT8U val);

extern INT8U get_plat_sw1(void);
extern void set_plat_sw1(INT8U val);
extern INT8U get_plat_sw2(void);
extern void set_plat_sw2(INT8U val);


extern INT8U get_rdy_sleep_flag(void);
extern void set_rdy_sleep_flag(INT8U val);


extern void set_export_data_flag(INT8U val);
extern INT8U get_export_data_flag(void);


extern INT8U get_test_step(void);
extern void set_test_step(INT8U val);


extern unsigned char get_chaichu_sos_flag(void);

extern void set_chaichu_sos_flag(unsigned char val);


extern unsigned char get_shoudong_login_flag(void);
extern void set_shoudong_login_flag(unsigned char val);

extern unsigned char get_sjz_beian_flag(void);

extern void set_sjz_beian_flag(unsigned char val);

extern unsigned char *get_pubkey_ptr(void);


extern INT16U GetSysParamSize(void);

extern void set_can_no_data_time(INT32U val);
extern INT32U get_can_no_data_time(void);

extern void set_acc_no_data_time(INT32U val);
extern INT32U get_acc_no_data_time(void);

extern void set_logout_flag(unsigned char channel,INT8U val);

extern INT8U get_logout_flag(unsigned char channel);

extern void LoadUserParam(void);
extern void UserParamDefault(void);
extern void SaveSysParam(void);



extern ProtocolTypeE get_plat_pro_type(unsigned char channel);
extern void set_plat_pro_type(unsigned char channel,ProtocolTypeE em);

extern unsigned int GetCan0Bps(void);

extern void SetCan0Bps(unsigned int canbps);

extern unsigned int GetCan1Bps(void);

extern void SetCan1Bps(unsigned int canbps);

extern unsigned int GetCan2Bps(void);

extern void SetCan2Bps(unsigned int canbps);


extern void SPSetApn(char *szApn);
extern void SPSetApnUsername(char *str);
extern void SPSetApnPassword(char *str);

extern char *SPGetApn(void);
extern char *SPGetApnUsername(void);
extern char *SPGetApnPassword(void);


extern INT8U GetSocketType(unsigned char idx);
extern INT8U ServerLinkIsTcp(unsigned char idx);

extern void SetSocketType(unsigned char idx,TcpSocketTypeE em);


extern void SetSN(void *szSN);
extern INT8S *GetSN(void);
extern void SetVIN(void *szVIN);
extern INT8S *GetVIN(void);
extern INT8U *GetEncryptionMethodPtr(void);
extern INT8U GetEncryptionMethod(void);

extern INT8U *GetBCDPhoneNo(void);
extern void SetBCDPhoneNo(INT8U bcd[6]);

extern INT8U *CN6_GetVerPtr(void);
extern INT8U *CN6_GetWayPtr(void);
extern void CN6_SetWay(INT8U ucWay);
extern void CN6_SetVer(INT8U ucVer);


extern INT8S *GetAuthenticationCode(void);
extern void SetAuthenticationCode(void *ptr);
extern INT8S *GetLicensePlate(void);
extern void SetLicensePlate(void *ptr);
extern INT8U *GetTerminalModel(void);
extern void SetTerminalModel(void *ptr);
extern INT8U *GetTerminalID(void);
extern void SetTerminalID(void *ptr);
extern void SetPlateColor(INT8U val);
extern INT8U GetPlateColor(void);
extern INT8U *GetManufacturerID(void);
extern void SetManufacturerID(void *ptr);
extern INT16U GetProviceID(void);
extern void SetProviceID(INT16U usID);
extern INT16U GetCityID(void);
extern void SetCityID(INT16U usID);

extern void SPSetServerPort(INT8U idx,INT16U usPort);
extern void SPSetServerAddr(INT8U idx,void *szAddr);
extern char *SPGetServerAddr(INT8U idx);
extern INT16U SPGetServerPort(INT8U idx);


extern INT16U GetMainPowerThreshold(void);
extern void SetMainPowerThreshold(INT16U val);

extern void SetLimitSpeed(INT8U val);

extern INT8U GetLimitSpeed(void);

extern EngineType get_engine_type(void);
extern void set_engine_type(EngineType st);
extern char *get_engine_type_str(EngineType st);

extern char *get_plat_pro_type_str(ProtocolTypeE st);

extern void SetCanChannel(INT8U val);
extern INT8U GetCanChannel(void);

extern unsigned int disp_sysparam(void *pOut,unsigned int outbuflen);


extern INT8U SysGetSleepBaseAccCanFlag(void);
extern void SysSetSleepBaseAccCanFlag(INT8U flag);

extern INT8U SysGetCanOuttime(void);
extern void SysSetCanOuttime(INT8U min);

/**************************************************************************
* 函  数: void SysGetChipId(char *chipId)
* 描  述: 获取芯片ID信息
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: void
**************************************************************************/
extern void SysGetChipId(void *chipId);


/**************************************************************************
* 函  数: void SysSetChipId(u8 *chipId, int len)
* 描  述: 设置芯片ID信息
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: void
**************************************************************************/
extern void SysSetChipId(void *chipId, int len);

extern char *get_chipid_ptr(void);

extern INT32U get_drive_cycle_cnt(void);

extern void set_drive_cycle_cnt(INT32U val);

extern INT8U is_beian_success(void);
extern void beian_success(void);

extern void set_can_speed_id(INT32U val);
extern INT32U get_can_speed_id(void);
extern void set_can_milage_id(INT32U val);
extern INT32U get_can_milage_id(void);

extern void set_can_bag_id(INT32U val);
extern INT32U get_can_bag_id(void);



extern void record_kaiji_time(INT8U year,INT8U month,INT8U day,INT8U hour,INT8U min,INT8U sec);
//最多记录最近6条开机记录
extern INT8U get_last_kaiji_time(INT8U idx,INT8U *year,INT8U *month,INT8U *day,INT8U *hour,INT8U *min,INT8U *sec);

extern void set_heart_beat_sw(INT8U val);
extern INT8U get_heart_beat_sw(void);
extern INT8U GetReportStatus(void);
extern void SetReportStatus(INT8U ucStatus);

extern INT8U *get_cur_date_ptr(void);
extern INT16U *get_gb_login_flow_ptr(void);
extern INT16U get_gb_login_flow_cnt(void);
extern void set_dst_chemen_switch(INT8U val);
extern void set_dst_chesuo_switch(INT8U val);
extern void set_dst_left_qian_chesuo_switch(INT8U val);
extern INT8U get_dst_chemen_switch(void);
extern INT8U get_dst_chesuo_switch(void);
extern INT8U get_dst_left_qian_chesuo_switch(void);


extern INT8U get_dst_fact_bind_status(void);
extern INT8U get_dst_fact_pro_ver_about_lock(void);
extern INT8U get_dst_fact_lock_status(void);
extern void set_dst_fact_bind_status(INT8U val);
extern void set_dst_fact_pro_ver_about_lock(INT8U val);
extern void set_dst_fact_lock_status(INT8U val);
extern INT8U get_dst_fact_kucunrong_limit_speed(void);
extern void set_dst_fact_kucunrong_limit_speed(INT8U val);
extern INT8U get_dst_fact_limit_speed_level(void);
extern void set_dst_fact_limit_speed_level(INT8U val);

extern void set_dst_net_error_accumulate_time(INT32U val);
extern INT32U get_dst_net_error_accumulate_time(void);
extern INT8U get_want_pro_ver_about_lock(void);
extern INT8U get_want_lock_status(void);
extern void set_want_pro_ver_about_lock(INT8U val);
extern void set_want_lock_status(INT8U val);
extern unsigned char get_lock_car_cmd_from_where(void);
extern void set_lock_car_cmd_from_where(unsigned char val);

extern unsigned char get_dst_no_auto_lock_when_meet_net_error_condition_flag(void);
extern void set_dst_no_auto_lock_when_meet_net_error_condition_flag(unsigned char val);
extern unsigned char get_dst_network_error_tip_flag(void);
extern void set_dst_network_error_tip_flag(unsigned char val);
extern unsigned char get_dst_network_error_flag(void);
extern void set_dst_network_error_flag(unsigned char val);
extern INT8U *get_dst_time_when_network_error(void);
extern void set_dst_time_when_network_error(INT8U t[6]);
extern INT32U get_dst_milage_when_network_error(void);
extern void set_dst_milage_when_network_error(INT32U val);
//备份锁车参数
extern void backup_dst_lock_car_param(void);
//恢复锁车参数
extern void resume_dst_lock_car_param(void);
extern INT8U get_dst_want_pro_ver_about_lock(void);
extern INT8U get_dst_want_lock_status(void);
extern void set_dst_want_pro_ver_about_lock(INT8U val);
extern void set_dst_want_lock_status(INT8U val);
extern INT8U get_dst_want_kucunrong_limit_speed(void);
extern void set_dst_want_kucunrong_limit_speed(INT8U val);
extern INT8U get_dst_want_limit_speed_level(void);
extern void set_dst_want_limit_speed_level(INT8U val);
extern INT8U get_dst_pay_tip_flag(void);
extern void set_dst_pay_tip_flag(INT8U val);
extern INT8U get_dst_keep_fit_tip_flag(void);
extern void set_dst_keep_fit_tip_flag(INT8U val);
extern INT8U get_dst_inspect_tip_flag(void);
extern void set_dst_inspect_tip_flag(INT8U val);
extern void set_dst_vin(void *pvin,unsigned int vinlen);
extern INT8U *get_dst_vin(void);


extern void clr_dst_vcu_ver(void);
extern void set_dst_vcu_ver(unsigned char idx,unsigned char ch);
extern INT8U *get_dst_vcu_ver_ptr(void);


extern void set_vcu_ver(unsigned char idx,unsigned char ch);
extern void set_mcu_ver(unsigned char idx,unsigned char ch);
extern void set_bms_ver(unsigned char idx,unsigned char ch);
extern INT8U *get_vcu_ver_ptr(void);
extern INT8U *get_mcu_ver_ptr(void);
extern INT8U *get_bms_ver_ptr(void);

extern void clr_bms_ver(void);
extern void clr_mcu_ver(void);
extern void clr_vcu_ver(void);

extern INT8U *get_sc_date(void);
extern void set_sc_date(INT8U dat[6]);


extern void load_param(void);
extern void save_param(void);
extern void easy_change_param_arrive(void *ptr,unsigned int len);
extern void save_easy_change_param(void);

extern void dst_start_test_net_error_lock(void);

#ifdef __cplusplus
}
#endif
#endif


