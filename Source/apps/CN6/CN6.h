#ifndef __CN6_H__
#define __CN6_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HEAD0 0x23
#define HEAD1 0x23

#define CMD_LOGIN 			0x01
#define CMD_REAL_INFO 		0x02
#define CMD_SUPPLY_INFO 	0x03
#define CMD_LOGOUT 			0x04
#define CMD_CALIBRATIONTIME 0x05

#define CMD_BEIAN 			0x07
#define CMD_BEIAN_ACK 		0x08



#define OBD_INFO					0x01
#define DATA_STREAM_INFO 			0x02
#define SH_DATA_STREAM_EX_INFO 		0x80	//上海补充数据流
#define KELIWUCHUANGANQI_INFO   	0x82	//颗粒物传感器信息 


#define OFFSET_DATA_UNIT (2+1+LEN_VIN+1+1+2)	//数据单元的偏移



#define LEN_VIN			17		//VIN长度
#define LEN_SOFTVER		18		//软件标定识别号长度
#define LEN_CVN			18		//标定验证码
#define LEN_IUPR        36


typedef struct tagOBDInfo
{
	INT8U BCDTime[6];
	INT8U ucOBDProtocol;	//OBD诊断协议	有效范围0-2  0:ISO15765 1:ISO27145 2:SAEJ1939 0xFE无效
	INT8U ucMilStatus;		//0:未点亮 1:点亮 0xFE:无效
	INT16U usSupportStatus;	//诊断支持状态
	INT16U usReadyStatus;	//诊断就绪状态
	INT8U szVIN[LEN_VIN + 1];
	INT8U szCalid[LEN_SOFTVER + 1];	//软件标定识别号
	INT8U szCVN[LEN_CVN + 1];		//标定验证码
	INT8U szIUPR[LEN_IUPR + 1];		//IUPR
	INT8U ucDTCTotalNum;		//故障码总数
	INT8U *pDTCList;		//故障码列表
}OBDInfo;

typedef struct tagDataStreamInfo
{
	INT8U BCDTime[6];
	INT16U usSpeed;	//车速 1/256 KM/h per bit
	INT8U uckPa;		//大气压力 0.5kPa/bit
	INT8S cActualTorquePercent;	//发动机实际扭矩与指示扭矩的百分比 范围:-125~125%
	INT8S cFrictionTorquePercent;	//摩擦扭矩百分比 范围:-125~125%
	INT16U usRPM;		//发动机转速 0.125rpm/bit
	INT16U usFuelFlow;	//燃料流量
	INT16S sUpperNOx;	//SCR上游氮氧化合物 0.05ppm/bit 数据范围:-200~3012.75ppm
	INT16S sLowerNOx;	//SCR下游氮氧化合物
	INT8U ucRemain;		//反应剂余量 0~100%
	INT16U usIntakeVolume;	//进气量 0.05Kg/h per bit

	INT16S sSCRInletTemperature;	//SCR入口温度 0.03125deg C/bit
	INT16S sSCROutletTemperature;	//SCR出口温度

	INT16U usDPF;		//DPF压差 0.1kPa/bit

	INT8S cCoolantTemperature;	//发动机冷却液温度

	INT8U ucLevel;		//邮箱液位
	INT8U ucGpsStatus;		//定位状态
	INT32U ulLong;		//经度
	INT32U ulLat;		//纬度
	INT32U ulMilage;	//累计里程

	INT16U guangxishouxishu;
	INT16U butouguangdu;
	INT16U keliwunongdu;
}DataStreamInfo;

typedef struct tagSHDataStreamExInfo
{
	INT8U BCDTime[6];			//采集时间
	INT8U engine_nj_mode;		//发动机扭矩模式 0：超速失效 1：转速控制 2：扭矩控制 3：转速/扭矩控制
	INT8U ymtb;					//油门踏板
	INT32U total_oil;			//累计油耗
	INT8U nsxwd;				//尿素箱温度
	INT32U sjnspsl;				//实际尿素喷射量
	INT32U ljnsxh;				//累计尿素消耗
	INT16U dpfpqwd;				//DPF 排气温度  

}SHDataStreamExInfo;


typedef struct tagSHALLDataStreamInfo
{
	INT8U BCDTime[6];
	INT16U usSpeed;	//车速 1/256 KM/h per bit
	INT8U uckPa;		//大气压力 0.5kPa/bit
	INT8S cActualTorquePercent;	//发动机实际扭矩与指示扭矩的百分比 范围:-125~125%
	INT8S cFrictionTorquePercent;	//摩擦扭矩百分比 范围:-125~125%
	INT16U usRPM;		//发动机转速 0.125rpm/bit
	INT16U usFuelFlow;	//燃料流量
	INT16S sUpperNOx;	//SCR上游氮氧化合物 0.05ppm/bit 数据范围:-200~3012.75ppm
	INT16S sLowerNOx;	//SCR下游氮氧化合物
	INT8U ucRemain;		//反应剂余量 0~100%
	INT16U usIntakeVolume;	//进气量 0.05Kg/h per bit

	INT16S sSCRInletTemperature;	//SCR入口温度 0.03125deg C/bit
	INT16S sSCROutletTemperature;	//SCR出口温度

	INT16U usDPF;		//DPF压差 0.1kPa/bit

	INT8S cCoolantTemperature;	//发动机冷却液温度

	INT8U ucLevel;		//邮箱液位
	INT8U ucGpsStatus;		//定位状态
	INT32U ulLong;		//经度
	INT32U ulLat;		//纬度
	INT32U ulMilage;	//累计里程


	//INT8U BCDTime[6];			//采集时间
	INT8U engine_nj_mode;		//发动机扭矩模式 0：超速失效 1：转速控制 2：扭矩控制 3：转速/扭矩控制
	INT8U ymtb;					//油门踏板
	INT32U total_oil;			//累计油耗
	INT8U nsxwd;				//尿素箱温度
	INT32U sjnspsl;				//实际尿素喷射量
	INT32U ljnsxh;				//累计尿素消耗
	INT16U dpfpqwd;				//DPF 排气温度  
}SHALLDataStreamInfo;


extern void CN6_Init(void *szVIN,INT8U Ver,INT8U Way);


//登录包
extern int LoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID);

//登出包
extern int LogoutDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID);

//终端校时包
extern int CalibrationTimePacket(void *pOutBuf,INT16U usOutBufSize);

//实时OBD信息
extern int RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);

//补传OBD信息
extern int SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);



//实时数据流信息
extern int RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo);

//补传数据流信息
extern int SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo);


//多包实时数据流信息
extern int RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//多包补传数据流信息
extern int SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//多包实时OBD和数据流信息
extern int GB17691_RealDataStreamAndObdInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);


extern int GB17691_SupplyDataStreamAndObdInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//实时数据流信息
extern int GB17691_RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo);

//多包实时数据流信息
extern int GB17691_RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//补传数据流信息
extern int GB17691_SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo);


//多包补传数据流信息
extern int GB17691_SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);


//多包实时数据流信息
extern int GB17691_RealDataStreamInfoExPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);


/*------------------------上海平台----------------------------------------*/
extern int SHLoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID);


//上海多包实时数据流信息
extern int GB17691_SHRealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

extern int GB17691_SHSupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//多包实时补充数据流信息
extern unsigned int GB17691_SHRealDataStreamExInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHDataStreamExInfo *pDataStreamInfo,unsigned int bagcnt);

//多包补传补充数据流信息
extern int GB17691_SHSupplyDataStreamExInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHDataStreamExInfo *pDataStreamInfo,unsigned int bagcnt);

//多包实时数据流信息
extern int GB17691_SHALLRealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHALLDataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

extern int GB17691_SHALLSupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHALLDataStreamInfo *pDataStreamInfo,unsigned int bagcnt);


//上海实时OBD信息
extern int SHRealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);

//上海补传OBD信息
extern int SHSupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);




extern unsigned char get_bengbu_login_flag(void);

//起始符 + 命令单元 + VIN + 应答标志
extern void anlyze_bengbu_server_data(void *pbuf,unsigned int len);


extern void anlyze_shanghai_server_data(void *pbuf,unsigned int len);


extern void anlyze_17691_server_data(unsigned int linkid,void *pbuf,unsigned int len);




extern unsigned int hj1239_beian_pack(unsigned char rtime[6],
										void *chipid,unsigned int chipid_len,
										void *public_key,unsigned int pubkey_len,
										void *vin,unsigned int vin_len,
										void *pOut,unsigned int outbuflen);


//HJ1239多包实时数据流信息
extern int HJ1239_RealDataStreamPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

extern int HJ1239_SupplyDataStreamPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//HJ1239实时OBD信息
extern int HJ1239_RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);


//HJ1239补传OBD信息
extern int HJ1239_SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo);


extern unsigned int df_17691_baseinfo_pack(void *pOutBuf,INT16U usOutBufSize,
												INT8U t[6],
												void *chipid,unsigned int chipid_len,
												void *piccid,unsigned int iccid_len,
												void *pesn,unsigned int esn_len
												);


//上报备案结果到东风平台
extern unsigned int df_17691_beian_result_pack(void *pOutBuf,INT16U usOutBufSize,
														INT8U t[6],
														void *beian_info,unsigned int beian_info_len,
														INT8U ack_code,INT8U ack_info);


//上报备案结果到东风平台
extern unsigned int df_17691_beian_result_pack1(void *pOutBuf,INT16U usOutBufSize,
														INT8U t[6],
														void *chip_id,unsigned int chipidlen,
														void *pubkey,unsigned int pubkeylen,
														void *pvin,unsigned int vinlen,
														void *phj1239_sign,unsigned int hj1239_sign_len,
														INT8U ack_code,INT8U ack_info);

extern void send_hj1239_beian_info_to_server(void *ip,unsigned int port);



#ifdef __cplusplus
}
#endif

#endif


