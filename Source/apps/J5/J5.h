#ifndef __J5_H__
#define __J5_H__

#ifdef __cplusplus
extern "C" {
#endif

#define J5_HEAD0 0x23
#define J5_HEAD1 0x23

#define J5_CMD_LOGIN 			0x01
#define J5_CMD_REAL_INFO 		0x02
#define J5_CMD_SUPPLY_INFO 		0x03
#define J5_CMD_LOGOUT 			0x04
#define J5_CMD_CALIBRATIONTIME 	0x05


#define J5_OBD_INFO			0x01
#define J5_DATA_STREAM_INFO 	0x02

#define J5_OFFSET_DATA_UNIT (2+1+J5_LEN_VIN+1+1+2)	//数据单元的偏移



#define J5_LEN_VIN			17		//VIN长度
#define J5_LEN_SOFTVER		18		//软件标定识别号长度
#define J5_LEN_CVN			18		//标定验证码
#define J5_LEN_IUPR         1


typedef struct tagJ5_OBDInfo
{
	INT8U BCDTime[6];
	INT8U ucOBDProtocol;	//OBD诊断协议	有效范围0-2  0:ISO15765 1:ISO27145 2:SAEJ1939 0xFE无效
	INT8U ucMilStatus;		//0:未点亮 1:点亮 0xFE:无效
	INT16U usSupportStatus;	//诊断支持状态
	INT16U usReadyStatus;	//诊断就绪状态
	INT8U szVIN[J5_LEN_VIN];
	INT8U szCalid[J5_LEN_SOFTVER + 1];	//软件标定识别号
	INT8U szCVN[J5_LEN_CVN + 1];		//标定验证码
	INT8U szIUPR[J5_LEN_IUPR + 1];		//IUPR
	INT8U ucDTCTotalNum;		//故障码总数
	INT8U *pDTCList;		//故障码列表
}J5_OBDInfo;

typedef struct tagJ5_DataStreamInfo
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
}J5_DataStreamInfo;

extern void J5_Init(void *szVIN,INT8U Ver,INT8U Way);

extern int J5_LoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID);

extern int J5_LogoutDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6]);

extern int J5_RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_OBDInfo *pOBDInfo);


//补传OBD信息
extern int J5_SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_OBDInfo *pOBDInfo);

//实时数据流信息
extern int J5_RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo);


//多包实时数据流信息
extern int J5_RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);

//补传数据流信息
extern int J5_SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo);

//多包补传数据流信息
extern int J5_SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo,unsigned int bagcnt);



#ifdef __cplusplus
}
#endif

#endif


