#include "uds_server.h"

#define UDS_SERVER_BUF_MAX_LEN 			256
unsigned char uds_server_tx_buf[UDS_SERVER_BUF_MAX_LEN] = {0};
unsigned int uds_server_tx_buf_len = 0;


unsigned int ai_gzm_list[10] = {0x01160190,0x01160290,0x01160490};
unsigned int s_gzm_cnt = 1;


unsigned int *get_gzm_list(void)
{
	return ai_gzm_list;
}

unsigned int get_gzm_cnt(void)
{
	return s_gzm_cnt;
}

void set_gzm_cnt(unsigned int val)
{
	s_gzm_cnt = val;
}



typedef struct tagDID_ACK
{
	unsigned short did;
	unsigned char *ack_content;
	unsigned int ack_len;
}DID_ACK;



void uds_server_tx(unsigned char channel,unsigned int canid,unsigned char *pdat,unsigned char datlen)
{
	CanTxMsg tx = {0};
	int ret = 0;
	unsigned int a = 0;
	unsigned int b = 0;

	if ((channel >= 3) || (pdat == 0) || (datlen == 0))
	{
		return ;
	}

	if (canid > 0x7FF)
	{
		tx.StdId = 0;
		tx.ExtId = canid;
		tx.IDE = CAN_ID_EXT;
	}
	else
	{
		tx.StdId = canid;
		tx.ExtId = 0;
		tx.IDE = CAN_ID_STD;
	}

	tx.RTR = CAN_RTR_DATA;
	tx.DLC = (datlen > 8) ? 8 : datlen;
	memcpy(tx.Data,pdat,(datlen > 8) ? 8 : datlen);

	a = (unsigned int)send_msg(S_MSG_CPU_CLOCK,0,0,0);
	ret = can_send_data(channel,&tx);
	b = (unsigned int)send_msg(S_MSG_CPU_CLOCK,0,0,0);

	DBG_Print(LEVEL2,"[CAN%u]TX:%X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",channel,canid,
			tx.Data[0],tx.Data[1],tx.Data[2],tx.Data[3],
			tx.Data[4],tx.Data[5],tx.Data[6],tx.Data[7]
			);

	if (ret)
	{
		DBG_Print(LEVEL1,"[uds][CAN%u]send data error:%u %uus\r\n",channel,ret,(b - a) / 108);
	}
}

void uds_server_response(unsigned char channel,unsigned int canid,void *pInputData,unsigned int datlen,void *response,unsigned int response_len)
{
	unsigned char dat[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned int templen = 0;
	unsigned char idx = 0;
	unsigned char *pmsg = (unsigned char *)pInputData;
	unsigned int back_data_len = 0;							//后续数据长度

	if ((pInputData == 0) || (datlen < 2))
	{
		return ;
	}

	if ((pmsg[0] == 0) || (pmsg[0] > 7))
	{
		return ;
	}

	back_data_len = pmsg[0] + response_len;
	if (back_data_len > 7)									//多帧发送
	{
		uds_server_tx_buf[idx++] = 0x10;					//首帧
		uds_server_tx_buf[idx++] = back_data_len;
		uds_server_tx_buf[idx++] = pmsg[1] + 0x40;

		if (pmsg[0] - 1)
		{
			memcpy(uds_server_tx_buf + idx,pmsg + 2,pmsg[0] - 1);
			idx += (pmsg[0] - 1);
		}

		if (response && response_len)
		{
			templen = response_len;
			if (templen >= (UDS_SERVER_BUF_MAX_LEN - 3))
			{
				templen = (UDS_SERVER_BUF_MAX_LEN - 3);
			}

			memcpy(uds_server_tx_buf + idx,response,templen);
			idx += templen;
		}

		memcpy(dat,uds_server_tx_buf,8);
		memcpy(uds_server_tx_buf,uds_server_tx_buf + 8,idx - 8);
		uds_server_tx_buf_len = idx - 8;
	}
	else											//单帧发送
	{
		dat[idx++] = back_data_len;
		dat[idx++] = pmsg[1] + 0x40;
		if (pmsg[0] - 1)
		{
			memcpy(dat + idx,pmsg + 2,pmsg[0] - 1);
			idx += (pmsg[0] - 1);
		}

		if (response && response_len)
		{
			memcpy(dat + idx,response,response_len);
			idx += response_len;
		}
	}

	uds_server_tx(channel,canid,dat,8);
}

void uds_server_response_did(unsigned char channel,unsigned int canid,unsigned char sid,DID_ACK *p_did_ack_list,unsigned int did_cnt)
{
	unsigned char dat[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned int i = 0;
	unsigned int clen = 0;
	unsigned char idx = 0;
	unsigned int templen = 0;
	unsigned int back_data_len = 0;

	if (did_cnt)
	{
		if (p_did_ack_list == 0)
		{
			return ;
		}
	}

	for (i = 0; i < did_cnt; i++)
	{
		clen += p_did_ack_list[i].ack_len;
	}

	back_data_len = 1 + (did_cnt * 2) + clen;
	if (back_data_len > 7)				//多帧发送
	{
		uds_server_tx_buf[idx++] = 0x10;			//首帧
		uds_server_tx_buf[idx++] = back_data_len;
		uds_server_tx_buf[idx++] = sid + 0x40;
		for (i = 0; i < did_cnt; i++)
		{
			uds_server_tx_buf[idx++] = p_did_ack_list[i].did >> 8;
			uds_server_tx_buf[idx++] = p_did_ack_list[i].did;

			templen = p_did_ack_list[i].ack_len;
			if ((templen + idx) >= UDS_SERVER_BUF_MAX_LEN)
			{
				templen = UDS_SERVER_BUF_MAX_LEN - idx;
			}
			memcpy(uds_server_tx_buf + idx,p_did_ack_list[i].ack_content,templen);
			idx += templen;

			if (idx >= (UDS_SERVER_BUF_MAX_LEN - 1)) break;
		}

		memcpy(dat,uds_server_tx_buf,8);
		memcpy(uds_server_tx_buf,uds_server_tx_buf + 8,idx - 8);
		uds_server_tx_buf_len = idx - 8;
	}
	else											//单帧发送
	{
		dat[idx++] = back_data_len;
		dat[idx++] = sid + 0x40;
		for (i = 0; i < did_cnt; i++)
		{
			dat[idx++] = p_did_ack_list[i].did >> 8;
			dat[idx++] = p_did_ack_list[i].did;

			memcpy(dat + idx,p_did_ack_list[i].ack_content,p_did_ack_list[i].ack_len);
			idx += p_did_ack_list[i].ack_len;
		}
	}

	uds_server_tx(channel,canid,dat,8);
}



void service22_handler(unsigned int *p_did,unsigned int did_cnt)
{
	unsigned char ver = get_software_version();
	INT8U *sc_date = get_sc_date();
	DID_ACK did_ack[10] = {0};
	unsigned int i = 0;
	unsigned char *ptr = 0;
	unsigned int len = 0;

	ptr = os_malloc(1536);
	if (ptr)
	{
		for (i = 0; i < did_cnt; i++)
		{
			if (i >= 10)
			{
				break ;
			}

			did_ack[i].did = p_did[i];
			did_ack[i].ack_content = ptr + len;

			switch (p_did[i])
			{
			case 0x0D01:
			*(did_ack[i].ack_content) = 17;
			did_ack[i].ack_len = 1;
			break;

			case 0xF080:
			did_ack[i].ack_len = 22;
			if (strstr(APP_VERSION,"HD22"))
			{
				memcpy(did_ack[i].ack_content + 0,"\x20\x01\x39\x33\x80",5);
				memcpy(did_ack[i].ack_content + 5,"\x00\x73",2);
				memcpy(did_ack[i].ack_content + 7,"\x20\x01\x39\x33\x80",5);
			}
			else
			{
				memcpy(did_ack[i].ack_content + 0,"\x20\x01\x22\x76\x80",5);
				memcpy(did_ack[i].ack_content + 5,"\x00\x73",2);
				memcpy(did_ack[i].ack_content + 7,"\x20\x01\x22\x76\x80",5);
			}
			
			did_ack[i].ack_content[12] = (unsigned int)send_msg(S_MSG_GET_HARDWARE_VERSION,0,0,0);
			did_ack[i].ack_content[13] = ver;
			memcpy(did_ack[i].ack_content + 14,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",8);
			
			break;
			case 0xF0FE:
			did_ack[i].ack_len = 24;
			
			did_ack[i].ack_content[0] = 0xFF;
			did_ack[i].ack_content[1] = 0xFF;
			did_ack[i].ack_content[2] = 0xFF;
			did_ack[i].ack_content[3] = 0xFF;
			
			did_ack[i].ack_content[4] = 0x73;
			did_ack[i].ack_content[5] = 0x01;
			
			did_ack[i].ack_content[6] = sc_date[2]; 		//日
			did_ack[i].ack_content[7] = sc_date[1]; 		//月
			did_ack[i].ack_content[8] = sc_date[0]; 		//年
			
			did_ack[i].ack_content[9] = 01; 				//型号
			
			did_ack[i].ack_content[10] = ver;
			did_ack[i].ack_content[11] = 0;
			did_ack[i].ack_content[12] = ver;
			
			did_ack[i].ack_content[13] = sc_date[2];		//日
			did_ack[i].ack_content[14] = sc_date[1];		//月
			did_ack[i].ack_content[15] = sc_date[0];		//年
			
			did_ack[i].ack_content[16] = 0;
			
			did_ack[i].ack_content[17] = 0x13;
			did_ack[i].ack_content[18] = 0x32;
			did_ack[i].ack_content[19] = 0xFD;
			
			did_ack[i].ack_content[20] = 1;

			did_ack[i].ack_content[21] = 0x12;
			did_ack[i].ack_content[22] = 0x28;
			did_ack[i].ack_content[23] = 0x33;
			
			break;
			case 0xF180:					//bootloader 版本
				memcpy(did_ack[i].ack_content,"BYSD_BOOT_V1.0.1",strlen("BYSD_BOOT_V1.0.1"));
				did_ack[i].ack_len = strlen("BYSD_BOOT_V1.0.1");
			break;
			case 0xF186:
				did_ack[i].ack_len = 1;
				did_ack[i].ack_content[0] = 1;			//当前所处的诊断会话模式
			break;
			case 0xF18B:
				did_ack[i].ack_content[0] = sc_date[2]; 	//日
				did_ack[i].ack_content[1] = sc_date[1]; 	//月
				did_ack[i].ack_content[2] = sc_date[0]; 	//年
				did_ack[i].ack_len = 3;
			break;
			case 0xF18C:
				memcpy(did_ack[i].ack_content,GetVIN(),17);
				did_ack[i].ack_len = 17;
			break;
			case 0xF193:
				did_ack[i].ack_content[0] = 0;
				did_ack[i].ack_content[1] = (unsigned int)send_msg(S_MSG_GET_HARDWARE_VERSION,0,0,0);
				did_ack[i].ack_len = 2;
			break;
			case 0xF195:
				did_ack[i].ack_content[0] = 0;
				did_ack[i].ack_content[1] = ver;
				did_ack[i].ack_len = 2;
			break;
			
			default:break;
			}

			len += did_ack[i].ack_len;
		}
		
		uds_server_response_did(0,UDS_SERVER_RESPONSE_ID,0x22,did_ack,did_cnt);

		os_free(ptr);
	}
}


//uds client send {0x77C,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x10,0xFF,0xFF,0xFF,0xFF}}
//uds server ack  {0x67C,0,CAN_Id_Standard,CAN_RTR_DATA,8,{length,0x62,0xF8,0x10,data0,data1,data2,data3}}
void uds_server_proc(unsigned char channel,CanRxMsg *pMsg)
{
	unsigned int canid = 0;
	unsigned char sid = 0;
	unsigned char dat[8] = {0};
	unsigned int templen = 0;

	if (pMsg == 0)
	{
		return ;
	}

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}

	if ((canid != UDS_SERVER_PHY_ID) && (canid != UDS_SERVER_FUNC_ID))
	{
		return ;
	}

	//发送剩余的数据帧
	if (pMsg->Data[0] == 0x30)
	{
		unsigned char i = 0;

		while (uds_server_tx_buf_len)
		{
			memset(dat,0xFF,sizeof(dat));
			dat[0] = 0x21 + i++;
			
			memcpy(dat + 1,uds_server_tx_buf + templen,(uds_server_tx_buf_len > 7) ? 7 : uds_server_tx_buf_len);
			uds_server_tx(0,UDS_SERVER_RESPONSE_ID,dat,8);

			i = i % 15;
			if (uds_server_tx_buf_len > 7)
			{
				uds_server_tx_buf_len -= 7;
				templen += 7;
			}
			else
			{
				uds_server_tx_buf_len = 0;
			}
		}
	}
	else
	{
		sid = pMsg->Data[1];
		switch (sid)
		{
		case 0x10:
		uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,"\x00\x32\x01\xF4",4);
		break;

		case 0x11:			//复位
			memset(dat,0xFF,sizeof(dat));
			dat[0] = 0x02;
			dat[1] = 0x51;
			dat[2] = pMsg->Data[2];
			uds_server_tx(0,UDS_SERVER_RESPONSE_ID,dat,8);
		break;

		case 0x14:			//清除诊断信息
			if ((pMsg->Data[2] == 0xFF) && (pMsg->Data[3] == 0xFF) && (pMsg->Data[4] == 0xFF))
			{
				set_gzm_cnt(0);
			}
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,0,0);
		break;

		case 0x19:			//故障码
			if (GetRunTime() < 40)
			{
				ai_gzm_list[0] = 1;
				set_gzm_cnt(1);
			}
			else
			{
				char *p_mpu_ver = 0;

				p_mpu_ver = send_msg(S_MSG_GET_MPU_SOFTWARE_VERSION,0,0,0);
				if (strlen(p_mpu_ver) < 4)
				{
					ai_gzm_list[0] = 2;
					set_gzm_cnt(1);
				}
				else
				{
					ai_gzm_list[0] = 0;
					set_gzm_cnt(0);
				}
			}
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,get_gzm_list(),get_gzm_cnt() * 4);
		break;
		case 0x22:
			if (pMsg->Data[0] >= 3)
			{
				unsigned int did[3] = {0};

				did[0] = (pMsg->Data[2] << 8) + pMsg->Data[3];
				did[1] = (pMsg->Data[4] << 8) + pMsg->Data[5];
				did[2] = (pMsg->Data[6] << 8) + pMsg->Data[7];
				templen = (pMsg->Data[0] - 1) / 2;
				if (templen > 3) templen = 3;
				service22_handler(did,templen);
			}
		break;

		case 0x27:
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,"\x11\x22\x33\x44",4);
		break;
		case 0x28:
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,0,0);
		break;
		case 0x3E:
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,0,0);
		break;
		case 0x85:
			uds_server_response(0,UDS_SERVER_RESPONSE_ID,pMsg->Data,8,0,0);
		break;

		default:

		break;
		}
	}
}





