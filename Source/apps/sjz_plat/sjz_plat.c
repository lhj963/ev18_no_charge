#include "sjz_plat.h"


#define CN6_HEAD0 0x23
#define CN6_HEAD1 0x23


#define CN6_HEAD_LEN   2			//国六 起始头 长度
#define CN6_CMD_LEN  	1			//国六命令长度	
#define CN6_VIN_LEN		17			//国六VIN长度
#define CN6_VER_LEN		1			//国六软件版本长度
#define CN6_ENTYPE_LEN  1			//国六加密模式长度
#define CN6_DATAUNIT_LEN 2			//国六标识数据单元长度
#define CN6_CHECK_LEN   1
#define CN6_MIN_LEN (CN6_HEAD_LEN+CN6_CMD_LEN+CN6_VIN_LEN+CN6_VER_LEN+CN6_ENTYPE_LEN+CN6_DATAUNIT_LEN+CN6_CHECK_LEN)


#define CN6_HEAD_OFFSET  				0
#define CN6_CMD_OFFSET  				CN6_HEAD_LEN
#define CN6_VIN_OFFSET  				(CN6_CMD_OFFSET+CN6_CMD_LEN)
#define CN6_VER_OFFSET  				(CN6_VIN_OFFSET+CN6_VIN_LEN)
#define CN6_ENTYPE_OFFSET  				(CN6_VER_OFFSET+CN6_VER_LEN)
#define CN6_DATAUNIT_LEN_OFFSET  		(CN6_ENTYPE_OFFSET+CN6_ENTYPE_LEN)
#define CN6_DATAUNIT_CONTENT_OFFSET  	(CN6_DATAUNIT_LEN_OFFSET+CN6_DATAUNIT_LEN)


#define HD_GET_CMD(p)				   (*((unsigned char *)p+CN6_CMD_OFFSET))
#define HD_GET_VIN_PTR(p)				((unsigned char *)p+ CN6_VIN_OFFSET)
#define HD_GET_VER(p)					(*((unsigned char *)p+CN6_VER_OFFSET))
#define HD_GET_ENTYPE(p)				(*((unsigned char *)p+CN6_ENTYPE_OFFSET))
#define HD_GET_CONTENT_LEN(p)			(((*((unsigned char *)p + CN6_DATAUNIT_LEN_OFFSET)) << 8) + *((unsigned char *)p + CN6_DATAUNIT_LEN_OFFSET + 1))
#define HD_GET_CONTENT_PTR(p)			((unsigned char *)p+ CN6_DATAUNIT_CONTENT_OFFSET)

#define HD_GET_ONE_PACK_LEN(p)          (CN6_MIN_LEN + HD_GET_CONTENT_LEN(p))


typedef struct tagSJZ_PLATSYSPARAM
{
	unsigned char vin[20];			//VIN
	unsigned char chipid[32];		//芯片ID
	unsigned char pubkey[64+16];		//公钥

	unsigned int vin_len;
	unsigned int chipid_len;
	unsigned int pubkey_len;

	unsigned int login_flow;
	unsigned int real_flow;

	unsigned char beian_flag : 1;	//备案标志
	unsigned char login_flag : 1;	//登录标志

}SJZ_PLATSYSPARAM;
SJZ_PLATSYSPARAM s_stSJZ_PLATSYSPARAM = {0};


void sjz_set_beian_flag(unsigned char val)
{
	s_stSJZ_PLATSYSPARAM.beian_flag = val;
}

unsigned char sjz_get_beian_flag(void)
{
	return s_stSJZ_PLATSYSPARAM.beian_flag;
}

void sjz_set_login_flag(unsigned char val)
{
	s_stSJZ_PLATSYSPARAM.login_flag = val;
}

unsigned char sjz_get_login_flag(void)
{
	return s_stSJZ_PLATSYSPARAM.login_flag;
}


unsigned int cn6_pcak(unsigned char cmd,unsigned char ver,unsigned char entype,
						void *DataUnit,unsigned int datlen,void *pOutBuf,unsigned int buflen)
{
	unsigned int rlen = 0;
	unsigned int templen = 0;
	unsigned char *ptr = (unsigned char *)pOutBuf;
	unsigned char xor = 0;

	if ((buflen < CN6_MIN_LEN) || (ptr == 0))
	{
		return 0;
	}

	//起始头
	ptr[rlen++] = CN6_HEAD0;
	ptr[rlen++] = CN6_HEAD1;

	//命令单元
	ptr[rlen++] = cmd;

	//VIN always occupies CN6_VIN_LEN so a short VIN cannot wrap the escape scan
	memset(ptr + rlen,0,CN6_VIN_LEN);
	if (s_stSJZ_PLATSYSPARAM.vin_len)
	{
		unsigned int vlen = s_stSJZ_PLATSYSPARAM.vin_len;
		if (vlen > CN6_VIN_LEN) vlen = CN6_VIN_LEN;
		memcpy(ptr+rlen,s_stSJZ_PLATSYSPARAM.vin,vlen);
	}
	rlen += CN6_VIN_LEN;

	ptr[rlen++] = ver;
	ptr[rlen++] = entype;

	if (datlen > (buflen - CN6_MIN_LEN))
	{
		datlen = buflen - CN6_MIN_LEN;
	}

	//数据单元长度
	ptr[rlen++] = datlen >> 8;
	ptr[rlen++] = datlen;

	if (DataUnit)
	{
		templen = datlen;
		if (templen) memcpy(ptr+rlen,DataUnit,templen);
		rlen += templen;
	}

	//校验
	xor = CommSumCheckCode(ptr + CN6_CMD_OFFSET,rlen - CN6_HEAD_LEN);
	ptr[rlen++] = xor;

	return rlen;
}


static void *hd_find_sub_str(void *pbuf,unsigned int ulBufLen,
							 void *SubStr,unsigned int ulSubStrLen)
{
	unsigned char *pSrc = (unsigned char *)pbuf;
	unsigned char *pSubStr = (unsigned char *)SubStr;
	unsigned int i = 0;
	unsigned int j = 0;

	if ((pbuf == 0) || (ulBufLen == 0) || (SubStr == 0) || (ulSubStrLen == 0))
	{
		return 0;
	}

	for (i = 0; (i + ulSubStrLen) <= ulBufLen; i++)
	{
		if (*(pSrc + i) == *pSubStr)	//找到第一个字节
		{
			for (j = 0; j < ulSubStrLen; j++)
			{
				if (*(pSubStr + j) != *(pSrc + i + j))
				{
					//return 0;
					break;
				}
			}
			if (j == ulSubStrLen)
			{
				return (pSrc + i);
			}
		}
	}
	return 0;
}

//转义 send_or_recv:1表示发送转义 0 表示接收转义
void hd_zy(char send_or_recv,void *pdat,unsigned int datlen)
{
	unsigned char *ptr = pdat;
	unsigned int rlen = datlen;
	unsigned int slen = 0;
	unsigned char *pTemp = 0;

	if ((pdat == 0) || (datlen == 0))
	{
		return ;
	}

	//数据还需要转义
	{
		unsigned int hdr = CN6_HEAD_LEN+CN6_CMD_LEN+CN6_VIN_LEN+CN6_VER_LEN+CN6_ENTYPE_LEN+CN6_DATAUNIT_LEN;
		if (rlen <= hdr)
		{
			return ;
		}
		ptr += hdr;
		slen = rlen - hdr;
	}

	while (1)
	{
		if (send_or_recv)
		{
			pTemp = hd_find_sub_str(ptr,slen,"\x23\x23",2);
		}
		else
		{
			pTemp = hd_find_sub_str(ptr,slen,"\xFC\xFC",2);
		}
		if (pTemp)
		{
			if (send_or_recv)
			{
				*pTemp = 0xFC;
				*(pTemp + 1) = 0xFC;
			}
			else
			{
				*pTemp = 0x23;
				*(pTemp + 1) = 0x23;
			}

			slen -= (pTemp - ptr);
			ptr = pTemp;
		}
		else
		{
			break;
		}
	}

}

unsigned int sjz_plat_pack(unsigned char cmd,void *content,unsigned int contentlen,void *pOutBuf,unsigned int buflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOutBuf;

	if ((pOutBuf == 0) || (buflen == 0))
	{
		return 0;
	}

	if ((cmd == HD_PLAT_CMD_REAL_DATA) || (cmd == HD_PLAT_CMD_BF_DATA))
	{
		rlen = cn6_pcak(cmd,0,0x01,content,contentlen,pOutBuf,buflen);
	}
	else
	{
		rlen = cn6_pcak(cmd,0,0x01,content,contentlen,pOutBuf,buflen);
	}

	hd_zy(1,ptr,rlen);

	return rlen;
}

unsigned int sjz_plat_unpack(void *pdat,unsigned int len,
			void (*pHandler)(unsigned char cmd,void *vin,unsigned char ver,unsigned char entype,void *content,unsigned int contentlen),
			void (*pErrorHandler)(void *,unsigned int))
{
	unsigned int slen = len;
	unsigned char *pTemp = 0;
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned int cnt = 0;
	unsigned int onepacklen = 0;
	unsigned char cmd = 0;
	unsigned char *vin = 0;
	unsigned char ver = 0;
	unsigned char entype = 0;
	unsigned char *content = 0;
	unsigned int contentlen = 0;

	if (pdat == 0) return 0;

	while (1)
	{
		unsigned int skipped = 0;
		unsigned char calc = 0;

		if (slen < CN6_MIN_LEN)
		{
			if (pErrorHandler) pErrorHandler(ptr,slen);
			break;
		}

		pTemp = hd_find_sub_str(ptr,slen,"\x23\x23",2);
		if (pTemp == 0)
		{
			if (pErrorHandler && slen) pErrorHandler(ptr,slen);
			break;
		}

		skipped = (unsigned int)(pTemp - ptr);
		if (skipped > slen)
		{
			break;
		}
		slen -= skipped;
		ptr = pTemp;
		if (slen < CN6_MIN_LEN)
		{
			if (pErrorHandler) pErrorHandler(ptr,slen);
			break;
		}

		contentlen = HD_GET_CONTENT_LEN(ptr);
		onepacklen = (unsigned int)CN6_MIN_LEN + contentlen;
		if ((onepacklen < contentlen) || (slen < onepacklen))
		{
			if (pErrorHandler) pErrorHandler(ptr,slen);
			break;
		}

		hd_zy(0,ptr,onepacklen);
		calc = CommSumCheckCode(ptr + CN6_CMD_OFFSET,onepacklen - CN6_HEAD_LEN - CN6_CHECK_LEN);
		if (calc == ptr[onepacklen - 1])
		{
			cmd = HD_GET_CMD(ptr);
			vin = HD_GET_VIN_PTR(ptr);
			ver = HD_GET_VER(ptr);
			entype = HD_GET_ENTYPE(ptr);
			content = HD_GET_CONTENT_PTR(ptr);
			contentlen = HD_GET_CONTENT_LEN(ptr);
			cnt++;
			if (pHandler) pHandler(cmd,vin,ver,entype,content,contentlen);
			ptr += onepacklen;
			slen -= onepacklen;
		}
		else if (slen > 2)
		{
			ptr += 2;
			slen -= 2;
		}
		else
		{
			break;
		}
	}

	return cnt;
}




unsigned int sjz_plat_login_pack(unsigned char rtime[6],
							void *iccid,unsigned int iccid_len,
							void *imei,unsigned int imei_len,
							void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *content = 0;

	content = get_small_mem();
	if (content == 0)
	{
		free_small_mem(content);
		return 0;
	}

	//时间
	content[rlen++] = rtime[0];
	content[rlen++] = rtime[1];
	content[rlen++] = rtime[2];
	content[rlen++] = rtime[3];
	content[rlen++] = rtime[4];
	content[rlen++] = rtime[5];

	//登录流水
	s_stSJZ_PLATSYSPARAM.login_flow++;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.login_flow >> 8;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.login_flow;

	//ICCID
	memcpy(content + rlen,iccid,iccid_len);
	rlen += iccid_len;

	//VIN
	memcpy(content + rlen,s_stSJZ_PLATSYSPARAM.vin,s_stSJZ_PLATSYSPARAM.vin_len);
	rlen += s_stSJZ_PLATSYSPARAM.vin_len;

	//IMEI
	memcpy(content + rlen,imei,imei_len);
	rlen += imei_len;


	slen = sjz_plat_pack(HD_PLAT_CMD_LOGIN,content,rlen,pOut,outbuflen);
	free_small_mem(content);

	return slen;
}


void sjz_plat_init(void *vin,unsigned int vin_len,void *chipid,unsigned int chipid_len,
					void *pubkey,unsigned int pubkey_len)
{
	if (vin_len > sizeof(s_stSJZ_PLATSYSPARAM.vin)) vin_len = sizeof(s_stSJZ_PLATSYSPARAM.vin);
	if (chipid_len > sizeof(s_stSJZ_PLATSYSPARAM.chipid)) chipid_len = sizeof(s_stSJZ_PLATSYSPARAM.chipid);
	if (pubkey_len > sizeof(s_stSJZ_PLATSYSPARAM.pubkey)) pubkey_len = sizeof(s_stSJZ_PLATSYSPARAM.pubkey);

	s_stSJZ_PLATSYSPARAM.vin_len = vin_len;
	s_stSJZ_PLATSYSPARAM.chipid_len = chipid_len;
	s_stSJZ_PLATSYSPARAM.pubkey_len = pubkey_len;

	if (vin && vin_len) memcpy(s_stSJZ_PLATSYSPARAM.vin,vin,vin_len);
	if (chipid && chipid_len) memcpy(s_stSJZ_PLATSYSPARAM.chipid,chipid,chipid_len);
	if (pubkey && pubkey_len) memcpy(s_stSJZ_PLATSYSPARAM.pubkey,pubkey,pubkey_len);
}

//定位状态 pos_status: 0 已定位 1 未定位
unsigned int sjz_plat_sos_pack(unsigned char rtime[6],unsigned char pos_status,INT32U lon,INT32U lat,void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *content = 0;

	content = get_small_mem();
	if (content == 0)
	{
		free_small_mem(content);
		return 0;
	}

	//时间
	content[rlen++] = rtime[0];
	content[rlen++] = rtime[1];
	content[rlen++] = rtime[2];
	content[rlen++] = rtime[3];
	content[rlen++] = rtime[4];
	content[rlen++] = rtime[5];

	//实时发送流水号
	s_stSJZ_PLATSYSPARAM.real_flow++;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.real_flow >> 8;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.real_flow;

	content[rlen++] = pos_status;

	content[rlen++] = lon >> 24;
	content[rlen++] = lon >> 16;
	content[rlen++] = lon >> 8;
	content[rlen++] = lon;


	content[rlen++] = lat >> 24;
	content[rlen++] = lat >> 16;
	content[rlen++] = lat >> 8;
	content[rlen++] = lat;

	slen = sjz_plat_pack(HD_PLAT_CMD_SOS,content,rlen,pOut,outbuflen);

	free_small_mem(content);

	return slen;
}


unsigned int sjz_plat_beian_pack(unsigned char rtime[6],void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *content = 0;
	int templen = 0;
	unsigned char *pSign = 0;

	pSign = get_small_mem();
	//content = get_small_mem();
	content = pSign + 128;

	if (pSign == 0)
	{
		free_small_mem(pSign);
		//free_small_mem(content);
		return 0;
	}

	//时间
	content[rlen++] = rtime[0];
	content[rlen++] = rtime[1];
	content[rlen++] = rtime[2];
	content[rlen++] = rtime[3];
	content[rlen++] = rtime[4];
	content[rlen++] = rtime[5];

	//芯片ID
	memcpy(content + rlen,s_stSJZ_PLATSYSPARAM.chipid,s_stSJZ_PLATSYSPARAM.chipid_len);
	rlen += s_stSJZ_PLATSYSPARAM.chipid_len;

	//公钥
	memcpy(content + rlen,s_stSJZ_PLATSYSPARAM.pubkey,s_stSJZ_PLATSYSPARAM.pubkey_len);
	rlen += s_stSJZ_PLATSYSPARAM.pubkey_len;

	//VIN
	memcpy(content + rlen,s_stSJZ_PLATSYSPARAM.vin,s_stSJZ_PLATSYSPARAM.vin_len);
	rlen += s_stSJZ_PLATSYSPARAM.vin_len;

	//签名信息带Z值
#if 1
	templen = sm2_chip_read_sign_data_with_z("1234567812345678",16,
	s_stSJZ_PLATSYSPARAM.pubkey,s_stSJZ_PLATSYSPARAM.pubkey_len,
	content + 6,
	s_stSJZ_PLATSYSPARAM.pubkey_len + s_stSJZ_PLATSYSPARAM.chipid_len + s_stSJZ_PLATSYSPARAM.vin_len,
	pSign,get_small_mem_size());
#endif


	if (templen <= 0)
	{
		free_small_mem(pSign);
		//free_small_mem(content);
		DBG_Print(0,"%s","备案签名失败\r\n");
		return 0;
	}
	DBG_Print(LEVEL2,"sign with z(%d):\r\n",templen);
	DBG_PrintBuf(LEVEL2,0,pSign,templen);

	content[rlen++] = 32;
	memcpy(content+rlen,pSign,32);
	rlen += 32;
	content[rlen++] = 32;
	memcpy(content+rlen,pSign+32,32);
	rlen += 32;

	slen = sjz_plat_pack(HD_PLAT_CMD_BEIAN,content,rlen,pOut,outbuflen);

	free_small_mem(pSign);
	//free_small_mem(content);

	return slen;
}


unsigned int _sjz_plat_stream_report(unsigned char cmd,unsigned char send_time[6],
		SJZStream *pSJZStream,unsigned int cnt,
		void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned int xlen = 0;
	unsigned char *content = 0;
	int templen = 0;
	int ret = 0;
	SJZStream *ps = 0;
	unsigned int i = 0;
	unsigned char *ppt = 0;
	unsigned char *pSign = 0;
	unsigned char *mw = 0;

	pSign = get_small_mem();
	mw = get_big_mem();
	content = get_big_mem();
	if ((content == 0) || (pSign == 0) || (mw == 0))
	{	
		free_big_mem(mw);
		free_big_mem(content);
		free_small_mem(pSign);
		return 0;
	}

	//发送时间
	content[rlen++] = send_time[0];
	content[rlen++] = send_time[1];
	content[rlen++] = send_time[2];
	content[rlen++] = send_time[3];
	content[rlen++] = send_time[4];
	content[rlen++] = send_time[5];

	//实时发送流水号
	s_stSJZ_PLATSYSPARAM.real_flow++;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.real_flow >> 8;
	content[rlen++] = s_stSJZ_PLATSYSPARAM.real_flow;

	for (i = 0; i < cnt; i++)
	{
		ps = pSJZStream + i;

		/*-------信息1-------*/
#if 1
		//信息标志类型
		content[rlen++] = HD_PLAT_INFO_TYPE_OBD;
		
		//采集时间
		content[rlen++] = ps->time[0];
		content[rlen++] = ps->time[1];
		content[rlen++] = ps->time[2];
		content[rlen++] = ps->time[3];
		content[rlen++] = ps->time[4];
		content[rlen++] = ps->time[5];

		content[rlen++] = ps->obd_pro;
		content[rlen++] = ps->mil_status;

		content[rlen++] = ps->support_status >> 8;
		content[rlen++] = ps->support_status;

		//content[rlen++] = ps->support_status >> 8;
		//content[rlen++] = ps->support_status;
		

		//if (ps->pvin) memcpy(content + rlen,ps->pvin,17);
		memcpy(content + rlen,GetVIN(),17);
		rlen += 17;


		//calid
		//if (ps->pcalid) memcpy(content+rlen,ps->pcalid,18);
		memset(content+rlen,'0',18);
		rlen += 18;

		//cvn
		//if (ps->pcvn) memcpy(content+rlen,ps->pcvn,18);
		memset(content+rlen,'0',18);
		rlen += 18;

		//iupr
		//if (ps->piupr) memcpy(content+rlen,ps->piupr,36);
		memset(content+rlen,'0',36);
		rlen += 36;

		content[rlen++] = ps->dtc;

		if (ps->dtc && ps->gzm_list && (ps->dtc < 0xFE))
		{
			unsigned int k = 0;
			for (k = 0; k < ps->dtc; k++)
			{
				content[rlen++] = *(ps->gzm_list + k*4 + 0);
				content[rlen++] = *(ps->gzm_list + k*4 + 1);
				content[rlen++] = *(ps->gzm_list + k*4 + 2);
				content[rlen++] = *(ps->gzm_list + k*4 + 3);
			}
		}

#endif

#if 1
		//信息标志类型
		content[rlen++] = HD_PLAT_INFO_TYPE_DATA_STREAM;
		
		//采集时间
		content[rlen++] = ps->time[0];
		content[rlen++] = ps->time[1];
		content[rlen++] = ps->time[2];
		content[rlen++] = ps->time[3];
		content[rlen++] = ps->time[4];
		content[rlen++] = ps->time[5];
		
		/*-----------信息体---------------*/
		content[rlen++] = ps->speed >> 8;						//1.车速
		content[rlen++] = ps->speed;
		
		content[rlen++] = ps->kpa;	//ObdGetPressureKpa();				//2.大气压
		content[rlen++] = ps->cActualTorquePercent;//ObdGetEngineTorquePercent();		//3.净输出扭矩
		content[rlen++] = ps->cFrictionTorquePercent;	//ObdGetFrictionTorquePercent();	//4.摩擦扭矩
		
		content[rlen++] = ps->rpm >> 8; 						//5.转速
		content[rlen++] = ps->rpm;
		content[rlen++] = ps->fuelflow >> 8;					//6.燃料流量
		content[rlen++] = ps->fuelflow;
		
		content[rlen++] = ps->up_Nox_out >> 8;						//7.上Nox输出
		content[rlen++] = ps->up_Nox_out; 		


		content[rlen++] = ps->down_Nox_out >> 8;						//8.下Nox输出
		content[rlen++] = ps->down_Nox_out; 		

		content[rlen++] = ps->ucRemain;	//ObdGetReagentRemain();				//9.反应剂余量
		content[rlen++] = ps->usIntakeVolume >> 8;//ObdGetAirIntakeVolume() >> 8; 			//11.进气量
		content[rlen++] = ps->usIntakeVolume;	//ObdGetAirIntakeVolume();
		
		
		content[rlen++] = ps->sSCRInletTemperature >> 8;//scr_down >> 8;				//8.SCR入口温度
		content[rlen++] = ps->sSCRInletTemperature;//scr_down;
		
		content[rlen++] = ps->sSCROutletTemperature >> 8;//scr_down >> 8;				//9.SCR出口温度
		content[rlen++] = ps->sSCROutletTemperature;//scr_down;
		
		content[rlen++] = ps->dpf >> 8;//ObdGetDPFDifferentialPressure() >> 8;		//10.DPF压差
		content[rlen++] = ps->dpf;  //ObdGetDPFDifferentialPressure();
		
		content[rlen++] = ps->cCoolantTemperature;	//ObdGetEngineCoolantTemperature();				//14.发动机冷却液温度
		
		content[rlen++] = ps->oil_level;	//ObdGetFuelTankLevel();				//13.邮箱液位
		
		
		
		content[rlen++] = ps->gps_status;									//15.定位状态
		
		content[rlen++] = ps->lon >> 24;							//16.经度
		content[rlen++] = ps->lon >> 16;
		content[rlen++] = ps->lon >> 8;
		content[rlen++] = ps->lon;
		
		content[rlen++] = ps->lat >> 24;							//17.纬度
		content[rlen++] = ps->lat >> 16;
		content[rlen++] = ps->lat >> 8;
		content[rlen++] = ps->lat;
		
		content[rlen++] = ps->milage >> 24;							//18.里程
		content[rlen++] = ps->milage >> 16;
		content[rlen++] = ps->milage >> 8;
		content[rlen++] = ps->milage;
		
		content[rlen++] = ps->guang_xishou_xishu >> 8;						//19
		content[rlen++] = ps->guang_xishou_xishu;
		
		content[rlen++] = ps->bu_tou_guang_du >> 8;						//
		content[rlen++] = ps->bu_tou_guang_du;
		
		content[rlen++] = ps->klw_nd >> 8;						//
		content[rlen++] = ps->klw_nd;

		content[rlen++] = ps->mil_status;		//20.mil状态
#endif

#if 1
		//信息标志类型
		content[rlen++] = HD_PLAT_INFO_TYPE_NO_ROAD;
		
		//采集时间
		content[rlen++] = ps->time[0];
		content[rlen++] = ps->time[1];
		content[rlen++] = ps->time[2];
		content[rlen++] = ps->time[3];
		content[rlen++] = ps->time[4];
		content[rlen++] = ps->time[5];

		//发动机状态
		content[rlen++] = ps->eng_status;


		content[rlen++] = ps->speed >> 8;						//1.车速
		content[rlen++] = ps->speed;


		content[rlen++] = ps->up_Nox_out >> 8;						//7.上Nox输出
		content[rlen++] = ps->up_Nox_out;
		
	
		content[rlen++] = ps->gps_status;									//15.定位状态
		
		content[rlen++] = ps->lon >> 24;							//16.经度
		content[rlen++] = ps->lon >> 16;
		content[rlen++] = ps->lon >> 8;
		content[rlen++] = ps->lon;
		
		content[rlen++] = ps->lat >> 24;							//17.纬度
		content[rlen++] = ps->lat >> 16;
		content[rlen++] = ps->lat >> 8;
		content[rlen++] = ps->lat;

		content[rlen++] = ps->guang_xishou_xishu >> 8;						//19
		content[rlen++] = ps->guang_xishou_xishu;
		
		content[rlen++] = ps->bu_tou_guang_du >> 8; 					//
		content[rlen++] = ps->bu_tou_guang_du;
		
		content[rlen++] = ps->klw_nd >> 8;						//
		content[rlen++] = ps->klw_nd;

		content[rlen++] = ps->drive_cycle_cnt >> 8;						//
		content[rlen++] = ps->drive_cycle_cnt;
#endif
	}

	DBG_Print(0,"消息体明文(%d):\r\n",rlen);
	DBG_PrintBuf(0,0,content,rlen);

	//对信息1~信息N进行签名
#if 1
	ret = sm2_chip_read_sign_data_with_z("1234567812345678",16,
	s_stSJZ_PLATSYSPARAM.pubkey,s_stSJZ_PLATSYSPARAM.pubkey_len,
	content + 8,rlen - 8,pSign,get_small_mem_size()
	);
#endif


	if (ret <= 0)
	{	
		free_big_mem(mw);
		free_big_mem(content);
		free_small_mem(pSign);

		DBG_Print(0,"信息签名失败.\r\n");
		return 0;
	}
	DBG_PrintBuf(LEVEL2,"数据流签名:\r\n",pSign,64);


	ppt = content + rlen;

	//签名
	ppt[xlen++] = 32;
	memcpy(ppt+xlen,pSign,32);
	xlen += 32;

	ppt[xlen++] = 32;
	memcpy(ppt+xlen,pSign+32,32);
	xlen += 32;

	xlen += (rlen - 8);

	slen = sjz_plat_pack(cmd,content,xlen + 8,pOut,outbuflen);

	free_big_mem(mw);
	free_big_mem(content);
	free_small_mem(pSign);

	return slen;
}

//实时多包
unsigned int sjz_plat_real_stream_report_multi_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,unsigned int cnt,
		void *pOut,unsigned int outbuflen)
{
	return _sjz_plat_stream_report(HD_PLAT_CMD_REAL_DATA,send_time,pSJZStream,cnt,pOut,outbuflen);
}

//实时一包
unsigned int sjz_plat_real_stream_report_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,void *pOut,unsigned int outbuflen)
{
	return _sjz_plat_stream_report(HD_PLAT_CMD_REAL_DATA,send_time,pSJZStream,1,pOut,outbuflen);
}


//补传多包
unsigned int sjz_plat_supply_stream_report_multi_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,unsigned int cnt,
		void *pOut,unsigned int outbuflen)
{
	return _sjz_plat_stream_report(HD_PLAT_CMD_BF_DATA,send_time,pSJZStream,cnt,pOut,outbuflen);
}

//补传一包
unsigned int sjz_plat_supply_stream_report_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,void *pOut,unsigned int outbuflen)
{
	return _sjz_plat_stream_report(HD_PLAT_CMD_BF_DATA,send_time,pSJZStream,1,pOut,outbuflen);
}

//请求校时
unsigned int sjz_plat_dev_request_js_pack(void *pOut,unsigned int outbuflen)
{
	unsigned char content[2] = {0,1};

	return sjz_plat_pack(HD_PLAT_CMD_REQUEST_JS,content,2,pOut,outbuflen);	
}


void sjz_plat_analyze(unsigned char cmd,void *vin,unsigned char ver,unsigned char entype,void *content,unsigned int contentlen)
{
	unsigned char *ptr = (unsigned char *)content;
	
	switch (cmd)
	{
	//备案应答
	case HD_PLAT_CMD_ACK_BEIAN:
	DBG_Print(0,"-----BEIAN ack--------\r\nStatus:%d info:%d\r\n--------------------\r\n",ptr[0],ptr[1]);
	sjz_set_beian_flag(1);
	if ((ptr[0] == 0x01) && (ptr[1] == 0x00))
	{
		DBG_Print(LEVEL0,"备案成功\r\n");
		beian_success();
	}
	else
	{
		DBG_Print(LEVEL0,"备案失败\r\n");
	}
	break;

	//登录应答
	case HD_PLAT_CMD_ACK_LOGIN:
	
	DBG_Print(0,"------login ack--------\r\nflow:%u ack:%d\r\n",
	(ptr[0] << 8) + ptr[1],ptr[2]);
	if (ptr[2] == 0x01)		//1表示登录成功
	{
		sjz_set_login_flag(1);
	}
	break;
	case HD_PLAT_CMD_ACK_DATA:

	DBG_Print(0,"-------DATA ACK-------\r\nack:%02X ack_cmd:%02X flow:%u\r\n",
	ptr[0],ptr[1],(ptr[2] << 8) + ptr[3]);

	break;
	default:break;
	}
}


