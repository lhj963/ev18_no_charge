#ifndef __DONGFENG_PLAT_H__
#define __DONGFENG_PLAT_H__

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



extern unsigned int xGB32960_DongfengBasicPack(void *pOut,unsigned int outbuflen,void *pVIN);


extern unsigned int xGB32960_DongfengNewBasicPack(void *pOutBuf,INT16U usOutBufSize,
																	void *pVIN,
																	INT8U t[6],
																	void *piccid,unsigned int iccid_len,
																	void *pesn,unsigned int esn_len
																	);

#ifdef __cplusplus
}
#endif

#endif

