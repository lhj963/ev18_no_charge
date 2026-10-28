/*************************************************************************
**  名称: Protocol.c文件
*   日期: 2019/3/27
*   作者: chensufei
*   描述: 提供平台协议通信数据的接口
*   修改记录: 
*	   日期		修改人		版本 		修改内容
*	2019/3/26 	chensufei	V0.01       软件初始化版本，
**************************************************************************/
#include "Protocol.h"

/**************************************************************************
** 宏定义
**************************************************************************/
#define M_PROTO_HEAD_LEN				(2)		/* 数据协议头长度 */
#define M_PROTO_HEAD_FLAG				(0x23)	/* 协议头标识号 */

#define M_PROTO_SIGN_DATA_LEN			(64)	/* 签名数据的长度 */

/**************************************************************************
** 结构体声明
**************************************************************************/

/* 车辆登入数据结构 */
typedef struct _VehLoginDataSt
{
	CommDateTimeSt time;	/* 数据组包时间 */
	u16 serial; 			/* 流水号，从1开始累加循环，最大时65531，循环周期是一天 */
	u8 iccid[M_ICCID_LEN];	/* ICCID信息 */
} __attribute__((packed)) VehLoginDataSt;

/* 信息数据标志类型 */
typedef enum _DataFlagTypeE
{
	E_PROTO_START_FLAG = 0,
	E_PROTO_OBD_INFO_FLAG,			/* OBD信息数据标志 */
	E_PROTO_STREAM_FLAG,			/* 数据流信息标志 */
	E_PROTO_ADD_STREAM_FLAG = 0x80,	/* 附加数据流信息标志 */
} DataFlagTypeE;

/* 车辆登出信息 */
typedef struct _VehLogoutDataSt
{
	CommDateTimeSt time;	/* 登出时间 */
	u16 serial;				/* 登出流水号，与当次登入流水号是一样的 */
} __attribute__((packed)) VehLogoutDataSt;

/**************************************************************************
** 全局变量定义
**************************************************************************/
static DataSerialSt s_protoSerial[E_TCP_CONN_MAX_ID];	/* 协议流水号 */

/* 协议序列号 */
static u16 s_protocolSendSerial = 0;

/* 计算信息的ZA值 */
static const u8 g_protocol_za_val[] =
{
	0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFC, 0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34,
	0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7, 0xF3, 0x97,
	0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92, 0xDD, 0xBC, 0xBD, 0x41,
	0x4D, 0x94, 0x0E, 0x93, 0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19,
	0x81, 0x19, 0x5F, 0x99, 0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94,
	0x8F, 0xE3, 0x0B, 0xBF, 0xF2, 0x66, 0x0B, 0xE1, 0x71, 0x5A,
	0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7, 0xBC, 0x37, 0x36, 0xA2,
	0xF4, 0xF6, 0x77, 0x9C, 0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69,
	0x21, 0x53, 0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40,
	0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0
};

/**************************************************************************
** 函数定义
**************************************************************************/
//依赖接口
/* 输入消息体数据，输出签名数据和长度 */
int GetSignData(u8 *data, int datalen, u8 *signData)
{
	return 0;
}
/**************************************************************************
* 函  数: u8 ProtocolGetVerId(ProtocolTypeE proType)
* 描  述: 获取软件版本ID
* 举  例: 
* 入  参: ProtocolTypeE proType: 不同的地标协议
* 出  参: void
* 返回值: u8: 软件版本号
**************************************************************************/
u8 ProtocolGetVerId(ProtocolTypeE proType)
{
	if (E_PROTO_NJYWK_TANGSHA_J5_TYPE == proType)
	{
		return 3;
	}

	return 0;
}

/**************************************************************************
* 函  数: DataSerialSt ProtocolGetSerial(u8 index)
* 描  述: 获取某一路通道的序列号信息
* 举  例: 
* 入  参: u8 index:数据通道
* 出  参: void
* 返回值: DataSerialSt*: 序列号信息
**************************************************************************/
DataSerialSt* ProtocolGetSerial(u8 index)
{
	return &(s_protoSerial[index]);
}


/**************************************************************************
* 函  数: int ProtocolCalcZaHashVal(u8 *za_hash, u32 maxlen)
* 描  述: 计算得到ZA哈希值
* 举  例: 
* 入  参: u32 maxlen:最大组包长度
* 出  参: u8 *za_hash:计算签名时的哈希值
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolCalcZaHashVal(u8 *za_hash, u32 maxlen)
{
	int ret = M_COMM_FUNC_FAIL;
	u8 *temp = 0;
	int templen = 0;
	SysParamInfoSt *pInfo = SysGetParam();

	do
	{
		temp = get_small_mem();
		if (!temp)
		{
			break;
		}

		//00 80 + ChipID + ZA_val + PubKey
		temp[templen++] = 0x00;
		temp[templen++] = 0x80;
		memcpy(&temp[templen], pInfo->chipId, M_PROTO_CHIP_ID_LEN);
		templen += M_PROTO_CHIP_ID_LEN;
		memcpy(&temp[templen], g_protocol_za_val, sizeof(g_protocol_za_val));
		templen += sizeof(g_protocol_za_val);

		memcpy(&temp[templen], pInfo->key, M_PROTO_KEY_LEN);
		templen += M_PROTO_KEY_LEN;

		DBG_PrintBuf(LEVEL_GPRS, "固定值+公钥\r\n", temp, templen);
		ret = hhsm_read_calc_hash(temp, templen, za_hash, maxlen);
		if (-1 == ret)
		{
			DBG_Print(LEVEL_GPRS, "ProtocolFilingData hhsm_read_calc_hash fail.\r\n");
			break;
		}

		DBG_PrintBuf(LEVEL_GPRS, "ZA(哈希值)\r\n", za_hash, ret);
	} while(0);


	if (temp)
	{
		free_small_mem(temp);
		temp = 0;
	}

	return ret;
}

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
int ProtocolPackSignFormatData(u8 *signData, int len, u8 *outdata)
{
	int ret = M_COMM_FUNC_FAIL;
	int datalen = 0;
	int i = 0;
	int partlen = 0;

	do
	{
		if ((0 == signData) || (M_PROTO_SIGN_DATA_LEN != len) || (0 == outdata))
		{
			break;
		}

		partlen = len / 2;

		//R值信息
		outdata[datalen++] = M_PROTO_SIGN_DATA_LEN;
		for (i = 0; i < partlen; i++)
		{
			datalen += sprintf((char *)&outdata[datalen], "%02x", signData[i]);
		}

		//S值信息
		outdata[datalen++] = M_PROTO_SIGN_DATA_LEN;
		for (i = partlen; i < len; i++)
		{
			datalen += sprintf((char *)&outdata[datalen], "%02x", signData[i]);
		}

		ret = datalen;
	} while(0);

	return ret;
}

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
int ProtocolLoginData(u8 index, ProtocolTypeE type, LoginInfoSt *data, u8 *outdata, u32 maxlen)
{
	LoginInfoSt *login = (LoginInfoSt *)data;
	VehLoginDataSt logindata;
	int datalen = 0;

	if ((0 == data) || (0 == outdata) || (0 == maxlen))
	{
		return M_COMM_FUNC_FAIL;
	}

	memset(&logindata, 0, sizeof(VehLoginDataSt));
	memcpy(&logindata.time, &login->time, sizeof(CommDateTimeSt));

	M_BIG_ENDIAN_U16(s_protoSerial[index].login, logindata.serial);
	s_protoSerial[index].lastLogin = s_protoSerial[index].login;
	/*s_protoSerial[index].login++;
	if (s_protoSerial[index].login > M_PROTO_MAX_SERIAL_VAL)
	{
		s_protoSerial[index].login = M_PROTO_SERIAL_INIT;
	}*/

	memcpy(logindata.iccid, login->iccid, sizeof(login->iccid));
	datalen = (int)sizeof(VehLoginDataSt);

	if (sizeof(VehLoginDataSt) > maxlen)
	{
		M_COMM_ERROR(M_PROTOCOL_MODULE, "pack len[%d] > maxlen[%d]",
			 					sizeof(VehLoginDataSt), maxlen);
		return M_COMM_FUNC_FAIL; 
	}

	memcpy(outdata, (u8 *)&logindata, datalen);

	return datalen;
}

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
int ProtocolLogoutData(u8 index, ProtocolTypeE type, LogoutInfoSt *data, u8 *outdata, u32 maxlen)
{
	LogoutInfoSt *logout = (LogoutInfoSt *)data;
	VehLogoutDataSt logoutdata;
	int datalen = 0;

	if ((0 == data) || (0 == outdata) || (0 == maxlen))
	{
		return M_COMM_FUNC_FAIL;
	}

	memset(&logoutdata, 0, sizeof(VehLogoutDataSt));
	memcpy(&logoutdata.time, &logout->time, sizeof(CommDateTimeSt));

	M_BIG_ENDIAN_U16(s_protoSerial[index].lastLogin, logoutdata.serial);

	if (sizeof(VehLogoutDataSt) > maxlen)
	{
		M_COMM_ERROR(M_PROTOCOL_MODULE, "pack len[%d] > maxlen[%d]",
			 					sizeof(VehLogoutDataSt), maxlen);
		return M_COMM_FUNC_FAIL; 
	}

	datalen = sizeof(VehLogoutDataSt);
	memcpy(outdata, (u8 *)&logoutdata, datalen);

	return datalen;
}

/**************************************************************************
* 函  数: int ProtocolBj5ObdData(u8 index, Bj5ObdDataSt *j5Data, u8 *outdata)
* 描  述: 组包京5的OBD数据信息
* 举  例: 
* 入  参: u8 index:数据通道
*		  Bj5ObdDataSt *j5Data ： 京5OBD数据信息
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
static int ProtocolBj5ObdData(u8 index, Bj5ObdDataSt *j5Data, u8 *outdata)
{
	int ret = M_COMM_FUNC_FAIL;
	int length = 0;
	u16 serial = 0;
	u8 i = 0;
	u32 errCode = 0;

	do
	{
		if ((0 == j5Data) || (0 == outdata))
		{
			break;
		}

		M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
		memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
		length += sizeof(u16);

		s_protoSerial[index].realtime++;
		if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
		{
			s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
		}

		/* OBD消息类型标志 */
		outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

		/* OBD数据 */
		M_BIG_ENDIAN_U16(j5Data->comm.diagSupptStaUn.state, serial);
		j5Data->comm.diagSupptStaUn.state = serial;

		M_BIG_ENDIAN_U16(j5Data->comm.diagReadyStaUn.state, serial);
		j5Data->comm.diagReadyStaUn.state = serial;
		
		memcpy(&outdata[length], (u8 *)&(j5Data->comm), sizeof(ObdCommDataSt));
		length += sizeof(ObdCommDataSt);

		/* calId值 */
		memcpy(&outdata[length], j5Data->calId, M_BJ5_CALID_LEN);
		length += M_BJ5_CALID_LEN;

		/* cvnId值 */
		memcpy(&outdata[length], j5Data->cvnId, M_CVNID_LEN);
		length += M_CVNID_LEN;

		/* iuprval值 */
		outdata[length++] = j5Data->iuprVal;

		/* 故障码个数 */
		outdata[length++] = j5Data->err.errCnt;

		/* 故障码组包 */
		for (i = 0; i < j5Data->err.errCnt; i++)
		{
			M_BIG_ENDIAN_U32(j5Data->err.errCode[i], errCode);
			memcpy(&outdata[length], &errCode, sizeof(u32));
			length += sizeof(u32);
		}

		if (j5Data->err.errCode)
		{
			free_big_mem(j5Data->err.errCode);
			j5Data->err.errCode = 0;
		}

		ret = length;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: int ProtocolCn6ObdData(u8 index, Cn6ObdDataSt *cn6Data, u8 *outdata)
* 描  述: 组包国6的OBD数据信息
* 举  例: 
* 入  参: u8 index:数据通道
*		  Cn6ObdDataSt *cn6Data ： 国6 OBD数据信息
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
static int ProtocolCn6ObdData(u8 index, Cn6ObdDataSt *cn6Data, u8 *outdata, ProtocolTypeE type)
{
	int ret = M_COMM_FUNC_FAIL;
	int length = 0;
	u16 serial = 0;
	u8 i = 0;
	u32 errCode = 0;
	int pos = 0;
	int needLen = 0;
	u8 *signData = 0;
	int signLen = 0;
	DataEncryptTypeE encryType = E_PROTO_ENPT_START;

	do
	{
		if ((0 == cn6Data) || (0 == outdata))
		{
			break;
		}

		if (E_PROTO_YT_BLG_CN6_TYPE != type)
		{
			/* OBD消息类型标志 */
			outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

			M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
			memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
			length += sizeof(u16);
		}
		else
		{
			/* 流水号 */
			M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
			memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
			length += sizeof(u16);

			/* OBD消息类型标志 */
			outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;
		}

		s_protoSerial[index].realtime++;
		if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
		{
			s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
		}
		

		/* OBD数据 */
		pos = length;	/* 记录需要加密OBD消息体的位置 */

		M_BIG_ENDIAN_U16(cn6Data->comm.diagSupptStaUn.state, serial);
		cn6Data->comm.diagSupptStaUn.state = serial;

		M_BIG_ENDIAN_U16(cn6Data->comm.diagReadyStaUn.state, serial);
		cn6Data->comm.diagReadyStaUn.state = serial;

		memcpy(&outdata[length], (u8 *)&(cn6Data->comm), sizeof(ObdCommDataSt));
		length += sizeof(ObdCommDataSt);

		/* calId值 */
		memcpy(&outdata[length], cn6Data->calId, M_CN6_CALID_LEN);
		length += M_CN6_CALID_LEN;

		/* cvnId值 */
		memcpy(&outdata[length], cn6Data->cvnId, M_CVNID_LEN);
		length += M_CVNID_LEN;

		/* iuprval值 */
		memcpy(&outdata[length], cn6Data->iuprVal, M_IUPR_LEN);
		length += M_IUPR_LEN;

		/* 故障码个数 */
		outdata[length++] = cn6Data->err.errCnt;

		/* 故障码组包 */
		for (i = 0; i < cn6Data->err.errCnt; i++)
		{
			M_BIG_ENDIAN_U32(cn6Data->err.errCode[i], errCode);
			memcpy(&outdata[length], &errCode, sizeof(u32));
			length += sizeof(u32);
		}

		if (cn6Data->err.errCode)
		{
			free_big_mem(cn6Data->err.errCode);
			cn6Data->err.errCode = 0;
		}

		/* 签名数据 */
		encryType = SysGetEncryptType();
		if ((E_PROTO_RSA_ENPT == encryType) || (E_PROTO_SM2_ENPT == encryType))
		{
			needLen = length - pos;	/* obd数据消息体的长度 */
			signData = (u8 *)get_small_mem();
			if (!signData)
			{
				break;
			}

			memset(signData, 0, get_small_mem_size());
			signLen = GetSignData(&outdata[pos], needLen, signData);
			if (M_COMM_FUNC_FAIL == signLen)
			{
				free_small_mem(signData);
				signData = 0;
				break;
			}

			/* 组包签名数据 */
			memcpy(&outdata[length], signData, signLen);
			length += signLen;

			free_small_mem(signData);
			signData = 0;
		}

		ret = length;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: int ProtocolJacCn6ObdData(u8 index, Cn6ObdDataSt *cn6Data, CommDateTimeSt *time, u8 *outdata)
* 描  述: 组包江淮国6的OBD数据信息
* 举  例:  流水号 + <信息类型 + 采集时间 + 消息体> - (1 - n)
* 入  参: u8 index:数据通道
*		  Cn6ObdDataSt *cn6Data ： 国6 OBD数据信息
*		  CommDateTimeSt *time : 采集的时间
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
static int ProtocolJacCn6ObdData(u8 index, Cn6ObdDataSt *cn6Data, CommDateTimeSt *time, u8 *outdata)
{
	int ret = M_COMM_FUNC_FAIL;
	int length = 0;
	u16 serial = 0;
	u8 i = 0;
	u32 errCode = 0;

	do
	{
		if ((0 == cn6Data) || (0 == outdata) || (0 == time))
		{
			break;
		}

		M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
		memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
		length += sizeof(u16);

		s_protoSerial[index].realtime++;
		if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
		{
			s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
		}

		/* OBD消息类型标志 */
		outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

		/* OBD消息采集时间 */
		memcpy(&outdata[length], (u8 *)time, sizeof(CommDateTimeSt));
		length += sizeof(CommDateTimeSt);

		M_BIG_ENDIAN_U16(cn6Data->comm.diagSupptStaUn.state, serial);
		cn6Data->comm.diagSupptStaUn.state = serial;

		M_BIG_ENDIAN_U16(cn6Data->comm.diagReadyStaUn.state, serial);
		cn6Data->comm.diagReadyStaUn.state = serial;

		memcpy(&outdata[length], (u8 *)&(cn6Data->comm), sizeof(ObdCommDataSt));
		length += sizeof(ObdCommDataSt);

		/* calId值 */
		memcpy(&outdata[length], cn6Data->calId, M_CN6_CALID_LEN);
		length += M_CN6_CALID_LEN;

		/* cvnId值 */
		memcpy(&outdata[length], cn6Data->cvnId, M_CVNID_LEN);
		length += M_CVNID_LEN;

		/* iuprval值 */
		memcpy(&outdata[length], cn6Data->iuprVal, M_IUPR_LEN);
		length += M_IUPR_LEN;

		/* 故障码个数 */
		outdata[length++] = cn6Data->err.errCnt;

		/* 故障码组包 */
		for (i = 0; i < cn6Data->err.errCnt; i++)
		{
			M_BIG_ENDIAN_U32(cn6Data->err.errCode[i], errCode);
			memcpy(&outdata[length], &errCode, sizeof(u32));
			length += sizeof(u32);
		}

		if (cn6Data->err.errCode)
		{
			free_big_mem(cn6Data->err.errCode);
			cn6Data->err.errCode = 0;
		}

		ret = length;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: int ProtocolShhObdData(u8 index, Cn6ObdDataSt *cn6Data, u8 *outdata)
* 描  述: 组包上海重型柴油机的OBD数据信息
* 举  例: 
* 入  参: u8 index:数据通道
*		  Cn6ObdDataSt *cn6Data ： 国6 OBD数据信息
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
static int ProtocolShhObdData(u8 index, Cn6ObdDataSt *cn6Data, u8 *outdata)
{
	int ret = M_COMM_FUNC_FAIL;
	int length = 0;
	u16 serial = 0;
	u8 i = 0;
	u32 errCode = 0;

	do
	{
		if ((0 == cn6Data) || (0 == outdata))
		{
			break;
		}

		M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
		memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
		length += sizeof(u16);

		s_protoSerial[index].realtime++;
		if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
		{
			s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
		}

		/* OBD消息类型标志 */
		outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

		M_BIG_ENDIAN_U16(cn6Data->comm.diagSupptStaUn.state, serial);
		cn6Data->comm.diagSupptStaUn.state = serial;

		M_BIG_ENDIAN_U16(cn6Data->comm.diagReadyStaUn.state, serial);
		cn6Data->comm.diagReadyStaUn.state = serial;

		memcpy(&outdata[length], (u8 *)&(cn6Data->comm), sizeof(ObdCommDataSt));
		length += sizeof(ObdCommDataSt);

		/* calId值 */
		memcpy(&outdata[length], cn6Data->calId, M_CN6_CALID_LEN);
		length += M_CN6_CALID_LEN;

		/* cvnId值 */
		memcpy(&outdata[length], cn6Data->cvnId, M_CVNID_LEN);
		length += M_CVNID_LEN;

		/* iuprval值 */
		memcpy(&outdata[length], cn6Data->iuprVal, M_IUPR_LEN);
		length += M_IUPR_LEN;

		/* 故障码个数 */
		outdata[length++] = cn6Data->err.errCnt;

		/* 故障码组包 */
		for (i = 0; i < cn6Data->err.errCnt; i++)
		{
			M_BIG_ENDIAN_U32(cn6Data->err.errCode[i], errCode);
			memcpy(&outdata[length], &errCode, sizeof(u32));
			length += sizeof(u32);
		}

		if (cn6Data->err.errCode)
		{
			free_big_mem(cn6Data->err.errCode);
			cn6Data->err.errCode = 0;
		}

		ret = length;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: void ProtocolBigendianStreamData(DataStreamSt *stream)
* 描  述: 对数据流信息进行大端转化
* 举  例: 
* 入  参: DataStreamSt *stream：数据流信息
* 出  参: DataStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianStreamData(DataStreamSt *stream)
{
	u32 dwordVal = 0;
	u16 wordVal = 0;

	/* 双字节的大小端转化 */
	M_BIG_ENDIAN_U16(stream->carSpeed, wordVal);
	stream->carSpeed = wordVal;

	M_BIG_ENDIAN_U16(stream->engineSpeed, wordVal);
	stream->engineSpeed = wordVal;

	M_BIG_ENDIAN_U16(stream->engineFuel, wordVal);
	stream->engineFuel = wordVal;

	M_BIG_ENDIAN_U16(stream->scrUpNoxSensorOut, wordVal);
	stream->scrUpNoxSensorOut = wordVal;

	M_BIG_ENDIAN_U16(stream->scrDownNoxSensorOut, wordVal);
	stream->scrDownNoxSensorOut = wordVal;

	M_BIG_ENDIAN_U16(stream->airIsntake, wordVal);
	stream->airIsntake = wordVal;

	M_BIG_ENDIAN_U16(stream->scrInTmper, wordVal);
	stream->scrInTmper = wordVal;

	M_BIG_ENDIAN_U16(stream->scrOutTmper, wordVal);
	stream->scrOutTmper = wordVal;

	M_BIG_ENDIAN_U16(stream->dpfPressDiff, wordVal);
	stream->dpfPressDiff = wordVal;

	/* 四字节的大小端转化 */
	M_BIG_ENDIAN_U32(stream->lonVal, dwordVal);
	stream->lonVal = dwordVal;

	M_BIG_ENDIAN_U32(stream->latVal, dwordVal);
	stream->latVal = dwordVal;

	M_BIG_ENDIAN_U32(stream->sumMil, dwordVal);
	stream->sumMil = dwordVal;
}

/**************************************************************************
* 函  数: void ProtocolBigendianAddStreamData(AddDataStreamSt *stream)
* 描  述: 对上海重型柴油车进行附加数据流信息进行大端转化
* 举  例: 
* 入  参: AddDataStreamSt *stream：数据流信息
* 出  参: AddDataStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianAddStreamData(AddDataStreamSt *stream)
{
	u32 dwordVal = 0;
	u16 wordVal = 0;

	/* 大小端转化 */
	/* 总油耗 */
	M_BIG_ENDIAN_U32(stream->sumFuel, dwordVal);
	stream->sumFuel = dwordVal;

	/* 实际尿素喷射量 */
	M_BIG_ENDIAN_U32(stream->actualUreaInjet, dwordVal);
	stream->actualUreaInjet = dwordVal;

	/* 总尿素消耗 */
	M_BIG_ENDIAN_U32(stream->sumUreaConsumption, dwordVal);
	stream->sumUreaConsumption = dwordVal;

	/* DPF 排气温度 */
	M_BIG_ENDIAN_U16(stream->dpfExhstTemp, wordVal);
	stream->dpfExhstTemp = wordVal;
}

/**************************************************************************
* 函  数: void ProtocolBigendianAddGdxcHzStreamData(AddDataXcHzStreamSt *stream)
* 描  述: 对广东旭诚杭标进行附加数据流信息进行大端转化
* 举  例: 
* 入  参: AddDataXcHzStreamSt *stream：数据流信息
* 出  参: AddDataXcHzStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
static void ProtocolBigendianAddGdxcHzStreamData(AddDataXcHzStreamSt *stream)
{
	u32 dwordVal = 0;
	u16 wordVal = 0;

	/* 大小端转化 */
	/* 总油耗 */
	M_BIG_ENDIAN_U32(stream->shh.sumFuel, dwordVal);
	stream->shh.sumFuel = dwordVal;

	/* 实际尿素喷射量 */
	M_BIG_ENDIAN_U32(stream->shh.actualUreaInjet, dwordVal);
	stream->shh.actualUreaInjet = dwordVal;

	/* 总尿素消耗 */
	M_BIG_ENDIAN_U32(stream->shh.sumUreaConsumption, dwordVal);
	stream->shh.sumUreaConsumption = dwordVal;

	/* 发动机总运行时间 */
	M_BIG_ENDIAN_U32(stream->sumEngineRuntm, dwordVal);
	stream->sumEngineRuntm = dwordVal;

	/* DPF 排气温度 */
	M_BIG_ENDIAN_U16(stream->shh.dpfExhstTemp, wordVal);
	stream->shh.dpfExhstTemp = wordVal;
}

/**************************************************************************
* 函  数: void ProtocolBigendianAddHzStreamData(AddDataHzStreamSt *stream)
* 描  述: 对杭州地标进行附加数据流信息进行大端转化
* 举  例: 
* 入  参: AddDataHzStreamSt *stream：数据流信息
* 出  参: AddDataHzStreamSt *stream：大端转化后的数据流信息
* 返回值: void
**************************************************************************/
void ProtocolBigendianAddHzStreamData(AddDataHzStreamSt *stream)
{
	u32 dwordVal = 0;
	u16 wordVal = 0;

	/* 大小端转化 */
	/* 环境温度，0xffff - 无效 */
	M_BIG_ENDIAN_U16(stream->environmentTmper, wordVal);
	stream->environmentTmper = wordVal;

	/* 蓄电池电压,数据范围：0~ 3212.75V,0xffff - 无效 */
	M_BIG_ENDIAN_U16(stream->batteryVol, wordVal);
	stream->batteryVol = wordVal;

	/* 总油耗,"0xFF，0xFF，0xFF，0xFF"表示无效 */
	M_BIG_ENDIAN_U32(stream->sumFuel, dwordVal);
	stream->sumFuel = dwordVal;

	/* 参考扭矩,"0xFF，0xFF"表示无效 */
	M_BIG_ENDIAN_U16(stream->refTorque, wordVal);
	stream->refTorque = wordVal;

	/* 发动机总运行时间, "0xFF，0xFF，0xFF，0xFF"表示无效 */
	M_BIG_ENDIAN_U32(stream->sumEngineRuntm, dwordVal);
	stream->sumEngineRuntm = dwordVal;

	/* DPF 排气温度 */
	M_BIG_ENDIAN_U16(stream->dpfExhstTemp, wordVal);
	stream->dpfExhstTemp = wordVal;

	/* 瞬时油耗 */
	M_BIG_ENDIAN_U16(stream->instFuel, wordVal);
	stream->instFuel = wordVal;

	/* 实际尿素喷射量 */
	M_BIG_ENDIAN_U32(stream->actualUreaInjet, dwordVal);
	stream->actualUreaInjet = dwordVal;

	/* 后处理上游排气温度 */
	M_BIG_ENDIAN_U16(stream->upExhstTemper, wordVal);
	stream->upExhstTemper = wordVal;

	/* 后处理下游排气温度 */
	M_BIG_ENDIAN_U16(stream->downExhstTemper, wordVal);
	stream->downExhstTemper = wordVal;

	/* 总尿素消耗 */
	M_BIG_ENDIAN_U32(stream->sumUreaConsumption, dwordVal);
	stream->sumUreaConsumption = dwordVal;
}

/**************************************************************************
* 函  数: int ProtocolRealtimeStreams(u8 index, ProtocolTypeE type, StreamRealtimeInfoSt *stream, CommDateTimeSt *time, u8 *outdata)
* 描  述: 组包多组实时数据流信息
* 举  例: 
* 入  参: u8 index:数据通道
*		  ProtocolTypeE type:协议类型
*		  StreamRealtimeInfoSt *stream: 多组实时数据路信息
*		  CommDateTimeSt *time: 首包采集的时间
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
static int ProtocolRealtimeStreams(u8 index, ProtocolTypeE type, StreamRealtimeInfoSt *stream, CommDateTimeSt *time, u8 *outdata)
{
	int ret = M_COMM_FUNC_FAIL;
	int length = 0;
	u16 serial = 0;
	u8 i = 0;
	DataEncryptTypeE encryType = E_PROTO_ENPT_START;

	do
	{
		if ((0 == stream) || (0 == outdata))
		{
			break;
		}

		if ((E_PROTO_BJ5_TYPE == type) || (E_PROTO_CN56_TYPE == type)
			|| (E_PROTO_SHH_TYPE == type) || (E_PROTO_GDXC_HZ_TYPE == type)) /* 上海重型柴油车和京5, 时间 + 流水号 + 每组信息 */
		{
			/* 流水号 */
			M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
			memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
			length += sizeof(u16);

			s_protoSerial[index].realtime++;
			if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
			{
				s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
			}

			if (E_PROTO_GDXC_HZ_TYPE == type)
			{
				Cn6ObdDataSt obd;
				u32 errCode = 0;

				/* 一组OBD数据信息 */
				memset(&obd, 0, sizeof(Cn6ObdDataSt));
				GprsGetCn6ObdInfo(&obd);

				/* OBD数据息类型标志 */
				outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

				memcpy(&outdata[length], (u8 *)&(obd.comm), sizeof(ObdCommDataSt));
				length += sizeof(ObdCommDataSt);

				memcpy(&outdata[length], (u8 *)(obd.calId), M_CN6_CALID_LEN);
				length += M_CN6_CALID_LEN;

				memcpy(&outdata[length], (u8 *)(obd.cvnId), M_CVNID_LEN);
				length += M_CVNID_LEN;

				memcpy(&outdata[length], (u8 *)(obd.iuprVal), M_IUPR_LEN);
				length += M_IUPR_LEN;

				/* 故障码个数 */
				outdata[length++] = obd.err.errCnt;
		
				/* 故障码组包 */
				for (i = 0; i < obd.err.errCnt; i++)
				{
					M_BIG_ENDIAN_U32(obd.err.errCode[i], errCode);
					memcpy(&outdata[length], &errCode, sizeof(u32));
					length += sizeof(u32);
				}

				if (obd.err.errCode)
				{
					free_big_mem(obd.err.errCode);
					obd.err.errCode = 0;
				}
			}

			/* 数据流信息 */
			for (i = 0; i < stream->count; i++)
			{
				/* 数据流消息类型标志 */
				outdata[length++] = (u8)E_PROTO_STREAM_FLAG;			

				/* 数据流信息 */
				ProtocolBigendianStreamData(&(stream->data[i]));
				memcpy(&outdata[length], (u8 *)&(stream->data[i]), sizeof(DataStreamSt));
				length += sizeof(DataStreamSt);

				if (SysGetCarFuelGasFlag()) //如果是燃气车
				{
					M_BIG_ENDIAN_U16(ObdGetCatalyticConverterPreThan(), serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);
	
					M_BIG_ENDIAN_U16(ObdGetCatalyticConverterAfterThan(), serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);
				}

				/* 需要添加附加数据信息 */
				if (E_PROTO_SHH_TYPE == type)
				{
					/* 数据流消息类型标志 */
					outdata[length++] = (u8)E_PROTO_ADD_STREAM_FLAG;			

					/* 数据流信息 */
					ProtocolBigendianAddStreamData(&(stream->addData[i]));
					memcpy(&outdata[length], (u8 *)&(stream->addData[i]), sizeof(AddDataStreamSt));
					length += sizeof(AddDataStreamSt);
				}
				else if (E_PROTO_GDXC_HZ_TYPE == type)
				{
					/* 数据流消息类型标志 */
					outdata[length++] = (u8)E_PROTO_ADD_STREAM_FLAG;

					/* 数据流信息 */
					ProtocolBigendianAddGdxcHzStreamData(&(stream->addXcHzData[i]));
					memcpy(&outdata[length], (u8 *)&(stream->addXcHzData[i]), sizeof(AddDataXcHzStreamSt));
					length += sizeof(AddDataXcHzStreamSt);
				}
			}
		}
		else if ((E_PROTO_HZ_TYPE == type) || (E_PROTO_GB17691_TYPE == type))/* 杭州地标和GB17691协议 */
		{
			/* 数据流信息 */
			for (i = 0; i < stream->count; i++)
			{
				/* 数据流消息类型标志 */
				outdata[length++] = (u8)E_PROTO_STREAM_FLAG;

				/* 序列号加在第一组数据流的类型标志之后 */
				if (0 == i)
				{
					/* 流水号 */
					M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);

					s_protoSerial[index].realtime++;
					if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
					{
						s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
					}
				}

				/* 数据流信息 */
				ProtocolBigendianStreamData(&(stream->data[i]));
				memcpy(&outdata[length], (u8 *)&(stream->data[i]), sizeof(DataStreamSt));
				length += sizeof(DataStreamSt);

				/* 杭标有特殊要求 */
				if (E_PROTO_HZ_TYPE == type)
				{
					/* 需要添加附加数据信息 */
					outdata[length++] = (u8)E_PROTO_ADD_STREAM_FLAG;	

					/* 长度 */
					M_BIG_ENDIAN_U16(sizeof(AddDataHzStreamSt), serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);

					/* 数据流信息 */
					ProtocolBigendianAddHzStreamData(&(stream->addHzData[i]));
					memcpy(&outdata[length], (u8 *)&(stream->addHzData[i]), sizeof(AddDataHzStreamSt));
					length += sizeof(AddDataHzStreamSt);
				}
			}
		}
		else if ((E_PROTO_HJ_CN6_TYPE == type) || (E_PROTO_NJYWK_TANGSHA_J5_TYPE == type)) /* 江淮新国6 */
		{
			M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
			memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
			length += sizeof(u16);

			s_protoSerial[index].realtime++;
			if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
			{
				s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
			}

			// 唐标数据 OBD + 数据流信息
			if (E_PROTO_NJYWK_TANGSHA_J5_TYPE == type)
			{
				/* 数据流消息类型标志 */
				Cn6ObdDataSt obd;
				u32 errCode = 0;

				/* 一组OBD数据信息 */
				memset(&obd, 0, sizeof(Cn6ObdDataSt));
				GprsGetCn6ObdInfo(&obd);

				/* OBD数据息类型标志 */
				outdata[length++] = (u8)E_PROTO_OBD_INFO_FLAG;

				/* 采集时间 */
				memcpy(&outdata[length], (u8 *)time, sizeof(CommDateTimeSt));
				length += sizeof(CommDateTimeSt);

				memcpy(&outdata[length], (u8 *)&(obd.comm), sizeof(ObdCommDataSt));
				length += sizeof(ObdCommDataSt);

				memcpy(&outdata[length], (u8 *)(obd.calId), M_CN6_CALID_LEN);
				length += M_CN6_CALID_LEN;

				memcpy(&outdata[length], (u8 *)(obd.cvnId), M_CVNID_LEN);
				length += M_CVNID_LEN;

				memcpy(&outdata[length], (u8 *)(obd.iuprVal), M_IUPR_LEN);
				length += M_IUPR_LEN;

				/* 故障码个数 */
				outdata[length++] = obd.err.errCnt;
		
				/* 故障码组包 */
				for (i = 0; i < obd.err.errCnt; i++)
				{
					M_BIG_ENDIAN_U32(obd.err.errCode[i], errCode);
					memcpy(&outdata[length], &errCode, sizeof(u32));
					length += sizeof(u32);
				}

				if (obd.err.errCode)
				{
					free_big_mem(obd.err.errCode);
					obd.err.errCode = 0;
				}
			}
		

			for (i = 0; i < stream->count; i++)
			{
				/* 数据流消息类型标志 */
				outdata[length++] = (u8)E_PROTO_STREAM_FLAG;

				/* 采集时间 */
				{
					CommDateTimeSt tm;
					u32 firstPackTimeSec = 0;

					firstPackTimeSec = CommSystimeToSec(time);
					firstPackTimeSec += i;

					memset(&tm, 0, sizeof(tm));
					CommSecToSystime(firstPackTimeSec, &tm);
					memcpy(&outdata[length], (u8 *)&tm, sizeof(CommDateTimeSt));
					length += sizeof(CommDateTimeSt);
				}

				/* 数据流信息 */
				
				ProtocolBigendianStreamData(&(stream->data[i]));
				memcpy(&outdata[length], (u8 *)&(stream->data[i]), sizeof(DataStreamSt));
				length += sizeof(DataStreamSt);					
			}
		}
		else if (E_PROTO_JAC_CN6_TYPE == type) /* 江淮国6 */
		{
			int pos = 0;
			int needLen = 0;
			u8 *signData = 0;
			int signLen = 0;

			for (i = 0; i < stream->count; i++)
			{
				/* 数据流消息类型标志 */
				outdata[length++] = (u8)E_PROTO_STREAM_FLAG;

				M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
				memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
				length += sizeof(u16);

				/* 数据流信息 */
				pos = length;	/* 记录需要加密数据流消息体的位置,10组,每组37bytes,最大370个字节 */
				ProtocolBigendianStreamData(&(stream->data[i]));
				memcpy(&outdata[length], (u8 *)&(stream->data[i]), sizeof(DataStreamSt));
				length += sizeof(DataStreamSt);

				/* 签名数据 */
				encryType = SysGetEncryptType();
				if ((E_PROTO_RSA_ENPT == encryType) || (E_PROTO_SM2_ENPT == encryType))
				{
					needLen = length - pos; /* obd数据消息体的长度 */
					signData = (u8 *)get_small_mem();
					if (!signData)
					{
						break;
					}

					memset(signData, 0, get_small_mem_size());
					signLen = GetSignData(&outdata[pos], needLen, signData);
					if (M_COMM_FUNC_FAIL == signLen)
					{
						free_small_mem(signData);
						signData = 0;
						break;
					}

					/* 组包签名数据 */
					memcpy(&outdata[length], signData, signLen);
					length += signLen;

					free_small_mem(signData);
					signData = 0;
				}
			}

			s_protoSerial[index].realtime++;
			if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
			{
				s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
			}
		}
		else if ((E_PROTO_CN6_TYPE == type) || (E_PROTO_YT_BLG_CN6_TYPE == type))	/* 国6 */
		{
			int pos = 0;
			int needLen = 0;
			u8 *signData = 0;
			int signLen = 0;

			for (i = 0; i < stream->count; i++)
			{
				if (E_PROTO_YT_BLG_CN6_TYPE != type)
				{
					/* 数据流消息类型标志 */
					outdata[length++] = (u8)E_PROTO_STREAM_FLAG;

					M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);
				}
				else
				{
					M_BIG_ENDIAN_U16(s_protoSerial[index].realtime, serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);

					/* 数据流消息类型标志 */
					outdata[length++] = (u8)E_PROTO_STREAM_FLAG;
				}

				s_protoSerial[index].realtime++;
				if (s_protoSerial[index].realtime > M_PROTO_MAX_SERIAL_VAL)
				{
					s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;//如果超过一天了，就得重新开始置 1
				}

				/* 数据流信息 */
				pos = length;	/* 记录需要加密数据流消息体的位置,10组,每组37bytes,最大370个字节 */
				ProtocolBigendianStreamData(&(stream->data[i]));
				memcpy(&outdata[length], (u8 *)&(stream->data[i]), sizeof(DataStreamSt));
				length += sizeof(DataStreamSt);

				if (SysGetCarFuelGasFlag())
				{
					M_BIG_ENDIAN_U16(ObdGetCatalyticConverterPreThan(), serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);

					M_BIG_ENDIAN_U16(ObdGetCatalyticConverterAfterThan(), serial);
					memcpy(&outdata[length], (u8 *)&serial, sizeof(u16));
					length += sizeof(u16);
				}

				/* 签名数据 */
				encryType = SysGetEncryptType();
				if ((E_PROTO_RSA_ENPT == encryType) || (E_PROTO_SM2_ENPT == encryType))
				{
					needLen = length - pos; /* obd数据消息体的长度 */
					signData = (u8 *)get_small_mem();
					if (!signData)
					{
						break;
					}

					memset(signData, 0, get_small_mem_size());
					signLen = GetSignData(&outdata[pos], needLen, signData);
					if (M_COMM_FUNC_FAIL == signLen)
					{
						free_small_mem(signData);
						signData = 0;
						break;
					}

					/* 组包签名数据 */
					memcpy(&outdata[length], signData, signLen);
					length += signLen;

					free_small_mem(signData);
					signData = 0;
				}
			}
		}

		ret = length;
	} while(0);

	return ret;
}

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
int ProtocolRealtimeData(u8 index, ProtocolTypeE type, RealtimeInfoSt *data, u8 *outdata, u32 maxlen)
{
	int ret = M_COMM_FUNC_FAIL;
	u32 length = 0;
	u8 *temp = 0;
	RealtimeInfoSt *info = (RealtimeInfoSt *)data;

	do
	{
		if ((0 == data) || (0 == outdata) || (0 == maxlen))
		{
			break;
		}

		memcpy(outdata, (u8 *)&info->time, sizeof(CommDateTimeSt));
		length += sizeof(CommDateTimeSt);

		temp = (u8 *)get_big_mem();
		if (!temp)
		{
			break;
		}

		memset(temp, 0, get_big_mem_size());
		if (E_REALTIME_OBD_DIAG == info->type)			/* OBD实时数据 */
		{
			if (E_PROTO_BJ5_TYPE == type)		/* 京5 */
			{
				ret = ProtocolBj5ObdData(index, &(info->realtimeUn.obd.obdUn.bj5), temp);
			}
			else if ((E_PROTO_CN6_TYPE == type) || (E_PROTO_HZ_TYPE  == type) || (E_PROTO_YT_BLG_CN6_TYPE == type)
				|| (E_PROTO_GB17691_TYPE == type) || (E_PROTO_JAC_CN6_TYPE == type) || (E_PROTO_BACK_DOOR_TYPE == type))	/* 国6和杭州地标 */
			{
				ret = ProtocolCn6ObdData(index, &(info->realtimeUn.obd.obdUn.cn6), temp, type);
			}
			else if ((E_PROTO_SHH_TYPE == type) || (E_PROTO_CN56_TYPE == type))	/* 上海重型协议和东风平台 */
			{
				ret = ProtocolShhObdData(index, &(info->realtimeUn.obd.obdUn.cn6), temp);
			}
			else if ((E_PROTO_HJ_CN6_TYPE == type) || (E_PROTO_NJYWK_TANGSHA_J5_TYPE == type)) //江淮新的协议
			{
				ret = ProtocolJacCn6ObdData(index, &(info->realtimeUn.obd.obdUn.cn6), &(info->time), temp);
			}

			if (M_COMM_FUNC_FAIL == ret)
			{
				M_COMM_ERROR(M_PROTOCOL_MODULE, "Protocol [%d] ObdData fail", type);
				break;
			}

			memcpy(&outdata[length], temp, ret);
			length += ret;
		}
		else if (E_REALTIME_STREAM_DATA == info->type)	/* 还是数据流实时数据 */
		{
			ret = ProtocolRealtimeStreams(index, type, &(info->realtimeUn.stream), &(info->time), temp);
			if (M_COMM_FUNC_FAIL == ret)
			{
				M_COMM_ERROR(M_PROTOCOL_MODULE, "ProtocolRealtimeStreams fail");
				break;
			}

			memcpy(&outdata[length], temp, ret);
			length += ret;
		}
	}while (0);

	if (temp)
	{
		free_big_mem(temp);
		temp = 0;
	}

	if (M_COMM_FUNC_FAIL == ret)
	{
		return M_COMM_FUNC_FAIL;
	}

	if (length > maxlen)
	{
		M_COMM_ERROR(M_PROTOCOL_MODULE, "pack len[%d] > maxlen[%d]", length, maxlen);
		return M_COMM_FUNC_FAIL; 
	}

	return length;
}

/**************************************************************************
* 函  数: int ProtocolFilingData(FilingInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包自动备案数据
* 举  例: 
* 入  参: FilingInfoSt *data:不同的命令单元，传入自动备案数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolFilingData(FilingInfoSt *data, u8 *outdata, u32 maxlen)
{
	FilingInfoSt *filing = (FilingInfoSt *)data;
	u8 *sign = 0;
	int datalen = 0;
	int ret = M_COMM_FUNC_FAIL;
	u8 *temp = 0;
	int templen = 0;

	do
	{
		if ((0 == data) || (0 == outdata) || (0 == maxlen))
		{
			break;
		}

		sign = (u8 *)get_micro_mem();
		if (!sign)
		{
			M_COMM_ERROR(M_PROTOCOL_MODULE, "get_micro_mem fail");
			break;
		}
		memset(sign, 0, get_micro_mem_size());
		ret = ProtocolCalcZaHashVal(sign, get_micro_mem_size());
		if (M_COMM_FUNC_FAIL == ret)
		{
			DBG_Print(LEVEL_GPRS, "ProtocolFilingData ProtocolCalcZaHashVal fail.\r\n");
			break;
		}
		
		temp = get_small_mem();
		if (!temp)
		{
			M_COMM_ERROR(M_PROTOCOL_MODULE, "get_small_mem temp fail");
			break;
		}

		memset(temp, 0, get_small_mem_size());
		memcpy(temp, sign, ret);
		templen += ret;

		/* 数据采集时间 */
		memcpy(&outdata[datalen], &(filing->time), sizeof(CommDateTimeSt));
		datalen += sizeof(CommDateTimeSt);

		/* 明文数据信息,芯片ID,公钥,VIN */
		memcpy(&outdata[datalen], filing->chipId, M_PROTO_CHIP_ID_LEN);
		datalen += M_PROTO_CHIP_ID_LEN;

		memcpy(&outdata[datalen], filing->key, M_PROTO_KEY_LEN);
		datalen += M_PROTO_KEY_LEN;

		memcpy(&outdata[datalen], filing->vin, M_VIN_LEN);
		datalen += M_VIN_LEN;

		//组包成新的信息
		memcpy(&temp[templen], &(filing->time), sizeof(CommDateTimeSt));
		templen += sizeof(CommDateTimeSt);

		memcpy(&temp[templen], filing->chipId, M_PROTO_CHIP_ID_LEN);
		templen += M_PROTO_CHIP_ID_LEN;

		memcpy(&temp[templen], filing->key, M_PROTO_KEY_LEN);
		templen += M_PROTO_KEY_LEN;

		memcpy(&temp[templen], filing->vin, M_VIN_LEN);
		templen += M_VIN_LEN;

		DBG_PrintBuf(LEVEL_GPRS, "ZA(哈希值) + 消息信息\r\n", temp, templen);

		//签名数据
		memset(sign, 0, get_micro_mem_size());
		ret = hhsm_read_sign_data(temp, templen, sign, get_micro_mem_size());
		if (-1 == ret)
		{
			//M_COMM_ERROR(M_PROTOCOL_MODULE, "hhsm_read_sign_data fail");
			ret = M_COMM_FUNC_FAIL;
			break;
		}
		DBG_PrintBuf(LEVEL_GPRS, "签名数据\r\n", sign, ret);
		
		outdata[datalen++] = 0x20;
		memcpy(&outdata[datalen], &sign[0], 0x20);
		datalen += 0x20;

		outdata[datalen++] = 0x20;
		memcpy(&outdata[datalen], &sign[0x20], 0x20);
		datalen += 0x20;

		if (datalen > maxlen)
		{
			//M_COMM_ERROR(M_PROTOCOL_MODULE, "datalen[%d] > maxlen[%d]", datalen, maxlen);
			ret = M_COMM_FUNC_FAIL;
			break;
		}

		ret = datalen;
	}while(0);

	if (sign)
	{
		free_micro_mem(sign);
		sign = 0;
	}

	if (temp)
	{
		free_small_mem(temp);
		temp = 0;
	}

	return ret;
}


/**************************************************************************
* 函  数: int ProtocolDisasAlarmData(u8 index, DisasAlarmInfoSt *data, u8 *outdata, u32 maxlen)
* 描  述: 平台数据协议组包防拆报警
* 举  例: 
* 入  参: u8 index:对应某一路
*		  DisasAlarmInfoSt *data:不同的命令单元，传入自动备案数据
*		  u32 maxlen:最大组包长度
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolDisasAlarmData(u8 index, DisasAlarmInfoSt *data, u8 *outdata, u32 maxlen)
{
	DisasAlarmInfoSt *alarm = (DisasAlarmInfoSt *)data;
	int datalen = 0;
	u32 dword = 0;
	u16 word = 0;

	if ((0 == data) || (0 == outdata) || (0 == maxlen))
	{
		return M_COMM_FUNC_FAIL;
	}

	//数据采集时间
	memcpy(outdata, (u8 *)&(alarm->time), sizeof(CommDateTimeSt));
	datalen += sizeof(CommDateTimeSt);

	//防拆报警流水号
	M_BIG_ENDIAN_U16(s_protoSerial[index].disasalarm, word);
	memcpy(&outdata[datalen], &word, sizeof(u16));
	datalen += sizeof(u16);

	s_protoSerial[index].disasalarm++;
	if (s_protoSerial[index].disasalarm > M_PROTO_MAX_SERIAL_VAL)
	{
		s_protoSerial[index].disasalarm = M_PROTO_SERIAL_INIT;
	}

	//定位状态
	outdata[datalen++] = alarm->locStaUn.state;

	//经纬度信息
	M_BIG_ENDIAN_U32(data->lat, dword);
	memcpy(&outdata[datalen], &dword, sizeof(u32));
	datalen += sizeof(u32);

	M_BIG_ENDIAN_U32(data->lon, dword);
	memcpy(&outdata[datalen], &dword, sizeof(u32));
	datalen += sizeof(u32);

	if (datalen > maxlen)
	{
		M_COMM_ERROR(M_PROTOCOL_MODULE, "pack len[%d] > maxlen[%d]", datalen, maxlen);
		return M_COMM_FUNC_FAIL; 
	}

	return datalen;
}

/**************************************************************************
* 函  数: int ProtocolHeaderData(DataCmdTypeE cmd, ProtocolTypeE proType, u8 *outdata)
* 描  述: 平台数据协议头组包
* 举  例: 
* 入  参: DataCmdTypeE cmd:命令类型
*		  ProtocolTypeE proType：协议类型
* 出  参: u8 *outdata: 传出组包后的协议数据
* 返回值: int > 0 ,组包数据协议后的数据长度; M_COMM_FUNC_FAIL - 失败
**************************************************************************/
int ProtocolHeaderData(DataCmdTypeE cmd, ProtocolTypeE proType, u8 *outdata)
{
	int datalen = 0;
	u8 vin[M_VIN_LEN] = {0};
	ObdDataInfoRt *pObd = ObdGetStructureData();

	if ((0 == outdata) || (E_PROTO_CMD_START == cmd))
	{
		return M_COMM_FUNC_FAIL;
	}

	outdata[datalen++] = M_PROTO_HEAD_FLAG;
	outdata[datalen++] = M_PROTO_HEAD_FLAG;

	/* 命令类型 */
	outdata[datalen++] = (u8)cmd;

	//应答标志
	/*if (E_PROTO_HJ_CN6_TYPE == proType) 
	{
		outdata[datalen++] = 0xFE;
	}*/

	SysGetVin(vin);
	if (0 == strlen((char *)vin))
	{
		memcpy(&outdata[datalen], (u8 *)pObd->VIN, M_VIN_LEN);
	}
	else
	{
		memcpy(&outdata[datalen], vin, M_VIN_LEN);
	}
	
	datalen += M_VIN_LEN;

	//if (E_PROTO_HJ_CN6_TYPE != proType)
	{
		outdata[datalen++] = ProtocolGetVerId(proType);
	}

	if ((E_PROTO_BJ5_TYPE == proType) || (E_PROTO_SHH_TYPE == proType)
		|| (E_PROTO_CN56_TYPE == proType) || (E_PROTO_GB17691_TYPE == proType)
		|| (E_PROTO_HZ_TYPE == proType) || (E_PROTO_JAC_CN6_TYPE == proType)
		|| (E_PROTO_HJ_CN6_TYPE == proType) || (E_PROTO_NJYWK_TANGSHA_J5_TYPE == proType))
	{
		outdata[datalen++] = 0x01;	/* 非加密 */
	}
	else if ((E_PROTO_CN6_TYPE == proType)
			|| (E_PROTO_YT_BLG_CN6_TYPE == proType)
			|| (E_PROTO_GDXC_HZ_TYPE == proType))
	{
		outdata[datalen++] = (u8)SysGetEncryptType();
	}
	else
	{
		outdata[datalen++] = 0x01;	/* 默认非加密 */
	}

	return datalen;
}

/**************************************************************************
* 函  数: int ProtocolParseData(u8 index, u8 *data, int datalen, void *outdata)
* 描  述: 平台数据协议，解析数据，输出需要的数据
* 举  例: 
* 入  参: u8 index: 第几路链路
*		  u8 *data：接收到的数据流信息
*		  int datalen:接收到的数据流信息长度
* 出  参: void *outdata:输出数据信息
* 返回值: int: M_COMM_FUNC_FAIL - 应答失败，M_COMM_FUNC_SUCC - 应答成功
**************************************************************************/
int ProtocolParseData(u8 index, u8 *data, int datalen, void *outdata)
{
	return 0;
}

/**************************************************************************
* 函  数: void ProtocolInit(void)
* 描  述: 平台数据协议，需要用到的数据初始化
* 举  例: 
* 入  参: 
* 出  参: void
* 返回值: void
**************************************************************************/
void ProtocolInit(void)
{
	u8 index = 0;

	/* 初始化默认参数 */
	for (index = 0; index < E_TCP_CONN_MAX_ID; index++)
	{
		s_protoSerial[index].lastLogin = M_PROTO_SERIAL_INIT;
		s_protoSerial[index].login = M_PROTO_SERIAL_INIT;
		s_protoSerial[index].realtime = M_PROTO_SERIAL_INIT;
		s_protoSerial[index].reissue = M_PROTO_SERIAL_INIT;
		s_protoSerial[index].disasalarm = M_PROTO_SERIAL_INIT;
	}
}


