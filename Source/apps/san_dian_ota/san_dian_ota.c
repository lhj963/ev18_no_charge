#include "san_dian_ota.h"

typedef struct tagSANDIAN_YIBIAO_DISP
{
	SANDIAN_UPDATE_TYPE eSANDIAN_UPDATE_TYPE;		//升级类型
	SANDIAN_UPDATE_WAY eSANDIAN_UPDATE_WAY;			//升级方式
	SANDIAN_UPDATE_STATUS eSANDIAN_UPDATE_STATUS;
	SANDIAN_UPDATE_LINGJIAN_ID eSANDIAN_UPDATE_LINGJIAN_ID;

	unsigned char request_update_flag;						//升级请求
}SANDIAN_YIBIAO_DISP;

SANDIAN_YIBIAO_DISP stSANDIAN_YIBIAO_DISP = {SANDIAN_UPDATE_TYPE_NONE,SANDIAN_UPDATE_WAY_NONE,SANDIAN_UPDATE_STATUS_NONE,SANDIAN_UPDATE_LINGJIAN_ID_NONE,0};

void sandian_set_update_type(SANDIAN_UPDATE_TYPE typ)
{
	stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_TYPE = typ;
}

void sandian_set_update_way(SANDIAN_UPDATE_WAY typ)
{
	stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_WAY = typ;
}

void sandian_set_update_status(SANDIAN_UPDATE_STATUS typ)
{
	stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_STATUS = typ;
}

void sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID typ)
{
	stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_LINGJIAN_ID = typ;
}

SANDIAN_UPDATE_LINGJIAN_ID sandian_get_update_lingjian_id(void)
{
	return stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_LINGJIAN_ID;
}

void sandian_set_update_request_flag(unsigned char val)
{
	stSANDIAN_YIBIAO_DISP.request_update_flag = val;
}

unsigned char sandian_get_update_request_flag(void)
{
	return stSANDIAN_YIBIAO_DISP.request_update_flag;
}

SANDIAN_UPDATE_WAY sandian_get_update_way(void)
{
	return stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_WAY;
}

unsigned int sandian_get_upgrade_progress(void)
{
	if (stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_LINGJIAN_ID == SANDIAN_UPDATE_LINGJIAN_ID_MCU)
	{
		return get_sandian_mcu_ota_progress();
	}
	else if (stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_LINGJIAN_ID == SANDIAN_UPDATE_LINGJIAN_ID_VCU)
	{
		return get_sandian_vcu1_ota_progress();
	}
	else if (stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_LINGJIAN_ID == SANDIAN_UPDATE_LINGJIAN_ID_BMS)
	{
		return get_sandian_bms1_ota_progress();
	}
	return 0;
}

void sandian_tx(unsigned char channel,unsigned int canid,unsigned char *pdat,unsigned char datlen)
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
		DBG_Print(LEVEL1,"[CAN%u]send data error:%u %uus\r\n",channel,ret,(b - a) / 108);
	}
}


//1:mcu_drv 2:mcu_app 11:bms_drv 12:bms_app 21:vcu_drv 22:vcu_app
void file_data_arrive(unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen)
{
	sandian_mcu_data_arrive(file_type,offset,pdat,datlen);
	sandian_bms1_data_arrive(file_type,offset,pdat,datlen);
	sandian_vcu1_data_arrive(file_type,offset,pdat,datlen);
}

void sandian_flow_ctrl_ack(unsigned char can_channel,unsigned int canid,void *pdata,unsigned int datlen)
{
	unsigned char *pdat = (unsigned char *)pdata;

	if ((pdat == 0) || (datlen == 0))
	{
		return ;
	}

	if ((canid == 0x7EF) || (canid == 0x7EE) || (canid == 0x7C8))			//VCU MCU BMS 
	{
		if ((pdat[0] & 0xF0) == 0x10)
		{
			if (canid == 0x7EF)
			{
				CanTxMsg tx = {0x7E7,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
				can_send_data(can_channel,&tx);
			}

			if (canid == 0x7EE)
			{
				CanTxMsg tx = {0x7E6,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
				can_send_data(can_channel,&tx);
			}

			if (canid == 0x7C8)
			{
				CanTxMsg tx = {0x7C0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
				can_send_data(can_channel,&tx);
			}

			//放在中断执行 禁止加打印调试信息
			//sandian_tx(can_channel,0x7DF,"\x30\x00\x00\x00\x00\x00\x00\x00",8);
		}
	}

}

void analyze_vcu_ver(unsigned int canid,void *pdata,unsigned int datlen)
{
	unsigned char *pdat = (unsigned char *)pdata;
	unsigned char temp = 0;
	unsigned char i = 0;

	if (canid != 0x7EF) 		//解析VCU版本
	{
		return ;
	}

	if ((pdat[0] & 0xF0) == 0x10)				//首帧
	{
		if ((pdat[2] == 0x62) && (pdat[3] == 0xF1) && (pdat[4] == 0x95))
		{
			clr_vcu_ver();
			set_vcu_ver(0,pdat[5]);
			set_vcu_ver(1,pdat[6]);
			set_vcu_ver(2,pdat[7]);
		}
	}
	else if ((pdat[0] & 0xF0) == 0x20)			//连续帧
	{
		temp = ((pdat[0] & 0x0F) - 1) * 7 + 3;
		for(i = 0; i < 7; i++)
		{
			set_vcu_ver(temp+i,pdat[1+i]);
		}
	}
}


void analyze_mcu_ver(unsigned int canid,void *pdata,unsigned int datlen)
{
	unsigned char *pdat = (unsigned char *)pdata;
	unsigned char temp = 0;
	unsigned char i = 0;

	if (canid != 0x7EE) 		//解析MCU版本
	{
		return ;	
	}

	if ((pdat[0] & 0xF0) == 0x10)				//首帧
	{
		if ((pdat[2] == 0x62) && (pdat[3] == 0xF1) && (pdat[4] == 0x95))
		{
			clr_mcu_ver();
			set_mcu_ver(0,pdat[5]);
			set_mcu_ver(1,pdat[6]);
			set_mcu_ver(2,pdat[7]);
		}
	}
	else if ((pdat[0] & 0xF0) == 0x20)			//连续帧
	{
		temp = ((pdat[0] & 0x0F) - 1) * 7 + 3;
		for(i = 0; i < 7; i++)
		{
			set_mcu_ver(temp+i,pdat[1+i]);
		}
	}
}


void analyze_bms_ver(unsigned int canid,void *pdata,unsigned int datlen)
{
	unsigned char *pdat = (unsigned char *)pdata;
	unsigned char temp = 0;
	unsigned char i = 0;

	if (canid != 0x7C8) 		//解析BMS版本
	{
		return ;	
	}

	if ((pdat[0] & 0xF0) == 0x10)				//首帧
	{
		if ((pdat[2] == 0x62) && (pdat[3] == 0xF1) && (pdat[4] == 0x95))
		{
			clr_bms_ver();
			set_bms_ver(0,pdat[5]);
			set_bms_ver(1,pdat[6]);
			set_bms_ver(2,pdat[7]);
		}
	}
	else if ((pdat[0] & 0xF0) == 0x20)			//连续帧
	{
		temp = ((pdat[0] & 0x0F) - 1) * 7 + 3;
		for(i = 0; i < 7; i++)
		{
			set_bms_ver(temp+i,pdat[1+i]);
		}
	}
}


void san_dian_ota_analyze(char channel,CanRxMsg *pMsg)
{
	static unsigned int s_sandian_dbg_last_time = 0;
	static unsigned int s_ack_ver_cnt = 0;
	unsigned int canid = 0;

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

	if ((canid == 0x7DF) || (canid == 0x7EF) || (canid == 0x7C8) || (canid == 0x7EE) || (canid == 0x74B) || (canid == 0x759) || (canid == 0x74A) || (canid == 0x77F))
	{
		DBG_Print(LEVEL2,"[CAN%d]RX:%X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",channel,canid,pMsg->Data[0],pMsg->Data[1],pMsg->Data[2],pMsg->Data[3],
									pMsg->Data[4],pMsg->Data[5],pMsg->Data[6],pMsg->Data[7]);
	}

	if (canid == 0x1811F428)
	{
		if (sandian_get_update_request_flag())
		{
			//仪表交互 同意升级
			if ((pMsg->Data[0] & 0x07) == 1)
			{
				sandian_set_update_request_flag(0); 		//交互同意了升级请求 就取消升级请求标志
				if (sandian_get_update_lingjian_id() == SANDIAN_UPDATE_LINGJIAN_ID_MCU)
				{
					sandian_mcu_restart_ota();
				}
				else if (sandian_get_update_lingjian_id() == SANDIAN_UPDATE_LINGJIAN_ID_VCU)
				{
					sandian_vcu1_restart_ota();
				}
				else if (sandian_get_update_lingjian_id() == SANDIAN_UPDATE_LINGJIAN_ID_BMS)
				{
					sandian_bms1_restart_ota();
				}
				
			}
		}
	}

	sandian_flow_ctrl_ack(channel,canid,pMsg->Data,8);

	if ((GetRunTime() >= 10) && ((GetRunTime() - s_sandian_dbg_last_time) >= 3))
	{
		s_ack_ver_cnt++;
		if (s_ack_ver_cnt <= 3)
		{
			if ((get_engine_type() == EngineType_EV18) || (get_engine_type() == EngineType_EM26) || (get_engine_type() == EngineType_DFAC))
			{
				sandian_tx(0,0x7DF,"\x03\x22\xF1\x95\xFF\xFF\xFF\xFF",8);
				sandian_tx(1,0x7DF,"\x03\x22\xF1\x95\xFF\xFF\xFF\xFF",8);
				sandian_tx(2,0x7DF,"\x03\x22\xF1\x95\xFF\xFF\xFF\xFF",8);

				//sandian_tx(0,0x0005,"\x55\x55\xAA\xAA\xAA\xAA\x55\x55",8);
				//sandian_tx(1,0x0005,"\x55\x55\xAA\xAA\xAA\xAA\x55\x55",8);
				//sandian_tx(2,0x0005,"\x55\x55\xAA\xAA\xAA\xAA\x55\x55",8);
			}
		}

		//DBG_Print(LEVEL2,"can fps:%u %u %u sos:%08X\r\n",get_can_rx_fps(0),get_can_rx_fps(1),get_can_rx_fps(2),gb32960_can_get_sos_flag());

		s_sandian_dbg_last_time = GetRunTime();
	}

	analyze_vcu_ver(canid,pMsg->Data,8);
	analyze_mcu_ver(canid,pMsg->Data,8);
	analyze_bms_ver(canid,pMsg->Data,8);

	vcu1_ota_proc(channel,pMsg);
	huichuan_mcu_ota_proc(channel,pMsg);
	//ningde_bms_ota_proc(channel,pMsg);
	lidong_bms_ota_proc(channel,pMsg);
}


void sandian_proc(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char flag = 0;
	static unsigned char s_vcu_ver[17] = {0};
	static unsigned char s_mcu_ver[17] = {0};
	static unsigned char s_bms_ver[17] = {0};
	static unsigned int s_sandian_last_time = 0;

	//版本发生变化需要上报平台
	if ((GetRunTime() - s_sandian_last_time) >= 600)
	{
		if (memcmp(s_vcu_ver,get_vcu_ver_ptr(),16))
		{
			flag = 1;
			memcpy(s_vcu_ver,get_vcu_ver_ptr(),16);
		}
		
		if (memcmp(s_mcu_ver,get_mcu_ver_ptr(),16))
		{
			flag = 1;
			memcpy(s_mcu_ver,get_mcu_ver_ptr(),16);
		}
		
		if (memcmp(s_bms_ver,get_bms_ver_ptr(),16))
		{
			flag = 1;
			memcpy(s_bms_ver,get_bms_ver_ptr(),16);
		}
		
		if (flag)
		{
			pbuf = os_malloc(1536);
			if (pbuf)
			{
				rlen = xGB32960_DongfengNewBasicPack(pbuf,1536,GetVIN(),
													get_rtc_time_ptr(),
													send_msg(S_MSG_GET_ICCID,0,0,0),20,
													GetVIN(),17);
		
				send_msg(S_MSG_SEND_TO_SERVER,(void *)0,pbuf,(void *)rlen);
				DBG_Print(LEVEL2,"(link%u)sandian ver change.df new basic info(%uBytes)\r\n",0,rlen);
				DBG_PrintBuf(LEVEL2,0,pbuf,rlen);
			
				os_free(pbuf);
			}
		}
		s_sandian_last_time = GetRunTime();
	}

}

void send_info_to_yibiao(void)
{
	CanTxMsg statusmsg = {0};
	unsigned char condition = 0;
	unsigned acc_on_flag = 0;
	unsigned soc_flag = 0;
	unsigned stop_flag = 0;
	unsigned shousha_flag = 1;
	unsigned dangwei_flag = 1;
	unsigned can_tongxin_flag = 0;
	unsigned main_power_flag = 0;
	unsigned charge_flag = 0;
	unsigned int main_power = 0;
	static unsigned char s_send_to_yb_flag = 0;

	if (stSANDIAN_YIBIAO_DISP.request_update_flag == 1)
	{
		s_send_to_yb_flag = 1;
	}

	if (s_send_to_yb_flag == 0)
	{
		return ;
	}

	statusmsg.IDE = CAN_Id_Extended;
	statusmsg.RTR = CAN_RTR_Data;
	statusmsg.StdId = 0;
	statusmsg.ExtId = 0x1812D4F5;
	statusmsg.DLC = 8;

	main_power = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);
	acc_on_flag = (unsigned int)send_msg(S_MSG_GET_ACC_STATUS,0,0,0);

	if (gb32960_can_get_soc() != 0xFF)
	{
		if (gb32960_can_get_soc() >= 60)
		{
			soc_flag = 1;
		}
		else
		{
			soc_flag = 0;
		}
	}
	else
	{
		soc_flag = 1;
	}

	//判断是否静止
	if (gb32960_can_get_speed() != 0xFFFF)
	{
		if (gb32960_can_get_speed() >= 50)
		{
			stop_flag = 0;
		}
		else
		{
			stop_flag = 1;
		}
	}
	else
	{
		stop_flag = 1;
	}

	//充电状态
	if ((gb32960_can_get_charge_status() == 1) || (gb32960_can_get_charge_status() == 2))
	{
		charge_flag = 1;
	}
	else
	{
		charge_flag = 0;
	}

	can_tongxin_flag = (get_can_no_data_time() > 3) ? 0 : 1;
	main_power_flag = (main_power >= 11300) ? 1 : 0;

	if (acc_on_flag && (charge_flag == 0) && stop_flag && can_tongxin_flag && main_power_flag && shousha_flag && dangwei_flag)
	{
		condition = 1;
	}
	else
	{
		condition = 0;
	}

	statusmsg.Data[0] = stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_TYPE | (stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_WAY << 2)
						| (stSANDIAN_YIBIAO_DISP.eSANDIAN_UPDATE_STATUS << 4) | (condition << 6);
	statusmsg.Data[1] = stSANDIAN_YIBIAO_DISP.request_update_flag;
	statusmsg.Data[2] = acc_on_flag | (soc_flag << 1) | (stop_flag << 2) | (shousha_flag << 3)
						| (dangwei_flag << 4) | (can_tongxin_flag << 5) | (main_power_flag << 6) | (charge_flag << 7);
	statusmsg.Data[3] = sandian_get_update_lingjian_id();
	statusmsg.Data[7] = sandian_get_upgrade_progress();

	can_send_data(0,&statusmsg);
	DBG_PrintBuf(LEVEL2,"[can]send to yibiao[0x1812D4F5]",statusmsg.Data,8);
}


void can_bus_transfer_file_proc1(CAN_BUS_OTA *p_can_bus_ota,CanRxMsg *pMsg,unsigned int file_type,CAN_BUS_OTA_STEP next_step)
{
	unsigned int canid = 0;
	unsigned int templen = 0;
	unsigned char dat[8] = {0};

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}

	switch (p_can_bus_ota->cur_status)
	{
	case 0:		
		templen = p_can_bus_ota->cur_tx_app_block_idx;
		dat[0] = 0x10;
		dat[1] = 0x0B;
		dat[2] = 0x34;
		dat[3] = 0x00;
		dat[4] = 0x44;
		dat[5] = p_can_bus_ota->app_blk[templen].start_addr >> 24;
		dat[6] = p_can_bus_ota->app_blk[templen].start_addr >> 16;
		dat[7] = p_can_bus_ota->app_blk[templen].start_addr >> 8;
		sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8); 				//连续帧 内存地址+长度
		p_can_bus_ota->cur_status = 1;
		break;
	case 1:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
		{
			templen = p_can_bus_ota->cur_tx_app_block_idx;
			p_can_bus_ota->app_bag_id = 0;
			p_can_bus_ota->app_tx_len = 0;
			p_can_bus_ota->app_len = p_can_bus_ota->app_blk[templen].block_size;

			dat[0] = 0x21;
			dat[1] = p_can_bus_ota->app_blk[templen].start_addr;

			dat[2] = p_can_bus_ota->app_blk[templen].block_size >> 24;
			dat[3] = p_can_bus_ota->app_blk[templen].block_size >> 16;
			dat[4] = p_can_bus_ota->app_blk[templen].block_size >> 8;
			dat[5] = p_can_bus_ota->app_blk[templen].block_size;

			dat[6] = 0x00;
			dat[7] = 0x00;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8); 				//连续帧 内存地址+长度

			p_can_bus_ota->cur_status = 2;
		}
	break;
	case 2:

	if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x03) && (pMsg->Data[1] == 0x7F) && (pMsg->Data[2] == 0x34) && (pMsg->Data[3] == 0x24))
	{
		os_sleep(8000);
		sandian_restart_ota(p_can_bus_ota);
	}

	if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x04) && (pMsg->Data[1] == 0x74) && (pMsg->Data[2] == 0x20))
	{
		p_can_bus_ota->transfer_max_len = (pMsg->Data[3] << 8) + pMsg->Data[4];
		DBG_Print(LEVEL2,"onece max transfer len:%uByes\r\n",p_can_bus_ota->transfer_max_len);		//MCU响应数据一次最大传输长度

		if (p_can_bus_ota->transfer_max_len >= 2048)
		{
			p_can_bus_ota->transfer_max_len = 2048;
		}
		else if (p_can_bus_ota->transfer_max_len >= 512)
		{
			p_can_bus_ota->transfer_max_len = 512;
		}
		else if (p_can_bus_ota->transfer_max_len >= 256)
		{
			p_can_bus_ota->transfer_max_len = 256;
		}
		else
		{
			p_can_bus_ota->transfer_max_len = 256;
		}

		p_can_bus_ota->next_app_bag_flag = 1;
		send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->app_offset),(void *)(p_can_bus_ota->transfer_max_len));
	}

	if (p_can_bus_ota->transfer_max_len && (p_can_bus_ota->app_tx_len < p_can_bus_ota->app_len))
	{
		unsigned int i = 0;

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x76))
		{
			p_can_bus_ota->next_app_bag_flag = 1;

			p_can_bus_ota->app_tx_len += p_can_bus_ota->app_cur_tx_len;

			if (p_can_bus_ota->app_tx_len >= p_can_bus_ota->app_len)											//传输完毕 退出
			{
				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x01\x37\xFF\xFF\xFF\xFF\xFF\xFF",8);

				p_can_bus_ota->cur_tx_app_block_idx++;
				if (p_can_bus_ota->cur_tx_app_block_idx >= p_can_bus_ota->app_block_cnt)
				{
					p_can_bus_ota->can_ota_step = next_step;
					return ;
				}

				p_can_bus_ota->cur_status = 0;

				DBG_Print(LEVEL2,"app transfer finish\r\n");
				p_can_bus_ota->next_app_bag_flag = 0;
				p_can_bus_ota->transfer_max_len = 0;
			}
		}

		if (p_can_bus_ota->next_app_bag_flag && p_can_bus_ota->app_data_arrive_flag)
		{
			p_can_bus_ota->next_app_bag_flag = 0;

			if ((p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len) <= 5)
			{
				p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len;
	
				dat[0] = p_can_bus_ota->app_cur_tx_len + 2;
				dat[1] = 0x36;
				dat[2] = 0x01 + p_can_bus_ota->app_bag_id;
				
				dat[3] = p_can_bus_ota->buf[0]; 							//data										
				dat[4] = p_can_bus_ota->buf[1];
				dat[5] = p_can_bus_ota->buf[2];
				dat[6] = p_can_bus_ota->buf[3];
				dat[7] = p_can_bus_ota->buf[4];

				p_can_bus_ota->app_data_arrive_flag = 0;
				p_can_bus_ota->app_offset += p_can_bus_ota->app_cur_tx_len;
				send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->app_offset),(void *)(p_can_bus_ota->transfer_max_len));
			}
			else
			{
				if ((p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len) > p_can_bus_ota->transfer_max_len)
				{
					p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->transfer_max_len;
				}
				else
				{
					p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len;
				}

				dat[0] = ((p_can_bus_ota->app_cur_tx_len + 2) >> 8) | 0x10;
				dat[1] = p_can_bus_ota->app_cur_tx_len + 2;
				dat[2] = 0x36;
				dat[3] = 0x01 + p_can_bus_ota->app_bag_id;
	
				dat[4] = p_can_bus_ota->buf[0]; 							//data										
				dat[5] = p_can_bus_ota->buf[1];
				dat[6] = p_can_bus_ota->buf[2];
				dat[7] = p_can_bus_ota->buf[3];
			}

			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
			p_can_bus_ota->last_send_cmd_time = GetRunTime();
			p_can_bus_ota->app_bag_id++;
		}

		if ((canid == p_can_bus_ota->response_id) && ((pMsg->Data[0] & 0xF0) == 0x30))
		{

			templen = 4;
			i = 0;
			while (templen < p_can_bus_ota->app_cur_tx_len)
			{
				dat[0] = 0x20 + ((i+1) % 16);
				dat[1] = p_can_bus_ota->buf[4 + (i * 7) + 0];
				dat[2] = p_can_bus_ota->buf[4 + (i * 7) + 1];
				dat[3] = p_can_bus_ota->buf[4 + (i * 7) + 2];
				dat[4] = p_can_bus_ota->buf[4 + (i * 7) + 3];									
				dat[5] = p_can_bus_ota->buf[4 + (i * 7) + 4];
				dat[6] = p_can_bus_ota->buf[4 + (i * 7) + 5];
				dat[7] = p_can_bus_ota->buf[4 + (i * 7) + 6];

				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);

				templen += 7;
				i++;
			}
			p_can_bus_ota->app_data_arrive_flag = 0;
			p_can_bus_ota->app_offset += p_can_bus_ota->app_cur_tx_len;
			send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->app_offset),(void *)(p_can_bus_ota->transfer_max_len));
		}
	}
	break;
	case 3:

	break;
	default:break;
	}	
}



void can_bus_transfer_file_proc(CAN_BUS_OTA *p_can_bus_ota,CanRxMsg *pMsg,unsigned int file_type,CAN_BUS_OTA_STEP next_step)
{
	unsigned int canid = 0;
	unsigned int templen = 0;
	unsigned char dat[8] = {0};

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}

	//file_type
	//11 21 31 属于驱动类型
	//12 22 32 属于APP类型

	if ((file_type % 10) == 1)						//drv 下载
	{
		if (p_can_bus_ota->drv_len == 0)
		{
			p_can_bus_ota->can_ota_step = next_step;
			return ;
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x04) && (pMsg->Data[1] == 0x74) && (pMsg->Data[2] == 0x20))
		{
			p_can_bus_ota->transfer_max_len = (pMsg->Data[3] << 8) + pMsg->Data[4];
			DBG_Print(LEVEL2,"onece max transfer len:%uByes\r\n",p_can_bus_ota->transfer_max_len);		//MCU响应数据一次最大传输长度

			if (p_can_bus_ota->transfer_max_len >= 2048)
			{
				p_can_bus_ota->transfer_max_len = 2048;
			}
			else if (p_can_bus_ota->transfer_max_len >= 512)
			{
				p_can_bus_ota->transfer_max_len = 512;
			}
			else if (p_can_bus_ota->transfer_max_len >= 256)
			{
				p_can_bus_ota->transfer_max_len = 256;
			}
			else
			{
				p_can_bus_ota->transfer_max_len = 256;
			}

			p_can_bus_ota->next_drv_bag_flag = 1;
			send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->drv_offset),(void *)(p_can_bus_ota->transfer_max_len));
			p_can_bus_ota->drv_offset += p_can_bus_ota->transfer_max_len;
		}
		
		if (p_can_bus_ota->transfer_max_len && (p_can_bus_ota->drv_tx_len < p_can_bus_ota->drv_len))
		{
			unsigned int i = 0;
		
			if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x76))
			{
				p_can_bus_ota->next_drv_bag_flag = 1;
		
				p_can_bus_ota->drv_tx_len += p_can_bus_ota->drv_cur_tx_len;
		
				if (p_can_bus_ota->drv_tx_len >= p_can_bus_ota->drv_len)											//传输完毕 退出
				{
					sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x01\x37\xFF\xFF\xFF\xFF\xFF\xFF",8);
		
					p_can_bus_ota->can_ota_step = next_step;
		
					DBG_Print(LEVEL2,"drv transfer finish\r\n");
					p_can_bus_ota->next_drv_bag_flag = 0;
					p_can_bus_ota->transfer_max_len = 0;
				}
			}
		
			if (p_can_bus_ota->next_drv_bag_flag && p_can_bus_ota->drv_data_arrive_flag)
			{
				p_can_bus_ota->next_drv_bag_flag = 0;
		
				if ((p_can_bus_ota->drv_len - p_can_bus_ota->drv_tx_len) <= 5)
				{
					p_can_bus_ota->drv_cur_tx_len = p_can_bus_ota->drv_len - p_can_bus_ota->drv_tx_len;
		
					dat[0] = p_can_bus_ota->drv_cur_tx_len + 2;
					dat[1] = 0x36;
					dat[2] = 0x01 + p_can_bus_ota->drv_bag_id;
					
					dat[3] = p_can_bus_ota->buf[0];								//data										
					dat[4] = p_can_bus_ota->buf[1];
					dat[5] = p_can_bus_ota->buf[2];
					dat[6] = p_can_bus_ota->buf[3];
					dat[7] = p_can_bus_ota->buf[4];

					p_can_bus_ota->drv_offset += p_can_bus_ota->drv_cur_tx_len;
				}
				else
				{
					if ((p_can_bus_ota->drv_len - p_can_bus_ota->drv_tx_len) > p_can_bus_ota->transfer_max_len)
					{
						p_can_bus_ota->drv_cur_tx_len = p_can_bus_ota->transfer_max_len;
					}
					else
					{
						p_can_bus_ota->drv_cur_tx_len = p_can_bus_ota->drv_len - p_can_bus_ota->drv_tx_len;
					}
					
					dat[0] = ((p_can_bus_ota->drv_cur_tx_len + 2) >> 8) | 0x10;
					dat[1] = p_can_bus_ota->drv_cur_tx_len + 2;
					dat[2] = 0x36;
					dat[3] = 0x01 + p_can_bus_ota->drv_bag_id;
		
					dat[4] = p_can_bus_ota->buf[0];								//data										
					dat[5] = p_can_bus_ota->buf[1];
					dat[6] = p_can_bus_ota->buf[2];
					dat[7] = p_can_bus_ota->buf[3];
				}
		
				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
				p_can_bus_ota->drv_bag_id++;
			}
		
			if ((canid == p_can_bus_ota->response_id) && ((pMsg->Data[0] & 0xF0) == 0x30))
			{
				templen = 4;
				i = 0;
				while (templen < p_can_bus_ota->drv_cur_tx_len)
				{
					dat[0] = 0x20 + ((i+1) % 16);
					dat[1] = p_can_bus_ota->buf[4 + (i * 7) + 0];
					dat[2] = p_can_bus_ota->buf[4 + (i * 7) + 1];
					dat[3] = p_can_bus_ota->buf[4 + (i * 7) + 2];
					dat[4] = p_can_bus_ota->buf[4 + (i * 7) + 3]; 									
					dat[5] = p_can_bus_ota->buf[4 + (i * 7) + 4];
					dat[6] = p_can_bus_ota->buf[4 + (i * 7) + 5];
					dat[7] = p_can_bus_ota->buf[4 + (i * 7) + 6];
		
					sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
		
					templen += 7;
					i++;

					//if ((i % 16) == 0) os_sleep(5);
				}
				
				p_can_bus_ota->drv_data_arrive_flag = 0;
				send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->drv_offset),(void *)(p_can_bus_ota->transfer_max_len));
				p_can_bus_ota->drv_offset += p_can_bus_ota->drv_cur_tx_len;
			}
		}

	}
	else if ((file_type % 10) == 2)
	{
		if (p_can_bus_ota->app_len == 0)
		{
			p_can_bus_ota->can_ota_step = next_step;
			return ;
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x04) && (pMsg->Data[1] == 0x74) && (pMsg->Data[2] == 0x20))
		{
			p_can_bus_ota->transfer_max_len = (pMsg->Data[3] << 8) + pMsg->Data[4];
			DBG_Print(LEVEL2,"onece max transfer len:%uByes\r\n",p_can_bus_ota->transfer_max_len);		//MCU响应数据一次最大传输长度

			if (p_can_bus_ota->transfer_max_len >= 2048)
			{
				p_can_bus_ota->transfer_max_len = 2048;
			}
			else if (p_can_bus_ota->transfer_max_len >= 512)
			{
				p_can_bus_ota->transfer_max_len = 512;
			}
			else if (p_can_bus_ota->transfer_max_len >= 256)
			{
				p_can_bus_ota->transfer_max_len = 256;
			}
			else
			{
				p_can_bus_ota->transfer_max_len = 256;
			}

			p_can_bus_ota->next_app_bag_flag = 1;
			send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->app_offset),(void *)(p_can_bus_ota->transfer_max_len));
			p_can_bus_ota->app_offset += p_can_bus_ota->transfer_max_len;
		}
		
		if (p_can_bus_ota->transfer_max_len && (p_can_bus_ota->app_tx_len < p_can_bus_ota->app_len))
		{
			unsigned int i = 0;
		
			if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x76))
			{
				p_can_bus_ota->next_app_bag_flag = 1;
		
				p_can_bus_ota->app_tx_len += p_can_bus_ota->app_cur_tx_len;

				if (p_can_bus_ota->app_tx_len >= p_can_bus_ota->app_len)											//传输完毕 退出
				{
					sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x01\x37\xFF\xFF\xFF\xFF\xFF\xFF",8);
		
					p_can_bus_ota->can_ota_step = next_step;
		
					DBG_Print(LEVEL2,"app transfer finish\r\n");
					p_can_bus_ota->next_app_bag_flag = 0;
					p_can_bus_ota->transfer_max_len = 0;
				}
			}

			if ((GetRunTime() - p_can_bus_ota->last_send_cmd_time) >= 8)
			{
				DBG_Print(LEVEL2,"sandian ack timeout need restart\r\n");
				//sandian_mcu_restart_ota();
				return ;
			}

			if (p_can_bus_ota->next_app_bag_flag && p_can_bus_ota->app_data_arrive_flag)
			{
				p_can_bus_ota->next_app_bag_flag = 0;
		
				if ((p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len) <= 5)
				{
					p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len;
		
					dat[0] = p_can_bus_ota->app_cur_tx_len + 2;
					dat[1] = 0x36;
					dat[2] = 0x01 + p_can_bus_ota->app_bag_id;
					
					dat[3] = p_can_bus_ota->buf[0];								//data										
					dat[4] = p_can_bus_ota->buf[1];
					dat[5] = p_can_bus_ota->buf[2];
					dat[6] = p_can_bus_ota->buf[3];
					dat[7] = p_can_bus_ota->buf[4];
				}
				else
				{
					if ((p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len) > p_can_bus_ota->transfer_max_len)
					{
						p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->transfer_max_len;
					}
					else
					{
						p_can_bus_ota->app_cur_tx_len = p_can_bus_ota->app_len - p_can_bus_ota->app_tx_len;
					}
					
					dat[0] = ((p_can_bus_ota->app_cur_tx_len + 2) >> 8) | 0x10;
					dat[1] = p_can_bus_ota->app_cur_tx_len + 2;
					dat[2] = 0x36;
					dat[3] = 0x01 + p_can_bus_ota->app_bag_id;
		
					dat[4] = p_can_bus_ota->buf[0];								//data										
					dat[5] = p_can_bus_ota->buf[1];
					dat[6] = p_can_bus_ota->buf[2];
					dat[7] = p_can_bus_ota->buf[3];
				}

				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
				p_can_bus_ota->last_send_cmd_time = GetRunTime();
				p_can_bus_ota->app_bag_id++;
			}

			if ((canid == p_can_bus_ota->response_id) && ((pMsg->Data[0] & 0xF0) == 0x30))
			{
				templen = 4;
				i = 0;
				while (templen < p_can_bus_ota->app_cur_tx_len)
				{
					dat[0] = 0x20 + ((i+1) % 16);
					dat[1] = p_can_bus_ota->buf[4 + (i * 7) + 0];
					dat[2] = p_can_bus_ota->buf[4 + (i * 7) + 1];
					dat[3] = p_can_bus_ota->buf[4 + (i * 7) + 2];
					dat[4] = p_can_bus_ota->buf[4 + (i * 7) + 3]; 									
					dat[5] = p_can_bus_ota->buf[4 + (i * 7) + 4];
					dat[6] = p_can_bus_ota->buf[4 + (i * 7) + 5];
					dat[7] = p_can_bus_ota->buf[4 + (i * 7) + 6];
		
					sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);

					templen += 7;
					i++;

					//if ((i % 16) == 0) os_sleep(5);
				}

				p_can_bus_ota->app_data_arrive_flag = 0;
				send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(p_can_bus_ota->app_offset),(void *)(p_can_bus_ota->transfer_max_len));
				p_can_bus_ota->app_offset += p_can_bus_ota->transfer_max_len;
			}
		}

	}

}



void sandian_start_ota_step(CAN_BUS_OTA *p_can_bus_ota,unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len)
{
	memset(p_can_bus_ota,0,sizeof(CAN_BUS_OTA));

	p_can_bus_ota->can_ota_step = (CAN_BUS_OTA_STEP)step;
	if (p_can_bus_ota->can_ota_step == CAN_BUS_OTA_NONE) sandian_set_update_status(SANDIAN_UPDATE_STATUS_NONE);

	p_can_bus_ota->drv_addr = drv_addr;
	p_can_bus_ota->drv_len = drv_len;
	p_can_bus_ota->app_addr = app_addr;
	p_can_bus_ota->app_len = app_len;
}

unsigned int can_bus_get_sandian_ota_progress(CAN_BUS_OTA *p_can_bus_ota)
{
	unsigned int total_len = 0;
	unsigned int i = 0;

	if (p_can_bus_ota->can_ota_step <= CAN_MCU_OTA_NONE) return 0;

	for (i = 0; i < p_can_bus_ota->drv_block_cnt; i++)
	{
		total_len += p_can_bus_ota->drv_blk[i].block_size;
	}

	for (i = 0; i < p_can_bus_ota->app_block_cnt; i++)
	{
		total_len += p_can_bus_ota->app_blk[i].block_size;
	}

	if ((p_can_bus_ota->drv_block_cnt + p_can_bus_ota->app_block_cnt) > 0)
	{
		return ((p_can_bus_ota->drv_offset + p_can_bus_ota->app_offset) * 100) / total_len;
	}
	else
	{
		return 0;
	}
}


void sandian_start_ota_step1(CAN_BUS_OTA *p_can_bus_ota,unsigned int step,FileBlockInfo *drv_blk,unsigned int drv_blk_cnt,FileBlockInfo *app_blk,unsigned int app_blk_cnt)
{
	memset(p_can_bus_ota,0,sizeof(CAN_BUS_OTA));

	p_can_bus_ota->can_ota_step = (CAN_BUS_OTA_STEP)step;
	if (p_can_bus_ota->can_ota_step == CAN_BUS_OTA_NONE) sandian_set_update_status(SANDIAN_UPDATE_STATUS_NONE);

	if (drv_blk_cnt > 6) drv_blk_cnt = 6;
	if (app_blk_cnt > 12) app_blk_cnt = 12;

	if (drv_blk_cnt && drv_blk)
	{
		p_can_bus_ota->drv_block_cnt = drv_blk_cnt;
		memcpy(p_can_bus_ota->drv_blk,drv_blk,drv_blk_cnt * sizeof(FileBlockInfo));
		p_can_bus_ota->drv_addr = p_can_bus_ota->drv_blk[0].start_addr;
		p_can_bus_ota->drv_len = p_can_bus_ota->drv_blk[0].block_size;	
	}

	if (app_blk_cnt && app_blk)
	{
		p_can_bus_ota->app_block_cnt = app_blk_cnt;
		memcpy(p_can_bus_ota->app_blk,app_blk,app_blk_cnt * sizeof(FileBlockInfo));

		p_can_bus_ota->app_addr = p_can_bus_ota->app_blk[0].start_addr;
		p_can_bus_ota->app_len = p_can_bus_ota->app_blk[0].block_size;	
	}
}

void sandian_data_arrive(CAN_BUS_OTA *p_can_bus_ota,unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen)
{
	unsigned int templen = 0;

	if (offset <= (p_can_bus_ota->last_bag_arrive_offset + p_can_bus_ota->last_bag_arrive_len))
	{
		templen = (p_can_bus_ota->last_bag_arrive_offset + p_can_bus_ota->last_bag_arrive_len) - offset;
		if (datlen > templen) ld_Bzip2_Update(datlen - templen,(unsigned char *)pdat + templen);
	}

	if ((file_type % 10) == 1)
	{
		p_can_bus_ota->drv_data_arrive_flag = 1;
	}
	else if ((file_type % 10) == 2)
	{
		p_can_bus_ota->app_data_arrive_flag = 1;
	}

	memcpy(p_can_bus_ota->buf,pdat,(datlen >= sizeof(p_can_bus_ota->buf)) ? sizeof(p_can_bus_ota->buf) : datlen);
	p_can_bus_ota->last_bag_arrive_offset = offset;
	p_can_bus_ota->last_bag_arrive_len = datlen;
}

void sandian_restart_ota(CAN_BUS_OTA *p_can_bus_ota)
{
	p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_1;
	p_can_bus_ota->next_drv_bag_flag = 0;
	p_can_bus_ota->next_app_bag_flag = 0;
	p_can_bus_ota->drv_data_arrive_flag = 0;
	p_can_bus_ota->app_data_arrive_flag = 0;
	p_can_bus_ota->last_send_cmd_time = 0;
	p_can_bus_ota->cnt = 0;
	p_can_bus_ota->drv_offset = 0;
	p_can_bus_ota->app_offset = 0;
	p_can_bus_ota->drv_tx_len = 0;
	p_can_bus_ota->app_tx_len = 0;
	p_can_bus_ota->drv_cur_tx_len = 0;
	p_can_bus_ota->app_cur_tx_len = 0;
	p_can_bus_ota->drv_bag_id = 0;
	p_can_bus_ota->app_bag_id = 0;
	p_can_bus_ota->transfer_max_len = 0;			//一次最大传输长度
	p_can_bus_ota->cur_tx_app_block_idx = 0;
	p_can_bus_ota->cur_status = 0;

	p_can_bus_ota->last_bag_arrive_offset = 0;
	p_can_bus_ota->last_bag_arrive_len = 0;
}




void can_bus_ota_proc(CAN_BUS_OTA *p_can_bus_ota,char channel,CanRxMsg *pMsg)
{
	unsigned int templen = 0;
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

	if (canid == p_can_bus_ota->response_id)
	{
		p_can_bus_ota->channel = channel;
	}

	switch (p_can_bus_ota->can_ota_step)
	{
	case CAN_BUS_OTA_STEP_1:
	{
		sandian_set_update_status(SANDIAN_UPDATE_STATUS_ING);
		sandian_tx(0,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		sandian_tx(1,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		sandian_tx(2,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_1_WAIT_ACK;
	}
	break;
	case CAN_BUS_OTA_STEP_1_WAIT_ACK:
	case CAN_BUS_OTA_STEP_2:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x03))
		{
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x04\x31\x01\x02\x03\xFF\xFF\xFF",8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_2_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_2_WAIT_ACK:
	case CAN_BUS_OTA_STEP_3:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01))
		{
			sandian_tx(p_can_bus_ota->channel,0x7DF,"\x02\x85\x02\xFF\xFF\xFF\xFF\xFF",8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_3_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_3_WAIT_ACK:
	case CAN_BUS_OTA_STEP_4:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0xC5) && (pMsg->Data[2] == 0x02))
		{
			sandian_tx(p_can_bus_ota->channel,0x7DF,"\x03\x28\x03\x03\xFF\xFF\xFF\xFF",8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_4_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_4_WAIT_ACK:
	case CAN_BUS_OTA_STEP_5:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x68) && (pMsg->Data[2] == 0x03))
		{
			p_can_bus_ota->last_send_cmd_time = GetRunTime();
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x02\x10\x02\xFF\xFF\xFF\xFF\xFF",8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_5_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_5_WAIT_ACK:
	case CAN_BUS_OTA_STEP_6:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x02))
		{
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x02\x27\x09\xFF\xFF\xFF\xFF\xFF",8);		//获取安全访问种子
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_6_WAIT_ACK;
		}

		if ((GetRunTime() - p_can_bus_ota->last_send_cmd_time) >= 15)
		{
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_5;
		}

	break;

	case CAN_BUS_OTA_STEP_6_WAIT_ACK:
	case CAN_BUS_OTA_STEP_7:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x67) && (pMsg->Data[2] == 0x09))				//发送安全密钥
		{
			unsigned int temp = 0;

			DBG_Print(LEVEL2,"can ota seed:%02X %02X %02X %02X\r\n",pMsg->Data[3],pMsg->Data[4],pMsg->Data[5],pMsg->Data[6]);
			p_can_bus_ota->seed = (pMsg->Data[3] << 24) + (pMsg->Data[4] << 16) + (pMsg->Data[5] << 8) +pMsg->Data[6];			

			if (p_can_bus_ota->p_func_calc_key)
			{
				temp = p_can_bus_ota->p_func_calc_key(p_can_bus_ota->seed);
			}

			dat[0] = 0x06;
			dat[1] = 0x27;
			dat[2] = 0x0A;
			dat[3] = temp >> 24;
			dat[4] = temp >> 16;
			dat[5] = temp >> 8;
			dat[6] = temp;
			dat[7] = 0xFF;

			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);

			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_7_WAIT_ACK;

			p_can_bus_ota->last_bag_arrive_offset = 0;
			p_can_bus_ota->last_bag_arrive_len = 0;
			ld_Bzip2_Init();
			ld_Bzip2_Start();	
		}
	break;

	case CAN_BUS_OTA_STEP_7_WAIT_ACK:
	case CAN_BUS_OTA_STEP_8:														//下载驱动

		if (p_can_bus_ota->drv_len)
		{
			if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x67) && (pMsg->Data[2] == 0x0A))
			{
				dat[0] = 0x10;
				dat[1] = 0x0B;
				dat[2] = 0x34;
				dat[3] = 0x00;
				dat[4] = 0x44;
				dat[5] = p_can_bus_ota->drv_addr >> 24;
				dat[6] = p_can_bus_ota->drv_addr >> 16;
				dat[7] = p_can_bus_ota->drv_addr >> 8;
				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);					//连续帧 内存地址+长度
			}
			
			if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
			{
				dat[0] = 0x21;
				dat[1] = p_can_bus_ota->drv_addr;
				dat[2] = p_can_bus_ota->drv_len >> 24;
				dat[3] = p_can_bus_ota->drv_len >> 16;
				dat[4] = p_can_bus_ota->drv_len >> 8;
				dat[5] = p_can_bus_ota->drv_len;
				dat[6] = 0xFF;
				dat[7] = 0xFF;
				sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);					//连续帧 内存地址+长度
				p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_8_WAIT_ACK;
			}
		}
		else
		{
			dat[0] = 0x10;
			dat[1] = 0x0D;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xFF;
			dat[5] = 0x00;
			dat[6] = 0x44;
			dat[7] = p_can_bus_ota->app_addr >> 24;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//例行依赖性检查		
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_11;
		}
	break;

	case CAN_BUS_OTA_STEP_8_WAIT_ACK:
	case CAN_BUS_OTA_STEP_9:
		can_bus_transfer_file_proc(p_can_bus_ota,pMsg,11,CAN_BUS_OTA_STEP_9_WAIT_ACK);		//分段文件传输
	break;

	case CAN_BUS_OTA_STEP_9_WAIT_ACK:
	case CAN_BUS_OTA_STEP_10:																
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x77))
		{
			p_can_bus_ota->drv_check_sum = ld_Bzip2_XOR();

			//s_test_len = sprintf(s_test_tip,"crc:%08X %08X %08X\r\n",ld_Bzip2_XOR(),ld_Bzip2_XOR1(),ld_Bzip2_XOR2());
			//send_msg(S_MSG_SAVE_MCU_LOG,0,s_test_tip,(void *)s_test_len);
			//p_can_bus_ota->drv_check_sum = 0xAC95AFB6;

			DBG_Print(LEVEL2,"drv transfer finish exit.checksum:%08X\r\n",p_can_bus_ota->drv_check_sum);

			dat[0] = 0x10;
			dat[1] = 0x08;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0x02;
			dat[5] = 0x02;
			dat[6] = p_can_bus_ota->drv_check_sum >> 24;
			dat[7] = p_can_bus_ota->drv_check_sum >> 16;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//例行完整性检测
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = p_can_bus_ota->drv_check_sum >> 8;
			dat[2] = p_can_bus_ota->drv_check_sum;
			dat[3] = 0xFF;
			dat[4] = 0xFF;
			dat[5] = 0xFF;
			dat[6] = 0xFF;
			dat[7] = 0xFF;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//例行完整性检测
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_10_A;
		}
	break;

	case CAN_BUS_OTA_STEP_10_A:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01))
		{
			//10 0C 2E F1 5A 01 02 03 
			dat[0] = 0x10;
			dat[1] = 0x0C;
			dat[2] = 0x2E;
			dat[3] = 0xF1;
			dat[4] = 0x5A;
			dat[5] = 0x01;
			dat[6] = 0x02;
			dat[7] = 0x03;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//写入指纹
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = 0x04;
			dat[2] = 0x05;
			dat[3] = 0x06;
			dat[4] = 0x07;
			dat[5] = 0x08;
			dat[6] = 0x09;
			dat[7] = 0x00;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_11;
		}
	break;


	case CAN_BUS_OTA_STEP_10_WAIT_ACK:
	case CAN_BUS_OTA_STEP_11:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x6E) && (pMsg->Data[2] == 0xF1) && (pMsg->Data[3] == 0x5A))
		{
			dat[0] = 0x10;
			dat[1] = 0x0D;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xFF;
			dat[5] = 0x00;
			dat[6] = 0x44;
			dat[7] = p_can_bus_ota->app_addr >> 24;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);		
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = p_can_bus_ota->app_addr >> 16;
			dat[2] = p_can_bus_ota->app_addr >> 8;
			dat[3] = p_can_bus_ota->app_addr;

			if (p_can_bus_ota->app_block_cnt > 1)
			{
				templen = p_can_bus_ota->app_blk[p_can_bus_ota->app_block_cnt - 1].start_addr + p_can_bus_ota->app_blk[p_can_bus_ota->app_block_cnt - 1].block_size - p_can_bus_ota->app_blk[0].start_addr;

				dat[4] = templen >> 24;
				dat[5] = templen >> 16;
				dat[6] = templen >> 8;
				dat[7] = templen;
			}
			else
			{
				dat[4] = p_can_bus_ota->app_len >> 24;
				dat[5] = p_can_bus_ota->app_len >> 16;
				dat[6] = p_can_bus_ota->app_len >> 8;
				dat[7] = p_can_bus_ota->app_len;
			}

			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_11_WAIT_ACK;
			p_can_bus_ota->last_send_cmd_time = GetRunTime();
			p_can_bus_ota->cnt = 0;

			p_can_bus_ota->last_bag_arrive_offset = 0;
			p_can_bus_ota->last_bag_arrive_len = 0;
			ld_Bzip2_Init();
			ld_Bzip2_Start();
		}
	break;

	case CAN_BUS_OTA_STEP_11_WAIT_ACK:
	case CAN_BUS_OTA_STEP_12:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x7F) && (pMsg->Data[2] == 0x31) && (pMsg->Data[3] == 0x78))
		{
			p_can_bus_ota->last_send_cmd_time = 0;
		}

		if ((GetRunTime() - p_can_bus_ota->last_send_cmd_time) >= 2)
		{
			p_can_bus_ota->cnt++;
			if (p_can_bus_ota->cnt >= 15)
			{
				p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_12_WAIT_ACK;
			}
			else
			{
				sandian_tx(p_can_bus_ota->channel,0x7DF,"\x02\x3E\x80\x00\x00\x00\x00\x00",8);
			}
			p_can_bus_ota->last_send_cmd_time = GetRunTime();
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xFF))
		{
			p_can_bus_ota->cur_status = 0;
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_12_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_12_WAIT_ACK:
	case CAN_BUS_OTA_STEP_13:
		//can_bus_transfer_file_proc(p_can_bus_ota,pMsg,12,CAN_BUS_OTA_STEP_13_WAIT_ACK);
		can_bus_transfer_file_proc1(p_can_bus_ota,pMsg,12,CAN_BUS_OTA_STEP_13_WAIT_ACK);
	break;
	
	case CAN_BUS_OTA_STEP_13_WAIT_ACK:
	case CAN_BUS_OTA_STEP_14:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x77))
		{
			p_can_bus_ota->app_check_sum = ld_Bzip2_XOR();
			//p_can_bus_ota->app_check_sum = 0xD7CAE4CE;

			DBG_Print(LEVEL2,"app transfer finish exit.check sum:%08X\r\n",p_can_bus_ota->app_check_sum);

			dat[0] = 0x10;
			dat[1] = 0x08;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0x02;
			dat[5] = 0x02;
			dat[6] = p_can_bus_ota->app_check_sum >> 24;
			dat[7] = p_can_bus_ota->app_check_sum >> 16;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//例行完整性检测
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = p_can_bus_ota->app_check_sum >> 8;
			dat[2] = p_can_bus_ota->app_check_sum;
			dat[3] = 0xFF;
			dat[4] = 0xFF;
			dat[5] = 0xFF;
			dat[6] = 0xFF;
			dat[7] = 0xFF;
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,dat,8);									//例行完整性检测
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_14_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_14_WAIT_ACK:
	case CAN_BUS_OTA_STEP_15:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0x02) && (pMsg->Data[4] == 0x02))
		{
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x04\x31\x01\xFF\x01\xFF\xFF\xFF",8);	//例行依赖性检查

			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_15_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_15_WAIT_ACK:
	case CAN_BUS_OTA_STEP_16:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xFF) && (pMsg->Data[4] == 0x01))
		{
			DBG_Print(LEVEL1,"send mcu reset cmd\r\n");
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x02\x11\x01\xFF\xFF\xFF\xFF\xFF",8);	//ECU复位

			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_16_WAIT_ACK;
		}
	break;
	case CAN_BUS_OTA_STEP_16_WAIT_ACK:
	case CAN_BUS_OTA_STEP_17:
		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x51) && (pMsg->Data[2] == 0x01))
		{
			os_sleep(8000);			//等待8秒 给足ECU复位所需的时间
			sandian_tx(p_can_bus_ota->channel,0x7DF,"\x02\x10\x01\x00\x00\x00\x00\x00",8);	//进入诊断会话模式
		}

		if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x01))
		{
			sandian_tx(p_can_bus_ota->channel,p_can_bus_ota->phy_id,"\x04\x14\xFF\xFF\xFF\xFF\xFF\xFF",8);
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_17_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_17_WAIT_ACK:
	case CAN_BUS_OTA_STEP_18:
		//if ((canid == p_can_bus_ota->response_id) && (pMsg->Data[1] == 0x54))
		{
			DBG_Print(LEVEL1,"sandian mcu ota finish\r\n");

			sandian_set_update_status(SANDIAN_UPDATE_STATUS_SUCCESS);

			p_can_bus_ota->last_send_cmd_time = GetRunTime();
			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_STEP_18_WAIT_ACK;
		}
	break;

	case CAN_BUS_OTA_STEP_18_WAIT_ACK:
		if ((GetRunTime() - p_can_bus_ota->last_send_cmd_time) >= 8)
		{
			sandian_tx(p_can_bus_ota->channel,0x7DF,"\x03\x22\xF1\x95\xFF\xFF\xFF\xFF",8);

			p_can_bus_ota->can_ota_step = CAN_BUS_OTA_NONE;
		}
	break;
	default:break;
	}
}



