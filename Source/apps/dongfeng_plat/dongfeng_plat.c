#include "dongfeng_plat.h"



unsigned int xGB32960_DongfengBasicPack(void *pOut,unsigned int outbuflen,void *pVIN)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned char *cp = "鄂A00001";
	char *psn = 0;
	char *p_mpu_ver = 0;
	unsigned int templen = 0;
	unsigned char tid[8] = {0};
	unsigned char imei[21] = {0};
	static unsigned int s_df_c6_flow = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	//时间
	memcpy(ptr,get_rtc_time_ptr(),6);
	rlen += 6;

	//类型
	ptr[rlen++] = 0xC6;

	//流水
	s_df_c6_flow++;
	ptr[rlen++] = s_df_c6_flow >> 16;
	ptr[rlen++] = s_df_c6_flow >> 8;
	ptr[rlen++] = s_df_c6_flow;

	//分包标识
	ptr[rlen++] = 0;

	//vin
	if (pVIN) memcpy(ptr + rlen,pVIN,17);
	rlen += 17;

	//车牌长度
	ptr[rlen++] = strlen((char *)cp);
	memcpy(ptr + rlen,cp,strlen((char *)cp));
	rlen += strlen((char *)cp);

	//终端ID 8Bytes
	send_msg(S_MSG_GET_IMEI,imei,(void *)21,0);
	templen = strlen((char *)imei);
	if (templen == 0)
	{
		return 0;
	}
	if (templen < 20)
	{
		memset(imei,'0',20 - templen);
		send_msg(S_MSG_GET_IMEI,imei + (20 - templen),(void *)16,0);
	}
	else
	{
		return 0;
	}
	iccid_str_to_8byte(imei,tid);
	memcpy(ptr + rlen,tid,8);
	rlen += 8;

	//硬件版本号
	ptr[rlen++] = 1;
	ptr[rlen++] = 1;

	//TBOX固件版本号
	ptr[rlen++] = 1;
	ptr[rlen++] = 1;


	//vcu序列号
	ptr[rlen++] = 6;
	memcpy(ptr + rlen,"123456",6);
	rlen += 6;

	//mcu序列号
	psn = send_msg(S_MSG_GET_IMEI,0,0,0);
	ptr[rlen++] = strlen(psn);
	memcpy(ptr + rlen,psn,strlen(psn));
	rlen += strlen(psn);

	//bms序列号
	ptr[rlen++] = 6;
	memcpy(ptr + rlen,"111111",6);
	rlen += 6;

	//vcu源地址
	ptr[rlen++] = 0x01;

	//mcu源地址
	ptr[rlen++] = 0x02;

	//bms源地址
	ptr[rlen++] = 0x03;

	//vcu版本号
	ptr[rlen++] = strlen("BYSD_VCU_V1.0.1");
	memcpy(ptr + rlen,"BYSD_VCU_V1.0.1",strlen("BYSD_VCU_V1.0.1"));
	rlen += strlen("BYSD_VCU_V1.0.1");

	//mcu版本
	p_mpu_ver = send_msg(S_MSG_GET_MPU_SOFTWARE_VERSION,0,0,0);
	if ((p_mpu_ver == 0) || (strlen(p_mpu_ver) == 0))
	{
		return 0;
	}

	templen = strlen(APP_VERSION) + strlen((char *)p_mpu_ver);
	if (templen >= 47)
	{
		templen = 47;
	}

	ptr[rlen++] = templen;
	memcpy(ptr+rlen,APP_VERSION,(strlen(APP_VERSION) > templen) ? templen : strlen(APP_VERSION));
	rlen += ((strlen(APP_VERSION) > templen) ? templen : strlen(APP_VERSION));
	if (templen > strlen(APP_VERSION))
	{
		memcpy(ptr + rlen,p_mpu_ver,templen - strlen(APP_VERSION));
		rlen += (templen - strlen(APP_VERSION));
	}

	//bms版本号
	ptr[rlen++] = strlen("BYSD_BMS_V1.0.1");
	memcpy(ptr + rlen,"BYSD_BMS_V1.0.1",strlen("BYSD_BMS_V1.0.1"));
	rlen += strlen("BYSD_BMS_V1.0.1");

	//换电控制状态
	ptr[rlen++] = 0;

	return _xDataBag(pOut,outbuflen,0x20,0xFE,pVIN,ptr,rlen);
}


unsigned int xGB32960_DongfengNewBasicPack(void *pOutBuf,INT16U usOutBufSize,
																	void *pVIN,
																	INT8U t[6],
																	void *piccid,unsigned int iccid_len,
																	void *pesn,unsigned int esn_len
																	)
{
	static unsigned int s_sandian_plat_flow = 0;
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
	s_sandian_plat_flow++;
	ptr[len++] = s_sandian_plat_flow >> 8;
	ptr[len++] = s_sandian_plat_flow;

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

	//IMSI
	memcpy(ptr + len,pimei,15);
	len += 15;

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
	ptr[len++] = strlen("鄂A00003");
	memcpy(ptr + len,"鄂A00003",strlen("鄂A00003"));
	len += strlen("鄂A00003");

	//VCU序列号
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

	//VCU所在CAN网络
	ptr[len++] = 1;					//0 整车CAN(BMS) 1 驱动CAN (VCU MCU)
	
	//VCU源地址
	ptr[len++] = 0xEF;

	templen = strlen((char *)get_vcu_ver_ptr());
	if (templen > 16)
	{
		templen = 16;
	}
	//VCU标定版本号
	ptr[len++] = templen;
	memcpy(ptr + len,get_vcu_ver_ptr(),templen);
	len += templen;

	//VCU型号
	ptr[len++] = strlen("CMMNS");
	memcpy(ptr + len,"CMMNS",strlen("CMMNS"));
	len += strlen("CMMNS");

	//VCU标定文件格式
	ptr[len++] = 3;			//1.bin 2.zip 3.gz

	//MCU序列号
	ptr[len++] = 3 + 15;
	memcpy(ptr + len,"MCU",3);
	len += 3;
	memcpy(ptr + len,pimei,15);
	len += 15;

	//MCU所在CAN网络
	ptr[len++] = 1;					//0 整车CAN(BMS) 1 驱动CAN (VCU MCU)

	//MCU源地址
	ptr[len++] = 0xEE;

	templen = strlen((char *)get_mcu_ver_ptr());
	if (templen > 16)
	{
		templen = 16;
	}
	//MCU标定版本号
	ptr[len++] = templen;
	memcpy(ptr + len,get_mcu_ver_ptr(),templen);
	len += templen;

	//MCU型号
	ptr[len++] = strlen("HUICHUAN");
	memcpy(ptr + len,"HUICHUAN",strlen("HUICHUAN"));
	len += strlen("HUICHUAN");

	//MCU标定文件格式
	ptr[len++] = 3;			//1.bin 2.zip 3.gz

	//BMS序列号
	ptr[len++] = 3 + 15;
	memcpy(ptr + len,"BMS",3);
	len += 3;
	memcpy(ptr + len,pimei,15);
	len += 15;

	//BMS所在CAN网络
	ptr[len++] = 0;					//0 整车CAN(BMS) 1 驱动CAN (VCU MCU)

	//BMS源地址
	ptr[len++] = 0xC8;

	//BMS标定版本号
	templen = strlen((char *)get_bms_ver_ptr());
	if (templen > 16)
	{
		templen = 16;
	}
	ptr[len++] = templen;
	memcpy(ptr + len,get_bms_ver_ptr(),templen);
	len += templen;

	//BMS型号
	ptr[len++] = strlen("BMS");
	memcpy(ptr + len,"BMS",strlen("BMS"));
	len += strlen("BMS");

	//BMS标定文件格式
	ptr[len++] = 1;			//1.bin 2.zip 3.gz


	//仪表方案选项
	ptr[len++] = strlen("tspBasic");
	memcpy(ptr + len,"tspBasic",strlen("tspBasic"));
	len += strlen("tspBasic");

	//充电控制
	ptr[len++] = 0;

	//OTA
	ptr[len++] = 0xFF;

	//锁车功能
	ptr[len++] = 0x01;

	//诊断功能
	ptr[len++] = 0x01;

	return _xDataBag(pOutBuf,usOutBufSize,0x35,0xFE,pVIN,ptr,len);	
}


