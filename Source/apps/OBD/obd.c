#include "OBD.h"

//变量定义
//OBD数据缓存结构体
CanInfo s_astCanInfo[MAX_CAN_CNT] = {0};


void clr_obd_data(void)
{
	unsigned char i = 0;

	memset(s_astCanInfo,0xFF,sizeof(s_astCanInfo));

	for (i = 0; i < MAX_CAN_CNT; i++)
	{
		can_set_dtc_cnt(i,0);
		s_astCanInfo[i].VIN[VIN_LEN] = 0;
	}
}


INT16U byte_to_int16u(INT8U D0,INT8U D1)
{
	return byte_to_int32u(0,0,D0,D1);
}

INT32U byte_to_int32u(INT8U D0,INT8U D1,INT8U D2,INT8U D3)
{
	return ((D0 << 24) + (D1 << 16) + (D2 << 8) + D3);
}

void can_set_speed(char channel,INT16U val)
{
	if (s_astCanInfo[channel].usSpeed != val)
	{
		s_astCanInfo[channel].usSpeed = val;
	}
}

INT16U can_get_speed(char channel)
{
	return s_astCanInfo[channel].usSpeed;
}

void can_set_kpa(char channel,INT8U val)
{
	if (s_astCanInfo[channel].uckPa != val)
	{
		s_astCanInfo[channel].uckPa = val;
	}
}

INT8U can_get_kpa(char channel)
{
	return s_astCanInfo[channel].uckPa;
}

void can_set_actualtorquepercent(char channel,INT8U val)
{
	if (s_astCanInfo[channel].cActualTorquePercent != val)
	{
		s_astCanInfo[channel].cActualTorquePercent = val;
	}
}

INT8U can_get_actualtorquepercent(char channel)
{
	return s_astCanInfo[channel].cActualTorquePercent;
}

void can_set_frictiontorquepercent(char channel,INT8U val)
{
	if (s_astCanInfo[channel].cFrictionTorquePercent != val)
	{
		s_astCanInfo[channel].cFrictionTorquePercent = val;
	}
}

INT8U can_get_frictiontorquepercent(char channel)
{
	return s_astCanInfo[channel].cFrictionTorquePercent;
}


INT16U can_get_rpm(char channel)
{
	return s_astCanInfo[channel].usRPM;
}

void can_set_rpm(char channel,INT16U val)
{
	if (s_astCanInfo[channel].usRPM != val)
	{
		s_astCanInfo[channel].usRPM = val;
	}
}

void can_set_fuelflow(char channel,INT16U val)
{
	if (s_astCanInfo[channel].usFuelFlow != val)
	{
		s_astCanInfo[channel].usFuelFlow = val;
	}
}

INT16U can_get_fuelflow(char channel)
{
	return s_astCanInfo[channel].usFuelFlow;
}

void can_set_milage(char channel,INT32U val)
{
	if (s_astCanInfo[channel].ulMilage != val)
	{
		s_astCanInfo[channel].ulMilage = val;
	}
}

INT32U can_get_milage(char channel)

{
	return s_astCanInfo[channel].ulMilage;
}

void can_set_oilboxlevel(char channel,INT8U val)
{
	if (s_astCanInfo[channel].ucLevel != val)
	{
		s_astCanInfo[channel].ucLevel = val;
	}
}

INT8U can_get_oilboxlevel(char channel)
{
	return s_astCanInfo[channel].ucLevel;
}


void can_set_coolanttemperature(char channel,INT8U val)
{
	if (s_astCanInfo[channel].cCoolantTemperature != val)
	{
		s_astCanInfo[channel].cCoolantTemperature = val;
	}
}

INT8U can_get_coolanttemperature(char channel)
{
	return s_astCanInfo[channel].cCoolantTemperature;
}


void can_set_DPF(char channel,INT16U val)
{
	if (s_astCanInfo[channel].usDPF != val)
	{
		s_astCanInfo[channel].usDPF = val;
	}
}

INT16U can_get_DPF(char channel)
{
	return s_astCanInfo[channel].usDPF;
}

void can_set_SCROutletTemperature(char channel,INT16U val)
{
	if (s_astCanInfo[channel].sSCROutletTemperature != val)
	{
		s_astCanInfo[channel].sSCROutletTemperature = val;
	}
}

INT16U can_get_SCROutletTemperature(char channel)
{
	return s_astCanInfo[channel].sSCROutletTemperature;
}


void can_set_SCRInletTemperature(char channel,INT16U val)
{
	if (s_astCanInfo[channel].sSCRInletTemperature != val)
	{
		s_astCanInfo[channel].sSCRInletTemperature = val;
	}
}

INT16U can_get_SCRInletTemperature(char channel)
{
	return s_astCanInfo[channel].sSCRInletTemperature;
}


void can_set_intakevolume(char channel,INT16U val)
{
	if (s_astCanInfo[channel].usIntakeVolume != val)
	{
		s_astCanInfo[channel].usIntakeVolume = val;
	}
}

INT16U can_get_intakevolume(char channel)
{
	return s_astCanInfo[channel].usIntakeVolume;
}


void can_set_remain(char channel,INT8U val)
{
	if (s_astCanInfo[channel].ucRemain != val)
	{
		s_astCanInfo[channel].ucRemain = val;
	}
}

INT8U can_get_remain(char channel)
{
	return s_astCanInfo[channel].ucRemain;
}


void can_set_lowerNOx(char channel,INT16U val)
{
	if (s_astCanInfo[channel].sLowerNOx != val)
	{
		s_astCanInfo[channel].sLowerNOx = val;
	}
}

INT16U can_get_lowerNOx(char channel)
{
	return s_astCanInfo[channel].sLowerNOx;
}

void can_set_upperNOx(char channel,INT16U val)
{
	if (s_astCanInfo[channel].sUpperNOx != val)
	{
		s_astCanInfo[channel].sUpperNOx = val;
	}
}

INT16U can_get_upperNOx(char channel)
{
	return s_astCanInfo[channel].sUpperNOx;
}


void can_set_dtc_cnt(char channel,INT8U val)
{
	if (s_astCanInfo[channel].DTC_CNT != val)
	{
		s_astCanInfo[channel].DTC_CNT = val;
	}
}

INT8U can_get_dtc_cnt(char channel)
{
	return s_astCanInfo[channel].DTC_CNT;
}

void can_set_mil_sta(char channel,INT8U val)
{
	if (s_astCanInfo[channel].MIL_STA != val)
	{
		s_astCanInfo[channel].MIL_STA = val;
	}
}


INT8U can_get_mil_sta(char channel)
{
	return s_astCanInfo[channel].MIL_STA;
}


void can_set_dst_sta(char channel,INT16U val)
{
	if (s_astCanInfo[channel].DST_STA != val)
	{
		s_astCanInfo[channel].DST_STA = val;
	}
}

INT16U can_get_dst_sta(char channel)
{
	return s_astCanInfo[channel].DST_STA;
}

void can_set_drt_sta(char channel,INT16U val)
{
	if (s_astCanInfo[channel].DRT_STA != val)
	{
		s_astCanInfo[channel].DRT_STA = val;
	}
}

INT16U can_get_drt_sta(char channel)
{
	return s_astCanInfo[channel].DRT_STA;
}


void can_set_obd_pro(char channel,INT8U val)
{
	if (s_astCanInfo[channel].OBD_PRO != val)
	{
		s_astCanInfo[channel].OBD_PRO = val;
	}
}

INT8U can_get_obd_pro(char channel)
{
	return s_astCanInfo[channel].OBD_PRO;
}


void can_set_calid(char channel,INT8U idx,INT8U dat)
{
	if (idx >= CALID_LEN) return ;
	s_astCanInfo[channel].CALID[idx] = dat;
}

INT8U can_get_calid(char channel,void *pOut,int outbuflen)
{
	if ((pOut == 0) || (outbuflen < CALID_LEN))
	{
		return 0;
	}
	memset(pOut,0,outbuflen);
	memcpy(pOut,s_astCanInfo[channel].CALID,CALID_LEN);
	return CALID_LEN;
}


void can_set_cvn_by_byte(char channel,unsigned char idx,unsigned char dat)
{
	if ((idx >= CVN_LEN) || (channel >= MAX_CAN_CNT))
	{
		return ;
	}

	s_astCanInfo[channel].CVN[idx] = dat;
}

void can_set_cvn(char channel,void *pdat,int datlen)
{
	if ((pdat == 0) || (datlen == 0)) return ;

	//字母或数字组成 不足18位的 用字符'0'补足18位
	memset(s_astCanInfo[channel].CVN,'0',sizeof(s_astCanInfo[channel].CVN));
	memcpy(s_astCanInfo[channel].CVN,pdat,(datlen >= CVN_LEN) ? CVN_LEN : datlen);
}

INT8U can_get_cvn(char channel,void *pOut,int outbuflen)
{
	if ((pOut == 0) || (outbuflen < CVN_LEN))
	{
		return 0;
	}

	memset(pOut,0,outbuflen);
	memcpy(pOut,s_astCanInfo[channel].CVN,CVN_LEN);
	return CVN_LEN;
}


void can_set_vin(char channel,INT8U idx,INT8U dat)
{
	if (idx >= VIN_LEN) return ;
	s_astCanInfo[channel].VIN[idx] = dat;
}

INT8U can_get_vin(char channel,void *pOut,int outbuflen)
{
	if ((pOut == 0) || (outbuflen < VIN_LEN))
	{
		return 0;
	}
	memset(pOut,0,outbuflen);
	memcpy(pOut,s_astCanInfo[channel].VIN,VIN_LEN);
	return VIN_LEN;
}

INT8U *can_get_vin_ptr(char channel)
{
	return s_astCanInfo[channel].VIN;
}


void save_set_vin(INT8S* dat)
{
	if (strlen(dat) == VIN_LEN)
	{
		memset(stSaveParam.df_jd_vin,0,VIN_LEN);
		strncpy(stSaveParam.df_jd_vin,dat,VIN_LEN);
	}
}

INT8U save_get_vin(void *pOut,int outbuflen)
{
	if ((pOut == 0) || (outbuflen < VIN_LEN))
	{
		return 0;
	}
	memset(pOut,0,outbuflen);
	memcpy(pOut,stSaveParam.df_jd_vin,VIN_LEN);
	return VIN_LEN;
}

INT8S *save_get_vin_ptr(void)
{
	return stSaveParam.df_jd_vin;
}


void can_set_iupr(char channel,INT8U idx,INT8U dat)
{
	if (idx >= IUPR_LEN) return ;
	s_astCanInfo[channel].IUPR[idx] = dat;
}

INT8U can_get_iupr(char channel,void *pOut,int outbuflen)
{
	if ((pOut == 0) || (outbuflen < IUPR_LEN))
	{
		return 0;
	}
	memset(pOut,0,outbuflen);
	memcpy(pOut,s_astCanInfo[channel].IUPR,IUPR_LEN);
	return IUPR_LEN;
}

void can_set_dtc_buf(char channel,INT8U idx,INT8U dat)
{
	if (idx >= DTC_BUF_LEN) return ;
	s_astCanInfo[channel].DTC_BUF[idx] = dat;
}

INT8U can_get_dtc_buf(char channel,INT8U idx)
{
	if (idx >= DTC_BUF_LEN) return 0;
	return s_astCanInfo[channel].DTC_BUF[idx];
}


INT8U *can_get_dtc_buf_ptr(char channel)
{
	if (channel >= MAX_CAN_CNT) return (INT8U *)(s_astCanInfo[0].DTC_BUF);
	return (INT8U *)(s_astCanInfo[channel].DTC_BUF);
}



void can_set_niuju_mode(char channel,INT8U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].niuju_mode = val;
}


INT8U can_get_niuju_mode(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].niuju_mode;
}

void can_set_niaosuxiangwendu(char channel,INT8U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].niaosuxiangwendu = val;
}

INT8U can_get_niaosuxiangwendu(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].niaosuxiangwendu;
}

void can_set_youmentaban(char channel,INT8U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].youmentaban = val;
}

INT8U can_get_youmentaban(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].youmentaban;
}

void can_set_zongniaosuxiaohaoliang(char channel,INT32U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].zongniaosuxiaohaoliang = val;
}

INT32U can_get_zongniaosuxiaohaoliang(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].zongniaosuxiaohaoliang;
}

void can_set_engine_total_run_time(char channel,INT32U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].total_run_time = val;
}

INT32U can_get_engine_total_run_time(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].total_run_time;
}


void can_set_shijiniaosupensheliang(char channel,INT32U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].shijiniaosupensheliang = val;
}

INT32U can_get_shijiniaosupensheliang(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].shijiniaosupensheliang;
}

void can_set_leijiyouhao(char channel,INT32U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].leijiyouhao = val;
}

INT32U can_get_leijiyouhao(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].leijiyouhao;
}



void can_set_dpfwendu(char channel,INT16U val)
{
	if (channel >= MAX_CAN_CNT)
	{
		return ;
	}

	s_astCanInfo[channel].dfpwendu = val;
}

INT16U can_get_dpfwendu(char channel)
{
	if (channel >= MAX_CAN_CNT)
	{
		return 0;
	}

	return s_astCanInfo[channel].dfpwendu;
}






void OBD_Display(void)
{

}

unsigned int OBD_Hex_To_DTC(unsigned char tempa,unsigned char tempb)
{
	unsigned int buf;
	buf  = (unsigned int)((tempa>>4)&0x0F)<<24;
	buf |= (unsigned int)((tempa>>0)&0x0F)<<16;
	buf |= (unsigned int)((tempb>>4)&0x0F)<<8;
	buf |= (unsigned int)((tempb>>0)&0x0F)<<0;
	return buf;
}


unsigned char s_moni_can_data_flag = 0;


unsigned char get_moni_can_data_flag(void)
{
	return s_moni_can_data_flag;
}

void set_moni_can_data_flag(unsigned char val)
{
	s_moni_can_data_flag = val;
}


#include "time.h"
void moni_can_data_proc(void)
{
	char channel = 0;
	unsigned int val_rand = 0;
	double disp_val = 0;
	static double s_remain = 87;
	static double s_remain_oil = 50;	//邮箱50L油
	double total_oil = 60;		//邮箱总共能装60L
	double take_oil = 0;		//消耗油量
	static double total_take_oil = 0;
	unsigned int i = 0;
	unsigned char temp[32] = {0};
	unsigned char *ptemp = 0;
	unsigned int cnt = 0;

	if (get_moni_can_data_flag() == 0)
	{
		return ;
	}

	//模拟有CAN数据到达 清除无CAN数据计时
	set_can_no_data_time(0);

	srand(GetRunTime());
	val_rand = rand();

	//DBG_Print(LEVEL0,"rand:%08X\r\n",val_rand);

	//1.车速
	can_set_speed(channel,0);

	//2.大气压力
	disp_val = 100 + ((val_rand % 3) / 3.0);	//100 - 103
	can_set_kpa(channel,disp_val * 2);

	//3.净输出扭矩百分比
	disp_val = val_rand % 60;
	can_set_actualtorquepercent(channel,125 + disp_val);

	//4.摩擦扭矩百分比
	disp_val = val_rand % 10;
	can_set_frictiontorquepercent(channel,125 + disp_val);

	//5.发动机转速
	can_set_rpm(channel,600 * 8);

	//6.发动机燃料流量
	disp_val = (val_rand % 90) / 100;
	take_oil = disp_val / 3600;
	can_set_fuelflow(channel,disp_val * 20);

	//7.上游NOX
	disp_val = (val_rand % 60) - 30;	//-30 30
	can_set_upperNOx(channel,(disp_val + 200) * 20);

	//8.下游NOX
	disp_val = (val_rand % 60) - 30;	//-30 30
	can_set_lowerNOx(channel,(disp_val + 200) * 20);

	//9.尿素箱液位
	disp_val = s_remain - (GetRunTime() / 600);
	can_set_remain(channel,disp_val * 5 / 2);

	//10.进气量
	disp_val = 1000 + (val_rand % 501);
	can_set_intakevolume(channel, disp_val * 20);

	//11.入口温度
	disp_val = 320 + (val_rand % 81);		//320 ~ 400
	can_set_SCRInletTemperature(channel,(disp_val + 273) / 0.03125);

	//11.出口温度
	disp_val += (val_rand % 101);		//
	can_set_SCROutletTemperature(channel,(disp_val + 273) / 0.03125);

	//12.DPF压差
	disp_val = (4 + (val_rand % 230)) / 10;		//0.4-23.3Kpa
	can_set_DPF(channel,disp_val * 10);

	//13.水温
	disp_val = 60 + (val_rand % 40);
	can_set_coolanttemperature(channel,disp_val + 40);

	//14.邮箱液位
	s_remain_oil -= take_oil;
	disp_val = s_remain_oil * 100 / total_oil;
	can_set_oilboxlevel(channel,disp_val / 0.4);

	//15.里程
	can_set_milage(channel,387230);


	//OBD诊断信息
	can_set_obd_pro(channel,2);
	can_set_drt_sta(channel,0x07);
	can_set_dst_sta(channel,0x07);

	cnt = 0;
	ptemp = (unsigned char *)APP_VERSION;
	for (i = 0; i < strlen(APP_VERSION); i++)
	{
		if ((ptemp[i] >= '0' && ptemp[i] <= '9')
		|| (ptemp[i] >= 'a' && ptemp[i] <= 'z')
		|| (ptemp[i] >= 'A' && ptemp[i] <= 'Z')
		)
		{
			temp[cnt++] = ptemp[i];
			if (cnt >= 18)
			{
				break;
			}
		}
	}
	for (i = 0; i < 18; i++)
	{
		can_set_calid(channel,i,temp[i]);
	}
	can_set_cvn(channel,"imei",4);
	

	//故障码	
	can_set_dtc_cnt(channel,1);
	can_set_dtc_buf(0,0,0x91);
	can_set_dtc_buf(0,1,0x76);
	can_set_dtc_buf(0,2,0x32);
	can_set_dtc_buf(0,3,0x24);


	//扭矩模式
	can_set_niuju_mode(channel,0x01);

	//dpf排温
	disp_val = 100 + (val_rand % 601);	//100-600
	can_set_dpfwendu(channel,(disp_val + 273)/0.03125);

	//油门踏板
	can_set_youmentaban(channel,0);

	//累计油耗
	total_take_oil += take_oil;
	can_set_leijiyouhao(channel,total_take_oil * 2);

	//尿素箱温度
	disp_val = 300 + (val_rand % 100);
	can_set_niaosuxiangwendu(channel,disp_val + 40);

	//发动机总运行时间


	can_set_zongniaosuxiaohaoliang(channel,GetRunTime() / 100);
}



