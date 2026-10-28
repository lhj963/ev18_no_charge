#include "dfgf_sy.h"

DFGFDataInfo sDFGFDataInfo = {0};

unsigned int short_loop_1811D4F5 = 0;

#define _SET_BIT(val,bit_no) val |= (1 << bit_no)
#define _CLR_BIT(val,bit_no) val &= (~(1 << bit_no))

void DFGF_set_1811D4F5(uint8_t * p_data)
{                    
	if(short_loop_1811D4F5)
	{
		if(short_loop_1811D4F5 > 3)//避免溢出
		{
			short_loop_1811D4F5 = 3;
		}
		
		if(stDFGFServerCmd.stDFGFCtrlStatus.lookup_car)
		{
			_SET_BIT(p_data[1],2);
			_SET_BIT(p_data[2],4);
		}
		else
		{
			stDFGFServerCmd.stDFGFCtrlStatus.lookup_car = 0;
		}
		
		if(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFZuoyiCtrl.jiare_switch)
		{
			_SET_BIT(p_data[3],0);
		}
		else
		{
			stDFGFServerCmd.stDFGFCtrlStatus.stDFGFZuoyiCtrl.jiare_switch = 0;
		}
		
		
		if(stDFGFServerCmd.stDFGFCtrlStatus.door_window_status[2] & 0x03 == 1)
		{
			_SET_BIT(p_data[2],6);
		}
		else
		{
			stDFGFServerCmd.stDFGFCtrlStatus.door_window_status[2] = 0;
		}
		short_loop_1811D4F5--;
	}
	else
	{
		_CLR_BIT(p_data[2],6);
		_CLR_BIT(p_data[1],2);
		_CLR_BIT(p_data[2],4);
		_CLR_BIT(p_data[3],0);
		
	}
}

void DFGF_set_0x1815D4F5(void * p_data)
{
	return ;

}
void DFGFParam_Init(void)
{
	return ;

}
void DFGF_Init(void *pvin)
{
	sDFGFDataInfo.login_flow_cnt = 0;
	DFGFParam_Init();
}
/*
//驱动电机序号
void dfgf_can_set_dianji_num(void)
{
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[0].dianji_num = 1;
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[1].dianji_num = 2;

}

INT8U dfgf_can_get_dianji_num(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_num;
}
*/
//驱动电机状态
void dfgf_can_set_dianji_status(INT8U num,INT8U status)
{
	if((status == 0x04) || (status > 0x05))
	{
		if(status == 0x07)
		{
			status = 0xFF;
		}
		else
		{
			status = 0xFE;
		}
	}
	if(status == 0x05)
	{
		status = 0x04;
	}
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_status = status;
}

INT8U dfgf_can_get_dianji_status(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_status;
}
//驱动电机控制器温度
void dfgf_can_set_dianji_ctrl_temp(INT8U num,INT8U status)
{
	
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_ctrl_temp = status;

}

INT8U dfgf_can_get_dianji_ctrl_temp(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_ctrl_temp;
}
//驱动电机转速
void dfgf_can_set_dianji_rpm(INT8U num,INT16U status)
{
	if((status < 0x4E20) || (status > 0x9C3E))
	{
		status = 0xFFFE;
	}
	else
	{
		status = status * 0.5 + 10000;
	}
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_rpm = status;

}
INT16U dfgf_can_get_dianji_rpm(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_rpm;
}

//驱动电机转矩
void dfgf_can_set_dianji_tor(INT8U num,INT16U status)
{
	if((status > 0xEA60))
	{
		status = 0xFFFE;
	}
	else
	{
		status = status - 10000;
	}
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_tor = status;

}

INT16U dfgf_can_get_dianji_tor(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_tor;
}
//驱动电机温度
void dfgf_can_set_dianji_temp(INT8U num,INT8U status)
{
	
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_temp = status;

}

INT8U dfgf_can_get_dianji_temp(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_temp;
}
//电机控制器输入电压
void dfgf_can_set_dianji_dianya(INT8U num,INT16U status)
{
	
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_dianya = status;

}

INT16U dfgf_can_get_dianji_dianya(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_dianya;
}
//电机控制器直流母线电流
void dfgf_can_set_dianji_dianliu(INT8U num,INT16U status)
{
	if((status < 0x55FA) || (status > 0xA406))
	{
		status = 0xFFFE;
	}
	else
	{
		status = status - 22000;
	}
	stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_dianliu = status;

}

INT16U dfgf_can_get_dianji_dianliu(INT8U num)
{
	return stDFGFDfacInfoPower.stDFGFDfacInfoPower_dianji[num].dianji_dianliu;
}

//车辆状态
void dfgf_can_set_car_status(INT8U status)
{
	if ((status == 0x01) || (status == 0x02) || (status == 0x03) || (status == 0xFE) || (status == 0xFF))
	{
		stDFGFDfacInfoPower.car_status = status;
	}
}

INT8U dfgf_can_get_car_status(void)
{
	return stDFGFDfacInfoPower.car_status;
}

//钥匙档位
void dfgf_can_set_key_dangwei(INT8U status)
{
	
	stDFGFDfacInfoPower.key_dangwei = status;

}

INT8U dfgf_can_get_key_dangwei(void)
{
	return stDFGFDfacInfoPower.key_dangwei;
}
extern INT8U gb32960_can_get_dangwei(void);
//档位
void dfgf_can_set_dangwei(INT8U status)
{
	
	//stDFGFDfacInfoPower.dangwei = status;
	stDFGFDfacInfoPower.dangwei = gb32960_can_get_dangwei();

}

INT8U dfgf_can_get_dangwei(void)
{
	return stDFGFDfacInfoPower.dangwei;
}

//充电状态
void dfgf_can_set_charge_in(INT8U status)
{
	stDFGFDfacInfoPower.charge_in = gb32960_can_get_charge_status();

}

INT8U dfgf_can_get_charge_in(void)
{
	return stDFGFDfacInfoPower.charge_in;
}
//车速
void dfgf_can_set_car_speed(INT16U status)
{
	
	stDFGFDfacInfoPower.car_speed = status * 10;

}

INT16U dfgf_can_get_car_speed(void)
{
	return stDFGFDfacInfoPower.car_speed;
}
//加速踏板开度
void dfgf_can_set_jiasu_taban(INT16U status)
{
	
	stDFGFDfacInfoPower.jiasu_taban = status * 0.1;

}

INT8U dfgf_can_get_jiasu_taban(void)
{
	return stDFGFDfacInfoPower.jiasu_taban;
}
//制动踏板开度
void dfgf_can_set_zhidong_taban(INT8U status)
{
	if(status == 0x01)
	{
		stDFGFDfacInfoPower.zhidong_taban = 100;
	}
	else
	{
		stDFGFDfacInfoPower.zhidong_taban = 0;
	}
}

INT8U dfgf_can_get_zhidong_taban(void)
{
	return stDFGFDfacInfoPower.zhidong_taban;
}

//手刹
void dfgf_can_set_hand_brk(INT8U status)
{
	
	stDFGFDfacInfoPower.hand_brk = status;

}

INT8U dfgf_can_get_hand_brk(void)
{
	return stDFGFDfacInfoPower.hand_brk;
}

//总运行时长
INT32U dfgf_can_get_run_time(void)
{
	return GetRunTime();
}

//累计里程
void dfgf_can_set_licheng(INT32U status)
{
	
	stDFGFDfacInfoPower.licheng = status/20;

}

INT32U dfgf_can_get_licheng(void)
{
	return stDFGFDfacInfoPower.licheng;
}

//电池容量
void dfgf_can_set_dianchi_yulaing(INT16U status)
{
	
	stDFGFDfacInfoPower.dianchi_yulaing = status;

}

INT16U dfgf_can_get_dianchi_yulaing(void)
{
	return stDFGFDfacInfoPower.dianchi_yulaing;
}

//SOC
void dfgf_can_set_soc_value(INT8U status)
{
	
	stDFGFDfacInfoPower.soc_value = status;

}

INT8U dfgf_can_get_soc_value(void)
{
	return stDFGFDfacInfoPower.soc_value;
}

//DCDC工作状态
void dfgf_can_set_dcdc_status(INT8U status)
{
	if(status == 0x01)
	{
		stDFGFDfacInfoPower.dcdc_status = 0;
	}
	else
	{
		stDFGFDfacInfoPower.dcdc_status = 1;
	}

}

INT8U dfgf_can_get_dcdc_status(void)
{
	return stDFGFDfacInfoPower.dcdc_status;
}
	
//电池系统总电压
void dfgf_can_set_zong_dianya(INT16U status)
{
	
	stDFGFDfacInfoPower.zong_dianya = status;

}

INT16U dfgf_can_get_zong_dianya(void)
{
	return stDFGFDfacInfoPower.zong_dianya;
}
//电池系统总电流
void dfgf_can_set_zong_dianliu(INT16U status)
{
	stDFGFDfacInfoPower.zong_dianliu = status - 22000;
}

INT16U dfgf_can_get_zong_dianliu(void)
{
	return stDFGFDfacInfoPower.zong_dianliu;
}

//充电连接状态
void dfgf_can_set_charge_connect_status(INT8U status)
{
	if(((status & 0x03) == 0x01)||(((status >> 2) & 0x03) == 0x01))
	{
		stDFGFDfacInfoPower.charge_connect_status = 1;
	}
	else
	{
		stDFGFDfacInfoPower.charge_connect_status = 0;
	}
	

}

INT8U dfgf_can_get_charge_connect_status(void)
{
	return stDFGFDfacInfoPower.charge_connect_status;
}

//充电时状态
void dfgf_can_set_charging_status(INT8U status)
{
	if(status == 0x04)
	{
		stDFGFDfacInfoPower.charging_status = 0x03;
	}
	else if((status == 0x01) || (status == 0x02))
	{
		stDFGFDfacInfoPower.charging_status = 0x01;
	}
	else if(status == 0x03)
	{
		stDFGFDfacInfoPower.charging_status = 0x0;
	}
	else
	{
		stDFGFDfacInfoPower.charging_status = 0x02;
	}
}

INT8U dfgf_can_get_charging_status(void)
{
	return stDFGFDfacInfoPower.charging_status;
}

						
//充电电压
void dfgf_can_set_charge_dianya(INT16U status)
{
	
	stDFGFDfacInfoPower.charge_dianya = status;

}

INT16U dfgf_can_get_charge_dianya(void)
{
	return stDFGFDfacInfoPower.charge_dianya;
}

//充电电流
void dfgf_can_set_charge_dianliu(INT16U status)
{
	if(dfgf_can_get_charging_status() == 0x01)
	{
		status = status - 22000;
	}
	else
	{
		status = 0;
	}
	stDFGFDfacInfoPower.charge_dianliu = status;
}

INT16U dfgf_can_get_charge_dianliu(void)
{
	return stDFGFDfacInfoPower.charge_dianliu;
}

//绝缘电阻
void dfgf_can_set_jueyuan_dianzu(INT16U status)
{
	
	stDFGFDfacInfoPower.jueyuan_dianzu = status;

}

INT16U dfgf_can_get_jueyuan_dianzu(void)
{
	return stDFGFDfacInfoPower.jueyuan_dianzu;
}

/*
//发动机状态
void dfgf_can_set_engine_status(INT8U status)
{
	
	stDFGFDfacInfoPower.engine_status = status;

}

INT8U dfgf_can_get_engine_status(void)
{
	return stDFGFDfacInfoPower.engine_status;
}
*/
//曲轴转速
void dfgf_can_set_quzhou_rpm(INT8U status)
{
	
	stDFGFDfacInfoPower.quzhou_rpm = status;

}

INT8U dfgf_can_get_quzhou_rpm(void)
{
	return stDFGFDfacInfoPower.quzhou_rpm;
}



//驱动电机个数
void dfgf_can_set_dianji_cnt(INT8U status)
{
	if(status > MAX_QUDONG_DIANJI_NUM)
	{
		status = MAX_QUDONG_DIANJI_NUM;
	}
	stDFGFDfacInfoPower.dianji_cnt = status;

}

INT8U dfgf_can_get_dianji_cnt(void)
{
	return stDFGFDfacInfoPower.dianji_cnt;
}


//续航里程
void dfgf_can_set_xuhang_mail(INT32U status)
{
	
	stDFGFDfacInfoPower.xuhang_mail = status*10/8;

}

INT32U dfgf_can_get_xuhang_mail(void)
{
	return stDFGFDfacInfoPower.xuhang_mail;
}

//电池衰减度
void dfgf_can_set_dianchi_shuaijian(INT8U status)
{
	
	stDFGFDfacInfoPower.dianchi_shuaijian = status;

}

INT8U dfgf_can_get_dianchi_shuaijian(void)
{
	return stDFGFDfacInfoPower.dianchi_shuaijian;
}

//瞬时电耗
void dfgf_can_set_shunshi_dianhao(INT16U status)
{
	
	stDFGFDfacInfoPower.shunshi_dianhao = status;

}

INT16U dfgf_can_get_shunshi_dianhao(void)
{
	return stDFGFDfacInfoPower.shunshi_dianhao;
}

//BMS续航里程

void dfgf_can_set_bms_xuhang_mail(INT32U status)
{
	
	stDFGFDfacInfoPower.bms_xuhang_mail = status;

}

INT32U dfgf_can_get_bms_xuhang_mail(void)
{
	return stDFGFDfacInfoPower.bms_xuhang_mail;
}

//累计充入电量
void dfgf_can_set_zong_charge_power(INT32U status)
{
	
	stDFGFDfacInfoPower.zong_charge_power = status;

}

INT32U dfgf_can_get_zong_charge_power(void)
{
	return stDFGFDfacInfoPower.zong_charge_power;
}


//累计放电电量

void dfgf_can_set_zong_loss_power(INT32U status)
{
	
	stDFGFDfacInfoPower.zong_loss_power = status;

}

INT32U dfgf_can_get_zong_loss_power(void)
{
	return stDFGFDfacInfoPower.zong_loss_power;
}

//单次充入电量

void dfgf_can_set_charge_power(INT16U status)
{
	
	stDFGFDfacInfoPower.charge_power = status;

}

INT16U dfgf_can_get_charge_power(void)
{
	return stDFGFDfacInfoPower.charge_power;
}

//累计回馈电量
void dfgf_can_set_zong_huikui_power(INT32U status)
{
	
	stDFGFDfacInfoPower.zong_huikui_power = status;

}

INT32U dfgf_can_get_zong_huikui_power(void)
{
	return stDFGFDfacInfoPower.zong_huikui_power;
}

//充电功率
void dfgf_can_set_charge_gonglv(INT32U status)
{
	
	stDFGFDfacInfoPower.charge_gonglv = status;

}

INT32U dfgf_can_get_charge_gonglv(void)
{
	return stDFGFDfacInfoPower.charge_gonglv;
}

//充电剩余时间
void dfgf_can_set_charge_time_shengyu(INT16U status)
{
	
	stDFGFDfacInfoPower.charge_time_shengyu = status;

}

INT16U dfgf_can_get_charge_time_shengyu(void)
{
	return stDFGFDfacInfoPower.charge_time_shengyu;
}

//充电结束优先级
void dfgf_can_set_charge_end_level(INT8U status)
{
	
	stDFGFDfacInfoPower.charge_end_level = status;

}

INT8U dfgf_can_get_charge_end_level(void)
{
	return stDFGFDfacInfoPower.charge_end_level;
}

//电池加热状态
void dfgf_can_set_dianchi_jiare_status(INT8U status)
{
	
	stDFGFDfacInfoPower.dianchi_jiare_status = status;

}

INT8U dfgf_can_get_dianchi_jiare_status(void)
{
	return stDFGFDfacInfoPower.dianchi_jiare_status;
}

//最高电池温度
void dfgf_can_set_max_dianchi_wendu(INT8U status)
{
	
	stDFGFDfacInfoPower.max_dianchi_wendu = status;

}

INT8U dfgf_can_get_max_dianchi_wendu(void)
{
	return stDFGFDfacInfoPower.max_dianchi_wendu;
}

//#######################################################
//方向盘角度信号	
void dfgf_can_set_fangxaingpan_angle(INT16U status)
{
	INT32U temp = status * 180 / 3.1374;
	temp = temp/1024;
	temp = temp - 1020;
	temp *= 10;
	stDFGFDfacInfoChassis.fangxaingpan_angle = temp;

}

INT16U dfgf_can_get_fangxaingpan_angle(void)
{
	return stDFGFDfacInfoChassis.fangxaingpan_angle;
}
//刹车磨损报警状态
void dfgf_can_set_shache_warn_status(INT8U status)
{
	
	stDFGFDfacInfoChassis.shache_warn_status = status;

}

INT8U dfgf_can_get_shache_warn_status(void)
{
	return stDFGFDfacInfoChassis.shache_warn_status;
}
//定速巡航
void dfgf_can_set_xunhang_status(INT8U status)
{
	if(status == 0x01)
	{
		stDFGFDfacInfoChassis.xunhang_status = 0x01;
	}
	else
	{
		stDFGFDfacInfoChassis.xunhang_status = 0;
	}
	

}

INT8U dfgf_can_get_xunhang_status(void)
{
	return stDFGFDfacInfoChassis.xunhang_status;
}
//巡航速度
void dfgf_can_set_xunhang_speed(INT8U status)
{
	
	stDFGFDfacInfoChassis.xunhang_speed = status;

}

INT8U dfgf_can_get_xunhang_speed(void)
{
	return stDFGFDfacInfoChassis.xunhang_speed;
}
//
void dfgf_can_set_aeb_status(INT8U status)
{
	
	stDFGFDfacInfoChassis.aeb_status = status;

}

INT8U dfgf_can_get_aeb_status(void)
{
	return stDFGFDfacInfoChassis.aeb_status;
}
//#################################################

//门锁开关状态

void dfgf_can_set_door_lock_status_wei(INT8U wei,INT8U value)
{
	if(value == 0x01)
	{
		setbit(stDFGFDfacInfoCarBody.door_lock_status,wei);
	}
	else
	{
		clrbit(stDFGFDfacInfoCarBody.door_lock_status,wei);
	}
}

void dfgf_can_set_door_lock_status(INT8U status)
{
	
	stDFGFDfacInfoCarBody.door_lock_status = status;

}

INT8U dfgf_can_get_door_lock_status(void)
{
	return stDFGFDfacInfoCarBody.door_lock_status;
}
//安全带状态
void dfgf_can_set_safe_bind_status_wei(INT8U wei,INT8U value)
{
	if(value == 0x01)
	{
		setbit(stDFGFDfacInfoCarBody.safe_bind_status,wei);
	}
	else
	{
		clrbit(stDFGFDfacInfoCarBody.safe_bind_status,wei);
	}
}

void dfgf_can_set_safe_bind_status(INT8U status)
{
	
	stDFGFDfacInfoCarBody.safe_bind_status = status;

}

INT8U dfgf_can_get_safe_bind_status(void)
{
	return stDFGFDfacInfoCarBody.safe_bind_status;
}

//车窗状态
void dfgf_can_set_windows_status_wei(INT8U wei,INT8U value)
{
	if((value == 0x04) || (value == 0x05))
	{
		setbit(stDFGFDfacInfoCarBody.windows_status,wei);
	}
	else
	{
		clrbit(stDFGFDfacInfoCarBody.windows_status,wei);
	}
}

void dfgf_can_set_windows_status(INT8U status)
{
	
	stDFGFDfacInfoCarBody.windows_status = status;

}

INT8U dfgf_can_get_windows_status(void)
{
	return stDFGFDfacInfoCarBody.windows_status;
}

//车灯状态
void dfgf_can_set_light_status_wei(INT16U wei,INT8U value)
{
	if(value == 0x01)
	{
		setbit(stDFGFDfacInfoCarBody.light_status,wei);
	}
	else
	{
		clrbit(stDFGFDfacInfoCarBody.light_status,wei);
	}
}

void dfgf_can_set_light_status(INT16U status)
{
	
	stDFGFDfacInfoCarBody.light_status = status;

}

 INT16U dfgf_can_get_light_status(void)
{
	return stDFGFDfacInfoCarBody.light_status;
}

//空调状态
void dfgf_can_set_air_cond_status_wei(INT8U wei,INT8U value)
{
	if(value == 0x01)
	{
		setbit(stDFGFDfacInfoCarBody.air_cond_status,wei);
	}
	else
	{
		clrbit(stDFGFDfacInfoCarBody.air_cond_status,wei);
	}
}

void dfgf_can_set_air_cond_status(INT16U status)
{
	
	stDFGFDfacInfoCarBody.air_cond_status = status;

}

INT16U dfgf_can_get_air_cond_status(void)
{
	return stDFGFDfacInfoCarBody.air_cond_status;
}

//吹风模式
void dfgf_can_set_feng_mode(INT8U status)
{
	
	stDFGFDfacInfoCarBody.feng_mode = status;

}

INT8U dfgf_can_get_feng_mode(void)
{
	return stDFGFDfacInfoCarBody.feng_mode;
}

//风速
void dfgf_can_set_feng_speed(INT8U status)
{
	
	stDFGFDfacInfoCarBody.feng_speed = status;

}

INT8U dfgf_can_get_feng_speed(void)
{
	return stDFGFDfacInfoCarBody.feng_speed;
}

//空调温度
void dfgf_can_set_air_cond_temper(INT8U status)
{
	
	stDFGFDfacInfoCarBody.air_cond_temper = status + 40;

}

INT8U dfgf_can_get_air_cond_temper(void)
{
	return stDFGFDfacInfoCarBody.air_cond_temper + 40;
}

//空调温度显示
void dfgf_can_set_air_cond_temper_disp(INT8U status)
{
	
	stDFGFDfacInfoCarBody.air_cond_temper_disp = status;

}

INT8U dfgf_can_get_air_cond_temper_disp(void)
{
	return stDFGFDfacInfoCarBody.air_cond_temper_disp;
}
//环境温度
void dfgf_can_set_env_temper(INT8U status)
{
	
	stDFGFDfacInfoCarBody.env_temper = status;

}

INT8U dfgf_can_get_env_temper(void)
{
	return stDFGFDfacInfoCarBody.env_temper;
}
//驾驶室内温度
void dfgf_can_set_driver_env_temper(INT8U status)
{
	
	stDFGFDfacInfoCarBody.driver_env_temper = status + 40;

}

INT8U dfgf_can_get_driver_env_temper(void)
{
	return stDFGFDfacInfoCarBody.driver_env_temper;
}

//风扇控制占空比
void dfgf_can_set_cond_fan_pwm(INT8U status)
{
	
	stDFGFDfacInfoCarBody.cond_fan_pwm = status + 40;

}

INT8U dfgf_can_get_cond_fan_pwm(void)
{
	return stDFGFDfacInfoCarBody.cond_fan_pwm;
}


//压缩机转速
void dfgf_can_set_cond_fan_rpm(INT16U status)
{
	
	stDFGFDfacInfoCarBody.cond_fan_rpm = status + 40;

}

INT16U dfgf_can_get_cond_fan_rpm(void)
{
	return stDFGFDfacInfoCarBody.cond_fan_rpm;
}

//方向盘加热状态
void dfgf_can_set_fangxaingpan_jiare_status(INT8U status)
{
	
	stDFGFDfacInfoCarBody.fangxaingpan_jiare_status = status;

}

INT8U dfgf_can_get_fangxaingpan_jiare_status(void)
{
	return stDFGFDfacInfoCarBody.fangxaingpan_jiare_status;
}

//风扇故障状态
void dfgf_can_set_fan_error_status(INT8U status)
{
	
	stDFGFDfacInfoCarBody.fan_error_status = status;

}

INT8U dfgf_can_get_fan_error_status(void)
{
	return stDFGFDfacInfoCarBody.fan_error_status;
}
//####################################################
//#######################################################
//UDS物理地址
/*
void dfgf_can_set_uds_addr(INT8U i,INT16U status)
{
	
	stDFGFWarnInfo.uds_addr = status;

}

INT16U dfgf_can_get_uds_addr(INT8U i)
{
	return stDFGFWarnInfo.uds_addr;
}


//报文协议
void dfgf_can_set_protocal(INT8U i,INT8U status)
{
	
	stDFGFWarnInfo.protocal = status;

}

INT8U dfgf_can_get_protocal(INT8U i)
{
	return stDFGFWarnInfo.protocal;
}


//故障等级
void dfgf_can_set_sos_level(INT8U i,INT8U status)
{
	
	stDFGFWarnInfo.sos_level = status;

}

INT8U dfgf_can_get_sos_level(INT8U i)
{
	return stDFGFWarnInfo.sos_level;
}


//故障码个数
void dfgf_can_set_sos_num(INT8U i,INT8U status)
{
	
	stDFGFWarnInfo.sos_num = status;

}

INT8U dfgf_can_get_sos_num(INT8U i)
{
	return stDFGFWarnInfo.sos_num;
}


//故障码
void dfgf_can_set_dtc_num(INT8U i,INT8U status)
{
	
	stDFGFWarnInfo.dtc_num = status;

}

INT8U dfgf_can_get_dtc_num(INT8U i)
{
	return stDFGFWarnInfo.dtc_num;
}


//快照数据长度
void dfgf_can_set_dtc_data_len(INT8U i,INT8U status)
{
	
	stDFGFWarnInfo.dtc_data_len = status;

}

INT8U dfgf_can_get_dtc_data_len(INT8U i)
{
	return stDFGFWarnInfo.dtc_data_len;
}


//快照数据
void dfgf_can_set_dtc_data(INT8U i,INT8U* status)
{
	memmove(stDFGFWarnInfo.dtc_data,status,dfgf_can_get_dtc_data_len());

}


INT8U* dfgf_can_get_dtc_data(INT8U i)
{
	return stDFGFWarnInfo.dtc_data;
}
*/
//#######################################################################
//#######################################################################

//快充1连接确认信号
void dfgf_can_set_connect_comfirm(INT8U status)
{
	
	if(((status & 0x03) == 0x01)||(((status >> 2) & 0x03) == 0x01))
	{
		stDFGFYanfaInfo.connect_comfirm = 1;
	}
	else
	{
		stDFGFYanfaInfo.connect_comfirm = 0;
	}

}

INT8U dfgf_can_get_connect_comfirm(void)
{
	return stDFGFYanfaInfo.connect_comfirm;
}
//CC连接状态
void dfgf_can_set_connect_status(INT16U status)
{
	
	stDFGFYanfaInfo.connect_status = status;

}

INT16U dfgf_can_get_connect_status(void)
{
	return stDFGFYanfaInfo.connect_status;
}
//DCDC输出电压
void dfgf_can_set_dcdc_dianya(INT16U status)
{
	
	stDFGFYanfaInfo.dcdc_dianya = status;

}

INT16U dfgf_can_get_dcdc_dianya(void)
{
	return stDFGFYanfaInfo.dcdc_dianya;
}



//DCDC输出电流
void dfgf_can_set_dcdc_dianliu(INT8U status)
{
	
	stDFGFYanfaInfo.dcdc_dianliu = status;

}

INT8U dfgf_can_get_dcdc_dianliu(void)
{
	return stDFGFYanfaInfo.dcdc_dianliu;
}



//DCDC温度
void dfgf_can_set_dcdc_temper(INT8U status)
{
	
	stDFGFYanfaInfo.dcdc_temper = status;

}

INT8U dfgf_can_get_dcdc_temper(void)
{
	return stDFGFYanfaInfo.dcdc_temper;
}



//DCDC输入功率
void dfgf_can_set_dcdc_gonglv(INT16U status)
{
	
	stDFGFYanfaInfo.dcdc_gonglv = status;

}

INT16U dfgf_can_get_dcdc_gonglv(void)
{
	return stDFGFYanfaInfo.dcdc_gonglv;
}

//压缩机母线电压
void dfgf_can_set_cond_dianya(INT8U status)
{
	INT16U temp_value = status * 4 / 3;
	stDFGFYanfaInfo.cond_dianya = temp_value;

}

INT8U dfgf_can_get_cond_dianya(void)
{
	return stDFGFYanfaInfo.cond_dianya;
}



//压缩机母线电流
void dfgf_can_set_cond_dianliu(INT8U status)
{
	
	stDFGFYanfaInfo.cond_dianliu = status;

}

INT8U dfgf_can_get_cond_dianliu(void)
{
	return stDFGFYanfaInfo.cond_dianliu;
}



//压缩机内部温度
void dfgf_can_set_cond_temper(INT8U status)
{
	
	stDFGFYanfaInfo.cond_temper = status;

}

INT8U dfgf_can_get_cond_temper(void)
{
	return stDFGFYanfaInfo.cond_temper;
}



//压缩机的实际功率
void dfgf_can_set_cond_gonglv(INT8U status)
{
	
	stDFGFYanfaInfo.cond_gonglv = status;

}

INT8U dfgf_can_get_cond_gonglv(void)
{
	return stDFGFYanfaInfo.cond_gonglv;
}

//WPTC出口温度
void dfgf_can_set_wptc_wendu(INT8U status)
{
	
	stDFGFYanfaInfo.wptc_wendu = status;

}

INT8U dfgf_can_get_wptc_wendu(void)
{
	return stDFGFYanfaInfo.wptc_wendu;
}



//WPTC电流值
void dfgf_can_set_wptc_dianliu(INT8U status)
{
	
	stDFGFYanfaInfo.wptc_dianliu = status;

}

INT8U dfgf_can_get_wptc_dianliu(void)
{
	return stDFGFYanfaInfo.wptc_dianliu;
}



//WPTC高压值
void dfgf_can_set_wptc_dianya(INT8U status)
{
	
	stDFGFYanfaInfo.wptc_dianya = status;

}

INT8U dfgf_can_get_wptc_dianya(void)
{
	return stDFGFYanfaInfo.wptc_dianya;
}



//WPTC功率值
void dfgf_can_set_wptc_gonglv(INT8U status)
{
	
	stDFGFYanfaInfo.wptc_gonglv = status;

}

INT8U dfgf_can_get_wptc_gonglv(void)
{
	return stDFGFYanfaInfo.wptc_gonglv;
}

//############################################

void dfgf_can_set_tire(INT8U weizhi,INT8U value,INT8U status)
{
	INT8U i = 0;
	
	switch(weizhi)
	{
		case 0x0:
			i=0;
		break;
		case 0x1:
			i=1;
		break;
		case 0xA:
			i=2;
		break;
		case 0xB:
			i=4;
		break;
		case 0xC:
			i=5;
		break;
		case 0xD:
			i=3;
		break;
		default:
			return;
		
	}
	stDFGFCarSOSInfo.stDFGFCarTireInfo[i].tire_weizhi = weizhi;
	stDFGFCarSOSInfo.stDFGFCarTireInfo[i].tire_value = value;
	stDFGFCarSOSInfo.stDFGFCarTireInfo[i].tire_status = status;
}


//封包
unsigned int _dfgf_DataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,INT8U ack_flag,INT16U cmdNumber,void *pVIN,void *pDataUnit,INT16U usDataUnitLen)
{
	INT16U len = 0;
	INT8U *pbuf = (INT8U *)pOutBuf;
	INT8U xor = 0;

	if (pbuf == 0)
	{
		return 0;
	}
	//判断缓冲区大小
	//起始符(2B)+协议版本(2B)+命令字(1B)+应答标志(1B)+VIN(17B)+数据压缩（1B）+加密方式(1B)+指令流水号（2B）+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
	if (usOutBufSize < (2+2+1+1+GB_LEN_VIN+1+1+2+2+usDataUnitLen+1))
	{
		return 0;
	}
	//起始字符串 2B
	pbuf[len++] = DFGFHEAD0;
	pbuf[len++] = DFGFHEAD0;
	
	//协议版本 2B
	pbuf[len++] = DFGF_VER_MAJ;
	pbuf[len++] = DFGF_VER_MIN;

	//命令单元 1B
	pbuf[len++] = cmd;

	//应答标志 1B
	pbuf[len++] = ack_flag;

	//车辆识别号
	if (pVIN)
	{
		memcpy(pbuf + len,pVIN,GB_LEN_VIN);
	}
	else
	{
		memset(pbuf + len,0,GB_LEN_VIN);
	}
	len += GB_LEN_VIN;
	
	//数据压缩	1B
	pbuf[len++] = 0x00;//sGBDataInfo.Way;

	//数据加密方式	1B
	pbuf[len++] = 0x00;//sGBDataInfo.Way;
	
	//指令流水号（2B）
	pbuf[len++] = cmdNumber >> 8;
	pbuf[len++] = cmdNumber;

	//数据单元长度	2B
	pbuf[len++] = usDataUnitLen >> 8;
	pbuf[len++] = usDataUnitLen;

	//数据单元
	if (pDataUnit && usDataUnitLen)
	{
		memmove(pbuf + len,pDataUnit,usDataUnitLen);
		len += usDataUnitLen;
	}

	//异或校验
	xor = XOR_Check(pbuf + 2,len - 2);
	pbuf[len++] = xor;
	return len;
}
//心跳
unsigned int DFGF_HeartBeatPack(void *pOutBuf,unsigned int outbuflen)
{
	return _dfgf_DataBag(pOutBuf,outbuflen,GB32960_HEART_BEAT_CMD,0,0xFE,GetVIN(),0,0);
}


unsigned int DFGF_LoginPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,
	INT8U time[6])
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int temp_login_flow_cnt = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + DFGF_CONTENT_OFFSET;

	//时间
	memcpy(ptr,time,6);
	rlen += 6;

	//登录流水号
	if (get_gb_login_flow_ptr() && (linkid < get_gb_login_flow_cnt()))
	{
		*(get_gb_login_flow_ptr() + linkid) = (*(get_gb_login_flow_ptr() + linkid)) + 1;
		temp_login_flow_cnt = *(get_gb_login_flow_ptr() + linkid);
	}
	else
	{
		temp_login_flow_cnt = 1;
	}
	
	sDFGFDataInfo.login_flow_cnt = temp_login_flow_cnt;
	ptr[rlen++] = sDFGFDataInfo.login_flow_cnt >> 8;
	ptr[rlen++] = sDFGFDataInfo.login_flow_cnt;

	return  _dfgf_DataBag(pOutBuf,outbuflen,DFGF_LOGIN_CMD,0,0xFE,GetVIN(),ptr,rlen);
	
}

unsigned int DFGF_LogoutPack(void *pOutBuf,unsigned int outbuflen,INT8U time[6])
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + DFGF_CONTENT_OFFSET;
	//时间
	memcpy(ptr,time,6);
	rlen += 6;

	//登录流水号
	ptr[rlen++] = sDFGFDataInfo.login_flow_cnt >> 8;
	ptr[rlen++] = sDFGFDataInfo.login_flow_cnt;

	return  _dfgf_DataBag(pOutBuf,outbuflen,DFGF_LOGOUT_CMD,0,0xFE,GetVIN(),ptr,rlen);
	
}

unsigned int get_dfgf_data_unit(void *pOut,unsigned int outbuflen,INT8U time[6])												
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned short sec_len = 0;
	//unsigned int qddj_dianliu = 0;
	unsigned int qddj_cnt = 0;
	unsigned int i = 0;
	unsigned short rlen_shishi = 0;
	unsigned short len_temp = 0;
	//unsigned short rlen_shishi_lingjian = 0;
	unsigned int lon_value  = (INT32U)send_msg(S_MSG_GET_LON,0,0,0);
	unsigned int lat_value  = (INT32U)send_msg(S_MSG_GET_LAT,0,0,0);	
	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}

	memset((unsigned char *)(&s_DFGFTboxStatu),0xFF,sizeof(s_DFGFTboxStatu));
	send_msg(S_MSG_GET_TBOX_STATUS,(void *)(&s_DFGFTboxStatu),(void *)(sizeof(s_DFGFTboxStatu)),0);
	//TBOX状态
	//采集时间
	if (time) memcpy(ptr,time,6);
	rlen += 6;
	ptr[rlen++] = DFGF_TBOX_STATUS;
	if(s_DFGFTboxStatu.dev_num_len > 32)
	{
		s_DFGFTboxStatu.dev_num_len = 32;
	}
	if(s_DFGFTboxStatu.imei_num_len > 32)
	{
		s_DFGFTboxStatu.imei_num_len = 32;
	}
	sec_len =0xFFFF ;
	rlen_shishi = rlen;
	
	ptr[rlen++] = sec_len>>8;
	ptr[rlen++] = sec_len;
	
	ptr[rlen++] = (s_DFGFTboxStatu.is_position);//定位状态
	ptr[rlen++] = (s_DFGFTboxStatu.stars_count);//GPS卫星数量
	
	ptr[rlen++] = lon_value >> 24;//经度
	ptr[rlen++] = lon_value >> 16;
	ptr[rlen++] = lon_value >> 8;
	ptr[rlen++] = lon_value;
	
	ptr[rlen++] = lat_value >> 24;//维度
	ptr[rlen++] = lat_value >> 16;
	ptr[rlen++] = lat_value >> 8;
	ptr[rlen++] = lat_value;
	
	ptr[rlen++] = (s_DFGFTboxStatu.gps_speed) >> 8;//GPS速度
	ptr[rlen++] = (s_DFGFTboxStatu.gps_speed);
	
	ptr[rlen++] = (s_DFGFTboxStatu.gps_angle);//方向
	
	ptr[rlen++] = (s_DFGFTboxStatu.gps_haiab) >> 8;//海拔
	ptr[rlen++] = (s_DFGFTboxStatu.gps_haiab);
	
	ptr[rlen++] = (s_DFGFTboxStatu.gps_signal);//GPS信号强度
	
	ptr[rlen++] = (s_DFGFTboxStatu.jizhan_lac) >> 8;//基站LAC
	ptr[rlen++] = (s_DFGFTboxStatu.jizhan_lac);
	
	ptr[rlen++] = (s_DFGFTboxStatu.jizhan_cid) >> 8;//基站CID
	ptr[rlen++] = (s_DFGFTboxStatu.jizhan_cid);
	
	ptr[rlen++] = (s_DFGFTboxStatu.main_dianya) >> 24;//B+电压
	ptr[rlen++] = (s_DFGFTboxStatu.main_dianya) >> 16;
	ptr[rlen++] = (s_DFGFTboxStatu.main_dianya) >> 8;
	ptr[rlen++] = (s_DFGFTboxStatu.main_dianya);
	
	ptr[rlen++] = (s_DFGFTboxStatu.signal_4g);//4G信号强度
	
	ptr[rlen++] = s_DFGFTboxStatu.dev_num_len;//设备号长度
	memcpy(ptr+rlen,s_DFGFTboxStatu.dev_num,s_DFGFTboxStatu.dev_num_len);//设备号
	rlen += s_DFGFTboxStatu.dev_num_len;
	
	ptr[rlen++] = s_DFGFTboxStatu.imei_num_len;//设备IMEI号长度
	memcpy(ptr+rlen,s_DFGFTboxStatu.imei_num,s_DFGFTboxStatu.imei_num_len);//设备IMEI
	rlen += s_DFGFTboxStatu.imei_num_len;

	ptr[rlen++] = s_DFGFTboxStatu.budian_status;//补电状态
	
	ptr[rlen++] = 0x00;//电池防盗告警
	
	ptr[rlen++] = 0x01;//OTA功能

	len_temp = rlen - rlen_shishi -2;
	ptr[rlen_shishi++] = len_temp >> 8;
	ptr[rlen_shishi++] = len_temp;
	
#if 1
	//新能源车辆业务拓展数据
	if (time) memcpy(ptr+rlen,time,6);
	DBG_Print(0,"time = %u!!!\r\n",time[0]);
	rlen += 6;
	ptr[rlen++] = DFGF_DFAC_INFO;
	sec_len = 0xFFFF;
	rlen_shishi = rlen;
	ptr[rlen++] = sec_len>>8;
	ptr[rlen++] = sec_len;
	
		//电车动力域数据
		ptr[rlen++] = 0x01;
		ptr[rlen++] = gb32960_can_get_car_status();//dfgf_can_get_car_status();//车辆状态
		ptr[rlen++] = dfgf_can_get_key_dangwei();//钥匙挡位  
		ptr[rlen++] = dfgf_can_get_dangwei();//档位
		ptr[rlen++] = dfgf_can_get_charge_in();//充电状态
		
		ptr[rlen++] = dfgf_can_get_car_speed() >> 8;//车速
		ptr[rlen++] = dfgf_can_get_car_speed();
		
		ptr[rlen++] = dfgf_can_get_jiasu_taban();//加速踏板
		ptr[rlen++] = dfgf_can_get_zhidong_taban();//制动踏板
		ptr[rlen++] = dfgf_can_get_hand_brk();//手刹
		
		ptr[rlen++] = dfgf_can_get_run_time() >> 24;//总运行时长
		ptr[rlen++] = dfgf_can_get_run_time() >> 16;
		ptr[rlen++] = dfgf_can_get_run_time() >> 8;
		ptr[rlen++] = dfgf_can_get_run_time();
		
		ptr[rlen++] = dfgf_can_get_licheng() >> 24;//累计里程
		ptr[rlen++] = dfgf_can_get_licheng() >> 16;
		ptr[rlen++] = dfgf_can_get_licheng() >> 8;
		ptr[rlen++] = dfgf_can_get_licheng();
		
		ptr[rlen++] = dfgf_can_get_dianchi_yulaing() >> 8;//电池容量
		ptr[rlen++] = dfgf_can_get_dianchi_yulaing();
		
		ptr[rlen++] = dfgf_can_get_soc_value();//SOC
		ptr[rlen++] = dfgf_can_get_dcdc_status();//DCDC
		
		ptr[rlen++] = dfgf_can_get_zong_dianya() >> 8;//电池系统总电压
		ptr[rlen++] = dfgf_can_get_zong_dianya();
		
		ptr[rlen++] = dfgf_can_get_zong_dianliu() >> 8;//电池系统总电流
		ptr[rlen++] = dfgf_can_get_zong_dianliu();
		
		ptr[rlen++] = dfgf_can_get_charge_connect_status();//充电接口状态  
		ptr[rlen++] = dfgf_can_get_charging_status();//充电时状态
		ptr[rlen++] = dfgf_can_get_charge_dianya() >>8;//充电电压
		ptr[rlen++] = dfgf_can_get_charge_dianya();
		ptr[rlen++] = dfgf_can_get_charge_dianliu() >> 8;//充电电流
		ptr[rlen++] = dfgf_can_get_charge_dianliu();
		
		ptr[rlen++] = dfgf_can_get_jueyuan_dianzu() >> 8;//绝缘电阻
		ptr[rlen++] = dfgf_can_get_jueyuan_dianzu();
		ptr[rlen++] = 0xFF;//发动机状态dfgf_can_get_engine_status()
		
		ptr[rlen++] = dfgf_can_get_quzhou_rpm() >> 8;
		ptr[rlen++] = dfgf_can_get_quzhou_rpm();
		
		qddj_cnt = dfgf_can_get_dianji_cnt();
		if (qddj_cnt > MAX_QUDONG_DIANJI_NUM)
		{
			qddj_cnt = MAX_QUDONG_DIANJI_NUM;
		}

		ptr[rlen++] = qddj_cnt;		//驱动电机个数

		for (i = 0; i < qddj_cnt; i++)
		{
			ptr[rlen++] = i+1;					//电机序号
			ptr[rlen++] = dfgf_can_get_dianji_status(i);//gb32960_can_get_qddj_status(i);					//电机状态
			ptr[rlen++] = dfgf_can_get_dianji_ctrl_temp(i);//gb32960_can_get_qddj_controller_wendu(i);			//控制器温度

			ptr[rlen++] = dfgf_can_get_dianji_rpm(i)>>8;//gb32960_can_get_qddj_rpm(i) >> 8;					//转速
			ptr[rlen++] = dfgf_can_get_dianji_rpm(i);//gb32960_can_get_qddj_rpm(i);


			ptr[rlen++] = dfgf_can_get_dianji_tor(i)>> 8;//gb32960_can_get_qddj_zhuanju(i) >> 8;				//转矩
			ptr[rlen++] = dfgf_can_get_dianji_tor(i);//gb32960_can_get_qddj_zhuanju(i);

			ptr[rlen++] = dfgf_can_get_dianji_temp(i);//gb32960_can_get_qddj_wendu(i);					//电机温度

			ptr[rlen++] = dfgf_can_get_dianji_dianya(i)>>8;//gb32960_can_get_qddj_input_dianya(i) >> 8;		//输入电压
			ptr[rlen++] = dfgf_can_get_dianji_dianya(i);
			
			ptr[rlen++] = 0xFF;//dfgf_can_get_dianji_dianliu(i) >> 8;								//控制器电流
			ptr[rlen++] = 0xFF;//dfgf_can_get_dianji_dianliu(i);
			
		}		
		
		ptr[rlen++] = dfgf_can_get_xuhang_mail() >> 24;//续航里程
		ptr[rlen++] = dfgf_can_get_xuhang_mail() >> 16;
		ptr[rlen++] = dfgf_can_get_xuhang_mail() >> 8;
		ptr[rlen++] = dfgf_can_get_xuhang_mail();
		
		ptr[rlen++] = dfgf_can_get_dianchi_shuaijian();//电池衰减度
		
		ptr[rlen++] = dfgf_can_get_shunshi_dianhao() >> 8;//瞬时电耗
		ptr[rlen++] = dfgf_can_get_shunshi_dianhao();
		
		ptr[rlen++] = dfgf_can_get_bms_xuhang_mail() >> 24;//BMS续航里程
		ptr[rlen++] = dfgf_can_get_bms_xuhang_mail() >> 16;
		ptr[rlen++] = dfgf_can_get_bms_xuhang_mail() >> 8;
		ptr[rlen++] = dfgf_can_get_bms_xuhang_mail();
		
		ptr[rlen++] = dfgf_can_get_zong_charge_power() >> 24;//累计充入电量
		ptr[rlen++] = dfgf_can_get_zong_charge_power() >> 16;
		ptr[rlen++] = dfgf_can_get_zong_charge_power() >> 8;
		ptr[rlen++] = dfgf_can_get_zong_charge_power();
		
		ptr[rlen++] = dfgf_can_get_zong_loss_power() >> 24;//累计放电电量
		ptr[rlen++] = dfgf_can_get_zong_loss_power() >> 16;
		ptr[rlen++] = dfgf_can_get_zong_loss_power() >> 8;
		ptr[rlen++] = dfgf_can_get_zong_loss_power();
		
		ptr[rlen++] = dfgf_can_get_charge_power() >> 8;//单次充入电量
		ptr[rlen++] = dfgf_can_get_charge_power();
		
		ptr[rlen++] = dfgf_can_get_zong_huikui_power() >> 24;//累计回馈电量
		ptr[rlen++] = dfgf_can_get_zong_huikui_power() >> 16;
		ptr[rlen++] = dfgf_can_get_zong_huikui_power() >> 8;
		ptr[rlen++] = dfgf_can_get_zong_huikui_power();
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_gonglv() >> 24;//充电功率
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_gonglv() >> 16;
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_gonglv() >> 8;
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_gonglv();
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_time_shengyu() >> 8;//充电剩余时间
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_time_shengyu();
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_charge_end_level();//充电结束优先级
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_dianchi_jiare_status();//电池加热状态

		ptr[rlen++] = dfgf_can_get_max_dianchi_wendu();//最高电池温度
		
		
		//电车底盘域数据
		ptr[rlen++] = 0x02;
		ptr[rlen++] = 0xFF;//货车载重dfgf_can_get_zaizhong_value()  
		ptr[rlen++] = 0xFF;
		ptr[rlen++] = dfgf_can_get_fangxaingpan_angle() >> 8;//方向盘角度信号
		ptr[rlen++] = dfgf_can_get_fangxaingpan_angle();
		
		ptr[rlen++] = dfgf_can_get_shache_warn_status();//刹车磨损报警状态
		ptr[rlen++] = dfgf_can_get_xunhang_status();//定速巡航
		ptr[rlen++] = dfgf_can_get_xunhang_speed();//巡航速度
		
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[0].tire_value;//胎压数据
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[1].tire_value;
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[2].tire_value;
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[3].tire_value;
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[4].tire_value;
		ptr[rlen++] = stDFGFCarSOSInfo.stDFGFCarTireInfo[5].tire_value;
		ptr[rlen++] = dfgf_can_get_aeb_status();//AEB状态
	
		//电车车身域数据
		ptr[rlen++] = 0x03;
		ptr[rlen++] = dfgf_can_get_door_lock_status();		//门锁开关状态
		ptr[rlen++] = dfgf_can_get_safe_bind_status();		//安全带状态
		ptr[rlen++] = dfgf_can_get_windows_status();		//车窗状态
		
		ptr[rlen++] = dfgf_can_get_light_status() >> 8;		//车灯状态
		ptr[rlen++] = dfgf_can_get_light_status();
		
		ptr[rlen++] = dfgf_can_get_air_cond_status() >> 8;	//空调状态
		ptr[rlen++] = dfgf_can_get_air_cond_status();
		ptr[rlen++] = dfgf_can_get_feng_mode();				//吹风模式
		ptr[rlen++] = dfgf_can_get_feng_speed();			//风速
		ptr[rlen++] = dfgf_can_get_air_cond_temper();		//空调温度
		ptr[rlen++] = 0xFF;//0x0;									//温度显示
		ptr[rlen++] = dfgf_can_get_env_temper();			//环境温度
		ptr[rlen++] = dfgf_can_get_driver_env_temper();		//驾驶室内温度
		
		ptr[rlen++] = dfgf_can_get_cond_fan_pwm();			//风扇控制占空比
				
		ptr[rlen++] = dfgf_can_get_cond_fan_rpm() >> 8;		//压缩机转速
		ptr[rlen++] = dfgf_can_get_cond_fan_rpm();
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_fangxaingpan_jiare_status();		//方向盘加热状态
		
		ptr[rlen++] = 0xFF;//dfgf_can_get_fan_error_status();		//风扇输入故障状态

	len_temp = rlen - rlen_shishi -2;
	ptr[rlen_shishi++] = len_temp >> 8;
	ptr[rlen_shishi++] = len_temp;
#endif	



#if 1
	// 实时数据上报-研发扩展数据0x05 一秒一次数据采集
	if (time) memcpy(ptr+rlen,time,6);
	rlen += 6;
	ptr[rlen++] = 0x05;

	rlen_shishi = rlen;
	ptr[rlen++] = 0xFF;
	ptr[rlen++] = 0xFF;


	ptr[rlen++] = dfgf_can_get_connect_comfirm();//快充1连接确认信号
	
	ptr[rlen++] = dfgf_can_get_connect_status()>>8;//CC连接状态
	ptr[rlen++] = dfgf_can_get_connect_status();
		
	ptr[rlen++] = dfgf_can_get_dcdc_dianya()>>8;//DCDC输出电压
	ptr[rlen++] = dfgf_can_get_dcdc_dianya();
		
	ptr[rlen++] = dfgf_can_get_dcdc_dianliu();//DCDC输出电流
	ptr[rlen++] = dfgf_can_get_dcdc_temper();//DCDC温度
	
	ptr[rlen++] = dfgf_can_get_dcdc_gonglv()>>8;//DCDC输入功率
	ptr[rlen++] = dfgf_can_get_dcdc_gonglv();
		
	ptr[rlen++] = dfgf_can_get_cond_dianya();//压缩机母线电压
	ptr[rlen++] = dfgf_can_get_cond_dianliu();//压缩机母线电流
	ptr[rlen++] = dfgf_can_get_cond_temper();//压缩机内部温度
	ptr[rlen++] = dfgf_can_get_cond_gonglv();//压缩机的实际功率
	
	ptr[rlen++] = dfgf_can_get_wptc_wendu();//WPTC出口温度
	ptr[rlen++] = dfgf_can_get_wptc_dianliu();//WPTC电流值
	ptr[rlen++] = dfgf_can_get_wptc_dianya();//WPTC高压值
	ptr[rlen++] = dfgf_can_get_wptc_gonglv();//WPTC功率值
	
	len_temp = rlen - rlen_shishi -2;
	ptr[rlen_shishi++] = len_temp >> 8;
	ptr[rlen_shishi++] = len_temp;
#endif
	DBG_Print(LEVEL0,"rlen = 0x%x \r\n",rlen);
	
	return rlen;
}

unsigned int _EV18_xDFGF_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6])
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < DFGF_CONTENT_OFFSET))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + DFGF_CONTENT_OFFSET;

	rlen += get_dfgf_data_unit(ptr,outbuflen - DFGF_CONTENT_OFFSET,time);
	return _dfgf_DataBag(pOut,outbuflen,cmd,0x0,0xFE,pVIN,ptr,rlen);
}

void collect_dfgf_pack(void)
{
	unsigned char *pbuf = 0;
	unsigned int rlen = 0;
	unsigned char link_status = 0x03;
	
	if (get_net_link_status(3))
	{
		link_status = 0x02;
	}
	
	pbuf = os_malloc(1536);
	if (pbuf)
	{
		
		rlen = _EV18_xDFGF_RealDataPack(pbuf,1536,link_status,
											GetVIN(),
											get_rtc_time_ptr());
		send_msg(S_MSG_SEND_TO_SERVER,(void *)3,pbuf,(void *)rlen);
		DBG_Print(LEVEL2,"(link%u)dfgf collect info(%uBytes)\r\n",3,rlen);
		os_free(pbuf);
	}

}

//起始符(2B)+协议版本(2B)+命令字(1B)+应答标志(1B)+VIN(17B)+数据压缩（1B）+加密方式(1B)+指令流水号（2B）+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
void anlyze_dfgf_sy_server_data(unsigned int linkid,void *pbuf,unsigned int len)
{
	unsigned char cmd = 0;
	unsigned char *psrc = (unsigned char *)pbuf;
	unsigned char *ptr = 0;
	unsigned char *pcontent = 0;
	unsigned int rlen = len;
	unsigned int datunitlen = 0;
	unsigned char xor = 0;
	unsigned int slen = 0;
	unsigned char buf[128] = {0};
	INT16U cmdNumber = 0;
	unsigned char i = 0;
	unsigned char rlen_temp = 0;
	unsigned char *ptr_temp = 0;

	if (len < DFGF_MIN_PACK_LEN)
	{
		DBG_Print(LEVEL0,"(link%u)dfgf cmd len too short info(%uBytes)\r\n",3,len);
		return ;
	}
	while (1)
	{
		ptr = _find_sub_str(psrc,rlen,"\x23\x23",2);
		if (ptr == 0)
		{
			break;
		}
		cmd = ptr[4];
		cmdNumber = (ptr[25]<<8) + ptr[26];
		datunitlen = (ptr[DFGF_CONTENT_OFFSET - 2] << 8) + ptr[DFGF_CONTENT_OFFSET - 1];
		if (rlen >= (DFGF_CONTENT_OFFSET+datunitlen+1))
		{
			pcontent = ptr + DFGF_CONTENT_OFFSET;
			xor = ptr[DFGF_CONTENT_OFFSET+datunitlen];
			DBG_Print(LEVEL0,"----------dfgf sy server ack----------\r\n"
								"cmd:%02X datunitlen:%u xor:%02X\r\n",
								cmd,datunitlen,xor);
			
			//通用应答
			if(ptr[5] == 0xFE)
			{
				slen = _dfgf_DataBag(buf,128,cmd,0x01,cmdNumber,(void *)GetVIN(),(void *)0,0);
				DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
				DBG_PrintBuf(LEVEL2,0,buf,slen);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
			}

			switch (cmd)
			{
			case 0x01:		//登录成功
				DBG_Print(LEVEL0,"(link%u)dfgf login success!!! \r\n",3);
			break;
			case 0x02:		//实时信息上传成功
				DBG_Print(LEVEL0,"(link%u)dfgf date update!!! \r\n",3);
			break;
			case 0x03:		//补发信息上传成功
				DBG_Print(LEVEL0,"(link%u)dfgf date update!!! \r\n",3);
			break;
			case 0x04:		//登出信息上传成功
				DBG_Print(LEVEL0,"(link%u)dfgf date update!!! \r\n",3);
			break;
	
			case 0x80://终端参数查询指令
			{
				switch(pcontent[6])
				{
					case 0x02://数据采集项控制指令 
					{
						ptr_temp = pcontent + 9;
						if(pcontent[8] > MAX_UPLOAD_SEC_NUM)
						{
							DBG_Print(LEVEL0,"the 0x80 cmd 0x02 upload sec num too big!!!\r\n");
							pcontent[8] = MAX_UPLOAD_SEC_NUM;
						}
						for(i = 0;i<pcontent[8];i++)
						{
							if ((ptr_temp[i*3] < 1) || (ptr_temp[i*3] > MAX_UPLOAD_SEC_NUM)) continue;
							stDFGFServerCmd.stDFGFDataUpload[(ptr_temp[i*3]) - 1].is_upload = (ptr_temp[i*3 +1]);
							stDFGFServerCmd.stDFGFDataUpload[(ptr_temp[i*3]) - 1].upload_freq = (ptr_temp[i*3 +2]);
							DBG_Print(LEVEL0,"the 0x80 cmd 0x02 set upload !!!\r\n");
						}
						
						//实时数据06应答
						ptr_temp = buf + DFGF_CONTENT_OFFSET;
						rlen_temp = 0;
						
						//响应时间
						send_msg(S_MSG_GET_RTC_TIME,ptr_temp,0,0);
						rlen_temp +=6;
						ptr_temp[rlen_temp++] = 0x02;
						
						ptr_temp[rlen_temp++] = 0x0;
						ptr_temp[rlen_temp++] = 4+3*MAX_UPLOAD_SEC_NUM;
						
						ptr_temp[rlen_temp++] = 0xFE;
						
						ptr_temp[rlen_temp++] = MAX_UPLOAD_SEC_NUM;
						for(i=0;i<MAX_UPLOAD_SEC_NUM;i++)
						{
							ptr_temp[rlen_temp++] = i + 1;
							ptr_temp[rlen_temp++] = stDFGFServerCmd.stDFGFDataUpload[i].is_upload;
							ptr_temp[rlen_temp++] = stDFGFServerCmd.stDFGFDataUpload[i].upload_freq;
						}
						ptr_temp[rlen_temp++] = 0x01;
						ptr_temp[rlen_temp++] = 0x00;
						ptr_temp[rlen_temp++] = 0x00;
					}
					break;
					
					case 0x03://锁车相关参数查询
					{
						unsigned short check_lockcar_cmd;
						unsigned char result = 0;
						
						check_lockcar_cmd = (pcontent[7] << 8) + pcontent[8];
						if(check_lockcar_cmd == 0x0193)//T-BOX与VCU绑定查询
						{
							result = get_fact_bind_status();
						}
						else if(check_lockcar_cmd == 0x0191)//锁车版本查询 
						{
							result = get_fact_pro_ver_about_lock();
						}
						else if(check_lockcar_cmd == 0x0190)//当前锁车状态查询
						{
							result = get_fact_lock_status();
						}
						else
						{
							DBG_Print(LEVEL0,"the 0x80 cmd check_lockcar_cmd (%u) is invalid!!!\r\n",check_lockcar_cmd);
						}
						
						//实时数据06应答
						ptr_temp = buf + DFGF_CONTENT_OFFSET;
						rlen_temp = 0;
						
						//响应时间
						send_msg(S_MSG_GET_RTC_TIME,ptr_temp,0,0);
						rlen_temp +=6;
						
						ptr_temp[rlen_temp++] = 0x03;
						
						ptr_temp[rlen_temp++] = 0;
						ptr_temp[rlen_temp++] = 4;
						
						ptr_temp[rlen_temp++] = 0xFE;
						
						ptr_temp[rlen_temp++] = 0x01;
						ptr_temp[rlen_temp++] = pcontent[7];
						ptr_temp[rlen_temp++] = pcontent[8];
						ptr_temp[rlen_temp++] = result;
					}
					break;
					case 0x04://日志查询
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
						if(pcontent[7] == 0x01)//立即上传日志
						{
							
						}
					}
					break;
					case 0x05://全量数据功能开关
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
						
					}
					break;
					case 0x06://ADAS智能视频功能开关
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
					}
					break;
					case 0x07://EDR数据上报开关
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
					}
					break;			
					case 0x08://APP 状态同步IVI信息
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
					}
					break;	
					case 0x09://IVI 状态同步APP信息
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
					}
					break;						
					default:
					{
						DBG_Print(LEVEL0,"the 0x80 cmd (%u) is valid!!!\r\n",pcontent[6]);
					}
					
				}
				
				slen = _dfgf_DataBag(buf,128,0x06,0x01,cmdNumber,GetVIN(),ptr_temp,rlen_temp);
				DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
				DBG_PrintBuf(LEVEL2,0,buf,slen);
				send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
				
			}

			break;
#if 1			
			case 0x81:
			{
				switch(pcontent[6])
				{
					case 0x01:// 新能源车型远程锁车指令 
					{
						if(pcontent[7] == 0x90)
						{	
							set_lock_car_cmd_from_where(1);
							if (pcontent[8] == 0x90)
							{
								set_want_lock_status(0);
							}
							else if (pcontent[8] == 0x91)
							{
								set_want_lock_status(1);
							}
							else if (pcontent[8] == 0x92)
							{
								set_want_lock_status(2);
							}
						}
						else if(pcontent[7] == 0x91)
						{
							set_lock_car_cmd_from_where(1);
							if(pcontent[8])
							{
								set_want_pro_ver_about_lock(1);
							}
							else
							{
								set_want_pro_ver_about_lock(0);
								set_want_lock_status(0);
							}
						}
						else if(pcontent[7] == 0x93)
						{
							set_lock_car_cmd_from_where(1);
							if (pcontent[8] == 1)
							{
								set_want_bind_status(1);
							}
							else if (pcontent[8] == 0x10)
							{
								set_want_bind_status(3);
								set_want_lock_status(0);
								set_want_pro_ver_about_lock(0);
							}
							else
							{
								set_want_bind_status(2);
								set_want_lock_status(0);
								set_want_pro_ver_about_lock(0);
							}
						}
						else
						{
							DBG_Print(LEVEL0,"the 0x81 cmd lockcar (%u) (&u)is invalid!!!\r\n",pcontent[7],pcontent[8]);
						}
						
						//实时数据05应答
						ptr_temp = buf + DFGF_CONTENT_OFFSET;
						rlen_temp = 0;
						
						//响应时间
						send_msg(S_MSG_GET_RTC_TIME,ptr_temp,0,0);
						rlen_temp +=6;
						
						ptr_temp[rlen_temp++] = 0x01;
						
						ptr_temp[rlen_temp++] = 0x0;
						ptr_temp[rlen_temp++] = 0x03;
						
						ptr_temp[rlen_temp++] = pcontent[7];//表49//命令类型-有问题
						
						ptr_temp[rlen_temp++] = pcontent[7];//表49
						ptr_temp[rlen_temp++] = pcontent[8];//表49
						ptr_temp[rlen_temp++] = 0x01;
						
						slen = _dfgf_DataBag(buf,128,0x05,0x01,cmdNumber,GetVIN(),ptr_temp,rlen_temp);
						DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
						DBG_PrintBuf(LEVEL2,0,buf,slen);
						send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
					}
					break;
					
					case 0x02://OTA 控制通知下发
					{
						DBG_Print(LEVEL0,"GB32960远程升级\r\n");
					}
					break;
					case 0x03://远程车辆控制指令下发
					{
						DBG_Print(LEVEL0,"DFGF 远程车辆控制\r\n");
						switch(pcontent[7])
						{
							case 0x01:// 远程ON档启动/关闭
							{
								DBG_Print(LEVEL0,"DFGF  远程ON档启动/关闭(%x)\r\n",pcontent[8]);
								if(pcontent[8] == 0x01)
								{
									//send_msg(S_MSG_OUT_IO_CTRL,0,(void *)0,0);
									set_want_startup_status(0);
									send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
									DBG_Print(LEVEL0,"set_want_startup_status vaule\r\n");
								}
								else if(pcontent[8] == 0x02)
								{
									set_want_lockdoor_status(1);//设置为默认解锁状态
									//send_msg(S_MSG_OUT_IO_CTRL,0,(void *)1,0);
									set_want_startup_status(1);
									send_msg(S_MSG_GPS_LED_CTRL,(void *)1,0,0);
									DBG_Print(LEVEL0,"set_want_startup_status vaule\r\n");
								}
							}
							break;
							case 0x02:// 远程门窗上锁/解锁
							{
								DBG_Print(LEVEL0,"DFGF  远程门窗上锁/解锁\r\n");
								DBG_Print(LEVEL0,"DFGF  pcontent[8] = %d\r\n",pcontent[8]);
								DBG_Print(LEVEL0,"DFGF  pcontent[9] = %d\r\n",pcontent[9]);
								DBG_Print(LEVEL0,"DFGF  pcontent[10] = %d\r\n",pcontent[10]);
								stDFGFServerCmd.stDFGFCtrlStatus.door_window_status[0] = pcontent[8];
								stDFGFServerCmd.stDFGFCtrlStatus.door_window_status[1] = pcontent[9];
								stDFGFServerCmd.stDFGFCtrlStatus.door_window_status[2] = pcontent[10];
								set_lockdoor_commond_flag(1);
								set_want_lockdoor_status(pcontent[8] & 0x01);
								short_loop_1811D4F5 = 3;
							}
							break;
							case 0x03:// 远程空调
							{
								DBG_Print(LEVEL0,"DFGF   远程空调\r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFAirCtrl),pcontent + 8,sizeof(DFGFAirCtrl));
							}
							break;
							case 0x04:// 远程寻车
							{
								DBG_Print(LEVEL0,"DFGF   远程寻车\r\n");
								stDFGFServerCmd.stDFGFCtrlStatus.lookup_car = pcontent[8];
								short_loop_1811D4F5 = 3;
							}
							break;
							
							case 0x06://预约电池加热
							{
								DBG_Print(LEVEL0,"DFGF  预约电池加热\r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFDianchiCtrl),pcontent + 8,sizeof(DFGFDianchiCtrl));
							}
							break;

							case 0x07://预约充电
							{
								DBG_Print(LEVEL0,"DFGF  预约充电\r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFChargeCtrl),pcontent + 8,sizeof(DFGFChargeCtrl));
							}
							break;
							
							case 0x08://远程喇叭
							{
								DBG_Print(LEVEL0,"DFGF  远程喇叭\r\n");
								stDFGFServerCmd.stDFGFCtrlStatus.horn_ctrl = pcontent[8];
							}
							break;							
							case 0x09:// 远程闪灯
							{
								DBG_Print(LEVEL0,"DFGF  远程闪灯\r\n");
								stDFGFServerCmd.stDFGFCtrlStatus.light_ctrl = pcontent[8];
							}
							break;	
							case 0x0A://上装控制（货箱/冷藏车温度）
							{
								DBG_Print(LEVEL0,"DFGF  上装控制（货箱/冷藏车温度）\r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFShangzhuangCtrl),pcontent + 8,sizeof(DFGFShangzhuangCtrl));
							}
							break;
							case 0x0B://方向盘加热 
							{
								DBG_Print(LEVEL0,"DFGF  方向盘加热 \r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFFangxaingpanCtrl),pcontent + 8,sizeof(DFGFFangxaingpanCtrl));
							}
							break;
							case 0x0C://远程座椅控制 
							{
								DBG_Print(LEVEL0,"DFGF  远程座椅控制 \r\n");
								memcpy(&(stDFGFServerCmd.stDFGFCtrlStatus.stDFGFZuoyiCtrl),pcontent + 8,sizeof(DFGFZuoyiCtrl));
								short_loop_1811D4F5 = 3;
							}
							break;
							case 0x0D://授权驾驶 
							{
								DBG_Print(LEVEL0,"DFGF  授权驾驶 \r\n");
							}
							break;							
							default:
							{
								DBG_Print(LEVEL0,"the 0x81 cmd ctrl (%u) is invalid!!!\r\n",pcontent[7]);
							}
						}
						
						//实时数据05应答
						ptr_temp = buf + DFGF_CONTENT_OFFSET;
						rlen_temp = 0;
						
						//响应时间
						send_msg(S_MSG_GET_RTC_TIME,ptr_temp,0,0);
						rlen_temp +=6;
						
						ptr_temp[rlen_temp++] = 0x03;
						
						ptr_temp[rlen_temp++] = 0x0;
						ptr_temp[rlen_temp++] = 3;
						
						ptr_temp[rlen_temp++] = pcontent[7];//命令类型-有问题
						
						ptr_temp[rlen_temp++] = pcontent[7];
						
						ptr_temp[rlen_temp++] = 0x01;
						
						ptr_temp[rlen_temp++] = 0x00;
						ptr_temp[rlen_temp++] = 0x00;
						
						slen = _dfgf_DataBag(buf,128,0x05,0x01,cmdNumber,GetVIN(),ptr_temp,rlen_temp);
						DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
						DBG_PrintBuf(LEVEL2,0,buf,slen);
						send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
					}
					break;
							
					default:
					{
						DBG_Print(LEVEL0,"the 0x81 cmd (%u) is invalid!!!\r\n",pcontent[6]);
					}
				}
			}
			break;
#endif
			default:break;
			}

		}
		else
		{
			DBG_Print(LEVEL0,"server data format error.\r\n");
			break;
		}
		psrc = ptr + DFGF_CONTENT_OFFSET + datunitlen + 1;
		if (((unsigned char *)pbuf + len) > psrc)
		{
			rlen = (unsigned char *)pbuf + len - psrc;
		}
		else
		{
			break;
		}
	}
}


void dfgf_sy_analyze(unsigned char channel,CanRxMsg *prxmsg)
{
	
	INT32U canid = 0;
	INT8U *pdat = 0;

	if (prxmsg == 0)
	{
		return ;
	}
	pdat = prxmsg->Data;
	canid = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;

	switch (canid)
	{
		case 0x0C0128D0:
		{
			dfgf_can_set_dianji_dianya(0,pdat[0] + (pdat[1] << 8));
			dfgf_can_set_dianji_dianya(1,pdat[0] + (pdat[1] << 8));
			//dfgf_can_set_dianji_dianliu(0,(pdat[0] + (pdat[1] << 8)));
			//dfgf_can_set_dianji_rpm(0,(pdat[2] + (pdat[3] << 8)));
			dfgf_can_set_car_speed(pdat[5]);
			//dfgf_can_set_jiasu_taban(pdat[6] + (pdat[7] << 8));
		}
		break;
		case 0x0C01F4D0:
		{
			dfgf_can_set_air_cond_status_wei(2,(pdat[3] >> 4) & 0x01);
			//dfgf_can_set_air_cond_status_wei(4,(pdat[0] >> 2) & 0x03);
			//dfgf_can_set_air_cond_status_wei(5,(pdat[0] >> 2) & 0x03);
			
		}	
		break;
		case 0x0C0228D0:
		{
			//dfgf_can_set_hand_brk((pdat[4] >> 0) & 0x01);
			//dfgf_can_set_zhidong_taban((pdat[4] >> 4) & 0x01);
			//dfgf_can_set_dianji_temp(0,pdat[6]);
			//dfgf_can_set_dianji_ctrl_temp(0,pdat[7]);
			if(pdat[1] > 0)
			{
				stDFGFWarnInfo.dtc_code[0] = pdat[3];
				setbit(stDFGFWarnInfo.dtc_flag,0);
			}
		}	
		break;
		case 0x0C02D4D0:
		{
			//dfgf_can_set_car_status((pdat[0] & 0x07));			
			dfgf_can_set_dangwei((pdat[1] >> 0) & 0x07);
			//dfgf_can_set_dcdc_status((pdat[1] >> 3) & 0x07);
			dfgf_can_set_dianji_cnt(pdat[2]);
			//dfgf_can_set_dianji_num();	
			dfgf_can_set_dianji_status(pdat[3] - 1,(pdat[5] & 0x07));
			dfgf_can_set_dianji_ctrl_temp(pdat[3] - 1,(pdat[6]));
			dfgf_can_set_dianji_rpm(pdat[3] - 1,(pdat[2] + (pdat[3] << 8)));
			dfgf_can_set_dianji_tor(pdat[3] - 1,pdat[4] + (pdat[5] << 8));
			dfgf_can_set_dianji_temp(pdat[3] - 1,pdat[7]);
		}	
		break;
		case 0x0C0328D0:
		{
			dfgf_can_set_xunhang_status((pdat[2] >> 0) & 0x03);
			dfgf_can_set_key_dangwei((pdat[2] >> 2) & 0x03);
			dfgf_can_set_xunhang_speed(pdat[3]);
			dfgf_can_set_xuhang_mail(pdat[5] + (pdat[6] << 8));
		}	
		break;		
		case 0x0C04D9D0:
		{
			dfgf_can_set_cond_fan_pwm(pdat[1]);

			dfgf_can_set_air_cond_status_wei(6,(pdat[5] >> 1) & 0x01);//低速风扇状态
			dfgf_can_set_air_cond_status_wei(7,(pdat[5] >> 2) & 0x01);//高速风扇状态
			dfgf_can_set_air_cond_status_wei(8,(pdat[5] >> 7) & 0x01);//干燥器气压开关状态
			dfgf_can_set_air_cond_status_wei(9,(pdat[5] >> 4) & 0x01);//PTC请求状态
			dfgf_can_set_air_cond_status_wei(10,(pdat[5] >> 0) & 0x01);//电控冷却水泵工作状态
		}	
		break;
		case 0x0CF00400:
		{
			dfgf_can_set_quzhou_rpm(pdat[3] + (pdat[4] << 8));
		}	
		break;
		case 0x1800D0F4:
		{
			dfgf_can_set_zong_dianya(pdat[0] + (pdat[1] << 8));
			//dfgf_can_set_dianji_dianya(0,pdat[0] + (pdat[1] << 8));
			//dfgf_can_set_dianji_dianya(1,pdat[0] + (pdat[1] << 8));
			//dfgf_can_set_charge_dianya(pdat[0] + (pdat[1] << 8));
			
			dfgf_can_set_zong_dianliu(pdat[2] + (pdat[3] << 8));
			dfgf_can_set_charge_dianliu(pdat[2] + (pdat[3] << 8));
			
			dfgf_can_set_soc_value(pdat[4]);
			
			//dfgf_can_set_charge_connect_status(pdat[5] &0x0F);
			//dfgf_can_set_connect_comfirm(pdat[5] &0x0F);
			
			//dfgf_can_set_connect_status((pdat[6] >> 0) & 0x03);
		}	
		break;
		case 0x1801D0DA:
		{
			dfgf_can_set_air_cond_status_wei(1,(pdat[6] >> 0) & 0x03);
			
			//dfgf_can_set_cond_dianya(pdat[0]);
			//dfgf_can_set_cond_dianliu(pdat[2]);
			//dfgf_can_set_cond_temper(pdat[1]);
			//dfgf_can_set_cond_gonglv(pdat[3]);
			
			//dfgf_can_set_cond_fan_rpm(pdat[4] + (pdat[5] << 8));
		}
		break;
		case 0x1801D4F4:
		{
			dfgf_can_set_charge_in(pdat[0] & 0x07);
			dfgf_can_set_charging_status(pdat[0] & 0x07);
			dfgf_can_set_jueyuan_dianzu(pdat[1] + (pdat[2] << 8));			
		}	
		break;
		case 0x1801E5F4:
		{
			dfgf_can_set_connect_comfirm((pdat[4] >> 6) & 0x03);
		}	
		break;
		case 0x1801F428:
		{
			dfgf_can_set_licheng(pdat[0] + (pdat[1] << 8)+ (pdat[2] << 16) + (pdat[3] << 24));
		}	
		break;
		case 0x1801F528:
		{
			dfgf_can_set_shache_warn_status((pdat[2] >> 4) & 0x01);
			
			dfgf_can_set_shunshi_dianhao(pdat[4] + (pdat[5] << 8));
		}	
		break;
		case 0x1802D0DB:
		{
			dfgf_can_set_air_cond_status_wei(3,(pdat[0] >> 0) & 0x01);
			
			dfgf_can_set_wptc_wendu(pdat[1]);
			dfgf_can_set_wptc_dianliu(pdat[2]);
			dfgf_can_set_wptc_dianya(pdat[3]);
			dfgf_can_set_wptc_gonglv(pdat[4]);
		}
		case 0x180CD4F4:
		{
			dfgf_can_set_charge_power(pdat[0] + (pdat[1] << 8));
			dfgf_can_set_zong_huikui_power(pdat[3] + (pdat[4] << 8)+ (pdat[5] << 16));
		}	
		break;
		case 0x1810D0DC:
		{
			dfgf_can_set_env_temper(pdat[1]);			
		}	
		break;		
		case 0x1812D0D9:
		{
			dfgf_can_set_cond_dianya(pdat[0]);
			dfgf_can_set_cond_dianliu(pdat[1]);
			dfgf_can_set_cond_temper(pdat[2]);
			dfgf_can_set_cond_gonglv(pdat[3]);
			dfgf_can_set_cond_fan_rpm(pdat[4] + (pdat[5] << 8));
		}	
		break;		
		case 0x1815E2F4:
		{
			//dfgf_can_set_dianchi_shuaijian(pdat[7]);
			dfgf_can_set_zong_charge_power(pdat[0] + (pdat[1] << 8)+ (pdat[2] << 16));
			dfgf_can_set_zong_loss_power(pdat[4] + (pdat[5] << 8)+ (pdat[6] << 16));
		}	
		break;
		case 0x181C28F4:
		{
			dfgf_can_set_max_dianchi_wendu(pdat[4] );
		}
		break;
		case 0x1820E2F4:
		{
			dfgf_can_set_dianchi_yulaing(pdat[0] + (pdat[1] << 8));		
		}
		break;
		case 0x18F0010B:
		{
			dfgf_can_set_zhidong_taban(pdat[1]);		
		}
		break;		
		case 0x18F120F0:
		{
			//dfgf_can_set_dianji_rpm(0,pdat[2] + (pdat[3] << 8));
			//dfgf_can_set_dianji_tor(0,pdat[4] + (pdat[5] << 8));
			//dfgf_can_set_dianji_temp(0,pdat[6]);
			//dfgf_can_set_dianji_ctrl_temp(0,pdat[7]);				
		}	
		break;
		case 0x18FF4A19:
			
		{
			dfgf_can_set_air_cond_status_wei(0,(pdat[0] >> 2) & 0x03);
			dfgf_can_set_feng_mode((pdat[1] >> 0) & 0x07);
			//dfgf_can_set_feng_speed((pdat[1] >> 3) & 0x1F);
			dfgf_can_set_driver_env_temper(pdat[5]);				
		}	
		break;
		case 0x18FF4B19:
		{
			dfgf_can_set_zhidong_taban(pdat[1]);		
		}
		break;
		case 0x18FF50E5:
		{
			dfgf_can_set_connect_status((pdat[4] >> 4) & 0x03);
		}
		break;
		case 0x187C9CA7:
		{
			dfgf_can_set_dcdc_dianya(pdat[1]);
			dfgf_can_set_dcdc_dianliu(pdat[2]);
			dfgf_can_set_dcdc_temper(pdat[3]);
			dfgf_can_set_dcdc_gonglv(pdat[4]);
		}
		break;
		case 0x18FF7021:
		{
			dfgf_can_set_light_status_wei(0,((pdat[4] >> 4) & 0x03));
			dfgf_can_set_light_status_wei(1,((pdat[4] >> 6) & 0x03));
			//dfgf_can_set_light_status_wei(2,((pdat[0] >> 4) & 0x03));
			dfgf_can_set_light_status_wei(3,((pdat[0] >> 4) & 0x03));
			dfgf_can_set_light_status_wei(4,((pdat[0] >> 6) & 0x03));
			dfgf_can_set_light_status_wei(5,((pdat[1] >> 4) & 0x03));
			dfgf_can_set_light_status_wei(6,((pdat[6] >> 2) & 0x03));
			dfgf_can_set_light_status_wei(7,((pdat[4] >> 2) & 0x03));
			dfgf_can_set_light_status_wei(8,((pdat[0] >> 0) & 0x03));
			dfgf_can_set_light_status_wei(9,((pdat[0] >> 2) & 0x03));
			
			//dfgf_can_set_safe_bind_status_wei(0,(pdat[1] >> 6) & 0x03);
			//dfgf_can_set_safe_bind_status_wei(1,(pdat[6] >> 6) & 0x03);
			
			//dfgf_can_set_door_lock_status_wei(0,(pdat[3] >> 0) & 0x03);
			//dfgf_can_set_door_lock_status_wei(1,(pdat[2] >> 4) & 0x03);
			//dfgf_can_set_door_lock_status_wei(2,(pdat[2] >> 6) & 0x03);
			//dfgf_can_set_door_lock_status_wei(3,(pdat[2] >> 0) & 0x03);
			//dfgf_can_set_door_lock_status_wei(4,(pdat[2] >> 2) & 0x03);
		}
		break;
		case 0x18FF7321:
		{
			dfgf_can_set_windows_status_wei(0,(pdat[0] >> 0) & 0x07);
			dfgf_can_set_windows_status_wei(1,(pdat[0] >> 3) & 0x07);
		}	
		break;

		
//故障码信息
		
		case 0x0C0128D2:
		{
			if(pdat[0] > 0)
			{
				stDFGFWarnInfo.dtc_code[1] = pdat[1] + 688;
				setbit(stDFGFWarnInfo.dtc_flag,1);
			}
		}	
		break;	
		
		case 0x181B28F4:
		{
			if(pdat[0] > 0)
			{
				stDFGFWarnInfo.dtc_code[3] = pdat[0] + (pdat[1] << 8);
				setbit(stDFGFWarnInfo.dtc_flag,3);
			}
			
		}	
		break;
		
		case 0x1801F4E2:
		{
			if(pdat[5] > 0)
			{
				stDFGFWarnInfo.dtc_code[4]= pdat[6] + (pdat[7] & 0x0F)<< 8;
				setbit(stDFGFWarnInfo.dtc_flag,4);
			}
			
		}	
		break;
		
		case 0x18E7EF21:
		{
			if(pdat[0] > 0)
			{
				stDFGFWarnInfo.dtc_code[5] = pdat[1] + (pdat[2] & 0x0F)<< 8;
				setbit(stDFGFWarnInfo.dtc_flag,5);
			}
			
		}	
		break;
		
		case 0x18E1EF20:
		{
			if(pdat[3] > 0)
			{
				stDFGFWarnInfo.dtc_code[6] = pdat[4] + (pdat[5] & 0x0F)<< 8;
				setbit(stDFGFWarnInfo.dtc_flag,6);
			}
			
		}	
		break;
		
		case 0x187D9CA7:
		{
			if(pdat[5] > 0)
			{

				stDFGFWarnInfo.dtc_code[7] = pdat[6] + (pdat[7] & 0x0F)<< 8;
				setbit(stDFGFWarnInfo.dtc_flag,7);
			}
			
		}	
		break;
		
		
//新能源拓展协议



//车辆告警上报
		case 0x1811D4F5:
		{
			if((pdat[4]>> 4) & 0x3 == 0x01)
			{
				//电池被盗，报警
			}
		}	
		break;
		case 0x18FF6033:
		{
			dfgf_can_set_tire(pdat[0],pdat[1],(pdat[6] & 0x7));
		}	
		break;

		default:
			break;
	}
	
}
