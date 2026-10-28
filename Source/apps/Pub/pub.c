#include "pub.h"

static unsigned int s_can_cnt[3] = {0};
static INT8U f_send_1801D0D5_JD = 0;
void check_can_bps_match(unsigned char channel)
{
	if (get_can_bps_match_ok_flag(channel) == 0)
	{
		s_can_cnt[channel]++;
		if (s_can_cnt[channel] >= 3)
		{
			set_can_bps_match_ok_flag(channel,1);
		}
	}
}

void detect_bps(unsigned char channel)
{
	static unsigned char s_can_bps_idx[3] = {0};
	static unsigned int s_cur_can_bps[3] = {250,250,250};
	const unsigned int s_can_bps[] = {500,250};
	unsigned char bps_cnt = (sizeof(s_can_bps) / sizeof(s_can_bps[0]));

	if (channel >= 3)
	{
		return ;
	}

	if (get_can_bps_match_ok_flag(channel) == 0)
	{
		DBG_Print(LEVEL2,"[CAN%u]change can bps:%u\r\n",channel,s_can_bps[s_can_bps_idx[channel]]);

		s_can_cnt[channel] = 0;
		s_cur_can_bps[channel] = s_can_bps[s_can_bps_idx[channel]];
		can_init(channel,s_can_bps[s_can_bps_idx[channel]]);

		s_can_bps_idx[channel]++;
		if (s_can_bps_idx[channel] >= bps_cnt)
		{
			s_can_bps_idx[channel] = 0;
		}
	}
	else
	{
		if ((channel == 0) && (GetCan0Bps() == 0)) SetCan0Bps(s_cur_can_bps[channel]);
		if ((channel == 1) && (GetCan1Bps() == 0)) SetCan1Bps(s_cur_can_bps[channel]);
		if ((channel == 2) && (GetCan2Bps() == 0)) SetCan2Bps(s_cur_can_bps[channel]);
	}
}

void auto_detect_can_bps_proc(void)
{
	static unsigned int s_can_auto_dete_time = 0;
	if ((GetRunTime() - s_can_auto_dete_time) >= 4)
	{
		detect_bps(0);
		detect_bps(1);
		detect_bps(2);
		s_can_auto_dete_time = GetRunTime();
	}

	//report the auto vin
	if (get_auto_vin_ok_flag() && (GetRunTime() > 50))
	{
		static unsigned int s_send_flag = 0;

		if (s_send_flag == 0)
		{
			send_msg(S_MSG_SEND_PARAM_TO_MPU,GetVIN(),(void *)17,0);
			s_send_flag = 1;
			DBG_Print(LEVEL2,"auto get vin success(vin:%s)\r\n",GetVIN());
		}
	}
}

typedef enum tagAutoGetVinStep
{
	AutoGetVinStep_None = 0,
	AutoGetVinStep_Start,
	AutoGetVinStep_WaitStartAck,
	AutoGetVinStep_StartAckOk,
	AutoGetVinStep_GetThreeBagCmd,
	AutoGetVinStep_WaitThreeBag,
	AutoGetVinStep_End,
}AutoGetVinStep;


AutoGetVinStep eAutoGetVinStep = AutoGetVinStep_None;

void set_auto_vin_step(AutoGetVinStep val)
{
	eAutoGetVinStep = val;
}

AutoGetVinStep get_auto_vin_step(void)
{
	return eAutoGetVinStep;
}

void _send_to_can_bus(CanTxMsg *ptxmsg)
{
	unsigned char i = 0;

	for (i = 0; i < 1; i++)
	{
		if (get_can_bps_match_ok_flag(i) && GetCan0Bps())
		{
			can_send_data(i,ptxmsg);
			if (get_recv_send_can_flag()) can_data_in_queue(0,ptxmsg->IDE,ptxmsg->IDE ? ptxmsg->ExtId : ptxmsg->StdId,ptxmsg->Data,8);
		}
	}
}

void send_to_can_bus_18FF01FB(void)
{
	CanTxMsg statusmsg = {0};
	unsigned char status = 0;

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x18FF01FB;
	statusmsg.DLC = 8;

	//bit7 表示连接网络
	status = (1 << 3) | (1 << 4) | (1 << 7);
	statusmsg.Data[0] = status;
	statusmsg.Data[1] = 0;				//S_MSG_GET_GPS_ANT_STATUS
	
	status = get_auto_vin_ok_flag() << 2;
	statusmsg.Data[2] = status;
	statusmsg.Data[3] = (unsigned int)send_msg(S_MSG_GET_SIM_CSQ,0,0,0);
	statusmsg.Data[4] = 0x03;			//product type
	
	statusmsg.Data[7] = 0x06;			//有效卫星数量
	
	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_18FEF3FB(void)
{
	CanTxMsg statusmsg = {0};
	unsigned long lon = 0;
	unsigned long lat = 0;

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x18FEF3FB;
	statusmsg.DLC = 8;
	
	lon = (unsigned long)send_msg(S_MSG_GET_LON,0,0,0);
	lat = (unsigned long)send_msg(S_MSG_GET_LAT,0,0,0);
	
	lon *= 10;
	lon += 2100000000;
	lat *= 10;
	lat += 2100000000;
	
	statusmsg.Data[0] = lat;
	statusmsg.Data[1] = lat >> 8;
	statusmsg.Data[2] = lat >> 16;
	statusmsg.Data[3] = lat >> 24;
	
	statusmsg.Data[4] = lon;
	statusmsg.Data[5] = lon >> 8;
	statusmsg.Data[6] = lon >> 16;
	statusmsg.Data[7] = lon >> 24;
	
	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_18FEE6FB(void)
{
	CanTxMsg statusmsg = {0};
	unsigned char t[6] = {0};

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x18FEE6FB;
	statusmsg.DLC = 8;

	send_msg(S_MSG_GET_RTC_TIME,t,0,0);
	if (is_time_format_ok(t))
	{
		statusmsg.Data[0] = t[5] * 4;
		statusmsg.Data[1] = t[4];
		statusmsg.Data[2] = t[3];
		statusmsg.Data[3] = t[1];
		statusmsg.Data[4] = t[2] * 4;
		statusmsg.Data[5] = t[0] + 15;
		statusmsg.Data[6] = 0;
		statusmsg.Data[7] = 0;
	}
	else
	{
		memcpy(statusmsg.Data,"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",8);
	}

	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_18FEE8FB(void)
{
	CanTxMsg statusmsg = {0};
	INT16U fxj = 0;
	INT16U gaodu = 0;

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x18FEE8FB;
	statusmsg.DLC = 8;

	fxj = (INT32U)send_msg(S_MSG_GET_ANGLE,0,0,0);
	gaodu = (INT32U)send_msg(S_MSG_GET_GAODU,0,0,0);

	//fxj *= 128;
	gaodu *= 8;
	gaodu += (2500 * 8);

	statusmsg.Data[0] = fxj;
	statusmsg.Data[1] = fxj >> 8;
	statusmsg.Data[6] = gaodu;
	statusmsg.Data[7] = gaodu >> 8;

	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_3B1(void)
{
	CanTxMsg statusmsg = {0};

	statusmsg.IDE = CAN_Id_Standard;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0x3B1;
	statusmsg.ExtId = 0;
	statusmsg.DLC = 8;
	
	memset(statusmsg.Data,0xFF,8);
	statusmsg.Data[0] = 0xF8;
	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_3B2(void)
{
	CanTxMsg statusmsg = {0};
	unsigned long lon = 0;
	unsigned long lat = 0;

	statusmsg.IDE = CAN_Id_Standard;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0x3B2;
	statusmsg.ExtId = 0;
	statusmsg.DLC = 8;
	
	lon = (unsigned long)send_msg(S_MSG_GET_LON,0,0,0);
	lat = (unsigned long)send_msg(S_MSG_GET_LAT,0,0,0);
	lon *= 10;
	lon += 2100000000;
	lat *= 10;
	lat += 2100000000;
	
	statusmsg.Data[0] = lat;
	statusmsg.Data[1] = lat >> 8;
	statusmsg.Data[2] = lat >> 16;
	statusmsg.Data[3] = lat >> 24;
	
	statusmsg.Data[4] = lon;
	statusmsg.Data[5] = lon >> 8;
	statusmsg.Data[6] = lon >> 16;
	statusmsg.Data[7] = lon >> 24;
	_send_to_can_bus(&statusmsg);
}

void send_to_can_bus_3B3(void)
{
	CanTxMsg statusmsg = {0};
	unsigned char t[6] = {0};

	statusmsg.IDE = CAN_Id_Standard;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0x3B3;
	statusmsg.ExtId = 0;
	statusmsg.DLC = 8;
	send_msg(S_MSG_GET_RTC_TIME,t,0,0);
	statusmsg.Data[0] = t[5] * 4;
	statusmsg.Data[1] = t[4];
	statusmsg.Data[2] = t[3];
	statusmsg.Data[3] = t[1];
	statusmsg.Data[4] = t[2] * 4;
	statusmsg.Data[5] = t[0] + 15;
	_send_to_can_bus(&statusmsg);
}


void _send_to_can_bus_18X1D0D5(INT32U id)
{
	CanTxMsg statusmsg = {0};
	static INT8U s_life = 0x01;
	static INT8U bind_continue_time = 0;
	
	f_send_1801D0D5_JD = 1;//限速
	
	if(get_want_bind_status() == 2)
	{
		bind_continue_time++;
		if((get_fact_bind_status()==0) && (bind_continue_time>15))
		{
			set_want_bind_status(0);
			bind_continue_time = 0;
		}
	}
	
	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = id;
	statusmsg.DLC = 8;

	statusmsg.Data[0] = (get_want_pro_ver_about_lock() << 2) + get_want_lock_status();
	statusmsg.Data[1] = 0xFF;
	statusmsg.Data[2] = (get_want_bind_status() & 0x03) | (get_dst_want_bms_charge_flag() << 2);
	//if (get_want_bind_status() == 1)
	if (1)
	{
		statusmsg.Data[3] = 0x1A;
		statusmsg.Data[4] = 0x23;
		statusmsg.Data[5] = 0x00;
		statusmsg.Data[6] = 0x00;
	}
	else
	{
		unsigned char *p_iccid_head = 0;

		p_iccid_head = get_iccid_head_bytes_in_ecu();

		statusmsg.Data[3] = p_iccid_head[0];
		statusmsg.Data[4] = p_iccid_head[1];
		statusmsg.Data[5] = p_iccid_head[2];
		statusmsg.Data[6] = p_iccid_head[3];
	}

	statusmsg.Data[7] = s_life;
	s_life++;
	if (s_life == 0)
	{
		s_life = 1;
	}

	can_send_data(get_lock_car_can_channel(),&statusmsg);
	if (get_recv_send_can_flag())
	{
		can_data_in_queue(get_lock_car_can_channel(),statusmsg.IDE,(statusmsg.IDE == CAN_Id_Extended) ? statusmsg.ExtId : statusmsg.StdId,statusmsg.Data,8);
	}
}

static int send_18X4D0D5_times = 0;

int get_send_18X4D0D5_times(void)
{
	return send_18X4D0D5_times;
}

void set_send_18X4D0D5_times(int temp)
{
	send_18X4D0D5_times = temp;
}

void _send_to_can_bus_18X4D0D5(INT32U id)
{
	CanTxMsg statusmsg = {0};
	unsigned char iccid[21] = {0};
	INT8U *piccid = 0;

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = id;
	statusmsg.DLC = 8;

	send_msg(S_MSG_GET_ICCID,iccid,(void *)20,0);

	if( (get_send_18X4D0D5_times() > 0) && (get_want_bind_status() == 1))				//希望绑定的时候 填真实的ICCID 其他时候填VCU返回的ICCID
	{
		iccid_str_to_8byte(iccid,statusmsg.Data);
		send_18X4D0D5_times--;
	}
	else
	{
		piccid = get_iccid_bytes_in_ecu();
		memcpy(statusmsg.Data,piccid,8);
	}

	can_send_data(get_lock_car_can_channel(),&statusmsg);	
	if (get_recv_send_can_flag())
	{
		can_data_in_queue(get_lock_car_can_channel(),statusmsg.IDE,(statusmsg.IDE == CAN_Id_Extended) ? statusmsg.ExtId : statusmsg.StdId,statusmsg.Data,8);
	}
}


void dst_send_to_can_bus_1801D0D5(void)
{
	CanTxMsg statusmsg = {0};
	static INT8U s_dst_life = 0x01;

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x1801D0D5;
	statusmsg.DLC = 8;

	statusmsg.Data[0] = (get_dst_want_pro_ver_about_lock() << 2) | get_dst_want_lock_status() | (get_dst_want_limit_speed_level() << 4) | (get_dst_want_kucunrong_limit_speed() << 6);
	statusmsg.Data[1] = (get_dst_pay_tip_flag() << 4) | get_dst_keep_fit_tip_flag();
	if (get_dst_no_auto_lock_when_meet_net_error_condition_flag())				//不自动锁车标志置起来了就不能发送网络异常标志报文
	{
		statusmsg.Data[2] = (get_dst_want_bms_charge_flag() << 2) | (get_dst_inspect_tip_flag() << 5);
	}
	else
	{
		statusmsg.Data[2] = (get_dst_want_bms_charge_flag() << 2) | (get_dst_network_error_tip_flag() << 4) | (get_dst_inspect_tip_flag() << 5);
	}
	statusmsg.Data[3] = 0x1A;
	statusmsg.Data[4] = 0x23;
	statusmsg.Data[5] = 0x00;
	statusmsg.Data[6] = 0x00;

	statusmsg.Data[7] = s_dst_life;
	s_dst_life++;
	if (s_dst_life == 0)
	{
		s_dst_life = 1;
	}
	_send_to_can_bus(&statusmsg);
}


void send_to_can_bus_1811D0D5(void)
{
	_send_to_can_bus_18X1D0D5(0x1811D0D5);
}

void send_to_can_bus_1801D0D5(void)
{
	_send_to_can_bus_18X1D0D5(0x1801D0D5);
}

void send_to_can_bus_1814D0D5(void)
{
	_send_to_can_bus_18X4D0D5(0x1814D0D5);
}

void send_to_can_bus_1804D0D5(void)
{
	_send_to_can_bus_18X4D0D5(0x1804D0D5);
}

//东风基础版本锁车逻辑 1秒执行一次
void df_base_lock_car_proc(void)
{
	static INT8U flag_first_tbox_ON = 0x01;
	if (get_electric_car_class() && get_iccid_head_flag_in_ecu())
	{
		unsigned char *p_iccid_head = 0;

		p_iccid_head = get_iccid_head_bytes_in_ecu();
		//iccid的前四位 8986 转换成四字节 0000231A	 不是8986就说明未绑定ICCID
		if ((p_iccid_head[0] != 0x1A) || (p_iccid_head[1] != 0x23) || (p_iccid_head[2] != 0x00) ||(p_iccid_head[3] != 0x00))
		{
			set_want_pro_ver_about_lock(0);
			set_fact_bind_status(0);
		}
	}

	if ((get_lock_car_cmd_from_where() != 1) && (get_lock_car_cmd_from_where() != 2))
	{
		unsigned char iccid[21] = {0};
		send_msg(S_MSG_GET_ICCID,iccid,(void *)20,0);
		if ((get_iccid_head_flag_in_ecu()) && ((is_string(iccid,20)) || (GetRunTime() > 50)))
		{

			if(flag_first_tbox_ON)
			{
				if((get_want_bind_status() == 0x01) && (get_fact_bind_status() != 0x01))
				{
					set_send_18X4D0D5_times(20);
				}
				flag_first_tbox_ON = 0;
			}
			
			if (get_electric_car_class() == 1)
			{
				send_to_can_bus_1801D0D5();
				send_to_can_bus_1804D0D5();
			}
			else if (get_electric_car_class() == 2)
			{
				send_to_can_bus_1811D0D5();
				send_to_can_bus_1814D0D5();
			}
		}
	}

	if (get_lock_car_cmd_from_where() == 1) 			//锁车命令来自东风平台
	{
		unsigned char iccid[21] = {0};
		send_msg(S_MSG_GET_ICCID,iccid,(void *)20,0);
		if ((get_iccid_head_flag_in_ecu()) && ((is_string(iccid,20)) || (GetRunTime() > 50)))
		{
			if(flag_first_tbox_ON)
			{
				if((get_want_bind_status() == 0x01) && (get_fact_bind_status() != 0x01))
				{
					set_send_18X4D0D5_times(20);
				}
				flag_first_tbox_ON = 0;
			}
			if (get_electric_car_class() == 1)
			{
				send_to_can_bus_1801D0D5();
				send_to_can_bus_1804D0D5();
			}
			else if (get_electric_car_class() == 2)
			{
				send_to_can_bus_1811D0D5();
				send_to_can_bus_1814D0D5();
			}
		}
	}
}

#define LOW_VOL_THRESHOLD				11000		//低压检测阈值
#define BMS_BD_VOL_THRESHOLD 			12000		//智能补电阈值

void send_to_can_bus_1801D0D5_JD(void)
{
	static INT8U life = 0;
	CanTxMsg statusmsg = {0};
	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x1801D0D5;
	statusmsg.DLC = 8;
	
	life++;
	if(life == 0)
	{
		life++;
	}
	statusmsg.Data[0] = 0x00;
	statusmsg.Data[1] = 0xFF;
	statusmsg.Data[2] = 0x02;
	statusmsg.Data[7] = life;

	can_send_data(0,&statusmsg);	
}
extern void DFGF_set_1811D4F5(uint8_t * p_data);
void send_to_can_bus_1811D4F5(void)
{
	static INT8U life = 0;
	CanTxMsg statusmsg = {0};
	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x1811D4F5;
	statusmsg.DLC = 8;
	
	life++;
	if(life == 0)
	{
		life++;
	}

	if((get_want_startup_status() == 1) && (get_bcm_online_status()))
	{
		if(get_want_lockdoor_status())
		{
			statusmsg.Data[1] = 0x01;//车门解锁
		}
		else
		{
			statusmsg.Data[1] = 0x02;//车门闭锁
		}
	}
	
	DFGF_set_1811D4F5(statusmsg.Data);

	statusmsg.Data[4] = get_want_startup_status() & 0x03;
	statusmsg.Data[7] = life;

	can_send_data(0,&statusmsg);	
}

void send_to_can_bus_1815D4F5(void)
{
	static INT8U life = 0;
	CanTxMsg statusmsg = {0};
	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x1815D4F5;
	statusmsg.DLC = 8;
	
	life++;
	if(life == 0)
	{
		life++;
	}

	statusmsg.Data[7] = life;

	can_send_data(0,&statusmsg);	
}

void ev18_lockdoor_proc(void)
{
	send_to_can_bus_1811D4F5();
}

//检测补电功能
void ev18_check_bms_charge_proc(void)
{
	static unsigned char s_fact_bms_not_charge_cnt = 0;
	static unsigned char s_start_charge_cnt = 0;
	static unsigned char s_stop_charge_cnt = 0;
	static unsigned char s_cmp_cnt = 0;
	static unsigned int s_bms_charge_time = 0;
	unsigned int main_power_vol = 0;

	if (get_dst_want_bms_charge_flag() == 0)			//已经发了智能补电请求  就要取消限制 使能检测停止补电请求
	{
		s_fact_bms_not_charge_cnt = 0;

		send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
	
		//ACC开的情况下不允许智能补电请求
		if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0))
		{
			return ;
		}
		
		//定时唤醒的时候进行检测
		if (GetRunTime() > 90)
		{
			return ;
		}
	}

	main_power_vol = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
	if (get_dst_fact_bms_charge_flag())
	{
		s_bms_charge_time++;

		//情况一:充电大于3小时 停止补电
		if (s_bms_charge_time >= (3 * 3600))
		{
			set_dst_want_bms_charge_flag(0);
			send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
		}

		//情况二:主电源电压大于13.8V且补电时长大于30分钟 停止补电
		if (main_power_vol > 13800)
		{
			s_stop_charge_cnt++;
			if (s_stop_charge_cnt >= 10)
			{
				s_stop_charge_cnt = 0;

				if (s_bms_charge_time >= (30 * 60))
				{
					set_dst_want_bms_charge_flag(0);
					send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
				}
			}
		}
		else
		{
			s_stop_charge_cnt = 0;
		}

		//情况三:车门、车锁开了 结束补电
		if ((get_dst_chemen_switch() == 1) || (get_dst_chesuo_switch() == 1) || (get_dst_left_qian_chesuo_switch() == 1))
		{
			set_dst_want_bms_charge_flag(0);
			send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
		}

		//情况四:充电电压越充电压越小 判断为DCDC故障 结束补电
		if (get_dst_budian_init_vol() >= (1000 + main_power_vol))
		{
			s_cmp_cnt++;
			if (s_cmp_cnt >= 7)
			{
				s_cmp_cnt = 0;
				set_dst_want_bms_charge_flag(0);
				send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
			}
		}
		else
		{
			s_cmp_cnt = 0;
		}
	}
	else
	{
		//检测到主电源电压低于11V 车门关闭、车锁 三个条件同时满足 开启智能补电功能
		if (main_power_vol <= BMS_BD_VOL_THRESHOLD)
		{
			s_start_charge_cnt++;
			if (s_start_charge_cnt >= 10)
			{
				s_start_charge_cnt = 0;

				if ((get_dst_chemen_switch() == 0) && (get_dst_chesuo_switch() == 0) && (get_dst_left_qian_chesuo_switch() == 0))
				{
					send_msg(S_MSG_OUT_IO_CTRL,0,(void *)1,0);
					
					if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0) == 0)
					{
						set_dst_want_bms_charge_flag(1);
						set_dst_budian_init_vol(main_power_vol);
					}
				}
			}
		}
		else
		{
			s_start_charge_cnt = 0;
		}

		//VCU退出补电 TBOX也要发送退出报文
		if (get_dst_want_bms_charge_flag())
		{
			s_fact_bms_not_charge_cnt++;

			if (s_fact_bms_not_charge_cnt >= 8)
			{
				s_fact_bms_not_charge_cnt = 0;
				set_dst_want_bms_charge_flag(0);
				send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
			}
		}
		else
		{
			s_fact_bms_not_charge_cnt = 0;
			send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
		}

	}
}



void can_send_proc(void)
{
	static unsigned int s_can_send_time1 = 0;
	static unsigned int s_can_send_time2 = 0;
	static INT8U lockdoor_count = 0;
	static INT8U f_send_1811D4F5 = 0;

	//向CAN总线发送自定义的状态数据
	if (get_can_test_mode() == 1)
	{
		send_status_to_canbus();
	}

	if ((get_auto_vin_ok_flag() == 0) && 
		(get_can_bps_match_ok_flag(0) 
		|| get_can_bps_match_ok_flag(1) 
		|| get_can_bps_match_ok_flag(2)
		)
		)
	{
		static unsigned int s_can_send_time = 0;

		if ((GetRunTime() - s_can_send_time) >= 6)
		{
			//set_auto_vin_step(AutoGetVinStep_Start);
			//DBG_Print(LEVEL2,"start auto get vin\r\n");
			s_can_send_time = GetRunTime();
		}
	}

	//send get vin cmd
	switch (get_auto_vin_step())
	{
	case AutoGetVinStep_Start:
	{
		CanTxMsg txmsg = {0,0x18EA0021,CAN_ID_EXT,CAN_RTR_DATA,3,{0xEC,0xFE,0x00,0,0,0,0,0}};
		//CanTxMsg txmsgqc = {0,0x18DA00F1,CAN_ID_EXT,CAN_RTR_DATA,4,{0x03,0x22,0xF8,0x02,0,0,0,0}};

		eAutoGetVinStep = AutoGetVinStep_WaitStartAck;

		_send_to_can_bus(&txmsg);
	}
	break;

	case AutoGetVinStep_WaitStartAck:

	break;

	case AutoGetVinStep_StartAckOk:
	case AutoGetVinStep_GetThreeBagCmd:
	{
		CanTxMsg txmsg = {0,0x18EC0021,CAN_ID_EXT,CAN_RTR_DATA,3,{0x11,0x03,0x01,0xFF,0xFF,0xEC,0xFE,0}};

		eAutoGetVinStep = AutoGetVinStep_WaitThreeBag;
		_send_to_can_bus(&txmsg);
	}
	break;

	case AutoGetVinStep_WaitThreeBag:

	break;

	case AutoGetVinStep_End:
	{
		CanTxMsg txmsg = {0,0x18EC0021,CAN_ID_EXT,CAN_RTR_DATA,3,{0x13,0x12,0x00,0x04,0xFF,0xEC,0xFE,0}};

		_send_to_can_bus(&txmsg);
		eAutoGetVinStep = AutoGetVinStep_None;
	}
	break;
	default:break;
	}

	if (get_dst_want_bms_charge_flag())		//达到了智能补电的条件 发送智能补电的报文
	{
		if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0))
		{
			dst_send_to_can_bus_1801D0D5();			
		}
		else
		{
			send_to_can_bus_1801D0D5();
		}
	}
	else
	{
		//地上铁锁车报文
		if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0)) dst_lock_car_proc();
	}

	//send the tbox status to can bus
	if ((GetRunTime() - s_can_send_time1) >= 1)
	{
		//DBG_Print(LEVEL0,"GetRunTime:%d \r\n",GetRunTime());
		//检测自动补电功能
		if (get_engine_type() == EngineType_EV18)
		{
			//ev18_check_bms_charge_proc();
		}

		if (get_dst_want_bms_charge_flag() == 0)
		{
			df_base_lock_car_proc();
		}

		//如果刷的不是地上铁的平台 需要清除命令来自地上铁的标志
		if ((GetRunTime() > 45) && (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0) == 0) && 
			(send_msg(S_MSG_GET_SERVER_PORT,(void *)0,0,0) || send_msg(S_MSG_GET_SERVER_PORT,(void *)2,0,0))
			)
		{
			if (get_lock_car_cmd_from_where() == 2)
			{
				set_lock_car_cmd_from_where(0);
			}
		}

		//燃油车
		switch (get_ryc_engine_type())
		{
		case EngineType_ZD30:
			send_to_can_bus_3B1();
			send_to_can_bus_3B2();
			send_to_can_bus_3B3();
		break;
		case EngineType_M9T:
		case EngineType_YC:
		case EngineType_QC:
		case EngineType_CC:
		case EngineType_KMS:
			if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0))				//ACC关的情况下不发送
			{
				send_to_can_bus_18FF01FB();
				send_to_can_bus_18FEF3FB();
				//send_to_can_bus_18FEE8FB();
			}
		break;

		default:break;
		}

		if (get_engine_type() == EngineType_EV18)
		{
			if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0))				//ACC关的情况下不发送
			{
				send_to_can_bus_18FF01FB();
				send_to_can_bus_18FEF3FB();
				send_to_can_bus_18FEE8FB();
			}
		}

		s_can_send_time1 = GetRunTime();
	}

	if ((GetRunTime() - s_can_send_time2) >= 5)
	{
		switch (get_ryc_engine_type())
		{
		case EngineType_M9T:
		case EngineType_YC:
		case EngineType_QC:
		case EngineType_CC:
		case EngineType_KMS:
			if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0)) send_to_can_bus_18FEE6FB();
		break;
		default:break;
		}

		if (get_engine_type() == EngineType_EV18)
		{
			if (send_msg(S_MSG_GET_ACC_STATUS,0,0,0)) send_to_can_bus_18FEE6FB();
		}

		//发送信息给仪表
		send_info_to_yibiao();

		s_can_send_time2 = GetRunTime();
	}

	lockdoor_count++;
	if(lockdoor_count  > 4)
	{
		//if (get_engine_type() == EngineType_EV18)
		{
			if((send_msg(S_MSG_GET_ACC_STATUS,0,0,0)) || (f_send_1811D4F5  && (get_acc_no_data_time() < 40)))
			{
				ev18_lockdoor_proc();
				f_send_1811D4F5 = 1;
			}
		}
		lockdoor_count=0;
		//DBG_Print(LEVEL2,"get_want_startup_status:%d \r\n",get_want_startup_status());
	}

	if(f_send_1801D0D5_JD)
	{
		send_to_can_bus_1801D0D5_JD();
	}

}

void analyze_can_vin(unsigned char channel,CanRxMsg *prxmsg)
{
	INT32U canid = 0;
	INT8U *pdat = 0;
	char *pvin = GetVIN();
	unsigned int i = 0;

	if (prxmsg == 0)
	{
		return ;
	}

	//return ;

	pdat = prxmsg->Data;
	canid = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;

	//DBG_Print(0,"id:%08X\r\n",canid);
	//DBG_PrintBuf(0,0, pdat,8);

	//EM26
	if ((canid == 0x0566) && pdat[0])
	{
		for (i = 0; i < 6; i++)
		{
			can_set_vin(0,((pdat[0] - 1) * 6) + i,pdat[i + 1]);
		}
	}

	//D78只解析EV CAN上的VIN   CAN0接EV CAN  接线接错了会导致VIN解析出错
	if (channel == 0)
	{
		if (canid == 0x0492)
		{
			for (i = 0; i < 6; i++)
			{
				can_set_vin(0,3 + i,pdat[i]);
			}
		}
		else if (canid == 0x04B2)
		{
			for (i = 0; i < 8; i++)
			{
				can_set_vin(0,9 + i,pdat[i]);
			}
		}
		else if (canid == 0x04D2)
		{
			can_set_vin(0,0,pdat[2]);
			can_set_vin(0,1,pdat[1]);
			can_set_vin(0,2,pdat[0]);
		
			pvin[0] = pdat[2];
			pvin[1] = pdat[1];
			pvin[2] = pdat[0];
		}
	}

	//EV18/DFAC15/DFAC16
	if (canid == 0x0C0728D0)
	{
		for (i = 0; i < 8; i++)
		{
			can_set_vin(0,i + 9,pdat[7 - i]);
		}
	}
	else if (canid == 0x0C0828D0)
	{
		for (i = 0; i < 8; i++)
		{
			can_set_vin(0,i + 1,pdat[7 - i]);
		}
	}
	else if (canid == 0x0C0928D0)
	{
		can_set_vin(0,0,pdat[0]);
	}

	if (get_auto_vin_step() == AutoGetVinStep_WaitStartAck)
	{
		if ((canid == 0x18EC2100) && (pdat[0] == 0x10))
		{
			set_auto_vin_step(AutoGetVinStep_StartAckOk);
		}
	}
	else if (get_auto_vin_step() == AutoGetVinStep_WaitThreeBag)
	{
		if (canid == 0x18EB2100)
		{
			if ((pdat[0] >= 1) && (pdat[0] <= 3))
			{
				for (i = 0; i < 7; i++)
				{
					if (((pdat[0] - 1) * 7 + i) < 17)
					{
						can_set_vin(0,(pdat[0] - 1) * 7 + i,pdat[i + 1]);
						can_set_vin(1,(pdat[0] - 1) * 7 + i,pdat[i + 1]);
						can_set_vin(2,(pdat[0] - 1) * 7 + i,pdat[i + 1]);

						pvin[(pdat[0] - 1) * 7 + i] = pdat[i + 1];
					}
				}

				if (pdat[0] == 0x03)
				{
					//vin recv finish
					set_auto_vin_step(AutoGetVinStep_End);
				}
			}
		}
	}

}


#define MAX_RECV_CAN_BUF_CNT 500
unsigned char s_aucRecvCanData[MAX_RECV_CAN_BUF_CNT][17] = {0};
unsigned int s_can_ridx = 0;
unsigned int s_can_widx = 0;

void can_data_in_queue(unsigned char channel,unsigned char is_extid,unsigned int id,void *pdat,unsigned char datlen)
{
	unsigned short ms = get_ms_mod();
	unsigned char *pt = get_rtc_time_ptr();

	//分/秒/毫秒
	s_aucRecvCanData[s_can_widx][0] = pt[4];
	s_aucRecvCanData[s_can_widx][1] = pt[5];
	s_aucRecvCanData[s_can_widx][2] = ms >> 8;
	s_aucRecvCanData[s_can_widx][3] = ms;

	if (is_extid)
	{
		s_aucRecvCanData[s_can_widx][4] = channel | (1 << 2);
	}
	else
	{
		s_aucRecvCanData[s_can_widx][4] = channel;
	}
	s_aucRecvCanData[s_can_widx][5] = id >> 24;
	s_aucRecvCanData[s_can_widx][6] = id >> 16;
	s_aucRecvCanData[s_can_widx][7] = id >> 8;
	s_aucRecvCanData[s_can_widx][8] = id;
	memcpy(&(s_aucRecvCanData[s_can_widx][9]),pdat,datlen);

	s_can_widx++;
	if (s_can_widx >= MAX_RECV_CAN_BUF_CNT)
	{
		s_can_widx = 0;
	}
}

void store_can_msg(void)
{
	static unsigned char s_buf[1500 + 32] = {0};
	unsigned int widx = s_can_widx;
	CanRxMsg rxmsg = {0};
	unsigned int rlen = 0;
	unsigned int canid = 0;
	unsigned char is_extid = 0;
	unsigned char channel = 0;
	unsigned char *pt = get_rtc_time_ptr();

	s_buf[rlen++] = pt[0];
	s_buf[rlen++] = pt[1];
	s_buf[rlen++] = pt[2];
	s_buf[rlen++] = pt[3];

	while (s_can_ridx != widx)
	{
		memcpy(s_buf + rlen,s_aucRecvCanData[s_can_ridx],17);
		rlen += 17;

		channel = s_aucRecvCanData[s_can_ridx][4] & 0x03;
		is_extid = s_aucRecvCanData[s_can_ridx][4] & (1 << 2);
		canid = (s_aucRecvCanData[s_can_ridx][5] << 24) + (s_aucRecvCanData[s_can_ridx][6] << 16) +
				(s_aucRecvCanData[s_can_ridx][7] << 8) + s_aucRecvCanData[s_can_ridx][8];
		if (is_extid)
		{
			rxmsg.IDE = CAN_ID_EXT;
			rxmsg.StdId = 0;
			rxmsg.ExtId = canid;
		}
		else
		{
			rxmsg.IDE = CAN_ID_STD;
			rxmsg.StdId = canid;
			rxmsg.ExtId = 0;
		}
		rxmsg.DLC = 8;
		rxmsg.RTR = CAN_RTR_DATA;
		memcpy(rxmsg.Data,&(s_aucRecvCanData[s_can_ridx][9]),8);

		//CAN原始报文上报服务器
		if (is_in_catch_canid_list(canid))
		{
			report_to_server_temp(channel,&rxmsg);
		}

		//解析
		check_can_bps_match(channel);
		set_can_no_data_time(0);
		set_can_data_arrive_flag(channel,1);

		gb32960_analyze(channel,&rxmsg);
		//j1939_analysis(0,&rxmsg);
		uds_analysis(0,&rxmsg);
		analyze_can_vin(channel,&rxmsg);
		san_dian_ota_analyze(channel,&rxmsg);
		uds_server_proc(channel,&rxmsg);
		dfgf_sy_analyze(channel,&rxmsg);

		if ((rxmsg.StdId == 0x665) && (memcmp(rxmsg.Data,"BYSDTEST",8) == 0))
		{
			set_can_test_mode(1);
		}

		s_can_ridx++;
		if (s_can_ridx >= MAX_RECV_CAN_BUF_CNT)
		{
			s_can_ridx = 0;
		}

		if (rlen >= 1500)
		{
			break;
		}
	}

	//当发生报警时 自动开启CAN报文录像
	if (gb32960_can_get_sos_flag() || gb32960_can_get_max_sos_level() || (gb32960_can_get_car_status() == 0xFE))
	{
		//set_collect_can_sw(1);
	}

	if (get_collect_can_sw())
	{
		if (rlen > 4)
		{
			send_msg(S_MSG_STORE_CAN_DATA,s_buf,(void *)rlen,0);
		}
	}
}


void can0_hook(CanRxMsg *prxmsg)
{
	INT32U canid = 0;

	stat_can_rx_fps(0); 			//统计桢率
	can_data_in_queue(0,prxmsg->IDE,prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId,prxmsg->Data,8);

	//流控制回复
	canid = (prxmsg->IDE == CAN_Id_Extended) ? prxmsg->ExtId : prxmsg->StdId;
	if ((canid == 0x7E8) || (canid == 0x18DAF100))
	{
		if ((prxmsg->Data[0] & 0xF0) == 0x10)
		{
			//set_need_send_flow_ctrl_flag(1);
			//iso27145_can_send_flow_ctrl_proc();
		}
	}

	//sandian_flow_ctrl_ack(0,canid,prxmsg->Data,8);
}

void can1_hook(CanRxMsg *prxmsg)
{
	stat_can_rx_fps(1);
	can_data_in_queue(1,prxmsg->IDE,prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId,prxmsg->Data,8);

	//sandian_flow_ctrl_ack(1,prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId,prxmsg->Data,8);
}

void can2_hook(CanRxMsg *prxmsg)
{
	stat_can_rx_fps(2);
	can_data_in_queue(2,prxmsg->IDE,prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId,prxmsg->Data,8);
	//sandian_flow_ctrl_ack(2,prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId,prxmsg->Data,8);
}

void one_ms_hook(void)
{
	static unsigned int s_s_time = 0;

	s_s_time++;
	if (s_s_time >= 100)
	{
		dst_stat_charge_dl();
		s_s_time = 0;
	}
	inc_ms_mod();
}

void led_proc(void)
{
	if (get_can_data_arrive_flag(0) || get_can_data_arrive_flag(1) || get_can_data_arrive_flag(2))				//总线上有CAN数据闪烁
	{
		static unsigned char s_can_led = 0;
		static unsigned char s_led_cnt = 0;
		static unsigned char s_led_cnt1 = 0;

		if (get_collect_can_sw())		//开启CAN报文录像功能 速闪表明正在进行CAN报文录像
		{
			s_led_cnt1++;
			if (s_led_cnt1 >= 4)
			{
				s_led_cnt1 = 0;
				s_can_led = !s_can_led;
			}
		}
		else
		{
			s_led_cnt++;
			if (s_led_cnt >= 15)
			{
				s_led_cnt = 0;
				s_can_led = !s_can_led;
			}
		}
		send_msg(S_MSG_CAN_LED_CTRL,(void *)s_can_led,0,0);
	}
	else if (get_can_bps_match_ok_flag(0) || get_can_bps_match_ok_flag(1) || get_can_bps_match_ok_flag(2))		//有一路CAN波特率匹配成功就亮灯
	{
		send_msg(S_MSG_CAN_LED_CTRL,(void *)1,0,0);
	}
	else
	{
		send_msg(S_MSG_CAN_LED_CTRL,0,0,0);
	}
}

void disp_dbg_info(void)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int cur_vol = 0;

	ptr = os_malloc(1536);
	if (ptr)
	{
		cur_vol = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
		rlen = snprintf((char *)ptr,1500,
				"车型:%s 自动获取VIN:%s\n"
				"地上铁信号[门:%s 锁:%s ACC:%s] TBOX状态:%s\n"
				"充电电量:%u(单位:0.01kwh) CAN总线:%s\n"
				"VCU版本:%s\n"
				"锁车版本:%u 锁车状态:%s 限速等级:%d 库存限速:%d\n"
				"补电状态:%s 起始补电电压:%umv 当前电压:%umv\n"
				"VCU是否允许补电:%s\n"

				"三电VCU版本:%s\n"
				"三电MCU版本:%s\n"
				"三电BMS版本:%s\n"
				"step:%u 进度:%u/100\n"
				,

				get_engine_type_str(get_engine_type()),
				can_get_vin_ptr(0),
				get_dst_chemen_switch() ? "开" : "关",
				get_dst_left_qian_chesuo_switch() ? "开" : "关",
				get_dst_chesuo_switch() ? "ON" : "OFF",

				get_dst_tbox_work_status_tip(),
				_get_dst_charge_dl(),

				(get_can_no_data_time() > 3) ? "无数据" : "有数据",
				get_dst_vcu_ver_ptr(),
				get_dst_fact_pro_ver_about_lock(),
				get_dst_fact_lock_status() ? "限速" : "未限速",
				get_dst_fact_lock_status() ? (get_dst_fact_limit_speed_level() + 1) : get_dst_fact_limit_speed_level(),
				get_dst_fact_kucunrong_limit_speed(),

				get_dst_want_bms_charge_flag() ? "补电中" : "未补",get_dst_budian_init_vol(),cur_vol,
				get_dst_fact_bms_charge_flag() ? "允许" : "不允许",

				get_vcu_ver_ptr(),get_mcu_ver_ptr(),get_bms_ver_ptr(),

				get_sandian_mcu_ota_step(),sandian_get_upgrade_progress()
				);

		send_msg(S_MSG_DBG_DISP,ptr,(void *)rlen,0);

		os_free(ptr);
	}
}

DataStreamInfo dsi_buf[10] = {0};
OBDInfo obdinfo = {0};

void collect_17691_data(void)
{
	static unsigned int bagcnt = 0;
	static unsigned char s_port_cnt = 0;
	unsigned int rlen = 0;
	unsigned char *pbuf = 0;
	unsigned char channel = 0;
	unsigned char time[6] = {0};
	unsigned char gps_sta = 0;
	INT32U lon = 0;
	INT32U lat = 0;

	send_msg(S_MSG_GET_RTC_TIME,time,0,0);
	lon = (INT32U)send_msg(S_MSG_GET_LON,0,0,0);
	lat = (INT32U)send_msg(S_MSG_GET_LAT,0,0,0);
	
	memcpy(dsi_buf[bagcnt].BCDTime,time,6);

	if (1)
	{

	}
	else
	{
		gps_sta |= 0x01;
	}
	if (0)
	{
		gps_sta |= (1 << 1);
	}
	if (0)
	{
		gps_sta |= (1 << 2);
	}

	dsi_buf[bagcnt].ucGpsStatus = gps_sta;
	dsi_buf[bagcnt].ulLat = lat;
	dsi_buf[bagcnt].ulLong = lon;

	dsi_buf[bagcnt].usSpeed = can_get_speed(channel);
	
	dsi_buf[bagcnt].uckPa = can_get_kpa(channel);
	dsi_buf[bagcnt].cActualTorquePercent = can_get_actualtorquepercent(channel);
	dsi_buf[bagcnt].cFrictionTorquePercent = can_get_frictiontorquepercent(channel);
	dsi_buf[bagcnt].usRPM = can_get_rpm(channel);
	dsi_buf[bagcnt].usFuelFlow = can_get_fuelflow(channel);
	dsi_buf[bagcnt].sUpperNOx = can_get_upperNOx(channel);
	dsi_buf[bagcnt].sLowerNOx = can_get_lowerNOx(channel);
	dsi_buf[bagcnt].ucRemain = can_get_remain(channel);
	dsi_buf[bagcnt].usIntakeVolume = can_get_intakevolume(channel);
	dsi_buf[bagcnt].sSCRInletTemperature = can_get_SCRInletTemperature(channel);
	dsi_buf[bagcnt].sSCROutletTemperature = can_get_SCROutletTemperature(channel);
	dsi_buf[bagcnt].usDPF = can_get_DPF(channel);
	dsi_buf[bagcnt].cCoolantTemperature = can_get_coolanttemperature(channel);
	dsi_buf[bagcnt].ucLevel = can_get_oilboxlevel(channel);
	dsi_buf[bagcnt].ulMilage = can_get_milage(channel);
	
	//dsi_buf[bagcnt].ulMilage = 10000;
	//dsi_buf[bagcnt].usSpeed = 10 << 8;
	//dsi_buf[bagcnt].uckPa = 200;
	//dsi_buf[bagcnt].cFrictionTorquePercent = 128;

	bagcnt++;
	if (bagcnt >= 10)
	{
		pbuf = os_malloc(1536);
		if (pbuf)
		{
			obdinfo.ucDTCTotalNum = can_get_dtc_cnt(0);
			obdinfo.pDTCList = can_get_dtc_buf_ptr(0);
			can_get_calid(0,obdinfo.szCalid,sizeof(obdinfo.szCalid));
			can_get_cvn(0,obdinfo.szCVN,sizeof(obdinfo.szCVN));
			can_get_iupr(0,obdinfo.szIUPR,sizeof(obdinfo.szIUPR));
			can_get_vin(0,obdinfo.szVIN,17);
			obdinfo.ucMilStatus = can_get_mil_sta(0);
			obdinfo.ucOBDProtocol = can_get_obd_pro(0);
			obdinfo.usReadyStatus = can_get_drt_sta(0);
			obdinfo.usSupportStatus = can_get_dst_sta(0);
			memcpy(obdinfo.BCDTime,get_rtc_time_ptr(),6);
			//server1 17691
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)1,0,0))	//判断服务器是否有效
			{
				if (get_net_link_status(1))
				{
					rlen = GB17691_RealDataStreamAndObdInfoPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				else
				{
					rlen = GB17691_SupplyDataStreamAndObdInfoPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				send_msg(S_MSG_SEND_TO_SERVER,(void *)1,pbuf,(void *)rlen);
			}

			//server3 17691
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)3,0,0))	//判断服务器是否有效
			{
				if (get_net_link_status(3))
				{
					rlen = GB17691_RealDataStreamAndObdInfoPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				else
				{
					rlen = GB17691_SupplyDataStreamAndObdInfoPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				send_msg(S_MSG_SEND_TO_SERVER,(void *)3,pbuf,(void *)rlen);
			}

			//server2 hj1239
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)2,0,0))		//判断服务器是否有效
			{
				static unsigned int s_last_obd_time = 0;
				static unsigned char s_obd_info_flag = 0;
				static unsigned char s_last_acc_sta_hj = 0;

				if (get_net_link_status(2))
				{
					rlen = HJ1239_RealDataStreamPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				else
				{
					rlen = HJ1239_SupplyDataStreamPacketMutli(pbuf,1536,&obdinfo,dsi_buf,bagcnt);
				}
				send_msg(S_MSG_SEND_TO_SERVER,(void *)2,pbuf,(void *)rlen);

				if (is_calid_request_ok())
				{
					//开机一分钟 上报OBD信息 或者 每隔 12小时 上报一次 或者 ACC状态发生变化
					if (((GetRunTime() >= 60) && (s_obd_info_flag == 0)) 
						|| ((GetRunTime() - s_last_obd_time) >= (12 * 3600))
						|| ((s_last_acc_sta_hj != (unsigned int)send_msg(S_MSG_GET_ACC_STATUS,0,0,0)) && send_msg(S_MSG_GET_ACC_STATUS,0,0,0) && (GetRunTime() >= 60))
						)
					{
						rlen = df_17691_baseinfo_pack(pbuf,1536,
														get_rtc_time_ptr(),
														send_msg(S_MSG_GET_JIAMI_CHIP_ID,0,0,0),16,
														send_msg(S_MSG_GET_ICCID,0,0,0),20,
														GetVIN(),17
														);
						DBG_PrintBuf(LEVEL2,"df hj base info:\r\n",pbuf,rlen);
						send_msg(S_MSG_SEND_TO_SERVER,(void *)2,pbuf,(void *)rlen);
					
						rlen = CalibrationTimePacket(pbuf,1536);
						send_msg(S_MSG_SEND_TO_SERVER,(void *)2,pbuf,(void *)rlen);
					
						if (get_net_link_status(2))
						{
							rlen = HJ1239_RealOBDInfoPacket(pbuf,1536,&obdinfo);
						}
						else
						{
							rlen = HJ1239_SupplyOBDInfoPacket(pbuf,1536,&obdinfo);
						}
						send_msg(S_MSG_SEND_TO_SERVER,(void *)2,pbuf,(void *)rlen);
						s_obd_info_flag = 1;
						s_last_acc_sta_hj = (unsigned int)send_msg(S_MSG_GET_ACC_STATUS,0,0,0);
					
						s_last_obd_time = GetRunTime();
					}
				}
			}

			//hj1239备案平台 连续1分钟无应答关闭备案平台
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)5,0,0))
			{
				s_port_cnt++;
				if (s_port_cnt > 6)
				{
					s_port_cnt = 0;
					send_msg(S_MSG_SET_SERVER_IP_PORT,(void *)5,"0",0);
				}
			}
			else
			{
				s_port_cnt = 0;
			}
			os_free(pbuf);
		}
		bagcnt = 0;
	}
}

void usart_data_arrive(void *buf,unsigned int rlen)
{
	analyze_usercmd(buf,rlen,0,0);
}

void login_gb32960_plat(INT8U linkid)
{
	INT8U time[6] = {0};
	unsigned char *ptr = 0;
	INT16U rlen = 0;
	char *piccid = 0;

	//登入前先清除登入成功标志，等待平台登录应答后再允许上报数据
	set_gb32960_login_ok_flag(linkid, 0);

	send_msg(S_MSG_GET_RTC_TIME,time,0,0);
	piccid = send_msg(S_MSG_GET_ICCID,0,0,0);

	ptr = os_malloc(1536);
	if (ptr)
	{
		rlen = GB32960_LoginPack(ptr+3,1024,linkid,time,piccid,0,0,0);

		DBG_Print(LEVEL2,"(link%d)GB32960 login(%uBytes):\r\n",linkid,rlen);
		DBG_PrintBuf(LEVEL2,0,ptr+3,rlen);

		//net_send_in_cb(linkid,ptr+3,rlen);
		send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,ptr+3,(void *)rlen);
		os_free(ptr);
	}
}

void server_disconnect(INT8U linkid)
{
	set_net_link_status(linkid,0);
	set_gb32960_login_ok_flag(linkid,0);

}

void server_connect_ok(INT8U linkid)
{
	set_net_link_status(linkid,1);

	if ((linkid == 0) || (linkid == 4))
	{
		char *ip = 0;

		login_gb32960_plat(linkid);

		ip = send_msg(S_MSG_GET_SERVER_ADDR,0,0,0);
		//df ota protocol
		if ((linkid == 0) && (strcmp(ip,"ev.cwjh.cn") == 0))
		//if (linkid == 0)
		{
			unsigned char *pbuf = 0;
			unsigned int rlen = 0;

			//DBG_Print(LEVEL2,"ip == ev.cwjh.cn\r\n");
			pbuf = os_malloc(1536);
			if (pbuf)
			{
				//老的上报基础信息协议格式不用 采用新的格式上报
				//rlen = xGB32960_DongfengBasicPack(pbuf,1536,GetVIN());
				//send_msg(S_MSG_SEND_TO_SERVER,(void *)0,pbuf,(void *)rlen);
				//DBG_Print(LEVEL2,"(link%u)df basic info(%uBytes)\r\n",linkid,rlen);
				//DBG_PrintBuf(LEVEL2,0,pbuf,rlen);

				rlen = xGB32960_DongfengNewBasicPack(pbuf,1536,GetVIN(),
													get_rtc_time_ptr(),
													send_msg(S_MSG_GET_ICCID,0,0,0),20,
													GetVIN(),17);

				send_msg(S_MSG_SEND_TO_SERVER,(void *)0,pbuf,(void *)rlen);
				DBG_Print(LEVEL2,"(link%u)df new basic info(%uBytes)\r\n",linkid,rlen);
				DBG_PrintBuf(LEVEL2,0,pbuf,rlen);

				os_free(pbuf);
			}
		}
	}
	else if (linkid == 3)				//东风股份商研院 车云协议
	{
		unsigned char *pbuf = 0;
		unsigned int rlen = 0;
		
		DBG_Print(LEVEL0,"(link%u)dfgf link OK!!!\r\n",linkid);
		
		pbuf = os_malloc(1536);
		if (pbuf)
		{
			rlen = DFGF_LoginPack(pbuf,1000,linkid,get_rtc_time_ptr());
			send_msg(S_MSG_SEND_TO_SERVER,(void *)3,pbuf,(void *)rlen);
			DBG_Print(LEVEL0,"(link%u)dfgf login info(%uBytes)\r\n",linkid,rlen);
			os_free(pbuf);
		}
	}

}

void server_data_arrive(unsigned char channel,void *pdat,unsigned int datlen)
{
	
	switch (channel)
	{
	case 0:					//gb32960
		gb32960_ack_plat(channel,pdat,datlen);
	break;

	case 2:					//hj1239
		anlyze_17691_server_data(channel,pdat,datlen);
	break;

	case 1:					//17691
	case 3:
		anlyze_dfgf_sy_server_data(channel,pdat,datlen);
	break;

	case 4:					//dst
		gb32960_ack_plat(channel,pdat,datlen);
		dst_ack_plat(channel,pdat,datlen);
	break;
	case 5:					//HJ备案平台的响应信息
		anlyze_17691_server_data(channel,pdat,datlen);
	break;

	case 6:					//时代电服平台响应信息
		sddf_ack_plat(channel,pdat,datlen);
	break;
	case 7:					//test back door
		adhd_analyze(channel,pdat,datlen);
	break;

	case 8:					//back door
		link4_data_arrive(pdat,datlen);
	break;
	}
	
}

void linux_cmd_ack_data_arrive(void *pdat,unsigned int datlen)
{
	unsigned char *psend = 0;
	unsigned int rlen = 0;

	DBG_Print(LEVEL2,"[app]linux cmd ack(%uBytes):\r\n%s\r\n",datlen,pdat);

	psend = os_malloc(1536);
	if (psend)
	{
		rlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,pdat,datlen,psend,1536);	
		send_msg(S_MSG_SEND_TO_SERVER,(void *)8,psend,(void *)rlen);
		os_free(psend);
	}
}

void sms_msg_arrive(unsigned char *ptime,void *phone_num,void *sms_content,unsigned int sms_content_len)
{
	unsigned char *psend = 0;
	unsigned char *p = 0;
	unsigned int rlen = 0;
	unsigned int xlen = 0;
	unsigned char *ptr = (unsigned char *)sms_content;
	unsigned char *ack = 0;

	DBG_Print(LEVEL0,"[app]sms arrive(%s):\r\n%s\r\n",phone_num,sms_content);
	
	if (strstr(sms_content, "DFAC#ShutDown#tBox"))
	{
		report_logout_to_server(2,0);
		DBG_Print(LEVEL0,"SMS shutdown the system!!!\n");

	}
	else
	{
		DBG_Print(LEVEL0,"Do Not Find DFAC#ShutDown#tBox!!!\n");
	}
	

	psend = os_malloc(1536);
	if (psend)
	{
		p = os_malloc(1536);
		if (p)
		{
			xlen = sprintf((char *)p,"[sms:%s]",(char *)phone_num);
			if (sms_content_len < 1024)
			{
				memcpy(p + xlen,sms_content,sms_content_len);
				xlen += sms_content_len;
			}
			else
			{
				memcpy(p + xlen,"sms too long",strlen("sms too long"));
				xlen += strlen("sms too long");
			}
			p[xlen++] = '\r';
			p[xlen++] = '\n';

			rlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,p,xlen,psend,1536); 
			send_msg(S_MSG_SEND_TO_SERVER,(void *)8,psend,(void *)rlen);

			os_free(p);
		}

		os_free(psend);
	}

	ack = os_malloc(512);
	if (ack)
	{
		rlen = (unsigned int)send_msg(S_MSG_EXECUTE_DRV_CMD,ptr,ack,(void *)512);
		if (rlen)
		{
			psend = os_malloc(1536);
			if (psend)
			{
				xlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,ack,rlen,psend,1536);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)8,psend,(void *)xlen);
				send_msg(S_MSG_SEND_SMS,phone_num,(void *)ack,(void *)rlen);
				os_free(psend);
			}
		}
#if 1
		rlen = analyze_usercmd(ptr,sms_content_len,ack,512);
		if (rlen)
		{
			psend = os_malloc(1536);
			if (psend)
			{
				xlen = tccp_pack(TCCP_ACK_TRANS_CMD_MSG_ID,ack,rlen,psend,1536);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)8,psend,(void *)xlen);
				send_msg(S_MSG_SEND_SMS,phone_num,(void *)ack,(void *)rlen);
				os_free(psend);
			}
		}
#endif
		os_free(ack);
	}
}


void cache_data_arrive(unsigned char cache_data_type_id,void *pdat,unsigned int rlen)
{
	if (pdat && rlen)
	{
		DBG_PrintBuf(LEVEL1,"32960 sos bf\r\n",(unsigned char *)pdat + 24,6);
		send_msg(S_MSG_SEND_TO_SERVER,0,pdat,(void *)rlen);
	}
}

void report_logout_to_server(unsigned int sleep_mode,unsigned int auto_wake_time_inv)
{
	unsigned int rlen = 0;
	unsigned char *pbuf = 0;

	//休眠的时候将网络错误累加时间保存到参数区
	set_dst_net_error_accumulate_time(get_dst_net_error_accumulate_time_tmp());
	save_param();

	//登出
	pbuf = os_malloc(512);
	if (pbuf)
	{
		send_msg(S_MSG_GET_RTC_TIME,get_rtc_time_ptr(),(void *)6,0);
		
		rlen = DFGF_LogoutPack(pbuf,512,get_rtc_time_ptr());
		send_msg(S_MSG_SEND_TO_SERVER,(void *)3,pbuf,(void *)rlen);

		rlen = GB32960_LogoutPack(pbuf,512,0,get_rtc_time_ptr());
		send_msg(S_MSG_SEND_TO_SERVER,(void *)0,pbuf,(void *)rlen);
		
		//登出后清除登入成功标志，不再上报采集的数据
		set_gb32960_login_ok_flag(0, 0);

		os_free(pbuf);
	}
	os_sleep(4000);
	send_msg(S_MSG_ENTER_SLEEP_MODE,(void *)sleep_mode,0,(void *)auto_wake_time_inv);
}

static unsigned char s_main_off_flag = 0;

void reset_main_off_flag(void)
{
	s_main_off_flag = 0;
}

void app_main_power_off_callback(void)
{

	if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0))		//地上铁 断电不立即休眠
	{
		if (s_main_off_flag == 0)
		{
			s_main_off_flag = 1;
			device_heart_beat1();
		}
		set_main_power_off_flag(1);
	}
	else
	{
		//启动时间大于40秒 才可以进入休眠模式
		//因为4G模块启动大概需要30秒
		//MCU要把断电信号传递到4G模块才算安全下电
		if (GetRunTime() >= 40)
		{
			if (s_main_off_flag == 0)
			{
				s_main_off_flag = 1;
				device_heart_beat1();
				report_logout_to_server(2,0);
			}
		}
	}
}

void sddf_charge_status_proc(void)
{
	static unsigned char s_sddf_last_charge_status = 0;
	unsigned int rlen = 0;
	unsigned char *pbuf = 0;

	if (s_sddf_last_charge_status != gb32960_can_get_charge_status())	//充电状态发生改变
	{
		if ((s_sddf_last_charge_status == 1) && ((gb32960_can_get_charge_status() == 3) || (gb32960_can_get_charge_status() == 4)))				//充电完成 上报电量
		{
			pbuf = os_malloc(512);
			if (pbuf)
			{
				rlen = sddf_charge_data_pack(pbuf,1024,GetVIN(),get_rtc_time_ptr());
				DBG_Print(LEVEL3,"sddf charge data info(%uBytes):\r\n",rlen);
				DBG_PrintBuf(LEVEL3,0,pbuf,rlen);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)6,pbuf,(void *)rlen);

				os_free(pbuf);
			}
		}

		s_sddf_last_charge_status = gb32960_can_get_charge_status();
	}	
}

void collect_sddf_pack(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char t[6] = {0};
	static unsigned char s_sddf_power_on_report = 0;

	//端口为0 表示不传时代电服平台
	//未获取到VIN 不传
	if ((send_msg(S_MSG_GET_SERVER_PORT,(void *)6,0,0) == 0) 
		|| (GetRunTime() < 25)
		|| (is_string(GetVIN(),17) == 0)
		)
	{
		return ;
	}

	//未获取到里程 不上传值
	if (is_get_milage_id() == 0)
	{
		return ;
	}

	pbuf = os_malloc(1536);
	if (pbuf)
	{
		send_msg(S_MSG_GET_RTC_TIME,t,0,0);

		if (get_net_link_status(0))
		{
			rlen = sddf_xGB32960_RealDataPack(pbuf,1536,GetVIN(),t,0,
										(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
										(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
										);
		}
		else
		{
			rlen = sddf_xGB32960_BFDataPack(pbuf,1536,GetVIN(),t,0,
										(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
										(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
										);
		}

		DBG_Print(LEVEL3,"sddf data(%uBytes):\r\n",rlen);
		DBG_PrintBuf(LEVEL3,0,pbuf,rlen);
		send_msg(S_MSG_SEND_TO_SERVER,(void *)6,pbuf,(void *)rlen);

		//心跳包
		rlen = GB32960_HeartBeatPack(pbuf,128);
		DBG_Print(LEVEL3,"sddf heart(%uBytes):\r\n",rlen);
		DBG_PrintBuf(LEVEL3,0,pbuf,rlen);
		send_msg(S_MSG_SEND_TO_SERVER,(void *)6,pbuf,(void *)rlen);

		if (get_sddf_login_ok_flag() && (s_sddf_power_on_report == 0))
		{
			s_sddf_power_on_report = 1;

			rlen = sddf_BasicDataPack(pbuf,1024,GetVIN(),t);
			DBG_Print(LEVEL3,"sddf basic info(%uBytes):\r\n",rlen);
			DBG_PrintBuf(LEVEL3,0,pbuf,rlen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)6,pbuf,(void *)rlen);
		}

		os_free(pbuf);
	}
}

void sddf_proc(void)
{
	sddf_charge_status_proc();
}

void collect_one_dst_pack(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char t[6] = {0};

	pbuf = os_malloc(1536);
	if (pbuf)
	{
		send_msg(S_MSG_GET_RTC_TIME,t,0,0);

		if (is_get_milage_id())						//获取到了里程 发送带国标的数据
		{
			if (get_net_link_status(4))
			{
				rlen = dst_RealDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = dst_BFDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		}
		else										//没获取到里程 发送不带国标的数据
		{
			if (get_net_link_status(4))
			{
				rlen = dst_zdy_data_pack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = dst_zdy_data_pack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		}

		if ((GetRunTime() > 25) && is_string(GetVIN(),17))
		{
			DBG_Print(LEVEL3,"dst data(%uBytes):\r\n",rlen);
			DBG_PrintBuf(LEVEL3,0,pbuf,rlen);

			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0))
			{
				send_msg(S_MSG_SEND_TO_SERVER,(void *)4,pbuf,(void *)rlen);
			}
		}
		os_free(pbuf);
	}
}
void collect_one_32960_zuocang_pack(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char t[6] = {0};
	pbuf = os_malloc(1536);
	if (pbuf)
	{
		send_msg(S_MSG_GET_RTC_TIME,t,0,0);
		if (get_net_link_status(0) && get_gb32960_login_ok_flag(0) && (is_string(GetVIN(),17)))
		{
			rlen = EV18_xGB32960_zuocang_RealDataPack(pbuf,1536,GetVIN(),t,0,
										(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
										(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
										);
			send_msg(S_MSG_SEND_TO_SERVER,0,pbuf,(void *)rlen);
			
		}
		os_free(pbuf);
	}
}

void collect_one_32960_pack(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char t[6] = {0};
	
	if(!(get_collect_32960_acc_status()))
	{
		if(get_engine_type() == EngineType_EV18)
		{
			set_collect_32960_acc_status(send_msg(S_MSG_GET_ACC_STATUS,0,0,0)?1:0);
		}
		else if(get_engine_type() == EngineType_DFAC)
		{
			if (get_can_no_data_time() < 3)
			{
				set_collect_32960_acc_status(1);
			}
		}
		
		return ;
	}

	//登入前不发送其他类型的报文；登出后不再上报采集的32960数据
	if (!get_gb32960_login_ok_flag(0))
	{
		return ;
	}

	pbuf = os_malloc(1536);
	if (pbuf)
	{
		send_msg(S_MSG_GET_RTC_TIME,t,0,0);

		if (get_engine_type() == EngineType_D78)				// D78车型新增自定义数据
		{
			if (get_net_link_status(0))
			{
				rlen = D78_xGB32960_RealDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = D78_xGB32960_BFDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		}
		else if(get_engine_type() == EngineType_EM26)
		{
			if (get_net_link_status(0))
			{
				rlen = EM26_xGB32960_RealDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = EM26_xGB32960_BFDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		}
		else if(get_engine_type() == EngineType_EV18)
		{
			if (get_net_link_status(0))
			{
				rlen = EV18_xGB32960_RealDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = EV18_xGB32960_BFDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		    
		}
		else
		{
			if (get_net_link_status(0))
			{
				rlen = xGB32960_RealDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
			else
			{
				rlen = xGB32960_BFDataPack(pbuf,1536,GetVIN(),t,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0)
											);
			}
		}

		if ((GetRunTime() > 25) && is_string(GetVIN(),17))
		{
			DBG_Print(LEVEL3,"gb32690 data(%uBytes):\r\n",rlen);
			DBG_PrintBuf(LEVEL3,0,pbuf,rlen);
			if (send_msg(S_MSG_GET_SERVER_PORT,0,0,0))
			{
				send_msg(S_MSG_SEND_TO_SERVER,0,pbuf,(void *)rlen);
			}
		}
		
		os_free(pbuf);
	}
}

INT32U s_32960_zuocang_collect_time = 50;
INT32U zuocang_no_driver_time = 0;

void collect_data_proc(void)
{
	unsigned char t[6] = {0};
	static INT8U s_last_cdp_sec_time = 0;
	static INT32U s_32960_collect_time = 0;
//	static INT32U s_17691_collect_time = 0;
	
	if(get_want_startup_status()
		&& (gb32960_can_get_qddj_rpm(0) < 20300) && (gb32960_can_get_qddj_rpm(0) > 19700))
	{
		unsigned int no_mpu_msg_time =  (INT32U)send_msg(S_MSG_GET_MPU_STATUS,0,0,0);
		//DBG_Print(LEVEL0,"no_mpu_msg_time:%d\r\n",no_mpu_msg_time);
		if(no_mpu_msg_time > 180)
		{
			set_want_startup_status(0);
			send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
			set_s_32960_zuocang_data_change(1);
			DBG_Print(LEVEL0,"no_mpu_msg_time:%d\r\n",no_mpu_msg_time);
		}
	}
	//EM26 EV18这几款车型需要存储每秒的缓存数据
	if ((get_engine_type() == EngineType_EM26) || (get_engine_type() == EngineType_EV18) || (get_engine_type() == EngineType_DFAC))
	{
		send_msg(S_MSG_SET_STORE_CACHE_DATA_FLAG,(void *)1,0,0);
	}
	else
	{
		send_msg(S_MSG_SET_STORE_CACHE_DATA_FLAG,0,0,0);
	}

	//link8
	//server4_connected();

	//link7
	//server_test_connected();

	//时间不正确 禁止采集数据
	if (is_time_format_ok(get_rtc_time_ptr()) == 0)
	{
		return ;
	}

	if (send_msg(S_MSG_GET_SERVER_PORT,0,0,0))	//判断服务器是否有效
	{
		if (get_engine_type() == EngineType_D78)		//D78车型 报警监测
		{
			d78_gb32960_sos_proc(collect_one_32960_pack);
		}
		else											//标准的GB32960报警监测
		{
			gb32960_sos_proc(collect_one_32960_pack);
		}
		sandian_proc();
	}

	// 1s
	send_msg(S_MSG_GET_RTC_TIME,t,0,0);

	if (t[5] != s_last_cdp_sec_time)
	{
		s_last_cdp_sec_time = t[5];

		//link0 link4
		s_32960_collect_time++;
		if (s_32960_collect_time >= gb32960_get_caiji_time_inteval())
		{
			
			s_32960_collect_time = 0;
			//CAN总线上无数据 地上铁不需要用无效数据来刷新
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0) == 0)
			{
				//保留 最后一帧有效数据 不进行数据无效填充
				//gb32960_fresh();
			}

			if (is_usually_collect())
			{
				DBG_Print(LEVEL0,"collect_one_32960_pack!!!\r\n");
				collect_one_32960_pack();
				//collect_one_dst_pack();
				
				gb32960_can_set_charge_set_dtc_cnt(0);
				gb32960_can_set_qddj_dtc_cnt(0);
				gb32960_can_set_ecu_dtc_cnt(0);
				gb32960_can_set_other_dtc_cnt(0);
			}
			//collect_sddf_pack();
		}
/*		
		s_32960_zuocang_collect_time++;
		if((s_32960_zuocang_collect_time > 170) || (get_s_32960_zuocang_data_change() && s_32960_zuocang_collect_time > 2))
		{
			s_32960_zuocang_collect_time = 0;
			set_s_32960_zuocang_data_change(0);
			collect_one_32960_zuocang_pack();
		}
*/		
		if(get_want_startup_status() == 0)
		{
			if((gb32960_can_get_qddj_rpm(0) < 20300) && (gb32960_can_get_qddj_rpm(0) > 19700)
			&& ((get_hand_brk_status() == 0x01) || (get_brk_status() == 0x01)))
			//&& ((gb32960_can_get_dangwei() == 0xF) || (gb32960_can_get_dangwei() == 0x0)))
			{
				set_want_startup_status(0);
				send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
				set_s_32960_zuocang_data_change(1);
			}
		}

		if(get_want_startup_status()
			&& (gb32960_can_get_qddj_rpm(0) < 20300) && (gb32960_can_get_qddj_rpm(0) > 19700))
		{
			if((get_key_in_status() == 0x1) && (get_key_status() == 0x02))
			{
				set_want_startup_status(0);
				send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
				set_s_32960_zuocang_data_change(1);
				DBG_Print(LEVEL0,"get_key_in_status,VCU_KeyPosition ON->START\r\n");
			}
			
			zuocang_no_driver_time++;
			if(zuocang_no_driver_time > 7200)
			{
				set_want_startup_status(0);
				send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
				set_s_32960_zuocang_data_change(1);
				zuocang_no_driver_time = 0;
				DBG_Print(LEVEL0,"zuocang_no_driver_time:%d,run_time:%d\r\n",zuocang_no_driver_time,GetRunTime());
			}
		}
		collect_dfgf_pack();
	}
}

//地上铁锁车逻辑 100毫秒执行一次
void dst_lock_car_proc(void)
{
	if (get_lock_car_cmd_from_where() == 2) 		//锁车命令来自地上铁 执行地上铁的报文
	{
		dst_send_to_can_bus_1801D0D5();
	}
}

//检测网络信号 进行车辆锁车 解锁等操作
void dst_check_network_proc(void)
{
#define NETWORK_ERROR_THRESHOLD_TIME 			(30 * 60)							//网络故障的判断时间 连续多长时间网络信号值不达标 判定为网络故障
#define NETWORK_ERROR_LOCK_CAR_TIME				(48 * 3600)							//网络错误之后多长时间进行锁车
#define NETWORK_ERROR_LOCK_CAR_MILAGE			500									//网络异常 跑多少公里进行锁车 单位0.1km

	static unsigned int s_no_dbm_cnt = 0;
	static unsigned int s_resume_dbm_cnt = 0;
	unsigned char csq = 0;
	unsigned char t1[6] = {0};
	unsigned char t2[6] = {0};

	//锁车版本的情况下
	if (get_dst_fact_pro_ver_about_lock() && get_dst_want_pro_ver_about_lock())
	{
		csq = csq;
		csq = (unsigned int)send_msg(S_MSG_GET_SIM_CSQ,0,0,0);
		//if (csq < 8)					//信号值小于 -99dbm
		if (get_net_link_status(4) == 0)
		{
			s_resume_dbm_cnt = 0;
			s_no_dbm_cnt++;
			if (s_no_dbm_cnt >= NETWORK_ERROR_THRESHOLD_TIME)						//持续3分钟信号值小于 -99dbm 报网络故障
			{
				if (is_get_milage_id())												//在获取到里程的情况下才能记录网络错误时的里程点
				{
					if (get_dst_network_error_flag() != 1)
					{
						set_dst_milage_when_network_error(gb32960_can_get_milage());		//记录网络错误时的里程点
						set_dst_network_error_flag(1);
					}
				}
			}
		}
		//else if (csq > 10)			//信号值大于-91dbm
		else if (get_net_link_status(4))
		{
			s_resume_dbm_cnt++;
			if (s_resume_dbm_cnt >= 4)
			{
				s_no_dbm_cnt = 0;
				if (get_dst_network_error_tip_flag() == 1)									//网络信号恢复
				{
					//恢复锁车参数
					resume_dst_lock_car_param();
				}

				set_dst_time_when_network_error(t2);									//时间点记录清零
				set_dst_milage_when_network_error(0);									//网络错误时的里程记录清零
				set_dst_network_error_flag(0);
				set_dst_network_error_tip_flag(0);
				set_dst_no_auto_lock_when_meet_net_error_condition_flag(0);
			}
		}

		//网络异常限速的三大条件
		//一、断网
		//二、里程指标:从断网那一刻的里程要开足50Km(当满足断网行驶50Km时 仪表开始提示)
		//三、时间指标:从仪表提示那一刻开始算要满足48小时
		if (get_dst_network_error_flag() == 1)
		{
			//网络错误之后 行驶里程累计超过50km后下发仪表提示
			if ((gb32960_can_get_milage() >= get_dst_milage_when_network_error())
				&& ((gb32960_can_get_milage() - get_dst_milage_when_network_error()) >= NETWORK_ERROR_LOCK_CAR_MILAGE)
				)
			{
				if (get_dst_network_error_tip_flag() != 1)
				{
					//库存融限速状态和一级限速状态下 即使满足网络异常锁车条件也不进行自动锁车
					if (get_dst_want_lock_status() && (get_dst_want_kucunrong_limit_speed() || (get_dst_want_limit_speed_level() == 0)))
					{
						set_dst_no_auto_lock_when_meet_net_error_condition_flag(1);
					}
					else
					{
						set_dst_no_auto_lock_when_meet_net_error_condition_flag(0);
					}

					//首先备份一下锁车参数 网络异常恢复之后要恢复成正常的锁车参数
					backup_dst_lock_car_param();

					send_msg(S_MSG_GET_RTC_TIME,t2,0,0);
					set_dst_time_when_network_error(t2);								//记录网络错误时的时间点

					set_dst_network_error_tip_flag(1);
				}

				//防止时间不合格 等到时间合格的情况下要将合格的时间再次更新为网络错误的那个时间点
				memcpy(t1,get_dst_time_when_network_error(),6);
				send_msg(S_MSG_GET_RTC_TIME,t2,0,0);
				if ((is_time_format_ok(t1) == 0) && is_time_format_ok(t2))
				{
					set_dst_time_when_network_error(t2);								//记录网络错误时的时间点
				}
			}

			//仪表提示超过48小时 进行锁车下发
			////网络信号差持续48小时以及累计里程超过50KM 自动进入一级锁车模式
			if (get_dst_network_error_tip_flag())
			{
				//网络异常时间累加
				set_dst_net_error_accumulate_time_tmp(get_dst_net_error_accumulate_time_tmp() + 1);

				memcpy(t1,get_dst_time_when_network_error(),6);
				send_msg(S_MSG_GET_RTC_TIME,t2,0,0);

				//时间有效的情况下判断时间
				//时间无效的情况下判断累计运行时间
				if ((get_dst_net_error_accumulate_time_tmp() >= NETWORK_ERROR_LOCK_CAR_TIME) 
					|| (is_time_format_ok(t1) && is_time_format_ok(t2) && (two_time_inv(t1,t2) >= NETWORK_ERROR_LOCK_CAR_TIME))
					)
				{
					//不自动锁车标志没置起来 说明可以下发自动一级限速指令
					if (get_dst_no_auto_lock_when_meet_net_error_condition_flag() == 0)
					{
						//设置一级锁车模式
						set_dst_want_pro_ver_about_lock(1);
						set_dst_want_lock_status(1);
						set_dst_want_limit_speed_level(0);
						set_dst_want_kucunrong_limit_speed(0);
					}
				}
			}
			else
			{
				set_dst_time_when_network_error(t2);
				set_dst_net_error_accumulate_time_tmp(0);
				set_dst_net_error_accumulate_time(0);
			}
		}
		else
		{
			set_dst_net_error_accumulate_time_tmp(0);
			set_dst_net_error_accumulate_time(0);
		}
	}
}

//判断电池是否低电 在电池供电的模式下
unsigned char is_bat_low_in_bat_supply_mode(void)
{
	unsigned int vol = 0;
	unsigned int hv = 0;

	if (send_msg(S_MSG_GET_POWER_SUPPLY_MODE,0,0,0))	// 1表示电池供电
	{
		hv = (unsigned int)send_msg(S_MSG_GET_HARDWARE_VERSION,0,0,0);
		vol = (unsigned int)send_msg(S_MSG_GET_BAT_VOL,0,0,0);

		if (hv == 1)
		{
			if (vol < 4300)
			{
				return 1;
			}
		}
		else if ((hv == 2) || (hv == 3))
		{
			if (vol < 2200)
			{
				return 1;
			}
		}
	}

	return 0;
}

void dst_tbox_status_proc(void)
{
	unsigned int vol = 0;
	static unsigned char s_low_cnt = 0;
	static unsigned char s_low_flag = 0;
	static unsigned char s_off_status_time = 0;
	static unsigned char s_low_status_time = 0;
	static unsigned char s_sleep_status_time = 0;

	//开机前1分钟 不进行低压检测
	if (GetRunTime() > 60)
	{
		vol = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
		if ((vol > 7000) && (vol < LOW_VOL_THRESHOLD))
		{
			s_low_cnt++;
			if (s_low_cnt >= 6)
			{
				s_low_flag = 1;
			}
		}
		else
		{
			s_low_cnt = 0;
			s_low_flag = 0;
		}
	}

	//设置TBOX状态
	if (get_main_power_off_flag())
	{
		set_dst_tbox_work_status(DST_TBOX_STATUS_MAIN_POWER_OFF);

		if ((GetRunTime() >= 40))
		{
			if (is_bat_low_in_bat_supply_mode()) //电池电量低 需要休眠 不休眠会是4G模块进入异常断电状态 容易引发系统崩溃
			{
				collect_one_dst_pack();
			}

			s_off_status_time++;
			if ((s_off_status_time >= 63) || is_bat_low_in_bat_supply_mode())
			{
				report_logout_to_server(2,0);								//CAN唤醒
			}
		}
	}
	else if (s_low_flag)
	{
		set_dst_tbox_work_status(DST_TBOX_STATUS_LOW_VOL);
		if (GetRunTime() >= 40)
		{
			s_low_status_time++;
			if (s_low_status_time >= 63)
			{
				report_logout_to_server(4,0);								// 12V唤醒
			}
		}
	}
	else if ((send_msg(S_MSG_GET_ACC_STATUS,0,0,0) == 0) && (get_can_no_data_time() > 3))
	{
		set_dst_tbox_work_status(DST_TBOX_STATUS_SLEEP);
		if (GetRunTime() >= 40)
		{
			s_sleep_status_time++;
			if (s_sleep_status_time >= 63)
			{
				vol = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
				if (vol < 9000)
				{
					report_logout_to_server(0,0);								// 电压低于9V 不自动唤醒
				}
				else
				{
					report_logout_to_server(8,3600 * 6);						// CAN唤醒 或者 自动唤醒
				}
			}
		}
	}
	else
	{
		s_low_status_time = 0;
		s_sleep_status_time = 0;
		set_dst_tbox_work_status(DST_TBOX_STATUS_WORK);
	}
}


void dst_charge_status_proc(void)
{
	static unsigned char s_charge_start_time[6] = {0};
	static unsigned char s_last_charge_status = 0;

	if (s_last_charge_status != gb32960_can_get_charge_status())	//充电状态发生改变
	{
		unsigned char buf[48] = {0};
		unsigned int rlen = 0;

		if (gb32960_can_get_charge_status() == 1)					//开始充电 记录停车充电的时间 清除充电电量
		{
			clr_charge_dl();
			memcpy(s_charge_start_time,get_rtc_time_ptr(),6);
		}

		if ((s_last_charge_status == 1) && ((gb32960_can_get_charge_status() == 3) || (gb32960_can_get_charge_status() == 4)))				//充电完成 上报电量
		{
			rlen = dst_cddl_data_pack(buf,48,GetVIN(),s_charge_start_time,get_rtc_time_ptr());
			send_msg(S_MSG_SEND_TO_SERVER,(void *)4,buf,(void *)rlen);
		}

		s_last_charge_status = gb32960_can_get_charge_status();
	}
}

void dst_vin_proc(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	static unsigned int s_last_report_vin_time = 0;			//上一次上报的时间
	static unsigned int s_vin_report_hz = 10;				//上报频率
	static unsigned char s_vin_report_cnt = 0;				//上报VIN次数

	//没定到位禁止上报VIN
	if (send_msg(S_MSG_GET_GPS_LOCAL_STATUS,0,0,0) == 0)
	{
		return ;
	}

	//vin发生变化
	if ((GetRunTime() > 50) && is_time_format_ok(get_rtc_time_ptr()) && is_string(GetVIN(),17))
	{
		if ((GetRunTime() - s_last_report_vin_time) >= s_vin_report_hz)
		{
			if (memcmp(GetVIN(),get_dst_vin(),17))
			{
				pbuf = os_malloc(512);
				if (pbuf)
				{
					rlen = dst_vin_data_pack(pbuf,512,GetVIN(),get_rtc_time_ptr());
					DBG_PrintBuf(LEVEL1,"report dst vin:\r\n",pbuf,rlen);
					send_msg(S_MSG_SEND_TO_SERVER,(void *)4,pbuf,(void *)rlen);
					os_free(pbuf);
					s_vin_report_cnt++;
					if (s_vin_report_cnt >= 3)				//上报三次之后 服务器还未应答就把频率改为30分钟上报一次
					{
						s_vin_report_hz = 30 * 60;
					}
				}
			}
			s_last_report_vin_time = GetRunTime();
		}
	}
}


// 100ms 执行一次
void dst_proc(void)
{
	static unsigned int s_dst_time = 0;
	unsigned char *pt = get_rtc_time_ptr();

	//第四路端口没开 表示不是地上铁的需求 直接跳出 不执行
	if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0) == 0)
	{
		return ;
	}

	//运行超过1天 对设备重启
	if ((GetRunTime() > (24 * 3600 * 1)) && ((pt[3] == 23) && (pt[4] == 59) && (pt[5] >= 56)))
	{
		//速度为0的情况下才重启 保证安全
		if (((gb32960_can_get_speed() == 0xFFFF) || (gb32960_can_get_speed() == 0))
			&& ((can_get_speed(0) == 0xFFFF) || (can_get_speed(0) == 0))
			)
		{
			send_msg(S_MSG_EXECUTE_DRV_CMD,"ATWST",0,0);
		}
	}

	//地上铁充电状态监测
	dst_charge_status_proc();

	if ((GetRunTime() - s_dst_time) >= 1)
	{
		//监测VIN变更
		dst_vin_proc();

		//检测地上铁服务器网络故障
		dst_check_network_proc();

		//地上铁TBOX状态监测
		dst_tbox_status_proc();

		s_dst_time = GetRunTime();
	}
}


//MPU下发消息到MCU app  mcu app解析MPU的消息函数
void user_mpu_msg_arrive(unsigned int msgid,void *pdat,unsigned int datlen)
{
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned int drv_addr = 0;
	unsigned int drv_len = 0;
	unsigned int app_addr = 0;
	unsigned int app_len = 0;

	switch (msgid)
	{
	case 1:								//MPU通知TBOX 三电MCU ota升级包下载完成
	{
		sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_MCU);
		//sandian_set_update_way(SANDIAN_UPDATE_WAY_QIANGZHI);
		sandian_set_update_way(SANDIAN_UPDATE_WAY_PUTONG);
		sandian_set_update_type(SANDIAN_UPDATE_TYPE_SHENGJI);

		drv_addr = (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
		drv_len = (ptr[4] << 24) + (ptr[5] << 16) + (ptr[6] << 8) + ptr[7];
		app_addr = (ptr[8] << 24) + (ptr[9] << 16) + (ptr[10] << 8) + ptr[11];
		app_len = (ptr[12] << 24) + (ptr[13] << 16) + (ptr[14] << 8) + ptr[15];
		if(get_engine_type() == EngineType_EM26)
		{
			drv_addr = 0x70000100;
			app_addr = 0x80028400;
		}
		DBG_Print(LEVEL2,"[mcu]drv:%08X %u app:%08X %u\r\n",drv_addr,drv_len,app_addr,app_len);

		if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_QIANGZHI)
		{
			sandian_mcu_start_ota_step(1,drv_addr,drv_len,app_addr,app_len);
		}
		else if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_PUTONG)
		{
			sandian_mcu_start_ota_step(0,drv_addr,drv_len,app_addr,app_len);
			sandian_set_update_request_flag(1);
		}
	}
	break;

	case 2:			//MPU通知TBOX 三电VCU ota升级包下载完成
		sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_VCU);
		//sandian_set_update_way(SANDIAN_UPDATE_WAY_QIANGZHI);
		sandian_set_update_way(SANDIAN_UPDATE_WAY_PUTONG);
		sandian_set_update_type(SANDIAN_UPDATE_TYPE_SHENGJI);

		drv_addr = (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
		drv_len = (ptr[4] << 24) + (ptr[5] << 16) + (ptr[6] << 8) + ptr[7];
		app_addr = (ptr[8] << 24) + (ptr[9] << 16) + (ptr[10] << 8) + ptr[11];
		app_len = (ptr[12] << 24) + (ptr[13] << 16) + (ptr[14] << 8) + ptr[15];
		if(get_engine_type() == EngineType_EM26)
		{
			drv_addr = 0xC0000000;
			app_addr = 0x80060000;
		}
		DBG_Print(LEVEL2,"[vcu]drv:%08X %u app:%08X %u\r\n",drv_addr,drv_len,app_addr,app_len);

		if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_QIANGZHI)
		{
			sandian_vcu1_start_ota_step(1,drv_addr,drv_len,app_addr,app_len);
		}
		else if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_PUTONG)
		{
			sandian_vcu1_start_ota_step(0,drv_addr,drv_len,app_addr,app_len);
			sandian_set_update_request_flag(1);
		}
	break;

	case 3:

	break;
	case 4:			//MPU通知TBOX 鲤动BMS ota升级包下载完成
	{
		//FileBlockInfo drv[2] = {{0x4002C000,8124},{0}};
		//FileBlockInfo app[9] = {{0x1000000,8},{0x1000010,8},{0x1000020,8},{0x1000030,9},{0x1000050,2},{0x1000400,786},{0x1001000,244},{0x1002000,2936},{0x1002B80,588047}};
		FileBlockInfo drv[4] = {0};
		FileBlockInfo app[12] = {0};
		unsigned short drv_blk_cnt = 0;
		unsigned short app_blk_cnt = 0;
		unsigned int i = 0;

		sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_BMS);
		sandian_set_update_way(SANDIAN_UPDATE_WAY_QIANGZHI);
		//sandian_set_update_way(SANDIAN_UPDATE_WAY_PUTONG);
		sandian_set_update_type(SANDIAN_UPDATE_TYPE_SHENGJI);

		drv_blk_cnt = ptr[0];
		if (drv_blk_cnt > 4) drv_blk_cnt = 4;
		for (i = 0; i < drv_blk_cnt; i++)
		{
			drv[i].start_addr = (ptr[1 + 8*i] << 24) + (ptr[2 + 8*i] << 16) + (ptr[3 + 8*i] << 8) + ptr[4 + 8*i];
			drv[i].block_size = (ptr[5 + 8*i] << 24) + (ptr[6 + 8*i] << 16) + (ptr[7 + 8*i] << 8) + ptr[8 + 8*i];
		}

		app_blk_cnt = ptr[1 + 8 * drv_blk_cnt];
		if (app_blk_cnt > 12) app_blk_cnt = 12;
		for (i = 0; i < app_blk_cnt; i++)
		{
			app[i].start_addr = (ptr[2 + 8*drv_blk_cnt + i*8] << 24) + (ptr[3 + 8*drv_blk_cnt + i*8] << 16) + (ptr[4 + 8*drv_blk_cnt + i*8] << 8) + ptr[5 + 8*drv_blk_cnt + i*8];
			app[i].block_size = (ptr[6 + 8*drv_blk_cnt + i*8] << 24) + (ptr[7 + 8*drv_blk_cnt + i*8] << 16) + (ptr[8 + 8*drv_blk_cnt + i*8] << 8) + ptr[9 + 8*drv_blk_cnt + i*8];
		}
		//DBG_Print(LEVEL2,"[bms]drv:%08X %u app:%08X %u\r\n",drv_addr,drv_len,app_addr,app_len);

		if (0)
		{
			char *ptip = 0;

			ptip = os_malloc(1536);
			if (ptip)
			{
				drv_len = 0;

				drv_len += sprintf(ptip + drv_len,"---drv info(%u)---\r\n",drv_blk_cnt);
				for (i = 0; i < drv_blk_cnt; i++)
				{
					drv_len += sprintf(ptip + drv_len,"%08X,%u\r\n",drv[i].start_addr,drv[i].block_size);
				}

				drv_len += sprintf(ptip + drv_len,"---app info(%u)---\r\n",app_blk_cnt);
				for (i = 0; i < app_blk_cnt; i++)
				{
					drv_len += sprintf(ptip + drv_len,"%08X,%u\r\n",app[i].start_addr,app[i].block_size);
				}
				send_msg(S_MSG_DBG_DISP,ptip,(void *)drv_len,0);

				os_free(ptip);
			}
		}

		if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_QIANGZHI)
		{
			sandian_bms1_start_ota_step1(1,drv,drv_blk_cnt,app,app_blk_cnt);
		}
		else if (sandian_get_update_way() == SANDIAN_UPDATE_WAY_PUTONG)
		{
			sandian_bms1_start_ota_step1(0,drv,drv_blk_cnt,app,app_blk_cnt);
			sandian_set_update_request_flag(1);
		}
	}
	break;

	default:break;
	}
}

void check_flow(void)
{
	if (memcmp(get_cur_date_ptr(),get_rtc_time_ptr(),3))
	{
		unsigned int i = 0;
		unsigned int cnt = get_gb_login_flow_cnt();
		INT16U *p = get_gb_login_flow_ptr();

		for (i = 0; i < cnt; i++)
		{
			p[i] = 0;
		}

		//memset(get_gb_login_flow_ptr(),0,get_gb_login_flow_cnt() * sizeof(INT16U));

		memcpy(get_cur_date_ptr(),get_rtc_time_ptr(),3);
	}
}

void mis_proc(void)
{
	unsigned char *pt = get_rtc_time_ptr();

	//每月2号 对设备进行检查数据连接情况 数据连接不是 对设备重启 使流量套餐恢复生效
	if ((GetRunTime() > (20 * 3600))
		&& ((pt[2] == 1) && (pt[3] == 23) && (pt[4] == 59) && (pt[5] >= 56))
		&& (get_net_link_status(0) == 0)
		&& (get_net_link_status(1) == 0)
		&& (get_net_link_status(2) == 0)
		&& (get_net_link_status(3) == 0)
		&& (get_net_link_status(4) == 0)
		&& (get_net_link_status(5) == 0)
		&& (get_net_link_status(8) == 0)
		)
	{
		//速度为0的情况下才重启 保证安全
		if (((gb32960_can_get_speed() == 0xFFFF) || (gb32960_can_get_speed() == 0))
			&& ((can_get_speed(0) == 0xFFFF) || (can_get_speed(0) == 0))
			)
		{
			send_msg(S_MSG_EXECUTE_DRV_CMD,"ATWST",0,0);
		}
	}

	//自动写入生产日期
	if (is_time_format_ok(get_sc_date()) == 0)
	{
		if (is_time_format_ok(get_rtc_time_ptr()))
		{
			set_sc_date(get_rtc_time_ptr());
		}
	}

	//检查平台登录流水号
	check_flow();
}

