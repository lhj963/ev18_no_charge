#include "SysParam.h"


SysParam s_stUserParam = {0};
//SysParam s_stYZUserParam = {0};

#define CATCH_CAN_CNT 			10

typedef struct tagQJBL
{
	INT8U logout_flag[6];
	INT8U ucHeartBeatSw : 1; 			//心跳开关
	INT8U ucReportStatusSw : 1;
	INT32U can_no_data_time;
	INT32U acc_no_data_time;
	unsigned char pubkey[128];
	unsigned char sjz_beian_flag : 1;		//手动备案
	unsigned char shoudong_login_flag : 1;	//手动登录
	unsigned char chaichu_sos_flag : 1;	//终端拆除报警
	unsigned char moni_can_data_flag : 1;		//模拟CAN总线数据

	INT8U test_step;		//测试

	INT8U export_data_flag : 1;		//导数据标志
	INT8U enter_sleep_status : 1;		//进入休眠状态
	INT8U rdy_sleep_flag : 1;			//准备休眠标志

	INT8U plat_sw1 : 1;		//第2路链路开关
	INT8U plat_sw2 : 1;		//第3路链路开关
	INT8U export_can_flag : 1;
	INT8U send_next_canfile_bag_flag : 1;

	INT8U five_hundred_ms_flag : 1;		//500毫秒标志
	INT8U main_power_off_flag : 1;
	INT8U can_test_mode;

	unsigned char can_bps_match_ok_flag[3];
	unsigned char can_data_arrive_flag[3];
	unsigned char auto_vin_ok_flag;

	unsigned short net_link_status;		//ervery bit means one link

	unsigned int _can_rx_fps[3];
	unsigned int can_rx_fps[3];

	unsigned char collect_can_sw : 1;	//采集CAN数据开关
	unsigned char recv_send_can_flag : 1;	//接收发送的CAN数据  将发送的CAN数据也放入CAN接收缓冲区中一并采集

	unsigned int ms_mod;
	unsigned char rtc_time[6];

	unsigned char test_vin[17 + 1];

	unsigned char electric_car_class;
	INT32U dst_net_error_accumulate_time_tmp;
	unsigned int canidlist[CATCH_CAN_CNT];
	unsigned int catch_can_cnt;

	EngineType stRYC_EngineType;		//燃油车发动机模式
}QJBL;

static QJBL s_stQJBL = {0};

INT8U get_can_test_mode(void)
{
	return s_stQJBL.can_test_mode;
}

void set_can_test_mode(INT8U val)
{
	s_stQJBL.can_test_mode = val;
}


//燃油车类型
EngineType get_ryc_engine_type(void)
{
	return s_stQJBL.stRYC_EngineType;
}

void set_ryc_engine_type(EngineType val)
{
	s_stQJBL.stRYC_EngineType = val;
}

void set_main_power_off_flag(unsigned char val)
{
	s_stQJBL.main_power_off_flag = val;
}

unsigned char get_main_power_off_flag(void)
{
	return s_stQJBL.main_power_off_flag;
}


unsigned int get_can_list_tip(void *pout,unsigned int outlen)
{
	unsigned int len = 0;

	if (pout && outlen)
	{
		unsigned int i = 0;

		for (i = 0; i < s_stQJBL.catch_can_cnt; i++)
		{
			if ((len + 9) > outlen)
			{
				break;
			}

			if (s_stQJBL.canidlist[i] > 0xFFFF)
			{
				len += sprintf((char *)pout + len,"%08X,",s_stQJBL.canidlist[i]);
			}
			else
			{
				len += sprintf((char *)pout + len,"%04X,",s_stQJBL.canidlist[i]);
			}
		}
	}
	return len;
}


void add_canid_in_list(unsigned int canid)
{
	unsigned int idx = s_stQJBL.catch_can_cnt;

	if (idx >= CATCH_CAN_CNT)
	{
		idx = 0;
	}
	s_stQJBL.canidlist[idx] = canid;

	if (s_stQJBL.catch_can_cnt < CATCH_CAN_CNT)
	{
		s_stQJBL.catch_can_cnt++;
	}
}

unsigned char is_in_catch_canid_list(unsigned int canid)
{
	unsigned int i = 0;

	if (s_stQJBL.catch_can_cnt == 0)
	{
		return 1;
	}

	for (i = 0; i < CATCH_CAN_CNT; i++)
	{
		if (s_stQJBL.canidlist[i] == canid)
		{
			return 1;
		}
	}
	return 0;
}

void set_dst_net_error_accumulate_time_tmp(INT32U val)
{
	s_stQJBL.dst_net_error_accumulate_time_tmp = val;
}

INT32U get_dst_net_error_accumulate_time_tmp(void)
{
	return s_stQJBL.dst_net_error_accumulate_time_tmp;
}


unsigned char get_electric_car_class(void)
{
	return s_stQJBL.electric_car_class;
}

void set_electric_car_class(unsigned char val)
{
	s_stQJBL.electric_car_class = val;
}

void *get_test_vin(void)
{
	return (void *)(s_stQJBL.test_vin);
}

void set_test_vin(void *ptr,unsigned char len)
{
	memset(s_stQJBL.test_vin,0,sizeof(s_stQJBL.test_vin));
	if (ptr && len)
	{
		memcpy(s_stQJBL.test_vin,ptr,(len > 17) ? 17 : len);
	}
}

void set_rtc_time(unsigned char rtc_time[6])
{
	if (rtc_time) memcpy(s_stQJBL.rtc_time,rtc_time,6);
}

unsigned char *get_rtc_time_ptr(void)
{
	return s_stQJBL.rtc_time;
}

unsigned int get_ms_mod(void)
{
	return s_stQJBL.ms_mod;
}

void inc_ms_mod(void)
{
	s_stQJBL.ms_mod++;
	if (s_stQJBL.ms_mod >= 1000)
	{
		s_stQJBL.ms_mod = 0;
	}
}

unsigned char get_collect_can_sw(void)
{
	return s_stQJBL.collect_can_sw;
}

void set_collect_can_sw(unsigned char val)
{
	s_stQJBL.collect_can_sw = val;
}

unsigned char get_recv_send_can_flag(void)
{
	return s_stQJBL.recv_send_can_flag;
}

void set_recv_send_can_flag(unsigned char val)
{
	s_stQJBL.recv_send_can_flag = val;
}

unsigned int get_can_rx_fps(unsigned char channel)
{
	if (channel >= 3)
	{
		return 0;
	}
	return s_stQJBL.can_rx_fps[channel];
}

void stat_can_rx_fps(unsigned char channel)
{
	if (channel >= 3)
	{
		return ;
	}

	s_stQJBL._can_rx_fps[channel] = s_stQJBL._can_rx_fps[channel] + 1;
}

void calc_can_rx_fps(unsigned char channel)
{
	if (channel >= 3)
	{
		return ;
	}
	s_stQJBL.can_rx_fps[channel] = s_stQJBL._can_rx_fps[channel];
	s_stQJBL._can_rx_fps[channel] = 0;
}


void set_net_link_status(unsigned char channel,unsigned char val)
{
	if (val)
	{
		s_stQJBL.net_link_status |= (1 << channel);
	}
	else
	{
		s_stQJBL.net_link_status &= (~(1 << channel));
	}
}

unsigned char get_net_link_status(unsigned char channel)
{
	if (s_stQJBL.net_link_status & (1 << channel))
	{
		return 1;
	}
	return 0;
}

unsigned char get_auto_vin_ok_flag(void)
{
	return s_stQJBL.auto_vin_ok_flag;
}

void set_auto_vin_ok_flag(unsigned char val)
{
	s_stQJBL.auto_vin_ok_flag = val;
}

unsigned char get_can_data_arrive_flag(unsigned char channel)
{
	if (channel >= (sizeof(s_stQJBL.can_data_arrive_flag) / sizeof(s_stQJBL.can_data_arrive_flag[0])))
	{
		return 0;
	}
	return s_stQJBL.can_data_arrive_flag[channel];
}

void set_can_data_arrive_flag(unsigned char channel,unsigned char val)
{
	if (channel >= (sizeof(s_stQJBL.can_data_arrive_flag) / sizeof(s_stQJBL.can_data_arrive_flag[0])))
	{
		return ;
	}
	s_stQJBL.can_data_arrive_flag[channel] = val;
}

unsigned char get_can_bps_match_ok_flag(unsigned char channel)
{
	if (channel >= (sizeof(s_stQJBL.can_bps_match_ok_flag) / sizeof(s_stQJBL.can_bps_match_ok_flag[0])))
	{
		return 0;
	}
	return s_stQJBL.can_bps_match_ok_flag[channel];
}


void set_can_bps_match_ok_flag(unsigned char channel,unsigned char val)
{
	if (channel >= (sizeof(s_stQJBL.can_bps_match_ok_flag) / sizeof(s_stQJBL.can_bps_match_ok_flag[0])))
	{
		return ;
	}
	s_stQJBL.can_bps_match_ok_flag[channel] = val;
}


INT8U get_send_next_canfile_bag_flag(void)
{
	return s_stQJBL.send_next_canfile_bag_flag;
}

void set_send_next_canfile_bag_flag(INT8U val)
{
	s_stQJBL.send_next_canfile_bag_flag = val;
}

INT8U get_five_hundred_ms_flag(void)
{
	return s_stQJBL.five_hundred_ms_flag;
}

void set_five_hundred_ms_flag(INT8U val)
{
	s_stQJBL.five_hundred_ms_flag = val;
}

INT8U get_export_can_flag(void)
{
	return s_stQJBL.export_can_flag;
}

void set_export_can_flag(INT8U val)
{
	s_stQJBL.export_can_flag = val;
}

INT8U get_plat_sw1(void)
{
	return s_stQJBL.plat_sw1;
}

void set_plat_sw1(INT8U val)
{
	s_stQJBL.plat_sw1 = val;
}

INT8U get_plat_sw2(void)
{
	return s_stQJBL.plat_sw2;
}

void set_plat_sw2(INT8U val)
{
	s_stQJBL.plat_sw2 = val;
}



INT8U get_rdy_sleep_flag(void)
{
	return s_stQJBL.rdy_sleep_flag;
}

void set_rdy_sleep_flag(INT8U val)
{
	s_stQJBL.rdy_sleep_flag = val;
}

void set_export_data_flag(INT8U val)
{
	s_stQJBL.export_data_flag = val;
}

INT8U get_export_data_flag(void)
{
	return s_stQJBL.export_data_flag;
}

INT8U get_test_step(void)
{
	return s_stQJBL.test_step;
}

void set_test_step(INT8U val)
{
	s_stQJBL.test_step = val;
}

unsigned char get_chaichu_sos_flag(void)
{
	return s_stQJBL.chaichu_sos_flag;
}

void set_chaichu_sos_flag(unsigned char val)
{
	s_stQJBL.chaichu_sos_flag = val;
}

unsigned char get_shoudong_login_flag(void)
{
	return s_stQJBL.shoudong_login_flag;
}

void set_shoudong_login_flag(unsigned char val)
{
	s_stQJBL.shoudong_login_flag = val;
}


unsigned char get_sjz_beian_flag(void)
{
	return s_stQJBL.sjz_beian_flag;
}

void set_sjz_beian_flag(unsigned char val)
{
	s_stQJBL.sjz_beian_flag = val;
}


//获取公钥
unsigned char *get_pubkey_ptr(void)
{
	return s_stQJBL.pubkey;
}


void set_logout_flag(unsigned char channel,INT8U val)
{
	s_stQJBL.logout_flag[channel] = val;
}

INT8U get_logout_flag(unsigned char channel)
{
	return s_stQJBL.logout_flag[channel];
}


void set_can_no_data_time(INT32U val)
{
	s_stQJBL.can_no_data_time = val;
}

INT32U get_can_no_data_time(void)
{
	return s_stQJBL.can_no_data_time;
}

void set_acc_no_data_time(INT32U val)
{
	s_stQJBL.acc_no_data_time = val;
}

INT32U get_acc_no_data_time(void)
{
	return s_stQJBL.acc_no_data_time;
}


void set_heart_beat_sw(INT8U val)
{
	s_stQJBL.ucHeartBeatSw = val;
}

INT8U get_heart_beat_sw(void)
{
	return s_stQJBL.ucHeartBeatSw;
}



INT8U GetReportStatus(void)
{
	return s_stQJBL.ucReportStatusSw;
}

void SetReportStatus(INT8U ucStatus)
{
	s_stQJBL.ucReportStatusSw = ucStatus;
}

void UserParamDefault(void)
{
	//unsigned int i = 0;

	s_stUserParam.usInitFlag = INIT_STORE_FLASH_FLAG;

	set_engine_type(EngineType_Compatible);

	//CAN1 CAN2波特率默认值
	SetCan0Bps(250);
	SetCan1Bps(250);
	SetCan2Bps(250);

	SPSetApnUsername(APN_DEFAULT);
	SPSetApnUsername(APN_USERNAME_DEFAULT);
	SPSetApnPassword(APN_PASSWORD_DEFAULT);

#if 1
	SPSetServerAddr(0,CN6_SERVER_ADDR_DEFAULT);
	SPSetServerAddr(1,CN6_SERVER_ADDR_DEFAULT1);
	SPSetServerAddr(2,CN6_SERVER_ADDR_DEFAULT2);
#endif
	SPSetServerPort(0,CN6_SERVER_PORT_DEFAULT);
	SPSetServerPort(1,CN6_SERVER_PORT_DEFAULT1);
	SPSetServerPort(2,CN6_SERVER_PORT_DEFAULT2);

	//默认国六17691协议
	set_plat_pro_type(0,E_PROTO_GB17691_TYPE);
	set_plat_pro_type(1,E_PROTO_GB17691_TYPE);
	set_plat_pro_type(2,E_PROTO_GB17691_TYPE);


	SysSetCanOuttime(M_CAN_NO_DATA_OUTTIME);
	SysSetSleepBaseAccCanFlag(1);

	//StoreOneData(BLOCK_SYS_PARAM_ID,&s_stUserParam,sizeof(s_stUserParam));
	//SaveSysParam();
	
}

char IsBPSOK(INT32U bps)
{
	if ((bps == 9600)|| (bps == 115200) || (bps == 4800) || (bps == 14400) || (bps == 19200) 
	|| (bps == 28800) || (bps == 38400) || (bps == 57600))
	{
		return 1;
	}
	return 0;
}

void LoadUserParam(void)
{
	DBG_Print(LEVEL0,"UserParam Size:%d\r\n",sizeof(s_stUserParam));
}


INT16U GetSysParamSize(void)
{
	return sizeof(SysParam);
}



void SetSN(void *szSN)
{
	if (szSN)
	{
		memset(s_stUserParam.szSN,0,sizeof(s_stUserParam.szSN));
		strncpy(s_stUserParam.szSN,szSN,MAX_SN_LEN);
	}
}

INT8S *GetSN(void)
{
	return s_stUserParam.szSN;
}


void SetVIN(void *szVIN)
{
	if (szVIN)
	{
		memset(s_stUserParam.szVIN,0,sizeof(s_stUserParam.szVIN));
		strncpy(s_stUserParam.szVIN,szVIN,MAX_VIN_LEN);
	}
}

INT8S *GetVIN(void)
{
	return s_stUserParam.szVIN;
}

INT8U *GetEncryptionMethodPtr(void)
{
	return &(s_stUserParam.ucEncryptionMethod);
}

INT8U *CN6_GetVerPtr(void)
{
	return &(s_stUserParam.ucCN6_Ver);
}

void CN6_SetVer(INT8U ucVer)
{
	s_stUserParam.ucCN6_Ver = ucVer;
}

INT8U *CN6_GetWayPtr(void)
{
	return &(s_stUserParam.ucCN6_Way);
}

void CN6_SetWay(INT8U ucWay)
{
	s_stUserParam.ucCN6_Way = ucWay;
}

INT8U GetEncryptionMethod(void)
{
	return s_stUserParam.ucEncryptionMethod;
}

INT8U *GetBCDPhoneNo(void)
{
	return s_stUserParam.BCDPhoneNo;
}

void SetBCDPhoneNo(INT8U bcd[6])
{
	if (bcd)
	{
		memcpy(s_stUserParam.BCDPhoneNo,bcd,6);
	}
}



unsigned int GetCan0Bps(void)
{
	return s_stUserParam.aeCanBaudRate[0];
}

void SetCan0Bps(unsigned int canbps)
{
		s_stUserParam.aeCanBaudRate[0] = canbps;
}


unsigned int GetCan1Bps(void)
{
	return s_stUserParam.aeCanBaudRate[1];
}

void SetCan1Bps(unsigned int canbps)
{
		s_stUserParam.aeCanBaudRate[1] = canbps;
}

unsigned int GetCan2Bps(void)
{
	return s_stUserParam.aeCanBaudRate[2];
}

void SetCan2Bps(unsigned int canbps)
{
	s_stUserParam.aeCanBaudRate[2] = canbps;
}



void SPSetApn(char *szApn)
{
	if (szApn)
	{
		memset(s_stUserParam.szApn,0,sizeof(s_stUserParam.szApn));
		strncpy(s_stUserParam.szApn,szApn,sizeof(s_stUserParam.szApn) - 1);
	}
}

void SPSetApnUsername(char *str)
{
	if (str)
	{
		memset(s_stUserParam.szApnUsername,0,sizeof(s_stUserParam.szApnUsername));
		strncpy(s_stUserParam.szApnUsername,str,sizeof(s_stUserParam.szApnUsername) - 1);
	}
}

void SPSetApnPassword(char *str)
{
	if (str)
	{
		memset(s_stUserParam.szApnPassword,0,sizeof(s_stUserParam.szApnPassword));
		strncpy(s_stUserParam.szApnPassword,str,sizeof(s_stUserParam.szApnPassword) - 1);
	}
}

char *SPGetApn(void)
{
	return s_stUserParam.szApn;
}

char *SPGetApnUsername(void)
{
	return s_stUserParam.szApnUsername;
}

char *SPGetApnPassword(void)
{
	return s_stUserParam.szApnPassword;
}

char *SPGetServerAddr(INT8U idx)
{
	return s_stUserParam.szServerAddr[idx];
}

void SPSetServerAddr(INT8U idx,void *szAddr)
{
	if (idx >= MAX_SVR_LINK_CNT)
	{
		return ;
	}
	if (szAddr == 0)
	{
		return ;
	}
	if (strcmp(s_stUserParam.szServerAddr[idx],(char *)szAddr) == 0)
	{
		return ;
	}
	memset(s_stUserParam.szServerAddr[idx],0,sizeof(s_stUserParam.szServerAddr[idx]));
	snprintf(s_stUserParam.szServerAddr[idx],sizeof(s_stUserParam.szServerAddr[idx]),"%s",(char *)szAddr);
}

void SPSetServerPort(INT8U idx,INT16U usPort)
{
	if (s_stUserParam.ausPort[idx] == usPort)
	{
		return ;
	}
	s_stUserParam.ausPort[idx] = usPort;

}

INT16U SPGetServerPort(INT8U idx)
{
	return s_stUserParam.ausPort[idx];
}

void SaveSysParam(void)
{
}

INT8U GetSocketType(unsigned char idx)
{
	if (idx >= MAX_SVR_LINK_CNT) return 1;

	return s_stUserParam.ucSocketType[idx];
}


INT8U ServerLinkIsTcp(unsigned char idx)
{
	if (idx >= MAX_SVR_LINK_CNT) return 1;

	if (s_stUserParam.ucSocketType[idx] == E_TCP_SOCKET_TCP)
	{
		return 1;
	}
	else if (s_stUserParam.ucSocketType[idx] == E_TCP_SOCKET_UDP)
	{
		return 0;
	}
	return 0;
}

ProtocolTypeE get_plat_pro_type(unsigned char channel)
{
	if (channel >= MAX_SVR_LINK_CNT) return (ProtocolTypeE)0;
	return s_stUserParam.aeProtocolTypeE[channel];
}

void set_plat_pro_type(unsigned char channel,ProtocolTypeE em)
{
	if (channel >= MAX_SVR_LINK_CNT) return ;
	s_stUserParam.aeProtocolTypeE[channel] = em;
}

void SetSocketType(unsigned char idx,TcpSocketTypeE em)
{
	if (idx >= MAX_SVR_LINK_CNT) return ;

	s_stUserParam.ucSocketType[idx] = em;
}


INT8S *GetAuthenticationCode(void)
{
	return s_stUserParam.aucAuthenticationCode;
}

void SetAuthenticationCode(void *ptr)
{
	if (ptr)
	{
		memset(s_stUserParam.aucAuthenticationCode,0,sizeof(s_stUserParam.aucAuthenticationCode));
		strncpy((char *)(s_stUserParam.aucAuthenticationCode),ptr,sizeof(s_stUserParam.aucAuthenticationCode) - 1);
	}
}

INT16U GetMainPowerThreshold(void)
{
	return s_stUserParam.usMainPowerThreshold;
}

void SetMainPowerThreshold(INT16U val)
{
	s_stUserParam.usMainPowerThreshold = val;
}

INT8S *GetLicensePlate(void)
{
	return s_stUserParam.szLicensePlate;
}

void SetLicensePlate(void *ptr)
{
	if (ptr)
	{
		memset(s_stUserParam.szLicensePlate,0,sizeof(s_stUserParam.szLicensePlate));
		strncpy(s_stUserParam.szLicensePlate,ptr,sizeof(s_stUserParam.szLicensePlate) - 1);
	}
}

INT8U *GetTerminalModel(void)
{
	return s_stUserParam.aucTerminalModel;
}

void SetTerminalModel(void *ptr)
{
	if (ptr)
	{
		//memset(s_stUserParam.aucTerminalModel,0,sizeof(s_stUserParam.aucTerminalModel));
		memcpy(s_stUserParam.aucTerminalModel,ptr,8);
	}
}


INT8U *GetTerminalID(void)
{
	return s_stUserParam.aucTerminalID;
}

void SetTerminalID(void *ptr)
{
	if (ptr)
	{
		strncpy((char *)s_stUserParam.aucTerminalID,(char *)ptr,7);
	}
}

void SetPlateColor(INT8U val)
{
	s_stUserParam.ucLicensePlateColor = val;
}

INT8U GetPlateColor(void)
{
	return s_stUserParam.ucLicensePlateColor;
}



INT8U *GetManufacturerID(void)
{
	return s_stUserParam.aucManufacturerID;
}

void SetManufacturerID(void *ptr)
{
	if (ptr)
	{
		//memset(s_stUserParam.aucManufacturerID,0,sizeof(s_stUserParam.aucManufacturerID));
		memcpy(s_stUserParam.aucManufacturerID,ptr,5);
	}
}


INT16U GetProviceID(void)
{
	return s_stUserParam.usProviceID;
}

void SetProviceID(INT16U usID)
{
	s_stUserParam.usProviceID = usID;
}


INT16U GetCityID(void)
{
	return s_stUserParam.usCityID;
}

void SetCityID(INT16U usID)
{
	s_stUserParam.usCityID = usID;
}


INT8U GetLimitSpeed(void)
{
	return s_stUserParam.ucLimitSpeed;
}

void SetCanChannel(INT8U val)
{
	//最多不超过3路CAN
	if (val >= 3) return ;
	
	if (val != s_stUserParam.ucCanChannel)
	{
		s_stUserParam.ucCanChannel = val;
	}
}

INT8U GetCanChannel(void)
{
	return s_stUserParam.ucCanChannel;
}


char *plat_pro_type_str[] = 
{
	"J5",			/* 京5协议 */
	"CN6",			/* 国六协议 */
	"ShH4/5",			/* 上海重型柴油车 */
	"CN56",			/* 国5-6协议，以国6为标准，不存在备案，防拆等逻辑，流水号在消息类型前面 */
	"HZ6",			/* 杭州地标协议 */
	"GB17691",		/* GB17691协议，以国6为标准，不存在备案，防拆等逻辑，流水号在第一组消息类型和消息体之间 */
	"JAC_CN6",		/* 江淮国6协议，以国6标准，流水号在消息类型前面，但是每一组都有流水号 */
	"江淮新国六",	/* 江淮新的国6协议，每一组数据中都有采集时间  */

	"E_PROTO_BACK_DOOR_TYPE", 	/* 远程管理协议  */
};

char *get_plat_pro_type_str(ProtocolTypeE st)
{
	if (st < E_PROTO_BACK_DOOR_TYPE)
	{
		return plat_pro_type_str[st];
	}
	return "unkow";
}

char *engine_type_str[] = 
{
"兼容",
"D78",
"EV18",
"ZD30",
"M9T",
"玉柴",
"全柴",
"潮柴",
"潍柴",
"康明斯",

"DFAC",
"E11K",
"EM26",
"LXC",
};

char *get_engine_type_str(EngineType st)
{
	if (st < EngineType_MAX)
	{
		return engine_type_str[st];
	}
	return "unkow";
}


/**************************************************************************
* 函  数: u8 SysGetSleepBaseAccCanFlag(void)
* 描  述: 获取休眠是基于ACC还是CAN来判断
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: u8 :0 - 基于ACC-0判断， 1-基于CAN接收超时-默认10min(可配置)
**************************************************************************/
INT8U SysGetSleepBaseAccCanFlag(void)
{
	return s_stUserParam.sleepBaseAccCanFlag;
}

/**************************************************************************
* 函  数: void SysSetSleepBaseAccCanFlag(u8 flag)
* 描  述: 设置休眠是基于ACC还是CAN来判断
* 举  例: 
* 入  参: u8 flag：基于ACC-0判断， 1-基于CAN接收超时-默认10min(可配置)
* 出  参: void
* 返回值: void
**************************************************************************/
void SysSetSleepBaseAccCanFlag(INT8U flag)
{
	if (flag != s_stUserParam.sleepBaseAccCanFlag)
	{
		s_stUserParam.sleepBaseAccCanFlag = flag;
	}
}


/**************************************************************************
* 函  数: u8 SysGetCanOuttime(void)
* 描  述: 获取Can的接收超时时间，单位min
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: u8 :超时时间，单位min
**************************************************************************/
INT8U SysGetCanOuttime(void)
{
	return s_stUserParam.canNoDataOuttime;
}

/**************************************************************************
* 函  数: void SysSetCanOuttime(u8 min)
* 描  述: 设置CAN接收超时时间
* 举  例: 
* 入  参: u8 min：超时时间min
* 出  参: void
* 返回值: void
**************************************************************************/
void SysSetCanOuttime(INT8U min)
{
	if (min != s_stUserParam.canNoDataOuttime)
	{
		s_stUserParam.canNoDataOuttime = min;
	}
}

/**************************************************************************
* 函  数: void SysGetChipId(char *chipId)
* 描  述: 获取芯片ID信息
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: void
**************************************************************************/
void SysGetChipId(void *chipId)
{
	memcpy((void *)chipId, (void *)s_stUserParam.chipId, 16);
}

/**************************************************************************
* 函  数: void SysSetChipId(u8 *chipId, int len)
* 描  述: 设置芯片ID信息
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: void
**************************************************************************/
void SysSetChipId(void *chipId, int len)
{
	if (len <= 16)
	{
		memset(s_stUserParam.chipId, 0, sizeof(s_stUserParam.chipId));
		if (len) memcpy((void *)s_stUserParam.chipId, (void *)chipId, len);
	}
}

char *get_chipid_ptr(void)
{
	return (char *)s_stUserParam.chipId;
}

INT32U get_drive_cycle_cnt(void)
{
	return s_stUserParam.drive_cycle_cnt;
}

void set_drive_cycle_cnt(INT32U val)
{
	s_stUserParam.drive_cycle_cnt = val;
}

INT8U get_beian_flag(void)
{
	return s_stUserParam.beian_flag;
}

void set_beian_flag(INT8U val)
{
	s_stUserParam.beian_flag = val;
}



void beian_success(void)
{
	set_beian_flag(THE_BEIAN_SUCCESS_FLAG);
}

INT8U is_beian_success(void)
{
	if (get_beian_flag() == THE_BEIAN_SUCCESS_FLAG)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void set_can_speed_id(INT32U val)
{
	s_stUserParam.speed_canid = val;
}

INT32U get_can_speed_id(void)
{
	return s_stUserParam.speed_canid;
}

void set_can_milage_id(INT32U val)
{
	s_stUserParam.milage_canid = val;
}


INT32U get_can_milage_id(void)
{
	return s_stUserParam.milage_canid;
}

void set_can_bag_id(INT32U val)
{
	s_stUserParam.can_bag_id = val;
}


INT32U get_can_bag_id(void)
{
	return s_stUserParam.can_bag_id;
}


#define MAX_RECORD_CNT_KAIJI_TIME 6
void record_kaiji_time(INT8U year,INT8U month,INT8U day,INT8U hour,INT8U min,INT8U sec)
{
	INT8U idx = s_stUserParam.ucLicensePlateColor;

	if (idx >= MAX_RECORD_CNT_KAIJI_TIME)
	{
		idx = 0;
	}
	s_stUserParam.aucAuthenticationCode[idx * 6 + 0] = year;
	s_stUserParam.aucAuthenticationCode[idx * 6 + 1] = month;
	s_stUserParam.aucAuthenticationCode[idx * 6 + 2] = day;
	s_stUserParam.aucAuthenticationCode[idx * 6 + 3] = hour;
	s_stUserParam.aucAuthenticationCode[idx * 6 + 4] = min;
	s_stUserParam.aucAuthenticationCode[idx * 6 + 5] = sec;
	idx++;
	if (idx >= MAX_RECORD_CNT_KAIJI_TIME)
	{
		idx = 0;
	}
	s_stUserParam.ucLicensePlateColor = idx;
}

//最多记录最近6条开机记录
INT8U get_last_kaiji_time(INT8U idx,INT8U *year,INT8U *month,INT8U *day,INT8U *hour,INT8U *min,INT8U *sec)
{
	INT8U ridx = 0;
	if (idx >= MAX_RECORD_CNT_KAIJI_TIME)
	{
		return 1;
	}

	if (s_stUserParam.ucLicensePlateColor >= (1+idx))
	{
		ridx = s_stUserParam.ucLicensePlateColor - 1 - idx;
	}
	else
	{
		ridx = s_stUserParam.ucLicensePlateColor + MAX_RECORD_CNT_KAIJI_TIME - 1 - idx;
	}

	if (year)
	{
		*year = s_stUserParam.aucAuthenticationCode[ridx * 6 + 0];
	}
	if (month)
	{
		*month = s_stUserParam.aucAuthenticationCode[ridx * 6 + 1];
	}
	if (day)
	{
		*day = s_stUserParam.aucAuthenticationCode[ridx * 6 + 2];
	}
	if (hour)
	{
		*hour = s_stUserParam.aucAuthenticationCode[ridx * 6 + 3];
	}
	if (min)
	{
		*min = s_stUserParam.aucAuthenticationCode[ridx * 6 + 4];
	}
	if (sec)
	{
		*sec = s_stUserParam.aucAuthenticationCode[ridx * 6 + 5];
	}

	return 0;
}

unsigned int disp_sysparam(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	char *piccid = 0;
	char *pimei = 0;

	piccid = send_msg(S_MSG_GET_ICCID,0,0,0);
	pimei = send_msg(S_MSG_GET_IMEI,0,0,0);

	if (pOut)
	{
		rlen = snprintf((char *)pOut,outbuflen,"[%u]ver:%s VIN:%s "
												"iccid:%s\r\n"
												"can_fps:%u %u %u bps:%u %u %u",
												GetRunTime(),
												APP_VERSION,
												GetVIN(),
												piccid,
												get_can_rx_fps(0),
												get_can_rx_fps(1),
												get_can_rx_fps(2),
												GetCan0Bps(),GetCan1Bps(),GetCan2Bps()
												);
	}

	DBG_Print(0,"[%u]ver:%s VIN:%s chipid:%s drive_cnt:%u beian:%02X\r\n"
				"iccid:%s imei:%s speedid:%08X milageid:%08X\r\n"
				"tid:%s cnt:%u can_fps:%u %u %u bps:%u %u %u\r\n",
				GetRunTime(),
				APP_VERSION,
				GetVIN(),get_chipid_ptr(),get_drive_cycle_cnt(),get_beian_flag(),
				piccid,pimei,
				get_can_speed_id(),get_can_milage_id(),GetTerminalID(),0,
				get_can_rx_fps(0),
				get_can_rx_fps(1),
				get_can_rx_fps(2),
				GetCan0Bps(),GetCan1Bps(),GetCan2Bps()
				);
	return rlen;
}


typedef struct tagSaveParam
{
	INT16U flag;
	INT8U want_pro_ver_about_lock : 4;				//希望的协议版本 东风基础锁车版本
	//INT8U want_lock_status : 4;						//希望的锁车状态 东风基础锁车版本
	INT8U want_bind_status : 2;						//希望的锁车状态 东风基础锁车版本
	INT8U want_lock_status : 2;						//希望的锁车状态 东风基础锁车版本
	INT8U lock_car_cmd_from_where : 4;				//锁车指令来自哪个平台  1表示来自东风平台 2表示来自地上铁平台

	//dst 参数
	INT8U dst_network_error_flag : 2;				//网络异常标志
	INT8U dst_network_error_tip_flag : 1;			//网络异常仪表提示
	INT8U dst_no_auto_lock_when_meet_net_error_condition_flag : 1;		//即使满足网络异常条件也不进行自动锁车标志

	INT8U dst_want_pro_ver_about_lock : 4;			//dst希望的协议版本
	INT8U dst_want_lock_status : 4;					//dst希望的锁车状态
	INT8U dst_want_limit_speed_level : 4;			//dst希望的限速等级
	INT8U dst_want_kucunrong_limit_speed : 4;		//dst希望的库存融限速

	INT8U dst_want_pro_ver_about_lock_bak : 4;		//dst希望的协议版本	备份
	INT8U dst_want_lock_status_bak : 4;				//dst希望的锁车状态	备份
	INT8U dst_want_limit_speed_level_bak : 4;		//dst希望的限速等级	备份
	INT8U dst_want_kucunrong_limit_speed_bak : 4;	//dst希望的库存融限速 	备份

	INT8U dst_pay_tip_flag : 4; 					//缴费提示
	INT8U dst_keep_fit_tip_flag : 4;				//保养提示
	INT8U dst_inspect_tip_flag : 4; 				//年审提示

	INT32U dst_milage_when_network_error;
	INT8U dst_time_when_network_error[6];
	INT32U dst_net_error_accumulate_time;			//网络错误的累加时间
	INT8U dst_vin[17];								//vin记忆
	
	INT8S df_jd_vin[17];							//vin记忆
	
	INT8U dst_vcu_ver[16 + 1];						//vcu版本	

	INT8U dst_fact_bind_status : 4;					//dst实际的绑定状态
	INT8U dst_fact_pro_ver_about_lock : 4;			//dst实际的锁车协议版本
	INT8U dst_fact_lock_status : 4;					//dst实际的锁车状态 0 未锁  1 锁定
	INT8U dst_fact_limit_speed_level : 2;			//实际的限速等级
	INT8U dst_fact_kucunrong_limit_speed : 2;		//实际的库存融限速


	//三电系统相关版本号
	INT8U vcu_ver[16 + 1];							//vcu版本	
	INT8U mcu_ver[16 + 1];							//mc版本	
	INT8U bms_ver[16 + 1];							//bms版本	
	INT8U sc_date[6];								//生产日期
	INT8U iccid[21];
}SaveParam;

//状态容易发生变化的参数
typedef struct tagEasyChangeParam
{

	INT8U che_io_status;							//车门 车锁 等二值信号状态
	INT8U stEngineType;
	INT16U gb_login_flow[7];
	INT8U cur_date[3];
}EasyChangeParam;

SaveParam stSaveParam = {0};
SaveParam stYZSaveParam = {0};

EasyChangeParam stEasyChangeParam = {0};
EasyChangeParam stYZEasyChangeParam = {0};

INT8U *get_cur_date_ptr(void)
{
	return stEasyChangeParam.cur_date;
}

INT16U *get_gb_login_flow_ptr(void)
{
	return stEasyChangeParam.gb_login_flow;
}

INT16U get_gb_login_flow_cnt(void)
{
	return (sizeof(stEasyChangeParam.gb_login_flow) / sizeof(stEasyChangeParam.gb_login_flow[0]));
}


//车门开关 0关 1开
void set_dst_chemen_switch(INT8U val)
{
	if (val)
	{
		stEasyChangeParam.che_io_status |= (1 << 0);
	}
	else
	{
		stEasyChangeParam.che_io_status &= (~(1 << 0));
	}
}

//车锁开关 0关 1开
void set_dst_chesuo_switch(INT8U val)
{
	if (val)
	{
		stEasyChangeParam.che_io_status |= (1 << 1);
	}
	else
	{
		stEasyChangeParam.che_io_status &= (~(1 << 1));
	}
}

void set_dst_left_qian_chesuo_switch(INT8U val)
{
	if (val)
	{
		stEasyChangeParam.che_io_status |= (1 << 2);
	}
	else
	{
		stEasyChangeParam.che_io_status &= (~(1 << 2));
	}
}

INT8U get_dst_chemen_switch(void)
{
	return ((stEasyChangeParam.che_io_status & (1 << 0)) ? 1 : 0);
}

INT8U get_dst_chesuo_switch(void)
{
	return ((stEasyChangeParam.che_io_status & (1 << 1)) ? 1 : 0);
}

INT8U get_dst_left_qian_chesuo_switch(void)
{
	return ((stEasyChangeParam.che_io_status & (1 << 2)) ? 1 : 0);
}

EngineType get_engine_type(void)
{
	return (EngineType)(stEasyChangeParam.stEngineType);
}

void set_engine_type(EngineType st)
{
	if (stEasyChangeParam.stEngineType != st)
	{
		if (st < EngineType_MAX)
		{
			stEasyChangeParam.stEngineType = st;
		}
		else
		{
			stEasyChangeParam.stEngineType = EngineType_Compatible;
		}
	}
}

INT8U get_dst_fact_bind_status(void)
{
	return stSaveParam.dst_fact_bind_status;
}

INT8U get_dst_fact_pro_ver_about_lock(void)
{
	return stSaveParam.dst_fact_pro_ver_about_lock;
}

INT8U get_dst_fact_lock_status(void)
{
	return stSaveParam.dst_fact_lock_status;
}

void set_dst_fact_bind_status(INT8U val)
{
	stSaveParam.dst_fact_bind_status = val;
}

void set_dst_fact_pro_ver_about_lock(INT8U val)
{
	stSaveParam.dst_fact_pro_ver_about_lock = val;
}

void set_dst_fact_lock_status(INT8U val)
{
	stSaveParam.dst_fact_lock_status = val;
}

INT8U get_dst_fact_kucunrong_limit_speed(void)
{
	return stSaveParam.dst_fact_kucunrong_limit_speed;
}

void set_dst_fact_kucunrong_limit_speed(INT8U val)
{
	stSaveParam.dst_fact_kucunrong_limit_speed = val;
}

INT8U get_dst_fact_limit_speed_level(void)
{
	return stSaveParam.dst_fact_limit_speed_level;
}

void set_dst_fact_limit_speed_level(INT8U val)
{
	stSaveParam.dst_fact_limit_speed_level = val;
}

void clr_dst_vcu_ver(void)
{
	memset(stSaveParam.dst_vcu_ver,0,sizeof(stSaveParam.dst_vcu_ver));
}

void set_dst_vcu_ver(unsigned char idx,unsigned char ch)
{
	if (idx < 16)
	{
		stSaveParam.dst_vcu_ver[idx] = ch;
	}
}

INT8U *get_dst_vcu_ver_ptr(void)
{
	return stSaveParam.dst_vcu_ver;
}


void set_vcu_ver(unsigned char idx,unsigned char ch)
{
	if (idx < 16)
	{
		stSaveParam.vcu_ver[idx] = ch;
	}
}

void set_mcu_ver(unsigned char idx,unsigned char ch)
{
	if (idx < 16)
	{
		stSaveParam.mcu_ver[idx] = ch;
	}
}

void set_bms_ver(unsigned char idx,unsigned char ch)
{
	if (idx < 16)
	{
		stSaveParam.bms_ver[idx] = ch;
	}
}

void clr_bms_ver(void)
{
	memset(stSaveParam.bms_ver,0,sizeof(stSaveParam.bms_ver));
}

void clr_mcu_ver(void)
{
	memset(stSaveParam.mcu_ver,0,sizeof(stSaveParam.mcu_ver));
}

void clr_vcu_ver(void)
{
	memset(stSaveParam.vcu_ver,0,sizeof(stSaveParam.vcu_ver));
}

INT8U *get_vcu_ver_ptr(void)
{
	return stSaveParam.vcu_ver;
}

INT8U *get_mcu_ver_ptr(void)
{
	return stSaveParam.mcu_ver;
}

INT8U *get_bms_ver_ptr(void)
{
	return stSaveParam.bms_ver;
}


void set_dst_vin(void *pvin,unsigned int vinlen)
{
	if (pvin && vinlen)
	{
		memcpy(stSaveParam.dst_vin,pvin,(vinlen > 17) ? 17 : vinlen);
	}
}

INT8U *get_dst_vin(void)
{
	return stSaveParam.dst_vin;
}


void set_dst_net_error_accumulate_time(INT32U val)
{
	if (val >= (96 * 3600))
	{
		val = (96 * 3600);
	}
	stSaveParam.dst_net_error_accumulate_time = val;
}

INT32U get_dst_net_error_accumulate_time(void)
{
	return stSaveParam.dst_net_error_accumulate_time;
}

INT8U get_want_pro_ver_about_lock(void)
{
	return stSaveParam.want_pro_ver_about_lock;
}

INT8U get_want_lock_status(void)
{
	return stSaveParam.want_lock_status;
}

void set_want_pro_ver_about_lock(INT8U val)
{
	stSaveParam.want_pro_ver_about_lock = val;
}

void set_want_lock_status(INT8U val)
{
	stSaveParam.want_lock_status = val;
}

unsigned char get_lock_car_cmd_from_where(void)
{
	return stSaveParam.lock_car_cmd_from_where;
}

void set_lock_car_cmd_from_where(unsigned char val)
{
	stSaveParam.lock_car_cmd_from_where = val;
}

unsigned char get_dst_no_auto_lock_when_meet_net_error_condition_flag(void)
{
	return stSaveParam.dst_no_auto_lock_when_meet_net_error_condition_flag;
}

void set_dst_no_auto_lock_when_meet_net_error_condition_flag(unsigned char val)
{
	stSaveParam.dst_no_auto_lock_when_meet_net_error_condition_flag = val;
}

unsigned char get_dst_network_error_tip_flag(void)
{
	return stSaveParam.dst_network_error_tip_flag;
}

void set_dst_network_error_tip_flag(unsigned char val)
{
	stSaveParam.dst_network_error_tip_flag = val;
}

unsigned char get_dst_network_error_flag(void)
{
	return stSaveParam.dst_network_error_flag;
}

void set_dst_network_error_flag(unsigned char val)
{
	stSaveParam.dst_network_error_flag = val;
}


INT8U *get_dst_time_when_network_error(void)
{
	return stSaveParam.dst_time_when_network_error;
}

void set_dst_time_when_network_error(INT8U t[6])
{
	if (t)
	{
		memcpy(stSaveParam.dst_time_when_network_error,t,6);
	}
}

INT32U get_dst_milage_when_network_error(void)
{
	return stSaveParam.dst_milage_when_network_error;
}

void set_dst_milage_when_network_error(INT32U val)
{
	stSaveParam.dst_milage_when_network_error = val;
}


//备份锁车参数
void backup_dst_lock_car_param(void)
{
	stSaveParam.dst_want_kucunrong_limit_speed_bak = stSaveParam.dst_want_kucunrong_limit_speed;
	stSaveParam.dst_want_limit_speed_level_bak = stSaveParam.dst_want_limit_speed_level;
	stSaveParam.dst_want_lock_status_bak = stSaveParam.dst_want_lock_status;
	stSaveParam.dst_want_pro_ver_about_lock_bak = stSaveParam.dst_want_pro_ver_about_lock;
}

//恢复锁车参数
void resume_dst_lock_car_param(void)
{
	stSaveParam.dst_want_kucunrong_limit_speed = stSaveParam.dst_want_kucunrong_limit_speed_bak;
	stSaveParam.dst_want_limit_speed_level = stSaveParam.dst_want_limit_speed_level_bak;
	stSaveParam.dst_want_lock_status = stSaveParam.dst_want_lock_status_bak;
	stSaveParam.dst_want_pro_ver_about_lock = stSaveParam.dst_want_pro_ver_about_lock_bak;
}


INT8U get_dst_want_pro_ver_about_lock(void)
{
	return stSaveParam.dst_want_pro_ver_about_lock;
}

INT8U get_dst_want_lock_status(void)
{
	return stSaveParam.dst_want_lock_status;
}

void set_dst_want_pro_ver_about_lock(INT8U val)
{
	stSaveParam.dst_want_pro_ver_about_lock = val;
}

void set_dst_want_lock_status(INT8U val)
{
	stSaveParam.dst_want_lock_status = val;
}


INT8U get_dst_want_kucunrong_limit_speed(void)
{
	return stSaveParam.dst_want_kucunrong_limit_speed;
}

void set_dst_want_kucunrong_limit_speed(INT8U val)
{
	stSaveParam.dst_want_kucunrong_limit_speed = val;
}

INT8U get_dst_want_limit_speed_level(void)
{
	return stSaveParam.dst_want_limit_speed_level;
}

void set_dst_want_limit_speed_level(INT8U val)
{
	stSaveParam.dst_want_limit_speed_level = val;
}

INT8U get_dst_pay_tip_flag(void)
{
	return stSaveParam.dst_pay_tip_flag;
}

void set_dst_pay_tip_flag(INT8U val)
{
	stSaveParam.dst_pay_tip_flag = val;
}


INT8U get_dst_keep_fit_tip_flag(void)
{
	return stSaveParam.dst_keep_fit_tip_flag;
}

void set_dst_keep_fit_tip_flag(INT8U val)
{
	stSaveParam.dst_keep_fit_tip_flag = val;
}

INT8U get_dst_inspect_tip_flag(void)
{
	return stSaveParam.dst_inspect_tip_flag;
}

void set_dst_inspect_tip_flag(INT8U val)
{
	stSaveParam.dst_inspect_tip_flag = val;
}

INT8U *get_sc_date(void)
{
	return stSaveParam.sc_date;
}

void set_sc_date(INT8U dat[6])
{
	memcpy(stSaveParam.sc_date,dat,6);
}

void load_param(void)
{
	send_msg(S_MSG_READ_PARAM,&stSaveParam,(void *)(sizeof(SaveParam)),0);
	if ((stSaveParam.flag != INIT_STORE_FLASH_FLAG) 
		|| (get_want_lock_status() > 2)
		|| (get_want_pro_ver_about_lock() > 1))
	{
		DBG_Print(LEVEL2,"load param(fact:%04X want:%04X)\r\n",stSaveParam.flag,INIT_STORE_FLASH_FLAG);

		set_lock_car_cmd_from_where(0);

		set_want_bind_status(0);
		set_want_lock_status(0);
		set_want_pro_ver_about_lock(0);

		set_dst_want_lock_status(0);
		set_dst_want_pro_ver_about_lock(0);

		set_dst_inspect_tip_flag(0);
		set_dst_keep_fit_tip_flag(0);
		set_dst_pay_tip_flag(0);
		set_dst_want_kucunrong_limit_speed(0);
		set_dst_want_limit_speed_level(0);
		set_dst_network_error_flag(0);
		set_dst_network_error_tip_flag(0);
		set_dst_no_auto_lock_when_meet_net_error_condition_flag(0);

		set_dst_net_error_accumulate_time(0);
		
		
		stSaveParam.flag = INIT_STORE_FLASH_FLAG;
	}
	else
	{
		memcpy(&stYZSaveParam,&stSaveParam,sizeof(SaveParam));

	}

	if (get_dst_network_error_flag() > 1)
	{
		set_dst_network_error_flag(0);
	}
	
	if(is_string(stSaveParam.iccid,20))
	{
		char *piccid = 0;
		piccid = send_msg(S_MSG_GET_ICCID,0,0,0);
		memcpy(piccid,stSaveParam.iccid,20);
	}
}

void save_param(void)
{
	if(is_string((stSaveParam.iccid),20))
	{
		//DBG_Print(LEVEL0,"using save iccid\r\n");
	}
	else
	{
		unsigned char iccid[21] = {0};
		send_msg(S_MSG_GET_ICCID,iccid,(void *)20,0);
		if(is_string(iccid,20))
		{
			memcpy((stSaveParam.iccid),iccid,20);
		}
		
		DBG_Print(LEVEL0,"update st save iccid\r\n");
	}
	
	if (memcmp(&stSaveParam,&stYZSaveParam,sizeof(SaveParam)))
	{
		send_msg(S_MSG_WRITE_PARAM,&stSaveParam,(void *)(sizeof(SaveParam)),0);
		memcpy(&stYZSaveParam,&stSaveParam,sizeof(SaveParam));
		DBG_Print(LEVEL2,"save param\r\n");
	}
}

void easy_change_param_arrive(void *ptr,unsigned int len)
{
	if (ptr && len)
	{
		memcpy(&stEasyChangeParam,ptr,(len > sizeof(EasyChangeParam)) ? sizeof(EasyChangeParam) : len);
		memcpy(&stYZEasyChangeParam,&stEasyChangeParam,sizeof(EasyChangeParam));

	}
}

void save_easy_change_param(void)
{
	if (memcmp(&stEasyChangeParam,&stYZEasyChangeParam,sizeof(EasyChangeParam)))
	{
		send_msg(S_MSG_WRITE_EASY_CHANGE_PARAM,&stEasyChangeParam,(void *)(sizeof(EasyChangeParam)),0);
		memcpy(&stYZEasyChangeParam,&stEasyChangeParam,sizeof(EasyChangeParam));
		DBG_Print(LEVEL2,"save easy change param\r\n");
	}
}

void dst_start_test_net_error_lock(void)
{
	set_dst_want_kucunrong_limit_speed(1);
	set_dst_want_lock_status(0);
	set_dst_want_limit_speed_level(4);
	set_dst_want_pro_ver_about_lock(0);


	gb32960_can_set_milage(1180);
	set_dst_fact_pro_ver_about_lock(1);
}

