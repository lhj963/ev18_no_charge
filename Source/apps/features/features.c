#include "features.h"


typedef struct tagFeaturesData
{
	char car_off_flag : 1;
}FeaturesData;

FeaturesData stFeaturesData = {0};


char is_ver_speak_to_me(void *pver)
{
	char *ptr = (char *)pver;
	char *ptemp = 0;
	unsigned int rlen = 0;

	if (ptr == 0) return 0;

	if (strcmp(ptr,APP_VERSION) == 0)
	{
		return 1;
	}

	ptemp = strstr(ptr,"x");
	if (ptemp)
	{
		rlen = ptemp - ptr;
		if (rlen && (memcmp(ptr,APP_VERSION,rlen) == 0))
		{
			return 1;
		}
	}
	else
	{
		ptemp = strstr(ptr,"X");
		if (ptemp)
		{
			rlen = ptemp - ptr;
			if (rlen && (memcmp(ptr,APP_VERSION,rlen) == 0))
			{
				return 1;
			}
		}
	}
	return 0;
}

//一条规则 用[]表示
//例如升级规则 [update:<selfver:BCG_V1.0.1><targetver:BCG_V1.0.2>]
void ftp_cfg_file_arrive(void * p,unsigned int len)
{
	char temp[128] = {0};
	char *ptemp = 0;
	char *psrc = (char *)p;

	DBG_Print(LEVEL0,"ftp config file:\r\n%s\r\n",p);
	while (1)
	{
		ptemp = GetSubString(psrc,"[","]",temp,sizeof(temp));
		if (ptemp)
		{
			//匹配升级规则
			if (memcmp(temp,"update",strlen("update")) == 0)
			{
				char ver[41] = {0};
				if (GetSubString(ptemp,"<selfver:",">",ver,sizeof(ver)))
				{
					//比较自身版本
					if (is_ver_speak_to_me(ver))
					{
						if (GetSubString(ptemp,"<targetver:",">",ver,sizeof(ver)))
						{
							if (strcmp(APP_VERSION,ver))
							{
								strcat(ver,".bin");
								
								DBG_Print(LEVEL0,"ftp auto update:%s\r\n",ver);
								trigger_ftp_update(0,0,0,0,ver,(char)-1);							
							}
						}
					}
					else
					{
						DBG_Print(LEVEL0,"not speak to me.(myself ver:%s speak to:%s)\r\n",
						APP_VERSION,ver);
					}
				}
		
			}

			psrc = ptemp + 2;
		}
		else
		{
			break;
		}
	}
}




void drive_cycle_proc(void)
{
	INT16U cur_rpm = 0;

	static char s_xh_status = 1;		//1表示熄火   0表示点火状态
	static unsigned int s_xh_cnt = 0;

	cur_rpm = can_get_rpm(0);

	if (s_xh_status)		//熄火状态 下判断点火
	{
		//发动机有转速算 按一次驾驶循环算
		if ((cur_rpm > 300) && (cur_rpm < 60000))
		{
			stFeaturesData.car_off_flag = 0;			//未熄火
			s_xh_cnt = 0;
			s_xh_status = 0;
			set_drive_cycle_cnt(get_drive_cycle_cnt() + 1);
		}
	}
	else					//点火状态 下判断熄火
	{
		if (cur_rpm <= 300)
		{
			s_xh_cnt++;
			if (s_xh_cnt >= 10)
			{
				s_xh_cnt = 0;
				s_xh_status = 1;

				stFeaturesData.car_off_flag = 1;
			}
		}
	}

}


#define SEND_DATA_BUF_CNT 3

SendServerDataBuf s_stSendServerDataBuf[SEND_DATA_BUF_CNT] = {0};

void features_init(void)
{
	stFeaturesData.car_off_flag = 1;

	CreateQueue(SEND_DATA_SERVER_NUM,s_stSendServerDataBuf,sizeof(s_stSendServerDataBuf[0]),SEND_DATA_BUF_CNT);

	//上电自动获取FTP配置文件
	//reg_ftp_cfg_file_arrive_callback(ftp_cfg_file_arrive);

}

void send_data_to_server(void)
{
	SendServerDataBuf *p;

	p = QueueGetCurMsg(SEND_DATA_SERVER_NUM);
	if (p)
	{
		DBG_PrintBuf(LEVEL0,"send to server:\r\n",p->ptr,p->len);
		net_send_in_cb(2,p->ptr,p->len);

		free_big_mem(p->ptr);
		QueueMsgDealOk(SEND_DATA_SERVER_NUM);
	}
}




//判断车辆是否熄火
char is_car_off(void)
{
	return stFeaturesData.car_off_flag;
}


void features_proc(void)
{
	static INT32U s_last_time_sec = 0;

	if ((GetRunTime() - s_last_time_sec) >= 1)
	{
		s_last_time_sec = GetRunTime();
		moni_can_data_proc();
	}

	//判断驾驶循环数
	drive_cycle_proc();

}









