#include "J5.h"


typedef struct tagJ5_DataInfo
{
	INT8U *pVIN;
	INT8U Ver;
	INT8U Way;
}J5_DataInfo;


static J5_DataInfo sJ5DataInfo = {0};


typedef struct tagJ5_SaveData
{
	INT16U usLoginFlow;			//登录流水 以天为单位
	INT16U usRealInfoFlow;		//实时信息流水 以天为单位
	INT16U usSupplyInfoFlow;	//补发信息流水 以天为单位
}J5_SaveData;
static J5_SaveData stJ5SaveData = {0};




//封包
int J5_DataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,void *pDataUnit,INT16U usDataUnitLen)
{
	INT16U len = 0;
	INT8U *pbuf = (INT8U *)pOutBuf;

	if (pbuf == 0)
	{
		return -1;
	}
	//判断缓冲区大小
	//起始符(2B)+命令字(1B)+VIN(17B)+软件版本号(1B)+加密方式(1B)+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
	if (usOutBufSize < (2+1+J5_LEN_VIN+1+1+2+usDataUnitLen+1))
	{
		return -2;
	}
	//起始字符串 2B
	pbuf[len++] = J5_HEAD0;
	pbuf[len++] = J5_HEAD1;

	//命令单元 1B
	pbuf[len++] = cmd;

	//车辆识别号
	memcpy(pbuf + len,sJ5DataInfo.pVIN,J5_LEN_VIN);
	len += J5_LEN_VIN;

	//终端版本号 1B
	pbuf[len++] = (sJ5DataInfo.Ver);

	//数据加密方式	1B
	//pbuf[len++] = (sJ5DataInfo.Way);
	pbuf[len++] = 0x01;

	//数据单元长度	2B
	pbuf[len++] = usDataUnitLen >> 8;
	pbuf[len++] = usDataUnitLen;

	//数据单元
	if (pDataUnit)
	{
		memcpy(pbuf + len,pDataUnit,usDataUnitLen);
		len += usDataUnitLen;
	}

	//异或校验
	pbuf[len++] = XOR_Check(pbuf,len);

	return len;
}


void J5_Init(void *szVIN,INT8U Ver,INT8U Way)
{
	sJ5DataInfo.pVIN = szVIN;
	sJ5DataInfo.Ver = Ver;
	sJ5DataInfo.Way = Way;
}


//登录包
int J5_LoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID)
{
	INT8U temp[32] = {0};
	INT8U len = 0;
	char *p = (char *)szICCID;

	if ((BCDTime == 0) || (szICCID == 0))
	{
		return -1;
	}

	//时间
	memcpy(temp + len,BCDTime,6);
	len += 6;

	//流水号
	stJ5SaveData.usLoginFlow++;
	temp[len++] = stJ5SaveData.usLoginFlow >> 8;
	temp[len++] = stJ5SaveData.usLoginFlow;

	//ICCID
	if ((strlen(p) + 8) <= sizeof(temp))
	{
		strcpy((char *)(temp + len),p);
		len += strlen(p);
	}
	else
	{
		memcpy(temp + len,p,sizeof(temp) - 8);
		len += (sizeof(temp) - 8);
	}

	return J5_DataBag(pOutBuf,usOutBufSize,J5_CMD_LOGIN,temp,len);
}

//登出包
int J5_LogoutDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6])
{
	INT8U temp[32] = {0};
	INT8U len = 0;

	if (BCDTime == 0)
	{
		return 0;
	}

	//时间
	memcpy(temp + len,BCDTime,6);
	len += 6;

	//流水号
	temp[len++] = stJ5SaveData.usLoginFlow >> 8;
	temp[len++] = stJ5SaveData.usLoginFlow;

	return J5_DataBag(pOutBuf,usOutBufSize,J5_CMD_LOGOUT,temp,len);
}


//终端校时包
int J5_CalibrationTimePacket(void *pOutBuf,INT16U usOutBufSize)
{
	return J5_DataBag(pOutBuf,usOutBufSize,J5_CMD_CALIBRATIONTIME,0,0);
}


int J5_OBDInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,J5_OBDInfo *pOBDInfo)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + J5_OFFSET_DATA_UNIT;
	INT8U i = 0;
	INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pOBDInfo == 0)) return -1;

	usDataUnitLen = 6+1+2+6+J5_LEN_VIN+J5_LEN_SOFTVER+J5_LEN_CVN+J5_LEN_IUPR+1+4 * pOBDInfo->ucDTCTotalNum;	//数据单元长度

	if ((usOutBufSize - J5_OFFSET_DATA_UNIT) <= usDataUnitLen)
	{
		return -2;	//the buf have no enough size
	}
	//数据采集时间
	memcpy(ptr,pOBDInfo->BCDTime,6);
	len += 6;

	//信息流水号
	stJ5SaveData.usRealInfoFlow++;
	ptr[len++] = stJ5SaveData.usRealInfoFlow >> 8;
	ptr[len++] = stJ5SaveData.usRealInfoFlow;

	//信息类型标志
	ptr[len++] = J5_OBD_INFO;


	//OBD信息体
	ptr[len++] = pOBDInfo->ucOBDProtocol;
	ptr[len++] = pOBDInfo->ucMilStatus;
	ptr[len++] = pOBDInfo->usSupportStatus >> 8;
	ptr[len++] = pOBDInfo->usSupportStatus;
	ptr[len++] = pOBDInfo->usReadyStatus >> 8;
	ptr[len++] = pOBDInfo->usReadyStatus;

	memcpy(ptr + len, pOBDInfo->szVIN, J5_LEN_VIN);
	len += J5_LEN_VIN;

	memcpy(ptr + len, pOBDInfo->szCalid, J5_LEN_SOFTVER);
	len += J5_LEN_SOFTVER;

	memcpy(ptr + len, pOBDInfo->szCVN, J5_LEN_CVN);
	len += J5_LEN_CVN;
	
	memcpy(ptr + len, pOBDInfo->szIUPR, J5_LEN_IUPR);
	len += J5_LEN_IUPR;

	ptr[len++] = pOBDInfo->ucDTCTotalNum;

	for (i = 0; i < pOBDInfo->ucDTCTotalNum; i++)
	{
		ptr[len++] = *(pOBDInfo->pDTCList + 0);
		ptr[len++] = *(pOBDInfo->pDTCList + 1);
		ptr[len++] = *(pOBDInfo->pDTCList + 2);
		ptr[len++] = *(pOBDInfo->pDTCList + 3);
	}
	return J5_DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}

int J5_DataStreamInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								J5_DataStreamInfo *pDataStreamInfo1,unsigned int bagcnt)
{
	J5_DataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + J5_OFFSET_DATA_UNIT;
	unsigned int i = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return -1;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stJ5SaveData.usRealInfoFlow++;
	ptr[len++] = stJ5SaveData.usRealInfoFlow >> 8;
	ptr[len++] = stJ5SaveData.usRealInfoFlow;

	for (i = 0; i < bagcnt; i++)
	{
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = J5_DATA_STREAM_INFO;
		
		//数据流信息体
		ptr[len++] = pDataStreamInfo->usSpeed >> 8;
		ptr[len++] = pDataStreamInfo->usSpeed;
		
		ptr[len++] = pDataStreamInfo->uckPa;
		
		ptr[len++] = pDataStreamInfo->cActualTorquePercent;
		ptr[len++] = pDataStreamInfo->cFrictionTorquePercent;
		ptr[len++] = pDataStreamInfo->usRPM >> 8;
		ptr[len++] = pDataStreamInfo->usRPM;
		
		ptr[len++] = pDataStreamInfo->usFuelFlow >> 8;
		ptr[len++] = pDataStreamInfo->usFuelFlow;
		
		ptr[len++] = pDataStreamInfo->sUpperNOx >> 8;
		ptr[len++] = pDataStreamInfo->sUpperNOx;
		
		ptr[len++] = pDataStreamInfo->sLowerNOx >> 8;
		ptr[len++] = pDataStreamInfo->sLowerNOx;
		ptr[len++] = pDataStreamInfo->ucRemain;
		
		ptr[len++] = pDataStreamInfo->usIntakeVolume >> 8;
		ptr[len++] = pDataStreamInfo->usIntakeVolume;
		
		ptr[len++] = pDataStreamInfo->sSCRInletTemperature >> 8;
		ptr[len++] = pDataStreamInfo->sSCRInletTemperature;
		
		ptr[len++] = pDataStreamInfo->sSCROutletTemperature >> 8;
		ptr[len++] = pDataStreamInfo->sSCROutletTemperature;
		
		ptr[len++] = pDataStreamInfo->usDPF >> 8;
		ptr[len++] = pDataStreamInfo->usDPF;
		
		ptr[len++] = pDataStreamInfo->cCoolantTemperature;
		
		ptr[len++] = pDataStreamInfo->ucLevel;
		ptr[len++] = pDataStreamInfo->ucGpsStatus;
		
		ptr[len++] = pDataStreamInfo->ulLong >> 24; 
		ptr[len++] = pDataStreamInfo->ulLong >> 16;
		ptr[len++] = pDataStreamInfo->ulLong >> 8;
		ptr[len++] = pDataStreamInfo->ulLong;
		
		ptr[len++] = pDataStreamInfo->ulLat >> 24;	
		ptr[len++] = pDataStreamInfo->ulLat >> 16;
		ptr[len++] = pDataStreamInfo->ulLat >> 8;
		ptr[len++] = pDataStreamInfo->ulLat;
		
		ptr[len++] = pDataStreamInfo->ulMilage >> 24;	
		ptr[len++] = pDataStreamInfo->ulMilage >> 16;
		ptr[len++] = pDataStreamInfo->ulMilage >> 8;
		ptr[len++] = pDataStreamInfo->ulMilage;
	}

	return J5_DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


//实时OBD信息
int J5_RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_OBDInfo *pOBDInfo)
{
	return J5_OBDInfoBag(pOutBuf,usOutBufSize,J5_CMD_REAL_INFO,pOBDInfo);
}

//补传OBD信息
int J5_SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_OBDInfo *pOBDInfo)
{
	return J5_OBDInfoBag(pOutBuf,usOutBufSize,J5_CMD_SUPPLY_INFO,pOBDInfo);
}

//实时数据流信息
int J5_RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo)
{
	return J5_DataStreamInfoBag(pOutBuf,usOutBufSize,J5_CMD_REAL_INFO,pDataStreamInfo,1);
}

//多包实时数据流信息
int J5_RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return J5_DataStreamInfoBag(pOutBuf,usOutBufSize,J5_CMD_REAL_INFO,pDataStreamInfo,bagcnt);
}

//补传数据流信息
int J5_SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo)
{
	return J5_DataStreamInfoBag(pOutBuf,usOutBufSize,J5_CMD_SUPPLY_INFO,pDataStreamInfo,1);
}

//多包补传数据流信息
int J5_SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,J5_DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return J5_DataStreamInfoBag(pOutBuf,usOutBufSize,J5_CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt);
}


