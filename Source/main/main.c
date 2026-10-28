#include "sysapi.h"
#include "apps.h"

//指定地址存放版本号 厂商标识等信息
const  char ver_info[]  __attribute__ ((at(SOFTWARE_INFO_ADDR))) = 
"<" MANUFACTURER_FLAG ">" "<" APPLICATION_VERSION ">" "<" __DATE__ ">" "<" __TIME__ ">"; 


OS_STK task1_stk[384] = {0};
OS_STK task2_stk[384] = {0};

void can_task(void)
{
	while (1)
	{
		//燃油车发动机类型
		switch (get_ryc_engine_type())
		{
		case EngineType_ZD30:
		case EngineType_M9T:
		case EngineType_YC:
		case EngineType_QC:
		case EngineType_CC:
		case EngineType_KMS:
			uds_can_send_proc();
		break;
		default:break;
		}

		//auto dete the can bps
		auto_detect_can_bps_proc();

		can_send_proc();

		//对于部分EV18车辆 没获取到VIN 需要主动发请求
		//帧率要大于90fps 表示车辆正常工作 正常工作的情况下才去请求VIN
		if ((get_0c0728d0_flag() == 0)
		&& (get_auto_vin_ok_flag() == 0)
		&& (get_bps_version() == 2)
		&& (get_can_rx_fps(0) > 90)
		&& (GetRunTime() >= 5)
		&& (get_ryc_engine_type() == EngineType_Compatible)
		)
		{
			//请求VIN指令
			CanTxMsg txmsg = {0,0x180528F4,CAN_Id_Extended,CAN_RTR_DATA,8,{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

			can_send_data(0,&txmsg);
			if (get_recv_send_can_flag())
			{
				can_data_in_queue(get_lock_car_can_channel(),txmsg.IDE,(txmsg.IDE == CAN_Id_Extended) ? txmsg.ExtId : txmsg.StdId,txmsg.Data,8);
			}
		}
		os_sleep(100);
	}
}

void store_task(void)
{
	while (1)
	{
		store_can_msg();
		led_proc();
		//can_transfor_test();

		if (get_sandian_mcu_ota_step() || get_sandian_bms_ota_step() || get_sandian_vcu1_ota_step() || get_sandian_bms1_ota_step())
		{
			os_sleep(1);
		}
		else
		{
			os_sleep(10);
		}
	}
}
int mcu_start_update_flag = 0;
int main(void)
{
	unsigned int cnt = 0;
	

	//根据硬件版本设置软件版本号
	snprintf(APP_VERSION,sizeof(APP_VERSION),"%s",APPLICATION_VERSION);
	cnt = (unsigned int)send_msg(S_MSG_GET_HARDWARE_VERSION,0,0,0);
	if (cnt)
	{
		char *p = 0;
		p = strstr(APP_VERSION,"_V");
		if (p)
		{
			*(p + 2) = cnt + '0';
		}
	}

	DBG_Print(LEVEL0,"Software Version:%s Compile @ %s %s\r\n",APP_VERSION,__TIME__,__DATE__);

	clr_obd_data();

	load_param();
	
	set_dst_net_error_accumulate_time_tmp(get_dst_net_error_accumulate_time());

	CN6_Init(GetVIN(),get_software_version(),1);
	GB32960_Init(GetVIN(),1,get_gb_login_flow_ptr(),get_gb_login_flow_cnt());

	uds_init(_send_to_can_bus);

	reg_callback_msg(CB_MSG_DBG_UART_DATA_ARRIVE,usart_data_arrive);
	reg_callback_msg(CB_MSG_CONNECT_SERVER_OK,server_connect_ok);
	reg_callback_msg(CB_MSG_SERVER_DISCONNECT,server_disconnect);
	reg_callback_msg(CB_MSG_SERVER_DATA_ARRIVE,server_data_arrive);
	reg_callback_msg(CB_MSG_LINUX_CMD_ACK_ARRIVE,linux_cmd_ack_data_arrive);
	reg_callback_msg(CB_MSG_SMS_ARRIVE,sms_msg_arrive);
	reg_callback_msg(CB_MSG_CACHE_DATA_ARRIVE,cache_data_arrive);
	reg_callback_msg(CB_MSG_MAIN_POWER_OFF,app_main_power_off_callback);
	reg_callback_msg(CB_MSG_EASY_CHANGE_PARAM_ARRIVE,easy_change_param_arrive);
	reg_callback_msg(CB_MSG_FILE_DATA_ARRIVE,file_data_arrive);
	reg_callback_msg(CB_MSG_USER_MPU_MSG_ARRIVE,user_mpu_msg_arrive);


	install_hook_msg(HOOK_MSG_CAN0_DATA_ARRIVE,can0_hook);
	install_hook_msg(HOOK_MSG_CAN1_DATA_ARRIVE,can1_hook);
	install_hook_msg(HOOK_MSG_CAN2_DATA_ARRIVE,can2_hook);
	install_hook_msg(HOOK_MSG_ONE_MS,one_ms_hook);

	//固定CAN波特率
	set_can_bps_match_ok_flag(0,1);
	set_can_bps_match_ok_flag(1,1);
	set_can_bps_match_ok_flag(2,1);

	//根据版本号来设定CAN总线波特率
	//V1.A.B
	//A等于2标识250波特率
	//A等于5标识500波特率
	//A等于7标识CAN0 500波特率 CAN1 CAN2 250波特率
	if (get_bps_version() == 2)
	{
		SetCan0Bps(250);
		SetCan1Bps(250);
		SetCan2Bps(250);
	}
	else if (get_bps_version() == 5)
	{
		SetCan0Bps(500);
		SetCan1Bps(500);
		SetCan2Bps(500);
	}
	else if (get_bps_version() == 7)
	{
		SetCan0Bps(500);
		SetCan1Bps(250);
		SetCan2Bps(250);
	}

	can_init(0,GetCan0Bps());
	can_init(1,GetCan1Bps());
	can_init(2,GetCan2Bps());

	os_create_thread(8,can_task,&task1_stk[383],&task1_stk[0]);
	os_create_thread(9,store_task,&task2_stk[383],&task2_stk[0]);
	cnt = 10;
	while (1)
	{
		if(!mcu_start_update_flag)
		{
			collect_data_proc();			//采集数据
			//dst_proc();						//地上铁
			//sddf_proc();					//时代电服
		}

		//adhd_proc();					//奥动协议	
		mis_proc();						//杂项

		cnt++;
		if (cnt >= 10)
		{
			cnt = 0;

			//disp_dbg_info();
			if (is_string(get_test_vin(),17))
			{
				memcpy(GetVIN(),get_test_vin(),17);
				set_auto_vin_ok_flag(1);
			}
			else if (is_string(can_get_vin_ptr(0),17))
			{
				memcpy(GetVIN(),can_get_vin_ptr(0),17);
				save_set_vin(GetVIN());
				set_auto_vin_ok_flag(1);
			}
			else
			{
				send_msg(S_MSG_GET_VIN,GetVIN(),0,0);
				if (is_string(GetVIN(),17))
				{
					save_set_vin(GetVIN());
					//DBG_Print(LEVEL0,"save VIN to Flash!!!\r\n");
				}
				else
				{
					if (is_string(save_get_vin_ptr(),17))
					{
						memcpy(GetVIN(),save_get_vin_ptr(),17);
						//DBG_Print(LEVEL0,"read VIN from Flash!!!\r\n");
					}
				}
			}

			save_param();
			save_easy_change_param();
			calc_can_rx_fps(0);
			calc_can_rx_fps(1);
			calc_can_rx_fps(2);
			set_can_no_data_time(get_can_no_data_time() + 1);
			if (get_can_no_data_time() >= 3)
			{
				set_can_data_arrive_flag(0,0);
				set_can_data_arrive_flag(1,0);
				set_can_data_arrive_flag(2,0);
			}
			
			if(send_msg(S_MSG_GET_ACC_STATUS,0,0,0))
			{
				set_acc_no_data_time(0);
			}
			else
			{
				set_acc_no_data_time(get_acc_no_data_time() + 1);
			}
			
			if (send_msg(S_MSG_GET_SERVER_PORT,(void *)4,0,0) == 0)
			{
				//3分钟无CAN数据且ACC关闭 进入休眠模式
				if ((get_can_no_data_time() >= (180)) && (send_msg(S_MSG_GET_ACC_STATUS,0,0,0) == 0) && (get_acc_no_data_time() >= (180)))
				{
					DBG_Print(LEVEL0,"can_no_data_time:%d acc_no_data_time %d\r\n",get_can_no_data_time(),get_acc_no_data_time());
					if (0)
					{
						report_logout_to_server(2,3600 * 6);
					}
					else
					{
						report_logout_to_server(2,0);
					}
				}
			}

			if (get_moni_can_data_flag())
			{
				gb32960_moni_data_proc();
				moni_can_data_proc();
			}
		}

		send_msg(S_MSG_GET_RTC_TIME,get_rtc_time_ptr(),(void *)6,0);
		os_sleep(100);
	}
}


