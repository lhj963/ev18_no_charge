#include "adhd.h"

#define ADHD_DATA_OFFSET (2+1+17+2)


typedef struct tagADHDStruction
{
	unsigned char auth_ok_flag : 1;			//验证OK标志
	unsigned char random[4];				//随机数

}ADHDStruction;
ADHDStruction stADHDStruction = {0};


//奥动换电协议

INT16U CRC16RTU(void *pszBuf,unsigned int unLength)
{
	INT16U CRC=0XFFFF;
	INT16U CRC_count;

	for(CRC_count=0;CRC_count<unLength;CRC_count++)
	{
		int i;

		CRC=CRC^*((unsigned char *)pszBuf+CRC_count);
		for(i=0;i<8;i++)
		{
			if(CRC&1)
			{
				CRC>>=1;
				CRC^=0xA001;
			}
			else
			{
				CRC>>=1;
			}
		}
	}
	return CRC;
}


unsigned int adhd_pack(void *pOut,unsigned int outbuflen,unsigned char cmd,void *pvin,void *pcontent,unsigned int contentlen)
{
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int rlen = 0;
	unsigned int templen = 0;

	if ((ptr == 0) || (outbuflen < 20))
	{
		return 0;
	}

	ptr[rlen++] = 0x23;
	ptr[rlen++] = 0x23;
	ptr[rlen++] = cmd;
	if (pvin) memcpy(ptr + rlen,pvin,17);
	rlen += 17;

	if (pcontent && contentlen && (outbuflen >= 22))
	{
		templen = (outbuflen >= (contentlen + 22)) ? contentlen : (outbuflen - 22);

		ptr[rlen++] = templen;
		ptr[rlen++] = templen >> 8;
		
		if (templen) memcpy(ptr + rlen,pcontent,templen);
		rlen += templen;
	}

	return rlen;
}


unsigned int adhd_request_radom_pack(void *pOut,unsigned int outbuflen,void *pvin)
{
	return adhd_pack(pOut,outbuflen,0x01,pvin,0,0);
}

unsigned int adhd_send_radom_pack(void *pOut,unsigned int outbuflen,void *pvin,void *pRecvRadom,unsigned int recvlen)
{
	unsigned int crc = 0;
	unsigned char buf[4] = {0};

	crc = CRC16RTU(pRecvRadom,recvlen);

	buf[0] = crc;
	buf[1] = crc >> 8;
	buf[2] = 0;
	buf[3] = 0;
	return adhd_pack(pOut,outbuflen,0x02,pvin,buf,4);
}


unsigned int adhd_car_status_pack(void *pOut,unsigned int outbuflen,void *pvin)
{
	unsigned char *ptr = (unsigned char *)pOut + ADHD_DATA_OFFSET;
	unsigned int rlen = 0;

	ptr[rlen++] = 0;			//高压状态
	ptr[rlen++] = 0;			//驻车状态
	ptr[rlen++] = 0;			//epb状态
	ptr[rlen++] = 0;			//换电允许状态
	ptr[rlen++] = 0;			//车速
	ptr[rlen++] = 0;


	return adhd_pack(pOut,outbuflen,0x03,pvin,ptr,rlen);
}

unsigned int adhd_connector_status_pack(void *pOut,unsigned int outbuflen,void *pvin)
{
	unsigned char *ptr = (unsigned char *)pOut + ADHD_DATA_OFFSET;
	unsigned int rlen = 0;

	ptr[rlen++] = 0;			//锁止状态 0-锁止；1-未锁止
	ptr[rlen++] = 0;			//连接器连接状态 0-连接正常；1-连接异常

	return adhd_pack(pOut,outbuflen,0x04,pvin,ptr,rlen);
}

unsigned int adhd_bms_pack(void *pOut,unsigned int outbuflen,void *pvin)
{
	unsigned char *ptr = (unsigned char *)pOut + ADHD_DATA_OFFSET;
	unsigned int rlen = 0;

	memcpy(ptr,"123456781234567812345678",24);		//电池编码
	rlen += 24;
	ptr[rlen++] = 0;		//电池入网状态

	ptr[rlen++] = 0;		//当前里程
	ptr[rlen++] = 0;

	ptr[rlen++] = 0;		//SOC
	ptr[rlen++] = 0;

	ptr[rlen++] = 0;		//总里程
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	ptr[rlen++] = 0;		//当前SOC
	ptr[rlen++] = 0;		//当前SOH

	//单体电压最高
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	//单体电压最低
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	//高压极柱温度1-6
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	//可用容量
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	//可用能量
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	return adhd_pack(pOut,outbuflen,0x05,pvin,ptr,rlen);
}


//响应后台下发的换电状态
unsigned int adhd_responese_hdstatus_pack(void *pOut,unsigned int outbuflen,void *pvin)
{
	return adhd_pack(pOut,outbuflen,0x06,pvin,0,0);
}

void deal_adhd(unsigned char linkid,void *pdat,unsigned int datlen)
{
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned char cmd = 0;
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned int contentlen = 0;

	cmd = *(ptr + 2);
	contentlen = ptr[20] + (ptr[21] << 8);

	switch (cmd)
	{
	case 0x81:
		pbuf = os_malloc(1024);
		if (pbuf)
		{
			rlen = adhd_send_radom_pack(pbuf,1024,GetVIN(),ptr + ADHD_DATA_OFFSET,contentlen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,pbuf,(void *)rlen);
			os_free(pbuf);
		}
	break;
	case 0x82:
		stADHDStruction.auth_ok_flag = ((*(unsigned char *)(ptr + ADHD_DATA_OFFSET)) ? 0 : 1);

	break;
	
	case 0x84:	//查询连接器状态
		pbuf = os_malloc(1024);
		if (pbuf)
		{
			rlen = adhd_connector_status_pack(pbuf,1024,GetVIN());
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,pbuf,(void *)rlen);
			os_free(pbuf);
		}
	break;

	case 0x86:
		pbuf = os_malloc(1024);
		if (pbuf)
		{
			rlen = adhd_responese_hdstatus_pack(pbuf,1024,GetVIN());
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,pbuf,(void *)rlen);
		
			os_free(pbuf);
		}
	break;
		
	default:break;
	}
}


void adhd_analyze(unsigned char linkid,void *pdat,unsigned int datlen)
{
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned int templen = datlen;
	unsigned int contentlen = 0;
	unsigned char vin_temp[18] = {0};
	
	unsigned char cmd = 0;

	while (1)
	{
		ptr = _find_sub_str(ptr,templen,"\x23\x23",2);
		if (ptr)
		{
			cmd = ptr[2];
			if(cmd != 0x84)
			{
				contentlen = ptr[20] + (ptr[21] << 8);
				if (contentlen >= templen)			//数据有误 寻找下一个23 23
				{
					ptr += 2;
					if (templen >= (ptr - (unsigned char *)pdat))
					{
						templen -= (ptr - (unsigned char *)pdat);
					}
					else
					{
						templen = 0;
					}
					continue;
				}
			}
			
			memcpy(vin_temp,ptr + 3,17);
			deal_adhd(linkid,ptr,ADHD_DATA_OFFSET + contentlen);

			ptr += 2;
			if (templen >= (ptr - (unsigned char *)pdat))
			{
				templen -= (ptr - (unsigned char *)pdat);
			}
			else
			{
				templen = 0;
			}
		}
		else
		{
			break;
		}
	}
}


void adhd_proc(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	static unsigned int s_last_check_time = 0;
	static unsigned int s_last_send_time = 0;
	static unsigned int send_flag = 0;

	if (get_net_link_status(7) == 0)
	{
		stADHDStruction.auth_ok_flag = 0;
	}
	else
	{
		if (stADHDStruction.auth_ok_flag == 0)
		{
			if (((GetRunTime() - s_last_check_time) >= 10) && get_net_link_status(7))
			{
				pbuf = os_malloc(1024);
				if (pbuf)
				{
					rlen = adhd_request_radom_pack(pbuf,1024,GetVIN());
					send_msg(S_MSG_SEND_TO_SERVER,(void *)7,pbuf,(void *)rlen);
					os_free(pbuf);
				}
			
				 s_last_check_time = GetRunTime();
			}

		}
		else
		{
			if ((GetRunTime() - s_last_send_time) >= 1)
			{
				pbuf = os_malloc(1536);
				if (pbuf)
				{
					if(send_flag)
					{
						rlen = adhd_car_status_pack(pbuf,1536,GetVIN());
						send_msg(S_MSG_SEND_TO_SERVER,(void *)7,pbuf,(void *)rlen);
						send_flag = 0;
					}
					else
					{
						rlen = adhd_bms_pack(pbuf,1536,GetVIN());
						send_msg(S_MSG_SEND_TO_SERVER,(void *)7,pbuf,(void *)rlen);
						send_flag = 1;
					}
					os_free(pbuf);
				}

				s_last_send_time = GetRunTime();
			}
		}
	}
}

