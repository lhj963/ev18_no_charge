/*************************************************************************
**  名称: Protocol.h文件
*   日期: 2019/3/27
*   作者: chensufei
*   描述: 提供平台协议通信接口和数据结构的文件
*		
*   修改记录: 
*	   日期						修改内容
*	2019/3/27		|		软件初始化版本，提供函数有:
					|		a.
**************************************************************************/
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//#include "./SysParam/Config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "./Common/Comm_Func.h"
//#include "./SysParam/System_Param.h"

#define M_ICCID_LEN					(20)		/* ICCID的长度 */
#define M_VIN_LEN					(17)		/* VIN码长度 */
#define M_SN_LEN					(24)		/* 设备的SN长度 */

#define M_IMEI_LEN					(15)		/* IMEI的长度 */

#define M_CALID_LEN					(18)		/* 校验标定识别号长度 */
#define M_CVNID_LEN					(18)		/* 标定验证码长度 */
#define M_IUPR_LEN					(36)		/* IUPR值长度 */



#define M_IP_DOMAIN_LEN				(32)		/* 支持最大IP和域名的长度 */

#define M_FTP_FILE_PATH_LEN			(40)		/* 文件目录 */
#define M_FTP_FILE_NAME_LEN			(36)		/* FTP升级文件长度 */

#define M_PROTO_KEY_LEN				(64)		/* 公钥长度 */
#define M_PROTO_CHIP_ID_LEN			(16)		/* 芯片ID */


/**************************************************************************
** 宏定义
**************************************************************************/
#define M_CN6_CALID_LEN					(18)	/* 国6的校验标定识别号长度 */
#define M_BJ5_CALID_LEN					(16)	/* 京5的校验标定识别号长度 */
#define M_PROTO_MAX_SERIAL_VAL			(65531)	/* 协议号最大值 */
#define M_PROTO_SERIAL_INIT				(1)		/* 序列号初始值 */

/**************************************************************************
** 结构体声明
**************************************************************************/
/* 报文中流水号 */
typedef struct _DataSerialSt
{
	u16 lastLogin;			/* 对应登入的流水号，作为当次登出的流水号 */
	u16 login;				/* 登入流水号, 默认从1开始 */
	u16 realtime;			/* 实时数据流水号 */
	u16 reissue;			/* 补偿数据流水号 */
	u16 disasalarm;			/* 防拆报警流水号 */
} DataSerialSt;

/* 依赖的额登入信息数据 */
typedef struct _LoginInfoSt
{
	CommDateTimeSt time;			/* 登入的时间 */
	u8 iccid[M_ICCID_LEN];			/* ICCID信息 */
} __attribute__((packed)) LoginInfoSt;

/* OBD诊断协议类型 */
typedef enum _ObdDiagProtoTypeE
{
	E_OBD_PROTO_IOS15765 = 0,		/* ISO15865协议 */
	E_OBD_PROTO_IOS27145,			/* IOS27145协议 */
	E_OBD_PROTO_SAEJ1939,			/* SAEJ1939协议 */
	E_OBD_PROTO_INVALID = 0xfe,		/* 无效协议 */
} ObdDiagProtoTypeE;

/* 备案失败的原因 */
typedef enum _FilingFailReasonE
{
	E_PROTO_CHIP_ID_ERR = 0x01,		/* 芯片ID错误 */
	E_PROTO_CHIP_FILED,				/* 芯片已备案 */
	E_PROTO_KEY_ERR,				/* 密钥错误(弃用) */
	E_PROTO_VIN_ERR,				/* VIN错误 */
} FilingFailReasonE;

/* OBD数据信息 */
typedef struct _ObdCommDataSt
{
	u8 diagProto;					/* 诊断协议,0-2有效值范围，0-IOS15765,1-IOS27145,2-SAEJ1939,0xfe-无效 */
	u8 milState;					/* MIL状态，0-未点亮，1-点亮，0xfe-无效 */

    __packed union
	{
		u16 state;					/* 诊断支持状态的使能位,0-不支持，1-支持 */
		__packed struct
		{
			u16 calstMonSta:1;				/* 崔化转化器监控状态 */
			u16 hertCalsMonSta:1;			/* 加热催化转化器监控状态 */
			u16 eptSysMonSta:1;				/* 蒸发系统监控 */
			u16 secAirSysMonSta:1;			/* 二次空气系统监控 */
			u16 aCSysRefMonSta:1;			/* A/C系统致冷剂监控 */
			u16 exhtSenMonSta:1;			/* 排气传感器加热器监控 */
			u16 exhtSenHertMonSta:1;		/* 排气传感器加热器监控 */
			u16 egrVvtSysMonSta:1;			/* EGR系统和VVT监控 */
			u16 coldAidSysMonSta:1;			/* 冷启动辅助系统监控 */
			u16 bstPreCtrlSysMonSta:1;		/* 增压压力控制系统 */
			u16 dstPrtlFlteMonSta:1;  		/* DPF监控 */
			u16 noxCvertCtstAdbrMonSta:1;	/* 选择性催化还原系统(SCR)或NOx吸附器 */
			u16 nmhcCvertCtstMonSta:1;	 	/* NMHC氧化催化器监控 */
			u16 mfireMonSupptSta:1;		 	/* 失火监控 */
			u16 fuelSysMonSupptSta:1;	 	/* 燃油系统监控状态 */
			u16 cphvCmptMonSupptSta:1;	 	/* 综合成分监控 */
		} bitSt;
	} diagSupptStaUn;

	__packed union
	{
		u16 state;							/* 诊断就绪状态*/
		__packed struct 
		{
			u16 calstMonSta:1;				/* 崔化转化器监控状态 */
			u16 hertCalsMonSta:1;			/* 加热催化转化器监控状态 */
			u16 eptSysMonSta:1;				/* 蒸发系统监控 */
			u16 secAirSysMonSta:1;			/* 二次空气系统监控 */
			u16 aCSysRefMonSta:1;			/* A/C系统致冷剂监控 */
			u16 exhtSenMonSta:1;			/* 排气传感器加热器监控 */
			u16 exhtSenHertMonSta:1;		/* 排气传感器加热器监控 */
			u16 egrVvtSysMonSta:1;			/* EGR系统和VVT监控 */
			u16 coldAidSysMonSta:1;			/* 冷启动辅助系统监控 */
			u16 bstPreCtrlSysMonSta:1;		/* 增压压力控制系统 */
			u16 dstPrtlFlteMonSta:1;  		/* DPF监控 */
			u16 noxCvertCtstAdbrMonSta:1;	/* 选择性催化还原系统(SCR)或NOx吸附器 */
			u16 nmhcCvertCtstMonSta:1;	 	/* NMHC氧化催化器监控 */
			u16 mfireMonSupptSta:1;	  	 	/* 失火监控 */
			u16 fuelSysMonSupptSta:1; 	 	/* 燃油系统监控状态 */
			u16 cphvCmptMonSupptSta:1;	 	/* 综合成分监控 */
		} bitSt;
	} diagReadyStaUn;

	u8 vin[M_VIN_LEN];						/* vin码信息 */
}__attribute__((packed)) ObdCommDataSt;

/* 故障信息 */
typedef struct _ObdErrInfoSt
{
	u8 errCnt;								/* 故障码总数，有效值范围0-253,无效值'0xFE' */
	u32 *errCode;							/* 故障码信息列表 */
} ObdErrInfoSt;

/* 京5 OBD信息 */
typedef struct _Bj5ObdDataSt
{
	ObdCommDataSt comm;						/* 京5和国6公共的数据部分 */
	u8 calId[M_BJ5_CALID_LEN];				/* 校准标定识别号,软件标定识别号由生产企业自定义，字母或数字组成，不足后面补字符"0" */
	u8 cvnId[M_CVNID_LEN];					/* 标定验证码,标定验证码由生产企业自定义，字母或数字组成，不足后面补字符"0" */
	u8 iuprVal;								/* IUPR值，直接从CAN模块中获取 */
	ObdErrInfoSt err;						/* 故障码信息 */
} Bj5ObdDataSt;

/* 国6 OBD信息 */
typedef struct _Cn6ObdDataSt
{
	ObdCommDataSt comm;						/* 京5和国6公共的数据部分 */
	u8 calId[M_CN6_CALID_LEN];				/* 校准标定识别号,软件标定识别号由生产企业自定义，字母或数字组成，不足后面补字符"0" */
	u8 cvnId[M_CVNID_LEN];					/* 标定验证码,标定验证码由生产企业自定义，字母或数字组成，不足后面补字符"0" */
	u8 iuprVal[M_IUPR_LEN];					/* IUPR值，直接从CAN模块中获取 */
	ObdErrInfoSt err;						/* 故障码信息 */
} Cn6ObdDataSt;

/* 数据流信息数据格式 */
typedef struct _DataStreamSt
{
	u16 carSpeed;			/* 车速,精度1/256 km/h每位, 范围0~250.996km/h, '0xff,0xff'无效 */
	u8 airPress;			/* 大气压力值,精度0.5kpa/bit,范围0-125kpa, '0xff'无效 */
	u8 engineOutTorq;		/* 发送动机净输出扭矩,精度1%bit,范围-125~125%,偏移量-125 */
	u8 fricTorq;			/* 摩擦扭矩,精度1%bit,范围-125~125%,偏移量-125 */
	u16 engineSpeed;		/* 发动机转速,精度0.125rpm/bit,0-8031.875rpm,'0xff,0xff'表示无效 */
	u16 engineFuel;			/* 发动机燃料流量,0.05L/h,0~3212.75L/h,'0xff,0xff'表示无效 */
	u16 scrUpNoxSensorOut;	/* SCR上游NOx传感器输出值,偏移量-200,0.05ppm/bit, -200~3012.75ppm,'0xff,0xff'无效 */
	u16 scrDownNoxSensorOut;/* SCR下游NOx传感器输出值,偏移量-200,0.05ppm/bit, -200~3012.75ppm,'0xff,0xff'无效 */
	u8 rgentBalance;		/* 反应剂余量,精度0.4%/bit,0~100%,'0xff'无效 */
	u16 airIsntake;			/* 进气量,精度0.05kg/h/bit,0~3212.75kg/h,'0xff,0xff'无效 */
	u16 scrInTmper;			/* SCR入口温度,0.03125deg C/bit,范围-273~1734.96875deg C, '0xff,0xff'无效 */
	u16 scrOutTmper;		/* SCR出口温度,0.03125deg C/bit,范围-273~1734.96875deg C, '0xff,0xff'无效 */
	u16 dpfPressDiff;		/* DPF压差,0.1kpa/bit, 0~6425.5kpa,'0xff,0xff'无效 */
	u8 engineCoolantTmper;	/* 发动机冷却液温度,1deg C/bit, -40~210 degC, '0xff'无效 */
	u8 oilLevel;			/* 油箱液位，0.4%bit, 0 ~ 100%,'0xff'无效 */
	__packed union
	{
		u8 state;			/* 定位状态, */
		__packed struct
		{
			u8 gpsSta:1;	/* 0 - 有效定位，1 - 无效定位;当网络连接正常时，无定位的情况下，发送最后一个定位信息，并置为无效定位 */
			u8 latSta:1;	/* 纬度状态，0 - 北纬，1-南纬 */
			u8 lonSta:1;	/* 精度状态，0 - 东经，1-西经 */
			u8 restore:5;	/* 其他保留 */
		} bitSt;
	} postionStaUn;

	u32 lonVal;				/* 经度值,0.000001°/bit,范围0 ~ 180.000000°,'0xff,0ff,0xff,0xff'无效 */
	u32 latVal;				/* 纬度值,0.000001°/bit,范围0 ~ 90.000000°,'0xff,0ff,0xff,0xff'无效 */
	u32 sumMil;				/* 总里程数，0.1km/bit,'0xff,0xff,0xff,0xff'无效 */
}__attribute__((packed)) DataStreamSt;

/* 上海重型柴油车实时补充数据信息 */
typedef struct _AddDataStreamSt
{
	u8 engineTorqMode;		/* 发动机扭矩模式,0 - 超速失效，1-转速控制，2-扭矩控制，3-转速/扭矩控制 */
	u8 aceletorPedal;		/* 油门踏板，0xff-无效 */
	u32 sumFuel;			/* 总油耗 */
	u8 ureaBoxTemp;			/* 尿素箱温度 */
	u32 actualUreaInjet;	/* 实际尿素喷射量 */
	u32 sumUreaConsumption;	/* 总尿素消耗 */
	u16 dpfExhstTemp;		/* DPF 排气温度 */
} __attribute__((packed)) AddDataStreamSt;

/* 广东旭诚杭标实时补充数据信息 */
typedef struct _AddDataXcHzStreamSt
{
	AddDataStreamSt shh;	/* 上海附加数据 */
	u32 sumEngineRuntm;		/* 发动机总运行时间, "0xFF，0xFF，0xFF，0xFF"表示无效 */
} __attribute__((packed)) AddDataXcHzStreamSt;

/* 杭州地标补充的数据信息 */
typedef struct _AddDataHzStreamSt
{
	u16 environmentTmper;	/* 环境温度，0xffff - 无效 */
	u8 aceletorPedal;		/* 油门踏板，0xff-无效 */
	u8 footBrakeSta;		/* 脚刹状态, 0x00：off,0x01：on,0xff - 无效 */
	u16 batteryVol;			/* 蓄电池电压,数据范围：0~ 3212.75V,0xffff - 无效 */
	u32 sumFuel;			/* 总油耗,"0xFF，0xFF，0xFF，0xFF"表示无效 */
	u16 refTorque;			/* 参考扭矩,"0xFF，0xFF"表示无效 */
	u32 sumEngineRuntm;		/* 发动机总运行时间, "0xFF，0xFF，0xFF，0xFF"表示无效 */
	u8 ureaTankLevelScr;	/* 尿素箱液位（SCR）,"0xFF"表示无效 */
	u8 ureaBoxTemp;			/* 尿素箱温度（SCR）,"0xFF"表示无效 */
	u16 dpfExhstTemp;		/* DPF 排气温度 */
	u16 instFuel;			/* 瞬时油耗 */
	u8 engineTorqMode;		/* 发动机扭矩模式,0 - 超速失效，1-转速控制，2-扭矩控制，3-转速/扭矩控制,9-正常 */
	u32 actualUreaInjet;	/* 实际尿素喷射量 */
	u16 upExhstTemper;		/* 后处理上游排气温度 */
	u16 downExhstTemper;	/* 后处理下游排气温度 */
	u32 sumUreaConsumption;	/* 总尿素消耗 */
	u8 dpfExhstBackPress;	/* DPF/POC 排气背压 */
} __attribute__((packed)) AddDataHzStreamSt;

/* 实时数据分成两种，1-OBD诊断实时数据，2-数据流实时信息 */
typedef enum _RealtimeDataTypeE
{
	E_REALTIME_OBD_DIAG,	/* OBD诊断实时数据 */
	E_REALTIME_STREAM_DATA,	/* 数据流实时信息 */
} RealtimeDataTypeE;

/* OBD实时数据信息 */
typedef struct _ObdRealtimeInfoSt
{
	union
	{
		Bj5ObdDataSt bj5;			/* 京5的OBD信息 */
		Cn6ObdDataSt cn6;			/* 国6的OBD信息 */
	} obdUn;
} ObdRealtimeInfoSt;

/* 数据流实时信息 */
typedef struct _StreamRealtimeInfoSt
{
	u8 count;						/* 最大数据流条数，10条 */
	DataStreamSt *data;				/* 依赖的流数据 */
	AddDataStreamSt *addData;		/* 上海重型柴油车的附加的数据信息 */
	AddDataHzStreamSt *addHzData;	/* 杭州附加的数据信息 */
	AddDataXcHzStreamSt *addXcHzData;/* 广东旭诚杭标协议 */
} StreamRealtimeInfoSt;

/* 实时数据结构体 */
typedef struct _RealtimeInfoSt
{
	CommDateTimeSt time;			/* 采集的时间 */
	RealtimeDataTypeE type;			/* 实时数据类型 */
	union
	{
		ObdRealtimeInfoSt obd;		/* OBD实时信息 */
		StreamRealtimeInfoSt stream;/* 数据流实时信息 */
	} realtimeUn;
} RealtimeInfoSt;

/* 车辆登出信息 */
typedef struct _LogoutInfoSt
{
	CommDateTimeSt time;			/* 登出时间 */
} LogoutInfoSt;

/* 数据防篡改基础信息自动备案 */
typedef struct _FilingInfoSt
{
	CommDateTimeSt time;			/* 数据采集时间 */
	u8 chipId[M_PROTO_CHIP_ID_LEN];	/* 芯片ID */
	u8 key[M_PROTO_KEY_LEN];		/* 公钥 */
	u8 vin[M_VIN_LEN];				/* VIN码 */
} FilingInfoSt;

/* 拆机报警 */
typedef struct _DisasAlarmInfoSt
{
	CommDateTimeSt time;			/* 报警时间 */
	union
	{
		u8 state;					/* 定位状态, */
		struct
		{
			u8 gpsSta:1;			/* 0 - 有效定位，1 - 无效定位;当网络连接正常时，无定位的情况下，发送最后一个定位信息，并置为无效定位 */
			u8 latSta:1;			/* 纬度状态，0 - 北纬，1-南纬 */
			u8 lonSta:1;			/* 精度状态，0 - 东经，1-西经 */
			u8 restore:5;			/* 其他保留 */
		} bitSt;
	} locStaUn;
	u32 lat;						/* 经度 */
	u32 lon;						/* 纬度 */
} DisasAlarmInfoSt;

/* 命令单元，每一条报文的标识 */
typedef enum _DataCmdTypeE
{
	E_PROTO_CMD_START = 0,
	E_PROTO_CMD_VEH_LOGIN,				/* 车辆登入,上行 */
	E_PROTO_CMD_REALTIME_PACK,			/* 实时信息上报,上行 */
	E_PROTO_CMD_REISSUE_PACK,			/* 补发信息上报,上行 */
	E_PROTO_CMD_VEH_LOGOUT,				/* 车辆登出,上行 */
	E_PROTO_CMD_DEV_TIME = 0x05,		/* 终端设备向平台校时,上行 */
	E_PROTO_CMD_DISAS_ALRAM = 0x6,		/* 拆机报警,上行 */
	E_PROTO_CMD_TAMPER_FILING = 0x7,	/* 数据防篡改基础信息自动备案,上行 */
	E_PROTO_CMD_TAMPER_FILING_ACK = 0x8,/* 备案结果应答,下行 */

	E_PROTO_CMD_XC_PUB_KEY_ID = 0x82,	/* 备案防篡改信息 */
	E_PROTO_CMD_XC_FILING_DATA_ACK,		/* 旭诚科技，备案结果应答,包括下方的公钥信息,下行 */
	E_PROTO_CMD_XC_CFG_IP_PORT,			/* 配置IP和端口信息 */
} DataCmdTypeE;

/**************************************************************************
** 全局变量声明
**************************************************************************/

/**************************************************************************
** 函数声明
**************************************************************************/
/**************************************************************************
* 函  数: DataSerialSt ProtocolGetSerial(u8 index)
* 描  述: 获取某一路通道的序列号信息
* 举  例: 
* 入  参: u8 index:数据通道
* 出  参: void
* 返回值: DataSerialSt*: 序列号信息
**************************************************************************/
DataSerialSt* ProtocolGetSerial(u8 index);

/**************************************************************************
* 函  数: int ProtocolCalcZaHashVal(u8 *za_hash, u32 maxlen)
* 描  述: 计算得到ZA哈希值
* 举  例: 
* 入  参: u32 maxlen:最大组包长度
* 出  参: u8 *za_hash:计算签名时的哈希值
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolCalcZaHashVal(u8 *za_hash, u32 maxlen);

/**************************************************************************
* 函  数: static int ProtocolPackSignFormatData(u8 *signData, int len, u8 *outdata)
* 描  述: 组包签名格式数据， rlen + rval + slen + sval
* 举  例: 
*		64个字节的签名数据
*		rlen = 0x40, rval = 16进制 ASCII 编码 R值
*		slen = 0x40, sval = 16进制 ASCII 编码 S值

* 入  参: u8 *signData:签名数据
*		  int len: 签名数据长度
* 出  参: u8 *outdata: rlen + rval + slen + sval格式的数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolPackSignFormatData(u8 *signData, int len, u8 *outdata);

/**************************************************************************
* 函  数: int ProtocolHeaderData(DataCmdTypeE cmd, ProtocolTypeE proType, u8 *outdata)
* 描  述: 平台数据协议头组包
* 举  例: 
* 入  参: DataCmdTypeE cmd:命令类型
*		  ProtocolTypeE proType：协议类型
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolHeaderData(DataCmdTypeE cmd, ProtocolTypeE proType, u8 *outdata);

/**************************************************************************
* 函  数: void ProtocolLoginData(u8 index, ProtocolTypeE type, LoginInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包登入报文
* 举  例: 
* 入  参: u8 index:数据通道
*		  ProtocolTypeE type: 协议类型
*		  LoginInfoSt *data: 传入组包协议的数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolLoginData(u8 index, ProtocolTypeE type, LoginInfoSt *data, u8 *outdata, u32 maxlen);

/**************************************************************************
* 函  数: void ProtocolLogoutData(u8 index,ProtocolTypeE type, LogoutInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包登出报文
* 举  例: 
* 入  参: u8 index:数据通道
*		  ProtocolTypeE type: 协议类型
*		  LogoutInfoSt *data: 传入组包协议的数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolLogoutData(u8 index, ProtocolTypeE type, LogoutInfoSt *data, u8 *outdata, u32 maxlen);

/**************************************************************************
* 函  数: void ProtocolRealtimeData(u8 index, ProtocolTypeE type, RealtimeInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包实时信息报文
* 举  例: 
* 入  参: u8 index:数据通道
*		  ProtocolTypeE type: 协议类型
*		  RealtimeInfoSt *data: 传入组包协议的数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolRealtimeData(u8 index, ProtocolTypeE type, RealtimeInfoSt *data, u8 *outdata, u32 maxlen);

/**************************************************************************
* 函  数: int ProtocolFilingData(FilingInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包自动备案数据
* 举  例: 
* 入  参: FilingInfoSt *data:不同的命令单元，传入自动备案数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolFilingData(FilingInfoSt *data, u8 *outdata, u32 maxlen);

/**************************************************************************
* 函  数: int ProtocolParseData(u8 index, u8 *data, int datalen, void *outdata)
* 描  述: 平台数据协议，解析数据，输出需要的数据
* 举  例: 
* 入  参: u8 index: 第几路链路
*	      u8 *data：接收到的数据流信息
*		  int datalen:接收到的数据流信息长度
* 出  参: void *outdata:输出数据信息
* 返回值: int: M_COMM_FUNC_FAIL - 应答失败，M_COMM_FUNC_SUCC - 应答成功
**************************************************************************/
int ProtocolParseData(u8 index, u8 *data, int datalen, void *outdata);

/**************************************************************************
* 函  数: int ProtocolFilingData(DisasAlarmInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包防拆报警
* 举  例: 
* 入  参: u8 index:对应某一路
*		  DisasAlarmInfoSt *data:不同的命令单元，传入自动备案数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolDisasAlarmData(u8 index, DisasAlarmInfoSt *data, u8 *outdata, u32 maxlen);

/**************************************************************************
* 函  数: void ProtocolBigendianStreamData(DataStreamSt *stream)
* 描  述: 对数据流信息进行大端转化
* 举  例: 
* 入  参: DataStreamSt *stream：数据流信息
* 出  参: DataStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianStreamData(DataStreamSt *stream);

/**************************************************************************
* 函  数: void ProtocolBigendianAddStreamData(AddDataStreamSt *stream)
* 描  述: 对上海重型柴油车进行附加数据流信息进行大端转化
* 举  例: 
* 入  参: AddDataStreamSt *stream：数据流信息
* 出  参: AddDataStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianAddStreamData(AddDataStreamSt *stream);
/**************************************************************************
* 函  数: void ProtocolBigendianAddHzStreamData(AddDataHzStreamSt *stream)
* 描  述: 对杭州地标进行附加数据流信息进行大端转化
* 举  例: 
* 入  参: AddDataHzStreamSt *stream：数据流信息
* 出  参: AddDataHzStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianAddHzStreamData(AddDataHzStreamSt *stream);

/**************************************************************************
* 函  数: void ProtocolInit(void)
* 描  述: 平台数据协议，需要用到的数据初始化
* 举  例: 
* 入  参: 
* 出  参: void
* 返回值: void
**************************************************************************/
void ProtocolInit(void);

#ifdef __cplusplus
}
#endif

#endif



