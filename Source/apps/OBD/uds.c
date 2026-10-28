#include "uds.h"

typedef struct tagISO27145Protocol
{
	SENDCANMSGPROC pSendCanMsgFunc;
	unsigned char need_send_flow_ctrl_flag;
}ISO27145Protocol;

ISO27145Protocol s_stISO27145Protocol = {0};



typedef enum tagCAN_ACK_INDEX
{
	OBD_PRO_INDEX = 0,
	MIL_INDEX,
	VIN_INDEX,
	CALID_INDEX,
	CVN_INDEX,
	IUPR_INDEX,
	DTC_INDEX,
	EV18_VCU_VER_INDEX
}CAN_ACK_INDEX;

unsigned char ISO27145_CAN_REQ_ACK_FLAG_LIST[8] = {0};

//CAN请求数据列表
const CanTxMsg ISO27145_CAN_REQ_LIST[7][8] = 
{
//obd pro
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x10,0xFF,0xFF,0xFF,0xFF}},//0,OBD协议0-15765,1-27145,2-1939
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x10,0xFF,0xFF,0xFF,0xFF}},//0,OBD协议0-15765,1-27145,2-1939
},

//mil dts drs
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF4,0x01,0xFF,0xFF,0xFF,0xFF}},//1,MIL 诊断支持列表 诊断就绪列表
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF4,0x01,0xFF,0xFF,0xFF,0xFF}},//1,MIL 诊断支持列表 诊断就绪列表

	{0,0x18EA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0xCE,0xFE,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}},//1,MIL 诊断支持列表 诊断就绪列表
},

//vin
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x02,0xFF,0xFF,0xFF,0xFF}},//2,VIN
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x02,0xFF,0xFF,0xFF,0xFF}},//2,VIN
	{0,0x18EA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0xEC,0xFE,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}},//2,VIN

},

//calid
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x04,0xFF,0xFF,0xFF,0xFF}},//3,CALID
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x04,0xFF,0xFF,0xFF,0xFF}},//3,CALID
	{0,0x18EA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x00,0xD3,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}},//3,CALID
},

//cvn
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x06,0xFF,0xFF,0xFF,0xFF}},//4,CVN
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x06,0xFF,0xFF,0xFF,0xFF}},//4,CVN
},

//iupr
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x0B,0xFF,0xFF,0xFF,0xFF}},//5,IUPR
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x03,0x22,0xF8,0x0B,0xFF,0xFF,0xFF,0xFF}},//5,IUPR
	{0,0x18EA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x00,0xC2,0x00,0xFF,0xFF,0xFF,0xFF,0xFF}},//5,IUPR
},

//dtc
{
	{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x05,0x19,0x42,0x33,0x08,0x1E,0xFF,0xFF}},//6,DTC
	{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x05,0x19,0x42,0x33,0x08,0x1E,0xFF,0xFF}},//6,DTC
},

};

//流控帧
CanTxMsg ISO27145_CAN_FLOW_CTRL_CMD[] = 
{
{0,0x18DA00F1,CAN_Id_Extended,CAN_RTR_DATA,8,{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},//全柴发动机 奔驰重卡
{0x7E0,0,CAN_Id_Standard,CAN_RTR_DATA,8,{0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},		//玉柴发动机

};

void iso27145_can_send_flow_ctrl_proc(void)
{
	unsigned char i = 0;
	unsigned char req_cnt = 0;

	if (s_stISO27145Protocol.pSendCanMsgFunc == 0)
	{
		return ;
	}

	if (s_stISO27145Protocol.need_send_flow_ctrl_flag)
	{
		req_cnt = sizeof(ISO27145_CAN_FLOW_CTRL_CMD) / sizeof(ISO27145_CAN_FLOW_CTRL_CMD[0]);
		for (i = 0; i < req_cnt; i++)
		{
			s_stISO27145Protocol.pSendCanMsgFunc(&ISO27145_CAN_FLOW_CTRL_CMD[i]);
		}
		s_stISO27145Protocol.need_send_flow_ctrl_flag = 0;
	}

}

void set_need_send_flow_ctrl_flag(unsigned char val)
{
	s_stISO27145Protocol.need_send_flow_ctrl_flag = val;
}

void iso27145_can_send_req_list(void)
{
	unsigned char req_cnt = 0;
	unsigned char i = 0;
	unsigned char every_item_req_cnt = 0;
	CanTxMsg *ptxmsg = 0;
	static unsigned char s_send_idx = 0;

	if (s_stISO27145Protocol.pSendCanMsgFunc == 0) return ;

	req_cnt = sizeof(ISO27145_CAN_REQ_LIST) / sizeof(ISO27145_CAN_REQ_LIST[0]);
	every_item_req_cnt = sizeof(ISO27145_CAN_REQ_LIST[s_send_idx]) / sizeof(ISO27145_CAN_REQ_LIST[s_send_idx][0]);

	if (ISO27145_CAN_REQ_ACK_FLAG_LIST[s_send_idx] == 0)		//该项数据有没有得到响应 继续请求
	{
		for (i = 0; i < every_item_req_cnt; i++)
		{
			ptxmsg = (CanTxMsg *)(&(ISO27145_CAN_REQ_LIST[s_send_idx][i]));
			if (ptxmsg->StdId || ptxmsg->ExtId) s_stISO27145Protocol.pSendCanMsgFunc(ptxmsg);
		}
	}

	s_send_idx++;
	if (s_send_idx >= req_cnt)
	{
		s_send_idx = 0;
	}
}

void uds_can_send_proc(void)
{
	static unsigned char s_can_send_proc_cnt = 0;

	s_can_send_proc_cnt++;
	if (s_can_send_proc_cnt >= 15)
	{
		iso27145_can_send_req_list();
	
		s_can_send_proc_cnt = 0;
	}

	//iso27145_can_send_flow_ctrl_proc();
}

char is_calid_request_ok(void)
{
	return ISO27145_CAN_REQ_ACK_FLAG_LIST[CALID_INDEX];
}

void uds_init(SENDCANMSGPROC SendCanMsgFunc)
{
	s_stISO27145Protocol.pSendCanMsgFunc = SendCanMsgFunc;
	s_stISO27145Protocol.need_send_flow_ctrl_flag = 0;
}

//对接收的CAN数据进行解析
void uds_analysis(char channel,CanRxMsg *pMsg)
{
	unsigned char *data = 0;
	unsigned char i = 0;
	unsigned char temp = 0;
	unsigned int canid = 0;
	static unsigned char FCD_Flag = 0;

	if (pMsg == 0)
	{
		return ;
	}

	data = pMsg->Data;
	canid = pMsg->IDE ? pMsg->ExtId : pMsg->StdId;

	switch (canid)
	{
	case 0x18FD0700:					//mil
		can_set_mil_sta(channel,data[0]);
	break;
	case 0x18FECE00:					//dts drs
		ISO27145_CAN_REQ_ACK_FLAG_LIST[MIL_INDEX] = 1;

	break;
	case 0x18FEEC00:					//vin
	if ((data[0] & 0xF0) == 0x20)
	{
		temp = ((data[0] & 0x0F) - 1) * 7;
		for(i = 0; i < 7; i++)
		{
			can_set_vin(channel,temp + i,data[1 + i]);
		}
		if ((data[0] & 0x0F) == 3) ISO27145_CAN_REQ_ACK_FLAG_LIST[VIN_INDEX] = 1;
	}
	break;
	case 0x18D30000:					//calid

	ISO27145_CAN_REQ_ACK_FLAG_LIST[CALID_INDEX] = 1;
	break;

	case 0x18C20000:					//iupr
		ISO27145_CAN_REQ_ACK_FLAG_LIST[IUPR_INDEX] = 1;
	break;
	default:break;
	}

	if ((canid != 0x7E8) && (canid != 0x7EF) && (canid != 0x18DAF100))
	{
		return ;
	}

	//数据解析
	if((data[0] & 0x0F) && (data[1] == 0x62) && (data[2] == 0xF4) && (data[3] == 0x01))
	{
		can_set_mil_sta(channel,data[4] & 0x01);
		can_set_dst_sta(channel,byte_to_int16u(data[5] & 0x07,data[6]));
		can_set_drt_sta(channel,byte_to_int16u((data[5] & 0x70) >> 4,data[7]));	

		ISO27145_CAN_REQ_ACK_FLAG_LIST[MIL_INDEX] = 1;
	}
	else if((data[0] & 0x0F) && (data[1] == 0x62) && (data[2] == 0xF8) && (data[3] == 0x10))
	{
		can_set_obd_pro(channel,data[4]);

		ISO27145_CAN_REQ_ACK_FLAG_LIST[OBD_PRO_INDEX] = 1;
	}
	else if((data[0] <= 7) && (data[1] == 0x62) && (data[2] == 0xF8) && (data[3] == 0x04)) //CALID
	{
		ISO27145_CAN_REQ_ACK_FLAG_LIST[CALID_INDEX] = 1;
		if (data[0] > 3)
		{
			for (i = 0; i < (data[0] - 3); i++)
			{
				can_set_calid(channel,i,data[4 + i]);
			}
		}
	}
	else if((data[0] <= 7) && (data[1] == 0x62) && (data[2] == 0xF8) && (data[3] == 0x06)) //CVN
	{
		ISO27145_CAN_REQ_ACK_FLAG_LIST[CVN_INDEX] = 1;
		if (data[0] > 3)
		{
			for (i = 0; i < (data[0] - 3); i++)
			{
				can_set_cvn_by_byte(channel,i,data[4 + i]);
			}
		}
	}
	///////////////////连续帧/////////////////////////////////////////////////////
	else if((data[0] & 0xF0) == 0x10)			//高四位为1 表示连续帧 首帧 需要发送流控制请求后续帧数据
	{
		s_stISO27145Protocol.need_send_flow_ctrl_flag = 1;

		if((data[2] == 0x62) && (data[3] == 0xF8) && (data[4] == 0x02))	//VIN
		{
			FCD_Flag = 1;

			can_set_vin(channel,0,data[5]);
			can_set_vin(channel,1,data[6]);
			can_set_vin(channel,2,data[7]);
		}
		else if((data[2] == 0x62) && (data[3] == 0xF8) && (data[4] == 0x04)) //CALID
		{
			FCD_Flag = 2;

			can_set_calid(channel,0,data[5]);
			can_set_calid(channel,1,data[6]);
			can_set_calid(channel,2,data[7]);			
		}
		else if((data[2] == 0x62) && (data[3] == 0xF8) && (data[4] == 0x06)) //CVN
		{
			FCD_Flag = 3;

			can_set_cvn_by_byte(channel,0,data[5]);
			can_set_cvn_by_byte(channel,1,data[6]);
			can_set_cvn_by_byte(channel,2,data[7]);
		}
		else if((data[2] == 0x62) && (data[3] == 0xF8) && (data[4] == 0x0B))	//IUPR
		{
			FCD_Flag = 4;

			can_set_iupr(channel,0,data[5]);
			can_set_iupr(channel,1,data[6]);
			can_set_iupr(channel,2,data[7]);
		}
		else if((data[2] == 0x59) && (data[3] == 0x42) && (data[4] == 0x33))	//DTC
		{
			FCD_Flag = 8;
			if (data[1] > 6) can_set_dtc_cnt(channel,(data[1] - 6) / 5);

			ISO27145_CAN_REQ_ACK_FLAG_LIST[DTC_INDEX] = 1;
		}
		else if((data[2] == 0x62) && (data[3] == 0xF1) && (data[4] == 0x95))	//ev18 vcu ver
		{
			FCD_Flag = 9;

			clr_dst_vcu_ver();
			set_dst_vcu_ver(0,data[5]);
			set_dst_vcu_ver(1,data[6]);
			set_dst_vcu_ver(2,data[7]);
		}

		iso27145_can_send_flow_ctrl_proc();
	}
	else if((data[0] & 0xF0) == 0x20)			//连续帧
	{
		switch(FCD_Flag)
		{
			case 0x01:	//VIN
			temp = ((data[0] & 0x0F) - 1) * 7 + 3;
			for(i = 0; i < 7; i++)
			{
				can_set_vin(channel,temp+i,data[1+i]);
			}
			ISO27145_CAN_REQ_ACK_FLAG_LIST[VIN_INDEX] = 1;
			break;
			case 0x02:	//CALID
			temp = ((data[0] & 0x0F) - 1) *7 + 3;
			for(i = 0;i < 7; i++)
			{
				can_set_calid(channel,temp+i,data[1+i]);
			}
			ISO27145_CAN_REQ_ACK_FLAG_LIST[CALID_INDEX] = 1;
			break;
			case 0x03:	//CVN
			temp = ((data[0] & 0x0F) - 1) * 7 + 3;
			for (i = 0; i < 7; i++)
			{
				can_set_cvn_by_byte(channel,temp + i,data[1 + i]);
			}

			ISO27145_CAN_REQ_ACK_FLAG_LIST[CVN_INDEX] = 1;
			break;
			case 0x04:	//IUPR
			temp = ((data[0] & 0x0F) - 1) * 7 + 3;
			for(i = 0; i < 7; i++)
			{
				if((temp+i) > 35) break;
				can_set_iupr(channel,temp+i,data[1+i]);
			}
			ISO27145_CAN_REQ_ACK_FLAG_LIST[IUPR_INDEX] = 1;
			break;
			case 0x09:
				ISO27145_CAN_REQ_ACK_FLAG_LIST[EV18_VCU_VER_INDEX] = 1;
				temp = ((data[0] & 0x0F) - 1) * 7 + 3;
				for(i = 0; i < 7; i++)
				{
					set_dst_vcu_ver(temp+i,data[1+i]);
				}
			break;
			case 0x08:	//DTC
			switch ((data[0] & 0x0F) % 5)
			{
			case 0:
				temp = (data[0] & 0x0F) / 5;
				temp = 4 * 7 * temp + 0;
				can_set_dtc_buf(channel,temp++,data[2]);
				can_set_dtc_buf(channel,temp++,data[3]);
				can_set_dtc_buf(channel,temp++,data[4]);
				can_set_dtc_buf(channel,temp++,data[5]);
				can_set_dtc_buf(channel,temp++,data[7]);
			break;
			case 1:
				temp = (data[0] & 0x0F) / 5;
				temp = 4 * 7 * temp + 5;
				can_set_dtc_buf(channel,temp++,data[1]);
				can_set_dtc_buf(channel,temp++,data[2]);
				can_set_dtc_buf(channel,temp++,data[3]);
				can_set_dtc_buf(channel,temp++,data[5]);
				can_set_dtc_buf(channel,temp++,data[6]);
				can_set_dtc_buf(channel,temp++,data[7]);
			break;
			case 2:
				temp = (data[0] & 0x0F) / 5;
				temp = 4 * 7 * temp + 11;
				can_set_dtc_buf(channel,temp++,data[1]);
				can_set_dtc_buf(channel,temp++,data[3]);
				can_set_dtc_buf(channel,temp++,data[4]);
				can_set_dtc_buf(channel,temp++,data[5]);
				can_set_dtc_buf(channel,temp++,data[6]);
			break;
			case 3:
				temp = (data[0] & 0x0F) / 5;
				temp = 4 * 7 * temp + 16;
				can_set_dtc_buf(channel,temp++,data[1]);
				can_set_dtc_buf(channel,temp++,data[2]);
				can_set_dtc_buf(channel,temp++,data[3]);
				can_set_dtc_buf(channel,temp++,data[4]);
				can_set_dtc_buf(channel,temp++,data[6]);
				can_set_dtc_buf(channel,temp++,data[7]);
			break;
			case 4:
				temp = (data[0] & 0x0F) / 5;
				temp = 4 * 7 * temp + 22;
				can_set_dtc_buf(channel,temp++,data[1]);
				can_set_dtc_buf(channel,temp++,data[2]);
				can_set_dtc_buf(channel,temp++,data[4]);
				can_set_dtc_buf(channel,temp++,data[5]);
				can_set_dtc_buf(channel,temp++,data[6]);
				can_set_dtc_buf(channel,temp++,data[7]);
			break;

			default:break;
			}
			break;
			default:break;				
		}
	}

}



