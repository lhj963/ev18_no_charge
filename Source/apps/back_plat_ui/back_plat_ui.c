#include "back_plat_ui.h"




unsigned int heart_beat_time_interval = 40;

unsigned char rp_can_raw_sw = 0;
unsigned int send_idx = 0;

unsigned char get_report_can_raw_sw(void)
{
	return rp_can_raw_sw;
}

void set_report_can_raw_sw(unsigned char val)
{
	rp_can_raw_sw = val;
	send_idx = 0;
}

unsigned int get_heart_beat_time_interval(void)
{
	return heart_beat_time_interval;
}

void set_heart_beat_time_interval(unsigned int val)
{
	heart_beat_time_interval = val;
}

#define BLOCK_CAN_BUS_DATA_ID	3		//CAN记录

void remote_log(void *pdat,unsigned int datlen)
{
	unsigned char *ptr = 0;
	unsigned char *psend = 0;
	unsigned int rlen = 0;
	unsigned int xlen = 0;
	unsigned char iccid[24] = {0};

	psend = os_malloc(1536);
	ptr = os_malloc(1536);

	if ((ptr == 0) || (psend == 0))
	{
		os_free(ptr);
		os_free(psend);
		return ;
	}

	send_msg(S_MSG_GET_ICCID,iccid,(void *)20,0);

	ptr[xlen++] = 20;
	memcpy(ptr + xlen,iccid,20);
	xlen += 20;
	ptr[xlen++] = 0;
	ptr[xlen++] = 0;

	rlen = tccp_pack(TCCP_DEVICE_REPORT_LOG_MSG_ID,ptr,xlen,psend,1536);
	os_free(ptr);

	send_msg(S_MSG_SEND_TO_SERVER,(void *)8,psend,(void *)rlen);
	os_free(psend);
}

void _analyze_tccp(unsigned char channel,unsigned int msgid,void * content,unsigned int contentlen)
{
	unsigned int rlen = 0;
	static char ack[512] = {0};
	unsigned char *ptr = 0;
	unsigned int xlen = 0;
	unsigned char *psend = 0;

	ptr = content;
	
	switch (msgid)
	{
	case TCCP_SET_BACKDOOR_HEART_INV_MSG_ID:	//设置心跳时间间隔
	if (contentlen < 2) break;
	set_heart_beat_time_interval((ptr[0] << 8) + ptr[1]);
	DBG_Print(LEVEL0,"set heart beat time interval:%u\r\n",(ptr[0] << 8) + ptr[1]);
	break;
	case TCCP_TRANS_CMD_MSG_ID:
	rlen = (unsigned int)send_msg(S_MSG_EXECUTE_DRV_CMD,ptr,ack,(void *)(sizeof(ack)));
	if (rlen)
	{
		psend = os_malloc(1536);
		if (psend)
		{
			xlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,ack,rlen,psend,1536);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)channel,psend,(void *)xlen);
			os_free(psend);
		}
	}
#if 1
	rlen = analyze_usercmd(ptr,contentlen,ack,sizeof(ack));
	if (rlen)
	{
		psend = os_malloc(1536);
		if (psend)
		{
			xlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,ack,rlen,psend,1536);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)channel,psend,(void *)xlen);
			os_free(psend);
		}
	}
#endif
	break;
	default:break;
	}

}

void analyze_tccp(unsigned int msgid,void * content,unsigned int contentlen)
{
	_analyze_tccp(8,msgid,content,contentlen);
}


void link4_data_arrive(void *pdat,unsigned int rlen)
{
	DBG_Print(0,"[bd]Server Data(%d):\r\n",rlen);
	DBG_PrintBuf(0,0,pdat,rlen);

	tccp_unpack(pdat,rlen,analyze_tccp,0);
}

static INT32U s_bk_last_send_time = 0;
void server4_connect_ok(void)
{
	device_heart_beat1();
	s_bk_last_send_time = GetRunTime();
}

void server4_connected(void)
{
	//char flag = 0;
	static INT8U s_s_cnt = 0;

	if ((GetRunTime() - s_bk_last_send_time) >= get_heart_beat_time_interval())
	{
		device_heart_beat1();

		s_bk_last_send_time = GetRunTime();
	}

	s_s_cnt++;
	if (s_s_cnt >= 5)
	{
		s_s_cnt = 0;

	}
}

#if 1
static INT32U s_bk_test_last_send_time = 0;

void server_test_connect_ok(void)
{
	test_device_heart_beat();
	s_bk_test_last_send_time = GetRunTime();
}

void server_test_connected(void)
{
	if ((GetRunTime() - s_bk_test_last_send_time) >= get_heart_beat_time_interval())
	{
		test_device_heart_beat();

		s_bk_test_last_send_time = GetRunTime();
	}
}

void test_analyze_tccp(unsigned int msgid,void * content,unsigned int contentlen)
{
	_analyze_tccp(7,msgid,content,contentlen);
}

void link_test_data_arrive(void *pdat,unsigned int rlen)
{
	DBG_Print(0,"[test]Server Data(%d):\r\n",rlen);
	DBG_PrintBuf(0,0,pdat,rlen);

	tccp_unpack(pdat,rlen,test_analyze_tccp,0);
}
#endif


RecvCanMsg s_can_msg[10] = {0};
void report_to_server_temp(unsigned char can_channel,CanRxMsg *prxmsg)
{
	static unsigned int s_last_rp_time = 0;
	static unsigned char rp_cnt = 0;
	unsigned int slen = 0;
	unsigned char *pbuf = 0;
	unsigned char pt[6] = {0};

	send_msg(S_MSG_GET_RTC_TIME,(void *)pt,(void *)6,0);	

	s_can_msg[rp_cnt].decs = ((pt[0] - 20) << 26) | (pt[1] << 22) | (pt[2] << 17) | (pt[3] << 12) | (pt[4] << 6) | pt[5] | (can_channel << 30);
	memcpy(&(s_can_msg[rp_cnt].msg),prxmsg,sizeof(CanRxMsg));
	rp_cnt++;
	if (rp_cnt >= 10)
	{
		rp_cnt = 0;
		if (get_report_can_raw_sw() && ((GetRunTime() - s_last_rp_time) >= 1))
		{
			pbuf = os_malloc(1536);
			if (pbuf)
			{
				slen = tccp_pack(TCCP_RP_CAN_CMD_MSG_ID,s_can_msg,sizeof(s_can_msg),pbuf,1500);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)8,(void *)pbuf,(void *)slen);
				os_free(pbuf);
				s_last_rp_time = GetRunTime();
			}
		}
	}
}



