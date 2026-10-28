#include "ningde_bms_ota.h"


typedef enum tagCAN_BMS_OTA_STEP
{
	CAN_BMS_OTA_NONE = 0,

	CAN_BMS_OTA_STEP_1,
	CAN_BMS_OTA_STEP_1_WAIT_ACK,

	CAN_BMS_OTA_STEP_2,
	CAN_BMS_OTA_STEP_2_WAIT_ACK,


	CAN_BMS_OTA_STEP_3,
	CAN_BMS_OTA_STEP_3_WAIT_ACK,


	CAN_BMS_OTA_STEP_4,
	CAN_BMS_OTA_STEP_4_WAIT_ACK,


	CAN_BMS_OTA_STEP_5,
	CAN_BMS_OTA_STEP_5_WAIT_ACK,


	CAN_BMS_OTA_STEP_6,
	CAN_BMS_OTA_STEP_6_WAIT_ACK,


	CAN_BMS_OTA_STEP_7,
	CAN_BMS_OTA_STEP_7_WAIT_ACK,


	CAN_BMS_OTA_STEP_8,
	CAN_BMS_OTA_STEP_8_WAIT_ACK,

	CAN_BMS_OTA_STEP_9,
	CAN_BMS_OTA_STEP_9_WAIT_ACK,

	CAN_BMS_OTA_STEP_10,
	CAN_BMS_OTA_STEP_10_WAIT_ACK,

	CAN_BMS_OTA_STEP_11,
	CAN_BMS_OTA_STEP_11_WAIT_ACK,

	CAN_BMS_OTA_STEP_12,
	CAN_BMS_OTA_STEP_12_WAIT_ACK,

	CAN_BMS_OTA_STEP_13,
	CAN_BMS_OTA_STEP_13_WAIT_ACK,

	CAN_BMS_OTA_STEP_14,
	CAN_BMS_OTA_STEP_14_WAIT_ACK,

	CAN_BMS_OTA_STEP_15,
	CAN_BMS_OTA_STEP_15_WAIT_ACK,

	CAN_BMS_OTA_STEP_16,
	CAN_BMS_OTA_STEP_16_WAIT_ACK,

	CAN_BMS_OTA_STEP_17,
	CAN_BMS_OTA_STEP_17_WAIT_ACK,

	CAN_BMS_OTA_STEP_18,
	CAN_BMS_OTA_STEP_18_WAIT_ACK,
}CAN_BMS_OTA_STEP;


typedef struct tagNDBMSOTA
{
	unsigned char channel;					//CAN通道
	CAN_BMS_OTA_STEP ota_step;			//当前的升级步骤
	unsigned long seed;						//安全访问的种子

	unsigned char next_drv_bag_flag : 1;
	unsigned char next_app_bag_flag : 1;
	unsigned char drv_data_arrive_flag : 1;
	unsigned char app_data_arrive_flag : 1;

	unsigned int last_send_cmd_time;

	unsigned int drv_check_sum;				//驱动文件checksum
	unsigned int app_check_sum;				//app文件checksum

	unsigned int drv_offset;
	unsigned int app_offset;
	unsigned int drv_tx_len;
	unsigned int app_tx_len;

	unsigned int drv_cur_tx_len;
	unsigned int app_cur_tx_len;

	unsigned int drv_addr;					//驱动地址
	unsigned int drv_len;
	unsigned int drv_bag_id;

	unsigned int app_addr;					//app地址
	unsigned int app_len;
	unsigned int app_bag_id;

	unsigned int transfer_max_len;			//一次最大传输长度

	unsigned char buf[512];
}NDBMSOTA;

NDBMSOTA s_stNDBMSOTA = {0};


unsigned int get_sandian_bms_ota_step(void)
{
	return (unsigned int)(s_stNDBMSOTA.ota_step);
}

void sandian_bms_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len)
{
	memset(&s_stNDBMSOTA,0,sizeof(s_stNDBMSOTA));

	s_stNDBMSOTA.ota_step = (CAN_BMS_OTA_STEP)step;

	s_stNDBMSOTA.drv_addr = drv_addr;
	s_stNDBMSOTA.drv_len = drv_len;
	s_stNDBMSOTA.app_addr = app_addr;
	s_stNDBMSOTA.app_len = app_len;	
}

void ningde_bms_transfer_file_proc(CanRxMsg *pMsg,unsigned int file_type,CAN_MCU_OTA_STEP next_step)
{
}


const unsigned int mask= 0xAAAAAAAA;
unsigned int ComputeKey(unsigned int seed, unsigned int mask)
{
	unsigned char key[4];
	unsigned int retKey;
	unsigned char i=0,j=0;
	unsigned char result=0x00;
	union
	{
		char byte[4];
		unsigned int wort;
	}seedlokal;

	seedlokal.wort = seed;
	if(seedlokal.byte[0]==0&&seedlokal.byte[1]==0)
	{
	/* Do Nothing*/
	}
	else
	{
		for(i=0;i<40;i++)
		{
			if(seedlokal.wort & 0x80000000)
			{
				seedlokal.wort = seedlokal.wort << 1;
				seedlokal.wort = seedlokal.wort ^ mask;
			}
			else
			{
				seedlokal.wort=seedlokal.wort<<1;
			}
		}
		for(i = 0; i < 4; i++)
		{
			result=0x00;
			j=0;
			for(; j<8; j++)
			{
				if(j<4)
				{
					result|=(seedlokal.byte[i]& (0x01 << (7-j) ) ) >> (7-2*j);
				}
				else
				{
					result|=(seedlokal.byte[i]& (0x01 << (7-j) ) ) << (2*j-7);
				}
			}
			key[3 - i] = result;
		}
	}
	retKey =((unsigned int)key[3] << 24) + ((unsigned int)key[2]<<16)+((unsigned int)key[1]<<8)+(unsigned int)key[0];
	return retKey;
}

void ningde_bms_ota_proc(char channel,CanRxMsg *pMsg)
{
	unsigned int canid = 0;
	unsigned char dat[8] = {0};

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}

	if (canid == 0x7C8)
	{
		s_stNDBMSOTA.channel = channel;
	}

	switch (s_stNDBMSOTA.ota_step)
	{
	case CAN_BMS_OTA_STEP_1:
	{
		sandian_tx(0,0x7C0,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		//sandian_tx(1,0x7C0,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		//sandian_tx(2,0x7C0,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_1_WAIT_ACK;
	}
	break;
	case CAN_BMS_OTA_STEP_1_WAIT_ACK:
	case CAN_BMS_OTA_STEP_2:
		if ((canid == 0x7C8) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x03))
		{
			sandian_tx(s_stNDBMSOTA.channel,0x7C0,"\x04\x31\x01\xFF\x02\xFF\xFF\xFF",8);
			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_2_WAIT_ACK;
		}
	break;

	case CAN_BMS_OTA_STEP_2_WAIT_ACK:
	case CAN_BMS_OTA_STEP_3:
		//if ((canid == 0x7C8) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01))
		{
			sandian_tx(s_stNDBMSOTA.channel,0x7C0,"\x02\x85\x02\xFF\xFF\xFF\xFF\xFF",8);
			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_3_WAIT_ACK;
		}
	break;

	case CAN_BMS_OTA_STEP_3_WAIT_ACK:
	case CAN_BMS_OTA_STEP_4:
		if ((canid == 0x7C8) && (pMsg->Data[1] == 0xC5) && (pMsg->Data[2] == 0x02))
		{
			sandian_tx(s_stNDBMSOTA.channel,0x7C0,"\x03\x28\x03\x01\xFF\xFF\xFF\xFF",8);
			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_4_WAIT_ACK;
		}
	break;


	case CAN_BMS_OTA_STEP_4_WAIT_ACK:
	case CAN_BMS_OTA_STEP_5:
		if ((canid == 0x7C8) && (pMsg->Data[1] == 0x68) && (pMsg->Data[2] == 0x03))
		{
			s_stNDBMSOTA.last_send_cmd_time = GetRunTime();

			sandian_tx(s_stNDBMSOTA.channel,0x7C0,"\x02\x10\x02\xFF\xFF\xFF\xFF\xFF",8);
			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_5_WAIT_ACK;
		}
	break;

	case CAN_BMS_OTA_STEP_5_WAIT_ACK:
	case CAN_BMS_OTA_STEP_6:
		if ((canid == 0x7C8) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x02))
		{
			sandian_tx(s_stNDBMSOTA.channel,0x7C0,"\x02\x27\x09\xFF\xFF\xFF\xFF\xFF",8);		//获取安全访问种子
			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_6_WAIT_ACK;
		}

		if ((GetRunTime() - s_stNDBMSOTA.last_send_cmd_time) >= 3)
		{
			//s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_5;
		}

	break;

	case CAN_BMS_OTA_STEP_6_WAIT_ACK:
	case CAN_BMS_OTA_STEP_7:
		if ((canid == 0x7C8) && (pMsg->Data[1] == 0x67) && (pMsg->Data[2] == 0x09))				//发送安全密钥
		{
			unsigned int temp = 0;

			DBG_Print(LEVEL2,"bms seed:%02X %02X %02X %02X\r\n",pMsg->Data[3],pMsg->Data[4],pMsg->Data[5],pMsg->Data[6]);
			s_stNDBMSOTA.seed = (pMsg->Data[3] << 24) + (pMsg->Data[4] << 16) + (pMsg->Data[5] << 8) +pMsg->Data[6];

			temp = ComputeKey(s_stNDBMSOTA.seed,mask);

			dat[0] = 0x06;
			dat[1] = 0x27;
			dat[2] = 0x0A;
			dat[3] = temp >> 24;
			dat[4] = temp >> 16;
			dat[5] = temp >> 8;
			dat[6] = temp;
			dat[7] = 0xFF;

			sandian_tx(s_stNDBMSOTA.channel,0x7C0,dat,8);

			s_stNDBMSOTA.ota_step = CAN_BMS_OTA_STEP_7_WAIT_ACK;
		}
	break;

	default:break;
	}
}












