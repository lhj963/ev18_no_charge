#include "zhongche_plat.h"


unsigned short zhongche_CRC16(unsigned char *message,unsigned int len)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned short temp = 0;

	temp = 0xFFFF;
	for (i = 0; i < len; i++)
	{
		temp = temp ^ message[i];
		for(j = 0; j < 8; j++)
		{
			if(temp & 0x0001)
			{
				temp = temp >> 1;
				temp = temp ^ 0x1021;
			}
			else
			{
				temp = temp >> 1;
			}
		}
	}
	return temp;
}


unsigned int ZHONGCHE_LoginPack(void *pOutBuf,unsigned int outbuflen,
										void *pVIN,INT8U terminalid[6],void *iccid)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + GB32960_CONTENT_OFFSET;

	if (terminalid)
	{
		memcpy(ptr+rlen,terminalid,6);
	}
	rlen += 6;

	//ICCID
	if (iccid)
	{
		memcpy(ptr + rlen,iccid,GB_LEN_ICCID);
	}
	rlen += GB_LEN_ICCID;

	return _xDataBag(pOutBuf,outbuflen,ZHONGCHE_LOGIN_CMD,0xFE,pVIN,ptr,rlen);
}


//故障预测
unsigned int ZHONGCHE_Guzhangyuce(void *pOutBuf,unsigned int outbuflen,INT8U time[6])
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int i = 0;
	unsigned int temp = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + GB32960_CONTENT_OFFSET;

	//----------动力电池故障预测数据--------
	//采集时间
	memcpy(ptr+rlen,time,6);
	rlen += 6;

	ptr[rlen++] = 0xF3;		//数据类型标志
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00;		//软件版本号
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x00;

	ptr[rlen++] = 0x01;		//电压不均衡故障预测位

	ptr[rlen++] = 0x00;		//电压不均衡 TOP1 单体编号
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00;		//电压不均衡 TOP2 单体编号
	ptr[rlen++] = 0x02;

	ptr[rlen++] = 0x00;		//电压不均衡 TOP3 单体编号
	ptr[rlen++] = 0x03;

	ptr[rlen++] = 0x00;		//电压不均衡 TOP4 单体编号
	ptr[rlen++] = 0x04;

	ptr[rlen++] = 0x00;		//电压不均衡 TOP5 单体编号
	ptr[rlen++] = 0x05;


	ptr[rlen++] = 0x01;		//温度不均衡故障预测位

	ptr[rlen++] = 0x00;		//温度不均衡 TOP1 单体编号
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00;		//温度不均衡 TOP2 单体编号
	ptr[rlen++] = 0x02;

	ptr[rlen++] = 0x00;		//温度不均衡 TOP3 单体编号
	ptr[rlen++] = 0x03;

	ptr[rlen++] = 0x00;		//温度不均衡 TOP4 单体编号
	ptr[rlen++] = 0x04;

	ptr[rlen++] = 0x00;		//温度不均衡 TOP5 单体编号
	ptr[rlen++] = 0x05;

	//----------电池统计数据(最大值)-----------
	//采集时间
	memcpy(ptr+rlen,time,6);
	rlen += 6;

	ptr[rlen++] = 0xF3;		//数据类型标志
	ptr[rlen++] = 0x11;

	ptr[rlen++] = 0x00;		//软件版本号
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x00;

	ptr[rlen++] = 0x00;		//电池单体个数
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00;		//电池温度探针个数
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00; 	//总电压最大值
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00; 	//正向电流最大值
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00; 	//负向电流最大值
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 200;		//SOC 最大值

	ptr[rlen++] = 0x00; 	//正极绝缘电阻最大值
	ptr[rlen++] = 0x01;

	ptr[rlen++] = 0x00; 	//负极绝缘电阻最大值
	ptr[rlen++] = 0x01;


	//单体电压最大值列表
	for (i = 0; i < temp; i++)
	{

	}

	//单体温度最大值列表
	for (i = 0; i < temp; i++)
	{

	}


	return xDataBag(pOutBuf,outbuflen,ZHONGCHE_GZYC_DATA_CMD,ptr,rlen);
}

unsigned int zy_pdu(void *pdat,unsigned int datlen)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned int xlen = datlen;

	//头0x7E 尾0x7E 不纳入转义范围
	for (i = 1; i < (xlen - 1); i++)
	{
		if (ptr[i] == 0x7E)
		{
			//从i+1开始往后移动一个字节
			for (j = xlen-1; j > i; j--)
			{
				ptr[j+1] = ptr[j];
			}
			
			ptr[i] = 0x7D;
			ptr[i+1] = 0x02;

			xlen++;
		}
		else if (ptr[i] == 0x7D)
		{
			//从i+1开始往后移动一个字节
			for (j = xlen-1; j > i; j--)
			{
				ptr[j+1] = ptr[j];
			}
			
			ptr[i] = 0x7D;
			ptr[i+1] = 0x01;

			xlen++;
		}
	}
	return xlen;
}

unsigned short gps_can_msg_flower = 0;	//消息流水
unsigned char devid[6] = {0};			//终端ID
INT8U subdevcode = 3;
INT8U gps_gys_code = 4;

unsigned int ZHONGCHE_GpsAndCanPack(void *pOutBuf,unsigned int outbuflen,
												INT16U gps_msgid,void *gps_msgbody,INT16U gps_msgbody_len,
												INT16U can_msgid,void *can_msgbody,INT16U can_msgbody_len
												)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	INT16U crc16jy = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + GB32960_CONTENT_OFFSET;

	//消息头
	ptr[rlen++] = 0x7E;					//包头

	ptr[rlen++] = gps_msgid;				//设备消息ID
	ptr[rlen++] = gps_msgid >> 8;

	ptr[rlen++] = gps_msgbody_len;			//设备消息体长度
	ptr[rlen++] = gps_msgbody_len >> 8;

	ptr[rlen++] = can_msgid;				//CAN消息ID
	ptr[rlen++] = can_msgid >> 8;

	ptr[rlen++] = can_msgbody_len;			//CAN消息体长度
	ptr[rlen++] = can_msgbody_len >> 8;

	gps_can_msg_flower++;
	ptr[rlen++] = gps_can_msg_flower;			//消息流水
	ptr[rlen++] = gps_can_msg_flower >> 8;
	//终端ID
	memcpy(ptr+rlen,GetTerminalID(),6);					//终端ID
	rlen += 6;

	ptr[rlen++] = subdevcode;		//子设备代码
	ptr[rlen++] = gps_gys_code;				//GPS供应商代码
	ptr[rlen++] = 0;					//密钥 0

	//消息体
	if (gps_msgbody && gps_msgbody_len)
	{
		memcpy(ptr+rlen,gps_msgbody,gps_msgbody_len);
		rlen += gps_msgbody_len;
	}

	if (can_msgbody && can_msgbody_len)
	{
		memcpy(ptr+rlen,can_msgbody,can_msgbody_len);
		rlen += can_msgbody_len;
	}

	//包尾
	crc16jy = zhongche_CRC16(ptr + 1,rlen - 1);

	ptr[rlen++] = crc16jy;			//crc16校验
	ptr[rlen++] = crc16jy >> 8;

	ptr[rlen++] = 0x7E;					//包尾

	rlen = zy_pdu(ptr,rlen);

	return xDataBag(pOutBuf,outbuflen,ZHONGCHE_GPS_AND_CAN_DATA_CMD,ptr,rlen);
}


#define MAX_REPORT_CAN_MSG_CNT 		400
OneCanMsg stOneCanMsg[MAX_REPORT_CAN_MSG_CNT] = {0};
INT16U report_can_msg_cnt = 0;
INT16U last_s_can_idx = 0;
INT16U s_total_can_id_cnt = 0;

void update_can_msg(INT8U channel,INT32U canid,INT8U dat[8])
{
	unsigned short i = 0;
	unsigned short s_write_idx = 0;

	//在缓冲区中查询是否存在该ID
	for (i = 0; i < MAX_REPORT_CAN_MSG_CNT; i++)
	{
		if (stOneCanMsg[i].canid == canid)
		{
			s_write_idx = i;
			break ;
		}
	}

	//如果整个缓冲区中都未找到该CAN ID 查找无效的数据 将无效的数据空间腾出来 给该id使用
	if (i >= MAX_REPORT_CAN_MSG_CNT)
	{
		for (i = 0; i < MAX_REPORT_CAN_MSG_CNT; i++)
		{
			if (stOneCanMsg[i].is_ok_flag == 0)
			{
				s_write_idx = i;

				s_total_can_id_cnt++;
				break;
			}
		}
	}

	//缓冲区中没有多余的空间容纳该帧CAN数据了 只能丢弃
	if (i >= MAX_REPORT_CAN_MSG_CNT)
	{
		return ;
	}

	s_write_idx = i;
	stOneCanMsg[s_write_idx].is_ok_flag = 1;
	stOneCanMsg[s_write_idx].channel = channel;
	stOneCanMsg[s_write_idx].canid = canid;
	memcpy(stOneCanMsg[s_write_idx].dat,dat,8);

	//消息条数增加
	if (report_can_msg_cnt < MAX_REPORT_CAN_MSG_CNT)
	{
		report_can_msg_cnt++;
	}

}

void clr_report_can_msg(void)
{
	unsigned short i = 0;

	report_can_msg_cnt = 0;

	for (i = 0; i < MAX_REPORT_CAN_MSG_CNT; i++)
	{
		stOneCanMsg[i].is_ok_flag = 0;
	}
}

INT16U get_total_can_id_cnt(void)
{
	return s_total_can_id_cnt;
}



//实时CAN基本信息上报
unsigned int _ZHONGCHE_RealCanBasePack(void *pOutBuf,unsigned int outbuflen,
													char is_abc,ZCCanBase *pdat,
													INT16U *aucIdx,INT16U idx_cnt)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int xlen = 0;
	unsigned int i = 0;
	unsigned int idx = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = get_big_mem();
	if (ptr == 0)
	{
		return 0;
	}

	ptr[rlen++] = pdat->reason;						//上报原因

	//real_base_can_flow++;						//包序号
	ptr[rlen++] = pdat->bagid;
	ptr[rlen++] = pdat->bagid >> 8;
	ptr[rlen++] = pdat->bagid >> 16;
	ptr[rlen++] = pdat->bagid >> 24;


	ptr[rlen++] = pdat->lon;					//经度
	ptr[rlen++] = pdat->lon >> 8;
	ptr[rlen++] = pdat->lon >> 16;
	ptr[rlen++] = pdat->lon >> 24;

	ptr[rlen++] = pdat->lat;					//纬度
	ptr[rlen++] = pdat->lat >> 8;
	ptr[rlen++] = pdat->lat >> 16;
	ptr[rlen++] = pdat->lat >> 24;

	ptr[rlen++] = pdat->speed;		//速度
	ptr[rlen++] = pdat->speed >> 8;

	ptr[rlen++] = pdat->gaodu;		//高度
	ptr[rlen++] = pdat->gaodu >> 8;

	ptr[rlen++] = pdat->fxj;		//方向角
	ptr[rlen++] = pdat->fxj >> 8;

	ptr[rlen++] = pdat->utc_year;		//年
	ptr[rlen++] = pdat->utc_month;		//月
	ptr[rlen++] = pdat->utc_day;		//日
	ptr[rlen++] = pdat->utc_hour;		//时
	ptr[rlen++] = pdat->utc_min;		//分
	ptr[rlen++] = pdat->utc_sec;		//秒
	ptr[rlen++] = pdat->utc_baifenzhiyi_sec;		//百分之一秒

	ptr[rlen++] = 1;				//CAN 数据格式

	if (is_abc)
	{
		ptr[rlen++] = idx_cnt + 3;		//CAN 数据包个数
	}
	else
	{
		ptr[rlen++] = idx_cnt;		//CAN 数据包个数
	}
#if 1
	//for (i = 0; i < 3; i++)
	if (is_abc)
	{
		INT32U tempid = 0;

		ptr[rlen++] = (INT8U)0;					//和上包CAN数据的时间间隔 单位:ms
		ptr[rlen++] = 0;

		tempid = 0x180AA7A3;
		ptr[rlen++] = tempid;			//CAN id
		ptr[rlen++] = tempid >> 8;
		ptr[rlen++] = tempid >> 16;
		ptr[rlen++] = tempid >> 24;

		ptr[rlen++] = 1;					//can 通道
		memcpy(ptr + rlen,send_180AA7A3_dat,8);				//can数据
		rlen += 8;

		ptr[rlen++] = (INT8U)0; 				//和上包CAN数据的时间间隔 单位:ms
		ptr[rlen++] = 0 >> 8;
		
		tempid = 0x180BA7A3;
		ptr[rlen++] = tempid;			//CAN id
		ptr[rlen++] = tempid >> 8;
		ptr[rlen++] = tempid >> 16;
		ptr[rlen++] = tempid >> 24;
		
		ptr[rlen++] = 1;					//can 通道
		memcpy(ptr + rlen,send_180BA7A3_dat,8); 			//can数据
		rlen += 8;

		ptr[rlen++] = (INT8U)0; 				//和上包CAN数据的时间间隔 单位:ms
		ptr[rlen++] = 0 >> 8;
		
		tempid = 0x180CA7A3;
		ptr[rlen++] = tempid;			//CAN id
		ptr[rlen++] = tempid >> 8;
		ptr[rlen++] = tempid >> 16;
		ptr[rlen++] = tempid >> 24;
		
		ptr[rlen++] = 1;					//can 通道
		memcpy(ptr + rlen,send_180CA7A3_dat,8); 			//can数据
		rlen += 8;
	}
#endif

	for (i = 0; i < idx_cnt; i++)
	{
		idx = aucIdx[i];

		ptr[rlen++] = (INT8U)0; 				//和上包CAN数据的时间间隔 单位:ms
		ptr[rlen++] = 0;
		
		ptr[rlen++] = stOneCanMsg[idx].canid;			//CAN id
		ptr[rlen++] = stOneCanMsg[idx].canid >> 8;
		ptr[rlen++] = stOneCanMsg[idx].canid >> 16;
		ptr[rlen++] = stOneCanMsg[idx].canid >> 24;
		
		ptr[rlen++] = stOneCanMsg[idx].channel; 				//can 通道
		
		memcpy(ptr + rlen,stOneCanMsg[idx].dat,8);				//can数据
		rlen += 8;
	}

	xlen = ZHONGCHE_GpsAndCanPack(pOutBuf,outbuflen,0,0,0,ZHONGCHE_CAN_MSGID_BASE,ptr,rlen);

	free_big_mem(ptr);
	return xlen;
}


//实时CAN基本信息上报
unsigned int ZHONGCHE_RealCanBasePack(void *pOutBuf,unsigned int outbuflen,char is_first_bag,ZCCanBase *pdat)
{
	unsigned short i = 0;
	unsigned short cnt = 0;
	INT16U aucIdx[80] = {0};

	//没有CAN消息 就直接跳出去
	if ((report_can_msg_cnt == 0) && (is_first_bag == 0))
	{
		return 0;
	}

	if (is_first_bag)
	{
		last_s_can_idx = 0;
	}

	for (i = last_s_can_idx; i < MAX_REPORT_CAN_MSG_CNT; i++)
	{
		if (stOneCanMsg[i].is_ok_flag)
		{
			aucIdx[cnt++] = i;
			if (cnt >= 80)
			{
				break ;
			}
		}
	}

	last_s_can_idx = i + 1;

	if (cnt == 0)
	{
		return 0;
	}
	return _ZHONGCHE_RealCanBasePack(pOutBuf,outbuflen,is_first_bag,pdat,aucIdx,cnt);
}







//附加数据
unsigned int ZHONGCHE_ExdataPack(void *pOutBuf,unsigned int outbuflen,ZCExData *pdat)
{
	unsigned char ptr[32] = {0};
	unsigned int rlen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr[rlen++] = 3;		//附加信息个数


	ptr[rlen++] = 0x01;		//ID
	ptr[rlen++] = 1;		//长度
	ptr[rlen++] = pdat->status;		//内容  SD卡状态

	ptr[rlen++] = 0x02;		//ID
	ptr[rlen++] = 2;		//长度
	ptr[rlen++] = pdat->main_power_vol;		//内容  电压电压 单位:0.1V
	ptr[rlen++] = pdat->main_power_vol >> 8;		//内容

	ptr[rlen++] = 0x03;		//ID
	ptr[rlen++] = 1;		//长度
	ptr[rlen++] = pdat->bat_vol;		//内容  电池电压 单位:0.1V

	return ZHONGCHE_GpsAndCanPack(pOutBuf,outbuflen,ZHONGCHE_GPS_MSGID_EXDATA,ptr,rlen,0,0,0);
}



//心跳
unsigned int ZHONGCHE_HeartBeatPack(void *pOutBuf,unsigned int outbuflen,ZCHeartBeatData *pdat)
{
	unsigned char ptr[64] = {0};
	unsigned int rlen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr[rlen++] = pdat->yy;		//上报原因

	ptr[rlen++] = pdat->lon;		//经度
	ptr[rlen++] = pdat->lon >> 8;
	ptr[rlen++] = pdat->lon >> 16;
	ptr[rlen++] = pdat->lon >> 24;


	ptr[rlen++] = pdat->lat;		//纬度
	ptr[rlen++] = pdat->lat >> 8;
	ptr[rlen++] = pdat->lat >> 16;
	ptr[rlen++] = pdat->lat >> 24;


	ptr[rlen++] = pdat->speed;		//速度
	ptr[rlen++] = pdat->speed >> 8;

	ptr[rlen++] = pdat->gaodu;		//高度
	ptr[rlen++] = pdat->gaodu >> 8;

	ptr[rlen++] = pdat->fxj;		//
	ptr[rlen++] = pdat->fxj >> 8;

	ptr[rlen++] = pdat->milage;		//里程
	ptr[rlen++] = pdat->milage >> 8;
	ptr[rlen++] = pdat->milage >> 16;
	ptr[rlen++] = pdat->milage >> 24;

	
	ptr[rlen++] = pdat->hv;		//
	ptr[rlen++] = pdat->hv >> 8;

	memcpy(ptr+rlen,pdat->sv,4);
	rlen += 4;

	ptr[rlen++] = pdat->year;
	ptr[rlen++] = pdat->month;
	ptr[rlen++] = pdat->day;
	ptr[rlen++] = pdat->hour;
	ptr[rlen++] = pdat->min;
	ptr[rlen++] = pdat->sec;

	ptr[rlen++] = pdat->gps_md_status;
	return ZHONGCHE_GpsAndCanPack(pOutBuf,outbuflen,ZHONGCHE_GPS_MSGID_HEARTBEAT,ptr,rlen,0,0,0);
}


char zhongcheiccid[21] = {0};
INT8U s_car_lock_status = 0;
INT8U plat_lock_car_cmd = 0;

void set_plat_lock_car_cmd(INT8U val)
{
	plat_lock_car_cmd = val;
}

INT8U get_plat_lock_car_cmd(void)
{
	return plat_lock_car_cmd;
}

INT8U get_car_lock_status(void)
{
	return s_car_lock_status;
}

void set_car_lock_status(INT8U val)
{
	s_car_lock_status = val;
}


//判断车辆是否处于锁车状态
INT8U is_car_lock_status(void)
{
	// 2 6 7表示锁车状态
	if ((s_car_lock_status == 2) || (s_car_lock_status == 6) || (s_car_lock_status == 7))
	{
		return 1;
	}
	return 0;
}

//判断车辆是否处于解锁状态
INT8U is_car_unlock_status(void)
{
	// 0 3 4 5表示解锁状态
	if ((s_car_lock_status == 0) || (s_car_lock_status == 3) || (s_car_lock_status == 4) || (s_car_lock_status == 5))
	{
		return 1;
	}
	return 0;
}

void *get_zhongche_iccid(void)
{
	return zhongcheiccid;
}

void set_zhongche_iccid(void *piccid)
{
	if (piccid)
	{
		memcpy(zhongcheiccid,piccid,20);
	}
}


void zhongche_deal(INT8U linkid,INT8U cmd,INT8U ack_flag,void *pvin,INT8U jiami_way,void *content,INT16U contentlen)
{
	unsigned char ptr[64] = {0};
	unsigned int rlen = 0;
	unsigned char body[16] = {0};
	unsigned int xlen = 0;
	unsigned short msgid = 0;
	unsigned char *pcontent = (unsigned char *)content;

	switch (cmd)
	{
	case ZHONGCHE_SETVIN_CMD:		//设置VIN 和 ICCID
	DBG_PrintBuf(LEVEL0,"set vin:\r\n",content,17);
	DBG_PrintBuf(LEVEL0,"set ICCID:\r\n",(unsigned char *)content + 17,20);

	SetVIN(content);
	set_zhongche_iccid((char *)content + 17);

	break;

	case ZHONGCHE_GPS_AND_CAN_DATA_CMD:
	msgid = pcontent[5] + (pcontent[6] << 8);

	DBG_Print(LEVEL3,"msgid:%04X\r\n",msgid);	
	if (msgid == ZHONGCHE_TC_CAN_MSG_SUB_CMD)		//透传CAN消息
	{
		body[rlen++] = 1;		//1表示成功 2表示失败
		xlen = ZHONGCHE_GpsAndCanPack(ptr,64,0,0,0,ZHONGCHE_GPS_MSGID_ASK_TC,body,rlen);
		net_send_in_cb(linkid,ptr,xlen);
	}
	else if (msgid == ZHONGCHE_SQ_SUB_CMD)			//车辆授权
	{
		set_plat_lock_car_cmd(pcontent[20]);
		DBG_Print(LEVEL3,"lock cmd:%02X\r\n",get_plat_lock_car_cmd());

		body[rlen++] = 1;		//1表示成功 2表示失败
		xlen = ZHONGCHE_GpsAndCanPack(ptr,64,0,0,0,ZHONGCHE_GPS_MSGID_ASK_SQ,body,rlen);
		net_send_in_cb(linkid,ptr,xlen);
		DBG_Print(LEVEL3,"[link%u]终端应答平台车辆授权(%uBytes):\r\n",linkid,xlen);
		DBG_PrintBuf(LEVEL3,0,ptr,xlen);
	}
	else											//其他消息
	{
		//未识别命令		
		body[rlen++] = 0;		//终端消息命令号
		body[rlen++] = 0;
		body[rlen++] = 0;		//Can网络消息命令号
		body[rlen++] = 0;
		body[rlen++] = 1;		//未识别

		xlen = ZHONGCHE_GpsAndCanPack(ptr,64,0,0,0,ZHONGCHE_GPS_MSGID_UNKOWNCMD,body,rlen);
		net_send_in_cb(linkid,ptr,xlen);
	}	
	break;
	default:

	break;
	}
}


#define MIN_PACK_LEN (2+2+17+1+2+1)

#define CMD_OFFSET     				2
#define CMD_ACK_FLAG_OFFSET    		3
#define VIN_OFFSET					4
#define JIAMI_WAY_OFFSET			21
#define CONTENT_LEN_OFFSET			22
#define CONTENT_OFFSET				24

void zhongche_analyze(INT8U linkid,void *pdat,unsigned int len)
{
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned char *phead = 0;

	unsigned char cmd = 0;
	unsigned char ack_flag = 0;
	unsigned char *pvin = 0;
	unsigned char jiami_way = 0;
	unsigned char *content = 0;
	unsigned short contentlen = 0;
	unsigned char xor = 0;

	if (len < MIN_PACK_LEN)
	{
		return ;
	}

	if ((pdat == 0) || (len == 0))
	{
		return ;
	}

	while (1)
	{
		phead = _find_sub_str(ptr,len,"\x23\x23",2);
		if (phead == 0)
		{
			break;
		}

		//长度检验
		if ((ptr + len - phead) < MIN_PACK_LEN)
		{
			break;
		}

		cmd = *(phead + CMD_OFFSET);
		ack_flag = *(phead + CMD_ACK_FLAG_OFFSET);
		pvin = phead + VIN_OFFSET;
		jiami_way = *(phead + JIAMI_WAY_OFFSET);
		contentlen = ((*(phead + CONTENT_LEN_OFFSET)) << 8) + (*(phead + CONTENT_LEN_OFFSET + 1));
		content = phead + CONTENT_OFFSET;

		//长度检验
		if ((phead + CONTENT_OFFSET + contentlen) >= (ptr+len))
		{
			break;
		}
		xor = *(phead + CONTENT_OFFSET + contentlen);

		DBG_Print(LEVEL2,"-------(link%d)zhongche server ack-------\r\n"
						"cmd:%02X ack:%02X vin:%s jiami:%02X dataunitlen:%d xor:%02X\r\n",
						linkid,cmd,ack_flag,pvin,jiami_way,contentlen,xor);
		zhongche_deal(linkid,cmd,ack_flag,pvin,jiami_way,content,contentlen);
		ptr = phead + CONTENT_OFFSET + contentlen + 1;
		len -= (CONTENT_OFFSET + contentlen + 1);
	}
}


unsigned char leap_month_day[] = 
{0,31,29,31,30,31,30,31,31,30,31,30,31};

unsigned char no_leap_month_day[] = 
{0,31,28,31,30,31,30,31,31,30,31,30,31};

/**
公历闰年能被4整除且不能被100整除的为闰年，平均每四年中会有一个闰年，
世纪年能被400整除的则是闰年，例如2000是闰年，而1900年不是闰年，
对于数值很大的年份，如果能够整除3200以及172800则是闰年
*/
char _is_leap_year(unsigned int year)
{
	unsigned char flag = 0;

	if (((year % 4) == 0) && (year % 100))
	{
		flag = 1;
	}

	if ((year % 400) == 0)
	{
		flag = 1;
	}
	return flag;
}

//year年month月day天 加 want_to_add_day 天 want_to_add_day 不能超过27
char _add_day(unsigned int *year,
			unsigned char *month,unsigned char *day,unsigned char want_to_add_day)
{
	char leap_year_flag = _is_leap_year(*year);	//闰年标志
	unsigned char cur_month_day = 0;		//当前月份的天数
	unsigned long after_add = *day + want_to_add_day;

	if (leap_year_flag)
	{
		cur_month_day = leap_month_day[*month];
	}
	else
	{
		cur_month_day = no_leap_month_day[*month];
	}

	if (after_add > cur_month_day)
	{
		*day = after_add - cur_month_day;
		if ((*month + 1) > 12)
		{
			*month = 1;
			*year = *year + 1;
		}
		else
		{
			*month = *month + 1;
		}
	}
	else
	{
		*day = after_add;
	}


	return 0;
}


void dec_day(INT8U year,INT8U month,INT8U day,
				INT8U *result_year,INT8U *result_month,INT8U *result_day)
{
	unsigned char *pmonthday = 0;

	if (day != 1)	//只要day不是第一天就随便减
	{
		*result_year = year;
		*result_month = month;
		*result_day = day - 1;
	}
	else
	{
		if (month != 1)	//不是一月随便减
		{
			if (_is_leap_year(year+2000))
			{
				pmonthday = leap_month_day;
			}
			else
			{
				pmonthday = no_leap_month_day;
			}

			*result_year = year;
			*result_month = month - 1;
			*result_day = pmonthday[month - 1];
		}
		else			//一月
		{
			if (_is_leap_year(year + 2000 - 1))
			{
				pmonthday = leap_month_day;
			}
			else
			{
				pmonthday = no_leap_month_day;
			}

			*result_year = year - 1;
			*result_month = 12;
			*result_day = pmonthday[12];
		}
	}
}

void localtime_to_utc(char zone,INT8U local_year,INT8U local_month,INT8U local_day,INT8U local_hour,
								INT8U *utc_year,INT8U *utc_month,INT8U *utc_day,INT8U *utc_hour)
{
	INT8U xzone = 0;

	if ((signed char)zone >= (char)0)
	{
		if (local_hour >= zone)
		{
			if (utc_year) *utc_year = local_year;
			if (utc_month) *utc_month = local_month;
			if (utc_day) *utc_day = local_day;
			if (utc_hour) *utc_hour = local_hour - zone;
		}
		else
		{
			if (utc_hour)
			{
				*utc_hour = 24 + local_hour - zone;
			}
		
			//倒退一天
			dec_day(local_year,local_month,local_day,utc_year,utc_month,utc_day);
		}

	}
	else
	{
		xzone = -zone;

		if ((local_hour + xzone) >= 24)				//小时产生进位
		{
			unsigned int tempyear = 0;
			unsigned char tempmonth = 0;
			unsigned char tempday = 0;

			*utc_hour = local_hour + xzone - 24;		//处理小时

			tempyear = local_year + 2000;
			tempmonth = local_month;
			tempday = local_day;

			_add_day(&tempyear,&tempmonth,&tempday,1);
			*utc_year = tempyear - 2000;
			*utc_month = tempmonth;
			*utc_day = tempday;
		}
		else
		{
			if (utc_year) *utc_year = local_year;
			if (utc_month) *utc_month = local_month;
			if (utc_day) *utc_day = local_day;
			if (utc_hour) *utc_hour = local_hour + xzone;
		}
	}
}

char is_vin_ok(void *vin)
{
	INT8U i = 0;
	INT8U dat = 0;

	for (i = 0; i < 17; i++)
	{
		dat = *((INT8U *)vin + i);

		if (((dat >= '0') && (dat <= '9'))
			|| ((dat >= 'a') && (dat <= 'z'))
			|| ((dat >= 'A') && (dat <= 'Z'))
		)
		{

		}
		else
		{
			return 0;
		}
	}
	return 1;
}

unsigned char s_zhongche_vin[17] = {0};

void zhongche_can_vin_analyze(CanRxMsg *prxmsg)
{
	INT32U can_id = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;
	INT8U dat = prxmsg->Data[0];

	if (can_id == 0x1803A709)
	{
		if ((prxmsg->Data[0] == 0x01) || (prxmsg->Data[0] == 0x02))
		{
			memcpy(s_zhongche_vin + (dat - 1) * 7,prxmsg->Data + 1,7);
		}
		else if (prxmsg->Data[0] == 0x03)
		{
			memcpy(s_zhongche_vin + 14,prxmsg->Data + 1,3);

			if (is_vin_ok(s_zhongche_vin))
			{
				SetVIN(s_zhongche_vin);
			}
		}
	}
}




void zhongche_can_analyze(CanRxMsg *prxmsg)
{
	INT32U can_id = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;

	if (can_id == 0x1802A709)			//指示当前锁车状态
	{
		set_car_lock_status(prxmsg->Data[1]);
	}
}



