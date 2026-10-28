#include "usercmd.h"


typedef int (*SystemCmdFunc)(void *content,unsigned int contentlen,void *outbuf,unsigned int outbuflen);


/* 用户指令结构体 */
typedef struct tagSystemCommand
{
	void *pCmd;				//指令类型
	SystemCmdFunc pFunc;	//执行相应的函数
	char *pTip;
}SystemCommand;

int Cmd_Help(void *pArg,INT32U len,void *pOut,INT32U outlen);


int Cmd_GetSoftwareVersion(void *content,unsigned int contentlen,void *pOut,unsigned int outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0,"Software Version:%s Compile @ %s %s\r\n",APP_VERSION,__TIME__,__DATE__);
	if (pOut)
	{
		rlen = snprintf(pOut,outlen,"Software Version:%s Compile @ %s %s\r\n",APP_VERSION,__TIME__,__DATE__);
	}

	return rlen;
}

/**************************************************************************
* 函  数: int Cmd_DefaultDevice(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 恢复出厂设置
* 举  例: ATRST
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*	      INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
int Cmd_DefaultDevice(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0, "Factory Device Start...\r\n");
	//empty_acce_store_block();
	UserParamDefault();
	DBG_Print(LEVEL0,"Factory Device Finish.\r\n");
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen, "Factory Device Finish");
	}

	return rlen;
}




int Cmd_Test(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return 0;
}

int Cmd_SetApn(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;


	SPSetApn((char *)pArg);
	if (pOut)
	{
		rlen = snprintf((char *)pOut, outlen, "Set APN:%s", (char *)pArg);
	}


	DBG_Print(LEVEL0,"set apn:%s\r\n",SPGetApn());
	return rlen;
}

int Cmd_GetApn(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;
	char *apn = 0;

	apn = SPGetApn();
	DBG_Print(LEVEL0,"Get APN:%s user:%s password:%s\r\n", apn,SPGetApnUsername(),SPGetApnPassword());
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Get APN:%s user:%s password:%s",
		apn,SPGetApnUsername(),SPGetApnPassword());
	}

	return rlen;
}

int Cmd_SetApnUser(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0,"Set APN User:%s\r\n",(char *)pArg);

	SPSetApnUsername((char *)pArg);
	if (pOut)
	{
		rlen = snprintf((char *)pOut, outlen, "Set APN User:%s", SPGetApnUsername());
	}

	return rlen;
}


int Cmd_SetApnPsw(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;

	DBG_Print(LEVEL0,"Set APN Password:%s\r\n",(char *)pArg);

	SPSetApnPassword((char *)pArg);
	if (pOut)
	{
		rlen = snprintf((char *)pOut, outlen, "Set APN Password:%s", SPGetApnPassword());
	}

	return rlen;
}


int Cmd_GetPlat(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return 0;
}

int Cmd_SetVin(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned rlen = 0;

	SetVIN((char *)pArg);
	send_msg(S_MSG_SEND_PARAM_TO_MPU,GetVIN(),(void *)17,0);
	DBG_Print(LEVEL0,"Set VIN:%s\r\n",(char *)pArg);
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Set VIN:%s\r\n",GetVIN());
	}
	return rlen;
}

int Cmd_GetVin(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;

	DBG_Print(0,"<VIN:%s><canvin:%s>\r\n",GetVIN(),can_get_vin_ptr(0));
	if (pOut && outlen)
	{
		rlen = snprintf(pOut,outlen,"<VIN:%s><canvin:%s>",GetVIN(),can_get_vin_ptr(0));
	}
	return rlen;
}


int Cmd_GetInfo(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;
	unsigned char *pt = get_dst_time_when_network_error();
	unsigned char *sc_date = get_sc_date();

	rlen = disp_sysparam(pOut,outlen);

	if (pOut && outlen)
	{
		rlen += snprintf((char *)pOut + rlen,outlen - rlen,
						"(df)bind_pro_lock:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X "
						"%u sos:%08X %u %02X flg:%d\r\n"
						"(dst)b_pro_lock:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X "
						"c_l_k:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X tip:%d_%d_%d where:%d,%d,%u,%u,%02d-%02d-%02d %02d:%02d:%02d %u d78:%u %u(0.01kwh)\r\n"
						"%u %u %u(%02u%02u%02u%02u%02u%02u)\r\n",
						get_fact_bind_status(),get_fact_pro_ver_about_lock(),get_fact_lock_status(),
						get_want_bind_status(),get_want_pro_ver_about_lock(),get_want_lock_status(),
						get_electric_car_class(),gb32960_can_get_sos_flag(),gb32960_can_get_max_sos_level(),get_test_step(),gb32960_can_get_sos_test(),

						get_dst_fact_bind_status(),get_dst_fact_pro_ver_about_lock(),get_dst_fact_lock_status(),0,get_dst_want_pro_ver_about_lock(),get_dst_want_lock_status(),

						get_dst_fact_bms_charge_flag(),get_dst_fact_limit_speed_level(),get_dst_fact_kucunrong_limit_speed(),
						get_dst_want_bms_charge_flag(),get_dst_want_limit_speed_level(),get_dst_want_kucunrong_limit_speed(),
						get_dst_keep_fit_tip_flag(),get_dst_pay_tip_flag(),get_dst_inspect_tip_flag(),get_lock_car_cmd_from_where(),
						get_dst_network_error_flag(),get_dst_network_error_tip_flag(),get_dst_milage_when_network_error(),pt[0],pt[1],pt[2],pt[3],pt[4],pt[5],get_dst_net_error_accumulate_time_tmp(),
						get_d78_huandian_ver(),_get_dst_charge_dl(),get_dst_chemen_switch(),get_dst_chesuo_switch(),get_dst_left_qian_chesuo_switch(),
						sc_date[0],sc_date[1],sc_date[2],sc_date[3],sc_date[4],sc_date[5]
						);

	}

	DBG_Print(LEVEL0,
					"(df)bind_pro_lock:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X "
					"%u sos:%08X %u %02X flg:%d\r\n"
					"(dst)b_pro_lock:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X "
					"c_l_k:(fact)%02X_%02X_%02X (want)%02X_%02X_%02X tip:%d_%d_%d where:%d,%d,%u,%02d-%02d-%02d %02d:%02d:%02d\r\n",
					get_fact_bind_status(),get_fact_pro_ver_about_lock(),get_fact_lock_status(),
					get_want_bind_status(),get_want_pro_ver_about_lock(),get_want_lock_status(),
					get_electric_car_class(),gb32960_can_get_sos_flag(),gb32960_can_get_max_sos_level(),get_test_step(),gb32960_can_get_sos_test(),
	
					get_dst_fact_bind_status(),get_dst_fact_pro_ver_about_lock(),get_dst_fact_lock_status(),0,get_dst_want_pro_ver_about_lock(),get_dst_want_lock_status(),
	
					get_dst_fact_bms_charge_flag(),get_dst_fact_limit_speed_level(),get_dst_fact_kucunrong_limit_speed(),
					get_dst_want_bms_charge_flag(),get_dst_want_limit_speed_level(),get_dst_want_kucunrong_limit_speed(),
					get_dst_keep_fit_tip_flag(),get_dst_pay_tip_flag(),get_dst_inspect_tip_flag(),get_lock_car_cmd_from_where(),
					get_dst_network_error_flag(),get_dst_milage_when_network_error(),pt[0],pt[1],pt[2],pt[3],pt[4],pt[5]
					);
	return rlen;
}

/**************************************************************************
* 函  数: int Cmd_GetSn(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 获取SN序列号
* 举  例: ATSN?
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*	      INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
int Cmd_GetSn(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	char *sn = 0;

	sn = GetSN();
	DBG_Print(LEVEL0, "Get <SN:%s>\r\n", sn);	
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Get SN:%s", sn);
	}
	return rlen;
}

/**************************************************************************
* 函  数: int Cmd_GetPubKey(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 获取加密芯片的公钥
* 举  例: ATPUBKEY?
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*		  INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
static int Cmd_GetPubKey(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	unsigned char key[66] = {0};

	//sm2_chip_read_public_key(key,sizeof(key));
	send_msg(S_MSG_GET_PUBLIC_KEY,key,(void *)66,0);

	DBG_PrintBuf(LEVEL0, "Get Pub Key:\r\n", key, 64);

	if (pOut)
	{
		rlen = ArrayToHexStr1(key,64,pOut,outlen);	
	}

	return rlen;	
}

/**************************************************************************
* 函  数: int Cmd_SetChipId(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 设置加密芯片的ID
* 举  例: ATCHIPID=xxxx //最长长度为16个字节
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*		  INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
static int Cmd_SetChipId(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	unsigned char *pdata = (unsigned char *)pArg;

	if (len <= 16)
	{
		SysSetChipId(pdata, len);

		DBG_PrintBuf(0, "Set ChipId Successful:\r\n", pdata, len);
		if (pOut)
		{
			rlen = snprintf((char *)pOut, outlen, "Set ChipId Successful");
		}
	}

	return rlen;
}

/**************************************************************************
* 函  数: int Cmd_GetChipId(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 获取加密芯片的ID
* 举  例: ATCHIPID?
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*		  INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
static int Cmd_GetChipId(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	unsigned char i = 0;
	char *pSms = (char *)pOut;
	unsigned char chipid[16] = {0};

	SysGetChipId(chipid);
	DBG_PrintBuf(0, "Get ChipId:\r\n", chipid, sizeof(chipid));
	if (pOut)
	{
		rlen += snprintf(pSms, outlen, "Get ChipId:");
		for (i = 0; i < 16; i++)
		{
			rlen += snprintf(&pSms[rlen], outlen - rlen, "%02x ", chipid[i]);
		}
	}

	return rlen;
}



/**************************************************************************
* 函  数: int Cmd_SetSn(void *pArg,INT32U len,void *pOut,INT32U outlen)
* 描  述: 设置SN序列号
* 举  例: ATSN=XXXXXXXXXXXXXXXXXXXXXX
* 入  参: void *pArg: 指令的参数
*		  INT32U len: 参数的长度
*	      INT32U outlen：输出信息的最大长度
* 出  参: void *pOut：输出信息
* 返回值: int:输出信息的实际长度
**************************************************************************/
int Cmd_SetSn(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0,"Set SN:%s\r\n", (char *)pArg);
	SetSN((char *)pArg);
	if (pOut)
	{
		rlen = snprintf((char *)pOut, outlen, "Set SN:%s", (char *)pArg);
	}

	return rlen;
}
int Cmd_SetCan1Bps(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	int bps = atoi((char *)pArg);

	DBG_Print(LEVEL0,"Set CAN1 Bps:%u\r\n", bps);

	SetCan0Bps(bps);
	can_init(0,bps);
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Set CAN1 Bps:%u", bps);
	}

	return rlen;
}

int Cmd_GetCan1Bps(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0,"Get CAN1 Bps:%u %u %u\r\n", GetCan0Bps(),GetCan1Bps(),GetCan2Bps());
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Get CAN1 Bps:%u %u %u", GetCan0Bps(),GetCan1Bps(),GetCan2Bps());
	}

	return rlen;
}

int Cmd_SetCan2Bps(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	int bps = atoi((char *)pArg);

	DBG_Print(LEVEL0,"Set CAN2 Bps:%u\r\n", bps);

	SetCan1Bps(bps);
	can_init(1,bps);
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Set CAN2 Bps:%u", bps);
	}

	return rlen;
}

int Cmd_GetCan2Bps(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;

	DBG_Print(LEVEL0,"Get CAN2 Bps:%u\r\n", GetCan1Bps());
	if (pOut)
	{
		rlen = snprintf((char *)pOut,outlen,"Get CAN2 Bps:%u", GetCan1Bps());
	}

	return rlen;
}


int set_ip(char idx,void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;

	SPSetServerAddr(idx,pArg);
	//connect_server(idx,SPGetServerAddr(idx),SPGetServerPort(idx),ServerLinkIsTcp(idx));

	if (idx == 0)
	{	
		DBG_Print(LEVEL0,"<FIRSTIP:%s>\r\n",SPGetServerAddr(0));
		if (pOut) rlen = snprintf((char *)pOut,outlen,"FIRSTIP:%s",SPGetServerAddr(0));
	}
	else if (idx == 1)
	{
		DBG_Print(LEVEL0,"<SECONDIP:%s>\r\n",SPGetServerAddr(1));
		if (pOut) rlen = snprintf((char *)pOut,outlen,"SECONDIP:%s",SPGetServerAddr(1));
	}
	else if (idx == 2)
	{
		DBG_Print(LEVEL0,"<THIRDIP:%s>\r\n",SPGetServerAddr(2));
		if (pOut) rlen = snprintf((char *)pOut,outlen,"THIRDIP:%s",SPGetServerAddr(2));
	}
	return rlen;
}

int set_port(char idx,void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	unsigned int rlen = 0;
	unsigned int port = atoi((char *)pArg);

	SPSetServerPort(idx,port);
	//connect_server(idx,SPGetServerAddr(idx),SPGetServerPort(idx),ServerLinkIsTcp(idx));

	if (idx == 0)
	{
		DBG_Print(LEVEL0,"<FIRSTPORT:%d>\r\n",port);
		if (pOut) rlen = snprintf((char *)pOut,outlen,"FIRSTPORT:%d",port);
	}
	else if (idx == 1)
	{
		DBG_Print(LEVEL0,"<SECONDPORT:%d>\r\n",port);
		if (pOut) rlen = snprintf((char *)pOut,outlen,"SECONDPORT:%d",port);
	}
	else if (idx == 2)
	{
		DBG_Print(LEVEL0,"<THIRDPORT:%d>\r\n",port);
		if (pOut) rlen = snprintf((char *)pOut,outlen,"THIRDPORT:%d",port);
	}
	return rlen;
}


int Cmd_SetFirstIP(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_ip(0,pArg,len,pOut,outlen);
}

int Cmd_SetSecondIP(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_ip(1,pArg,len,pOut,outlen);
}

int Cmd_SetThirdIP(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_ip(2,pArg,len,pOut,outlen);
}


int Cmd_SetFirstPort(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_port(0,pArg,len,pOut,outlen);
}

int Cmd_SetSecondPort(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_port(1,pArg,len,pOut,outlen);
}

int Cmd_SetThirdPort(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return set_port(2,pArg,len,pOut,outlen);
}

char *find_str_tail(void *ptr,void *tip)
{
	char *pRet = 0;
	if ((ptr == 0) || (tip == 0) || (strlen((char *)tip) == 0))
	{
		return 0;
	}

	pRet = strstr((char *)ptr,(char *)tip);
	if (pRet)
	{
		return (pRet + strlen((char *)tip));
	}
	return 0;
}

unsigned long real_idx = 0;
unsigned long mq_idx = 0;
unsigned long hb_idx = 0;
unsigned long loginout_idx = 0;

INT8U s_start_time[6] = {0};
INT8U s_end_time[6] = {0};

void export_print(unsigned long index,INT8U year,INT8U month,INT8U day,
											INT8U hour,INT8U min,INT8U sec,
											void *pbuf,unsigned int len)

{
}

void _export_print(unsigned long index,INT8U year,INT8U month,INT8U day,
											INT8U hour,INT8U min,INT8U sec,
											INT8U car_status,INT8U charge_status,INT16U speed,
											void *pbuf,unsigned int len)
{
}


void real_data_handler(unsigned long index,void * buf,unsigned int buflen)
{
	INT8U *pbuf = (INT8U *)buf;
	INT16U templen = 0;
	INT8U desTime[6] = {0};

	templen = (pbuf[0] << 8) + pbuf[1];
	if (templen < 1450)
	{
		desTime[0] = *(pbuf+3+24);
		desTime[1] = *(pbuf+3+25);
		desTime[2] = *(pbuf+3+26);
		desTime[3] = *(pbuf+3+27);
		desTime[4] = *(pbuf+3+28);
		desTime[5] = *(pbuf+3+29);
		if (is_between_time(desTime,s_start_time,s_end_time))
		{
			_export_print(real_idx,*(pbuf+3+24),*(pbuf+3+25),*(pbuf+3+26),
								*(pbuf+3+27),*(pbuf+3+28),*(pbuf+3+29),
								pbuf[3+31],pbuf[3+32],byte_to_int16u(pbuf[3+34],pbuf[3+35]),
						pbuf + 3,templen);
			real_idx++;
		}
	}
}

void mq_data_handler(unsigned long index,void * buf,unsigned int buflen)
{
	INT8U *pbuf = (INT8U *)buf;
	INT16U templen = 0;
	INT8U desTime[6] = {0};

	templen = (pbuf[0] << 8) + pbuf[1];
	if (templen < 1450)
	{
		desTime[0] = *(pbuf+3+24);
		desTime[1] = *(pbuf+3+25);
		desTime[2] = *(pbuf+3+26);
		desTime[3] = *(pbuf+3+27);
		desTime[4] = *(pbuf+3+28);
		desTime[5] = *(pbuf+3+29);
		if (is_between_time(desTime,s_start_time,s_end_time))
		{
			_export_print(mq_idx,*(pbuf+3+24),*(pbuf+3+25),*(pbuf+3+26),
								*(pbuf+3+27),*(pbuf+3+28),*(pbuf+3+29),
								pbuf[3+31],pbuf[3+32],byte_to_int16u(pbuf[3+34],pbuf[3+35]),
						pbuf + 3,templen);
			mq_idx++;
		}

	}
}

void heart_record_handler(unsigned long index,void * buf,unsigned int buflen)
{
	INT8U *pbuf = (INT8U *)buf;
	INT16U templen = 0;
	INT8U desTime[6] = {0};

	templen = (pbuf[0] << 8) + pbuf[1];
	if (templen < 250)
	{
		desTime[0] = *(pbuf+3+templen);
		desTime[1] = *(pbuf+3+templen + 1);
		desTime[2] = *(pbuf+3+templen + 2);
		desTime[3] = *(pbuf+3+templen + 3);
		desTime[4] = *(pbuf+3+templen + 4);
		desTime[5] = *(pbuf+3+templen + 5);
		if (is_between_time(desTime,s_start_time,s_end_time))
		{
			export_print(hb_idx,*(pbuf+3+templen),*(pbuf+3+templen+1),*(pbuf+3+templen + 2),
								*(pbuf+3+templen+3),*(pbuf+3+templen+4),*(pbuf+3+templen+5),
						pbuf + 3,templen);
			hb_idx++;

		}
	}
}

void loginout_record_handler(unsigned long index,void * buf,unsigned int buflen)
{
	INT8U *pbuf = (INT8U *)buf;
	INT16U templen = 0;
	INT8U desTime[6] = {0};

	templen = (pbuf[0] << 8) + pbuf[1];
	if (templen < 256)
	{
		desTime[0] = *(pbuf+3+24);
		desTime[1] = *(pbuf+3+25);
		desTime[2] = *(pbuf+3+26);
		desTime[3] = *(pbuf+3+27);
		desTime[4] = *(pbuf+3+28);
		desTime[5] = *(pbuf+3+29);
		if (is_between_time(desTime,s_start_time,s_end_time))
		{
			export_print(loginout_idx,*(pbuf+3+24),*(pbuf+3+25),*(pbuf+3+26),
								*(pbuf+3+27),*(pbuf+3+28),*(pbuf+3+29),
						pbuf + 3,templen);
			loginout_idx++;
		}
	}
}

//按时间段导出数据
void export_record_by_time(INT8U start_time[6],INT8U end_time[6])
{

}


int Cmd_Expand(void *pArg,unsigned int len,void *pOut,unsigned int outlen)
{
	char *ptr = (char *)pArg;
	char *ptemp = 0;
	//unsigned int a = 0;
	//unsigned int b = 0;
	char szTemp[256 + 1] = {0};
	char *pTemp = 0;
	char *pOutBuf = (char *)pOut;
	int ack_len = 0;
	unsigned int templen = 0;

	pOutBuf = pOutBuf;
	templen = templen;

	if (strstr(ptr,"recv_send_can:"))
	{
		if (strstr(ptr,"recv_send_can:1"))
		{
			set_recv_send_can_flag(1);
		}
		
		if (strstr(ptr,"recv_send_can:0"))
		{
			set_recv_send_can_flag(0);
		}
		DBG_Print(LEVEL2,"recv send can:%u\r\n",get_recv_send_can_flag());
		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"recv send can:%u\r\n",get_recv_send_can_flag());
	}

	if (strstr(ptr,"recvcan"))
	{
		CanTxMsg txmsg = {0};
		INT8U hex[4] = {0};
		
		//CAN ID
		GetSubString(ptr,":"," ",szTemp,16);
		if (strlen(szTemp) < 8)
		{
			//补0
			for (templen = 0; templen < (8 - strlen(szTemp)); templen++)
			{
				*(szTemp + 16 + templen) = '0';
			}
			memcpy(szTemp + 16 + templen,szTemp,strlen(szTemp));
		}
		else
		{
			memcpy(szTemp + 16,szTemp,8);
		}
		HexStrToArray((INT8U *)szTemp + 16,hex,4);
		templen = (hex[0] << 24) + (hex[1] << 16) + (hex[2] << 8) + hex[3];
		
		//CAN DATA
		ptemp = strstr((char *)pArg," ");
		if (ptemp)
		{
			HexStrToArray((INT8U *)ptemp + 1,(INT8U *)szTemp,32);
			memcpy(txmsg.Data,szTemp,8);
		}
		
		txmsg.DLC = 8;
		if (templen > 0x7FF)
		{
			txmsg.ExtId = templen;
			txmsg.IDE = CAN_ID_EXT;
		}
		else
		{
			txmsg.StdId = templen;
			txmsg.IDE = CAN_ID_STD;
		}
		
		txmsg.RTR = CAN_RTR_DATA;
		
		GetSubString(ptr,"recvcan",":",szTemp,16);
		//can_send_data(atoi(szTemp),&txmsg);

		can_data_in_queue(atoi(szTemp),(templen > 0x7FF) ? 1 : 0,templen,txmsg.Data,8);

		DBG_Print(LEVEL4,"[CAN%u]RX:%X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",atoi(szTemp),
						templen,txmsg.Data[0],txmsg.Data[1],txmsg.Data[2],txmsg.Data[3],
						txmsg.Data[4],txmsg.Data[5],txmsg.Data[6],txmsg.Data[7]);
	}
	//手动发送CAN数据 
	//ATEX=sendcan0:7DF 00 01 02 03 04 05 06 07
	//ATEX=sendcan0:18FE5201 00 01 02 03 04 05 06 07
	if (strstr(ptr,"sendcan"))
	{
		CanTxMsg txmsg = {0};
		INT8U hex[4] = {0};

		//CAN ID
		GetSubString(ptr,":"," ",szTemp,16);
		if (strlen(szTemp) < 8)
		{
			//补0
			for (templen = 0; templen < (8 - strlen(szTemp)); templen++)
			{
				*(szTemp + 16 + templen) = '0';
			}
			memcpy(szTemp + 16 + templen,szTemp,strlen(szTemp));
		}
		else
		{
			memcpy(szTemp + 16,szTemp,8);
		}
		HexStrToArray((INT8U *)szTemp + 16,hex,4);
		templen = (hex[0] << 24) + (hex[1] << 16) + (hex[2] << 8) + hex[3];

		//CAN DATA
		ptemp = strstr((char *)pArg," ");
		if (ptemp)
		{
			HexStrToArray((INT8U *)ptemp + 1,(INT8U *)szTemp,32);
			memcpy(txmsg.Data,szTemp,8);
		}

		txmsg.DLC = 8;
		if (templen > 0x7FF)
		{
			txmsg.ExtId = templen;
			txmsg.IDE = CAN_ID_EXT;
		}
		else
		{
			txmsg.StdId = templen;
			txmsg.IDE = CAN_ID_STD;
		}

		txmsg.RTR = CAN_RTR_DATA;

		GetSubString(ptr,"sendcan",":",szTemp,16);
		can_send_data(atoi(szTemp),&txmsg);

		DBG_Print(LEVEL2,"[CAN%u]TX:%X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",atoi(szTemp),
						templen,txmsg.Data[0],txmsg.Data[1],txmsg.Data[2],txmsg.Data[3],
						txmsg.Data[4],txmsg.Data[5],txmsg.Data[6],txmsg.Data[7]);
	}
#if 1
	if (strstr((char *)pArg,"test_sd_mcu_ota"))
	{
		static unsigned char s_test = 0;

		const unsigned char m[] = 
		{
			0x23,0x23,0xA0,0xFE,0x4C,0x45,0x57,0x56,0x45,0x42,0x31,0x34,0x36,0x4D,0x4E,0x32,
			0x32,0x30,0x35,0x30,0x32,0x01,0x00,0xAE,0x16,0x0A,0x14,0x0B,0x17,0x13,0xC4,0xDE,
			0xA5,0x00,0x01,0xEE,0x12,0x4D,0x43,0x55,0x38,0x36,0x38,0x33,0x34,0x33,0x30,0x34,
			0x33,0x34,0x39,0x36,0x30,0x35,0x33,0x14,0x38,0x39,0x38,0x36,0x30,0x34,0x39,0x33,
			0x31,0x39,0x32,0x31,0x39,0x30,0x34,0x36,0x32,0x32,0x30,0x33,0x10,0x53,0x31,0x30,
			0x30,0x2E,0x30,0x30,0x34,0x43,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x58,0x66,0x74,
			0x70,0x3A,0x2F,0x2F,0x66,0x74,0x70,0x5F,0x63,0x77,0x6A,0x68,0x5F,0x62,0x79,0x5F,
			0x6D,0x66,0x75,0x3A,0x43,0x37,0x71,0x6F,0x46,0x67,0x6D,0x7A,0x4C,0x58,0x51,0x38,
			0x51,0x77,0x74,0x6B,0x40,0x65,0x76,0x2E,0x63,0x77,0x6A,0x68,0x2E,0x63,0x6E,0x3A,
			0x33,0x30,0x30,0x32,0x31,0x2F,0x67,0x62,0x33,0x32,0x39,0x36,0x30,0x2F,0x6D,0x63,
			0x75,0x2F,0x53,0x31,0x30,0x30,0x2E,0x30,0x30,0x34,0x43,0x32,0x35,0x35,0x2E,0x32,
			0x35,0x35,0x2E,0x7A,0x69,0x70,0x4E,0xBA,0x6E,0x1B,0xE3,0xD6,0x89,0x3C,0xB7,0xDD,
			0x4B,0x55,0xF2,0x44,0x33,0xC1,0xD4
		};

		const unsigned char m1[] = 
		{
			0x23,0x23,0xA0,0xFE,0x4C,0x45,0x57,0x56,0x45,0x42,0x31,0x34,0x36,0x4D,0x4E,0x32,
			0x32,0x30,0x35,0x30,0x32,0x01,0x00,0xAE,0x16,0x0A,0x14,0x0F,0x04,0x16,0xC4,0xB7,
			0x7D,0x00,0x01,0xEE,0x12,0x4D,0x43,0x55,0x38,0x36,0x38,0x33,0x34,0x33,0x30,0x34,
			0x33,0x34,0x39,0x36,0x30,0x35,0x33,0x14,0x38,0x39,0x38,0x36,0x30,0x34,0x39,0x33,
			0x31,0x39,0x32,0x31,0x39,0x30,0x34,0x36,0x32,0x32,0x30,0x33,0x10,0x53,0x31,0x30,
			0x30,0x2E,0x30,0x30,0x35,0x43,0x32,0x35,0x35,0x2E,0x32,0x35,0x35,0x58,0x66,0x74,
			0x70,0x3A,0x2F,0x2F,0x66,0x74,0x70,0x5F,0x63,0x77,0x6A,0x68,0x5F,0x62,0x79,0x5F,
			0x6D,0x66,0x75,0x3A,0x43,0x37,0x71,0x6F,0x46,0x67,0x6D,0x7A,0x4C,0x58,0x51,0x38,
			0x51,0x77,0x74,0x6B,0x40,0x65,0x76,0x2E,0x63,0x77,0x6A,0x68,0x2E,0x63,0x6E,0x3A,
			0x33,0x30,0x30,0x32,0x31,0x2F,0x67,0x62,0x33,0x32,0x39,0x36,0x30,0x2F,0x6D,0x63,
			0x75,0x2F,0x53,0x31,0x30,0x30,0x2E,0x30,0x30,0x35,0x43,0x32,0x35,0x35,0x2E,0x32,
			0x35,0x35,0x2E,0x7A,0x69,0x70,0x9A,0xEF,0xC4,0x8B,0xE1,0xA2,0x5F,0x56,0x92,0x5A,
			0xC3,0x48,0xEE,0x7E,0x19,0x90,0x6C
		};

		s_test = !s_test;
		if (s_test)
		{
			gb32960_ack_plat(0,(void *)m,sizeof(m));
		}
		else
		{
			gb32960_ack_plat(0,(void *)m1,sizeof(m1));
		}
	}
#endif
	if (strstr((char *)pArg,"get_boxid"))
	{
		iccid_str_to_8byte("111111111111111111",szTemp);
		DBG_PrintBuf(LEVEL0,"boxid:",szTemp,8);
	}

	pTemp = find_str_tail(pArg,"test_sl_sos:");
	if (pTemp)
	{
		if (atoi(pTemp) < 32)
		{
			gb32960_can_set_sos_test(1);
			gb32960_can_set_sos_flag_by_bit(atoi(pTemp),1);
			if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"test_sl_sos:%dbit",atoi(pTemp));
		}
		else
		{
			gb32960_can_set_sos_test(0);
			gb32960_can_set_sos_flag(0);
			gb32960_can_set_max_sos_level(0);
			if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"test_sl_sos close");
		}
	}

	if (strstr(pArg,"beian"))
	{
		//<ip5:g6check.vecc.org.cn><port5:20006>
		send_hj1239_beian_info_to_server("g6check.vecc.org.cn",20006);

		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"send beian info\r\n");
	}

	if (GetSubString((char *)pArg,"<chipid:",">",szTemp,sizeof(szTemp)))
	{
		send_msg(S_MSG_SET_JIAMI_CHIP_ID,szTemp,(void *)(strlen(szTemp)),0);
		DBG_Print(LEVEL1,"chip id set ok(%s)\r\n",szTemp);
		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"chip id set ok(%s)\r\n",szTemp);
	}
	if (strstr(pArg,"loadparam"))
	{
		load_param();
	}

	pTemp = find_str_tail(pArg,"set_want_bind:");
	if (pTemp)
	{
		set_want_bind_status(atoi(pTemp));
		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_bind:%d",
				get_want_bind_status());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_want_pro:");
	if (pTemp)
	{
		set_want_pro_ver_about_lock(atoi(pTemp));
		if (pOut && outlen)
		{
			ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_pro:%d",
			get_want_pro_ver_about_lock());
		}
	}

	pTemp = find_str_tail(pArg,"set_want_lock:");
	if (pTemp)
	{
		set_want_lock_status(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_lock:%d",
				get_want_lock_status());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_ns_tip:");
	if (pTemp)
	{
		set_dst_inspect_tip_flag(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_ns_tip:%d",
				get_dst_inspect_tip_flag());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_by_tip:");
	if (pTemp)
	{
		set_dst_keep_fit_tip_flag(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_by_tip:%d",
				get_dst_keep_fit_tip_flag());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_jf_tip:");
	if (pTemp)
	{
		set_dst_pay_tip_flag(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_jf_tip:%d",
				get_dst_pay_tip_flag());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_want_charge:");
	if (pTemp)
	{
		set_dst_want_bms_charge_flag(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_charge:%d",
				get_dst_want_bms_charge_flag());
			}
		}
	}

	pTemp = find_str_tail(pArg,"set_want_limit_level:");
	if (pTemp)
	{
		set_dst_want_limit_speed_level(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_limit_level:%d",
				get_dst_want_limit_speed_level());
			}
		}
	}
	
	pTemp = find_str_tail(pArg,"set_want_kcr:");
	if (pTemp)
	{
		set_dst_want_kucunrong_limit_speed(atoi(pTemp));

		if (pOut && outlen)
		{
			if (pOut && outlen)
			{
				ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get_want_kcr:%d",
				get_dst_want_kucunrong_limit_speed());
			}
		}
	}

	//ATEX=set_collect_can_sw:1

	if (strstr(pArg,"set_collect_can_sw:1"))
	{
		set_collect_can_sw(1);
		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"collect_can_sw:1");
	}
	else if (strstr(pArg,"set_collect_can_sw:0"))
	{
		set_collect_can_sw(0);

		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"collect_can_sw:0");
	}

	if (strstr(pArg,"senddata"))
	{
		unsigned char test_data[] = 
		{
		0x23,0x23,0x02,0xFE,0x54,0x45,0x53,0x54,0x43,0x57,0x4A,0x48,0x31,0x32,0x33,0x34,
		0x35,0x39,0x39,0x39,0x39,0x01,0x00,0xFA,0x15,0x0C,0x03,0x11,0x0B,0x28,0x01,0x01,
		0x03,0x01,0x00,0x00,0x00,0x00,0x09,0xEC,0x0C,0xA8,0x27,0x74,0x11,0x04,0x07,0x50,
		0xC3,0x00,0x00,0x05,0x01,0x06,0xCE,0x39,0x54,0x01,0xD1,0x37,0x10,0x08,0x01,0x01,
		0x00,0x00,0x00,0x00,0x00,0x54,0x00,0x01,0x54,0x0F,0x1A,0x0F,0x19,0x0F,0x18,0x0F,
		0x19,0x0F,0x18,0x0F,0x19,0x0F,0x18,0x0F,0x19,0x0F,0x16,0x0F,0x19,0x0F,0x16,0x0F,
		0x15,0x0F,0x16,0x0F,0x15,0x0F,0x12,0x0F,0x14,0x0F,0x12,0x0F,0x18,0x0F,0x12,0x0F,
		0x18,0x0F,0x18,0x0F,0x18,0x0F,0x18,0x0F,0x18,0x0F,0x19,0x0F,0x18,0x0F,0x18,0x0F,
		0x18,0x0F,0x18,0x0F,0x18,0x0F,0x18,0x0F,0x18,0x0F,0x12,0x0F,0x18,0x0F,0x12,0x0F,
		0x16,0x0F,0x13,0x0F,0x16,0x0F,0x14,0x0F,0x16,0x0F,0x14,0x0F,0x17,0x0F,0x14,0x0F,
		0x17,0x0F,0x18,0x0F,0x17,0x0F,0x18,0x0F,0x16,0x0F,0x18,0x0F,0x16,0x0F,0x13,0x0F,
		0x16,0x0F,0x13,0x0F,0x16,0x0F,0x13,0x0F,0x16,0x0F,0x16,0x0F,0x16,0x0F,0x16,0x0F,
		0x15,0x0F,0x16,0x0F,0x15,0x0F,0x17,0x0F,0x15,0x0F,0x17,0x0F,0x19,0x0F,0x17,0x0F,
		0x19,0x0F,0x14,0x0F,0x19,0x0F,0x14,0x0F,0x19,0x0F,0x14,0x0F,0x19,0x0F,0x18,0x0F,
		0x19,0x0F,0x17,0x0F,0x17,0x0F,0x17,0x0F,0x17,0x0F,0x19,0x0F,0x17,0x0F,0x19,0x0F,
		0x17,0x09,0x01,0x01,0x00,0x1C,0x3A,0x3B,0x3A,0x3A,0x3A,0x3A,0x3A,0x3B,0x3A,0x3A,
		0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,0x3A,
		0x3A,0x3A,0xC2,
		};
		send_msg(S_MSG_SEND_TO_SERVER,0,test_data,(void *)(sizeof(test_data)));
		DBG_Print(0,"send to server\r\n");
	}

	if (strstr(pArg,"getdw"))
	{
		test_calc_charge_dl();
		if (pOut && outlen) ack_len += sprintf((char *)pOut + ack_len,"dw:%02X\r\n",gb32960_can_get_dangwei());
	}

	if (strstr(pArg,"plat_sw1:1"))
	{
		DBG_Print(LEVEL0,"plat_sw1:1\r\n");
		set_plat_sw1(1);
	}
	if (strstr(pArg,"plat_sw1:0"))
	{
		DBG_Print(LEVEL0,"plat_sw1:0\r\n");
		set_plat_sw1(0);
	}

	if (strstr(pArg,"plat_sw2:1"))
	{
		DBG_Print(LEVEL0,"plat_sw2:1\r\n");
		set_plat_sw2(1);
	}
	if (strstr(pArg,"plat_sw2:0"))
	{
		DBG_Print(LEVEL0,"plat_sw2:0\r\n");
		set_plat_sw2(0);
	}


	ptemp = strstr(pArg,"testvin?");
	if (ptemp)
	{
		if (pOut && outlen)
		{
			ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get test:%s\r\n",(char *)(get_test_vin()));
		}
		DBG_Print(LEVEL0,"get test:%s\r\n",get_test_vin());
	}

	ptemp = strstr(pArg,"dfvin?");
	if (ptemp)
	{
		DBG_Print(LEVEL0,"get vin2:%s\r\n",GetLicensePlate());
	}

	ptemp = strstr(pArg,"dfvin:");
	if (ptemp)
	{
		memcpy(szTemp,ptemp + strlen("dfvin:"),17);
		szTemp[17] = 0;

		SetLicensePlate(szTemp);
		DBG_Print(LEVEL0,"set vin2:%s\r\n",GetLicensePlate());
	}

	ptemp = strstr(pArg,"testvin:");
	if (ptemp)
	{
		memcpy(szTemp,ptemp + strlen("testvin:"),17);
		szTemp[17] = 0;

		set_test_vin(szTemp,17);
		if (pOut && outlen)
		{
			ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,"get test:%s\r\n",(char *)(get_test_vin()));
		}
		DBG_Print(LEVEL0,"set test vin:%s\r\n",(char *)(get_test_vin()));
	}
	
	ptemp = strstr(pArg,"terminalid:");
	if (ptemp)
	{
		SetTerminalID(ptemp+strlen("terminalid:"));
		DBG_Print(LEVEL0,"set terminalid:%s\r\n",GetTerminalID());
	}

	ptemp = strstr(pArg,"terminalid?");
	if (ptemp)
	{
		DBG_Print(LEVEL0,"get terminalid:%s\r\n",GetTerminalID());
	}

	//上传CAN报文
	if (strstr(pArg,"sc_can:1"))
	{
		set_report_can_raw_sw(1);
		DBG_Print(LEVEL0,"start repot can data\r\n");
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"start repot can data\r\n");
	}

	if (strstr(pArg,"sc_can:0"))
	{
		set_report_can_raw_sw(0);
		DBG_Print(LEVEL0,"stop repot can data\r\n");
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"stop repot can data\r\n");
	}

	//运维平台心跳时间间隔
	pTemp = strstr(pArg,"set_beat_time:");
	if (pTemp)
	{
		DBG_Print(LEVEL0,"set heart beat time interval:%u\r\n",atoi(pTemp + 14));
		//set_heart_beat_time_interval(atoi(pTemp + 14));

		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"set heart beat time interval:%u\r\n",atoi(pTemp + 14));
	}

	pTemp = strstr(pArg,"set_test_step:");
	if (pTemp)
	{
		DBG_Print(LEVEL0,"set test step:%u\r\n",atoi(pTemp + 14));
		set_test_step(atoi(pTemp + 14));

		//sandian_mcu_start_ota_step(atoi(pTemp + 14),0x9800,1028,0x308400,341108);
		sandian_mcu_start_ota_step(atoi(pTemp + 14),0x9800,1028,0x308400,289272);

		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"set test step:%u\r\n",atoi(pTemp + 14));
	}

	pTemp = strstr(pArg,"set_test_step1:");
	if (pTemp)
	{
		DBG_Print(LEVEL0,"set test step:%u\r\n",atoi(pTemp + 15));
		set_test_step(atoi(pTemp + 15));

		sandian_mcu_start_ota_step(atoi(pTemp + 15),0x9800,1028,0x308400,289448);
		//sandian_mcu_start_ota_step(atoi(pTemp + 15),0x9800,1028,0x308400,289272);

		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"set test step:%u\r\n",atoi(pTemp + 15));
	}

	if (strstr(pArg,"moni_can_data:0"))
	{
		set_moni_can_data_flag(0);
		DBG_Print(LEVEL0,"停止模拟CAN数据\r\n");
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"stop mo ni can data");
	}

	if (strstr(pArg,"moni_can_data:1"))
	{
		set_moni_can_data_flag(1);
		DBG_Print(LEVEL0,"启动模拟CAN数据\r\n");
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"start mo ni can data");
	}

	//设置CAN ID
	if (GetSubString(pArg,"<addcan:",">",szTemp,32))
	{
		INT32U id = 0;
		INT8U hex[8] = {0};

		HexStrToArray((INT8U *)szTemp,hex,8);

		id = (hex[0] << 24) + (hex[1] << 16) + (hex[2] << 8) + hex[3];
		add_canid_in_list(id);
		DBG_Print(0,"add id:%08X\r\n",id);
		if (pOut && outlen) ack_len += sprintf((char *)pOut + ack_len,"add id:%08X\r\n",id);
	}

	if (strstr(pArg,"get_can_list"))
	{
		get_can_list_tip(szTemp,128);
		if (pOut && outlen) ack_len += sprintf((char *)pOut + ack_len,"can list:%s\r\n",szTemp);
		DBG_Print(LEVEL1,"can list:%s\r\n",szTemp);
	}

	pTemp = find_str_tail(pArg,"set_can_speed_id:");
	if (pTemp)
	{
		INT32U id = 0;
		INT8U hex[8] = {0};

		HexStrToArray((INT8U *)pTemp,hex,8);

		id = (hex[0] << 24) + (hex[1] << 16) + (hex[2] << 8) + hex[3];
		set_can_speed_id(id);
		DBG_Print(0,"set speed id:%08X\r\n",id);
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"set speed id:%08X\r\n",id);
	}

	pTemp = find_str_tail(pArg,"set_can_milage_id:");
	if (pTemp)
	{
		INT32U id = 0;
		INT8U hex[8] = {0};

		HexStrToArray((INT8U *)pTemp,hex,8);

		id = (hex[0] << 24) + (hex[1] << 16) + (hex[2] << 8) + hex[3];
		set_can_milage_id(id);
		DBG_Print(0,"set milage id:%08X\r\n",id);
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"set milage id:%08X\r\n",id);
	}

	if (strstr((char *)pArg,"sjz_beian"))
	{
		set_sjz_beian_flag(1);
		DBG_Print(0,"执行平台备案申请\r\n");
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"execute beian");
	}

	if (strstr((char *)pArg,"shoudong_login:1"))
	{
		set_shoudong_login_flag(1);
		DBG_Print(0,"手动登录\r\n");
		
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"execute login");
	}

	if (strstr((char *)pArg,"chaichu_sos:0"))
	{
		set_chaichu_sos_flag(0);
	}

	if (strstr((char *)pArg,"chaichu_sos:1"))
	{
		set_chaichu_sos_flag(1);
		DBG_Print(0,"拆除报警\r\n");
		
		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"chai chu sos");
	}

	/* 设置CAN没有数据的接收超时时间,单位时分钟,ATEX=CANOUTTIME,value */
	ptr = find_str_tail(pArg, "CANOUTTIME,");
	if (ptr)
	{
		INT8U min = (INT8U)atoi(ptr);
		if (min > 0)
		{
			DBG_Print(LEVEL0,"Set Can recv data outtime[%d min] success\r\n", min);
			SysSetCanOuttime(min);
			
		}			
	}
	
	/* 设置休眠基于模式，acc-0,还是can未接收超时-1,ATEX=SLEEPMODE,value */
	ptr = find_str_tail(pArg, "SLEEPMODE,");
	if (ptr)
	{
		INT8U flag = (INT8U)atoi(ptr);

		DBG_Print(LEVEL0,"Set Sleep Mode Base On %s success\r\n", 
		(0 == flag) ? "Acc" : "Can No Data Outtime");

		if (pOut) ack_len += sprintf((char *)pOut + ack_len,"Set Sleep Mode Base On %s success\r\n", 
		(0 == flag) ? "Acc" : "Can No Data Outtime");
	
		SysSetSleepBaseAccCanFlag(flag);	
	}
	
	/* 查询休眠基于什么模式 */
	ptr = find_str_tail(pArg, "SLEEPMODE?");
	if (ptr)
	{
		INT8U flag = SysGetSleepBaseAccCanFlag();
	
		if (!flag)
		{
			DBG_Print(LEVEL0,"Get Sleep Mode Base On Acc\r\n");
			
			if (pOut) ack_len += sprintf((char *)pOut + ack_len,"Get Sleep Mode Base On Acc");
		}
		else
		{
			DBG_Print(LEVEL0,"Get Sleep Mode Base On Can No Data Outtime[%d min]\r\n", 
			SysGetCanOuttime());
			if (pOut) ack_len += sprintf((char *)pOut + ack_len,"Get Sleep Mode Base On Can No Data Outtime[%d min]", 
			SysGetCanOuttime());
		}
	}

	//获取发动机类型
	if (strstr(pArg,"ENGINETYPE?"))
	{
		if (pOut && outlen)
		{
			ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,
			"engine type:%s ryc_type:%s\r\n",get_engine_type_str(get_engine_type()),get_engine_type_str(get_ryc_engine_type()));

		}

		DBG_Print(LEVEL0,"engine type:%s ryc_type:%s\r\n",get_engine_type_str(get_engine_type()),get_engine_type_str(get_ryc_engine_type()));
	}

	//设置发动机类型
	ptemp = strstr(pArg,"ENGINETYPE=");
	if (ptemp)
	{
		set_engine_type((EngineType)atoi(ptemp + strlen("ENGINETYPE=")));

		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,
		"engine type:%s\r\n",get_engine_type_str(get_engine_type()));

		DBG_Print(LEVEL0,"set engine type:%s\r\n",get_engine_type_str(get_engine_type()));
	}

	//清除发动机类型
	ptemp = strstr(pArg,"CLRENGINETYPE");
	if (ptemp)
	{
		set_engine_type(EngineType_Compatible);
		set_ryc_engine_type(EngineType_Compatible);

		if (pOut && outlen) ack_len += snprintf((char *)pOut + ack_len,outlen - ack_len,
		"engine type:%s\r\n",get_engine_type_str(get_engine_type()));

		DBG_Print(LEVEL0,"set engine type:%s\r\n",get_engine_type_str(get_engine_type()));
	}

	if (strstr(pArg,"txcan"))
	{

		DBG_Print(0,"send tx test.\r\n");
	}
	

	if (strstr(pArg,"GPSAT:"))
	{
		DBG_Print(0,"send to gps:%s\r\n",ptr + 6);
		
		//send_to_gps(ptr + 6,strlen(ptr + 6));
	}


	return ack_len;
}


static int Cmd_SetIpPort(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	return 0;
}

/* 所有支持的用户指令 */
SystemCommand const stCmdList[] = 
{
{"ATVER?",	Cmd_GetSoftwareVersion,"查询软件版本"},

{"ATRST",			Cmd_DefaultDevice}, 	//恢复出厂设置

{"ATTEST",			Cmd_Test},

//设置 查询汽车VIN
{"ATVIN=",			Cmd_SetVin,"设置车辆VIN 格式:ATVIN=xxxxxxxxxxxxxxxxx (xxxxxxxxxxxxxxxxx为车辆VIN)"},
{"ATVIN?",			Cmd_GetVin,"查询车辆VIN"},
{"ATINFO?",			Cmd_GetInfo},

//设置 查询SN号
//{"ATSN=",			Cmd_SetSn},
//{"ATSN?",			Cmd_GetSn},


{"ATCHIPID=",		Cmd_SetChipId,"设置加密芯片ID 格式:ATCHIPID=xxxx   (xxx为芯片ID)"},
{"ATCHIPID?",		Cmd_GetChipId,"查询加密芯片ID"},
{"ATPUBKEY?", 		Cmd_GetPubKey,"查询加密芯片公钥"},


//设置 查询CAN1波特率
{"ATCAN1BPS=",		Cmd_SetCan1Bps,"设置第一路CAN波特率 举例: 设置波特率为500K  ATCAN1BPS=500"},
{"ATCAN1BPS?",		Cmd_GetCan1Bps,"查询第一路CAN波特率"},

//设置 查询CAN2波特率
{"ATCAN2BPS=",		Cmd_SetCan2Bps,"设置第二路CAN波特率 举例: 设置波特率为500K  ATCAN2BPS=500"},
{"ATCAN2BPS?",		Cmd_GetCan2Bps,"查询第二路CAN波特率"},


{"ATAPN=",			Cmd_SetApn,"设置APN 举例:设置APN为CMNET:ATAPN=CMNET"},
{"ATAPN?",			Cmd_GetApn,"查询APN"},


{"ATPLAT?", 		Cmd_GetPlat,"获取设备连接的服务器信息"},
{"ATFIRSTIP=",		Cmd_SetFirstIP,"设置第一路IP 举例:ATFIRSTIP=114.114.114.114 或者 ATFIRSTIP=www.bcg.cn"},
{"ATFIRSTPORT=",	Cmd_SetFirstPort,"设置第一路端口 举例:ATFIRSTPORT=9100"},

{"ATSECONDIP=", 	Cmd_SetSecondIP,"设置第二路IP"},
{"ATSECONDPORT=",	Cmd_SetSecondPort,"设置第二路端口"},

{"ATTHIRDIP=",		Cmd_SetThirdIP,"设置第三路IP"},
{"ATTHIRDPORT=",	Cmd_SetThirdPort,"设置第三路端口"},

{"ATIPPORT=",		Cmd_SetIpPort,"设置IP端口 格式:ATIPPORT=第几路连接,协议类型,TCP,服务器地址,服务器端口 举例:ATIPPORT=0,1,TCP,127.0.0.1,60000"},

{"ATEX=",			Cmd_Expand,"扩展"},
{"ATHELP",			Cmd_Help,"指令帮助"}

};

int Cmd_Help(void *pArg,INT32U len,void *pOut,INT32U outlen)
{
	int rlen = 0;
	unsigned int i = 0;
	unsigned int cnt = sizeof(stCmdList) / sizeof(stCmdList[0]);

	for (i = 0; i < cnt; i++)
	{
		DBG_Print(LEVEL0,"[APP]%s 用途:%s\r\n",
		stCmdList[i].pCmd,stCmdList[i].pTip ? stCmdList[i].pTip : "none");
	}

	if (pOut && outlen)
	{
		rlen = snprintf(pOut,outlen,"sms not support the ATHELP command.");
	}
	return rlen;	
}


//手动设置的VIN优先 没有手动配置VIN的情况下 通过CAN获取发动机VIN
INT8U get_vin(void *pOut,int outbuflen)
{
	char can_vin[20] = {0};
	unsigned char channel = GetCanChannel();

	if ((pOut == 0) || (outbuflen < VIN_LEN))
	{
		return 0;
	}

	can_get_vin(channel,can_vin,sizeof(can_vin));

	if (IsObjNULL(can_vin,VIN_LEN))
	{
		memcpy(pOut,GetVIN(),VIN_LEN);
	}
	else
	{
		memcpy(pOut,can_vin,VIN_LEN);
	}
	return VIN_LEN;
}



unsigned int get_obd_data(void *pOutBuf,unsigned int outbuflen)
{
	unsigned int rlen = 0;
#if 1
	unsigned char *candata = (unsigned char *)pOutBuf;
	unsigned char channel = GetCanChannel();
	unsigned int temp = 0;
	unsigned char i = 0;

	//rlen = get_vin(candata,outbuflen);
	memcpy(candata,GetVIN(),17);
	rlen = 17;

	if (can_get_speed(channel) && (can_get_speed(channel) != 0xFFFF))
	{
		temp = can_get_speed(channel);
	}
	else if (gb32960_can_get_speed() != 0xFFFF)
	{
		temp = (gb32960_can_get_speed() / 10) * 256;
	}
	else
	{
		temp = 0;
	}

	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	candata[rlen++] = can_get_kpa(channel);
	candata[rlen++] = can_get_actualtorquepercent(channel);
	candata[rlen++] = can_get_frictiontorquepercent(channel);
	temp = can_get_rpm(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	temp = can_get_fuelflow(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;

	temp = can_get_upperNOx(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	temp = can_get_lowerNOx(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	candata[rlen++] = can_get_remain(channel);
	
	temp = can_get_intakevolume(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;

	temp = can_get_SCRInletTemperature(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	temp = can_get_SCROutletTemperature(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	temp = can_get_DPF(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;

	candata[rlen++] = can_get_coolanttemperature(channel);
	if (can_get_oilboxlevel(channel) && (can_get_oilboxlevel(channel) != 0xFF))
	{
		candata[rlen++] = can_get_oilboxlevel(channel);
	}
	else
	{
		candata[rlen++] = gb32960_can_get_soc() / 0.4;
	}

	if (can_get_milage(channel) && (can_get_milage(channel) != 0xFFFFFFFF))
	{
		temp = can_get_milage(channel);
	}
	else if (gb32960_can_get_milage() != 0xFFFFFFFF)
	{
		temp = gb32960_can_get_milage();
	}
	else
	{
		temp = 0;
	}

	candata[rlen++] = temp >> 24;
	candata[rlen++] = temp >> 16;
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;

	
	candata[rlen++] = can_get_obd_pro(channel);
	candata[rlen++] = can_get_mil_sta(channel);

	temp = can_get_dst_sta(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;
	
	temp = can_get_drt_sta(channel);
	candata[rlen++] = temp >> 8;
	candata[rlen++] = temp;

	can_get_iupr(channel,candata + rlen,outbuflen - rlen);
	rlen += IUPR_LEN;

	if (can_get_dtc_cnt(channel) < 20)
	{
		candata[rlen++] = can_get_dtc_cnt(channel);
		for (i = 0; i < can_get_dtc_cnt(channel); i++)
		{
			if (i >= (DTC_BUF_LEN / 4))
			{
				break; 
			}
			candata[rlen++] = can_get_dtc_buf(channel,i * 4 + 0);
			candata[rlen++] = can_get_dtc_buf(channel,i * 4 + 1);
			candata[rlen++] = can_get_dtc_buf(channel,i * 4 + 2);
			candata[rlen++] = can_get_dtc_buf(channel,i * 4 + 3);
		}
	}
	else
	{
		candata[rlen++] = 0;
	}

	can_get_calid(channel,candata + rlen, CALID_LEN);
	rlen += CALID_LEN;

	can_get_cvn(channel,candata + rlen, CVN_LEN);
	rlen += (CVN_LEN);
#endif
	return rlen;
}




#define _SET_BIT(val,bit_no) val |= (1 << bit_no)
#define _CLR_BIT(val,bit_no) val &= (~(1 << bit_no))

/**
第0位表示:SIM卡插入状态 0表示未插入 1表示已插入
第1位表示是否连上平台标志 0表示未连上 1表示已连上
第2位表示ACC状态 0表示OFF  1表示ON
第3位表示FLASH是否OK 0表示不OK  1表示OK
第4位表示加密芯片是否OK
第5位表示供电模式 0表示主电源供电 1表示电池供电
第6位表示SD卡是否OK
第7位表示是否连上第二路平台标志 0表示未连上 1表示已连上
第8位表示是否连上第三路平台标志 0表示未连上 1表示已连上
第9位表示是否连上第四路平台标志 0表示未连上 1表示已连上

第10位表示emmc芯片是否OK
第11位表示gsensor芯片是否OK
第12位表示是否连上第五路平台标志 0表示未连上 1表示已连上
第13位表示是否连上第六路平台标志 0表示未连上 1表示已连上
第14位表示是否连上第七路平台标志 0表示未连上 1表示已连上
第15位表示wifi是否OK
第16-19位 新能源车型
第20-23位 燃油车车型

第24位表示CAN0是否OK
第25位表示CAN1是否OK
第26位表示CAN2是否OK

27-31位保留 待定
*/
INT32U _get_sw_status(void)
{
	INT32U ret = 0;
	INT8U i = 0;

	if (1)
	{
		_SET_BIT(ret,0);
	}
	else
	{
		_CLR_BIT(ret,0);
	}

	if (get_net_link_status(0))
	{
		_SET_BIT(ret,1);
	}
	else
	{
		_CLR_BIT(ret,1);
	}

	if ((unsigned int)send_msg(S_MSG_GET_ACC_STATUS,0,0,0))
	{
		_SET_BIT(ret,2);
	}
	else
	{
		_CLR_BIT(ret,2);
	}

	if ((unsigned int)send_msg(S_MSG_GET_FLASH_CHIP_STATUS,0,0,0))
	{
		_SET_BIT(ret,3);
	}
	else
	{
		_CLR_BIT(ret,3);
	}

	if ((unsigned int)send_msg(S_MSG_GET_JIAMI_CHIP_STATUS,0,0,0))
	{
		_SET_BIT(ret,4);
	}
	else
	{
		_CLR_BIT(ret,4);
	}

	//第5bit 供电模式
	if (send_msg(S_MSG_GET_POWER_SUPPLY_MODE,0,0,0))
	{
		_SET_BIT(ret,5);
	}
	else
	{
		_CLR_BIT(ret,5);
	}

	//第6bit SD卡标志
	if (0)
	{
		_SET_BIT(ret,6);
	}
	else
	{
		_CLR_BIT(ret,6);
	}

	//二三四路平台连接标志
	for (i = 0; i < 3; i++)
	{		
		if (get_net_link_status(i + 1))
		{
			_SET_BIT(ret,7 + i);
		}
		else
		{
			_CLR_BIT(ret,7 + i);
		}
	}

	//第10bit emmc
	if ((unsigned int)send_msg(S_MSG_GET_EMMC_CHIP_STATUS,0,0,0))
	{
		_SET_BIT(ret,10);
	}
	else
	{
		_CLR_BIT(ret,10);
	}

	//第11bit g-sensor
	if ((unsigned int)send_msg(S_MSG_GET_GSENSOR_CHIP_STATUS,0,0,0))
	{
		_SET_BIT(ret,11);
	}
	else
	{
		_CLR_BIT(ret,11);
	}

	//第12 13 14bit 五六七路平台连接
	for (i = 0; i < 3; i++)
	{
		if (get_net_link_status(i + 4))
		{
			_SET_BIT(ret,12 + i);
		}
		else
		{
			_CLR_BIT(ret,12 + i);
		}
	}

	//第15bit wifi
	if ((unsigned int)send_msg(S_MSG_GET_WIFI_MODULE_STATUS,0,0,0))
	{
		_SET_BIT(ret,15);
	}
	else
	{
		_CLR_BIT(ret,15);
	}

	//第24 25 26bit CAN状态
	if ((unsigned int)send_msg(S_MSG_GET_CAN0_STATUS,0,0,0))
	{
		_SET_BIT(ret,24);
	}
	else
	{
		_CLR_BIT(ret,24);
	}
	if ((unsigned int)send_msg(S_MSG_GET_CAN1_STATUS,0,0,0))
	{
		_SET_BIT(ret,25);
	}
	else
	{
		_CLR_BIT(ret,25);
	}
	if ((unsigned int)send_msg(S_MSG_GET_CAN2_STATUS,0,0,0))
	{
		_SET_BIT(ret,26);
	}
	else
	{
		_CLR_BIT(ret,26);
	}

	ret |= ((((get_ryc_engine_type() << 4) + get_engine_type()) & 0xFF) << 16);

	return ret;
}
#if 1

unsigned int get_status(void *pOutBuf,unsigned int outbuflen)
{
	INT8U *rxbuf = (INT8U *)pOutBuf;
	INT16U len = 0;
	INT32U ulRunTime = 0;
	INT16U usMainPowerVoltage = 0;
	INT16U usBatVoltage = 0;
	INT16U temp = 0;
	INT32U lonlat = 0;
	INT8U lonlatflag = 0;
	char *piccid = 0;
	char *pimei = 0;
	unsigned char *p_mpu_ver = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	piccid = send_msg(S_MSG_GET_ICCID,0,0,0);
	pimei = send_msg(S_MSG_GET_IMEI,0,0,0);

	ulRunTime = GetRunTime();
	usMainPowerVoltage = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
	usBatVoltage = (unsigned int)send_msg(S_MSG_GET_BAT_VOL,0,0,0);

	//设备rtc时间
	send_msg(S_MSG_GET_RTC_TIME,rxbuf + len,0,0);
	len += 6;

	//设备运行时间
	rxbuf[len++] = ulRunTime >> 24;
	rxbuf[len++] = ulRunTime >> 16;
	rxbuf[len++] = ulRunTime >> 8;
	rxbuf[len++] = ulRunTime;

	//主电源电压
	rxbuf[len++] = usMainPowerVoltage >> 8;
	rxbuf[len++] = usMainPowerVoltage;

	//电池电压
	rxbuf[len++] = usBatVoltage >> 8;
	rxbuf[len++] = usBatVoltage;

	//CPU内部Flash大小
	temp = GetCpuFlashSize();
	rxbuf[len++] = temp >> 8;
	rxbuf[len++] = temp;

	//CPU序列号
	rxbuf[len++] = 12;
	//GetCpuID(rxbuf + len,outbuflen - len);
	len += 12;

	//是否定位
	rxbuf[len++] = (INT32U)send_msg(S_MSG_GET_GPS_LOCAL_STATUS,0,0,0);;

	//GpsGetLonInfo(&lonlat, &lonlatflag);

	lonlat = (INT32U)send_msg(S_MSG_GET_LON,0,0,0);
	lonlatflag = 0;

	//东西经
	rxbuf[len++] = lonlatflag;
	//经度
	rxbuf[len++] = lonlat >> 24;
	rxbuf[len++] = lonlat >> 16;
	rxbuf[len++] = lonlat >> 8;
	rxbuf[len++] = lonlat;

	//GpsGetLatInfo(&lonlat, &lonlatflag);
	lonlat = (INT32U)send_msg(S_MSG_GET_LAT,0,0,0);
	lonlatflag = 0;

	//南北纬
	rxbuf[len++] = lonlatflag;
	//纬度
	rxbuf[len++] = lonlat >> 24;
	rxbuf[len++] = lonlat >> 16;
	rxbuf[len++] = lonlat >> 8;
	rxbuf[len++] = lonlat;


	//软件版本号
	p_mpu_ver = send_msg(S_MSG_GET_MPU_SOFTWARE_VERSION,0,0,0);
	temp = strlen(APP_VERSION) + strlen((char *)p_mpu_ver);
	if (temp >= 47)
	{
		temp = 47;
	}

	rxbuf[len++] = temp;
	memcpy(rxbuf + len,APP_VERSION,strlen(APP_VERSION));
	len += strlen(APP_VERSION);
	if (temp > strlen(APP_VERSION)) memcpy(rxbuf + len,p_mpu_ver,temp - strlen(APP_VERSION));
	len += (temp - strlen(APP_VERSION));

	//编译日期
	rxbuf[len++] = strlen(__DATE__) + strlen(__TIME__) + 1;
	memcpy(rxbuf + len,__DATE__,strlen(__DATE__));
	len += strlen(__DATE__);
	rxbuf[len++] = ' ';
	memcpy(rxbuf + len,__TIME__,strlen(__TIME__));
	len += strlen(__TIME__);

	//通信模块是否存在
	rxbuf[len++] = 1;
	//GPS模块是否存在
	rxbuf[len++] = (unsigned int)send_msg(S_MSG_GET_GPS_MODULE_STATUS,0,0,0);

	//状态位
	rxbuf[len++] = _get_sw_status() >> 24;
	rxbuf[len++] = _get_sw_status() >> 16;
	rxbuf[len++] = _get_sw_status() >> 8;
	rxbuf[len++] = _get_sw_status();

	//定位星数
	rxbuf[len++] = (unsigned int)send_msg(S_MSG_GET_GPS_STAR_NUM,0,0,0);
	//SIM卡信号强度
	rxbuf[len++] = (unsigned int)send_msg(S_MSG_GET_SIM_CSQ,0,0,0);
	//GPS天线状态
	rxbuf[len++] = (unsigned int)send_msg(S_MSG_GET_GPS_ANT_STATUS,0,0,0);

	temp = strlen(piccid);
	if (temp >= MAX_SN_LEN)
	{
		temp = MAX_SN_LEN - 1;
	}
	rxbuf[len++] = temp;
	//SN号 用iccid代替
	if (temp) memcpy((char *)rxbuf + len,piccid,temp);
	len += temp;

	//IMEI
	temp = strlen(pimei);
	if (temp >= 20)
	{
		temp = 20;
	}
	rxbuf[len++] = temp;
	if (temp) memcpy((char *)rxbuf + len,pimei,temp);
	len += temp;

	return len;
}


#endif

void report_can_data(void)
{
	unsigned char *ptr = 0;
	unsigned char *candata = 0;
	unsigned int rlen = 0;
	unsigned int sendlen = 0;

	if ((ptr == 0) || (candata == 0))
	{

		return ;
	}

	rlen = get_obd_data(candata,1536);

	sendlen = tccp_pack(TCCP_SLAVE_SEND_CAN_MSG_ID,candata,rlen,ptr,1536);

	DBG_PrintBuf(LEVEL5,"send can data:\r\n",ptr,sendlen);

}

#if 1
void device_heart_beat(void)
{
	unsigned int obd_len = 0;
	unsigned char status_len = 0;
	unsigned char *pSend = 0;
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	unsigned int slen = 0;

	if ((pSend == 0) || (ptr == 0))
	{

		return ;
	}

	obd_len = get_obd_data(pSend,1536);
	status_len = get_status(pSend + obd_len,1536 - obd_len);
	rlen = obd_len + status_len;

	slen = tccp_pack(TCCP_DEVICE_HEART_BEAT_MSG_ID1,pSend,rlen,ptr,1536);

	
	DBG_Print(LEVEL3,"send heart beat(%d):\r\n",slen);
	DBG_PrintBuf(LEVEL3,0,ptr,slen);
}
#endif

void _device_heart_beat1(unsigned char channel)
{
	unsigned int obd_len = 0;
	unsigned char status_len = 0;
	unsigned char *pSend = 0;
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	unsigned int slen = 0;
	
	pSend = os_malloc(1536);
	ptr = os_malloc(1536);
	if ((pSend == 0) || (ptr == 0))
	{
		os_free(pSend);	
		os_free(ptr);
		return ;
	}

	obd_len = get_obd_data(pSend,1536);
	status_len = get_status(pSend + obd_len,1536 - obd_len);
	rlen = obd_len + status_len;

	slen = tccp_pack(TCCP_DEVICE_HEART_BEAT_MSG_ID1,pSend,rlen,ptr,1536);
	os_free(pSend);

	DBG_Print(LEVEL3,"send heart beat(%d):\r\n",slen);
	DBG_PrintBuf(LEVEL3,0,ptr,slen);

	send_msg(S_MSG_SEND_TO_SERVER,(void *)channel,ptr,(void *)slen);
	os_free(ptr);
}

void device_heart_beat1(void)
{
	_device_heart_beat1(8);
}

void test_device_heart_beat(void)
{
	_device_heart_beat1(7);
}

void send_status_to_canbus(void)
{
	unsigned char *pSend = 0;
	unsigned int xlen = 0;
	CanTxMsg txmsg = {0,0,CAN_Id_Extended,CAN_RTR_DATA,8,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
	unsigned char i = 0;

	pSend = os_malloc(1536);
	if (pSend)
	{
		xlen = get_status(pSend,1536);

		while (xlen)
		{
			txmsg.ExtId = 0;
			txmsg.StdId = 0x666;
			txmsg.DLC = 8;
			txmsg.IDE = CAN_Id_Standard;
			txmsg.RTR = CAN_RTR_DATA;

			txmsg.Data[0] = i;
			memcpy(&(txmsg.Data[1]),pSend + (i * 7),7);
			can_send_data(0,&txmsg);
			can_send_data(1,&txmsg);
			can_send_data(2,&txmsg);

			i++;
			xlen = (xlen >= 7) ? (xlen - 7) : 0;

			//DBG_PrintBuf(LEVEL2,"[CAN0]",txmsg.Data,8);
		}
		os_free(pSend);
	}
}

void user_cmd_ex(void *buf,unsigned int len)
{

}

unsigned int analyze_usercmd(void *ptr,unsigned int len,void *pOut,unsigned int outbuflen)
{
	//unsigned char *temp = (unsigned char *)ptr;
	//char *pTemp = 0;
	INT16U i = 0;
	static char acTemp[128] = {0};
	//static char acAnswer[512] = {0};
	unsigned int rlen = 0;

	if ((ptr == 0) || (len == 0))
	{
		return 0;
	}
	user_cmd_ex(ptr,len);

	for (i = 0; i < ARRAY_ELEMENT_CNT(stCmdList); i++)
	{
		if (strstr((char *)ptr,stCmdList[i].pCmd))	//逐一对比指令表
		{
			if (GetSubString((char *)ptr,stCmdList[i].pCmd,"\r\n",acTemp,sizeof(acTemp)) ||
			GetSubString((char *)ptr,stCmdList[i].pCmd,0,acTemp,sizeof(acTemp))
			)
			{
				if (stCmdList[i].pFunc)
				{
					DBG_Print(LEVEL3,"cmd:%s\r\narg:%s\r\n",stCmdList[i].pCmd,acTemp);
					
					rlen = stCmdList[i].pFunc(acTemp,strlen(acTemp),pOut,outbuflen);
				}
			}
		}
	}

	return rlen;
}











