#include "dst.h"

#define DST_ZDY_INFO 				0x0A		//地上铁自定义
#define DST_LL_INFO 				0x0B		//地上铁冷链
#define DST_SOS_INFO 				0x0C		//地上铁故障报警


#define DST_PLAT_LIMIT_SPEED_CMD			0x84
#define DST_PLAT_VER_PRO_CMD				0x85
#define DST_PLAT_BIND_CMD					0x86
#define DST_PLAT_OPEN_DOOR_CMD				0x87
#define DST_PLAT_BY_TIP_CMD					0x88		//保养提醒
#define DST_PLAT_JF_TIP_CMD					0x89		//缴费提醒
#define DST_PLAT_NS_TIP_CMD					0x8A		//年审提醒



DST_TBOX_STATUS s_dst_tbox_work_status = DST_TBOX_STATUS_WORK;		//1工作 2休眠 3低电保护 4主电源断开

char *s_dst_tbox_work_status_tip[] = 
{
	"未知",
	"工作",
	"休眠",
	"低电保护",
	"主电源断开",	
};

double s_charge_dl = 0;


//计算充电电量 100毫秒执行一次
void dst_stat_charge_dl(void)
{
	//充电状态下进行 充电电量累加
	if ((gb32960_can_get_charge_status() == 1) && (gb32960_can_get_total_dianliu() < 10000))
	{
		unsigned int a = 0;

		a = 10000 - gb32960_can_get_total_dianliu();
		s_charge_dl += (gb32960_can_get_total_dianya()  * a);
		//s_charge_dl /= 10;
	}
}

//总的充电量 单位0.01kwh
unsigned int _get_dst_charge_dl(void)
{
	return (s_charge_dl / 36000000);
}

void test_calc_charge_dl(void)
{
	unsigned int temp = 0;
	unsigned int i = 0;

	s_charge_dl = 0;
	gb32960_can_set_charge_status(1);

	gb32960_can_set_total_dianya(5650);
	gb32960_can_set_total_dianliu(10000 - 750);

	for (i = 0; i < (60 * 19 * 10); i++)
	{
		dst_stat_charge_dl();
	}

	temp = _get_dst_charge_dl();
	DBG_Print(LEVEL0,"charge dl:%u(0.01kwh)\r\n",temp);
}

void clr_charge_dl(void)
{
	s_charge_dl = 0;
}

void set_dst_tbox_work_status(DST_TBOX_STATUS val)
{
	s_dst_tbox_work_status = val;
}

DST_TBOX_STATUS get_dst_tbox_work_status(void)
{
	return s_dst_tbox_work_status;
}

char *get_dst_tbox_work_status_tip(void)
{
	if (s_dst_tbox_work_status >= DST_TBOX_STATUS_MAX)
	{
		return "未知";
	}
	return s_dst_tbox_work_status_tip[s_dst_tbox_work_status];

}

//上报VIN
unsigned int dst_vin_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U first_get_vin_time[6])
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;
	unsigned int temp = 0;

	if ((pOut == 0) || (outbuflen < GB32960_MIN_PACK_LEN) || (pVIN == 0))
	{
		return 0;
	}

	temp = 1 + 2 + 6 + 4 + 4 + 20 + 17;
	//长度
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	//型号
	ptr[rlen++] = 0x00;
	//整车类型
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x03;

	//首次获取VIN时间
	memcpy(ptr + rlen,first_get_vin_time,6);
	rlen += 6;

	//经纬度
	temp = (INT32U)send_msg(S_MSG_GET_LON,0,0,0);
	ptr[rlen++] = temp >> 24;
	ptr[rlen++] = temp >> 16;
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	temp = (INT32U)send_msg(S_MSG_GET_LAT,0,0,0);
	ptr[rlen++] = temp >> 24;
	ptr[rlen++] = temp >> 16;
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	//iccid
	send_msg(S_MSG_GET_ICCID,ptr + rlen,(void *)20,0);
	rlen += 20;

	//vin
	memcpy(ptr + rlen,pVIN,17);
	rlen += 17;

	return _xDataBag(pOut,outbuflen,0x0B,0xFE,pVIN,ptr,rlen);
}



//充电电量
unsigned int dst_cddl_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U start_time[6],INT8U end_time[6])
{
	unsigned int rlen = 0;
	unsigned char ptr[16] = {0};
	unsigned int temp = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}

	//长度
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x0E;

	memcpy(ptr + rlen,start_time,6);
	rlen += 6;

	memcpy(ptr + rlen,end_time,6);
	rlen += 6;

	//电量
	temp = _get_dst_charge_dl(); 
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	return _xDataBag(pOut,outbuflen,0x0C,0xFE,pVIN,ptr,rlen);
}


unsigned int _dst_zdy_data_pack(void *pOut,unsigned int outbuflen,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = pOut;
	unsigned int temp = 0;
	unsigned int dst_zdy_len_offset = 0;
	unsigned int start_len = 0;
	unsigned int end_len = 0;
	unsigned char *ptemp = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}

	//地上铁自定义数据
	ptr[rlen++] = DST_ZDY_INFO;

	dst_zdy_len_offset = rlen;

	//数据长度
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	start_len = rlen;

	//设备型号编号
	ptr[rlen++] = 3;

	//供电标识
	ptr[rlen++] = 0;

	//固件版本号 5B
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;
	ptr[rlen++] = get_software_version();

	//继电器状态
	ptr[rlen++] = 0;

	//终端绑定状态
	ptr[rlen++] = get_dst_fact_bind_status();

	//车辆控制器版本状态
	ptr[rlen++] = get_dst_fact_pro_ver_about_lock();

	//锁车状态
	if (get_dst_fact_lock_status())
	{
		ptr[rlen++] = get_dst_fact_kucunrong_limit_speed() ? 6 : (get_dst_fact_limit_speed_level() + 1);
	}
	else
	{
		ptr[rlen++] = 0;		//get_dst_fact_kucunrong_limit_speed() ? 6 : get_dst_fact_limit_speed_level();
	}

#if 1
	if (gb32960_can_get_charge_status() == 1)
	{
		temp = _get_dst_charge_dl();

		//本次充电电量
		ptr[rlen++] = temp >> 8;
		ptr[rlen++] = temp;
	}
	else
	{
		//本次充电电量
		ptr[rlen++] = 0;
		ptr[rlen++] = 0;
	}
#endif

	ptemp = get_dst_vcu_ver_ptr();
	//vcu版本
	ptr[rlen++] = ptemp[0];
	ptr[rlen++] = ptemp[1];
	ptr[rlen++] = ptemp[2];
	ptr[rlen++] = ptemp[3];
	ptr[rlen++] = ptemp[4];
	ptr[rlen++] = ptemp[5];
	ptr[rlen++] = ptemp[6];
	ptr[rlen++] = ptemp[7];

	//车辆快慢充标志
	ptr[rlen++] = 0;

	//锁车标志
	if (get_dst_want_lock_status())
	{
		ptr[rlen++] = get_dst_want_kucunrong_limit_speed() ? 6 : (get_dst_want_limit_speed_level() + 1);
	}
	else
	{
		ptr[rlen++] = get_dst_want_kucunrong_limit_speed() ? 6 : get_dst_want_limit_speed_level();
	}

	//故障数
	ptr[rlen++] = 0;

	//车型识别
	ptr[rlen++] = 0xFF;
	ptr[rlen++] = 0xFF;

	//tbox 工作状态
	ptr[rlen++] = get_dst_tbox_work_status();

	//车辆ON信号
	ptr[rlen++] = (unsigned int)send_msg(S_MSG_GET_ACC_STATUS,0,0,0);

	//车辆CHARGE信号
	temp = gb32960_can_get_charge_status();
	if (temp == 1)
	{
		ptr[rlen++] = 1;
	}
	else
	{
		ptr[rlen++] = 0;
	}

	//CAN唤醒状态
	ptr[rlen++] = (get_can_no_data_time() > 3) ? 0 : 1;

	//can总线上是否有数据
	ptr[rlen++] = (get_can_no_data_time() > 3) ? 0 : 1;

	temp = (unsigned int)send_msg(S_MSG_GET_MAIN_VOL,0,0,0);

	//车辆电瓶电压
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	temp = (unsigned int)send_msg(S_MSG_GET_BAT_VOL,0,0,0);

	//TBOX电池电压
	ptr[rlen++] = temp >> 8;
	ptr[rlen++] = temp;

	//双联路IP长度
	ptr[rlen++] = strlen("depot32960.flybees.com.cn:6658");
	memcpy(ptr + rlen,"depot32960.flybees.com.cn:6658",strlen("depot32960.flybees.com.cn:6658"));
	rlen += strlen("depot32960.flybees.com.cn:6658");

	//双联路状态
	ptr[rlen++] = get_net_link_status(4);

	//提示
	ptr[rlen++] = get_dst_keep_fit_tip_flag() ? 1 : 0;
	ptr[rlen++] = get_dst_pay_tip_flag() ? 1 : 0;
	ptr[rlen++] = get_dst_inspect_tip_flag() ? 1 : 0;

	//网络信号
	ptr[rlen++] = (unsigned int)send_msg(S_MSG_GET_SIM_CSQ,0,0,0);

	//gps工作状态 0正常 1断开 2短路
	temp = (unsigned int)send_msg(S_MSG_GET_GPS_ANT_STATUS,0,0,0);
	if (temp == E_GPS_ANT_NORMAL)
	{
		temp = 0;
	}
	else if (temp == E_GPS_ANT_OPEN)
	{
		temp = 1;
	}
	else
	{
		temp = 2;
	}
	ptr[rlen++] = temp;

	//emmc
	ptr[rlen++] = (unsigned int)send_msg(S_MSG_GET_EMMC_CHIP_STATUS,0,0,0);

	//空调反馈电流
	ptr[rlen++] = 0;
	ptr[rlen++] = 0;

	//水泵故障
	ptr[rlen++] = 0;

	//真空泵状态
	ptr[rlen++] = 0;

	//真空度
	ptr[rlen++] = 0;

	//ptc工作状态
	ptr[rlen++] = 0;

	//ptc继电器实际状态
	ptr[rlen++] = 0;

	//动力电池加热继电器状态
	ptr[rlen++] = 0;

	//快充继电器粘连故障
	ptr[rlen++] = 0;

	//空调工作状态
	ptr[rlen++] = 0;

	//手刹状态
	ptr[rlen++] = (gb32960_can_get_dangwei() >> 4) & 0x01;

	end_len = rlen;

	ptr[dst_zdy_len_offset] = (end_len - start_len) >> 8;
	ptr[dst_zdy_len_offset + 1] = (end_len - start_len);

	return rlen;
}

unsigned int dst_zdy_data_pack(void *pOut,unsigned int outbuflen,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	
	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	//采集时间
	memcpy(ptr,time,6);
	rlen += 6;

#if 1
	//位置信息
	ptr[rlen++] = CAR_POS_INFO;
	ptr[rlen++] = pos_status;
	ptr[rlen++] = lon >> 24;
	ptr[rlen++] = lon >> 16;
	ptr[rlen++] = lon >> 8;
	ptr[rlen++] = lon;
	ptr[rlen++] = lat >> 24;
	ptr[rlen++] = lat >> 16;
	ptr[rlen++] = lat >> 8;
	ptr[rlen++] = lat;
#endif

	rlen += _dst_zdy_data_pack(ptr + rlen,outbuflen - rlen,pVIN,time,pos_status,lon,lat);

	return _xDataBag(pOut,outbuflen,GB32960_REAL_DATA_CMD,0xFE,pVIN,ptr,rlen);
}



unsigned int _dst_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	unsigned int temp = 0;
	unsigned int temp1 = 0;
	unsigned int i = 0;
	unsigned int j = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	//采集时间
	memcpy(ptr,time,6);
	rlen += 6;
#if 1
	//整车数据
	ptr[rlen++] = ZC_DATA_INFO;

	ptr[rlen++] = gb32960_can_get_car_status();
	ptr[rlen++] = gb32960_can_get_charge_status();
	ptr[rlen++] = gb32960_can_get_run_mode();
	ptr[rlen++] = gb32960_can_get_speed() >> 8;
	ptr[rlen++] = gb32960_can_get_speed();

	ptr[rlen++] = gb32960_can_get_milage() >> 24;
	ptr[rlen++] = gb32960_can_get_milage() >> 16;
	ptr[rlen++] = gb32960_can_get_milage() >> 8;
	ptr[rlen++] = gb32960_can_get_milage();

	ptr[rlen++] = gb32960_can_get_total_dianya() >> 8;
	ptr[rlen++] = gb32960_can_get_total_dianya();

	ptr[rlen++] = gb32960_can_get_total_dianliu() >> 8;
	ptr[rlen++] = gb32960_can_get_total_dianliu();

	ptr[rlen++] = gb32960_can_get_soc();
	ptr[rlen++] = gb32960_can_get_dc_dc_status();

	ptr[rlen++] = gb32960_can_get_dangwei();


	ptr[rlen++] = gb32960_can_get_jueyuandianzu() >> 8;
	ptr[rlen++] = gb32960_can_get_jueyuandianzu();

	ptr[rlen++] = gb32960_can_get_jiasutaban_val();
	ptr[rlen++] = gb32960_can_get_zhidongtaban_status();
#endif

#if 1
	//驱动电机数据
	ptr[rlen++] = QDDJ_INFO;

	temp = gb32960_can_get_qddj_cnt();
	if (temp > MAX_DJ_CNT)
	{
		temp = MAX_DJ_CNT;
	}

	ptr[rlen++] = temp;		//驱动电机个数

	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = gb32960_can_get_qddj_xuhao(i);				//电机序号
		ptr[rlen++] = gb32960_can_get_qddj_status(i);					//电机状态
		ptr[rlen++] = gb32960_can_get_qddj_controller_wendu(i);		//控制器温度

		ptr[rlen++] = gb32960_can_get_qddj_rpm(i) >> 8;				//转速
		ptr[rlen++] = gb32960_can_get_qddj_rpm(i);


		ptr[rlen++] = gb32960_can_get_qddj_zhuanju(i) >> 8;			//转矩
		ptr[rlen++] = gb32960_can_get_qddj_zhuanju(i);

		ptr[rlen++] = gb32960_can_get_qddj_wendu(i);			//电机温度

		ptr[rlen++] = gb32960_can_get_qddj_input_dianya(i) >> 8;			//输入电压
		ptr[rlen++] = gb32960_can_get_qddj_input_dianya(i);

		ptr[rlen++] = gb32960_can_get_qddj_controller_dianliu(i) >> 8;//控制器电流
		ptr[rlen++] = gb32960_can_get_qddj_controller_dianliu(i);
	}
#endif

#if 1
	if (gb32960_can_get_run_mode() != 0x01)	//非纯电模式下需要燃料电池
	{
		//燃料电池
		ptr[rlen++] = RLDC_INFO;
		ptr[rlen++] = gb32960_can_get_rldc_dianya() >> 8;
		ptr[rlen++] = gb32960_can_get_rldc_dianya();
		
		ptr[rlen++] = gb32960_can_get_rldc_dianliu() >> 8;
		ptr[rlen++] = gb32960_can_get_rldc_dianliu();

		ptr[rlen++] = gb32960_can_get_rldc_xiaohaolv() >> 8;
		ptr[rlen++] = gb32960_can_get_rldc_xiaohaolv();

		temp = gb32960_can_get_rldc_tanzhen_cnt();
		if (temp > MAX_TAN_ZHEN_CNT)
		{
			temp = MAX_TAN_ZHEN_CNT;
		}
		ptr[rlen++] = temp >> 8;		//探针总数
		ptr[rlen++] = temp;

		for (i = 0; i < temp; i++)
		{
			ptr[rlen++] = gb32960_can_get_rldc_tanzhen_wendu(i);
		}
		
		ptr[rlen++] = gb32960_can_get_max_wendu_in_qi_system() >> 8;		//氢系统中最高温度
		ptr[rlen++] = gb32960_can_get_max_wendu_in_qi_system();
		ptr[rlen++] = gb32960_can_get_max_wendu_tanzhen_id_in_qi_system();
		
		ptr[rlen++] = gb32960_can_get_max_qingqi_nongdu() >> 8; 	//最高氢气浓度
		ptr[rlen++] = gb32960_can_get_max_qingqi_nongdu();
		ptr[rlen++] = gb32960_can_get_max_qingqi_sensor_id();
		
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali() >> 8;
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali();
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali_sensor_id();
		
		ptr[rlen++] = gb32960_can_get_gaoya_dc_dc_status();
	}
#endif

#if 1
	//位置信息
	ptr[rlen++] = CAR_POS_INFO;
	ptr[rlen++] = pos_status;
	ptr[rlen++] = lon >> 24;
	ptr[rlen++] = lon >> 16;
	ptr[rlen++] = lon >> 8;
	ptr[rlen++] = lon;
	ptr[rlen++] = lat >> 24;
	ptr[rlen++] = lat >> 16;
	ptr[rlen++] = lat >> 8;
	ptr[rlen++] = lat;
#endif
#if 1
	//极值信息
	ptr[rlen++] = JIZHI_INFO;
	ptr[rlen++] = gb32960_can_get_max_dianya_sub_system_id();
	ptr[rlen++] = gb32960_can_get_max_dianya_danti_id();
	ptr[rlen++] = gb32960_can_get_max_danti_dianya_val() >> 8;
	ptr[rlen++] = gb32960_can_get_max_danti_dianya_val();

	ptr[rlen++] = gb32960_can_get_min_dianya_sub_system_id();
	ptr[rlen++] = gb32960_can_get_min_dianya_danti_id();
	ptr[rlen++] = gb32960_can_get_min_danti_dianya_val() >> 8;
	ptr[rlen++] = gb32960_can_get_min_danti_dianya_val();

	ptr[rlen++] = gb32960_can_get_max_wendu_sub_system_id();
	ptr[rlen++] = gb32960_can_get_max_wendu_tanzhen_id();
	ptr[rlen++] = gb32960_can_get_max_wendu_val();

	ptr[rlen++] = gb32960_can_get_min_wendu_sub_system_id();
	ptr[rlen++] = gb32960_can_get_min_wendu_tanzhen_id();
	ptr[rlen++] = gb32960_can_get_min_wendu_val();
#endif

#if 1
	//报警信息
	ptr[rlen++] = SOS_INFO;
	ptr[rlen++] = gb32960_can_get_max_sos_level();
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 24;
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 16;
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 8;
	ptr[rlen++] = gb32960_can_get_sos_flag();

	temp = gb32960_can_get_charge_set_dtc_cnt();
	if (temp > MAX_CAN_CHARGE_DTC_CNT)
	{
		temp = MAX_CAN_CHARGE_DTC_CNT;
	}
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{	
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 24;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 16;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 8;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i);
	}

	temp = gb32960_can_get_qddj_dtc_cnt();
	if (temp > MAX_QDDJ_DTC_CNT)
	{
		temp = MAX_QDDJ_DTC_CNT;
	}
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 24;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 16;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 8;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i);
	}

	temp = gb32960_can_get_ecu_dtc_cnt();
	if (temp > MAX_ECU_DTC_CNT)
	{
		temp = MAX_ECU_DTC_CNT;
	}
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 24;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 16;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 8;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i);
	}

	temp = gb32960_can_get_other_dtc_cnt();
	if (temp > MAX_OTHER_DTC_CNT)
	{
		temp = MAX_OTHER_DTC_CNT;
	}
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 24;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 16;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 8;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i);
	}
#endif
#if 1
	//电压数据
	ptr[rlen++] = VOL_INFO;

	temp = gb32960_can_get_dianchi_cnt();
	if (temp > MAX_DIANCHI_CNT)
	{
		temp = MAX_DIANCHI_CNT;
	}
	ptr[rlen++] = temp;
	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = stGB32960Param.stCNXTData[i].id;

		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianya >> 8;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianya;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianliu >> 8;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianliu;

		temp1 = stGB32960Param.stCNXTData[i].benzhen_danti_cnt;

		ptr[rlen++] = temp1 >> 8;
		ptr[rlen++] = temp1;

		ptr[rlen++] = stGB32960Param.stCNXTData[i].benzhen_start_id >> 8;
		ptr[rlen++] = stGB32960Param.stCNXTData[i].benzhen_start_id;

		temp1 = stGB32960Param.stCNXTData[i].benzhen_danti_cnt;
		if (temp1 == 0xFF) temp1 = 0;
		if (temp1 > DANTI_DIANCHI_CNT_EVERY_DIANCHI)
		{
			temp1 = DANTI_DIANCHI_CNT_EVERY_DIANCHI;
		}

		ptr[rlen++] = temp1;
		for (j = 0; j < temp1; j++)
		{
			ptr[rlen++] = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j] >> 8;
			ptr[rlen++] = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j];
		}
	}
#endif

#if 1
	//温度数据
	ptr[rlen++] = TEMP_INFO;

	temp = gb32960_can_get_dianchi_cnt();
	if (temp > MAX_DIANCHI_CNT)
	{
		temp = MAX_DIANCHI_CNT;
	}

	ptr[rlen++] = temp;
	for (i = 0; i < temp; i++)
	{
		ptr[rlen++] = stGB32960Param.stCNXTData[i].id;

		temp1 = stGB32960Param.stCNXTData[i].wendu_tanzhen_cnt;
		if (temp1 == 0xFFFF) temp1 = 0;
		if (temp1 > WENDU_TANZHEN_CNT_EVERY_DIANCHI)
		{
			temp1 = WENDU_TANZHEN_CNT_EVERY_DIANCHI;
		}

		ptr[rlen++] = temp1 >> 8;		//探针总数
		ptr[rlen++] = temp1;

		for (j = 0; j < temp1; j++)
		{
			ptr[rlen++] = stGB32960Param.stCNXTData[i].wendu[j];
		}		
	}
#endif

	rlen += _dst_zdy_data_pack(ptr + rlen,1024,pVIN,time,pos_status,lon,lat);

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}




unsigned int dst_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _dst_RealDataPack(pOut,outbuflen,GB32960_REAL_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int dst_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _dst_RealDataPack(pOut,outbuflen,GB32960_BF_DATA_CMD,pVIN,time,pos_status, lon,lat);
}



void dst_gb32960_deal(INT8U linkid,INT8U cmd,INT8U ack_flag,void *pvin,INT8U jiami_way,void *content,INT16U contentlen)
{
	unsigned char *ptr = 0;
	unsigned char buf[64] = {0};
	unsigned int slen = 0;

	ptr = (unsigned char *)content;
	switch (cmd)
	{
	case DST_PLAT_LIMIT_SPEED_CMD:
	{
		//unsigned char ver_pro = 0;
		unsigned char lock = 0;
		unsigned char kcr = 0;
		unsigned char limit_level = 0;

		if (ptr[0] == 0x55)
		{
			//ver_pro = 1;
			lock = 1;
			kcr = 0;
			limit_level = 0;
		}
		else if (ptr[0] == 0x56)
		{
			//ver_pro = 1;
			lock = 1;
			kcr = 0;
			limit_level = 1;
		}
		else if (ptr[0] == 0x57)
		{
			//ver_pro = 1;
			lock = 1;
			kcr = 0;
			limit_level = 2;
		}
		else if (ptr[0] == 0x60)
		{
			//ver_pro = 1;
			lock = 1;
			kcr = 1;
			limit_level = 0;
		}
		else if (ptr[0] == 0xAA)
		{
			//ver_pro = 0;
			lock = 0;
			kcr = 0;
			limit_level = 0;
		}

		//set_dst_want_pro_ver_about_lock(ver_pro);
		set_dst_want_lock_status(lock);
		set_dst_want_limit_speed_level(limit_level);
		set_dst_want_kucunrong_limit_speed(kcr);
		set_lock_car_cmd_from_where(2);
	}
	break;

	case DST_PLAT_VER_PRO_CMD:
		if (ptr[0] == 0x55)
		{
			set_dst_want_pro_ver_about_lock(1);
		}
		else if (ptr[0] == 0xAA)
		{
			set_dst_want_pro_ver_about_lock(0);
		}
		set_lock_car_cmd_from_where(2);
	break;
	case DST_PLAT_BIND_CMD:
		if (ptr[0] == 0x55)
		{
			//set_dst_want_bind_status(1);
		}
		else
		{
			//set_dst_want_bind_status(0);
		}
		set_lock_car_cmd_from_where(2);
	break;
	case DST_PLAT_OPEN_DOOR_CMD:

	break;
	case DST_PLAT_BY_TIP_CMD:
		if (ptr[0] >= 0x55)
		{
			set_dst_keep_fit_tip_flag(ptr[0] + 1 - 0x55);
		}
		else
		{
			set_dst_keep_fit_tip_flag(0);
		}
		set_lock_car_cmd_from_where(2);
	break;
	case DST_PLAT_JF_TIP_CMD:
		if (ptr[0] >= 0x55)
		{
			set_dst_pay_tip_flag(ptr[0] + 1 - 0x55);
		}
		else
		{
			set_dst_pay_tip_flag(0);
		}
		set_lock_car_cmd_from_where(2);
	break;
	case DST_PLAT_NS_TIP_CMD:
		if (ptr[0] >= 0x55)
		{
			set_dst_inspect_tip_flag(ptr[0] + 1 - 0x55);
		}
		else
		{
			set_dst_inspect_tip_flag(0);
		}
		set_lock_car_cmd_from_where(2);
	break;

	case 0x0B:
		set_dst_vin(GetVIN(),17);
	break;

	default:break;
	}

	if ((cmd >= 0x84) && (cmd <= 0x8A))
	{
		slen = _xDataBag(buf,64,cmd,0x01,GetVIN(),0,0);
		DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
		DBG_PrintBuf(LEVEL2,0,buf,slen);
		send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
	}
}



void dst_ack_plat(INT8U linkid,void *pdat,unsigned int len)
{
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned char *frame = 0;
	unsigned int framelen = 0;
	unsigned int skipped = 0;
	int st = 0;

	if ((pdat == 0) || (len < GB32960_MIN_PACK_LEN))
	{
		return ;
	}

	while (len >= GB32960_MIN_PACK_LEN)
	{
		st = gb32960_locate_frame(ptr,len,&frame,&framelen);
		if ((st == 0) || (frame == 0))
		{
			break;
		}
		skipped = (unsigned int)(frame - ptr);
		if ((skipped > len) || (framelen > (len - skipped)))
		{
			break;
		}
		if (st > 0)
		{
			unsigned char cmd = frame[GB32960_CMD_OFFSET];
			unsigned char ack_flag = frame[GB32960_CMD_ACK_FLAG_OFFSET];
			unsigned char *pvin = frame + GB32960_VIN_OFFSET;
			unsigned char jiami_way = frame[GB32960_JIAMI_WAY_OFFSET];
			unsigned short contentlen = (unsigned short)(((unsigned int)frame[GB32960_CONTENT_LEN_OFFSET] << 8) + frame[GB32960_CONTENT_LEN_OFFSET + 1]);
			unsigned char *content = frame + GB32960_CONTENT_OFFSET;
			unsigned char xor = frame[GB32960_CONTENT_OFFSET + contentlen];

			DBG_Print(LEVEL3,"-------(link%d)dst server ack-------\r\n"
							"cmd:%02X ack:%02X vin:%s jiami:%02X dataunitlen:%d xor:%02X\r\n",
							linkid,cmd,ack_flag,pvin,jiami_way,contentlen,xor);
			dst_gb32960_deal(linkid,cmd,ack_flag,pvin,jiami_way,content,contentlen);
		}
		else
		{
			DBG_Print(LEVEL1,"reject dst frame, bad BCC\r\n");
		}
		ptr = frame + framelen;
		len -= skipped + framelen;
	}
}
