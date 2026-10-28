#include "CN6.h"


typedef struct tagDataInfo
{
	INT8U *pVIN;
	INT8U Ver;
	INT8U Way;
}DataInfo;


DataInfo sDataInfo = {0};


typedef struct tagSaveData
{
	INT16U usLoginFlow;			//登录流水 以天为单位
	INT16U usRealInfoFlow;		//实时信息流水 以天为单位
	INT16U usSupplyInfoFlow;	//补发信息流水 以天为单位

	INT16U usHjLoginFlow;			//登录流水 以天为单位
	INT16U usHjRealInfoFlow;		//实时信息流水 以天为单位
	INT16U usHjSupplyInfoFlow;	//补发信息流水 以天为单位
}SaveData;
SaveData stSaveData = {0};

#define CN6_STREAM_BODY_LEN 37

static unsigned int cn6_payload_cap(unsigned int usOutBufSize)
{
	if (usOutBufSize <= (unsigned int)OFFSET_DATA_UNIT + 1)
	{
		return 0;
	}
	return usOutBufSize - (unsigned int)OFFSET_DATA_UNIT - 1;
}




//封包
int DataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,void *pDataUnit,INT16U usDataUnitLen)
{
	INT16U len = 0;
	INT8U *pbuf = (INT8U *)pOutBuf;

	if (pbuf == 0)
	{
		return 0;
	}
	//判断缓冲区大小
	//起始符(2B)+命令字(1B)+VIN(17B)+软件版本号(1B)+加密方式(1B)+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
	if (usOutBufSize < (2+1+LEN_VIN+1+1+2+usDataUnitLen+1))
	{
		return 0;
	}
	//起始字符串 2B
	pbuf[len++] = HEAD0;
	pbuf[len++] = HEAD1;

	//命令单元 1B
	pbuf[len++] = cmd;

	//车辆识别号
	memcpy(pbuf + len,sDataInfo.pVIN,LEN_VIN);
	len += LEN_VIN;

	//终端版本号 1B
	pbuf[len++] = sDataInfo.Ver;

	//数据加密方式	1B
	pbuf[len++] = sDataInfo.Way;

	//数据单元长度	2B
	pbuf[len++] = usDataUnitLen >> 8;
	pbuf[len++] = usDataUnitLen;

	//数据单元
	if (pDataUnit && usDataUnitLen)
	{
		memcpy(pbuf + len,pDataUnit,usDataUnitLen);
		len += usDataUnitLen;
	}

	//异或校验
	pbuf[len++] = XOR_Check(pbuf+2,len-2);

	return len;
}


void CN6_Init(void *szVIN,INT8U Ver,INT8U Way)
{
	sDataInfo.pVIN = szVIN;
	sDataInfo.Ver = Ver;
	sDataInfo.Way = Way;
}

//登录包
int LoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID)
{
	INT8U temp[32] = {0};
	INT8U len = 0;
	char *p = (char *)szICCID;

	if ((BCDTime == 0) || (szICCID == 0))
	{
		return 0;
	}

	//时间
	memcpy(temp + len,BCDTime,6);
	len += 6;

	//流水号
	stSaveData.usLoginFlow++;
	temp[len++] = stSaveData.usLoginFlow >> 8;
	temp[len++] = stSaveData.usLoginFlow;

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

	return DataBag(pOutBuf,usOutBufSize,CMD_LOGIN,temp,len);
}



//登录包
int SHLoginDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID)
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
	temp[len++] = 0;
	temp[len++] = 0;
	stSaveData.usLoginFlow++;
	temp[len++] = stSaveData.usLoginFlow >> 8;
	temp[len++] = stSaveData.usLoginFlow;

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

	return DataBag(pOutBuf,usOutBufSize,CMD_LOGIN,temp,len);
}

//登出包
int LogoutDataPacket(void *pOutBuf,INT16U usOutBufSize,INT8U BCDTime[6],void *szICCID)
{
	INT8U temp[32] = {0};
	INT8U len = 0;
	//char *p = (char *)szICCID;

	if ((BCDTime == 0) || (szICCID == 0))
	{
		return 0;
	}

	//时间
	memcpy(temp + len,BCDTime,6);
	len += 6;

	//流水号
	temp[len++] = stSaveData.usLoginFlow >> 8;
	temp[len++] = stSaveData.usLoginFlow;

	return DataBag(pOutBuf,usOutBufSize,CMD_LOGOUT,temp,len);
}


//终端校时包
int CalibrationTimePacket(void *pOutBuf,INT16U usOutBufSize)
{
	return DataBag(pOutBuf,usOutBufSize,CMD_CALIBRATIONTIME,0,0);
}



int SHOBDInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,OBDInfo *pOBDInfo)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	INT8U i = 0;
	INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pOBDInfo == 0)) return -1;

	usDataUnitLen = 6+1+2+6+LEN_VIN+LEN_SOFTVER+LEN_CVN+LEN_IUPR+1+4 * pOBDInfo->ucDTCTotalNum;	//数据单元长度

	if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	{
		return 0;	//the buf have no enough size
	}
	//数据采集时间
	memcpy(ptr,pOBDInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stSaveData.usRealInfoFlow++;
	ptr[len++] = stSaveData.usRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usRealInfoFlow;
	//信息类型标志
	ptr[len++] = OBD_INFO;

	//OBD信息体
	ptr[len++] = pOBDInfo->ucOBDProtocol;
	ptr[len++] = pOBDInfo->ucMilStatus;
	ptr[len++] = pOBDInfo->usSupportStatus >> 8;
	ptr[len++] = pOBDInfo->usSupportStatus;
	ptr[len++] = pOBDInfo->usReadyStatus >> 8;
	ptr[len++] = pOBDInfo->usReadyStatus;

	//memcpy(ptr + len, sDataInfo.pVIN, LEN_VIN);
	memcpy(ptr + len,pOBDInfo->szVIN,LEN_VIN);
	len += LEN_VIN;

	memcpy(ptr + len, pOBDInfo->szCalid, LEN_SOFTVER);
	len += LEN_SOFTVER;

	memcpy(ptr + len, pOBDInfo->szCVN, LEN_CVN);
	len += LEN_CVN;
	
	memcpy(ptr + len, pOBDInfo->szIUPR, LEN_IUPR);
	len += LEN_IUPR;

	ptr[len++] = pOBDInfo->ucDTCTotalNum;

	if (pOBDInfo->ucDTCTotalNum && (pOBDInfo->ucDTCTotalNum < 0xFE))
	{
		for (i = 0; i < pOBDInfo->ucDTCTotalNum; i++)
		{
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 0);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 1);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 2);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 3);
		}

	}
	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}



int OBDInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,OBDInfo *pOBDInfo)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	INT8U i = 0;
	INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pOBDInfo == 0)) return -1;

	usDataUnitLen = 6+1+2+6+LEN_VIN+LEN_SOFTVER+LEN_CVN+LEN_IUPR+1+4 * pOBDInfo->ucDTCTotalNum;	//数据单元长度

	if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	{
		return -2;	//the buf have no enough size
	}
	//数据采集时间
	memcpy(ptr,pOBDInfo->BCDTime,6);
	len += 6;
	//信息类型标志
	ptr[len++] = OBD_INFO;

	//实时信息流水号
	stSaveData.usRealInfoFlow++;
	ptr[len++] = stSaveData.usRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usRealInfoFlow;

	//OBD信息体
	ptr[len++] = pOBDInfo->ucOBDProtocol;
	ptr[len++] = pOBDInfo->ucMilStatus;
	ptr[len++] = pOBDInfo->usSupportStatus >> 8;
	ptr[len++] = pOBDInfo->usSupportStatus;
	ptr[len++] = pOBDInfo->usReadyStatus >> 8;
	ptr[len++] = pOBDInfo->usReadyStatus;

	//memcpy(ptr + len, sDataInfo.pVIN, LEN_VIN);
	memcpy(ptr + len,pOBDInfo->szVIN,LEN_VIN);
	len += LEN_VIN;

	memcpy(ptr + len, pOBDInfo->szCalid, LEN_SOFTVER);
	len += LEN_SOFTVER;

	memcpy(ptr + len, pOBDInfo->szCVN, LEN_CVN);
	len += LEN_CVN;
	
	memcpy(ptr + len, pOBDInfo->szIUPR, LEN_IUPR);
	len += LEN_IUPR;

	ptr[len++] = pOBDInfo->ucDTCTotalNum;

	if (pOBDInfo->ucDTCTotalNum < 0xFE)
	{
		for (i = 0; i < pOBDInfo->ucDTCTotalNum; i++)
		{
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 0);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 1);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 2);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 3);
		}

	}
	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


int DataStreamInfoBagEx(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								DataStreamInfo *pDataStreamInfo1,
								unsigned int bagcnt,char is_17691)
{
	DataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return -1;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 6) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + CN6_STREAM_BODY_LEN + 7;
		if ((!is_17691) || (i == 0)) need += 2;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;

		if (is_17691)
		{
			//只要一个流水号
			if (i == 0)
			{
				//实时信息流水号
				stSaveData.usRealInfoFlow++;
				ptr[len++] = stSaveData.usRealInfoFlow >> 8;
				ptr[len++] = stSaveData.usRealInfoFlow;
			}
		}
		else
		{
			//实时信息流水号
			stSaveData.usRealInfoFlow++;
			ptr[len++] = stSaveData.usRealInfoFlow >> 8;
			ptr[len++] = stSaveData.usRealInfoFlow;
		}
		
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


		//信息类型标志
		ptr[len++] = KELIWUCHUANGANQI_INFO;

		ptr[len++] = pDataStreamInfo->guangxishouxishu >> 8;
		ptr[len++] = pDataStreamInfo->guangxishouxishu;

		ptr[len++] = pDataStreamInfo->butouguangdu >> 8;
		ptr[len++] = pDataStreamInfo->butouguangdu;

		ptr[len++] = pDataStreamInfo->keliwunongdu >> 8;
		ptr[len++] = pDataStreamInfo->keliwunongdu;

	}

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


int DataStreamInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								DataStreamInfo *pDataStreamInfo1,
								unsigned int bagcnt,char is_17691)
{
	DataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return -1;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 6) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + CN6_STREAM_BODY_LEN;
		if ((!is_17691) || (i == 0)) need += 2;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;

		if (is_17691)
		{
			//只要一个流水号
			if (i == 0)
			{
				//实时信息流水号
				stSaveData.usRealInfoFlow++;
				ptr[len++] = stSaveData.usRealInfoFlow >> 8;
				ptr[len++] = stSaveData.usRealInfoFlow;
			}
		}
		else
		{
			//实时信息流水号
			stSaveData.usRealInfoFlow++;
			ptr[len++] = stSaveData.usRealInfoFlow >> 8;
			ptr[len++] = stSaveData.usRealInfoFlow;
		}
		
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

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}

int DataStreamAndObdInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
										OBDInfo *pOBDInfo,
										DataStreamInfo *pDataStreamInfo1,
										unsigned int bagcnt,char is_17691)
{
	DataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	unsigned int j = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOBDInfo == 0) || (pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return 0;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 6) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	for (i = 0; i < bagcnt; i++)
	{
		{
			unsigned int dtc = pOBDInfo->ucDTCTotalNum;
			if (dtc > 20) dtc = 20;
			need = 1 + 2 + CN6_STREAM_BODY_LEN;
			if (i == 0) need += 1 + 2 + 6 + LEN_VIN + LEN_SOFTVER + LEN_CVN + LEN_IUPR + 1 + (4 * dtc);
			if ((unsigned int)len + need > cap) break;
		}
		pDataStreamInfo = &pDataStreamInfo1[i];

		if (i == 0)
		{
			//信息类型标志
			ptr[len++] = OBD_INFO;
			//实时信息流水号
			stSaveData.usRealInfoFlow++;
			ptr[len++] = stSaveData.usRealInfoFlow >> 8;
			ptr[len++] = stSaveData.usRealInfoFlow;

			//OBD信息体
			ptr[len++] = pOBDInfo->ucOBDProtocol;
			ptr[len++] = pOBDInfo->ucMilStatus;
			ptr[len++] = pOBDInfo->usSupportStatus >> 8;
			ptr[len++] = pOBDInfo->usSupportStatus;
			ptr[len++] = pOBDInfo->usReadyStatus >> 8;
			ptr[len++] = pOBDInfo->usReadyStatus;
			
			//memcpy(ptr + len, sDataInfo.pVIN, LEN_VIN);
			memcpy(ptr + len,pOBDInfo->szVIN,LEN_VIN);
			len += LEN_VIN;
			
			memcpy(ptr + len, pOBDInfo->szCalid, LEN_SOFTVER);
			len += LEN_SOFTVER;
			
			memcpy(ptr + len, pOBDInfo->szCVN, LEN_CVN);
			len += LEN_CVN;
			
			memcpy(ptr + len, pOBDInfo->szIUPR, LEN_IUPR);
			len += LEN_IUPR;
			
			ptr[len++] = pOBDInfo->ucDTCTotalNum;
			
			if (pOBDInfo->ucDTCTotalNum > 20) pOBDInfo->ucDTCTotalNum = 20;
			for (j = 0; j < pOBDInfo->ucDTCTotalNum; j++)
			{
				ptr[len++] = *(pOBDInfo->pDTCList + 4 * j + 0);
				ptr[len++] = *(pOBDInfo->pDTCList + 4 * j + 1);
				ptr[len++] = *(pOBDInfo->pDTCList + 4 * j + 2);
				ptr[len++] = *(pOBDInfo->pDTCList + 4 * j + 3);
			}				
		}

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;

		ptr[len++] = stSaveData.usRealInfoFlow >> 8;
		ptr[len++] = stSaveData.usRealInfoFlow;

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

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}

int HJOBDInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,OBDInfo *pOBDInfo)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	INT8U i = 0;
	INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pOBDInfo == 0)) return 0;

	usDataUnitLen = 6+1+2+6+LEN_VIN+LEN_SOFTVER+LEN_CVN+LEN_IUPR+1+4 * pOBDInfo->ucDTCTotalNum;	//数据单元长度

	if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	{
		return 0;	//the buf have no enough size
	}

	//发送时间
	memcpy(ptr,pOBDInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stSaveData.usHjRealInfoFlow++;
	ptr[len++] = stSaveData.usHjRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usHjRealInfoFlow;

	//信息类型标志
	ptr[len++] = OBD_INFO;

	//数据采集时间
	memcpy(ptr + len,pOBDInfo->BCDTime,6);
	len += 6;

	//OBD信息体
	ptr[len++] = pOBDInfo->ucOBDProtocol;
	ptr[len++] = pOBDInfo->ucMilStatus;
	ptr[len++] = pOBDInfo->usSupportStatus >> 8;
	ptr[len++] = pOBDInfo->usSupportStatus;
	ptr[len++] = pOBDInfo->usReadyStatus >> 8;
	ptr[len++] = pOBDInfo->usReadyStatus;

	//memcpy(ptr + len, sDataInfo.pVIN, LEN_VIN);
	memcpy(ptr + len,pOBDInfo->szVIN,LEN_VIN);
	len += LEN_VIN;

	memcpy(ptr + len, pOBDInfo->szCalid, LEN_SOFTVER);
	len += LEN_SOFTVER;

	memcpy(ptr + len, pOBDInfo->szCVN, LEN_CVN);
	len += LEN_CVN;
	
	memcpy(ptr + len, pOBDInfo->szIUPR, LEN_IUPR);
	len += LEN_IUPR;

	ptr[len++] = pOBDInfo->ucDTCTotalNum;

	if (pOBDInfo->ucDTCTotalNum < 0xFE)
	{
		for (i = 0; i < pOBDInfo->ucDTCTotalNum; i++)
		{
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 0);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 1);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 2);
			ptr[len++] = *(pOBDInfo->pDTCList + 4 * i + 3);
		}

	}
	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


int HJDataStreamBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
										OBDInfo *pOBDInfo,
										DataStreamInfo *pDataStreamInfo1,
										unsigned int bagcnt,char is_17691)
{
	DataStreamInfo *pDataStreamInfo = 0;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOBDInfo == 0) || (pOutBuf == 0) || (pDataStreamInfo1 == 0) || (bagcnt == 0)) return 0;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 8) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	pDataStreamInfo = &pDataStreamInfo1[bagcnt - 1];

	//数据发送时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;	

	//实时信息流水号
	stSaveData.usHjRealInfoFlow++;
	ptr[len++] = stSaveData.usHjRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usHjRealInfoFlow;


	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + 6 + CN6_STREAM_BODY_LEN;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;

		//数据采集时间
		memcpy(ptr + len,pDataStreamInfo->BCDTime,6);
		len += 6;

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

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}



//上海数据流信息打包
int SHALLDataStreamInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								SHALLDataStreamInfo *pDataStreamInfo1,
								unsigned int bagcnt,char is_17691)
{
	SHALLDataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return -1;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 8) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stSaveData.usRealInfoFlow++;
	ptr[len++] = stSaveData.usRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usRealInfoFlow;

	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + CN6_STREAM_BODY_LEN + 1 + 17;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;
		
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

		//信息类型标志
		ptr[len++] = SH_DATA_STREAM_EX_INFO;
		
		//补充数据流信息体
		ptr[len++] = pDataStreamInfo->engine_nj_mode;		//发动机扭矩模式
		ptr[len++] = pDataStreamInfo->ymtb; 				//油门踏板
		
		ptr[len++] = pDataStreamInfo->total_oil >> 24;		//累计油耗
		ptr[len++] = pDataStreamInfo->total_oil >> 16;
		ptr[len++] = pDataStreamInfo->total_oil >> 8;
		ptr[len++] = pDataStreamInfo->total_oil;
		
		ptr[len++] = pDataStreamInfo->nsxwd;				//尿素箱温度
		
		ptr[len++] = pDataStreamInfo->sjnspsl >> 24;		//实际尿素喷射量
		ptr[len++] = pDataStreamInfo->sjnspsl >> 16;
		ptr[len++] = pDataStreamInfo->sjnspsl >> 8;
		ptr[len++] = pDataStreamInfo->sjnspsl;
		
		ptr[len++] = pDataStreamInfo->ljnsxh >> 24; 		//累计尿素消耗
		ptr[len++] = pDataStreamInfo->ljnsxh >> 16;
		ptr[len++] = pDataStreamInfo->ljnsxh >> 8;
		ptr[len++] = pDataStreamInfo->ljnsxh;
		
		ptr[len++] = pDataStreamInfo->dpfpqwd >> 8;			//DPF排气温度
		ptr[len++] = pDataStreamInfo->dpfpqwd;
	}

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


//上海数据流信息打包
int SHDataStreamInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								DataStreamInfo *pDataStreamInfo1,
								unsigned int bagcnt,char is_17691)
{
	DataStreamInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return -1;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 8) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return -2;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stSaveData.usRealInfoFlow++;
	ptr[len++] = stSaveData.usRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usRealInfoFlow;

	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + CN6_STREAM_BODY_LEN;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = DATA_STREAM_INFO;
		
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

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


//上海协议 补充数据流
unsigned int SHDataStreamExInfoBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,
								SHDataStreamExInfo *pDataStreamInfo1,
								unsigned int bagcnt,char is_17691)
{
	SHDataStreamExInfo *pDataStreamInfo = pDataStreamInfo1;
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;
	unsigned int i = 0;
	unsigned int cap = 0;
	unsigned int need = 0;
	//INT16U usDataUnitLen = 0;	//数据单元长度

	if ((pOutBuf == 0) || (pDataStreamInfo == 0) || (bagcnt == 0)) return 0;
	cap = cn6_payload_cap(usOutBufSize);
	if (cap < 8) return 0;

	//usDataUnitLen = 6+1+2+sizeof(DataStreamInfo) * bagcnt;	//数据单元长度

	//if ((usOutBufSize - OFFSET_DATA_UNIT) <= usDataUnitLen)
	//{
		//return 0;	//the buf have no enough size
	//}

	//数据采集时间
	memcpy(ptr,pDataStreamInfo->BCDTime,6);
	len += 6;

	//实时信息流水号
	stSaveData.usRealInfoFlow++;
	ptr[len++] = stSaveData.usRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usRealInfoFlow;

	for (i = 0; i < bagcnt; i++)
	{
		need = 1 + 17;
		if ((unsigned int)len + need > cap) break;
		pDataStreamInfo = &pDataStreamInfo1[i];

		//信息类型标志
		ptr[len++] = SH_DATA_STREAM_EX_INFO;

		
		//补充数据流信息体
		ptr[len++] = pDataStreamInfo->engine_nj_mode;		//发动机扭矩模式
		ptr[len++] = pDataStreamInfo->ymtb;					//油门踏板

		ptr[len++] = pDataStreamInfo->total_oil >> 24; 		//累计油耗
		ptr[len++] = pDataStreamInfo->total_oil >> 16;
		ptr[len++] = pDataStreamInfo->total_oil >> 8;
		ptr[len++] = pDataStreamInfo->total_oil;

		ptr[len++] = pDataStreamInfo->nsxwd;				//尿素箱温度
		
		ptr[len++] = pDataStreamInfo->sjnspsl >> 24;		//实际尿素喷射量
		ptr[len++] = pDataStreamInfo->sjnspsl >> 16;
		ptr[len++] = pDataStreamInfo->sjnspsl >> 8;
		ptr[len++] = pDataStreamInfo->sjnspsl;
		
		ptr[len++] = pDataStreamInfo->ljnsxh >> 24;			//累计尿素消耗
		ptr[len++] = pDataStreamInfo->ljnsxh >> 16;
		ptr[len++] = pDataStreamInfo->ljnsxh >> 8;
		ptr[len++] = pDataStreamInfo->ljnsxh;

																
		ptr[len++] = pDataStreamInfo->dpfpqwd >> 8;			//DPF排气温度
		ptr[len++] = pDataStreamInfo->dpfpqwd;
	}

	return DataBag(pOutBuf,usOutBufSize,cmd,ptr,len);
}


//上海实时OBD信息
int SHRealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return SHOBDInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pOBDInfo);
}

//上海补传OBD信息
int SHSupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return SHOBDInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pOBDInfo);
}


//实时OBD信息
int RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return OBDInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pOBDInfo);
}

//补传OBD信息
int SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return OBDInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pOBDInfo);
}

//实时数据流信息
int RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,1,0);
}

//多包实时数据流信息
int RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,0);
}

//补传数据流信息
int SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,1,0);
}

//多包补传数据流信息
int SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt,0);
}

//实时数据流信息
int GB17691_RealDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,1,1);
}

//多包实时数据流信息
int GB17691_SHRealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHDataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,1);
}

//多包实时数据流信息
int GB17691_SHALLRealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHALLDataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHALLDataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,1);
}

//多包补传数据流信息
int GB17691_SHALLSupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHALLDataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHALLDataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt,1);
}


//多包补传数据流信息
int GB17691_SHSupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHDataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt,1);
}

//多包实时数据流信息
int GB17691_RealDataStreamAndObdInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamAndObdInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pOBDInfo,pDataStreamInfo,bagcnt,1);
}


//多包实时数据流信息
int GB17691_RealDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,1);
}

//多包实时数据流信息
int GB17691_RealDataStreamInfoExPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamInfoBagEx(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,1);
}



//多包实时补充数据流信息
unsigned int GB17691_SHRealDataStreamExInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHDataStreamExInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHDataStreamExInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pDataStreamInfo,bagcnt,1);
}

int GB17691_SupplyDataStreamAndObdInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamAndObdInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pOBDInfo,pDataStreamInfo,bagcnt,1);
}

//补传数据流信息
int GB17691_SupplyDataStreamInfoPacket(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,1,1);
}

//多包补传数据流信息
int GB17691_SupplyDataStreamInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return DataStreamInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt,1);
}

//多包补传补充数据流信息
int GB17691_SHSupplyDataStreamExInfoPacketMutli(void *pOutBuf,INT16U usOutBufSize,SHDataStreamExInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return SHDataStreamExInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pDataStreamInfo,bagcnt,1);
}



unsigned char bengbu_login_flag = 0;

void set_bengbu_login_flag(unsigned char val)
{
	bengbu_login_flag = val;
}

unsigned char get_bengbu_login_flag(void)
{
	return bengbu_login_flag;
}

//起始符 + 命令单元 + VIN + 应答标志
void anlyze_bengbu_server_data(void *pbuf,unsigned int len)
{
	unsigned char cmd = 0;
	unsigned char ack = 0;
	unsigned char *psrc = (unsigned char *)pbuf;
	unsigned char *ptr = 0;
	unsigned int rlen = len;

	while (1)
	{
		ptr = _find_sub_str(psrc,rlen,"\x23\x23",2);
		if (ptr == 0)
		{
			break;
		}
		cmd = ptr[2];
		ack = ptr[3 + 17];
		DBG_Print(LEVEL3,"----------server ack----------\r\ncmd:%02X ack:%02X\r\n",
		cmd,ack);

		if (cmd == CMD_LOGIN)
		{
			if (ack == 0)
			{
				set_bengbu_login_flag(1);
			}
		}

		psrc = ptr + 21;
		rlen = (unsigned char *)pbuf + len - psrc;
	}
}



//起始符 + 命令单元 + 服务器时间 + 数据单元长度 + 数据单元 + 校验码
void anlyze_shanghai_server_data(void *pbuf,unsigned int len)
{
	unsigned char cmd = 0;
	unsigned char *psrc = (unsigned char *)pbuf;
	unsigned char *ptr = 0;
	unsigned int rlen = len;
	unsigned int datunitlen = 0;
	unsigned char xor = 0;

	if (len < (2+1+6+2+1))
	{
		return ;
	}
	while (1)
	{
		ptr = _find_sub_str(psrc,rlen,"\x23\x23",2);
		if (ptr == 0)
		{
			break;
		}
		cmd = ptr[2];
		datunitlen = (ptr[2+1+6] << 8) + ptr[2+1+6 + 1];
		if (rlen >= (2+1+6+2+datunitlen+1))
		{
			xor = ptr[2+1+6+2+datunitlen];
			DBG_Print(LEVEL0,"----------shanghai server ack----------\r\n"
			"cmd:%02X ack_time:%02d-%02d-%02d %02d:%02d:%02d datunitlen:%u xor:%02X\r\n",
			cmd,ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],datunitlen,xor);
			
			switch (cmd)
			{
			case 0x01:		//登录成功
			
			break;
			case 0x02:		//实时信息上传成功
			
			break;
			case 0x05:		//心跳包
			
			break;
			case 0x06:		//远程升级
			{
				unsigned char *url = ptr+2+1+6+2+2;
			
				ptr[2+1+6+2+datunitlen] = 0;	//校验码清0
				DBG_Print(LEVEL0,"http update:%s\r\n",url);
				//trigger_http_update(url);
			
			}
			break;
			default:break;
			}

		}
		else
		{
			DBG_Print(LEVEL0,"server data format error.\r\n");
			break;
		}

		psrc = ptr + 2 + 1 + 6 + 2 + datunitlen + 1;
		if (((unsigned char *)pbuf + len) > psrc)
		{
			rlen = (unsigned char *)pbuf + len - psrc;
		}
		else
		{
			break;
		}
	}
}


//起始符 + 命令单元 + VIN + 版本 + 加密方式 + 数据单元长度 + 数据单元 + 校验码
void anlyze_17691_server_data(unsigned int linkid,void *pbuf,unsigned int len)
{
	unsigned char cmd = 0;
	unsigned char *psrc = (unsigned char *)pbuf;
	unsigned char *ptr = 0;
	unsigned char *pcontent = 0;
	unsigned int rlen = len;
	unsigned int datunitlen = 0;
	unsigned char xor = 0;
	unsigned int slen = 0;
	unsigned char psend[64] = {0};
	unsigned char *pack = psend + OFFSET_DATA_UNIT;
	unsigned int xlen = 0;

	if (len < (2+1+17+2+2+1))
	{
		return ;
	}
	while (1)
	{
		ptr = _find_sub_str(psrc,rlen,"\x23\x23",2);
		if (ptr == 0)
		{
			break;
		}
		cmd = ptr[2];
		if (rlen < OFFSET_DATA_UNIT) break;
		datunitlen = (ptr[OFFSET_DATA_UNIT - 2] << 8) + ptr[OFFSET_DATA_UNIT - 1];
		if (rlen >= (2+1+17+2+2+datunitlen+1))
		{
			pcontent = ptr + OFFSET_DATA_UNIT;
			xor = ptr[2+1+17+2+2+datunitlen];
			DBG_Print(LEVEL3,"----------17691 server ack----------\r\n"
								"cmd:%02X datunitlen:%u xor:%02X\r\n",
								cmd,datunitlen,xor);
			switch (cmd)
			{
			case 0x01:		//登录成功
			
			break;
			case 0x02:		//实时信息上传成功
			
			break;
			case 0x05:		//校时
			
			break;
			case 0x08:
				if (linkid == 5)			//备案平台给了应答需要关闭连接
				{
					unsigned char *ps = 0;

					send_msg(S_MSG_SET_SERVER_IP_PORT,(void *)5,"0",0);

					ps = os_malloc(384);
					if (ps)
					{
						slen = df_17691_beian_result_pack(ps,384,
												get_rtc_time_ptr(),
												send_msg(S_MSG_GET_BEIAN_MSG_BODY_WITH_SIGN,0,0,0),169,
												pcontent[0],pcontent[1]
												);

						DBG_PrintBuf(LEVEL2,"report beian result to df server:\r\n",ps,slen);
						send_msg(S_MSG_SEND_TO_SERVER,(void *)2,ps,(void *)slen);
						os_free(ps);
					}
				}
			break;
	
			case 0x80:

			break;
			case 0x81:

			break;
			case 0x82:

			break;
			case 0x92:

			break;
			case 0x93:
			{
				xlen = 0;
				memcpy(pack,get_rtc_time_ptr(),6);
				xlen += 6;

				memcpy(pack + xlen,pcontent + 6,5);
				xlen += 5;

				pack[xlen++] = 0x01;

				slen = DataBag(psend,sizeof(psend),cmd,pack,xlen);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,psend,(void *)slen);
			}
			break;
			case 0x95:				//TSP通用控制命令
			{			
				//应答
				xlen = 0;
				memcpy(pack,get_rtc_time_ptr(),6);
				xlen += 6;
				
				pack[xlen++] = pcontent[6]; 		//id
				
				stSaveData.usHjRealInfoFlow++;
				pack[xlen++] = stSaveData.usHjRealInfoFlow >> 8;
				pack[xlen++] = stSaveData.usHjRealInfoFlow;
				
				//分包标识
				pack[xlen++] = pcontent[9];
				//平台流水
				pack[xlen++] = pcontent[7];
				pack[xlen++] = pcontent[8];
				
				pack[xlen++] = 0x01;			//结果状态
				
				slen = DataBag(psend,sizeof(psend),cmd,pack,xlen);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,psend,(void *)slen);

				if (pcontent[6] == 0x50)			//防篡改激活
				{
					if (pcontent[10])
					{
						DBG_Print(LEVEL0,"tsp request bei an\r\n");		//企业平台触发终端去国家平台备案
						//<ip5:g6check.vecc.org.cn><port5:20006>
						send_hj1239_beian_info_to_server("g6check.vecc.org.cn",20006);
					}
				}
				else if ((pcontent[6] == 0x52) && (datunitlen > 16))		//OTA
				{
					char *pcmd = 0;
					char *purl = 0;
					unsigned char urllen = 0;
					unsigned int templen = 0;

					pcmd = os_malloc(1536);
					if (pcmd)
					{
						purl = _find_sub_str(pcontent + 10,datunitlen - 10,"ftp://",strlen("ftp://"));
						if (purl)
						{
							urllen = *(purl - 1);
							if (urllen < 250)
							{
								//snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ftp://%s:%s@%s>",pusr,psw,purl + strlen("ftp://"));
								templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ");

								memcpy(pcmd + templen,purl,urllen);
								templen += urllen;

								pcmd[templen++] = '>';
								pcmd[templen] = 0;
							}
						}
						else
						{
							DBG_Print(LEVEL0,"%s","not fond ftp server\r\n");
							purl = _find_sub_str(pcontent + 10,datunitlen - 10,"http://",strlen("http://"));
						}

						if (purl && (templen > 0) && platform_remote_wget_allowed(pcmd))
						{
							DBG_Print(0,"[exe cmd]%s\r\n",pcmd);
							send_msg(S_MSG_EXECUTE_DRV_CMD,pcmd,0,0);
						}
						else if (purl)
						{
							DBG_Print(LEVEL0,"reject platform remote wget\r\n");
						}
						os_free(pcmd);
					}

				}
				else if (pcontent[6] == 0x53)		//取消OTA
				{

				}

			}
			break;
			default:break;
			}

		}
		else
		{
			DBG_Print(LEVEL0,"server data format error.\r\n");
			break;
		}

		psrc = ptr + 2+1+17+1+1 + 2 + datunitlen + 1;
		if (((unsigned char *)pbuf + len) > psrc)
		{
			rlen = (unsigned char *)pbuf + len - psrc;
		}
		else
		{
			break;
		}
	}
}




unsigned int hj1239_beian_pack(unsigned char rtime[6],
										void *chipid,unsigned int chipid_len,
										void *public_key,unsigned int pubkey_len,
										void *vin,unsigned int vin_len,
										void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *content = 0;

	if ((chipid == 0) || (chipid_len == 0)
		|| (chipid == 0) || (chipid_len == 0)
		|| (public_key == 0) || (pubkey_len == 0)
		|| (vin == 0) || (vin_len == 0)
		|| (pOut == 0) || (outbuflen < 20)
		)
	{
		return 0;
	}

	content = (unsigned char *)pOut + OFFSET_DATA_UNIT;

	//时间
	content[rlen++] = rtime[0];
	content[rlen++] = rtime[1];
	content[rlen++] = rtime[2];
	content[rlen++] = rtime[3];
	content[rlen++] = rtime[4];
	content[rlen++] = rtime[5];

	//芯片ID
	memcpy(content + rlen,chipid,chipid_len);
	rlen += chipid_len;

	//公钥
	memcpy(content + rlen,public_key,pubkey_len);
	rlen += pubkey_len;

	//VIN
	memcpy(content + rlen,vin,vin_len);
	rlen += vin_len;

	slen = DataBag(pOut,outbuflen,CMD_BEIAN,content,rlen);

	return slen;
}

//HJ1239多包实时数据流信息
int HJ1239_RealDataStreamPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return HJDataStreamBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pOBDInfo,pDataStreamInfo,bagcnt,1);
}

int HJ1239_SupplyDataStreamPacketMutli(void *pOutBuf,INT16U usOutBufSize,
							OBDInfo *pOBDInfo,DataStreamInfo *pDataStreamInfo,unsigned int bagcnt)
{
	return HJDataStreamBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pOBDInfo,pDataStreamInfo,bagcnt,1);
}

//HJ1239实时OBD信息
int HJ1239_RealOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return HJOBDInfoBag(pOutBuf,usOutBufSize,CMD_REAL_INFO,pOBDInfo);
}

//HJ1239补传OBD信息
int HJ1239_SupplyOBDInfoPacket(void *pOutBuf,INT16U usOutBufSize,OBDInfo *pOBDInfo)
{
	return HJOBDInfoBag(pOutBuf,usOutBufSize,CMD_SUPPLY_INFO,pOBDInfo);
}


//上报备案结果到东风平台
unsigned int df_17691_beian_result_pack(void *pOutBuf,INT16U usOutBufSize,
														INT8U t[6],
														void *beian_info,unsigned int beian_info_len,
														INT8U ack_code,INT8U ack_info)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;

	if ((pOutBuf == 0) || (usOutBufSize == 0)) return 0;

	//数据采集时间
	if (t) memcpy(ptr,t,6);
	len += 6;

	//防篡改激活结果上报标志
	ptr[len++] = 0x50;

	//实时信息流水号
	stSaveData.usHjRealInfoFlow++;
	ptr[len++] = stSaveData.usHjRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usHjRealInfoFlow;

	//分包标识
	ptr[len++] = 0;

	//信息体
	if (beian_info && beian_info_len)
	{
		memcpy(ptr + len,beian_info,beian_info_len);
		len += beian_info_len;
	}

	//应答状态码
	ptr[len++] = ack_code;

	//应答信息
	ptr[len++] = ack_info;

	return DataBag(pOutBuf,usOutBufSize,0x25,ptr,len);
}


unsigned int df_17691_beian_result_pack1(void *pOutBuf,INT16U usOutBufSize,
														INT8U t[6],
														void *chip_id,unsigned int chipidlen,
														void *pubkey,unsigned int pubkeylen,
														void *pvin,unsigned int vinlen,
														void *phj1239_sign,unsigned int hj1239_sign_len,
														INT8U ack_code,INT8U ack_info)
{
	INT16U len = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;

	if ((pOutBuf == 0) || (usOutBufSize == 0)) return 0;

	//数据采集时间
	if (t) memcpy(ptr,t,6);
	len += 6;

	//防篡改激活结果上报标志
	ptr[len++] = 0x50;

	//实时信息流水号
	stSaveData.usHjRealInfoFlow++;
	ptr[len++] = stSaveData.usHjRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usHjRealInfoFlow;

	//分包标识
	ptr[len++] = 0;

	//信息体

	//激活时的时间
	if (t) memcpy(ptr + len,t,6);
	len += 6;

	//芯片ID
	if (chip_id && chipidlen)
	{
		memcpy(ptr + len,chip_id,chipidlen);
		len += chipidlen;
	}

	//公钥
	if (pubkey && pubkeylen)
	{
		memcpy(ptr + len,pubkey,pubkeylen);
		len += pubkeylen;
	}
	//VIN
	if (pvin && vinlen)
	{
		memcpy(ptr + len,pvin,vinlen);
		len += vinlen;
	}

	//激活时的签名信息
	if (phj1239_sign && hj1239_sign_len)
	{
		memcpy(ptr + len,phj1239_sign,hj1239_sign_len);
		len += hj1239_sign_len;
	}
	//应答状态码
	ptr[len++] = ack_code;

	//应答信息
	ptr[len++] = ack_info;

	return DataBag(pOutBuf,usOutBufSize,0x25,ptr,len);
}


unsigned int df_17691_baseinfo_pack(void *pOutBuf,INT16U usOutBufSize,
												INT8U t[6],
												void *chipid,unsigned int chipid_len,
												void *piccid,unsigned int iccid_len,
												void *pesn,unsigned int esn_len
												)
{
	INT16U len = 0;
	INT16U templen = 0;
	unsigned char *pimei = 0;	
	char *p_mpu_ver = 0;
	INT8U *ptr = (INT8U *)pOutBuf + OFFSET_DATA_UNIT;

	if ((pOutBuf == 0) || (usOutBufSize == 0)) return 0;

	//数据采集时间
	if (t) memcpy(ptr,t,6);
	len += 6;

	//基础信息
	ptr[len++] = 0x51;

	//实时信息流水号
	stSaveData.usHjRealInfoFlow++;
	ptr[len++] = stSaveData.usHjRealInfoFlow >> 8;
	ptr[len++] = stSaveData.usHjRealInfoFlow;

	//分包标识
	ptr[len++] = 0;

	//信息体

	//协议版本
	ptr[len++] = 0x02;

	//终端ID
	pimei = send_msg(S_MSG_GET_IMEI,0,0,0);
	templen = strlen((char *)pimei);
	if (templen >= 24)
	{
		templen = 24;
	}
	ptr[len++] = templen;
	memcpy(ptr + len,pimei,templen);
	len += templen;

	//硬件版本
	ptr[len++] = strlen("V1.0");
	memcpy(ptr + len,"V1.0",strlen("V1.0"));
	len += strlen("V1.0");

	//软件版本
	p_mpu_ver = send_msg(S_MSG_GET_MPU_SOFTWARE_VERSION,0,0,0);
	if ((p_mpu_ver == 0) || (strlen((char *)p_mpu_ver) == 0))
	{
		return 0;
	}

	templen = strlen(APP_VERSION) + strlen((char *)p_mpu_ver);
	if (templen >= 47)
	{
		templen = 47;
	}

	ptr[len++] = templen;
	memcpy(ptr+len,APP_VERSION,(strlen(APP_VERSION) > templen) ? templen : strlen(APP_VERSION));
	len += ((strlen(APP_VERSION) > templen) ? templen : strlen(APP_VERSION));
	if (templen > strlen(APP_VERSION))
	{
		memcpy(ptr + len,p_mpu_ver,templen - strlen(APP_VERSION));
		len += (templen - strlen(APP_VERSION));
	}


	//终端型号
	ptr[len++] = strlen("JJ-Q2019");
	memcpy(ptr + len,"JJ-Q2019",strlen("JJ-Q2019"));
	len += strlen("JJ-Q2019");

	//加密芯片型号
	ptr[len++] = strlen("HSC32I1");
	memcpy(ptr + len,"HSC32I1",strlen("HSC32I1"));
	len += strlen("HSC32I1");

	//芯片ID
	if (chipid && chipid_len)
	{
		memcpy(ptr + len,chipid,chipid_len);
	}
	len += 16;

	//iccid
	if (piccid)
	{
		memcpy(ptr + len,piccid,20);
	}
	len += 20;

	//sim
	ptr[len++] = 13;
	memcpy(ptr + len,"144",3);
	if (piccid) memcpy(ptr + len + 3,(unsigned char *)piccid + 10,10);
	len += 13;

	//车牌号
	ptr[len++] = strlen("鄂A00001");
	memcpy(ptr + len,"鄂A00001",strlen("鄂A00001"));
	len += strlen("鄂A00001");

	//发动机序列号
	if (pesn && esn_len)
	{
		ptr[len++] = esn_len;
		memcpy(ptr + len,pesn,esn_len);
		len += esn_len;
	}
	else
	{
		ptr[len++] = 0;
	}

	//ECU源地址
	ptr[len++] = 1;

	//ECU标定版本号
	ptr[len++] = strlen("ECU_V1.0.1");
	memcpy(ptr + len,"ECU_V1.0.1",strlen("ECU_V1.0.1"));
	len += strlen("ECU_V1.0.1");

	//ECU型号
	ptr[len++] = strlen("CMMNS");
	memcpy(ptr + len,"CMMNS",strlen("CMMNS"));
	len += strlen("CMMNS");

	//HMI方案选项
	ptr[len++] = strlen("tspBasic");
	memcpy(ptr + len,"tspBasic",strlen("tspBasic"));
	len += strlen("tspBasic");

	//ECU标定文件格式
	ptr[len++] = 1;			//1.bin 2.zip 3.gz

	//OTA
	ptr[len++] = 0;

	//锁车功能
	ptr[len++] = 0;

	//诊断功能
	ptr[len++] = 0;

	return DataBag(pOutBuf,usOutBufSize,0x25,ptr,len);
}


void send_hj1239_beian_info_to_server(void *ip,unsigned int port)
{
	unsigned int templen = 0;
	unsigned char *szTemp = 0;

	if ((ip == 0) || (port == 0))
	{
		return ;
	}

	szTemp = os_malloc(256);
	if (szTemp)
	{
		send_msg(S_MSG_SET_SERVER_IP_PORT,(void *)5,ip,(void *)port);
		
		send_msg(S_MSG_GET_VIN,GetVIN(),0,0);
		send_msg(S_MSG_GET_PUBLIC_KEY,get_pubkey_ptr(),0,0);

		templen = hj1239_beian_pack(get_rtc_time_ptr(),
									send_msg(S_MSG_GET_JIAMI_CHIP_ID,0,0,0),16,
									get_pubkey_ptr(),64,
									GetVIN(),17,
									szTemp,256
									);
		
		send_msg(S_MSG_SEND_TO_SERVER,(void *)5,szTemp,(void *)templen);
		DBG_Print(LEVEL2,"beian to server %s:%u(%u):\r\n",ip,port,templen);
		DBG_PrintBuf(LEVEL2,0,szTemp,templen);

		os_free(szTemp);
	}
}


