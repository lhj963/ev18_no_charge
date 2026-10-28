#ifndef __ZHONGCHE_PLAT_H__
#define __ZHONGCHE_PLAT_H__

#ifdef __cplusplus
extern "C" {
#endif

//设备->平台命令
#define ZHONGCHE_LOGIN_CMD 				0xC0	//中车登录命令
#define ZHONGCHE_GPS_AND_CAN_DATA_CMD 	0xD0	//终端及 CAN 数据上传命令
#define ZHONGCHE_GZYC_DATA_CMD 			0xD2	//故障预测命令


#define ZHONGCHE_GPS_MSGID_HEARTBEAT 0x0181		//心跳
#define ZHONGCHE_GPS_MSGID_UNKOWNCMD 0x01D1		//未识别命令
#define ZHONGCHE_GPS_MSGID_EXDATA 	 0x0390		//终端附加数据
#define ZHONGCHE_GPS_MSGID_ASK_TC 	 0x0386		//CAN消息透传应答平台
#define ZHONGCHE_GPS_MSGID_ASK_SQ    0x0388		//授权应答平台

#define ZHONGCHE_CAN_MSGID_BASE 	0x02E8		//实时CAN基本信息上报


//平台->设备命令
#define ZHONGCHE_SETVIN_CMD 				0xC1	//设置VIN ICCID

#define ZHONGCHE_TC_CAN_MSG_SUB_CMD         0x0306		//企标透传CAN消息子命令
#define ZHONGCHE_SQ_SUB_CMD         		0x0308		//企标车辆授权消息子命令




typedef struct tagZCHeartBeatData
{
	INT8U yy;	//上报原因 0 心跳 1电话呼叫 2GPRS/短信呼叫
	INT32U lon;
	INT32U lat;
	INT16U speed;	//速度 单位0.5Km/h
	INT16U gaodu;	//高度
	INT16U fxj;		//方向角
	INT32U milage;	//里程
	INT16U hv;		//硬件版本号
	INT8U sv[4];	//软件版本号

	INT8U year;
	INT8U month;
	INT8U day;
	INT8U hour;
	INT8U min;
	INT8U sec;
	INT8U gps_md_status;		//GPS模块状态
}ZCHeartBeatData;

typedef struct tagZCExData
{
	INT8U status;
	INT16U main_power_vol;
	INT8U bat_vol;
}ZCExData;


typedef struct tagOneCanMsg
{
	INT8U is_ok_flag : 1;			//标识该帧数据是否有效
	INT8U channel : 2;
	INT32U canid;
	INT8U dat[8];
}OneCanMsg;

typedef struct tagZCCanBase
{
	INT8U reason;	//上报原因 0：正常上报；1:盲区补偿数据上报；2:远程呼叫数据上报(can帧时间)；3:包序号请求上报;4：AEBS回溯报文上报
	INT32U lon;
	INT32U lat;
	INT16U speed;	//速度 单位0.5Km/h
	INT16U gaodu;	//高度
	INT16U fxj;		//方向角
	INT8U utc_year;
	INT8U utc_month;
	INT8U utc_day;
	INT8U utc_hour;
	INT8U utc_min;
	INT8U utc_sec;
	INT8U utc_baifenzhiyi_sec;

	INT32U bagid;
}ZCCanBase;




extern unsigned int ZHONGCHE_LoginPack(void *pOutBuf,unsigned int outbuflen,
				void *pVIN,INT8U terminalid[6],void *iccid);

//附加数据
extern unsigned int ZHONGCHE_ExdataPack(void *pOutBuf,unsigned int outbuflen,ZCExData *pdat);

extern unsigned int ZHONGCHE_HeartBeatPack(void *pOutBuf,unsigned int outbuflen,ZCHeartBeatData *pdat);

//实时CAN基本信息上报
extern unsigned int ZHONGCHE_RealCanBasePack(void *pOutBuf,unsigned int outbuflen,char is_first_bag,ZCCanBase *pdat);



extern unsigned int ZHONGCHE_Guzhangyuce(void *pOutBuf,unsigned int outbuflen,INT8U time[6]);



extern void zhongche_analyze(INT8U linkid,void *pdat,unsigned int len);


extern void localtime_to_utc(char zone,INT8U local_year,INT8U local_month,INT8U local_day,INT8U local_hour,
								INT8U *utc_year,INT8U *utc_month,INT8U *utc_day,INT8U *utc_hour);


extern unsigned int zy_pdu(void *pdat,unsigned int datlen);


extern void update_can_msg(INT8U channel,INT32U canid,INT8U dat[8]);

extern void clr_report_can_msg(void);

extern INT16U get_total_can_id_cnt(void);

extern void zhongche_can_vin_analyze(CanRxMsg *prxmsg);

extern void zhongche_can_analyze(CanRxMsg *prxmsg);

extern char is_vin_ok(void *vin);

//判断车辆是否处于锁车状态
extern INT8U is_car_lock_status(void);
//判断车辆是否处于解锁状态
extern INT8U is_car_unlock_status(void);

extern INT8U get_car_lock_status(void);
extern INT8U get_plat_lock_car_cmd(void);

extern void set_plat_lock_car_cmd(INT8U val);

#ifdef __cplusplus
}
#endif

#endif

