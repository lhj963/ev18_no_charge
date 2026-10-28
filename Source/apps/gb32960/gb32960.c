#include "gb32960.h"


#define MAX_SDDF_DIANCHIBAO_CNT  3			//时代电服电池包最大个数

typedef struct tagSDDF_CHONGFANGDIAN_DATA
{
	INT8U xuhao;										// 序号

	INT8U SOH;											//电池包SOH
	INT16U total_fangdian_cycle;						//累计放电总cycle
	INT16U total_chongdian_cycle;						//累计充电总cycle
	INT32U total_zhanwai_fangdian_nengliang;			//累加换电站外放电能量
	INT32U total_zhanwai_huichong_nengliang;			//累加换电站外回充能量
	INT32U total_zhanwai_chaqiang_nengliang;			//累加换电站外插枪充电能量
	INT32U total_milage;								//电池包累计行驶里程
	INT32U total_fangdian_nengliang;					//电池包累计放电总能量
	INT32U total_chongdian_nengliang;					//电池包累计充电总能量
}SDDF_CHONGFANGDIAN_DATA;

typedef struct tagSDDF_BMU_INFO
{
	INT8U bmu_xuhao;					//bmu序号
	INT8U bmu_ver_poll_id[4];
	INT8U bmu_ver[21 * 4];
	INT8U bmu_mac[6];
}SDDF_BMU_INFO;

typedef struct tagSDDF_DIANCHIBAO_INFO
{
	INT8U xuhao;										//序号
	INT32U zhanwai_fd_nengliang;						//电池包小计站外放电能量
	INT32U zhanwai_cd_nengliang;						//电池包小计站外插枪充电能量
	INT32U zhanwai_hc_nengliang;						//电池包小计站外回充能量
	INT32U milage;										//电池包小计行驶里程
	INT8U zhuzheng_xianquan_status;						//电池包主正线圈状态
	INT8U zhufu_xianquan_status;						//电池包主负线圈状态
	INT16U dianliu;										//电池包电流
	INT16U dianya;										//电池包电压
	INT8U soc;											//电池包SOC
	INT16U soe;											//电池包SOE
}SDDF_DIANCHIBAO_INFO;

typedef struct tagDefProtocolStatus
{
	//东风基础锁车版本参数
	//INT8U want_bind_status;					//希望的绑定状态
	INT8U fact_bind_status;					//实际的绑定状态
	INT8U fact_pro_ver_about_lock;			//实际的锁车协议版本
	INT8U fact_lock_status;					//实际的锁车状态 0 未锁  1 锁定
	INT8U iccid_bytes_in_ecu[8];			//ecu中存储的ICCID后16位  16位 用8字节表示
	INT8U iccid_head_bytes_in_ecu[4];		//ecu中存储的ICCID前4位
	INT8U iccid_head_flag_in_ecu;			//获取到了存储在ecu的ICCID前4未标志
	INT8U lock_car_can_channel;				//锁车的CAN通道

	//dst锁车参数
	INT8U dst_want_bms_charge_flag;				//希望的充电请求标志	
	INT8U dst_fact_bms_charge_flag;				//实际充电条件标识
	INT16U budian_init_vol;						//开始补电的初始电压
	
	//EM26 车型制冷系统数据
	INT8U em26_fan_ls_status;					//低速风扇（或冷却风扇）工作状态
	INT8U em26_fan_hs_status;					//高速风扇（或冷凝风扇）工作状态
	INT8U em26_air_cond_status;					//空调基本状态
	INT16U em26_air_cond_speed;					//空调压缩机转速
	
	//EV18 车型制冷系统数据
	INT8U ev18_fan_ls_status;					//低速风扇（或冷却风扇）工作状态
	INT8U ev18_fan_hs_status;					//高速风扇（或冷凝风扇）工作状态
	INT16U ev18_air_cond_speed;					//空调压缩机转速
	INT8U ev18_air_cond_status;					//空调压缩机开关
	INT16U ev18_air_cond_ctrl_speed;			//压缩机控制器转速反馈
	INT8U ev18_air_cond_ctrl_status;			//压缩机控制器工作状态
	
	INT8U lockdoor_commond_flag;
	
	INT8U want_startup_status : 2;						
	INT8U want_lockdoor_status : 2;					
	INT8U want_control_air_cond : 4;				
	
	INT8U fact_startup_status : 2;						
	INT8U fact_lockdoor_status : 2;					
	INT8U fact_control_air_cond : 4;
	
	INT32U total_charge_in;
	INT32U total_charge_out;
	
	INT8U d_safe_belt_status;					//主驾安全带状态
	INT8U ds_front_door_status;					//主驾驶门状态
	INT8U l_front_door_status;					//闭锁/解锁状态反馈
	INT8U ON_switch_status;						//ON上电状态
	
	INT8U power_switch_status;					//电源总开关状态
	INT8U key_in_status;						//钥匙插入状态
	
	INT8U hand_brk_status;						//手刹挡位
	INT8U brk_status;							//制动状态
	INT8U ready_status;							//Ready
	
	INT8U key_status;							//钥匙挡位
	INT16U steer_wheel_angle;					//方向盘转角
	INT8U power_ctrl_allow:4;					//上下电允许指令
	INT8U zhufujiechuqi_status:2;				//主负接触器状态
	INT8U bms_wakeup_status:2;					//BMS 唤醒状态 

	INT8U vcu_wakeup_status:2;					//VCU唤醒其他控制器电源	
	INT8U door_lock_status:2;					//门锁状态 
	INT8U ce_door_switch:2;						//侧滑门开关
	INT8U back_door_switch:2;					//后背门开关
	
	INT8U l_f_door_switch:2;					//司机侧前门开关
	INT8U r_f_door_switch:2;					//副驾驶侧前门开关
	INT8U l_f_door_status:2;					//左前门锁执行器状态
	INT8U center_door_status:2;					//中央门控锁开关
	
	INT8U cond_switch:2;						//压缩机工作使能
	INT8U cond_switch_status:2;					//压缩机启停状态
	INT8U xunhuan_mode_status:2;				//内外循环状态
	INT8U shaung_mode_status:2;					//快速除霜模式 
	
	INT8U cond_far_func:2;						//空调数据长度 
	INT8U ac_quire_status:2;					//AC 请求
	INT8U air_work_status:2;					//空调工作状态 
	INT8U ptc_work_status:2;					//PTC 工作状态
	
	INT8U lose_power_fail_status:4;				//下电失败状态 
	INT8U lock_door_fail_status:4;				//上锁失败状态
	
	INT8U feng_mode_status:4;					//吹风模式
	INT8U feng_dangwei_status:4;				//风量显示 
	INT8U wendu_dangwei_status;				//wendu显示 
	
	//D78自定义数据
	INT8U d78_car_status;
	INT16U d78_soe;
	INT8U d78_soh;
	INT8U d78_dcdc_output_vol;					//DCDC输出电压
	INT16U d78_ptc;								//ptc功率
	INT16U d78_cce_rpm;							//cce转速

	INT8U d78_bat_yuchong_status;				//电池预充状态
	INT16U d78_bms_output_vol;					//bms输出电压
	INT16U d78_mcu_input_vol;					//mcu输入电压
	INT8U d78_dcdc_input_vol;					//DCDC输入电压


	INT8U d78_mcu_gz_level;						//mcu故障等级
	INT8U d78_mcu_gz_code[8];					//mcu故障代码
	INT8U d78_vcu_gz_level;
	INT8U d78_vcu_gz_code;

	INT8U d78_dcdc_gz_level; 					//dcdc故障等级
	INT8U d78_dcdc_gz_code[8];					//dcdc故障代码
	
	INT8U d78_bms_gz_level;
	INT8U d78_bms_gz_code;

	INT8U d78_obc_gz_level;
	INT8U d78_obc_gz_code;

	INT8U d78_chm_hot_level;					//chm制热系统故障等级
	INT8U d78_chm_cool_level;					//chm制冷系统故障等级
	INT8U d78_mcu_igbt_flag;					//mcu igbt标志位
	INT8U d78_zhenkong_gz_flag;					//真空故障标志位

	//时代电服数据
	//mbmu sbmu数据
	SDDF_BMU_INFO sddf_mbmu[1];
	SDDF_BMU_INFO sddf_sbmu[MAX_SDDF_DIANCHIBAO_CNT];

	//充放电数据
	SDDF_CHONGFANGDIAN_DATA sddf_cfd_data[MAX_SDDF_DIANCHIBAO_CNT];

	//充电数据
	INT16U sddf_benci_chongdian_rongliang[MAX_SDDF_DIANCHIBAO_CNT];		//电池包本次充电容量

	//ACP数据
	INT32U sddf_xitongfangdian_leiji_guoliu;							//系统放电累计过流容量
	INT32U sddf_xitonghuichong_leiji_guoliu;							//系统回充累计过流容量
	INT32U sddf_xitongchaqiang_leiji_guoliu;							//系统插枪充电累计过流容量
	INT16U sddf_xitong_pingjun_dianya;									//系统平均电压
	INT8U sddf_xitong_pingjun_wendu;									//系统平均温度
	INT8U sddf_dianchi_xt_zhu_zheng_status;								//电池系统主正接触器线圈状态
	INT8U sddf_dianchi_xt_zhu_fu_status;								//电池系统主负接触器线圈状态
	INT8U sddf_dianchi_zhanwai_chongdian_flag;							//电池站外充电起止标志
	INT32U sddf_xitong_zhanwai_chaqiang_xj_cnt;							//系统小计站外插枪充电次数
	INT32U sddf_xitong_zhanwai_chaqiang_lj_cnt;							//系统累计站外插枪充电次数
	INT8U sddf_dianchibao_cnt;											//电池包数量(1-3)
	SDDF_DIANCHIBAO_INFO sddf_dianchi[MAX_SDDF_DIANCHIBAO_CNT];


	//平台期望值
	INT8U sddf_want_chongdian_limit_soc;
	INT8U sddf_want_chongdian_limit_flag;

	//bms实际反馈值
	INT8U sddf_fact_chongdian_limit_soc;
	INT8U sddf_fact_chongdian_limit_flag;

	INT8U sddf_login_ok_flag;
	INT32U xuhang_licheng;							//续航里程
	INT8U xunhang_status;							//定速巡航
	INT8U xunhang_speed;							//巡航速度
	
}DefProtocolStatus;

DefProtocolStatus stDefProtocolStatus = {0};
INT8U sos_test_flag = 0; /* production default: alarm test mask off */
INT32U sos_test_flag_val = 0;
INT32U test_sos_time[19] = {0};
INT8U s_mq_flag = 0;
INT16U s_inteval = 10;
INT8U s_d78_huandian_ver = 0;		//D78换电版本

INT32U s_the_id_table = 0;

INT8U s_c0728d0_flag = 0;

static unsigned char s_ev18_cond = 0;

INT8U collect_32960_acc_status = 0;

INT8U bcm_online_status = 0;

void set_bcm_online_status(unsigned char val)
{
	bcm_online_status = val;
}

unsigned char get_bcm_online_status(void)
{
	return bcm_online_status;
}

void set_collect_32960_acc_status(unsigned char val)
{
	collect_32960_acc_status = val;
}

unsigned char get_collect_32960_acc_status(void)
{
	return collect_32960_acc_status;
}
static unsigned char s_32960_zuocang_data_change = 0;

void set_s_32960_zuocang_data_change(unsigned char val)
{
	s_32960_zuocang_data_change = val;
}

unsigned char get_s_32960_zuocang_data_change(void)
{
	return s_32960_zuocang_data_change;
}

void set_0c0728d0_flag(unsigned char val)
{
	s_c0728d0_flag = val;
}

unsigned char get_0c0728d0_flag(void)
{
	return s_c0728d0_flag;
}

unsigned char get_sddf_login_ok_flag(void)
{
	return stDefProtocolStatus.sddf_login_ok_flag;
}

void set_sddf_login_ok_flag(unsigned char val)
{
	stDefProtocolStatus.sddf_login_ok_flag = val;
}

static INT16U s_gb32960_login_ok_status = 0;

unsigned char get_gb32960_login_ok_flag(unsigned char linkid)
{
	if (linkid >= 16)
	{
		return 0;
	}
	return (s_gb32960_login_ok_status & (1 << linkid)) ? 1 : 0;
}

void set_gb32960_login_ok_flag(unsigned char linkid, unsigned char val)
{
	if (linkid >= 16)
	{
		return ;
	}
	if (val)
	{
		s_gb32960_login_ok_status |= (1 << linkid);
	}
	else
	{
		s_gb32960_login_ok_status &= ~(1 << linkid);
	}
}

void sddf_can_analyze(unsigned canid,void *pdat,unsigned int datlen)
{
	unsigned int temp = 0;

	switch (canid)
	{
	case 0x0501:

	break;
	case 0x0502:
		stDefProtocolStatus.sddf_xitongfangdian_leiji_guoliu = decode_can_frame(pdat,24,32,1);
		stDefProtocolStatus.sddf_xitonghuichong_leiji_guoliu = decode_can_frame(pdat,56,32,1);
	break;

	case 0x0503:
		stDefProtocolStatus.sddf_xitongchaqiang_leiji_guoliu = decode_can_frame(pdat,24,32,1);
		stDefProtocolStatus.sddf_xitong_pingjun_dianya = decode_can_frame(pdat,40,16,1);
		stDefProtocolStatus.sddf_xitong_pingjun_wendu = decode_can_frame(pdat,48,8,1);
		stDefProtocolStatus.sddf_dianchi_xt_zhu_zheng_status = decode_can_frame(pdat,63,1,1);
		stDefProtocolStatus.sddf_dianchi_xt_zhu_fu_status = decode_can_frame(pdat,62,1,1);
		stDefProtocolStatus.sddf_dianchi_zhanwai_chongdian_flag = decode_can_frame(pdat,60,2,1);
	break;

	case 0x0504:
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[0] = *((unsigned char *)pdat + 5);
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[1] = *((unsigned char *)pdat + 4);
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[2] = *((unsigned char *)pdat + 3);
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[3] = *((unsigned char *)pdat + 2);
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[4] = *((unsigned char *)pdat + 1);
		stDefProtocolStatus.sddf_mbmu[0].bmu_mac[5] = *((unsigned char *)pdat + 0);
	break;

	case 0x0505:
		stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_xj_cnt = decode_can_frame(pdat,24,32,1); 				//系统小计站外插枪充电次数
		stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_lj_cnt = decode_can_frame(pdat,56,32,1); 				//系统累计站外插枪充电次数
	break;

	case 0x0506:
		stDefProtocolStatus.sddf_mbmu[0].bmu_ver_poll_id[0] = decode_can_frame(pdat,0,4,1);;

		temp = decode_can_frame(pdat,4,4,1);
		if ((temp >= 1) && (temp <= 12))
		{
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 0) = *((unsigned char *)pdat + 1);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 1) = *((unsigned char *)pdat + 2);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 2) = *((unsigned char *)pdat + 3);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 3) = *((unsigned char *)pdat + 4);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 4) = *((unsigned char *)pdat + 5);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 5) = *((unsigned char *)pdat + 6);
			*(stDefProtocolStatus.sddf_mbmu[0].bmu_ver + (7 * (temp - 1)) + 6) = *((unsigned char *)pdat + 7);
		}
	break;

	case 0x0401:
		stDefProtocolStatus.sddf_sbmu[0].bmu_ver_poll_id[0] = decode_can_frame(pdat,0,4,1);

		temp = decode_can_frame(pdat,4,4,1);
		if ((temp >= 1) && (temp <= 12))
		{
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 0) = *((unsigned char *)pdat + 1);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 1) = *((unsigned char *)pdat + 2);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 2) = *((unsigned char *)pdat + 3);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 3) = *((unsigned char *)pdat + 4);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 4) = *((unsigned char *)pdat + 5);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 5) = *((unsigned char *)pdat + 6);
			*(stDefProtocolStatus.sddf_sbmu[0].bmu_ver + (7 * (temp - 1)) + 6) = *((unsigned char *)pdat + 7);
		}
	break;

	case 0x0402:
		stDefProtocolStatus.sddf_sbmu[1].bmu_ver_poll_id[0] = decode_can_frame(pdat,0,4,1);;

		temp = decode_can_frame(pdat,4,4,1);
		if ((temp >= 1) && (temp <= 12))
		{
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 0) = *((unsigned char *)pdat + 1);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 1) = *((unsigned char *)pdat + 2);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 2) = *((unsigned char *)pdat + 3);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 3) = *((unsigned char *)pdat + 4);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 4) = *((unsigned char *)pdat + 5);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 5) = *((unsigned char *)pdat + 6);
			*(stDefProtocolStatus.sddf_sbmu[1].bmu_ver + (7 * (temp - 1)) + 6) = *((unsigned char *)pdat + 7);
		}
	break;

	case 0x0403:
		stDefProtocolStatus.sddf_sbmu[2].bmu_ver_poll_id[0] = decode_can_frame(pdat,0,4,1);

		temp = decode_can_frame(pdat,4,4,1);
		if ((temp >= 1) && (temp <= 12))
		{
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 0) = *((unsigned char *)pdat + 1);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 1) = *((unsigned char *)pdat + 2);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 2) = *((unsigned char *)pdat + 3);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 3) = *((unsigned char *)pdat + 4);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 4) = *((unsigned char *)pdat + 5);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 5) = *((unsigned char *)pdat + 6);
			*(stDefProtocolStatus.sddf_sbmu[2].bmu_ver + (7 * (temp - 1)) + 6) = *((unsigned char *)pdat + 7);
		}
	break;

	case 0x0404:
		temp = decode_can_frame(pdat,0,4,1);
		if ((temp >= 1) && (temp <= MAX_SDDF_DIANCHIBAO_CNT))
		{
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[0] = *((unsigned char *)pdat + 6);
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[1] = *((unsigned char *)pdat + 5);
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[2] = *((unsigned char *)pdat + 4);
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[3] = *((unsigned char *)pdat + 3);
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[4] = *((unsigned char *)pdat + 2);
			stDefProtocolStatus.sddf_sbmu[temp - 1].bmu_mac[5] = *((unsigned char *)pdat + 1);
		}
	break;

	case 0x0101:

	break;

	case 0x0102:

	break;

	case 0x0103:

	break;
	case 0x0104:

	break;

	case 0x0105:

	break;

	case 0x0106:

	break;
	case 0x0107:

	break;
	case 0x0108:

	break;

	case 0x0201:

	break;
	case 0x0202:

	break;
	case 0x0203:

	break;
	case 0x0204:

	break;
	case 0x0205:

	break;
	case 0x0206:

	break;
	case 0x0207:

	break;
	case 0x0208:

	break;

	case 0x0301:

	break;
	case 0x0302:

	break;
	case 0x0303:

	break;
	case 0x0304:

	break;
	case 0x0305:

	break;
	case 0x0306:

	break;
	case 0x0307:

	break;
	case 0x0308:

	break;

	default:break;
	}
}


void moni_sddf_data(unsigned int seed)
{
	unsigned int i = 0;

	stDefProtocolStatus.sddf_xitongfangdian_leiji_guoliu = 12;					//系统放电累计过流容量
	stDefProtocolStatus.sddf_xitonghuichong_leiji_guoliu = 13;					//系统回充累计过流容量
	stDefProtocolStatus.sddf_xitongchaqiang_leiji_guoliu = 8;					//系统插枪充电累计过流容量
	stDefProtocolStatus.sddf_xitong_pingjun_dianya = 32000;							//系统平均电压
	stDefProtocolStatus.sddf_xitong_pingjun_wendu = 40 + 21;							//系统平均温度
	stDefProtocolStatus.sddf_dianchi_xt_zhu_zheng_status = 1; 					//电池系统主正接触器线圈状态
	stDefProtocolStatus.sddf_dianchi_xt_zhu_fu_status = 0;						//电池系统主负接触器线圈状态
	stDefProtocolStatus.sddf_dianchi_zhanwai_chongdian_flag = 2;					//电池站外充电起止标志
	stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_xj_cnt = 1500; 				//系统小计站外插枪充电次数
	stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_lj_cnt = 3000; 				//系统累计站外插枪充电次数
	stDefProtocolStatus.sddf_dianchibao_cnt = 3;									//电池包数量(1-3)

	for (i = 0; i < 3; i++)
	{
		stDefProtocolStatus.sddf_dianchi[i].xuhao = i + 1;										//序号
		stDefProtocolStatus.sddf_dianchi[i].zhanwai_fd_nengliang = 8;						//电池包小计站外放电能量
		stDefProtocolStatus.sddf_dianchi[i].zhanwai_cd_nengliang = 7;						//电池包小计站外插枪充电能量
		stDefProtocolStatus.sddf_dianchi[i].zhanwai_hc_nengliang = 6;						//电池包小计站外回充能量
		stDefProtocolStatus.sddf_dianchi[i].milage = 10000 + i;										//电池包小计行驶里程
		stDefProtocolStatus.sddf_dianchi[i].zhuzheng_xianquan_status = 0;						//电池包主正线圈状态
		stDefProtocolStatus.sddf_dianchi[i].zhufu_xianquan_status = 1;						//电池包主负线圈状态
		stDefProtocolStatus.sddf_dianchi[i].dianliu = 3012 + i;										//电池包电流
		stDefProtocolStatus.sddf_dianchi[i].dianya = 3272 + i;										//电池包电压
		stDefProtocolStatus.sddf_dianchi[i].soc = 100 + i;											//电池包SOC
		stDefProtocolStatus.sddf_dianchi[i].soe = 300 + i;											//电池包SOE	
	}

	//mbmu
	for (i = 0; i < 1; i++)
	{
		stDefProtocolStatus.sddf_mbmu[i].bmu_xuhao = i + 1; 				//bmu序号
		memcpy(stDefProtocolStatus.sddf_mbmu[i].bmu_ver_poll_id,"\x01\x02\x03\x04",4);
		snprintf((char *)(stDefProtocolStatus.sddf_mbmu[i].bmu_ver),sizeof(stDefProtocolStatus.sddf_mbmu[i].bmu_ver),"%s","MBMU_V1.0.3");
		memcpy(stDefProtocolStatus.sddf_mbmu[i].bmu_mac,"\xEE\x63\x32\x80\x12\xAD",6);
	}

	//sbmu
	for (i = 0; i < 3; i++)
	{
		stDefProtocolStatus.sddf_sbmu[i].bmu_xuhao = i + 1; 				//bmu序号
		memcpy(stDefProtocolStatus.sddf_sbmu[i].bmu_ver_poll_id,"\x55\x66\x77\x88",4);
		snprintf((char *)(stDefProtocolStatus.sddf_sbmu[i].bmu_ver),sizeof(stDefProtocolStatus.sddf_sbmu[i].bmu_ver),"%s","SBMU_V1.0.1");
		memcpy(stDefProtocolStatus.sddf_sbmu[i].bmu_mac,"\x99\xAA\xCC\x54\x73\xCF",6);
	}

	for (i = 0; i < 3; i++)
	{
		stDefProtocolStatus.sddf_cfd_data[i].xuhao = i + 1;										// 序号
		stDefProtocolStatus.sddf_cfd_data[i].SOH = 80;											//电池包SOH
		stDefProtocolStatus.sddf_cfd_data[i].total_fangdian_cycle = 32768;						//累计放电总cycle
		stDefProtocolStatus.sddf_cfd_data[i].total_chongdian_cycle = 62027;						//累计充电总cycle
		stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_fangdian_nengliang = 2700;			//累加换电站外放电能量
		stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_huichong_nengliang = 2800;			//累加换电站外回充能量
		stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_chaqiang_nengliang = 2900;			//累加换电站外插枪充电能量
		stDefProtocolStatus.sddf_cfd_data[i].total_milage = 73720 + i;								//电池包累计行驶里程
		stDefProtocolStatus.sddf_cfd_data[i].total_fangdian_nengliang = 6000 + i;					//电池包累计放电总能量
		stDefProtocolStatus.sddf_cfd_data[i].total_chongdian_nengliang = 7000 + i;					//电池包累计充电总能量	
	}
}

//EM26 车型制冷系统数据
INT8U get_em26_fan_ls_status(void)
{
	return stDefProtocolStatus.em26_fan_ls_status;
}

void set_em26_fan_ls_status(INT8U val)
{
	stDefProtocolStatus.em26_fan_ls_status = val;
}

INT8U get_em26_fan_hs_status(void)
{
	return stDefProtocolStatus.em26_fan_hs_status;
}

void set_em26_fan_hs_status(INT8U val)
{
	stDefProtocolStatus.em26_fan_hs_status = val;
}

INT8U get_em26_air_cond_status(void)
{
	return stDefProtocolStatus.em26_air_cond_status;
}

void set_em26_air_cond_status(INT8U val)
{
	stDefProtocolStatus.em26_air_cond_status = val;
}

INT16U get_em26_air_cond_speed(void)
{
	return stDefProtocolStatus.em26_air_cond_speed;
}

void set_em26_air_cond_speed(INT16U val)
{
	stDefProtocolStatus.em26_air_cond_speed = val;
}

//EV18 车型制冷系统数据
INT8U get_ev18_fan_ls_status(void)
{
	return stDefProtocolStatus.ev18_fan_ls_status;
}

void set_ev18_fan_ls_status(INT8U val)
{
	stDefProtocolStatus.ev18_fan_ls_status = val;
}

INT8U get_ev18_fan_hs_status(void)
{
	return stDefProtocolStatus.ev18_fan_hs_status;
}

void set_ev18_fan_hs_status(INT8U val)
{
	stDefProtocolStatus.ev18_fan_hs_status = val;
}

INT16U get_ev18_air_cond_speed(void)
{

	return stDefProtocolStatus.ev18_air_cond_speed;

	
}

void set_ev18_air_cond_speed(INT16U val)
{
	if(s_ev18_cond)
	{
		stDefProtocolStatus.ev18_air_cond_speed = 0xFFFF;
	}
	else
	{
		stDefProtocolStatus.ev18_air_cond_speed = val;
	}
	
}

INT8U get_ev18_air_cond_status(void)
{
	return stDefProtocolStatus.ev18_air_cond_status;
}

void set_ev18_air_cond_status(INT8U val)
{
	if(s_ev18_cond)
	{
		stDefProtocolStatus.ev18_air_cond_status = 0xFF;
	}
	else
	{
		stDefProtocolStatus.ev18_air_cond_status = val;
	}
	
}

INT32U get_total_charge_in(void)
{
	return stDefProtocolStatus.total_charge_in;
}

void set_total_charge_in(INT32U val)
{
	stDefProtocolStatus.total_charge_in = val;
}

INT32U get_total_charge_out(void)
{
	return stDefProtocolStatus.total_charge_out;
}

void set_total_charge_out(INT32U val)
{
	stDefProtocolStatus.total_charge_out = val;
}

INT16U get_ev18_air_cond_ctrl_speed(void)
{
	return stDefProtocolStatus.ev18_air_cond_ctrl_speed;
}

void set_ev18_air_cond_ctrl_speed(INT16U val)
{
	stDefProtocolStatus.ev18_air_cond_ctrl_speed = val;
}

INT8U get_ev18_air_cond_ctrl_status(void)
{
	return stDefProtocolStatus.ev18_air_cond_ctrl_status;
}

void set_ev18_air_cond_ctrl_status(INT8U val)
{
	stDefProtocolStatus.ev18_air_cond_ctrl_status = val;
}

INT8U get_d_safe_belt_status(void)
{
	return stDefProtocolStatus.d_safe_belt_status;
}

void set_d_safe_belt_status(INT8U val)
{
	stDefProtocolStatus.d_safe_belt_status = val;
}

INT8U get_ds_front_door_status(void)
{
	return stDefProtocolStatus.ds_front_door_status;
}

void set_ds_front_door_status(INT8U val)
{
	if(val != get_ds_front_door_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.ds_front_door_status = val;
}

INT8U get_l_front_door_status(void)
{
	return stDefProtocolStatus.l_front_door_status;
}

void set_l_front_door_status(INT8U val)
{
	if(val != get_l_front_door_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.l_front_door_status = val;
}

INT8U get_ON_switch_status(void)
{
	return stDefProtocolStatus.ON_switch_status;
}

void set_ON_switch_status(INT8U val)
{
	if(val != get_ON_switch_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	
	stDefProtocolStatus.ON_switch_status = val;
}

INT8U get_power_switch_status(void)
{
	return stDefProtocolStatus.power_switch_status;
}

void set_power_switch_status(INT8U val)
{
	stDefProtocolStatus.power_switch_status = val;
}

INT8U get_key_in_status(void)
{
	return stDefProtocolStatus.key_in_status;
}

void set_key_in_status(INT8U val)
{
	stDefProtocolStatus.key_in_status = val;
}

void set_hand_brk_status(INT8U val)
{
	stDefProtocolStatus.hand_brk_status = val;
}

INT8U get_hand_brk_status(void)
{
	return stDefProtocolStatus.hand_brk_status;
}

void set_brk_status(INT8U val)
{
	stDefProtocolStatus.brk_status = val;
}

INT8U get_brk_status(void)
{
	return stDefProtocolStatus.brk_status;
}
INT8U get_ready_status(void)
{
	return stDefProtocolStatus.ready_status;
}
void set_ready_status(INT8U val)
{
	if(val != get_ready_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.ready_status = val;
	/*
	if(get_want_startup_status() == val)
	{
		set_lose_power_fail_status(0);
	}
	else
	{
		set_lose_power_fail_status(1);
	}
	*/
}
INT8U get_key_status(void)
{
	return stDefProtocolStatus.key_status;
}

void set_key_status(INT8U val)
{
	if(val != get_key_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.key_status = val;
}
//---------------------------------------------------
INT32U get_xuhang_licheng(void)
{
	return stDefProtocolStatus.xuhang_licheng;
}

void set_xuhang_licheng(INT32U val)
{
	stDefProtocolStatus.xuhang_licheng = val;
}
//---------------------------------------------------
INT8U get_xunhang_status(void)
{
	return stDefProtocolStatus.xunhang_status;
}

void set_xunhang_status(INT8U val)
{
	if(val >0x01)
	{
		val = 0xFF;
	}
	stDefProtocolStatus.xunhang_status = val;
}
//---------------------------------------------------
INT8U get_xunhang_speed(void)
{
	return stDefProtocolStatus.xunhang_speed;
}

void set_xunhang_speed(INT8U val)
{
	stDefProtocolStatus.xunhang_speed = val;
}

//---------------------------------------------------
void set_power_ctrl_allow(INT8U val)
{
	stDefProtocolStatus.power_ctrl_allow = val;
}

INT8U get_power_ctrl_allow(void)
{
	return stDefProtocolStatus.power_ctrl_allow;
}
//---------------------------------------------------

void set_steer_wheel_angle(INT16U val)
{
	stDefProtocolStatus.steer_wheel_angle = val;
}

INT16U get_steer_wheel_angle(void)
{
	return stDefProtocolStatus.steer_wheel_angle;
}

void set_zhufujiechuqi_status(INT8U val)
{
	stDefProtocolStatus.zhufujiechuqi_status = val;
}

INT8U get_zhufujiechuqi_status(void)
{
	return stDefProtocolStatus.zhufujiechuqi_status;
}

//---------------------------------------------------
//---------------------------------------------------
void set_bms_wakeup_status(INT8U val)
{
	stDefProtocolStatus.bms_wakeup_status = val;
}

INT8U get_bms_wakeup_status(void)
{
	return stDefProtocolStatus.bms_wakeup_status;
}

//---------------------------------------------------
//---------------------------------------------------
void set_vcu_wakeup_status(INT8U val)
{
	stDefProtocolStatus.vcu_wakeup_status = val;
}

INT8U get_vcu_wakeup_status(void)
{
	return stDefProtocolStatus.vcu_wakeup_status;
}

//---------------------------------------------------
//---------------------------------------------------
void set_ce_door_switch(INT8U val)
{
	stDefProtocolStatus.ce_door_switch = val;
}

INT8U get_ce_door_switch(void)
{
	return stDefProtocolStatus.ce_door_switch;
}

//---------------------------------------------------
//---------------------------------------------------
void set_back_door_switch(INT8U val)
{
	stDefProtocolStatus.back_door_switch = val;
}

INT8U get_back_door_switch(void)
{
	return stDefProtocolStatus.back_door_switch;
}

//---------------------------------------------------
//---------------------------------------------------
void set_l_f_door_switch(INT8U val)
{
	stDefProtocolStatus.l_f_door_switch = val;
}

INT8U get_l_f_door_switch(void)
{
	return stDefProtocolStatus.l_f_door_switch;
}

//---------------------------------------------------
//---------------------------------------------------
void set_r_f_door_switch(INT8U val)
{
	stDefProtocolStatus.r_f_door_switch = val;
}

INT8U get_r_f_door_switch(void)
{
	return stDefProtocolStatus.r_f_door_switch;
}

//---------------------------------------------------
//---------------------------------------------------
INT8U get_l_f_door_status(void)
{
	return stDefProtocolStatus.l_f_door_status;
}
void set_l_f_door_status(INT8U val)
{
	if(val != get_l_f_door_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.l_f_door_status = val;
	/*
	if(get_want_lockdoor_status() == val)
	{
		set_lock_door_fail_status(0);
	}
	else
	{
		set_lock_door_fail_status(1);
	}
	*/
}

//---------------------------------------------------
//---------------------------------------------------
void set_center_door_status(INT8U val)
{
	stDefProtocolStatus.center_door_status = val;
}

INT8U get_center_door_status(void)
{
	return stDefProtocolStatus.center_door_status;
}

//---------------------------------------------------
//---------------------------------------------------
INT8U get_door_lock_status(void)
{
	return stDefProtocolStatus.door_lock_status;
}

void set_door_lock_status(INT8U val)
{
	if(val != get_door_lock_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.door_lock_status = val;
	
	if(!val)
	{
		set_lose_power_fail_status(0x02);
	}
}



//---------------------------------------------------
//---------------------------------------------------
void set_cond_switch(INT8U val)
{
	stDefProtocolStatus.cond_switch = val;
}

INT8U get_cond_switch(void)
{
	return stDefProtocolStatus.cond_switch;
}

//---------------------------------------------------
//---------------------------------------------------
void set_cond_switch_status(INT8U val)
{
	stDefProtocolStatus.cond_switch_status = val;
}

INT8U get_cond_switch_status(void)
{
	return stDefProtocolStatus.cond_switch_status;
}

//---------------------------------------------------
//---------------------------------------------------
void set_cond_far_func(INT8U val)
{
	stDefProtocolStatus.cond_far_func = val;
}

INT8U get_cond_far_func(void)
{
	return stDefProtocolStatus.cond_far_func;
}

//---------------------------------------------------
//---------------------------------------------------
INT8U get_ac_quire_status(void)
{
	return stDefProtocolStatus.ac_quire_status;
}

void set_ac_quire_status(INT8U val)
{
	if(val != get_ac_quire_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.ac_quire_status = val;
}



//---------------------------------------------------
//---------------------------------------------------

INT8U get_air_work_status(void)
{
	return stDefProtocolStatus.air_work_status;
}


void set_air_work_status(INT8U val)
{
	if(val != get_air_work_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.air_work_status = val;
}

//---------------------------------------------------
//---------------------------------------------------
INT8U get_xunhuan_mode_status(void)
{
	return stDefProtocolStatus.xunhuan_mode_status;
}

void set_xunhuan_mode_status(INT8U val)
{
	if(val != get_xunhuan_mode_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.xunhuan_mode_status = val;
}



//---------------------------------------------------
//---------------------------------------------------

INT8U get_shaung_mode_status(void)
{
	return stDefProtocolStatus.shaung_mode_status;
}

void set_shaung_mode_status(INT8U val)
{
	if(val != get_shaung_mode_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.shaung_mode_status = val;
}


//---------------------------------------------------
//---------------------------------------------------

INT8U get_ptc_work_status(void)
{
	return stDefProtocolStatus.ptc_work_status;
}

void set_ptc_work_status(INT8U val)
{
	if(val != get_ptc_work_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.ptc_work_status = val;
}


//---------------------------------------------------
//---------------------------------------------------
INT8U get_feng_mode_status(void)
{
	return stDefProtocolStatus.feng_mode_status;
}
void set_feng_mode_status(INT8U val)
{
	if(val != get_feng_mode_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.feng_mode_status = val;
}



//---------------------------------------------------
//---------------------------------------------------
INT8U get_feng_dangwei_status(void)
{
	return stDefProtocolStatus.feng_dangwei_status;
}

void set_feng_dangwei_status(INT8U val)
{
	if(val != get_feng_dangwei_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.feng_dangwei_status = val;
}
//---------------------------------------------------
//---------------------------------------------------

INT8U get_wendu_dangwei_status(void)
{
	return stDefProtocolStatus.wendu_dangwei_status;
}

void set_wendu_dangwei_status(INT8U val)
{
	if(val != get_wendu_dangwei_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	stDefProtocolStatus.wendu_dangwei_status = val;
}
//---------------------------------------------------
INT8U get_lose_power_fail_status(void)
{
	return stDefProtocolStatus.lose_power_fail_status;
}


void set_lose_power_fail_status(INT8U val)
{
	stDefProtocolStatus.lose_power_fail_status = val;
}

//---------------------------------------------------
//---------------------------------------------------
void set_lock_door_fail_status(INT8U val)
{
	stDefProtocolStatus.lock_door_fail_status = val;
}

INT8U get_lock_door_fail_status(void)
{
	return stDefProtocolStatus.lock_door_fail_status;
}

//---------------------------------------------------

INT8U get_d78_car_status(void)
{
	return stDefProtocolStatus.d78_car_status;
}

void set_d78_car_status(INT8U val)
{
	stDefProtocolStatus.d78_car_status = val;
}

INT16U get_d78_soe(void)
{
	return stDefProtocolStatus.d78_soe;
}

void set_d78_soe(INT16U val)
{
	stDefProtocolStatus.d78_soe = val;
}


INT8U get_d78_soh(void)
{
	return stDefProtocolStatus.d78_soh;
}

void set_d78_soh(INT8U val)
{
	stDefProtocolStatus.d78_soh = val;
}

INT8U get_d78_dcdc_output_vol(void)
{
	return stDefProtocolStatus.d78_dcdc_output_vol;
}

void set_d78_dcdc_output_vol(INT8U val)
{
	stDefProtocolStatus.d78_dcdc_output_vol = val;
}

INT16U get_d78_ptc(void)
{
	return stDefProtocolStatus.d78_ptc;
}

void set_d78_ptc(INT16U val)
{
	stDefProtocolStatus.d78_ptc = val;
}

INT16U get_d78_cce_rpm(void)
{
	return stDefProtocolStatus.d78_cce_rpm;
}

void set_d78_cce_rpm(INT16U val)
{
	stDefProtocolStatus.d78_cce_rpm = val;
}


INT8U get_d78_bat_yuchong_status(void)
{
	return stDefProtocolStatus.d78_bat_yuchong_status;
}

void set_d78_bat_yuchong_status(INT8U val)
{
	stDefProtocolStatus.d78_bat_yuchong_status = val;
}

INT16U get_d78_bms_output_vol(void)
{
	return stDefProtocolStatus.d78_bms_output_vol;
}

void set_d78_bms_output_vol(INT16U val)
{
	stDefProtocolStatus.d78_bms_output_vol = val;
}


INT16U get_d78_mcu_input_vol(void)
{
	return stDefProtocolStatus.d78_mcu_input_vol;
}

void set_d78_mcu_input_vol(INT16U val)
{
	stDefProtocolStatus.d78_mcu_input_vol = val;
}

INT8U get_d78_dcdc_input_vol(void)
{
	return stDefProtocolStatus.d78_dcdc_input_vol;
}

void set_d78_dcdc_input_vol(INT8U val)
{
	stDefProtocolStatus.d78_dcdc_input_vol = val;
}

INT8U get_d78_mcu_gz_level(void)
{
	return stDefProtocolStatus.d78_mcu_gz_level;
}

void set_d78_mcu_gz_level(INT8U val)
{
	stDefProtocolStatus.d78_mcu_gz_level = val;
}

INT8U *get_d78_mcu_gz_code(void)
{
	return stDefProtocolStatus.d78_mcu_gz_code;
}

void set_d78_mcu_gz_code(INT8U *gzcode,unsigned int len)
{
	if (gzcode && len)
	{
		memcpy(stDefProtocolStatus.d78_mcu_gz_code,gzcode,(len > 8) ? 8 : len);
	}
}

INT8U get_d78_vcu_gz_level(void)
{
	return stDefProtocolStatus.d78_vcu_gz_level;
}

void set_d78_vcu_gz_level(INT8U val)
{
	stDefProtocolStatus.d78_vcu_gz_level = val;
}

INT8U get_d78_vcu_gz_code(void)
{
	return stDefProtocolStatus.d78_vcu_gz_code;
}

void set_d78_vcu_gz_code(INT8U val)
{
	stDefProtocolStatus.d78_vcu_gz_code = val;
}


INT8U get_d78_dcdc_gz_level(void)
{
	return stDefProtocolStatus.d78_dcdc_gz_level;
}

void set_d78_dcdc_gz_level(INT8U val)
{
	stDefProtocolStatus.d78_dcdc_gz_level = val;
}

INT8U *get_d78_dcdc_gz_code(void)
{
	return stDefProtocolStatus.d78_dcdc_gz_code;
}

void set_d78_dcdc_gz_code(INT8U *gzcode,unsigned int len)
{
	if (gzcode && len)
	{
		memcpy(stDefProtocolStatus.d78_dcdc_gz_code,gzcode,(len > 8) ? 8 : len);
	}
}


INT8U get_d78_bms_gz_level(void)
{
	return stDefProtocolStatus.d78_bms_gz_level;
}

void set_d78_bms_gz_level(INT8U val)
{
	stDefProtocolStatus.d78_bms_gz_level = val;
}

INT8U get_d78_bms_gz_code(void)
{
	return stDefProtocolStatus.d78_bms_gz_code;
}

void set_d78_bms_gz_code(INT8U val)
{
	stDefProtocolStatus.d78_bms_gz_code = val;
}


INT8U get_d78_obc_gz_level(void)
{
	return stDefProtocolStatus.d78_obc_gz_level;
}

void set_d78_obc_gz_level(INT8U val)
{
	stDefProtocolStatus.d78_obc_gz_level = val;
}

INT8U get_d78_obc_gz_code(void)
{
	return stDefProtocolStatus.d78_obc_gz_code;
}

void set_d78_obc_gz_code(INT8U val)
{
	stDefProtocolStatus.d78_obc_gz_code = val;
}

INT8U get_d78_chm_hot_level(void)
{
	return stDefProtocolStatus.d78_chm_hot_level;
}

void set_d78_chm_hot_level(INT8U val)
{
	stDefProtocolStatus.d78_chm_hot_level = val;
}

INT8U get_d78_chm_cool_level(void)
{
	return stDefProtocolStatus.d78_chm_cool_level;
}

void set_d78_chm_cool_level(INT8U val)
{
	stDefProtocolStatus.d78_chm_cool_level = val;
}

INT8U get_d78_mcu_igbt_flag(void)
{
	return stDefProtocolStatus.d78_mcu_igbt_flag;
}

void set_d78_mcu_igbt_flag(INT8U val)
{
	stDefProtocolStatus.d78_mcu_igbt_flag = val;
}

INT8U get_d78_zhenkong_gz_flag(void)
{
	return stDefProtocolStatus.d78_zhenkong_gz_flag;
}

void set_d78_zhenkong_gz_flag(INT8U val)
{
	stDefProtocolStatus.d78_zhenkong_gz_flag = val;
}

// 时代电服mbmu_sbmu数据
unsigned int get_sddf_mbmu_sbmu_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;
	unsigned int i = 0;
	unsigned int j = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x81;

	for (i = 0; i < 1; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = i + 1;

		#if 1
		for (j = 0; j < 4; j++)
		{
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = stDefProtocolStatus.sddf_mbmu[i].bmu_ver_poll_id[j];
			if (((unsigned int)rlen + (unsigned int)(21)) > outbuflen) return rlen;
			memcpy(ptr + rlen,&(stDefProtocolStatus.sddf_mbmu[i].bmu_ver[21 * j]),21);
			rlen += 21;
		}
		#else
		if (((unsigned int)rlen + (unsigned int)(4)) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_mbmu[i].bmu_ver_poll_id,4);
		rlen += 4;

		if (((unsigned int)rlen + (unsigned int)((21*4))) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_mbmu[i].bmu_ver,21 * 4);
		rlen += (21*4);
		#endif

		if (((unsigned int)rlen + (unsigned int)(6)) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_mbmu[i].bmu_mac,6);
		rlen += 6;
	}

	templen = stDefProtocolStatus.sddf_dianchibao_cnt;
	if (templen >= MAX_SDDF_DIANCHIBAO_CNT)
	{
		templen = MAX_SDDF_DIANCHIBAO_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	for (i = 0; i < stDefProtocolStatus.sddf_dianchibao_cnt; i++)
	{
		if (i >= MAX_SDDF_DIANCHIBAO_CNT)
		{
			break;
		}

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = i + 1;

		#if 1
		for (j = 0; j < 4; j++)
		{
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = stDefProtocolStatus.sddf_sbmu[i].bmu_ver_poll_id[j];
			if (((unsigned int)rlen + (unsigned int)(21)) > outbuflen) return rlen;
			memcpy(ptr + rlen,&(stDefProtocolStatus.sddf_sbmu[i].bmu_ver[21 * j]),21);
			rlen += 21;
		}
		#else
		if (((unsigned int)rlen + (unsigned int)(4)) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_sbmu[i].bmu_ver_poll_id,4);
		rlen += 4;

		if (((unsigned int)rlen + (unsigned int)((21*4))) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_sbmu[i].bmu_ver,21 * 4);
		rlen += (21*4);
		#endif

		if (((unsigned int)rlen + (unsigned int)(6)) > outbuflen) return rlen;
		memcpy(ptr + rlen,stDefProtocolStatus.sddf_sbmu[i].bmu_mac,6);
		rlen += 6;
	}

	return rlen;
}

// 时代电服充放电数据
unsigned int get_sddf_chongfangdian_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;
	unsigned int i = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x82;

	templen = stDefProtocolStatus.sddf_dianchibao_cnt;
	if (templen >= MAX_SDDF_DIANCHIBAO_CNT)
	{
		templen = MAX_SDDF_DIANCHIBAO_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	for (i = 0; i < stDefProtocolStatus.sddf_dianchibao_cnt; i++)
	{
		if (i >= MAX_SDDF_DIANCHIBAO_CNT)
		{
			break;
		}

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = i + 1;


		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_cfd_data[i].SOH;


		templen = stDefProtocolStatus.sddf_cfd_data[i].total_fangdian_cycle;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_cfd_data[i].total_chongdian_cycle;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;
	

		templen = stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_fangdian_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_huichong_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_cfd_data[i].total_zhanwai_chaqiang_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		//累计里程
		templen = stDefProtocolStatus.sddf_cfd_data[i].total_milage;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		//累计放电能量
		templen = stDefProtocolStatus.sddf_cfd_data[i].total_fangdian_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;
		
		templen = stDefProtocolStatus.sddf_cfd_data[i].total_chongdian_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;
	}

	return rlen;
}


// 时代电服充电信息
unsigned int get_sddf_charge_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;
	unsigned int i = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x83;
	templen = stDefProtocolStatus.sddf_dianchibao_cnt;
	if (templen >= MAX_SDDF_DIANCHIBAO_CNT)
	{
		templen = MAX_SDDF_DIANCHIBAO_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	for (i = 0; i < templen; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = i + 1;

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_benci_chongdian_rongliang[i] >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_benci_chongdian_rongliang[i];
	}

	return rlen;
}

// 时代电服ACP数据
unsigned int get_sddf_acp_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;
	unsigned int i = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x80;

	templen = stDefProtocolStatus.sddf_xitongfangdian_leiji_guoliu;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	templen = stDefProtocolStatus.sddf_xitonghuichong_leiji_guoliu;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	templen = stDefProtocolStatus.sddf_xitongchaqiang_leiji_guoliu;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	templen = stDefProtocolStatus.sddf_xitong_pingjun_dianya;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = stDefProtocolStatus.sddf_xitong_pingjun_wendu;

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = stDefProtocolStatus.sddf_dianchi_xt_zhu_zheng_status;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = stDefProtocolStatus.sddf_dianchi_xt_zhu_fu_status;

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = stDefProtocolStatus.sddf_dianchi_zhanwai_chongdian_flag;

	templen = stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_xj_cnt;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	templen = stDefProtocolStatus.sddf_xitong_zhanwai_chaqiang_lj_cnt;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	templen = stDefProtocolStatus.sddf_dianchibao_cnt;
	if (templen >= MAX_SDDF_DIANCHIBAO_CNT)
	{
		templen = MAX_SDDF_DIANCHIBAO_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//电池包信息
	for (i = 0; i < stDefProtocolStatus.sddf_dianchibao_cnt; i++)
	{
		if (i >= MAX_SDDF_DIANCHIBAO_CNT)
		{
			break;
		}

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = i+ 1;//stDefProtocolStatus.sddf_dianchi[i].xuhao;

		templen = stDefProtocolStatus.sddf_dianchi[i].zhanwai_fd_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_dianchi[i].zhanwai_cd_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_dianchi[i].zhanwai_hc_nengliang;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_dianchi[i].milage;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_dianchi[i].zhuzheng_xianquan_status;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_dianchi[i].zhufu_xianquan_status;

		templen = stDefProtocolStatus.sddf_dianchi[i].dianliu;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;

		templen = stDefProtocolStatus.sddf_dianchi[i].dianya;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;
  
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stDefProtocolStatus.sddf_dianchi[i].soc;

		templen = stDefProtocolStatus.sddf_dianchi[i].soe;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = templen;
	}
	return rlen;
}

// D78自定义数据
unsigned int get_d78_zdy_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	//整车关键数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x81;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_car_status();
	templen = get_d78_soe();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_soh();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_dcdc_output_vol();
	templen = get_d78_ptc();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	templen = get_d78_cce_rpm();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//高压系统关键数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x82;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_bat_yuchong_status();
	templen = get_d78_bms_output_vol();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	templen = get_d78_mcu_input_vol();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_dcdc_input_vol();

	//电子牵引力故障关键数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x83;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_mcu_gz_level();
	if (((unsigned int)rlen + (unsigned int)(8)) > outbuflen) return rlen;
	memcpy(ptr + rlen,get_d78_mcu_gz_code(),8);
	rlen += 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_vcu_gz_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_vcu_gz_code();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_dcdc_gz_level();
	if (((unsigned int)rlen + (unsigned int)(8)) > outbuflen) return rlen;
	memcpy(ptr + rlen,get_d78_dcdc_gz_code(),8);
	rlen += 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_bms_gz_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_bms_gz_code();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_obc_gz_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_obc_gz_code();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_chm_hot_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_chm_cool_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_mcu_igbt_flag();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_d78_zhenkong_gz_flag();

	return rlen;
}


// EM26自定义数据 //if (get_engine_type() == EngineType_EM26)
unsigned int get_em26_zdy_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	//制冷系统数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x85;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_em26_fan_ls_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_em26_fan_hs_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_em26_air_cond_status();
	templen = get_em26_air_cond_speed();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x88;
	templen = get_total_charge_in();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	
	templen = get_total_charge_out();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	return rlen;
}
INT8U gb32960_can_get_dangwei(void);
INT8U gb32960_can_get_jiasutaban_val(void);
INT8U gb32960_can_get_zhidongtaban_status(void);
INT32U gb32960_get_sys_run_time(void);
// EV18自定义数据if (get_engine_type() == EngineType_EV18)
unsigned int get_ev18_zdy_data(void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int templen = 0;

	if ((ptr == 0) || (outbuflen < 3))
	{
		return 0;
	}

	//制冷系统数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x86;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ev18_fan_ls_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ev18_fan_hs_status();
	
	templen = get_ev18_air_cond_speed();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ev18_air_cond_status();
	
	templen = get_ev18_air_cond_ctrl_speed();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ev18_air_cond_ctrl_status();
	
	//
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x87;
	templen = get_total_charge_in();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	
	templen = get_total_charge_out();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	
#if 1
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x89;
	//GPS 速度
	templen = 0xFFFF;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
	
	//GPS方向
	templen = 0xFF;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//海拔 
	templen = 0xFFFF;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//钥匙档位
	templen = get_key_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//档位
	templen = gb32960_can_get_dangwei();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//加速踏板开度
	templen = gb32960_can_get_jiasutaban_val();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//手刹开关 
	templen = get_hand_brk_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//制动踏板开度
	templen = gb32960_can_get_zhidongtaban_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//累计回馈电量
	templen = 0xFFFFFFFF;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//充电时状态
	if(gb32960_can_get_charge_status() == 3)
	{
		templen = 0;
	}
	else if((gb32960_can_get_charge_status() == 1)||(gb32960_can_get_charge_status() == 2))
	{
		templen = 1;
	}
	else if(gb32960_can_get_charge_status() == 4)
	{
		templen = 4;
	}
	else
	{
		templen = 0xFF;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//续航里程
	templen = get_xuhang_licheng();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//总运行时长
	templen = gb32960_get_sys_run_time();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//方向盘角度信号
	templen = get_steer_wheel_angle();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//定速巡航
	templen = get_xunhang_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;

	//巡航速度
	templen = get_xunhang_speed();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = templen;
#endif
	
	return rlen;
}

//是否获取到里程ID
INT8U is_get_milage_id(void)
{
	return (s_the_id_table & 0x01);
}

INT8U get_d78_huandian_ver(void)
{
	return s_d78_huandian_ver;
}

void set_d78_huandian_ver(INT8U val)
{
	s_d78_huandian_ver = val;
}

INT8U get_dst_want_bms_charge_flag(void)
{
	return stDefProtocolStatus.dst_want_bms_charge_flag;
}

void set_dst_want_bms_charge_flag(INT8U val)
{
	stDefProtocolStatus.dst_want_bms_charge_flag = val;
}


INT8U get_dst_fact_bms_charge_flag(void)
{
	return stDefProtocolStatus.dst_fact_bms_charge_flag;
}

void set_dst_fact_bms_charge_flag(INT8U val)
{
	stDefProtocolStatus.dst_fact_bms_charge_flag = val;
}

void set_dst_budian_init_vol(unsigned int val)
{
	stDefProtocolStatus.budian_init_vol = val;
}

unsigned int get_dst_budian_init_vol(void)
{
	return stDefProtocolStatus.budian_init_vol;
}


INT8U *get_iccid_bytes_in_ecu(void)
{
	return stDefProtocolStatus.iccid_bytes_in_ecu;
}

void set_iccid_bytes_in_ecu(INT8U *piccid_bytes,unsigned int datlen)
{
	if (piccid_bytes && datlen && (datlen <= 8))
	{
		memcpy(stDefProtocolStatus.iccid_bytes_in_ecu,piccid_bytes,datlen);
	}
}

INT8U get_iccid_head_flag_in_ecu(void)
{
	return stDefProtocolStatus.iccid_head_flag_in_ecu;
}

void set_iccid_head_flag_in_ecu(INT8U val)
{
	stDefProtocolStatus.iccid_head_flag_in_ecu = val;
}


INT8U *get_iccid_head_bytes_in_ecu(void)
{
	return stDefProtocolStatus.iccid_head_bytes_in_ecu;
}

void set_iccid_head_bytes_in_ecu(INT8U *piccid_head_bytes,unsigned int datlen)
{
	if (piccid_head_bytes && datlen && (datlen <= 4))
	{
		memcpy(stDefProtocolStatus.iccid_head_bytes_in_ecu,piccid_head_bytes,datlen);
		set_iccid_head_flag_in_ecu(1);
	}
}

INT8U get_lock_car_can_channel(void)
{
	return stDefProtocolStatus.lock_car_can_channel;
}

void set_lock_car_can_channel(INT8U val)
{
	stDefProtocolStatus.lock_car_can_channel = val;
}

INT8U get_want_bind_status(void)
{
	//return stDefProtocolStatus.want_bind_status;
	return stSaveParam.want_bind_status;
}

INT8U get_fact_bind_status(void)
{
	return stDefProtocolStatus.fact_bind_status;
}

INT8U get_fact_pro_ver_about_lock(void)
{
	return stDefProtocolStatus.fact_pro_ver_about_lock;
}

INT8U get_fact_lock_status(void)
{
	return stDefProtocolStatus.fact_lock_status;
}

void set_want_bind_status(INT8U val)
{
	//stDefProtocolStatus.want_bind_status = val;
	stSaveParam.want_bind_status = val;
	if(val == 1)
	{
		set_send_18X4D0D5_times(20);
	}
}

void set_fact_bind_status(INT8U val)
{
	stDefProtocolStatus.fact_bind_status = val;
}

void set_fact_pro_ver_about_lock(INT8U val)
{
	stDefProtocolStatus.fact_pro_ver_about_lock = val;
}

void set_fact_lock_status(INT8U val)
{
	stDefProtocolStatus.fact_lock_status = val;
}

void set_want_startup_status(INT8U val)
{
	stDefProtocolStatus.want_startup_status = val;
}

INT8U get_want_startup_status(void)
{
	return stDefProtocolStatus.want_startup_status;
}

void set_lockdoor_commond_flag(INT8U val)
{
	stDefProtocolStatus.lockdoor_commond_flag = val;
}

INT8U get_lockdoor_commond_flag(void)
{
	return stDefProtocolStatus.lockdoor_commond_flag;
}

void set_want_lockdoor_status(INT8U val)
{
	stDefProtocolStatus.want_lockdoor_status = val;
}

INT8U get_want_lockdoor_status(void)
{
	return stDefProtocolStatus.want_lockdoor_status;
}

void set_want_control_air_cond(INT8U val)
{
	stDefProtocolStatus.want_control_air_cond = val;
}

INT8U get_want_control_air_cond(void)
{
	return stDefProtocolStatus.want_control_air_cond;
}

void set_fact_startup_status(INT8U val)
{
	stDefProtocolStatus.fact_startup_status = val;
}

INT8U get_fact_startup_status(void)
{
	return stDefProtocolStatus.fact_startup_status;
}

void set_fact_lockdoor_status(INT8U val)
{
	stDefProtocolStatus.fact_lockdoor_status = val;
}

INT8U get_fact_lockdoor_status(void)
{
	return stDefProtocolStatus.fact_lockdoor_status;
}

void set_fact_control_air_cond(INT8U val)
{
	stDefProtocolStatus.fact_control_air_cond = val;
}

INT8U get_fact_control_air_cond(void)
{
	return stDefProtocolStatus.fact_control_air_cond;
}

typedef struct tagDCWD
{
	INT8U id;
	INT16U benzhen_danti_cnt;		//本帧单体电池总数
	INT8U wendu[WENDU_TANZHEN_CNT_EVERY_DIANCHI];
}DCWD;


//可充电储能系统
typedef struct tagCNXTData
{
	INT8U id;				//可充电储能系统ID
	INT16U dianya;
	INT16U dianliu;

	INT16U benzhen_start_id;				//起始序号
	INT8U benzhen_danti_cnt;		//本帧单体电池总数
	INT16U danti_dianchi_dianya[DANTI_DIANCHI_CNT_EVERY_DIANCHI];		//单体电池电压

	INT16U wendu_tanzhen_cnt;			//温度探针个数
	INT8U wendu[WENDU_TANZHEN_CNT_EVERY_DIANCHI];
}CNXTData;



//驱动电机数据
typedef struct tagQDDJData
{
	INT8U driver_id;		//电机序号
	INT8U status;			//驱动电机状态 01耗电 02发电 03关闭 04准备状态
	INT8U controller_wendu;		//驱动电机控制器温度
	INT16U rpm;			//转速
	INT16U zhuanju;			//转矩
	INT8U dianji_wendu;		//电机温度
	INT16U input_vol;		//电机控制器输入电压
	INT16U controller_dianliu;		//电机控制器直流母线电流
}QDDJData;

typedef struct tagGB32960Param
{
	//整车数据
	INT8U car_status;	//车辆状态01:启动 02:熄火  03:其他状态
	INT8U charge_status;	//充电状态 01停车充电 02行驶充电 03未充电 04充电完成
	INT8U run_mode;			//1.纯电 2.混动 3.燃油
	INT16U speed;
	INT32U milage;
	INT16U total_vol;			//总电压
	INT16U total_dianliu;		//总电流
	INT8U soc;
	INT8U dc_dc_status;			//01工作 02断开
	INT8U dangwei;		//挡位信息
						//bit7-bit6  预留
						//bit5
						//1:有驱动力
						//0:无驱动力
						//bit4
						//1:有制动力
						//0:无制动力
						//bit3-bit0
						//0000  空挡
						//0001  1挡
						//0010  2挡
						//0011  3挡
						//0100  4挡
						//0101  5挡
						//0110  6挡
						//...
						//1101  倒挡
						//1110  自动D挡
						//1111  停车P挡
	INT16U jueyuandianzu;		//绝缘电阻
	INT8U jiasutaban_val;		//加速踏板行程值
	INT8U zhidongtaban_status;	//制动踏板状态

	//电机数据
	INT8U qddj_cnt;		//驱动电机个数
	QDDJData qddjdata[MAX_DJ_CNT];

	//燃料电池数据
	INT16U rldc_dianya;				//燃料电池电压
	INT16U rldc_dianliu;				//燃料电池电流
	INT16U rldc_rlxiaohaolv;				//燃料消耗率
	INT16U rldc_tanzhen_cnt;				//探针总数
	INT8U rldc_tanzhen_wendu[MAX_TAN_ZHEN_CNT];
	INT16U max_wendu_in_qi_system;	//氢系统中最高温度
	INT8U max_wendu_tanzhen_id_in_qi_system;		//最高温度的探针ID
	INT16U max_qingqi_nongdu;		//氢气最高浓度
	INT8U max_qingqi_sensor_id;
	INT16U max_qingqi_yali;
	INT8U max_qingqi_yali_sensor_id;
	INT8U gaoya_dc_dc_status;

	//发动机数据
	INT8U ecu_status;
	INT16U quzhou_rpm;
	INT16U ranliao_xiaohaolv;

	//位置数据
	INT8U pos_status;
	INT32U lon;
	INT32U lat;

	//极值信息
	INT8U max_vol_id;
	INT8U max_dianya_danti_id;
	INT16U max_vol_val;
	INT8U low_vol_id;
	INT8U low_danti_id;
	INT16U low_vol_val;
	INT8U max_wend_system_id;
	INT8U max_wend_tanzhen_id;
	INT8U max_wendu_val;
	INT8U low_wendu_system_id;
	INT8U low_wendu_tanzhen_id;
	INT8U low_wendu_val;
	//报警信息
	INT8U max_sos_level;		//最高报警等级
	INT32U sos_flag;			//通用报警标志
	INT8U can_charge_set_dtc_cnt;	//可充电装置故障总数	
	INT32U charge_dtc_buf[MAX_CAN_CHARGE_DTC_CNT];

	INT8U qddj_dtc_cnt;			//驱动电机故障总数
	INT32U qddj_dtc_buf[MAX_QDDJ_DTC_CNT];

	INT8U ecu_dtc_cnt;	//发动机故障总数
	INT32U ecu_dtc_buf[MAX_ECU_DTC_CNT];

	INT8U other_dtc_cnt;	//其他故障总数
	INT32U other_dtc_buf[MAX_OTHER_DTC_CNT];

	//可充电储能系统
	INT8U dianchi_cnt;		//可充电储能系统个数
	CNXTData stCNXTData[MAX_DIANCHI_CNT];
}GB32960Param;

typedef struct tagGB32960Time
{
	//整车数据
	INT32U car_status_time;	//车辆状态01:启动 02:熄火  03:其他状态
	INT32U charge_status_time;	//充电状态 01停车充电 02行驶充电 03未充电 04充电完成
	INT32U run_mode_time;			//1.纯电 2.混动 3.燃油
	INT32U speed_time;
	INT32U milage_time;
	INT32U total_vol_time;			//总电压
	INT32U total_dianliu_time;		//总电流
	INT32U soc_time;
	INT32U dc_dc_status_time;			//01工作 02断开
	INT32U dangwei_time;		//挡位信息
						//bit7-bit6  预留
						//bit5
						//1:有驱动力
						//0:无驱动力
						//bit4
						//1:有制动力
						//0:无制动力
						//bit3-bit0
						//0000  空挡
						//0001  1挡
						//0010  2挡
						//0011  3挡
						//0100  4挡
						//0101  5挡
						//0110  6挡
						//...
						//1101  倒挡
						//1110  自动D挡
						//1111  停车P挡
	INT32U jueyuandianzu_time;		//绝缘电阻
	INT32U jiasutaban_val_time;		//加速踏板行程值
	INT32U zhidongtaban_status_time;	//制动踏板状态

	//电机数据
	INT32U qddjdata_time;

	//燃料电池数据
	INT32U rldc_dianya_time;				//燃料电池电压
	INT32U rldc_dianliu_time;				//燃料电池电流
	INT32U rldc_rlxiaohaolv_time;				//燃料消耗率

	INT32U rldc_tanzhen_wendu_time;
	INT32U max_wendu_in_qi_system_time;	//氢系统中最高温度
	INT32U max_wendu_tanzhen_id_in_qi_system_time;		//最高温度的探针ID
	INT32U max_qingqi_nongdu_time;		//氢气最高浓度
	INT32U max_qingqi_sensor_id_time;
	INT32U max_qingqi_yali_time;
	INT32U max_qingqi_yali_sensor_id_time;
	INT32U gaoya_dc_dc_status_time;

	//发动机数据
	INT32U ecu_status_time;
	INT32U quzhou_rpm_time;
	INT32U ranliao_xiaohaolv_time;

	//报警信息
	INT32U can_charge_set_dtc_time;	//可充电装置故障总数
	INT32U qddj_dtc_time; 		//驱动电机故障总数
	INT32U ecu_dtc_time;	//发动机故障总数
	INT32U other_dtc_time;	//其他故障总数

	//可充电储能系统
	INT32U stCNXTData_time;
}GB32960Time;

INT8U gb32960_get_moni_enter_mq_flag(void)
{
	return s_mq_flag;
}

void gb32960_set_moni_enter_mq_flag(INT8U val)
{
	s_mq_flag = val;
}


//采集时间间隔
INT16U gb32960_get_caiji_time_inteval(void)
{
	return s_inteval;
}

void gb32960_set_caiji_time_inteval(INT16U val)
{
	s_inteval = val;
}

INT8U gb32960_can_get_sos_test(void)
{
	return sos_test_flag;
}

void gb32960_can_set_sos_test(INT8U val)
{
	sos_test_flag = val;
}


GB32960Param stGB32960Param = {0};
GB32960Time stGB32960Time = {0};

INT32U gb32960_get_sys_run_time(void)
{
	return GetRunTime();
}

INT8U gb32960_can_get_car_status(void)
{
	return stGB32960Param.car_status;
}


//设置车辆状态 01驱动 02熄火
void gb32960_can_set_car_status(INT8U status)
{
	if(status != gb32960_can_get_car_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	
	if ((status == 0x01) || (status == 0x02) || (status == 0x03) || (status == 0xFE) || (status == 0xFF))
	{
		stGB32960Param.car_status = status;
		stGB32960Time.car_status_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_charge_status(void)
{
	return stGB32960Param.charge_status;
}



void gb32960_can_set_charge_status(INT8U status)
{
	if(status != gb32960_can_get_charge_status())
	{
		set_s_32960_zuocang_data_change(1);
	}
	
	if ((status == 0x01) || (status == 0x02) || (status == 0x03) || (status == 0x04)
		|| (status == 0xFE) || (status == 0xFF))
	{
		stGB32960Param.charge_status = status;
		stGB32960Time.charge_status_time = gb32960_get_sys_run_time();
	}
}



void gb32960_can_set_run_mode(INT8U val)
{
	if ((val == 1) || (val == 2) || (val == 3) || (val == 0xFE) || (val == 0xFF))
	{
		stGB32960Param.run_mode = val;
		stGB32960Time.run_mode_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_run_mode(void)
{
	return stGB32960Param.run_mode;
}

void gb32960_can_set_speed(INT16U val)
{
	if ((val == 0xFFFF) || (val <= 2200))
	{
		stGB32960Param.speed = val;
		stGB32960Time.speed_time = gb32960_get_sys_run_time();
		
		if(val > 0)
		{
			set_lose_power_fail_status(0x01);			//下电失败状态 
			set_lock_door_fail_status(0x01);			//上锁失败状态
		}
	}
}

INT16U gb32960_can_get_speed(void)
{
	return stGB32960Param.speed;
}

void gb32960_can_set_milage(INT32U val)
{
	if ((val == 0xFFFFFFFE) || (val == 0xFFFFFFFF) || (val <= 9999999))
	{
		stGB32960Time.milage_time = gb32960_get_sys_run_time();
		stGB32960Param.milage = val;
		s_the_id_table |= (1 << 0);
	}
}

INT32U gb32960_can_get_milage(void)
{
	return stGB32960Param.milage;
}

void gb32960_can_set_total_dianya(INT16U val)
{
	if ((val <= 10000) || (val == 0xFFFF) || (val == 0xFFFE))
	{
		stGB32960Param.total_vol = val;
		stGB32960Time.total_vol_time = gb32960_get_sys_run_time();
	}
}

INT16U gb32960_can_get_total_dianya(void)
{
	//启动状态 返回stGB32960Param.total_vol 其他状态返回电池电压
	//if (gb32960_can_get_car_status() == 0x01)
	{
		return stGB32960Param.total_vol;
	}
	//return gb32960_can_get_cnxt_dianya(0);
}

void gb32960_can_set_total_dianliu(INT16U val)
{
	if ((val <= 20000) || (val == 0xFFFF) || (val == 0xFFFE))
	{
		stGB32960Param.total_dianliu = val;
		stGB32960Time.total_dianliu_time = gb32960_get_sys_run_time();
	}
}

INT16U gb32960_can_get_total_dianliu(void)
{
	//启动状态 返回stGB32960Param.total_dianliu 其他状态返回电池电流
	//if (gb32960_can_get_car_status() == 0x01)
	{
		return stGB32960Param.total_dianliu;
	}
	//return gb32960_can_get_cnxt_dianliu(0);
}


void gb32960_can_set_soc(INT8U status)
{
	if ((status <= 100) || (status == 0xFF) || (status == 0xFE))
	{
		stGB32960Param.soc = status;
		stGB32960Time.soc_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_soc(void)
{
	return stGB32960Param.soc;
}


void gb32960_can_set_dc_dc_status(INT8U status)
{
	if ((status == 0x01) || (status == 0x02) || (status == 0xFE) || (status == 0xFF))
	{
		stGB32960Param.dc_dc_status = status;
		stGB32960Time.dc_dc_status_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_dc_dc_status(void)
{
	return stGB32960Param.dc_dc_status;
}

void gb32960_can_set_dangwei(INT8U status,INT8U qdl,INT8U zdl)
{
	if ((status <= 6) || (status == 0x0F) || (status == 0x0E) || (status == 0x0D) || (status == 0xFF))
	{
		stGB32960Param.dangwei = status | (qdl << 5) | (zdl << 4);
		stGB32960Time.dangwei_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_dangwei(void)
{
	return stGB32960Param.dangwei;
}


void gb32960_can_set_jueyuandianzu(INT16U val)
{
	if ((val <= 60000) || (val == 0xFFFF) || (val == 0xFFFE))
	{
		stGB32960Param.jueyuandianzu = val;
		stGB32960Time.jueyuandianzu_time = gb32960_get_sys_run_time();
	}
}

INT16U gb32960_can_get_jueyuandianzu(void)
{
	return stGB32960Param.jueyuandianzu;
}



void gb32960_can_set_jiasutaban_val(INT8U status)
{
	stGB32960Param.jiasutaban_val = status;
	stGB32960Time.jiasutaban_val_time = gb32960_get_sys_run_time();
}

INT8U gb32960_can_get_jiasutaban_val(void)
{
	return stGB32960Param.jiasutaban_val;
}

void gb32960_can_set_zhidongtaban_status(INT8U status)
{
	stGB32960Param.zhidongtaban_status = status;
	stGB32960Time.zhidongtaban_status_time = gb32960_get_sys_run_time();
}

INT8U gb32960_can_get_zhidongtaban_status(void)
{
	return stGB32960Param.zhidongtaban_status;
}

/********极值相关参数*******/
void gb32960_can_set_max_dianya_sub_system_id(INT8U val)
{
	if (((val >= 1) && (val <= 250)) || (val == 0xFF))
	{
		stGB32960Param.max_vol_id = val;
	}
}

INT8U gb32960_can_get_max_dianya_sub_system_id(void)
{
	return stGB32960Param.max_vol_id;
}


void gb32960_can_set_max_dianya_danti_id(INT8U val)
{
	if (((val >= 1) && (val <= 250)) || (val == 0xFF))
	{
		stGB32960Param.max_dianya_danti_id = val;
	}
}

INT8U gb32960_can_get_max_dianya_danti_id(void)
{
	return stGB32960Param.max_dianya_danti_id;
}

void gb32960_can_set_max_danti_dianya_val(INT16U val)
{
	if ((val <= 15000) || (val == 0xFFFF))
	{
		stGB32960Param.max_vol_val = val;
	}
}

INT16U gb32960_can_get_max_danti_dianya_val(void)
{
	return stGB32960Param.max_vol_val;
}


void gb32960_can_set_min_dianya_sub_system_id(INT8U val)
{
	stGB32960Param.low_vol_id = val;
}

INT8U gb32960_can_get_min_dianya_sub_system_id(void)
{
	return stGB32960Param.low_vol_id;
}

void gb32960_can_set_min_dianya_danti_id(INT8U val)
{
	if (((val >= 1) && (val <= 250)) || (val == 0xFF))
	{
		stGB32960Param.low_danti_id = val;
	}
}

INT8U gb32960_can_get_min_dianya_danti_id(void)
{
	return stGB32960Param.low_danti_id;
}

void gb32960_can_set_min_danti_dianya_val(INT16U val)
{
	stGB32960Param.low_vol_val = val;
}

INT16U gb32960_can_get_min_danti_dianya_val(void)
{
	return stGB32960Param.low_vol_val;
}

void gb32960_can_set_max_wendu_sub_system_id(INT8U val)
{
	stGB32960Param.max_wend_system_id = val;
}

INT8U gb32960_can_get_max_wendu_sub_system_id(void)
{
	return stGB32960Param.max_wend_system_id;
}



void gb32960_can_set_max_wendu_tanzhen_id(INT8U val)
{
	stGB32960Param.max_wend_tanzhen_id = val;
}

INT8U gb32960_can_get_max_wendu_tanzhen_id(void)
{
	return stGB32960Param.max_wend_tanzhen_id;
}

void gb32960_can_set_max_wendu_val(INT8U val)
{
	stGB32960Param.max_wendu_val = val;
}

INT8U gb32960_can_get_max_wendu_val(void)
{
	return stGB32960Param.max_wendu_val;
}

void gb32960_can_set_min_wendu_sub_system_id(INT8U val)
{
	stGB32960Param.low_wendu_system_id = val;
}

INT8U gb32960_can_get_min_wendu_sub_system_id(void)
{
	return stGB32960Param.low_wendu_system_id;
}

void gb32960_can_set_min_wendu_tanzhen_id(INT8U val)
{
	stGB32960Param.low_wendu_tanzhen_id = val;
}

INT8U gb32960_can_get_min_wendu_tanzhen_id(void)
{
	return stGB32960Param.low_wendu_tanzhen_id;
}

void gb32960_can_set_min_wendu_val(INT8U val)
{
	stGB32960Param.low_wendu_val = val;
}

INT8U gb32960_can_get_min_wendu_val(void)
{
	return stGB32960Param.low_wendu_val;
}


void gb32960_can_set_ecu_status(INT8U val)
{
	stGB32960Param.ecu_status = val;
}

INT8U gb32960_can_get_ecu_status(void)
{
	return stGB32960Param.ecu_status;
}


void gb32960_can_set_ecu_quzhou_rpm(INT16U val)
{
	stGB32960Param.quzhou_rpm = val;
}

INT16U gb32960_can_get_ecu_quzhou_rpm(void)
{
	return stGB32960Param.quzhou_rpm;
}


void gb32960_can_set_ecu_ranliao_xiaohaolv(INT16U val)
{
	stGB32960Param.ranliao_xiaohaolv = val;
}

INT16U gb32960_can_get_ecu_ranliao_xiaohaolv(void)
{
	return stGB32960Param.ranliao_xiaohaolv;
}


void gb32960_can_set_qddj_cnt(INT8U val)
{
	stGB32960Param.qddj_cnt = val;
}

INT8U gb32960_can_get_qddj_cnt(void)
{
	return stGB32960Param.qddj_cnt;
}

void gb32960_can_set_qddj_xuhao(INT8U idx,INT8U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}
	if (val == 0) return ;
	stGB32960Param.qddjdata[idx].driver_id = val;
}

INT8U gb32960_can_get_qddj_xuhao(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 1;
	}

	return stGB32960Param.qddjdata[idx].driver_id;
}


void gb32960_can_set_qddj_status(INT8U idx,INT8U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	if ((val == 1) || (val == 2) || (val == 3) || (val == 4) || (val == 0xFE) || (val == 0xFF))
	{
		stGB32960Param.qddjdata[idx].status = val;
		stGB32960Time.qddjdata_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_qddj_status(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 3;
	}

	return stGB32960Param.qddjdata[idx].status;
}

void gb32960_can_set_qddj_controller_wendu(INT8U idx,INT8U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	if ((val <= 250) || (val == 0xFF))
	{
		stGB32960Param.qddjdata[idx].controller_wendu = val;
		stGB32960Time.qddjdata_time = gb32960_get_sys_run_time();
	}
}

INT8U gb32960_can_get_qddj_controller_wendu(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].controller_wendu;
}


void gb32960_can_set_qddj_rpm(INT8U idx,INT16U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	stGB32960Param.qddjdata[idx].rpm = val;
}

INT16U gb32960_can_get_qddj_rpm(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].rpm;
}

void gb32960_can_set_qddj_zhuanju(INT8U idx,INT16U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	stGB32960Param.qddjdata[idx].zhuanju = val;
}

INT16U gb32960_can_get_qddj_zhuanju(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].zhuanju;
}

void gb32960_can_set_qddj_wendu(INT8U idx,INT8U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	if ((val <= 250) || (val == 0xFF))
	{
		stGB32960Param.qddjdata[idx].dianji_wendu = val;
	}
}

INT8U gb32960_can_get_qddj_wendu(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].dianji_wendu;
}

void gb32960_can_set_qddj_input_dianya(INT8U idx,INT16U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	if ((val <= 60000) || (val == 0xFFFF) || (val == 0xFFFE))
	{
		stGB32960Param.qddjdata[idx].input_vol = val;
	}
}

INT16U gb32960_can_get_qddj_input_dianya(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].input_vol;
}

void gb32960_can_set_qddj_controller_dianliu(INT8U idx,INT16U val)
{
	if (idx >= MAX_DJ_CNT)
	{
		return ;
	}

	if ((val <= 20000) || (val == 0xFFFF) || (val == 0xFFFE))
	{
		stGB32960Param.qddjdata[idx].controller_dianliu = val;
	}
}

INT16U gb32960_can_get_qddj_controller_dianliu(INT8U idx)
{
	if (idx >= MAX_DJ_CNT)
	{
		return 0;
	}

	return stGB32960Param.qddjdata[idx].controller_dianliu;
}


void gb32960_can_set_rldc_dianya(INT16U val)
{
	if (val > 20000)
	{
		return ;
	}
	stGB32960Param.rldc_dianya = val;
}

INT16U gb32960_can_get_rldc_dianya(void)
{
	return stGB32960Param.rldc_dianya;
}

void gb32960_can_set_rldc_dianliu(INT16U val)
{
	if (val > 20000)
	{
		return ;
	}
	stGB32960Param.rldc_dianliu = val;
}

INT16U gb32960_can_get_rldc_dianliu(void)
{
	return stGB32960Param.rldc_dianliu;
}


void gb32960_can_set_rldc_xiaohaolv(INT16U val)
{
	stGB32960Param.rldc_rlxiaohaolv = val;
}

INT16U gb32960_can_get_rldc_xiaohaolv(void)
{
	return stGB32960Param.rldc_rlxiaohaolv;
}


void gb32960_can_set_rldc_tanzhen_cnt(INT16U val)
{
	stGB32960Param.rldc_tanzhen_cnt = val;
}

INT16U gb32960_can_get_rldc_tanzhen_cnt(void)
{
	return stGB32960Param.rldc_tanzhen_cnt;
}


void gb32960_can_set_rldc_tanzhen_wendu(INT8U idx,INT16U val)
{
	if (idx >= MAX_TAN_ZHEN_CNT)
	{
		return ;
	}

	stGB32960Param.rldc_tanzhen_wendu[idx] = val;
}

INT8U gb32960_can_get_rldc_tanzhen_wendu(INT8U idx)
{
	if (idx >= MAX_TAN_ZHEN_CNT)
	{
		return 0;
	}

	return stGB32960Param.rldc_tanzhen_wendu[idx];
}



void gb32960_can_set_max_wendu_in_qi_system(INT16U val)
{
	stGB32960Param.max_wendu_in_qi_system = val;
}

INT16U gb32960_can_get_max_wendu_in_qi_system(void)
{
	return stGB32960Param.max_wendu_in_qi_system;
}

void gb32960_can_set_max_wendu_tanzhen_id_in_qi_system(INT8U val)
{
	stGB32960Param.max_wendu_tanzhen_id_in_qi_system = val;
}

INT8U gb32960_can_get_max_wendu_tanzhen_id_in_qi_system(void)
{
	return stGB32960Param.max_wendu_tanzhen_id_in_qi_system;
}

void gb32960_can_set_max_qingqi_nongdu(INT16U val)
{
	stGB32960Param.max_qingqi_nongdu = val;
}

INT16U gb32960_can_get_max_qingqi_nongdu(void)
{
	return stGB32960Param.max_qingqi_nongdu;
}

void gb32960_can_set_max_qingqi_sensor_id(INT8U val)
{
	stGB32960Param.max_qingqi_sensor_id = val;
}

INT8U gb32960_can_get_max_qingqi_sensor_id(void)
{
	return stGB32960Param.max_qingqi_sensor_id;
}

void gb32960_can_set_max_qingqi_yali(INT16U val)
{
	stGB32960Param.max_qingqi_yali = val;
}

INT16U gb32960_can_get_max_qingqi_yali(void)
{
	return stGB32960Param.max_qingqi_yali;
}

void gb32960_can_set_max_qingqi_yali_sensor_id(INT8U val)
{
	stGB32960Param.max_qingqi_yali_sensor_id = val;
}

INT8U gb32960_can_get_max_qingqi_yali_sensor_id(void)
{
	return stGB32960Param.max_qingqi_yali_sensor_id;
}

void gb32960_can_set_gaoya_dc_dc_status(INT8U val)
{
	if ((val != 0x1) && (val != 0x02) && (val != 0xFE) && (val != 0xFF))
	{
		return ;
	}
	stGB32960Param.gaoya_dc_dc_status = val;
	stGB32960Time.gaoya_dc_dc_status_time = gb32960_get_sys_run_time();
}

INT8U gb32960_can_get_gaoya_dc_dc_status(void)
{
	return stGB32960Param.gaoya_dc_dc_status;
}


void gb32960_can_set_max_sos_level(INT8U val)
{
	if ((val <= 3) || (val == 0xFF) || (val == 0xFE))
	{
		stGB32960Param.max_sos_level = val;
	}
}

INT8U gb32960_can_get_max_sos_level(void)
{
	return stGB32960Param.max_sos_level;
}

void gb32960_can_set_sos_flag_by_bit(INT8U whitch_bit,INT8U val)
{
	if (val)
	{
		stGB32960Param.sos_flag |= (1 << whitch_bit);
		sos_test_flag_val |= (1 << whitch_bit);
	}
	else
	{
		if (gb32960_can_get_sos_test() == 0)
		{
			sos_test_flag_val &= (~(1 << whitch_bit));
		}
		stGB32960Param.sos_flag &= (~(1 << whitch_bit));
	}
}

void gb32960_can_set_sos_flag(INT32U val)
{
	stGB32960Param.sos_flag = val;
	sos_test_flag_val = val;
}

INT32U gb32960_can_get_sos_flag(void)
{
	if (gb32960_can_get_sos_test())
	{
		return sos_test_flag_val;
	}
	return stGB32960Param.sos_flag;
}


void gb32960_can_set_charge_set_dtc_cnt(INT8U val)
{
	stGB32960Param.can_charge_set_dtc_cnt = val;
}

INT8U gb32960_can_get_charge_set_dtc_cnt(void)
{
	return stGB32960Param.can_charge_set_dtc_cnt;
}

void gb32960_can_set_charge_set_dtc_list(INT8U i,INT32U dat)
{
	if (i >= MAX_CAN_CHARGE_DTC_CNT)
	{
		return ;
	}

	stGB32960Param.charge_dtc_buf[i] = dat;
}

INT32U gb32960_can_get_charge_set_dtc_list(INT8U i)
{
	if (i >= MAX_CAN_CHARGE_DTC_CNT)
	{
		return 0;
	}
	return stGB32960Param.charge_dtc_buf[i];
}

//驱动电机故障总数
void gb32960_can_set_qddj_dtc_cnt(INT8U val)
{
	stGB32960Param.qddj_dtc_cnt = val;
}

INT8U gb32960_can_get_qddj_dtc_cnt(void)
{
	return stGB32960Param.qddj_dtc_cnt;
}

void gb32960_can_set_qddj_dtc_list(INT8U i,INT32U dat)
{
	if (i >= MAX_QDDJ_DTC_CNT)
	{
		return ;
	}
	stGB32960Param.qddj_dtc_buf[i] = dat;
}

INT32U gb32960_can_get_qddj_dtc_list(INT8U i)
{
	if (i >= MAX_QDDJ_DTC_CNT)
	{
		return 0;
	}
	return stGB32960Param.qddj_dtc_buf[i];
}


void gb32960_can_set_ecu_dtc_cnt(INT8U val)
{
	stGB32960Param.ecu_dtc_cnt = val;
}

INT8U gb32960_can_get_ecu_dtc_cnt(void)
{
	return stGB32960Param.ecu_dtc_cnt;
}

void gb32960_can_set_ecu_dtc_list(INT8U i,INT32U dat)
{
	if (i >= MAX_ECU_DTC_CNT)
	{
		return ;
	}
	stGB32960Param.ecu_dtc_buf[i] = dat;
}

INT32U gb32960_can_get_ecu_dtc_list(INT8U i)
{
	if (i >= MAX_ECU_DTC_CNT)
	{
		return 0;
	}
	return stGB32960Param.ecu_dtc_buf[i];
}


void gb32960_can_set_other_dtc_cnt(INT8U val)
{
	stGB32960Param.other_dtc_cnt = val;
}

INT8U gb32960_can_get_other_dtc_cnt(void)
{
	return stGB32960Param.other_dtc_cnt;
}

void gb32960_can_set_other_dtc_list(INT8U i,INT32U dat)
{
	if (i >= MAX_OTHER_DTC_CNT)
	{
		return ;
	}
	stGB32960Param.other_dtc_buf[i] = dat;
}

void gb32960_can_add_other_dtc(INT32U dat)
{
	unsigned int i = 0;

	if ((dat == 0)||(dat == 192)||(dat == 214)||(dat == 200)) return ;

	stGB32960Time.other_dtc_time = gb32960_get_sys_run_time();

	for (i = 0; i < stGB32960Param.other_dtc_cnt; i++)
	{
		if (stGB32960Param.other_dtc_buf[i] == dat)
		{
			return ;
		}
	}

	if (stGB32960Param.other_dtc_cnt >= MAX_OTHER_DTC_CNT)
	{
		stGB32960Param.other_dtc_buf[0] = dat;
	}
	else
	{
		stGB32960Param.other_dtc_buf[stGB32960Param.other_dtc_cnt] = dat;
		stGB32960Param.other_dtc_cnt++;
	}
}


INT32U gb32960_can_get_other_dtc_list(INT8U i)
{
	if (i >= MAX_OTHER_DTC_CNT)
	{
		return 0;
	}
	return stGB32960Param.other_dtc_buf[i];
}


//储能系统个数
void gb32960_can_set_dianchi_cnt(INT8U val)
{
	stGB32960Param.dianchi_cnt = val;
}

INT8U gb32960_can_get_dianchi_cnt(void)
{
	return stGB32960Param.dianchi_cnt;
}


void gb32960_can_set_cnxt_id(INT8U idx,INT8U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].id = val;
}

INT8U gb32960_can_get_cnxt_id(INT8U idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 1;
	}

	return stGB32960Param.stCNXTData[idx].id;
}

void gb32960_can_set_cnxt_wendu_tanzhen_cnt(INT8U idx,INT16U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	if (val == 0xFF)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].wendu_tanzhen_cnt = val;
}

INT16U gb32960_can_get_cnxt_wendu_tanzhen_cnt(INT8U idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 1;
	}

	return stGB32960Param.stCNXTData[idx].wendu_tanzhen_cnt;
}



void gb32960_can_set_cnxt_wendu_tanzhen_wendu(INT8U idx,INT16U wendu_idx,INT8U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}
	if (wendu_idx >= WENDU_TANZHEN_CNT_EVERY_DIANCHI)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].wendu[wendu_idx] = val;
}

INT8U gb32960_can_get_cnxt_wendu_tanzhen_wendu(INT8U idx,INT16U wendu_idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 40;
	}

	if (wendu_idx >= WENDU_TANZHEN_CNT_EVERY_DIANCHI)
	{
		return 40;
	}

	return stGB32960Param.stCNXTData[idx].wendu[wendu_idx];
}



void gb32960_can_set_cnxt_dianya(INT8U idx,INT16U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].dianya = val;
	stGB32960Time.stCNXTData_time = gb32960_get_sys_run_time();
}

INT16U gb32960_can_get_cnxt_dianya(INT8U idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 1;
	}

	return stGB32960Param.stCNXTData[idx].dianya;
}


void gb32960_can_set_cnxt_dianliu(INT8U idx,INT16U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].dianliu = val;
	stGB32960Time.stCNXTData_time = gb32960_get_sys_run_time();
}

INT16U gb32960_can_get_cnxt_dianliu(INT8U idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 1;
	}

	return stGB32960Param.stCNXTData[idx].dianliu;
}

void gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(INT8U idx,INT8U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	if (val == 0xFF)
	{
		return ;
	}

	stGB32960Param.stCNXTData[idx].benzhen_danti_cnt = val;
}

INT8U gb32960_can_get_cnxt_benzhen_dantidianchi_total_cnt(INT8U idx)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 1;
	}

	return stGB32960Param.stCNXTData[idx].benzhen_danti_cnt;
}


void gb32960_can_set_cnxt_dantidianchi_dianya(INT8U idx,INT8U dianya_id,INT16U val)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return ;
	}

	if (dianya_id >= DANTI_DIANCHI_CNT_EVERY_DIANCHI)
	{
		return ;
	}
	stGB32960Param.stCNXTData[idx].danti_dianchi_dianya[dianya_id] = val;
	stGB32960Time.stCNXTData_time = gb32960_get_sys_run_time();
}

INT16U gb32960_can_get_cnxt_dantidianchi_dianya(INT8U idx,INT8U dianya_id)
{
	if (idx >= MAX_DIANCHI_CNT)
	{
		return 0;
	}

	if (dianya_id >= DANTI_DIANCHI_CNT_EVERY_DIANCHI)
	{
		return 0;
	}

	return stGB32960Param.stCNXTData[idx].danti_dianchi_dianya[dianya_id];
}


char *gb32960_get_car_status_str(INT8U val)
{
	if (val == 1)
	{
		return "启动";
	}
	else if (val == 2)
	{
		return "熄火";
	}
	else if (val == 3)
	{
		return "其他";
	}
	else if (val == 0xFE)
	{
		return "异常";
	}

	return "车辆状态未知";
}

char *gb32960_get_charge_status_str(INT8U val)
{
	if (val == 1)
	{
		return "停车充电";
	}
	else if (val == 2)
	{
		return "行驶充电";
	}
	else if (val == 3)
	{
		return "未充电";
	}
	else if (val == 4)
	{
		return "充电完成";
	}
	else if (val == 0xFE)
	{
		return "充电异常";
	}
	return "充电状态未知";
}


#define __SET_BIT(val,bit_no) val |= (1 << bit_no)
#define __CLR_BIT(val,bit_no) val &= (~(1 << bit_no))

void lxc_analyze(unsigned char channel,CanRxMsg *prxmsg)
{
	INT32U canid = 0;
	INT8U *pdat = 0;
	INT32U temp = 0;
	//INT32U temp1 = 0;
	//INT32U i = 0;
	static INT8U s_lxc_fact_zhuanju_feedback_val = 0;
	static INT8U s_lxc_charge_flag = 0;

	if (prxmsg == 0)
	{
		return ;
	}
	pdat = prxmsg->Data;
	canid = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;

	//旅行车
	switch (canid)
	{
	case 0x1800D0F4:
	gb32960_can_set_qddj_input_dianya(0,(pdat[1] << 8) + pdat[0]);
	break;

	case 0x0C04D9D0:
	gb32960_can_set_qddj_zhuanju(0,10 * (s_lxc_fact_zhuanju_feedback_val * 0.01 - 1.25) * (((pdat[7] << 8) + pdat[6]) * 0.1 - 2000) + 20000);
	//gb32960_can_set_qddj_zhuanju(0,((pdat[7] << 8) + pdat[6]));
	break;

	case 0x1801F428:
		gb32960_can_set_milage(((pdat[3] << 24) + (pdat[2] << 16) + (pdat[1] << 8) + pdat[0]) * 0.05);
	break;

	case 0x18C1EFF3:										//单体电池电压
	for (temp = 0; temp < 3; temp++)
	{
		if (pdat[0] + (pdat[1] << 8))
		{
			gb32960_can_set_cnxt_dantidianchi_dianya(0,pdat[0] + (pdat[1] << 8) - 1 + temp,pdat[2 + (temp * 2)] + (pdat[3 + (temp * 2)] << 8));
		}
	}
/*
	gb32960_can_set_max_danti_dianya_val(gb32960_can_get_cnxt_dantidianchi_dianya(0,0));
	gb32960_can_set_min_danti_dianya_val(gb32960_can_get_cnxt_dantidianchi_dianya(0,0));
	for (i = 0; i < gb32960_can_get_cnxt_benzhen_dantidianchi_total_cnt(0); i++)
	{
		temp1 = gb32960_can_get_cnxt_dantidianchi_dianya(0,i);
		if (gb32960_can_get_max_danti_dianya_val() < temp1)
		{
			gb32960_can_set_max_danti_dianya_val(temp1);
			gb32960_can_set_max_dianya_danti_id(i + 1);
		}

		if (gb32960_can_get_min_danti_dianya_val() > temp1)
		{
			gb32960_can_set_min_danti_dianya_val(temp1);
			gb32960_can_set_min_dianya_danti_id(i + 1);
		}
	}

	gb32960_can_set_max_dianya_sub_system_id(1);
	gb32960_can_set_min_dianya_sub_system_id(1);
*/
	break;
	case 0x18C2EFF3:
	for (temp = 0; temp < 6; temp++)
	{
		if (pdat[0] + (pdat[1] << 8))
		{
			gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,pdat[0] + (pdat[1] << 8) - 1 + temp,pdat[2 + temp]);
		}
	}
/*
	gb32960_can_set_max_wendu_val(gb32960_can_get_cnxt_wendu_tanzhen_wendu(0,0));
	gb32960_can_set_min_wendu_val(gb32960_can_get_cnxt_wendu_tanzhen_wendu(0,0));
	for (i = 0; i < gb32960_can_get_cnxt_wendu_tanzhen_cnt(0); i++)
	{
		temp1 = gb32960_can_get_cnxt_wendu_tanzhen_wendu(0,i);
		if (gb32960_can_get_max_wendu_val() < temp1)
		{
			gb32960_can_set_max_wendu_val(temp1);
			gb32960_can_set_max_wendu_tanzhen_id(i + 1);
		}
		
		if (gb32960_can_get_min_wendu_val() > temp1)
		{
			gb32960_can_set_min_wendu_val(temp1);
			gb32960_can_set_min_wendu_tanzhen_id(i + 1);
		}

		gb32960_can_set_max_wendu_sub_system_id(1);
		gb32960_can_set_min_wendu_sub_system_id(1);
	}
*/
	break;
	case 0x18E3EFF3:
		if (pdat[1] + (pdat[2] << 8)) gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(0,pdat[1] + (pdat[2] << 8));
		if (pdat[3]) gb32960_can_set_cnxt_wendu_tanzhen_cnt(0,pdat[3]);
	break;
	case 0x1885EFF3:
		gb32960_can_set_jueyuandianzu(decode_can_frame(pdat,32,16,0));
	break;
	case 0x1884EFF3:
		gb32960_can_set_total_dianliu(decode_can_frame(pdat,16,16,0));
		gb32960_can_set_total_dianya(decode_can_frame(pdat,32,16,0));
	
		gb32960_can_set_cnxt_dianliu(0,decode_can_frame(pdat,16,16,0));
		gb32960_can_set_cnxt_dianya(0,decode_can_frame(pdat,32,16,0));
	
		gb32960_can_set_soc(pdat[0] * 0.4);
	break;
	case 0x1883EFF3:
		temp = decode_can_frame(pdat,41,2,0);
		if (temp == 0)
		{
			temp = 3;
		}
		else if (temp == 2)
		{
			temp = 4;
		}
		else if (temp == 3)
		{
			temp = 0xFE;
		}
		if (s_lxc_charge_flag != 2) gb32960_can_set_charge_status(temp);
	break;

	case 0x1881EFF3:
		gb32960_can_set_sos_flag_by_bit(0,decode_can_frame(pdat,2,2,0));
		gb32960_can_set_sos_flag_by_bit(1,decode_can_frame(pdat,4,2,0));
		gb32960_can_set_sos_flag_by_bit(2,decode_can_frame(pdat,6,2,0));
		gb32960_can_set_sos_flag_by_bit(3,decode_can_frame(pdat,8,2,0));
		gb32960_can_set_sos_flag_by_bit(4,decode_can_frame(pdat,10,2,0));
		gb32960_can_set_sos_flag_by_bit(5,decode_can_frame(pdat,12,2,0));
		gb32960_can_set_sos_flag_by_bit(6,decode_can_frame(pdat,14,2,0));
		gb32960_can_set_sos_flag_by_bit(7,decode_can_frame(pdat,22,2,0));
		gb32960_can_set_sos_flag_by_bit(8,decode_can_frame(pdat,24,1,0));
		gb32960_can_set_sos_flag_by_bit(9,decode_can_frame(pdat,25,1,0));
		gb32960_can_set_sos_flag_by_bit(10,decode_can_frame(pdat,18,2,0));
		gb32960_can_set_sos_flag_by_bit(11,decode_can_frame(pdat,16,2,0));

		gb32960_can_set_sos_flag_by_bit(16,decode_can_frame(pdat,26,1,0));
	break;

	case 0x0C0228D0:
		gb32960_can_set_qddj_controller_wendu(0,pdat[6]);

		if ((pdat[4] >> 4) & 0x01)
		{
			temp = 1;
		}
		else
		{
			temp = 0;
		}
		gb32960_can_set_zhidongtaban_status(temp ? 100 : 0);
		//gb32960_can_add_other_dtc(pdat[3]);
	break;

	case 0x0C0328D0:

	break;
	case 0x0C0128D0:
		gb32960_can_set_qddj_controller_dianliu(0,(pdat[0] + (pdat[1] << 8)) - 22000);
		gb32960_can_set_qddj_rpm(0,(((pdat[2] + (pdat[3] << 8)) * 0.5) - 10000) + 20000);
		gb32960_can_set_speed(pdat[5] * 10);
		gb32960_can_set_jiasutaban_val(((pdat[7] << 8) + pdat[6]) * 0.1);
	break;

	case 0x0C02D4D0:
		gb32960_can_set_car_status(pdat[0] & 0x07);
		gb32960_can_set_run_mode((pdat[0] >> 3) & 0x07);
		temp = pdat[1] & 0x07;
		if (temp == 1)
		{
			gb32960_can_set_dangwei(0,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 2)
		{
			gb32960_can_set_dangwei(0x0D,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 3)
		{
			gb32960_can_set_dangwei(0x0E,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 5)
		{
			gb32960_can_set_dangwei(0x0F,pdat[1] & 0x80,pdat[1] & 0x40);
		}

		if (((pdat[1] >> 3) & 0x07) <= 2)
		{
			gb32960_can_set_dc_dc_status((pdat[1] >> 3) & 0x07);
		}
		else if (((pdat[1] >> 3) & 0x07) == 3)
		{
			gb32960_can_set_dc_dc_status(0xFE);
		}
		else if (((pdat[1] >> 3) & 0x07) == 7)
		{
			gb32960_can_set_dc_dc_status(0xFF);
		}

		gb32960_can_set_qddj_cnt(pdat[2]);
		gb32960_can_set_qddj_xuhao(0,pdat[3]);
		gb32960_can_set_qddj_wendu(0,pdat[4]);
		if ((pdat[5] & 0x07) == 0x05)
		{
			temp = 0x04;
		}
		else if ((pdat[5] & 0x07) == 0x06)
		{
			temp = 0xFE;
		}
		else if ((pdat[5] & 0x07) == 0x07)
		{
			temp = 0xFF;
		}
		else
		{
			temp = pdat[5] & 0x07;
		}
		gb32960_can_set_qddj_status(0,temp);

		gb32960_can_set_sos_flag_by_bit(12,pdat[7] & 0x01);
		gb32960_can_set_sos_flag_by_bit(13,(pdat[7] >> 4) & 0x01);
		gb32960_can_set_sos_flag_by_bit(14,(pdat[7] >> 1) & 0x01);
		gb32960_can_set_sos_flag_by_bit(15,(pdat[7] >> 3) & 0x01);
		gb32960_can_set_sos_flag_by_bit(17,(pdat[7] >> 2) & 0x01);
		s_lxc_fact_zhuanju_feedback_val = pdat[6];

		s_lxc_charge_flag = decode_can_frame(pdat,40,3,0);
		if (s_lxc_charge_flag == 2)
		{
			gb32960_can_set_charge_status(2);
		}
	break;

	default:break;
	}

	if (1)
	{
		temp = gb32960_can_get_sos_flag();
		if (
			(temp & (1 << 1))
			|| (temp & (1 << 2))
			|| (temp & (1 << 5))
			|| (temp & (1 << 7))
			|| (temp & (1 << 11))
			|| (temp & (1 << 18))
			)
		{
			gb32960_can_set_max_sos_level(3);
		}
		else if ((temp & (1 << 13))
			|| (temp & (1 << 14))
			|| (temp & (1 << 15))
			|| (temp & (1 << 17))
			)
		{
			gb32960_can_set_max_sos_level(2);
		}
		else if ((temp & (1 << 0))
			|| (temp & (1 << 3))
			|| (temp & (1 << 4))
			|| (temp & (1 << 6))
			|| (temp & (1 << 8))
			|| (temp & (1 << 9))
			|| (temp & (1 << 10))
			|| (temp & (1 << 12))
			|| (temp & (1 << 16))
			)
		{
			gb32960_can_set_max_sos_level(1);
		}
		else
		{
			gb32960_can_set_max_sos_level(0);
		}
	}

	if (gb32960_can_get_sos_test())
	{
		//持续四秒都没收到报警标志 则清除测试报警位
		for (temp = 0; temp < 19; temp++)
		{
			if ((stGB32960Param.sos_flag & (1 << temp)))
			{
				test_sos_time[temp] = GetRunTime();
			}

			if ((GetRunTime() - test_sos_time[temp]) >= 4)
			{
				sos_test_flag_val &= (~(1 << temp));
			}
		}
	}
}

extern INT32U zuocang_no_driver_time;
void gb32960_analyze(unsigned char channel,CanRxMsg *prxmsg)
{
	INT32U canid = 0;
	INT8U *pdat = 0;
	INT32U temp = 0;
	INT32U temp1 = 0;
	static unsigned char s_can_id_395_flag = 0;
	static unsigned char s_can_id_396_flag = 0;
	static INT8U s_fact_zhuanju_feedback_val = 0;
	static unsigned char qdl = 0;
	static unsigned char zdl = 0;
	static unsigned char s_wendutanzhen_system_flag = 0;	//0表示单系统 1表示多系统
	static unsigned char s_ev30_flag = 0;					//eh43标志
	static unsigned char s_e11k_flag = 0;
	static unsigned char s_can_id_354_flag = 0;
	static unsigned char s_ev18_flag = 1;
	static unsigned char s_23A_flag = 0;
	static unsigned char s_180CD4F4_flag = 0;

	temp1 = temp1;
	if (prxmsg == 0)
	{
		return ;
	}
	pdat = prxmsg->Data;
	canid = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;

	//车型判断
	switch (canid)
	{
	//EM26
	case 0x0566:
		set_engine_type(EngineType_EM26);
	break;
	
	case 0x0593:
		s_e11k_flag = 1;
	break;

	case 0x0492:
	case 0x04B2:
	case 0x04D2:
	case 0x0552:
		set_engine_type(EngineType_D78);
	break;

	case 0x0C0728D0:
	case 0x0C0828D0:
	case 0x0C0928D0:
		if (s_ev18_flag) set_engine_type(EngineType_EV18);
		set_0c0728d0_flag(1);
	break;
	//case 0x180128E0:
	case 0x18E1EF20:
	case 0x18E2EF20:
	case 0x18E3EF20:
	case 0x18E4EF20:
	//case 0x1807D4F4:
	//case 0x1808D4F4:
		s_ev18_flag = 0;
		set_engine_type(EngineType_DFAC);
	break;

	case 0x1884EFF3:
	case 0x18C1EFF3:
		s_ev18_flag = 0;
		set_engine_type(EngineType_LXC);
	break;

	default:break;
	}

	if (get_engine_type() == EngineType_LXC)
	{
		lxc_analyze(channel,prxmsg);
		return ;
	}
	//sddf_can_analyze(canid,pdat,8);
	switch (canid)
	{
	case 0x1801D9D0:
		set_steer_wheel_angle((pdat[2] << 8) + pdat[1]);
	break;
	//about lock car protocol	
	case 0x1802D5D0:		//DH1101/EM26/D92/D94
	case 0x1812D5D0:		//DH1103/EV18
		//东风基础版本锁车反馈信息
		if ((get_electric_car_class() == 1) && (canid == 0x1802D5D0))
		{
			set_fact_lock_status((pdat[0] & 0x03) ? 1 : 0);
			set_fact_pro_ver_about_lock(((pdat[0] >> 2) & 0x03) ? 1 : 0);
			set_fact_bind_status((pdat[2] & 0x03) ? 1 : 0);

			set_iccid_head_bytes_in_ecu(pdat + 3,4);
		}
		else if ((get_electric_car_class() == 2) && (canid == 0x1812D5D0))
		{
			set_fact_lock_status((pdat[0] & 0x03) ? 1 : 0);
			set_fact_pro_ver_about_lock(((pdat[0] >> 2) & 0x03) ? 1 : 0);
			set_fact_bind_status((pdat[2] & 0x03) ? 1 : 0);

			set_iccid_head_bytes_in_ecu(pdat + 3,4);
		}

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

		//地上铁锁车反馈信息
		if (canid == 0x1802D5D0)
		{
			static unsigned char s_last_bms_charge_flag = 0;

			set_dst_fact_lock_status((pdat[0] & 0x03) ? 1 : 0);
			set_dst_fact_pro_ver_about_lock(((pdat[0] >> 2) & 0x03) ? 1 : 0);
			set_dst_fact_bind_status((pdat[2] & 0x03) ? 1 : 0);

			set_dst_fact_limit_speed_level((pdat[0] >> 4) & 0x03);
			set_dst_fact_kucunrong_limit_speed(pdat[0] >> 6);
			set_dst_fact_bms_charge_flag((pdat[2] >> 2) & 0x01);

			if (get_dst_fact_bms_charge_flag() != s_last_bms_charge_flag)
			{
				if (get_dst_fact_bms_charge_flag() == 0)
				{
					set_dst_want_bms_charge_flag(0);						//bms反馈 禁止补电 停止补电请求
				}

				s_last_bms_charge_flag = get_dst_fact_bms_charge_flag();
			}
		}
	break;
	case 0x1803D5D0:		//DH1101/EM26/D92/D94
	case 0x1813D5D0:		//DH1103/EV18
		if (canid == 0x1803D5D0)
		{
			set_electric_car_class(get_electric_car_class() | 0x01);
			//set_electric_car_class(1);
			set_iccid_bytes_in_ecu(pdat,8);
		}
		else if (canid == 0x1813D5D0)
		{
			set_electric_car_class(get_electric_car_class() | 0x02);
			//set_electric_car_class(2);
			set_iccid_bytes_in_ecu(pdat,8);
		}
		set_lock_car_can_channel(channel);
	break;
	//DFAC
	case 0x18E3EF20:
		gb32960_can_set_rldc_dianliu((pdat[1] << 8) + pdat[0]);
		gb32960_can_set_rldc_dianya((pdat[3] << 8) + pdat[2]);
	break;
	case 0x18E4EF20:
		gb32960_can_set_gaoya_dc_dc_status(pdat[0]);
	break;

	case 0x18E5EF20:
		//gb32960_can_set_rldc_tanzhen_cnt((pdat[5] << 8) + pdat[4]);
	break;

	case 0x18E3EF21:
		gb32960_can_set_max_qingqi_yali(((pdat[3] << 8) + pdat[2]) * 0.1);
		gb32960_can_set_max_qingqi_yali_sensor_id(pdat[4] & 0x0F);
	break;
	case 0x18E5EF21:
		gb32960_can_set_max_wendu_in_qi_system(pdat[0] * 10);
		gb32960_can_set_max_wendu_tanzhen_id_in_qi_system(pdat[2] & 0x0F);
		gb32960_can_set_max_qingqi_nongdu((pdat[4] << 8) + pdat[3]);
		gb32960_can_set_max_qingqi_sensor_id(pdat[7] & 0x0F);
	break;

	case 0x180220D0:
		gb32960_can_set_rldc_xiaohaolv((pdat[3] << 8) + pdat[2]);
		gb32960_can_set_ecu_ranliao_xiaohaolv((pdat[3] << 8) + pdat[2]);
	break;
	case 0x180120D0:
		#if 1
		temp = (pdat[2] >> 6) & 0x03;
		if (temp == 0)			//燃电模式
		{
			temp = 3;
		}
		else if (temp == 1)		//纯电模式
		{
			temp = 1;
		}
		else if (temp == 2)		//调试模式
		{
			temp = 2;
		}
		else
		{
			temp = 2;
		}
		gb32960_can_set_run_mode(temp);
		#endif
	break;
	case 0x1801F428:
		gb32960_can_set_milage(((pdat[3] << 24) + (pdat[2] << 16) + (pdat[1] << 8) + pdat[0]) * 0.05);
	break;
	case 0x18F0010B:
		if ((pdat[1] != 0xFF) && (pdat[1] != 0xFE))
		{
			temp = pdat[1] * 0.4;
		}
		else
		{
			temp = 0;
		}
		//gb32960_can_set_zhidongtaban_status(temp);
	break;
		
	case 0x18FF7321:
		set_power_switch_status((pdat[0] >> 6) & 0x03);
		set_key_in_status((pdat[1] >> 6) & 0x03);
	break;
	
	case 0x18FFF9D0:
		set_feng_mode_status((pdat[0]) & 0x0F);
		set_feng_dangwei_status((pdat[0] >> 4) & 0x0F);
		set_xunhuan_mode_status((pdat[1] >> 2) & 0x03);
		set_ac_quire_status((pdat[1] >> 4) & 0x03);
		set_shaung_mode_status((pdat[1] >> 6) & 0x03);
	
	break;
		
	case 0x0C0128D0:
		gb32960_can_set_qddj_controller_dianliu(0,(pdat[0] + (pdat[1] << 8)) - 22000);
		gb32960_can_set_qddj_rpm(0,(((pdat[2] + (pdat[3] << 8)) * 0.5) - 10000) + 20000);
		gb32960_can_set_speed(pdat[5] * 10);
		gb32960_can_set_jiasutaban_val(((pdat[7] << 8) + pdat[6]) * 0.1);
		set_collect_32960_acc_status(1);
	break;
	case 0x0C0228D0:
		gb32960_can_set_qddj_controller_wendu(0,pdat[6]);
		set_hand_brk_status((pdat[4]) & 0x01); //手刹状态
		if ((pdat[4] >> 4) & 0x01)
		{
			zdl = 1;
		}
		else
		{	
			zdl = 0;
		}
		gb32960_can_set_zhidongtaban_status(zdl ? 100 : 0);

		gb32960_can_add_other_dtc(pdat[3]);
		set_ready_status((pdat[5]>>6) & 0x01); //整车启动状态
	break;


	//EV18/DFAC15/DFAC16
	case 0x18FF7021:
		set_bcm_online_status(1);
		temp1 = 0;

		//侧滑门
		temp = pdat[2] & 0x03;
		set_ce_door_switch(temp);
		if (temp == 1)
		{
			temp1 |= (1 << 0);
		}
		//后背门
		temp = (pdat[2] >> 2) & 0x03;
		set_back_door_switch(temp);
		if (temp == 1)
		{
			temp1 |= (1 << 1);
		}
		//司机侧前门
		temp = (pdat[2] >> 4) & 0x03;
		set_l_f_door_switch(temp);
		if (temp == 1)
		{
			temp1 |= (1 << 2);
		}

		//副驾驶侧前门
		temp = (pdat[2] >> 6) & 0x03;
		set_r_f_door_switch(temp);
		if (temp == 1)
		{
			temp1 |= (1 << 3);
		}

		//中央门
		temp = pdat[4] & 0x03;
		set_center_door_status(temp);
		if (temp == 1)
		{
			temp1 |= (1 << 4);
		}

		if (temp1)		//有一个门开 都算时门开
		{
			set_door_lock_status(1);
			set_dst_chemen_switch(1);
		}
		else
		{
			set_door_lock_status(0);
			set_dst_chemen_switch(0);
		}

		//左前门锁  0关  1开 2错误 3无效
		if ((pdat[3] & 0x03) == 0x01)
		{
			set_l_f_door_status(1);
			set_dst_left_qian_chesuo_switch(1);
		
		}
		else
		{
			set_l_f_door_status(0);
			set_dst_left_qian_chesuo_switch(0);
			
		}

		//acc信号当做车锁信号
		temp1 = 0;
		temp = (pdat[3] >> 2) & 0x03;
		if (temp == 1)
		{
			temp1 |= (1 << 0);
		}

		temp = (pdat[3] >> 4) & 0x03;
		if (temp == 1)
		{
			temp1 |= (1 << 1);
		}
		
		if (temp1)		//有一个ACC开 都算是ACC开
		{
			set_dst_chesuo_switch(1);
		}
		else
		{
			set_dst_chesuo_switch(0);
		}
		set_d_safe_belt_status((pdat[1] >> 6) & 0x03);
		set_ds_front_door_status((pdat[2] >> 4) & 0x03);
		set_l_front_door_status((pdat[3]) & 0x03);
		set_ON_switch_status((pdat[3] >> 4) & 0x03);
		
		if(((pdat[7] >> 6) & 0x03) ==0x1)
		{
			zuocang_no_driver_time = 0;
		}
	break;
	case 0x1804D4F4:
	/*
		gb32960_can_set_max_dianya_sub_system_id(pdat[0]);
		gb32960_can_set_max_dianya_danti_id(pdat[1]);
		//gb32960_can_set_max_danti_dianya_val(gb32960_can_get_cnxt_dantidianchi_dianya(0,gb32960_can_get_max_dianya_danti_id() - 1));

		gb32960_can_set_min_dianya_sub_system_id(pdat[2]);
		gb32960_can_set_min_dianya_danti_id(pdat[3]);
		//gb32960_can_set_min_danti_dianya_val(gb32960_can_get_cnxt_dantidianchi_dianya(0,gb32960_can_get_min_dianya_danti_id() - 1));

		gb32960_can_set_max_wendu_sub_system_id(pdat[4]);
		gb32960_can_set_max_wendu_tanzhen_id(pdat[5]);
		//gb32960_can_set_max_wendu_val(gb32960_can_get_cnxt_wendu_tanzhen_wendu(0,gb32960_can_get_max_wendu_tanzhen_id() - 1));

		gb32960_can_set_min_wendu_sub_system_id(pdat[6]);
		gb32960_can_set_min_wendu_tanzhen_id(pdat[7]);
		//gb32960_can_set_min_wendu_val(gb32960_can_get_cnxt_wendu_tanzhen_wendu(0,gb32960_can_get_min_wendu_tanzhen_id() - 1));
	*/
	break;
	case 0x1805D4F4:
		gb32960_can_set_cnxt_dantidianchi_dianya(0,pdat[0] - 1,((pdat[2] << 8) + pdat[1]) * 10);
		gb32960_can_set_cnxt_dantidianchi_dianya(0,pdat[3] - 1,((pdat[5] << 8) + pdat[4]) * 10);

		if (get_engine_type() == EngineType_DFAC)
		{
			if (pdat[7]) gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(0,pdat[7]);
		}
	break;
	case 0x1810D4F4:
		gb32960_can_set_cnxt_dantidianchi_dianya(1,pdat[0] - 1,((pdat[2] << 8) + pdat[1]) * 10);
		gb32960_can_set_cnxt_dantidianchi_dianya(1,pdat[3] - 1,((pdat[5] << 8) + pdat[4]) * 10);
		if (get_engine_type() == EngineType_DFAC)
		{
			if (pdat[7]) gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(1,pdat[7]);
			gb32960_can_set_dianchi_cnt(2);
		}
	break;
	case 0x1806D4F4:						//单系统探针ID
		if (s_wendutanzhen_system_flag == 0)
		{
			if (gb32960_can_get_dianchi_cnt() == 1)
			{
				for (temp = 0; temp < 4; temp++)
				{
					gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,pdat[temp * 2] - 1,pdat[(temp * 2) + 1]);
				}
			}
			else								//子系统数量大于1 要拆成2个
			{
				for (temp = 0; temp < 4; temp++)
				{
					if ((pdat[temp * 2] - 1) >= gb32960_can_get_cnxt_wendu_tanzhen_cnt(0))
					{
						temp1 = 1;
					}
					else
					{
						temp1 = 0;
					}

					if (temp1)
					{
						gb32960_can_set_cnxt_wendu_tanzhen_wendu(temp1,pdat[temp * 2] - 1 - gb32960_can_get_cnxt_wendu_tanzhen_cnt(0),pdat[(temp * 2) + 1]);
					}
					else
					{
						gb32960_can_set_cnxt_wendu_tanzhen_wendu(temp1,pdat[temp * 2] - 1,pdat[(temp * 2) + 1]);
					}
				}
			}
		}
	break;
	case 0x1807D4F4:						//多系统探针ID 子系统1
		s_wendutanzhen_system_flag = 1;
		for (temp = 0; temp < 3; temp++)
		{
			gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,pdat[temp * 2] - 1,pdat[(temp * 2) + 1]);
		}
		gb32960_can_set_cnxt_wendu_tanzhen_cnt(0,pdat[7]);
	
		for (temp = 0; temp < 3; temp++)
		{
			gb32960_can_set_rldc_tanzhen_wendu(pdat[temp * 2] - 1,pdat[(temp * 2) + 1]);
		}
		gb32960_can_set_rldc_tanzhen_cnt(pdat[7]);
		
	break;
	case 0x1808D4F4:						//多系统探针ID 子系统2
		s_wendutanzhen_system_flag = 1;
		for (temp = 0; temp < 3; temp++)
		{
			gb32960_can_set_cnxt_wendu_tanzhen_wendu(1,pdat[temp * 2] - 1 - gb32960_can_get_cnxt_wendu_tanzhen_cnt(0),pdat[(temp * 2) + 1]);
		}
		gb32960_can_set_cnxt_wendu_tanzhen_cnt(1,pdat[7]);

	break;
	case 0x1801D4F4:
		if ((pdat[0] & 0x07) == 0x05)
		{
			temp = 0xFE;
		}
		else if ((pdat[0] & 0x07) == 0x07)
		{
			temp = 0xFF;
		}
		else
		{
			temp = pdat[0] & 0x07;
		}
		gb32960_can_set_charge_status(temp);

		gb32960_can_set_jueyuandianzu((pdat[2] << 8) + pdat[1]);

		gb32960_can_set_sos_flag_by_bit(0,pdat[3] & 0x01);
		gb32960_can_set_sos_flag_by_bit(1,(pdat[3] >> 1) & 0x01);
		gb32960_can_set_sos_flag_by_bit(2,(pdat[3] >> 2) & 0x01);
		gb32960_can_set_sos_flag_by_bit(3,(pdat[3] >> 3) & 0x01);
		gb32960_can_set_sos_flag_by_bit(4,(pdat[3] >> 4) & 0x01);
		gb32960_can_set_sos_flag_by_bit(7,(pdat[3] >> 5) & 0x01);
		gb32960_can_set_sos_flag_by_bit(8,(pdat[3] >> 6) & 0x01);

		gb32960_can_set_sos_flag_by_bit(5,(pdat[4] >> 0) & 0x01);
		gb32960_can_set_sos_flag_by_bit(6,(pdat[4] >> 1) & 0x01);
		gb32960_can_set_sos_flag_by_bit(9,(pdat[4] >> 2) & 0x01);
		gb32960_can_set_sos_flag_by_bit(10,(pdat[4] >> 3) & 0x01);
		gb32960_can_set_sos_flag_by_bit(11,(pdat[4] >> 4) & 0x01);
		gb32960_can_set_sos_flag_by_bit(16,(pdat[4] >> 5) & 0x01);
		gb32960_can_set_sos_flag_by_bit(18,(pdat[4] >> 6) & 0x01);

		if (pdat[6] && (pdat[6] != 0xFF))
		{
			gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(0,pdat[6]);

			if (gb32960_can_get_dianchi_cnt() > 1)
			{
				gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(1,pdat[6]);
			}
		}

		if (pdat[7] && (pdat[7] != 0xFF))
		{
			if (gb32960_can_get_dianchi_cnt() > 1)
			{
				gb32960_can_set_cnxt_wendu_tanzhen_cnt(0,pdat[7] / 2);
				gb32960_can_set_cnxt_wendu_tanzhen_cnt(1,pdat[7] - (pdat[7] / 2));
			}
			else
			{
				gb32960_can_set_cnxt_wendu_tanzhen_cnt(0,pdat[7]);
			}
		}
	break;
	case 0x1800D0F4:
		if (get_engine_type() == EngineType_EV18)
		{
			gb32960_can_set_total_dianya((pdat[1] << 8) + pdat[0]);
			gb32960_can_set_total_dianliu(((pdat[3] << 8) + pdat[2]) - 22000);
			gb32960_can_set_soc(pdat[4] * 0.4);
		}
		//else
		{
			gb32960_can_set_rldc_dianya((pdat[1] << 8) + pdat[0]);
			gb32960_can_set_rldc_dianliu(((pdat[3] << 8) + pdat[2]) - 32000);

			gb32960_can_set_qddj_input_dianya(0,(pdat[1] << 8) + pdat[0]);
			gb32960_can_set_cnxt_dianya(0,(pdat[1] << 8) + pdat[0]);
			gb32960_can_set_cnxt_dianliu(0,((pdat[3] << 8) + pdat[2]) - 22000);
		}

	break;
	
	case 0x180CD4F4:
		if (get_engine_type() == EngineType_EV18)
		{
		   s_180CD4F4_flag = pdat[2] & 0x03;
		}
	break;
	
	case 0x181A28F4:

		gb32960_can_set_sos_flag_by_bit(5,pdat[4] & 0x01);	//32bit
		gb32960_can_set_sos_flag_by_bit(6,pdat[4] & 0x02);	//33bit
		gb32960_can_set_sos_flag_by_bit(7,pdat[4] & 0x04);	//34bit
		gb32960_can_set_sos_flag_by_bit(4,pdat[4] & 0x40);	//38bit
		gb32960_can_set_sos_flag_by_bit(1,pdat[4] & 0x20);	//37bit
		gb32960_can_set_sos_flag_by_bit(11,(pdat[5] >> 1) & 0x03);	//41 42 bit
	break;
	case 0x181B28F4:
		if (get_engine_type() != EngineType_EV18)
		{
			gb32960_can_set_total_dianya((pdat[1] << 8) + pdat[0]);
			gb32960_can_set_total_dianliu(((pdat[3] << 8) + pdat[2]) - 22000);
			gb32960_can_set_soc(pdat[4] * 0.4);
		}
	break;
		
	case 0x181C28F4:
/*
		if (get_engine_type() == EngineType_EV18)
		{
			gb32960_can_set_min_danti_dianya_val(((pdat[1] << 8) + pdat[0])*10);
			gb32960_can_set_max_danti_dianya_val(((pdat[3] << 8) + pdat[2])*10);
			gb32960_can_set_max_wendu_val(pdat[4]);
			gb32960_can_set_min_wendu_val(pdat[5]);
		}
*/
	break;
		
	case 0x0C01F4D0:
		set_power_ctrl_allow((pdat[4]) & 0x03);
	break;
	case 0x0C02D4D0:
		gb32960_can_set_car_status(pdat[0] & 0x07);
		gb32960_can_set_run_mode((pdat[0] >> 3) & 0x07);
		temp = pdat[1] & 0x07;
		if (temp == 1)
		{
			gb32960_can_set_dangwei(0,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 2)
		{
			gb32960_can_set_dangwei(0x0D,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 3)
		{
			gb32960_can_set_dangwei(0x0E,pdat[1] & 0x80,pdat[1] & 0x40);
		}
		else if (temp == 5)
		{
			gb32960_can_set_dangwei(0x0F,pdat[1] & 0x80,pdat[1] & 0x40);
		}

		if (((pdat[1] >> 3) & 0x07) <= 2)
		{
			gb32960_can_set_dc_dc_status((pdat[1] >> 3) & 0x07);
		}
		else if (((pdat[1] >> 3) & 0x07) == 3)
		{
			gb32960_can_set_dc_dc_status(0xFE);
		}
		else if (((pdat[1] >> 3) & 0x07) == 7)
		{
			gb32960_can_set_dc_dc_status(0xFF);
		}

		gb32960_can_set_qddj_cnt(pdat[2]);
		gb32960_can_set_qddj_xuhao(0,pdat[3]);
		gb32960_can_set_qddj_wendu(0,pdat[4]);
		if ((pdat[5] & 0x07) == 0x05)
		{
			temp = 0x04;
		}
		else if ((pdat[5] & 0x07) == 0x06)
		{
			temp = 0xFE;
		}
		else if ((pdat[5] & 0x07) == 0x07)
		{
			temp = 0xFF;
		}
		else
		{
			temp = pdat[5] & 0x07;
		}
		gb32960_can_set_qddj_status(0,temp);

		s_fact_zhuanju_feedback_val = pdat[6];
		gb32960_can_set_sos_flag_by_bit(12,pdat[7] & 0x01);
		gb32960_can_set_sos_flag_by_bit(13,(pdat[7] >> 4) & 0x01);
		gb32960_can_set_sos_flag_by_bit(14,(pdat[7] >> 1) & 0x01);
		gb32960_can_set_sos_flag_by_bit(15,(pdat[7] >> 3) & 0x01);
		gb32960_can_set_sos_flag_by_bit(17,(pdat[7] >> 2) & 0x01);

		if (get_engine_type() == EngineType_DFAC)
		{
			gb32960_can_set_max_sos_level((pdat[7] >> 6) & 0x03);
		}
	break;
		
	case 0x0C0328D0:
		set_xunhang_status(pdat[2] & 0x03); //钥匙档位
		set_key_status((pdat[2] >> 2) & 0x03); //钥匙档位
		set_zhufujiechuqi_status((pdat[2] >> 4) & 0x03);
		set_vcu_wakeup_status((pdat[7]) & 0x03);
		set_xuhang_licheng(((pdat[6] << 8) + pdat[5])*1.25);
		set_xunhang_speed(pdat[3]); //巡航车速
	break;
	case 0x0C04D9D0:
		gb32960_can_set_qddj_zhuanju(0,10 * (s_fact_zhuanju_feedback_val * 0.01 - 1.25) * (((pdat[7] << 8) + pdat[6]) * 0.1 - 2000) + 20000);
		//gb32960_can_set_qddj_zhuanju(0,((pdat[7] << 8) + pdat[6]));
		if (get_engine_type() == EngineType_EV18)
		{
			set_ev18_fan_ls_status((pdat[5] >> 1) & 0x01);
			set_ev18_fan_hs_status((pdat[5] >> 2) & 0x01);
		}
	break;
	
	
	case 0x1812D0D9:
		if (get_engine_type() == EngineType_EV18)
		{
			set_ev18_air_cond_speed((pdat[5] << 8) + pdat[4]);
			set_ev18_air_cond_status((pdat[7] ) & 0x0F);
		}
		break;
	
	case 0x1801D0DA:
		if (get_engine_type() == EngineType_EV18)
		{
			s_ev18_cond = 1;
			set_ev18_air_cond_ctrl_speed((pdat[5] << 8) + pdat[4]);
			set_ev18_air_cond_ctrl_status((pdat[6] ) & 0x03);
		}
		break;
	case 0x1815E2F4:
		if(get_engine_type() == EngineType_EV18)
		{
			set_total_charge_in	((pdat[2] << 16) +(pdat[1] << 8) + pdat[0]);
			set_total_charge_out((pdat[6] << 16) +(pdat[5] << 8) + pdat[4]);
		}
	break;

	//---------------------EM26/D78--------------------------
	case 0x0370:
	if (get_engine_type() == EngineType_EM26)
	{
		gb32960_can_set_sos_flag_by_bit(16,decode_can_frame(pdat,38,1,1));
	}
	break;
	case 0x0354:
		s_can_id_354_flag = 1;
		if (get_engine_type() == EngineType_EM26)
		{
			gb32960_can_set_speed(decode_can_frame(pdat,8,16,1) / 10);
		}
	break;
	//D78自定义数据解析
	case 0x0576:
	if (get_engine_type() == EngineType_D78)
	{
		set_d78_vcu_gz_code(pdat[0]);
	}
	else if (get_engine_type() == EngineType_EM26)
	{
		gb32960_can_add_other_dtc(pdat[0]);
	}
	break;
	case 0x0512:
	set_d78_bat_yuchong_status(pdat[6] & 0x03);
	break;
	case 0x0366:
	set_d78_ptc((pdat[6] << 8) + pdat[7]);
	set_d78_chm_hot_level((pdat[0] >> 3) & 0x03);		// 3 4 bit
	set_d78_chm_cool_level((pdat[1] >> 3) & 0x03);		// 11 12 bit
	break;
	case 0x0367:
	set_d78_cce_rpm(decode_can_frame(pdat,56,12,1));
	break;
	case 0x024A:
	set_d78_soe(decode_can_frame(pdat,56,10,1));
	set_d78_bms_gz_level(pdat[4] & 0x03);		//32 33bit
	break;
	case 0x0583:
	set_d78_bms_gz_code(pdat[0]);
	break;
	
	case 0x0584:
		if (get_engine_type() == EngineType_EM26)
		{
			set_total_charge_in	(decode_can_frame(pdat,24,20,1));
			set_total_charge_out(decode_can_frame(pdat,52,20,1));
		}
	break;
	
	case 0x02E1:
	set_d78_obc_gz_level((pdat[1] >> 2) & 0x03);		// 10 11bit
	break;
	case 0x0595:
	set_d78_obc_gz_code(pdat[0]);
	break;

	case 0x0521:
	gb32960_can_set_car_status(pdat[0]);
	gb32960_can_set_charge_status(pdat[1]);
	gb32960_can_set_run_mode(pdat[2]);
	gb32960_can_set_jiasutaban_val(pdat[3]);

	if (get_engine_type() == EngineType_EM26)
	{
		//gb32960_can_set_max_sos_level(pdat[4]);
		qdl = pdat[5] & 0x01;
		zdl = (pdat[5] >> 1) & 0x01;
	}
	else
	{
		zdl = (pdat[5] >> 1) & 0x01;
		gb32960_can_set_zhidongtaban_status(zdl ? 100 : 0);
	}
	break;

	case 0x0139:
	if (get_engine_type() == EngineType_D78)
	{
		set_d78_vcu_gz_level((pdat[7] >> 2) & 0x03);		// 58 59bit
		temp = (pdat[5] >> 5) & 0x07;
		set_d78_car_status(temp);
		if (temp == 0)						//熄火
		{
			temp = 0x02;
		}
		else if (temp == 1)					//上电
		{

		}
		else if (temp == 2)					//插枪
		{

		}
		else if (temp == 3)					//其他状态
		{
			temp = 0x03;
		}
		else if (temp == 4)					//异常
		{
			temp = 0xFE;
		}
		else if (temp == 5)					//启动
		{
			temp = 0x01;
		}
		else if (temp == 6)					//慢充
		{

		}
		else if (temp == 7)					//快充
		{

		}
		//暂时先不用这七个状态
		//gb32960_can_set_car_status(temp);
	}
	else
	{
		if (get_engine_type() == EngineType_EM26)
		{
			gb32960_can_set_zhidongtaban_status(pdat[6]);
			if (s_ev30_flag)
			{
				zdl = pdat[7] & 0x03;
				gb32960_can_set_zhidongtaban_status(zdl ? 100 : 0);
			}
		}
	}
	break;
	case 0x035A:
	if (get_engine_type() == EngineType_EM26)
	{
		gb32960_can_set_qddj_rpm(0,((pdat[0] << 8) + pdat[1]) - 32768 + 20000);
		temp = (pdat[2] >> 4) & 0x03;
		if ((temp == 0) || (temp == 3))
		{
			temp = 2;
		}
		else if (temp == 2)
		{
			temp = 0xFE;
		}
		else if (temp == 1)
		{
			temp = 1;
		}
		gb32960_can_set_dc_dc_status(temp);
	}
	break;

	case 0x0234:
	if (get_engine_type() == EngineType_D78)
	{
		set_d78_dcdc_output_vol(pdat[3]);
		set_d78_dcdc_input_vol(pdat[6]);

		temp = pdat[7] & 0x07;
		if (temp == 4)
		{
			gb32960_can_set_dc_dc_status(1);
		}
		else if ((temp == 1) || (temp == 2) || (temp == 3))
		{
			gb32960_can_set_dc_dc_status(2);
		}
		else if (temp == 5)
		{
			gb32960_can_set_dc_dc_status(0xFE);
		}
		else if (temp == 0)
		{
			gb32960_can_set_dc_dc_status(0xFF);
		}

		//20 21bit   0,1,2 no error 3:error
		temp = (pdat[2] >> 4) & 0x03;
		set_d78_dcdc_gz_level(temp);
		gb32960_can_set_sos_flag_by_bit(14,(temp == 3) ? 1 : 0);
	}
	else if (get_engine_type() == EngineType_EM26)
	{
		temp = decode_can_frame(pdat,20,2,1);
		gb32960_can_set_sos_flag_by_bit(14,(temp == 2) ? 1 : 0);
	}
	break;

	case 0x05F0:
		gb32960_can_set_qddj_cnt(pdat[0]);
		gb32960_can_set_qddj_xuhao(0,pdat[1]);
		gb32960_can_set_qddj_status(0,pdat[2]);
	break;

	case 0x0211:
		if (get_engine_type() == EngineType_EM26)
		{
			if (s_can_id_354_flag == 0)
			{
				gb32960_can_set_speed((pdat[3] << 5) + (pdat[4] >> 3));
			}
			if(!((s_e11k_flag)|| (s_ev30_flag)))
			{
				temp = pdat[7];
				if (temp == 4)			//D
				{
					temp = 14;
				}
				else if (temp == 5)		//N
				{
					temp = 0;
				}
				else if (temp == 6)		//R
				{
					temp = 13;
				}
				else
				{
					temp = 15;
				}
				gb32960_can_set_dangwei(temp,qdl,zdl);
			}
			if ((pdat[2] & 0x01) == 1)
			{
				gb32960_can_set_sos_flag_by_bit(13,1);
			}
			else
			{
				gb32960_can_set_sos_flag_by_bit(13,0);
			}			
		}
		else
		{
			gb32960_can_set_speed(((pdat[3] & 0x7F) << 6) + (pdat[4] >> 2));
			set_d78_zhenkong_gz_flag(pdat[2] & 0x03);		// 16 17bit
		}
	break;

	case 0x0135:
	if ((get_engine_type() != EngineType_EM26)|| (s_ev30_flag)|| (s_e11k_flag))
	{
		if (pdat[7] == 7)
		{
			gb32960_can_set_dangwei(0x0F,qdl,zdl);
		}
		else if (pdat[7] == 6)
		{
			gb32960_can_set_dangwei(0x0D,qdl,zdl);
		}
		else if (pdat[7] == 4)
		{
			gb32960_can_set_dangwei(0x0E,qdl,zdl);
		}
		else if (pdat[7] == 5)
		{
			gb32960_can_set_dangwei(0x00,qdl,zdl);
		}
		else
		{
			gb32960_can_set_dangwei(pdat[7],qdl,zdl);
		}
	}
	set_collect_32960_acc_status(1);
	break;
	case 0x0395:
		s_can_id_395_flag = 1;
		gb32960_can_set_qddj_input_dianya(0,(pdat[0] << 8) + pdat[1]);
		gb32960_can_set_qddj_controller_dianliu(0,(pdat[2] << 8) + pdat[3]);
	break;
	case 0x396:
		s_can_id_396_flag = 1;

		temp = (pdat[4] << 8) + pdat[5] - 10000;
		gb32960_can_set_qddj_zhuanju(0,temp);

		gb32960_can_set_qddj_wendu(0,pdat[6]);		
		gb32960_can_set_qddj_controller_wendu(0,pdat[7]);
	break;
	case 0x0133:
		if (s_can_id_396_flag == 0)
		{
			gb32960_can_set_qddj_controller_wendu(0,pdat[5]);
			gb32960_can_set_qddj_wendu(0,pdat[6]);
		}

		if (s_can_id_395_flag == 0)
		{
			temp = ((pdat[2] & 0x0F) << 6) + (pdat[3] >> 2);
			temp1 = temp * 10;
			gb32960_can_set_qddj_input_dianya(0,temp1);
			
			temp = ((pdat[3] & 0x03) << 8) + pdat[4] - 512;
			temp1 = temp * 10 + 10000;
			gb32960_can_set_qddj_controller_dianliu(0,temp1);
		}

		set_d78_mcu_input_vol(decode_can_frame(pdat,26,10,1));
	break;
	case 0x0131:
		set_d78_mcu_gz_level((pdat[5] >> 2) & 0x03);		//42 43bit
		gb32960_can_set_qddj_rpm(0,(pdat[0] << 8) + pdat[1] - 32768 + 20000);

		if (s_can_id_396_flag == 0)
		{
			temp = (decode_can_frame(pdat,27,13,1) * 0.5) - 2048;
			temp1 = (temp * 10) + 20000;
			gb32960_can_set_qddj_zhuanju(0,temp1);
		}
	break;

	case 0x03F1:
		if (get_engine_type() == EngineType_EM26)
		{
			gb32960_can_set_milage(((pdat[3] << 16) + (pdat[4] << 8) + pdat[5]) * 10);
		}
	break;

	case 0x056C://低/高速风扇(或冷却风扇)工作状态
		if (get_engine_type() == EngineType_EM26)
		{
			set_em26_fan_ls_status((pdat[3] & 0x02)>> 1);
			set_em26_fan_hs_status((pdat[3] & 0x04)>> 2);
		}
	break;
		
	case 0x0182://基本状态
		if (get_engine_type() == EngineType_EM26)
		{
			set_em26_air_cond_status((pdat[1] & 0x03)>> 0);
		}
	break;
		
	case 0x0181://空调压缩机转速反馈
		if (get_engine_type() == EngineType_EM26)
		{
			set_em26_air_cond_speed((pdat[1] << 8) + pdat[2]);
		}
	break;
	//-------------------------end EM26/D78----------------------------
	case 0x033D:
		s_ev30_flag = 1;
	break;

	case 0x026E:
		gb32960_can_set_total_dianliu(((pdat[4] << 8) + pdat[5]) + 5000);
		gb32960_can_set_cnxt_dianliu(0,((pdat[4] << 8) + pdat[5]) + 5000);

		gb32960_can_set_jueyuandianzu((pdat[0] << 8) + pdat[1]);
	break;

	//D78
	case 0x0552:
		gb32960_can_set_milage(((pdat[4] << 16) + (pdat[5] << 8) + pdat[6]) * 10);
	break;
	case 0x0310:
		//gb32960_can_set_total_dianya((pdat[6] << 8) + pdat[7]);
	break;
	case 0x0152:
	{
		gb32960_can_set_total_dianliu((pdat[4] << 8) + pdat[5] + 5000);
		gb32960_can_set_jueyuandianzu((pdat[0] << 8) + pdat[1]);
		gb32960_can_set_cnxt_dianliu(0,(pdat[4] << 8) + pdat[5] + 5000);
	}
	break;
	case 0x013F:
		if (get_engine_type() == EngineType_D78)
		{
			set_d78_mcu_igbt_flag(pdat[0] & 0x01);
			set_d78_mcu_gz_code(pdat,8);
			gb32960_can_set_sos_flag_by_bit(15,decode_can_frame(pdat,36,1,1));
			gb32960_can_set_sos_flag_by_bit(17,decode_can_frame(pdat,40,1,1));
		}
		else if (get_engine_type() == EngineType_EM26)
		{
			gb32960_can_set_sos_flag_by_bit(15,decode_can_frame(pdat,5,1,1));
			gb32960_can_set_sos_flag_by_bit(17,decode_can_frame(pdat,11,1,1));
		}
	break;
	case 0x00A0:
		if (get_engine_type() == EngineType_D78)
		{
			if ((pdat[0] & 0x03) == 3)
			{
				gb32960_can_set_sos_flag_by_bit(13,1);
			}
			else
			{
				gb32960_can_set_sos_flag_by_bit(13,0);
			}
		}
	break;
	case 0x0153:
	{
		set_d78_soh(decode_can_frame(pdat,22,8,1));
		gb32960_can_set_soc(((pdat[0] << 2) + ((pdat[1] & 0xC0) >> 6)) / 10);

		gb32960_can_set_sos_flag_by_bit(0,pdat[6] & 0x01);		//48bit
		gb32960_can_set_sos_flag_by_bit(1,pdat[5] & 0x80);		//47bit
		gb32960_can_set_sos_flag_by_bit(2,pdat[5] & 0x08);		//43bit
		gb32960_can_set_sos_flag_by_bit(3,pdat[5] & 0x10);		//44bit
		gb32960_can_set_sos_flag_by_bit(4,pdat[6] & 0x10);		//52bit
		gb32960_can_set_sos_flag_by_bit(5,pdat[5] & 0x01);		//40bit
		gb32960_can_set_sos_flag_by_bit(6,pdat[5] & 0x02);		//41bit
		gb32960_can_set_sos_flag_by_bit(7,pdat[6] & 0x08);		//51bit
		if (get_engine_type() == EngineType_D78)
		{
			gb32960_can_set_sos_flag_by_bit(8,pdat[6] & 0x20);		//53bit
			gb32960_can_set_sos_flag_by_bit(9,pdat[6] & 0x40);		//54bit

			//20 21 bit:0 2 正常 1报警 3无效
			if ((pdat[2] & 0x30))
			{
				gb32960_can_set_sos_flag_by_bit(16,1);
			}
			else
			{
				gb32960_can_set_sos_flag_by_bit(16,0);
			}
			gb32960_can_set_sos_flag_by_bit(18,pdat[5] & 0x08);		//43bit
		}
		else if (get_engine_type() == EngineType_EM26)
		{
			gb32960_can_set_sos_flag_by_bit(18,pdat[4] & 0x80);		//39bit
		}
		gb32960_can_set_sos_flag_by_bit(10,pdat[5] & 0x04);		//42bit
		gb32960_can_set_sos_flag_by_bit(11,pdat[7] & 0x01);		//56bit
	}
	break;
	case 0x022A:
		gb32960_can_set_sos_flag_by_bit(12,pdat[0] & 0x80);
		set_d78_dcdc_gz_code(pdat,8);
	break;
	
	case 0x023A:
	if(get_engine_type() == EngineType_EM26)
	{
		s_23A_flag = pdat[4] & 0x03;
	}
	break;

	case 0x052F:
	case 0x05AF:
		if ((pdat[2] << 8) + pdat[3]) gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(0,(pdat[2] << 8) + pdat[3]);
		if ((pdat[4] << 8) + pdat[5]) gb32960_can_set_cnxt_wendu_tanzhen_cnt(0,(pdat[4] << 8) + pdat[5]);
	break;
	case 0x0545:
	{
		//如果数据全是0 或者全是FF 屏蔽掉index为0的数据帧
		if ((pdat[0] == 0) && 
			(
			((pdat[1] == 0x00) && (pdat[2] == 0x00) && (pdat[3] == 0x00) && (pdat[4] == 0x00) && (pdat[5] == 0x00) && (pdat[6] == 0x00) && (pdat[7] == 0x00))
			||
			((pdat[1] == 0xFF) && (pdat[2] == 0xFF) && (pdat[3] == 0xFF) && (pdat[4] == 0xFF) && (pdat[5] == 0xFF) && (pdat[6] == 0xFF) && (pdat[7] == 0xFF))
			)
			)
		{

		}
		else
		{
			static unsigned char s_dtdy_index_0_exsit_flag = 0;
			
			if (pdat[0] == 0)
			{
				s_dtdy_index_0_exsit_flag = 1;
			}
			for (temp = 0; temp < 3; temp++)
			{
				if (s_dtdy_index_0_exsit_flag)
				{
					gb32960_can_set_cnxt_dantidianchi_dianya(0,(pdat[0] * 3) + temp,(pdat[(2*temp)+1] << 8) + pdat[(2*temp)+2]);
				}
				else
				{
					gb32960_can_set_cnxt_dantidianchi_dianya(0,(pdat[0] - 1) + temp,(pdat[(2*temp)+1] << 8) + pdat[(2*temp)+2]);
				}
			}
		}
	}
	break;
	case 0x054B:
	{
		//如果数据全是0 或者全是FF 屏蔽掉index为0的数据帧
		if ((pdat[0] == 0) && 
			(
			((pdat[1] == 0x00) && (pdat[2] == 0x00) && (pdat[3] == 0x00) && (pdat[4] == 0x00) && (pdat[5] == 0x00) && (pdat[6] == 0x00) && (pdat[7] == 0x00))
			||
			((pdat[1] == 0xFF) && (pdat[2] == 0xFF) && (pdat[3] == 0xFF) && (pdat[4] == 0xFF) && (pdat[5] == 0xFF) && (pdat[6] == 0xFF) && (pdat[7] == 0xFF))
			)
			)
		{

		}
		else
		{
			static unsigned char s_wdtz_index_0_exsit_flag = 0;
			static unsigned char s_wdtz_index_1_exsit_flag = 0;
			static unsigned char s_wdtz_index_2_exsit_flag = 0;
			
			if (pdat[0] == 0)
			{
				s_wdtz_index_0_exsit_flag = 1;
			}
			else if (pdat[0] == 1)
			{
				s_wdtz_index_1_exsit_flag = 1;
			}
			else if (pdat[0] == 2)
			{
				s_wdtz_index_2_exsit_flag = 1;
			}

			for (temp = 0; temp < 7; temp++)
			{
				if (s_wdtz_index_0_exsit_flag)										// 0 1 2 3 4
				{
					gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,(pdat[0] * 7) + temp,pdat[temp+1]);
				}
				else if (s_wdtz_index_1_exsit_flag && s_wdtz_index_2_exsit_flag)	// 1 2 3 4 5
				{
					gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,((pdat[0] - 1) * 7) + temp,pdat[temp+1]);
				}
				else																// 1 8 15 22 29
				{
					gb32960_can_set_cnxt_wendu_tanzhen_wendu(0,pdat[0] - 1 + temp,pdat[temp+1]);
				}
			}
		}
	}
	break;
	case 0x0320:
		gb32960_can_set_total_dianya((pdat[2] << 8) + pdat[3]);
		gb32960_can_set_cnxt_dianya(0,(pdat[2] << 8) + pdat[3]);
		set_d78_bms_output_vol((pdat[0] << 8) + pdat[1]);
	break;

	case 0x0525:
/*
		gb32960_can_set_max_dianya_sub_system_id(pdat[0]);
		gb32960_can_set_max_dianya_danti_id(pdat[1]);
		gb32960_can_set_max_danti_dianya_val((pdat[2] << 8) + pdat[3]);
		gb32960_can_set_min_dianya_sub_system_id(pdat[4]);
		gb32960_can_set_min_dianya_danti_id(pdat[5]);
		gb32960_can_set_min_danti_dianya_val((pdat[6] << 8) + pdat[7]);
*/	
	break;
	case 0x0541:
/*
		gb32960_can_set_max_wendu_sub_system_id(pdat[0]);
		gb32960_can_set_max_wendu_tanzhen_id(pdat[1]);
		gb32960_can_set_max_wendu_val(pdat[2]);

		gb32960_can_set_min_wendu_sub_system_id(pdat[3]);
		gb32960_can_set_min_wendu_tanzhen_id(pdat[4]);
		gb32960_can_set_min_wendu_val(pdat[5]);
*/
	break;

	default:break;
	}

	if (get_engine_type() == EngineType_D78)
	{
		temp = gb32960_can_get_sos_flag();
		if ((temp & (1 << 0))
			|| (temp & (1 << 1))
			|| (temp & (1 << 2))
			|| (temp & (1 << 3))
			|| (temp & (1 << 5))
			|| (temp & (1 << 6))
			|| (temp & (1 << 7))
			|| (temp & (1 << 11))
			|| (temp & (1 << 13))
			|| (temp & (1 << 18))
			)
		{
			gb32960_can_set_max_sos_level(3);
		}
		else if ((temp & (1 << 10))
			|| (temp & (1 << 12))
			|| (temp & (1 << 14))
			|| (temp & (1 << 15))
			|| (temp & (1 << 16))
			|| (temp & (1 << 17))
			)
		{
			gb32960_can_set_max_sos_level(2);
		}
		else if ((temp & (1 << 4))
			|| (temp & (1 << 8))
			|| (temp & (1 << 9))
			)
		{
			gb32960_can_set_max_sos_level(1);
		}
		else
		{
			gb32960_can_set_max_sos_level(0);
		}
	}
	else if (get_engine_type() == EngineType_EM26)
	{
		temp = gb32960_can_get_sos_flag();
		if (0)
		{
			gb32960_can_set_max_sos_level(3);
		}
		else if ((temp & (1 << 11))
			|| (temp & (1 << 13))			
			|| (temp & (1 << 14))
			|| (temp & (1 << 15))
			|| (temp & (1 << 17))
			|| (temp & (1 << 18))
			)
		{
			gb32960_can_set_max_sos_level(2);
		}
		else if (
				(temp & (1 << 0))
			||	(temp & (1 << 1))
			|| 	(temp & (1 << 2))
			||  (temp & (1 << 3))
			|| 	(temp & (1 << 4))		
			|| 	(temp & (1 << 5))
			||  (temp & (1 << 6))		
			|| 	(temp & (1 << 7))
			||  (temp & (1 << 8))
			||  (temp & (1 << 9))
			||  (temp & (1 << 10))
			||  (temp & (1 << 12))
			||  (temp & (1 << 16))
			)
		{
			gb32960_can_set_max_sos_level(1);
		}
		else
		{
			gb32960_can_set_max_sos_level(0);
		}
		
		if(s_23A_flag > gb32960_can_get_max_sos_level())
		{
			gb32960_can_set_max_sos_level(s_23A_flag);
		}
	}
	else if (get_engine_type() == EngineType_EV18)
	{
		temp = gb32960_can_get_sos_flag();
		if (0)
		{
			gb32960_can_set_max_sos_level(3);
		}
		else if ((temp & (1 << 11))
			|| (temp & (1 << 13))			
			|| (temp & (1 << 14))
			|| (temp & (1 << 15))
			|| (temp & (1 << 17))
			|| (temp & (1 << 18))		
			)
		{
			gb32960_can_set_max_sos_level(2);
		}
		else if ((temp & (1 << 0))
			|| (temp & (1 << 1))
			|| (temp & (1 << 2))		
			|| (temp & (1 << 3))
			|| (temp & (1 << 4))
			|| (temp & (1 << 5))		
			|| (temp & (1 << 6))
			|| (temp & (1 << 7))		
			|| (temp & (1 << 8))
			|| (temp & (1 << 9))
			|| (temp & (1 << 10))
			|| (temp & (1 << 12))
			|| (temp & (1 << 16))
			)
		{
			gb32960_can_set_max_sos_level(1);
		}
		else
		{
			gb32960_can_set_max_sos_level(0);
		}
		if((temp & 0xA6)&&(s_180CD4F4_flag > gb32960_can_get_max_sos_level()))
		{
			gb32960_can_set_max_sos_level(s_180CD4F4_flag);
		}
	}

	if (gb32960_can_get_sos_test())
	{
		//持续四秒都没收到报警标志 则清除测试报警位
		for (temp = 0; temp < 19; temp++)
		{
			if ((stGB32960Param.sos_flag & (1 << temp)))
			{
				test_sos_time[temp] = GetRunTime();
			}

			if ((GetRunTime() - test_sos_time[temp]) >= 4)
			{
				sos_test_flag_val &= (~(1 << temp));
			}
		}
	}

}

void gb32960_fresh(void)
{
	unsigned int i = 0;
	unsigned int j = 0;

	if ((gb32960_get_sys_run_time() - stGB32960Time.car_status_time) >= 8)
	{
		gb32960_can_set_car_status(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.charge_status_time) >= 8)
	{
		gb32960_can_set_charge_status(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.run_mode_time) >= 8)
	{
		gb32960_can_set_run_mode(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.speed_time) >= 8)
	{
		gb32960_can_set_speed(0xFFFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.milage_time) >= 8)
	{
		//gb32960_can_set_milage(0xFFFFFFFF);
	}


	if ((gb32960_get_sys_run_time() - stGB32960Time.total_vol_time) >= 8)
	{
		gb32960_can_set_total_dianya(0xFFFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.total_dianliu_time) >= 8)
	{
		gb32960_can_set_total_dianliu(0xFFFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.soc_time) >= 8)
	{
		gb32960_can_set_soc(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.dc_dc_status_time) >= 8)
	{
		gb32960_can_set_dc_dc_status(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.dangwei_time) >= 8)
	{
		gb32960_can_set_dangwei(0xFF,1,1);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.jueyuandianzu_time) >= 8)
	{
		gb32960_can_set_jueyuandianzu(0xFFFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.jiasutaban_val_time) >= 8)
	{
		gb32960_can_set_jiasutaban_val(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.zhidongtaban_status_time) >= 8)
	{
		gb32960_can_set_zhidongtaban_status(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.qddjdata_time) >= 8)
	{
		for (i = 0; i < MAX_DJ_CNT; i++)
		{
			gb32960_can_set_qddj_status(i,0xFF);
			gb32960_can_set_qddj_controller_wendu(i,0xFF);
			gb32960_can_set_qddj_controller_dianliu(i,0xFFFF);
			gb32960_can_set_qddj_rpm(i,0xFFFF);
			gb32960_can_set_qddj_zhuanju(i,0xFFFF);
			gb32960_can_set_qddj_wendu(i,0xFF);
			gb32960_can_set_qddj_input_dianya(i,0xFFFF);
		}
	}


	if ((gb32960_get_sys_run_time() - stGB32960Time.stCNXTData_time) >= 8)
	{
		for (i = 0; i < MAX_DIANCHI_CNT; i++)
		{
			gb32960_can_set_cnxt_dianliu(i,0xFFFF);
			gb32960_can_set_cnxt_dianya(i,0xFFFF);

			for (j = 0; j < DANTI_DIANCHI_CNT_EVERY_DIANCHI; j++)
			{
				gb32960_can_set_cnxt_dantidianchi_dianya(i,j,0xFFFF);
				gb32960_can_set_cnxt_wendu_tanzhen_wendu(i,j,0xFF);
			}
		}

		gb32960_can_set_max_dianya_sub_system_id(0xFF);
		gb32960_can_set_min_dianya_sub_system_id(0xFF);

		gb32960_can_set_max_dianya_danti_id(0xFF);
		gb32960_can_set_min_dianya_danti_id(0xFF);

		gb32960_can_set_max_danti_dianya_val(0xFFFF);
		gb32960_can_set_min_danti_dianya_val(0xFFFF);

		gb32960_can_set_max_wendu_val(0xFF);
		gb32960_can_set_min_wendu_val(0xFF);

		gb32960_can_set_max_wendu_sub_system_id(0xFF);
		gb32960_can_set_max_wendu_tanzhen_id(0xFF);
		gb32960_can_set_min_wendu_sub_system_id(0xFF);
		gb32960_can_set_min_wendu_tanzhen_id(0xFF);
	}

	if ((gb32960_get_sys_run_time() - stGB32960Time.other_dtc_time) >= 8)
	{
		gb32960_can_set_other_dtc_cnt(0);
	}
}

void GB32960Param_Init(void)
{
#if 1
	unsigned int i = 0;

	memset(&stGB32960Param,0x00,sizeof(stGB32960Param));
	gb32960_can_set_dianchi_cnt(1);
	gb32960_can_set_qddj_cnt(1);
	gb32960_can_set_rldc_tanzhen_cnt(1);

	gb32960_can_set_charge_set_dtc_cnt(0);
	gb32960_can_set_qddj_dtc_cnt(0);
	gb32960_can_set_ecu_dtc_cnt(0);
	gb32960_can_set_other_dtc_cnt(0);
	gb32960_can_set_sos_flag(0);

	//可充电储能系统
	for (i = 0; i < MAX_DIANCHI_CNT; i++)
	{
		gb32960_can_set_cnxt_id(i,i + 1);
		stGB32960Param.stCNXTData[i].benzhen_start_id = 1;
		gb32960_can_set_cnxt_wendu_tanzhen_cnt(i,0);
		gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(i,0);
	}
#else
	unsigned int i = 0;

	//整车数据
	gb32960_can_set_car_status(0x02);			//熄火
	gb32960_can_set_charge_status(0x03);
	gb32960_can_set_run_mode(0x01);
	gb32960_can_set_speed(0);
	gb32960_can_set_total_dianya(0);
	gb32960_can_set_total_dianliu(0);
	gb32960_can_set_soc(0);
	gb32960_can_set_dc_dc_status(0x02);
	gb32960_can_set_dangwei(0x0F,0,0);
	gb32960_can_set_jueyuandianzu(0);
	gb32960_can_set_jiasutaban_val(0);
	gb32960_can_set_zhidongtaban_status(0);
	

	stGB32960Param.max_vol_id = 1;
	stGB32960Param.max_dianya_danti_id = 1;
	stGB32960Param.max_vol_val = 2;

	stGB32960Param.low_vol_id = 1;
	stGB32960Param.low_danti_id = 1;
	stGB32960Param.low_vol_val = 1;

	stGB32960Param.max_wend_system_id = 1;
	stGB32960Param.max_wend_tanzhen_id = 1;
	stGB32960Param.max_wendu_val = 2;

	stGB32960Param.low_wendu_system_id = 1;
	stGB32960Param.low_wendu_tanzhen_id = 1;
	stGB32960Param.low_wendu_val = 1;

	//发动机数据
	gb32960_can_set_ecu_status(0x02);
	gb32960_can_set_ecu_quzhou_rpm(0);
	gb32960_can_set_ecu_ranliao_xiaohaolv(0);
	

	//驱动电机
	gb32960_can_set_qddj_cnt(1);
	for (i = 0; i < MAX_DJ_CNT; i++)
	{
		gb32960_can_set_qddj_xuhao(i,i+1);
		gb32960_can_set_qddj_status(i,3);
		gb32960_can_set_qddj_rpm(i,0x0);

		gb32960_can_set_qddj_zhuanju(i,0x0);

		gb32960_can_set_qddj_controller_wendu(i,0 + 40);
		gb32960_can_set_qddj_controller_dianliu(i,0x0);
		gb32960_can_set_qddj_wendu(i,0 + 40);
		gb32960_can_set_qddj_input_dianya(i,150);
	}

	//燃料电池信息
	gb32960_can_set_rldc_dianliu(0);
	gb32960_can_set_rldc_dianya(0);
	gb32960_can_set_rldc_xiaohaolv(0);
	gb32960_can_set_rldc_tanzhen_cnt(1);
	for (i = 0; i < MAX_TAN_ZHEN_CNT; i++)
	{
		gb32960_can_set_rldc_tanzhen_wendu(i,0);
	}
	gb32960_can_set_max_wendu_in_qi_system(100);
	gb32960_can_set_rldc_tanzhen_cnt(1);
	gb32960_can_set_max_wendu_tanzhen_id_in_qi_system(1);
	gb32960_can_set_max_qingqi_sensor_id(1);
	gb32960_can_set_max_qingqi_yali_sensor_id(1);
	gb32960_can_set_gaoya_dc_dc_status(1);

	//报警数据
	//gb32960_can_set_max_sos_level(2);
	//gb32960_can_set_sos_flag(0xFFFFFFFF);
	//gb32960_can_set_charge_set_dtc_cnt(1);
	//gb32960_can_set_charge_set_dtc_list(0,0x33333333);


	gb32960_can_set_dianchi_cnt(1);

	//可充电储能系统
	for (i = 0; i < MAX_DIANCHI_CNT; i++)
	{
		gb32960_can_set_cnxt_id(i,i + 1);
		stGB32960Param.stCNXTData[i].benzhen_start_id = 1;

		gb32960_can_set_cnxt_wendu_tanzhen_cnt(i,1);

		gb32960_can_set_cnxt_wendu_tanzhen_wendu(i,0,40+20);
		gb32960_can_set_cnxt_wendu_tanzhen_wendu(i,1,40+20);
		gb32960_can_set_cnxt_wendu_tanzhen_wendu(i,2,40+20);

		gb32960_can_set_cnxt_benzhen_dantidianchi_total_cnt(i,1);
		gb32960_can_set_cnxt_dantidianchi_dianya(i,0,3300);
		gb32960_can_set_cnxt_dantidianchi_dianya(i,1,3300);
		gb32960_can_set_cnxt_dantidianchi_dianya(i,2,3300);
	}
	//空调系统
	//EM26 车型制冷系统数据
	set_em26_fan_ls_status(0x0);					//低速风扇（或冷却风扇）工作状态
	set_em26_fan_hs_status(0x0);					//高速风扇（或冷凝风扇）工作状态
	set_em26_air_cond_status(0x0);					//空调基本状态
	set_em26_air_cond_speed(0x0);				//空调压缩机转速
	
	//EV18 车型制冷系统数据
	set_ev18_fan_ls_status(0x0);					//低速风扇（或冷却风扇）工作状态
	set_ev18_fan_hs_status(0x0);					//高速风扇（或冷凝风扇）工作状态
	set_ev18_air_cond_speed(0x0);				//空调压缩机转速
	set_ev18_air_cond_status(0x0);					//空调压缩机开关
	set_ev18_air_cond_ctrl_speed(0x0);			//压缩机控制器转速反馈
	set_ev18_air_cond_ctrl_status(0x0);			//压缩机控制器工作状态
	
	//测试
	stDefProtocolStatus.sddf_dianchibao_cnt = 3;
#endif
}


typedef struct tagGBDataInfo
{
	INT8U *pVIN;
	INT8U Way;

	INT16U *p_login_flow;
	unsigned char login_flow_cnt;
}GBDataInfo;


GBDataInfo sGBDataInfo = {0};

void GB32960_Init(void *pvin,INT8U way,INT16U *pLoginFlow,unsigned int flow_cnt)
{
	sGBDataInfo.pVIN = pvin;
	sGBDataInfo.Way = way;

	sGBDataInfo.p_login_flow = pLoginFlow;
	sGBDataInfo.login_flow_cnt = flow_cnt;

	GB32960Param_Init();
}

//封包
unsigned int _eDataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,INT8U ack_flag,void *pVIN,void *pDataUnit,INT16U usDataUnitLen)
{
	INT16U len = 0;
	INT8U *pbuf = (INT8U *)pOutBuf;
	INT8U xor = 0;

	if (pbuf == 0)
	{
		return 0;
	}
	//判断缓冲区大小
	//起始符(2B)+命令字(1B)+应答标志(1B)+VIN(17B)+加密方式(1B)+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
	if (usOutBufSize < (2+1+1+GB_LEN_VIN+1+2+usDataUnitLen+1))
	{
		return 0;
	}
	//起始字符串 2B
	pbuf[len++] = GBHEAD0;
	pbuf[len++] = GBHEAD1;

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

	//数据加密方式	1B
	pbuf[len++] = 0x03;//sGBDataInfo.Way;

	//数据单元长度	2B
	pbuf[len++] = usDataUnitLen >> 8;
	pbuf[len++] = usDataUnitLen;

	//数据单元
	if (pDataUnit && usDataUnitLen)
	{
		memcpy(pbuf + len,pDataUnit,usDataUnitLen);
		len += usDataUnitLen;
	}

	//异或校验
	xor = XOR_Check(pbuf + 2,len - 2);
	pbuf[len++] = xor;
	return len;
}

//封包
unsigned int _xDataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,INT8U ack_flag,void *pVIN,void *pDataUnit,INT16U usDataUnitLen)
{
	INT16U len = 0;
	INT8U *pbuf = (INT8U *)pOutBuf;
	INT8U xor = 0;

	if (pbuf == 0)
	{
		return 0;
	}
	//判断缓冲区大小
	//起始符(2B)+命令字(1B)+应答标志(1B)+VIN(17B)+加密方式(1B)+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)
	if (usOutBufSize < (2+1+1+GB_LEN_VIN+1+2+usDataUnitLen+1))
	{
		return 0;
	}
	//起始字符串 2B
	pbuf[len++] = GBHEAD0;
	pbuf[len++] = GBHEAD1;

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

	//数据加密方式	1B
	pbuf[len++] = sGBDataInfo.Way;

	//数据单元长度	2B
	pbuf[len++] = usDataUnitLen >> 8;
	pbuf[len++] = usDataUnitLen;

	//数据单元
	if (pDataUnit && usDataUnitLen)
	{
		memcpy(pbuf + len,pDataUnit,usDataUnitLen);
		len += usDataUnitLen;
	}

	//异或校验
	xor = XOR_Check(pbuf + 2,len - 2);
	pbuf[len++] = xor;

	return len;
}

unsigned int xDataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,void *pDataUnit,INT16U usDataUnitLen)
{
	return _xDataBag(pOutBuf,usOutBufSize,cmd,0xFE,sGBDataInfo.pVIN,pDataUnit,usDataUnitLen);
}

unsigned int GB32960_LoginPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,
			INT8U time[6],void *iccid,INT8U charge_cnt,
			void *pCode,INT8U code_len)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int templen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = (INT8U *)pOutBuf + GB32960_CONTENT_OFFSET;

	//时间
	memcpy(ptr,time,6);
	rlen += 6;

	//流水
	if (sGBDataInfo.p_login_flow && (linkid < sGBDataInfo.login_flow_cnt))
	{
		*(sGBDataInfo.p_login_flow + linkid) = (*(sGBDataInfo.p_login_flow + linkid)) + 1;
		templen = *(sGBDataInfo.p_login_flow + linkid);
	}
	else
	{
		templen = 1;
	}

	ptr[rlen++] = templen >> 8;
	ptr[rlen++] = templen;

	//ICCID
	if (iccid)
	{
		memcpy(ptr + rlen,iccid,GB_LEN_ICCID);
	}
	rlen += GB_LEN_ICCID;

	//可充电储能子系统个数
	ptr[rlen++] = gb32960_can_get_dianchi_cnt();//charge_cnt;

	if (code_len > 50)
	{
		code_len = 0;
	}
	ptr[rlen++] = code_len;
	//可充电储能系统编码长度
	if (pCode && code_len)
	{
		memcpy(ptr+rlen,pCode,code_len);
	}
	rlen += code_len;

	return xDataBag(pOutBuf,outbuflen,GB32960_LOGIN_CMD,ptr,rlen);
}



unsigned int GB32960_LogoutPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,INT8U time[6])
{
	INT8U txt[16] = {0};
	unsigned int rlen = 0;
	unsigned int templen = 0;

	memcpy(txt,time,6);
	rlen += 6;

	//流水
	if (sGBDataInfo.p_login_flow && (linkid < sGBDataInfo.login_flow_cnt))
	{
		templen = *(sGBDataInfo.p_login_flow + linkid);
	}
	else
	{
		templen = 1;
	}
	txt[rlen++] = templen >> 8;
	txt[rlen++] = templen;

	return xDataBag(pOutBuf,outbuflen,GB32960_LOGOUT_CMD,txt,rlen);
}

//心跳
unsigned int GB32960_HeartBeatPack(void *pOutBuf,unsigned int outbuflen)
{
	return xDataBag(pOutBuf,outbuflen,GB32960_HEART_BEAT_CMD,0,0);
}

//校时
unsigned int GB32960_JSPack(void *pOutBuf,unsigned int outbuflen)
{
	return xDataBag(pOutBuf,outbuflen,GB32960_JS_CMD,0,0);
}

#define MAX_STAR_NUM 32

typedef struct tagGPS
{
	INT8U ucGpsMode;		//模式指示(A=自主定位，D=差分，E=估算，N=数据无效)
	INT8U ucPositionStatus;	//定位状态'A' 'V'
	INT32U position_time_cnt;		//定位时长

	INT32U ulLat;		//纬度
	INT8U ucNS;		//南北纬
	INT32U ulLong;		//经度
	INT8U ucEW;		//东西经
	INT16U usGpsSpeed;	//单位:0.1Km/h  速度扩大10倍
	INT16U usAngle;		//方向角
	INT16U usGaodu;		//高度

	INT8U start_num;		//定位星数

	INT8U gsv_star_num;		//可视卫星数
	INT8U gsv_star_no[MAX_STAR_NUM];		//可视卫星号
	INT8U gsv_star_yj[MAX_STAR_NUM];		//可视卫仰角
	INT16U gsv_star_fwj[MAX_STAR_NUM];		//可视卫方位角
	INT8U gsv_star_xzb[MAX_STAR_NUM];		//可视卫星信噪比

	GpsAntStateE eAntStatus;	//天线状态

	INT16U usPDOP;		//扩大10倍 综合位置精度因子(0.5 - 99.9)
	INT16U usHDOP;		//扩大10倍 水平精度因子(0.5 - 99.9)
	INT16U usVDOP;		//扩大10倍 垂直精度因子(0.5 - 99.9)

	INT8U year;		//年
	INT8U month;	//月
	INT8U day;		//日
	INT8U hour;		//时
	INT8U min;		//分
	INT8U sec;		//秒
}GPS;

static GPS zc_stGPS = {0};

unsigned int get_gb32960_zuocang_data_unit(void *pOut,unsigned int outbuflen,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	static uint16_t zuocang_data_num = 0;
	INT32U temp_gps_data = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}
	
	send_msg(S_MSG_GET_GPS,(void *)(&zc_stGPS),0,0);

	//采集时间
	zuocang_data_num++;
	if(zuocang_data_num == 0)
	{
		zuocang_data_num = 1;
	}
	if (outbuflen < 6) return 0;
	if (time) memcpy(ptr,time,6);
	rlen += 6;
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = ZUOCANG_DATA_INFO;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = zuocang_data_num >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = zuocang_data_num;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x02;//协议版本
	
	//GPS数据，20字节
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = pos_status;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat;
	
	temp_gps_data = (INT32U)send_msg(S_MSG_GET_GPS_SPEED,0,0,0);
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp_gps_data/10;//速度
	
	temp_gps_data = (INT32U)send_msg(S_MSG_GET_ANGLE,0,0,0);
	temp_gps_data = temp_gps_data/128;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp_gps_data;//方向 
	
	temp_gps_data = (INT32U)send_msg(S_MSG_GET_GAODU,0,0,0);
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = zc_stGPS.usGaodu >>8;//高程
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = zc_stGPS.usGaodu;
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;//坡度 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;//加速度
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;//GSM 信号强度
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;//基站 LAC
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;//基站编号
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = 0x0;
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_key_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ready_status();//gb32960_can_get_car_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_hand_brk_status();
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_steer_wheel_angle()>>8;//方向盘转角
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_steer_wheel_angle();
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage();
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_power_ctrl_allow();//上下电允许指令
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_dc_dc_status();//DCDC 工作状态

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianya() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianya();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianliu() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianliu();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_soc();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_zhufujiechuqi_status();//主负接触器状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_charge_status();
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_bms_wakeup_status();//BMS 唤醒状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_vcu_wakeup_status();//VCU唤醒其他控制器电源
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ce_door_switch();//侧滑门开关
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_back_door_switch();//后背门开关
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_l_f_door_switch();//
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_r_f_door_switch();//副驾驶侧前门开关
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_l_f_door_status();//
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_center_door_status();//中央门控锁开关
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_door_lock_status();//门锁状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_cond_switch();//压缩机工作使能 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_cond_switch_status();//压缩机启停状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_cond_far_func();//空调数据长度 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ac_quire_status();//AC 请求 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_air_work_status();//空调工作状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_xunhuan_mode_status();//内外循环状态 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_shaung_mode_status();//快速除霜模式
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_ptc_work_status();//PTC 工作状态
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_feng_mode_status();//吹风模式  
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_feng_dangwei_status();//风量显示 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_wendu_dangwei_status();//温度显示
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_lose_power_fail_status();//下电失败状态 
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = get_lock_door_fail_status();//上锁失败状态
	
	return rlen;
	
}

//国标数据单元
unsigned int get_gb32960_data_unit(void *pOut,unsigned int outbuflen,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned int rlen_jizhi = 0;
	unsigned char *ptr = (unsigned char *)pOut;
	unsigned int temp = 0;
	unsigned int temp1 = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	
	unsigned short max_dianya_system_id = 0;
	unsigned short min_dianya_system_id = 0;
	
	unsigned short max_dianya_danti_id = 0;
	unsigned short min_dianya_danti_id = 0;
	
	unsigned short max_dianya_danti_value = 0;
	unsigned short min_dianya_danti_value = 0;
	
	unsigned short max_wendu_system_id = 0;
	unsigned short min_wendu_system_id = 0;
	
	unsigned short max_wendu_danti_id = 0;
	unsigned short min_wendu_danti_id = 0;
	
	unsigned short max_wendu_danti_value = 0;
	unsigned short min_wendu_danti_value = 0;

	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}

	//采集时间
	if (outbuflen < 6) return 0;
	if (time) memcpy(ptr,time,6);
	rlen += 6;
#if 1
	//整车数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = ZC_DATA_INFO;

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_car_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_charge_status();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_run_mode();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_speed() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_speed();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_milage();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianya() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianya();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianliu() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_total_dianliu();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_soc();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_dc_dc_status();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_dangwei();


	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_jueyuandianzu() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_jueyuandianzu();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_jiasutaban_val();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_zhidongtaban_status();
#endif

#if 1
	//驱动电机数据
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = QDDJ_INFO;

	temp = gb32960_can_get_qddj_cnt();
	if (temp > MAX_DJ_CNT)
	{
		temp = MAX_DJ_CNT;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;		//驱动电机个数

	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_xuhao(i);				//电机序号
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_status(i);					//电机状态
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_controller_wendu(i);		//控制器温度

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_rpm(i) >> 8;				//转速
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_rpm(i);


		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_zhuanju(i) >> 8;			//转矩
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_zhuanju(i);

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_wendu(i);			//电机温度

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_input_dianya(i) >> 8;			//输入电压
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_input_dianya(i);

		temp1 = gb32960_can_get_qddj_controller_dianliu(i);
		if ((temp1 > 10000) && (gb32960_can_get_total_dianliu() > 10000) && (temp1 > gb32960_can_get_total_dianliu()))
		{
			temp1 = gb32960_can_get_total_dianliu();
		}
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1 >> 8;//控制器电流
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1;
		
	}
#endif

#if 1
	if ((gb32960_can_get_run_mode() != 0x01) && (gb32960_can_get_run_mode() != 0xFF))	//非纯电模式下需要燃料电池
	{
		//燃料电池
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = RLDC_INFO;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_dianya() >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_dianya();
		
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_dianliu() >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_dianliu();

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_xiaohaolv() >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_rldc_xiaohaolv();

		temp = gb32960_can_get_rldc_tanzhen_cnt();
		if (temp > MAX_TAN_ZHEN_CNT)
		{
			temp = MAX_TAN_ZHEN_CNT;
		}
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp >> 8;		//探针总数
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp;

		for (i = 0; i < temp; i++)
		{
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = gb32960_can_get_rldc_tanzhen_wendu(i);
		}
		
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_wendu_in_qi_system() >> 8;		//氢系统中最高温度
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_wendu_in_qi_system();
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_wendu_tanzhen_id_in_qi_system();
		
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_nongdu() >> 8; 	//最高氢气浓度
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_nongdu();
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_sensor_id();
		
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali() >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali();
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_max_qingqi_yali_sensor_id();
		
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_gaoya_dc_dc_status();
	}
#endif

#if 1
	//位置信息
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = CAR_POS_INFO;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = pos_status;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lon;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = lat;
#endif
#if 1
	rlen_jizhi = rlen;
	//极值信息
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = JIZHI_INFO;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_dianya_sub_system_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_dianya_danti_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_danti_dianya_val() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_danti_dianya_val();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_dianya_sub_system_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_dianya_danti_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_danti_dianya_val() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_danti_dianya_val();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_wendu_sub_system_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_wendu_tanzhen_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_wendu_val();

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_wendu_sub_system_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_wendu_tanzhen_id();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_min_wendu_val();
#endif

#if 1
	//报警信息
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = SOS_INFO;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_max_sos_level();
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 24;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 16;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_sos_flag() >> 8;
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = gb32960_can_get_sos_flag();

	temp = gb32960_can_get_charge_set_dtc_cnt();
	if (temp == 0xFF) temp =  0;
	if (temp > MAX_CAN_CHARGE_DTC_CNT)
	{
		temp = MAX_CAN_CHARGE_DTC_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{	
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i) >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_charge_set_dtc_list(i);
	}
	
	temp = gb32960_can_get_qddj_dtc_cnt();
	if (temp == 0xFF) temp = 0;
	if (temp > MAX_QDDJ_DTC_CNT)
	{
		temp = MAX_QDDJ_DTC_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i) >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_qddj_dtc_list(i);
	}
	

	temp = gb32960_can_get_ecu_dtc_cnt();
	if (temp == 0xFF) temp = 0;
	if (temp > MAX_ECU_DTC_CNT)
	{
		temp = MAX_ECU_DTC_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i) >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_ecu_dtc_list(i);
	}
	

	temp = gb32960_can_get_other_dtc_cnt();
	if (temp == 0xFF) temp = 0;
	if (temp > MAX_OTHER_DTC_CNT)
	{
		temp = MAX_OTHER_DTC_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;

	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 24;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 16;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i) >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = gb32960_can_get_other_dtc_list(i);
	}

#endif
#if 1
	//电压数据
	max_dianya_danti_value = stGB32960Param.stCNXTData[0].danti_dianchi_dianya[0];
	min_dianya_danti_value = stGB32960Param.stCNXTData[0].danti_dianchi_dianya[0];
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = VOL_INFO;
	temp = gb32960_can_get_dianchi_cnt();
	if (temp > MAX_DIANCHI_CNT)
	{
		temp = MAX_DIANCHI_CNT;
	}
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;
	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[i].id;

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianya >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianya;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianliu >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[0].dianliu;

		temp1 = stGB32960Param.stCNXTData[i].benzhen_danti_cnt;
		if (temp1 == 0xFF) temp1 = 0;

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1 >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1;

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[i].benzhen_start_id >> 8;
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[i].benzhen_start_id;

		temp1 = stGB32960Param.stCNXTData[i].benzhen_danti_cnt;
		if (temp1 == 0xFF) temp1 = 0;
		if (temp1 > DANTI_DIANCHI_CNT_EVERY_DIANCHI)
		{
			temp1 = DANTI_DIANCHI_CNT_EVERY_DIANCHI;
		}

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1;
		for (j = 0; j < temp1; j++)
		{
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j] >> 8;
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j];
			
			if(stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j] > max_dianya_danti_value)
			{
				max_dianya_danti_value = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j];
				max_dianya_system_id = i;
				max_dianya_danti_id = j;
			}
			
			if(stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j] < min_dianya_danti_value)
			{
				min_dianya_danti_value = stGB32960Param.stCNXTData[i].danti_dianchi_dianya[j];
				min_dianya_system_id = i;
				min_dianya_danti_id = j;
			}
		}
	}
	
	gb32960_can_set_max_dianya_sub_system_id(max_dianya_system_id + 1);
	gb32960_can_set_max_dianya_danti_id(max_dianya_danti_id + 1);
	gb32960_can_set_max_danti_dianya_val(max_dianya_danti_value);


	gb32960_can_set_min_dianya_sub_system_id(min_dianya_system_id + 1);
	gb32960_can_set_min_dianya_danti_id(min_dianya_danti_id + 1);
	gb32960_can_set_min_danti_dianya_val(min_dianya_danti_value);
#endif

#if 1
	//温度数据
	max_wendu_danti_value = stGB32960Param.stCNXTData[0].wendu[0];
	min_wendu_danti_value = stGB32960Param.stCNXTData[0].wendu[0];
	
	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = TEMP_INFO;

	temp = gb32960_can_get_dianchi_cnt();
	if (temp > MAX_DIANCHI_CNT)
	{
		temp = MAX_DIANCHI_CNT;
	}

	if (rlen >= outbuflen) return rlen;
	ptr[rlen++] = temp;
	for (i = 0; i < temp; i++)
	{
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = stGB32960Param.stCNXTData[i].id;

		temp1 = stGB32960Param.stCNXTData[i].wendu_tanzhen_cnt;
		if (temp1 == 0xFFFF) temp1 = 0;
		if (temp1 > WENDU_TANZHEN_CNT_EVERY_DIANCHI)
		{
			temp1 = WENDU_TANZHEN_CNT_EVERY_DIANCHI;
		}

		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1 >> 8;		//探针总数
		if (rlen >= outbuflen) return rlen;
		ptr[rlen++] = temp1;

		for (j = 0; j < temp1; j++)
		{
			if (rlen >= outbuflen) return rlen;
			ptr[rlen++] = stGB32960Param.stCNXTData[i].wendu[j];
			if(stGB32960Param.stCNXTData[i].wendu[j] > max_wendu_danti_value)
			{
				max_wendu_danti_value = stGB32960Param.stCNXTData[i].wendu[j];
				max_wendu_system_id = i;
				max_wendu_danti_id = j;
			}
			if(stGB32960Param.stCNXTData[i].wendu[j] < min_wendu_danti_value)
			{
				min_wendu_danti_value = stGB32960Param.stCNXTData[i].wendu[j];
				min_wendu_system_id = i;
				min_wendu_danti_id = j;
			}
		}		
	}
	
	gb32960_can_set_max_wendu_sub_system_id(max_wendu_system_id + 1);
	gb32960_can_set_max_wendu_tanzhen_id(max_wendu_danti_id + 1);
	gb32960_can_set_max_wendu_val(max_wendu_danti_value);

	gb32960_can_set_min_wendu_sub_system_id(min_wendu_system_id + 1);
	gb32960_can_set_min_wendu_tanzhen_id(min_wendu_danti_id + 1);
	gb32960_can_set_min_wendu_val(min_wendu_danti_value);
#endif

#if 1
	//极值信息
	ptr[rlen_jizhi++] = JIZHI_INFO;
	ptr[rlen_jizhi++] = gb32960_can_get_max_dianya_sub_system_id();
	ptr[rlen_jizhi++] = gb32960_can_get_max_dianya_danti_id();
	ptr[rlen_jizhi++] = gb32960_can_get_max_danti_dianya_val() >> 8;
	ptr[rlen_jizhi++] = gb32960_can_get_max_danti_dianya_val();

	ptr[rlen_jizhi++] = gb32960_can_get_min_dianya_sub_system_id();
	ptr[rlen_jizhi++] = gb32960_can_get_min_dianya_danti_id();
	ptr[rlen_jizhi++] = gb32960_can_get_min_danti_dianya_val() >> 8;
	ptr[rlen_jizhi++] = gb32960_can_get_min_danti_dianya_val();

	ptr[rlen_jizhi++] = gb32960_can_get_max_wendu_sub_system_id();
	ptr[rlen_jizhi++] = gb32960_can_get_max_wendu_tanzhen_id();
	ptr[rlen_jizhi++] = gb32960_can_get_max_wendu_val();

	ptr[rlen_jizhi++] = gb32960_can_get_min_wendu_sub_system_id();
	ptr[rlen_jizhi++] = gb32960_can_get_min_wendu_tanzhen_id();
	ptr[rlen_jizhi++] = gb32960_can_get_min_wendu_val();
#endif
	
	return rlen;
}



unsigned int _sddf_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		rlen += get_gb32960_data_unit(ptr,cap,time,pos_status,lon,lat);
		if (rlen < cap) rlen += get_sddf_acp_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}

unsigned int _D78_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;


	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		rlen += get_gb32960_data_unit(ptr,cap,time,pos_status,lon,lat);
		if (rlen < cap) rlen += get_d78_zdy_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}

unsigned int _EV18_xGB32960_zuocang_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;


	rlen += get_gb32960_zuocang_data_unit(ptr,outbuflen - GB32960_CONTENT_OFFSET - 1,time,pos_status,lon,lat);

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}

unsigned int _EV18_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;


	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		rlen += get_gb32960_data_unit(ptr,cap,time,pos_status,lon,lat);
		if (rlen < cap) rlen += get_ev18_zdy_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}

unsigned int _EM26_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;


	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		rlen += get_gb32960_data_unit(ptr,cap,time,pos_status,lon,lat);
		if (rlen < cap) rlen += get_em26_zdy_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}


unsigned int _xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,INT8U cmd,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	rlen += get_gb32960_data_unit(ptr,outbuflen - GB32960_CONTENT_OFFSET - 1,time,pos_status,lon,lat);

	return _xDataBag(pOut,outbuflen,cmd,0xFE,pVIN,ptr,rlen);
}

unsigned int xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _xGB32960_RealDataPack(pOut,outbuflen,GB32960_REAL_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _xGB32960_RealDataPack(pOut,outbuflen,GB32960_BF_DATA_CMD,pVIN,time,pos_status, lon,lat);
}


unsigned int D78_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _D78_xGB32960_RealDataPack(pOut,outbuflen,GB32960_REAL_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int D78_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _D78_xGB32960_RealDataPack(pOut,outbuflen,GB32960_BF_DATA_CMD,pVIN,time,pos_status, lon,lat);
}
unsigned int EM26_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _EM26_xGB32960_RealDataPack(pOut,outbuflen,GB32960_REAL_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int EM26_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _EM26_xGB32960_RealDataPack(pOut,outbuflen,GB32960_BF_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int EV18_xGB32960_zuocang_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _EV18_xGB32960_zuocang_RealDataPack(pOut,outbuflen,GB32960_ZC_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int EV18_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _EV18_xGB32960_RealDataPack(pOut,outbuflen,GB32960_REAL_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

unsigned int EV18_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _EV18_xGB32960_RealDataPack(pOut,outbuflen,GB32960_BF_DATA_CMD,pVIN,time,pos_status, lon,lat);
}

//时代电服
unsigned int sddf_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _sddf_xGB32960_RealDataPack(pOut,outbuflen,0xC0,pVIN,time,pos_status, lon,lat);
}

unsigned int sddf_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat)
{
	return _sddf_xGB32960_RealDataPack(pOut,outbuflen,0xC1,pVIN,time,pos_status, lon,lat);
}

unsigned int sddf_BasicDataPack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6])
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	if (outbuflen < (GB32960_CONTENT_OFFSET + 1 + 6)) return 0;
	if (time) memcpy(ptr,time,6);
	rlen += 6;

	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		if (rlen < cap) rlen += get_sddf_mbmu_sbmu_data(ptr + rlen,cap - rlen);
		if (rlen < cap) rlen += get_sddf_chongfangdian_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,0xC2,0xFE,pVIN,ptr,rlen);
}

unsigned int sddf_charge_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6])
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	if (outbuflen < (GB32960_CONTENT_OFFSET + 1 + 6)) return 0;
	if (time) memcpy(ptr,time,6);
	rlen += 6;

	{
		unsigned int cap = outbuflen - GB32960_CONTENT_OFFSET - 1;
		if (rlen < cap) rlen += get_sddf_charge_data(ptr + rlen,cap - rlen);
	}

	return _xDataBag(pOut,outbuflen,0xC2,0xFE,pVIN,ptr,rlen);
}

unsigned int sddf_ack_charge_limit_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6])
{
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	if ((pOut == 0) || (outbuflen < (GB32960_CONTENT_OFFSET + 1)))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	//采集时间
	if (outbuflen < (GB32960_CONTENT_OFFSET + 1 + 10)) return 0;
	if (time) memcpy(ptr,time,6);
	rlen += 6;

	ptr[rlen++] = 0x84;
	ptr[rlen++] = 1;
	ptr[rlen++] = stDefProtocolStatus.sddf_fact_chongdian_limit_soc;
	ptr[rlen++] = stDefProtocolStatus.sddf_fact_chongdian_limit_flag;

	return _xDataBag(pOut,outbuflen,0xC2,0xFE,pVIN,ptr,rlen);
}


unsigned int sddf_LoginPlatPack(void *pVIN,void *puser,void *psw,void *pOut,unsigned int outbuflen)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	//char t_user[12]="dpca00000000";
	static unsigned int s_df_c6_flow = 0;
	//AES128
	/*
	用户名：dpca
	密码：(国标时间6个byte+固定密码)用aes加密后的密文
	AES加密算法：AES128
	AES加密模式：ECB
	AES补码方式：PKCS5Padding
	测试环境固定密码aesKey:ddddppppccccaaaa
	*/
	//unsigned  char key[16] = "ddddppppccccaaaa";
	unsigned  char in[16]="000000dpca";
	unsigned  char out[16];
	
	if ((pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}
	ptr = (unsigned char *)pOut + GB32960_CONTENT_OFFSET;

	//时间 6B
	memcpy(ptr,get_rtc_time_ptr(),6);
	rlen += 6;
	
	//流水2B
	s_df_c6_flow++;
	ptr[rlen++] = s_df_c6_flow >> 8;
	ptr[rlen++] = s_df_c6_flow;

	//user
	if (puser)
	{
		if (strlen((char *)puser) >= 12)
		{
			memcpy(ptr + rlen,puser,12);
			rlen += 12;
		}
		else						//向后补0 补足12位
		{
			memcpy(ptr + rlen,puser,strlen((char *)puser));
			rlen += strlen((char *)puser);
			memset(ptr + rlen,0,12 - strlen((char *)puser));
			rlen += (12 - strlen((char *)puser));
		}
	}
	else
	{
		memcpy(ptr + rlen,"dpca00000000",12);
		rlen += 12;
	}
	
	//拼接时间和pdca
	memcpy(in,get_rtc_time_ptr(),6);
	//DBG_PrintBuf(LEVEL0,"init in:",in,16);
	//补齐16byte
	PKCS7Padding(in,10);
	//开始加密
	if (psw)
	{
		AES128_ECB_encrypt(in, psw, out);
	}
	else
	{
		AES128_ECB_encrypt(in, "ddddppppccccaaaa", out);
	}
	
  //DBG_PrintBuf(LEVEL0,"\nin:",in,16);
  //DBG_PrintBuf(LEVEL0,"\nbuffer:",out,16);

	//password
	memcpy(&ptr[rlen],out,16);
	memset(&ptr[rlen+16],0,4);
	rlen += 20;
	
	//Encryption level 
	ptr[rlen++]=0x03;
	return _eDataBag(pOut,outbuflen,0x05,0xFE,pVIN,ptr,rlen);
}


void gb32960_moni_data_proc(void)
{
	unsigned int i = 0;
	//DRV_RUN_DATA_TYPE *drv_ptr = 0;
	unsigned int val_rand = 0;
	static unsigned int soc = 4100;
	unsigned char max_wd_val = 0;
	unsigned char min_wd_val = 0;
	unsigned char max_wd_id = 0;
	unsigned char min_wd_id = 0;

	unsigned int max_dy_val = 0;
	unsigned int min_dy_val = 0;
	unsigned char max_dy_id = 0;
	unsigned char min_dy_id = 0;


	static unsigned char s_moni_charge_status = 0x01;
	//static unsigned int s_charge_ok_cnt = 0;
	//drv_ptr = get_drv_run_data_ptr();

	srand(GetRunTime());
	val_rand = rand();
	moni_sddf_data(val_rand);

#if 1
		//整车数据
		gb32960_can_set_car_status(0x01);	//启动
		gb32960_can_set_charge_status(s_moni_charge_status);	//停车充电

		gb32960_can_set_run_mode(0x01);
		gb32960_can_set_speed(0);
		gb32960_can_set_milage(345720);
		gb32960_can_set_total_dianya(5620 + val_rand % 31); 	//563 - 565

		if (soc < 5000)
		{
			gb32960_can_set_total_dianliu(9000 + val_rand % 30);	//停车充电 小于-100A
			soc += 5;				//电量50以下
		}

		else if (soc < 9000)
		{
			gb32960_can_set_total_dianliu(9100 + val_rand % 20);	//停车充电 小于-90A
			soc += 3;
		}
		else if (soc < 10000)
		{	
			gb32960_can_set_total_dianliu(9960 + val_rand % 20);	//停车充电 小于-2A
			soc += 1;
		}
		else
		{
			gb32960_can_set_total_dianliu(10000 + val_rand % 30);		
			s_moni_charge_status = 0x04;		//充电完成

		}
		gb32960_can_set_soc(soc / 100);				//充电电量

		gb32960_can_set_dc_dc_status(0x01);		//工作
		//gb32960_can_get_dangwei();						//挡位

		gb32960_can_set_jueyuandianzu(8300 + val_rand % 900);	//8300 - 9200	
		gb32960_can_set_jiasutaban_val(0);
		gb32960_can_set_zhidongtaban_status(0);
#endif

#if 1
		//驱动电机数据
		gb32960_can_set_qddj_cnt(1);
		if (1)
		{
			gb32960_can_set_qddj_xuhao(0,1);				//电机序号
			gb32960_can_set_qddj_status(0,0x04);					//电机状态
			gb32960_can_set_qddj_controller_wendu(0,38 + val_rand % 3 + 40); 	//控制器温度 38 39 40
			gb32960_can_set_qddj_rpm(0,20000); 			//转速
			gb32960_can_set_qddj_zhuanju(0,20000); 		//转矩
			gb32960_can_set_qddj_wendu(0,38 + val_rand % 3 + 40);			//电机温度
			gb32960_can_set_qddj_input_dianya(0,5790 + val_rand % 20);				//579 580
			gb32960_can_set_qddj_controller_dianliu(0,10130 + val_rand % 55);		//13 - 18
		}
#endif


#if 1
		//电压数据
		gb32960_can_set_dianchi_cnt(1);
		if (1)
		{
			stGB32960Param.stCNXTData[0].id = 1;

			stGB32960Param.stCNXTData[0].dianya = gb32960_can_get_total_dianya();

			stGB32960Param.stCNXTData[0].dianliu = gb32960_can_get_total_dianliu();
			stGB32960Param.stCNXTData[0].benzhen_start_id = 1;	
			stGB32960Param.stCNXTData[0].benzhen_danti_cnt = 171;
		
			for (i = 0; i < 171; i++)
			{
				val_rand = rand();
				stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i] = 3200 + val_rand % 109;

				if (stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i] > max_dy_val)
				{
					max_dy_val = stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i];
					max_dy_id = i;
				}

				if (i == 0) min_dy_val = stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i];
				if (stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i] < min_dy_val)
				{
					min_dy_val = stGB32960Param.stCNXTData[0].danti_dianchi_dianya[i];
					min_dy_id = i;
				}
			}
		}
#endif
#if 1
		stGB32960Param.stCNXTData[0].id = 1;
		stGB32960Param.stCNXTData[0].wendu_tanzhen_cnt = 64;

		//温度数据
		for (i = 0; i < 64; i++)
		{
			val_rand = rand();
			stGB32960Param.stCNXTData[0].wendu[i] = 60 + val_rand % 4;	//20 - 23

			if (stGB32960Param.stCNXTData[0].wendu[i] > max_wd_val)
			{
				max_wd_val = stGB32960Param.stCNXTData[0].wendu[i];
				max_wd_id = i;
			}

			if (i == 0) min_wd_val = stGB32960Param.stCNXTData[0].wendu[i];
			if (stGB32960Param.stCNXTData[0].wendu[i] < min_wd_val)
			{
				min_wd_val = stGB32960Param.stCNXTData[0].wendu[i];
				min_wd_id = i;
			}
		}
#endif


#if 1
	//极值信息
	gb32960_can_set_max_dianya_sub_system_id(1);
	gb32960_can_set_max_dianya_danti_id(max_dy_id + 1);
	gb32960_can_set_max_danti_dianya_val(max_dy_val);

	gb32960_can_set_min_dianya_sub_system_id(1);
	gb32960_can_set_min_dianya_danti_id(min_dy_id + 1);
	gb32960_can_set_min_danti_dianya_val(min_dy_val);

	gb32960_can_set_max_wendu_sub_system_id(1);
	gb32960_can_set_max_wendu_tanzhen_id(max_wd_id + 1);
	gb32960_can_set_max_wendu_val(max_wd_val);

	gb32960_can_set_min_wendu_sub_system_id(1);
	gb32960_can_set_min_wendu_tanzhen_id(min_wd_id + 1);
	gb32960_can_set_min_wendu_val(min_wd_val);
#endif



	//燃料电池
	gb32960_can_set_rldc_dianya(val_rand % 20000);
	gb32960_can_set_rldc_dianliu(gb32960_can_get_rldc_dianya() / 3284);
	gb32960_can_set_rldc_xiaohaolv(val_rand % 60000);
	gb32960_can_set_rldc_tanzhen_cnt(1);
	gb32960_can_set_rldc_tanzhen_wendu(0,val_rand % 210 + 40);

	gb32960_can_set_max_wendu_in_qi_system(val_rand % 2000 + 400);
	gb32960_can_set_max_wendu_tanzhen_id_in_qi_system(1);
	gb32960_can_set_max_qingqi_nongdu(val_rand % 60000);
	gb32960_can_set_max_qingqi_sensor_id(1);
	gb32960_can_set_max_qingqi_yali(val_rand % 1000);
	gb32960_can_set_max_qingqi_yali_sensor_id(1);
	if (val_rand % 2)
	{
		gb32960_can_set_gaoya_dc_dc_status(1);
	}
	else
	{
		gb32960_can_set_gaoya_dc_dc_status(2);
	}
}


INT32U s_sos_collect_time = 0;
INT32U get_sos_collect_time(void)
{
	return s_sos_collect_time;
}

void set_sos_collect_time(INT32U val)
{
	s_sos_collect_time = val;
}


char s_xh_f = 0;
char is_car_xh(void)
{
	return s_xh_f;
}

void gb32960_proc(void)
{
	static INT32U ss_xh_cnt = 0;

	//车辆状态持续20秒为熄火状态 判断为熄火
	//if (gb32960_can_get_car_status() == 0x02)
	//车辆速度持续300秒为0 判定为熄火
	//if (gb32960_can_get_speed() == 0)

	if (get_can_no_data_time() > 10)
	{
		ss_xh_cnt++;
		if (ss_xh_cnt >= 100)
		{
			s_xh_f = 1;
		}
	}
	else
	{
		ss_xh_cnt = 0;
		s_xh_f = 0;
	}
}

void ota_ctrl(INT8U linkid,INT8U cmd,void *content,unsigned int contentlen)
{
	INT8U buf[64] = {0};
	unsigned char ack[13] = {0};
	unsigned int slen = 0;
	char *pcmd = 0;
	char *purl = 0;
	unsigned char urllen = 0;
	unsigned int templen = 0;
	unsigned char *pcontent = (unsigned char *)content;
	INT8U subcmd = 0;
	INT8U can_src_addr = 0;				//CAN 网络源地址 EF 表示VCU C8 标识BMS  EE 表示MCU

	if ((content == 0) || (contentlen <= 10))
	{
		return ;
	}

	subcmd = *(pcontent + 6);
	can_src_addr = *(pcontent + 11);
	//应答
	memcpy(ack,pcontent,7);
	ack[7] = 0;						//流水 
	ack[8] = 1;

	ack[9] = *(pcontent + 9);		//分包标识
	ack[10] = *(pcontent + 7);		//平台下发的信息流水
	ack[11] = *(pcontent + 8);
	ack[12] = 1;					//应答

	slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,13);
	DBG_Print(LEVEL2,"(link%d)new ack plat ota cmd(%uBytes):\r\n",linkid,slen);
	DBG_PrintBuf(LEVEL2,0,buf,slen);
	send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);

	pcmd = os_malloc(1536);
	if (pcmd)
	{
		purl = _find_sub_str(pcontent + 10,contentlen - 10,"ftp://",strlen("ftp://"));
		if (purl)
		{
			urllen = *(purl - 1);
			if (urllen && (urllen < 250))
			{
				//snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ftp://%s:%s@%s>",pusr,psw,purl + strlen("ftp://"));

				if (cmd == 0xA5)			//TBOX ota
				{
					if (subcmd == 0x52)
					{
						templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ");
					}
				}
				else if (cmd == 0xA0)		//零部件OTA
				{
					if (subcmd == 0xC4)
					{
						sandian_set_update_type(SANDIAN_UPDATE_TYPE_SHENGJI);
						sandian_set_update_way(SANDIAN_UPDATE_WAY_PUTONG);
						if (can_src_addr == 0xEE) 		//MCU
						{
							sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_MCU);
							templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/sandian_mcu.zip ");
						}
						else if (can_src_addr == 0xC8)	//BMS
						{
							sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_BMS);
							templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/sandian_bms.zip ");
						}
						else if (can_src_addr == 0xEF)	//VCU
						{
							sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID_VCU);
							templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/sandian_vcu.zip ");
						}
					}
				}

				memcpy(pcmd + templen,purl,urllen);
				templen += urllen;

				pcmd[templen++] = '>';
				pcmd[templen] = 0;
			}
		}
		else
		{
			DBG_Print(LEVEL0,"%s","not fond ftp server\r\n");
			//purl = _find_sub_str(pcontent + 10,contentlen - 10,"http://",strlen("http://"));
		}

		if (purl)
		{
			DBG_Print(0,"[exe cmd]%s\r\n",pcmd);
			if (platform_remote_wget_allowed(pcmd))
			{
				send_msg(S_MSG_EXECUTE_DRV_CMD,pcmd,0,0);
			}
			else
			{
				DBG_Print(LEVEL0,"reject platform remote wget\r\n");
			}
		}
		os_free(pcmd);
	}
}

extern INT32U s_32960_zuocang_collect_time;

void driver_ctrl(INT8U linkid,INT8U cmd,void *content,unsigned int contentlen)
{
	INT8U buf[64] = {0};
	unsigned char ack[13] = {0};
	unsigned int slen = 0;
	unsigned char *pcontent = (unsigned char *)content;
	INT8U subcmd = 0;
	
	if ((content == 0) || (contentlen <= 10))
	{
		return ;
	}
	if (get_gb32960_login_ok_flag(linkid) == 0)
	{
		DBG_Print(LEVEL0,"ignore 0xA7 without login session\r\n");
		return ;
	}
	
	subcmd = *(pcontent + 6);
	
	switch (subcmd) 	//命令ID
	{
		case 0x61:		//启动
		{
			//if(get_key_in_status() != 0x1)
			//if((get_key_in_status() != 0x1) || (get_key_status() == 0x00))
			{
				if((*(pcontent + 10)) < 0x02)
				{
					if(*(pcontent + 10))
					{
						set_want_startup_status(1);
						send_msg(S_MSG_GPS_LED_CTRL,(void *)1,0,0);
						DBG_Print(LEVEL0,"set_want_startup_status vaule up\r\n");
					}
					else
					{
						set_want_startup_status(0);
						DBG_Print(LEVEL0,"qddj_rpm(0):%d\r\n,qddj_rpm(1):%d\r\n,dangwei:0x%x,\r\nhand_brk:%d\r\n",gb32960_can_get_qddj_rpm(0),
						gb32960_can_get_qddj_rpm(1),gb32960_can_get_dangwei(),get_hand_brk_status());
						if((gb32960_can_get_qddj_rpm(0) < 20300) && (gb32960_can_get_qddj_rpm(0) > 19700)
						&& ((get_hand_brk_status() == 0x01) || (get_brk_status() == 0x01)))
						//&& ((gb32960_can_get_dangwei() == 0xF) || (gb32960_can_get_dangwei() == 0x0)))
						{
							send_msg(S_MSG_GPS_LED_CTRL,(void *)0,0,0);
							DBG_Print(LEVEL0,"set_want_startup_status vaule dowm\r\n");
						}
						else
						{
							DBG_Print(LEVEL0,"set_want_startup_status failed\r\n");
						}
					}
					
				}
				else
				{
					DBG_Print(LEVEL0,"set_want_startup_status vaule error\r\n");
				}
			}
			s_32960_zuocang_collect_time = 1;
			set_s_32960_zuocang_data_change(1);
		}
		break;
		
		case 0x62:		//车门
		{
			if((*(pcontent + 12)) < 0x02)
			{
				set_want_lockdoor_status(*(pcontent + 12));
				DBG_Print(LEVEL0,"set_want_lockdoor_status vaule%d\r\n",(*(pcontent + 12)));
			}
			else
			{
				DBG_Print(LEVEL0,"set_want_lockdoor_status vaule error\r\n");
			}
			s_32960_zuocang_collect_time = 1;
			set_s_32960_zuocang_data_change(1);
			set_lockdoor_commond_flag(1);
		}
		break;
		/*
		case 0x63:		//空调
		{
			if((*(pcontent + 6)) < 0x02)
			{
				set_want_control_air_cond(*(pcontent + 6));
				DBG_Print(LEVEL0,"set_want_control_air_cond vaule\r\n");
			}
			else
			{
				DBG_Print(LEVEL0,"set_want_control_air_cond vaule error\r\n");
			}
			set_s_32960_zuocang_data_change(1);
		}
		break;
		*/
		default:break;
		
	}
	
	//应答
	memcpy(ack,pcontent,7);
	ack[7] = 0;						//流水 
	ack[8] = 1;

	ack[9] = 0;		//分包标识
	ack[10] = *(pcontent + 7);		//平台下发的信息流水
	ack[11] = *(pcontent + 8);
	ack[12] = 1;					//应答

	slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,13);
	DBG_Print(LEVEL0,"(link%d)new ack plat zuo cang cmd(%uBytes):\r\n",linkid,slen);
	DBG_PrintBuf(LEVEL2,0,buf,slen);
	send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
}
extern int mcu_start_update_flag;
void gb32960_deal(INT8U linkid,INT8U cmd,INT8U ack_flag,void *pvin,INT8U jiami_way,void *content,INT16U contentlen)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;

	ptr = (unsigned char *)content;
	switch (cmd)
	{
	case GB32960_LOGIN_CMD:
		if (ack_flag == 0x01)
		{
			set_gb32960_login_ok_flag(linkid, 1);
			reset_main_off_flag();
		}
		else
		{
			set_gb32960_login_ok_flag(linkid, 0);
		}
		break;
	case GB32960_LOGOUT_CMD:
	set_gb32960_login_ok_flag(linkid, 0);
	break;
	case GB32960_JS_CMD:		//终端校时

	DBG_PrintBuf(LEVEL0,"GB32960平台响应校时:",content,6);
	break;
	case GB32960_PLAT_CTRL_CMD:
	{
		INT8U buf[64] = {0};
		INT8U ack[32] = {0};
		INT8U ack_time[6] = {0};
		unsigned int slen = 0;
		
		DBG_Print(LEVEL0,"平台下发控制命令时间:%02X %02X %02X %02X %02X %02X\r\n命令ID:%02X\r\n",
		ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6]);
		
		switch (ptr[6]) 	//命令ID
		{
		case 0x00:		//未用
		
		break;
		case 0x01:		//远程升级
		DBG_Print(LEVEL0,"GB32960远程升级\r\n");
		
		break;
		case 0x02:		//关机
			DBG_Print(LEVEL0,"设备关机\r\n");
			//connect_server(linkid,"www.baidu.com",0,1);	//用断开服务器连接代替 假装设备关机
		break;
		case 0x03:		//设备复位
			//drv_analyze_usercmd("ATWST\r\n",7,0,0);
		break;
		case 0x04:
		DBG_Print(LEVEL0,"恢复出厂设置\r\n");
		
		break;
		case 0x05:		//断开链路
		DBG_Print(LEVEL0,"断开链路\r\n");
			//connect_server(linkid,"www.baidu.com",0,1);		//只要端口设置成0 就是断开链路
		break;
		case 0x06:		//设备报警
		DBG_Print(LEVEL0,"平台控制设备发出报警信号 报警等级:%u\r\n",ptr[7]);
		
		break;
		case 0x07:		//开启抽样检测链路
		DBG_Print(LEVEL0,"开启抽样检测链路\r\n");
		
		break;
		case 0x90:			//lock
			set_lock_car_cmd_from_where(1);
			if (ptr[7] == 0x90)
			{
				set_want_lock_status(0);
			}
			else if (ptr[7] == 0x91)
			{
				set_want_lock_status(1);
			}

			//send_msg(S_MSG_GET_RTC_TIME,ack_time,0,0);
			memcpy(ack_time,ptr,6);

			//响应时间
			ack[rlen++] = ack_time[0];
			ack[rlen++] = ack_time[1];
			ack[rlen++] = ack_time[2];
			ack[rlen++] = ack_time[3];
			ack[rlen++] = ack_time[4];
			ack[rlen++] = ack_time[5];

			ack[rlen++] = 0x90;
			ack[rlen++] = ptr[7];

			slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,rlen);
			DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
			DBG_PrintBuf(LEVEL2,0,buf,slen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
		break;
		case 0x91:			//pro version
			set_lock_car_cmd_from_where(1);
			//send_msg(S_MSG_GET_RTC_TIME,ack_time,0,0);
			memcpy(ack_time,ptr,6);
			//响应时间
			ack[rlen++] = ack_time[0];
			ack[rlen++] = ack_time[1];
			ack[rlen++] = ack_time[2];
			ack[rlen++] = ack_time[3];
			ack[rlen++] = ack_time[4];
			ack[rlen++] = ack_time[5];

			set_want_pro_ver_about_lock(ptr[7] ? 1 : 0);

			ack[rlen++] = 0x91;
			ack[rlen++] = ptr[7];

			slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,rlen);
			DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
			DBG_PrintBuf(LEVEL2,0,buf,slen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);

		break;

		case 0x93:			//bind ctrl  01:bind 02:unblind
		{
			set_lock_car_cmd_from_where(1);
			if (ptr[7] == 1)
			{
				set_want_bind_status(1);
			}
			else
			{
				set_want_bind_status(2);

				//解除绑定 清除锁车 清除锁车版本
				set_want_lock_status(0);
				set_want_pro_ver_about_lock(0);
			}

			//send_msg(S_MSG_GET_RTC_TIME,ack_time,0,0);
			memcpy(ack_time,ptr,6);
			//响应时间
			ack[rlen++] = ack_time[0];
			ack[rlen++] = ack_time[1];
			ack[rlen++] = ack_time[2];
			ack[rlen++] = ack_time[3];
			ack[rlen++] = ack_time[4];
			ack[rlen++] = ack_time[5];

			ack[rlen++] = 0x93;
			ack[rlen++] = ptr[7];
			send_msg(S_MSG_GET_ICCID,ack + rlen,0,0);
			rlen += 20;

			slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,rlen);
			DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
			DBG_PrintBuf(LEVEL2,0,buf,slen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);
		}
		break;
		default:break;
		}
		

	}
	break;
	case GB32960_PLAT_SET_PARAM_CMD:
	{
		DBG_Print(LEVEL0,"GB32960平台设置参数时间:%02X %02X %02X %02X %02X %02X\r\n设置参数个数:%u\r\n",
		ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6]);

	}
	break;
	case GB32960_PLAT_QUERY_PARAM_CMD:					//参数查询
	{
		INT8U i = 0;
		INT8U total_cnt = 0;
		INT8U paramid = 0;
		INT8U *psend = 0;
		unsigned int slen = 0;
		INT8U *ack = 0;
		INT8U ack_time[6] = {0};
		INT8U *ptemp = 0;
		INT32U temp = 0;

		psend = os_malloc(1536);
		if (psend)
		{
			ack = psend + 700;
			
			DBG_Print(LEVEL2,"GB32960 query param time:%02X %02X %02X %02X %02X %02X\r\n"
							"query param cnt:%u\r\n",
			ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6]);
			total_cnt = ptr[6];
			DBG_PrintBuf(LEVEL2,0,ptr + 7,total_cnt);

			//send_msg(S_MSG_GET_RTC_TIME,ack_time,0,0);
			memcpy(ack_time,ptr,6);
			//响应时间
			ack[rlen++] = ack_time[0];
			ack[rlen++] = ack_time[1];
			ack[rlen++] = ack_time[2];
			ack[rlen++] = ack_time[3];
			ack[rlen++] = ack_time[4];
			ack[rlen++] = ack_time[5];

			//参数总数
			ack[rlen++] = total_cnt;
			for (i = 0; i < total_cnt; i++)
			{
				paramid = ptr[7+i];
				ack[rlen++] = paramid;		//参数ID
				switch (paramid)
				{
					case 0x01:							//本地存储时间间隔  ms
						ack[rlen++] = 10000 >> 8;
						ack[rlen++] = (INT8U)10000;
					break;
					case 0x02:							//信息上报时间周期 s
						ack[rlen++] = 10 >> 8;
						ack[rlen++] = (INT8U)10;					
					break;
					case 0x03:							//出现报警时 信息上报周期 ms
						ack[rlen++] = 1000 >> 8;
						ack[rlen++] = (INT8U)1000;
					break;
					case 0x04:							//远程服务平台域名长度
						ptemp = send_msg(S_MSG_GET_SERVER_ADDR,(void *)2,0,0);
						if (strlen((char *)ptemp) > 32)
						{
							ack[rlen++] = 32;
						}
						else
						{
							ack[rlen++] = strlen((char *)ptemp);
						}
					break;
					case 0x05:
						ptemp = send_msg(S_MSG_GET_SERVER_ADDR,(void *)2,0,0);

						strncpy((char *)ack+rlen,(char *)ptemp,32);
						if (strlen((char *)ptemp) > 32)
						{
							rlen += 32;
						}
						else
						{
							rlen += strlen((char *)ptemp);
						}
					break;
					case 0x06:		//端口
						temp = (unsigned int)send_msg(S_MSG_GET_SERVER_PORT,(void *)2,0,0);
						ack[rlen++] = temp >> 8;
						ack[rlen++] = temp;
					break;
					case 0x07:				//硬件版本 5字节
						ack[rlen++] = 0x30;
						ack[rlen++] = 0x30;
						ack[rlen++] = 0x30;
						ack[rlen++] = 0x30;
						ack[rlen++] = 0x31;
					break;
					case 0x08:				//软件版本 5字节
						sprintf((char *)ack+rlen,"%05d",get_software_version());
						rlen += 5;
					break;
					case 0x09:				//心跳周期 s
						ack[rlen++] = 30;
					break;
					case 0x0A:				//终端应答超时时间 s (范围:1-600s)
						ack[rlen++] = 0;
						ack[rlen++] = 60;
					break;
					case 0x0B:				//平台应答超时时间 s
						ack[rlen++] = 0;
						ack[rlen++] = 60;
					break;
					case 0x0C:				//连续3次登录失败 下一次登录时间间隔 min
						ack[rlen++] = 30;
					break;
					case 0x0D:
						ptemp = send_msg(S_MSG_GET_SERVER_ADDR,0,0,0);
						if (strlen((char *)ptemp) > 32)
						{
							ack[rlen++] = 32;
						}
						else
						{
							ack[rlen++] = strlen((char *)ptemp);
						}
					break;
					case 0x0E:				//公共平台域名
						ptemp = send_msg(S_MSG_GET_SERVER_ADDR,0,0,0);
						strncpy((char *)ack+rlen,(char *)ptemp,32);
						if (strlen((char *)ptemp) > 32)
						{
							rlen += 32;
						}
						else
						{
							rlen += strlen((char *)ptemp);
						}
					break;
					case 0x0F:				//公共平台端口
						temp = (unsigned int)send_msg(S_MSG_GET_SERVER_PORT,0,0,0);
					
						ack[rlen++] = temp >> 8;
						ack[rlen++] = temp;
					break;
					case 0x10:
					ack[rlen++] = 0x01;		//是否抽样检测中 01表示是 02表示否
					break;
					case 0x90:				//是否处于锁车状态
						ack[rlen++] = get_fact_lock_status();
					break;
					case 0x91:
						ack[rlen++] = get_fact_pro_ver_about_lock();		//pro version
					break;
					case 0x93:					//bind status
						ack[rlen++] = get_fact_bind_status();
					break;
					default:
					break;
				}
			}

			slen = _xDataBag(psend,700,cmd,0x01,sGBDataInfo.pVIN,ack,rlen);

			DBG_Print(LEVEL2,"(link%d)ack plat query param(%u):\r\n",linkid,slen);
			DBG_PrintBuf(LEVEL2,0,psend,slen);

			//net_send_in_cb(linkid,psend,slen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,psend,(void *)slen);
			os_free(psend);
		}
	}
	break;
	case 0x92:
	{
		unsigned char subcmd = 0;
		INT8U ack[16] = {0};

		subcmd = ptr[6];
		if (contentlen >= 11)
		{
			INT8U buf[64] = {0};
			unsigned int slen = 0;

			memcpy(ack,ptr,11);
			//应答
			ack[11] = 1;
			slen = _xDataBag(buf,64,cmd,0x01,sGBDataInfo.pVIN,ack,12);
			DBG_Print(LEVEL2,"(link%d)ack plat ota cmd(%uBytes):\r\n",linkid,slen);
			DBG_PrintBuf(LEVEL2,0,buf,slen);
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,buf,(void *)slen);

			if (subcmd == 0xC8) 		//tbox ota
			{
				char *pcmd = 0;
				char *pusr = 0;
				char *psw = 0;
				char *purl = 0;
				unsigned char urllen = 0;
				unsigned char usrlen = 0;
				unsigned char pswlen = 0;
				unsigned int templen = 0;

				pcmd = os_malloc(1536);
				if (pcmd)
				{
					purl = _find_sub_str(ptr + 10,16,"ftp://",strlen("ftp://"));
					if (purl)
					{
						urllen = *(purl - 1);

						pusr = purl + urllen + 1;
						usrlen = *(purl + urllen);

						psw = pusr + usrlen + 1;
						pswlen = *(purl + urllen + 1 + usrlen);

						if ((urllen >= strlen("ftp://")) && (usrlen < 144) && (pswlen < 144))
						{
							//snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ftp://%s:%s@%s>",pusr,psw,purl + strlen("ftp://"));
							templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ftp://");
							memcpy(pcmd + templen,pusr,usrlen);
							templen += usrlen;
							pcmd[templen++] = ':';
							
							memcpy(pcmd + templen,psw,pswlen);
							templen += pswlen;
							pcmd[templen++] = '@';
							
							memcpy(pcmd + templen,purl + strlen("ftp://"),(urllen - strlen("ftp://")));
							templen += (urllen - strlen("ftp://")); 						

							pcmd[templen++] = '>';

							pcmd[templen] = 0;
							//DBG_Print(LEVEL1,"usr:%s psw:%s url:%s\r\n",pusr,psw,purl);
							//DBG_Print(LEVEL1,"exe cmd:%s\r\n",pcmd);

						}
					}
					else
					{
						//DBG_Print(LEVEL0,"%s","not fond ftp server\r\n");
						purl = _find_sub_str(ptr + 10,16,"http://",strlen("http://"));
						if (purl)
						{
							//ATEX=<linuxcmd:wget -O /data/box.bin http://129.211.170.236/BYSD_Z2021_MCU_V1.5.18BYSD_Z2021_MPU_V1.0.17.bin>
							templen = snprintf(pcmd,511,"ATEX=<linuxcmd:wget -O /data/box.bin ");
							if (*(purl - 1) < 450)
							{
								memcpy(pcmd + templen,purl,*(purl - 1));
								templen += *(purl - 1);
								pcmd[templen++] = '>';
								pcmd[templen] = 0;
							}
							else
							{
								purl = 0;
							}
						}
					}

					if (purl)
					{
						DBG_Print(LEVEL1,"exe cmd:%s\r\n",pcmd);
						if (platform_remote_wget_allowed(pcmd))
						{
							send_msg(S_MSG_EXECUTE_DRV_CMD,pcmd,0,0);
							mcu_start_update_flag = 1;
						}
						else
						{
							DBG_Print(LEVEL0,"reject platform remote wget\r\n");
						}
					}
					os_free(pcmd);
				}
			}
			else if (subcmd == 0xC4)
			{

			}
		}
	}
	break;

	case 0xA0:					//零部件 OTA控制
	case 0xA5:					//TBOX OTA控制
		ota_ctrl(linkid,cmd,content,contentlen);
	break;
	
	case 0xA7:					//TBOX OTA控制
		driver_ctrl(linkid,cmd,content,contentlen);
	break;

	default:break;
	}
}


int gb32960_locate_frame(unsigned char *buf,unsigned int len,
	unsigned char **pframe,unsigned int *framelen)
{
	unsigned char *phead = 0;
	unsigned int skipped = 0;
	unsigned int remain = 0;
	unsigned int contentlen = 0;
	unsigned int need = 0;
	unsigned char expect = 0;

	if (pframe) *pframe = 0;
	if (framelen) *framelen = 0;
	if ((buf == 0) || (len < GB32960_MIN_PACK_LEN))
	{
		return 0;
	}

	phead = (unsigned char *)_find_sub_str(buf,len,"\x23\x23",2);
	if (phead == 0)
	{
		return 0;
	}
	skipped = (unsigned int)(phead - buf);
	if (skipped > len)
	{
		return 0;
	}
	remain = len - skipped;
	if (remain < GB32960_MIN_PACK_LEN)
	{
		return 0;
	}

	contentlen = ((unsigned int)phead[GB32960_CONTENT_LEN_OFFSET] << 8) + phead[GB32960_CONTENT_LEN_OFFSET + 1];
	need = (unsigned int)GB32960_CONTENT_OFFSET + contentlen + 1;
	if ((need < contentlen) || (remain < need))
	{
		return 0;
	}

	if (pframe) *pframe = phead;
	if (framelen) *framelen = need;

	expect = XOR_Check(phead + 2,(INT32U)GB32960_CONTENT_OFFSET + contentlen - 2);
	if (expect != phead[GB32960_CONTENT_OFFSET + contentlen])
	{
		return -1;
	}
	return 1;
}

static void gb32960_dispatch_frames(INT8U linkid,void *pdat,unsigned int len,
	void (*deal)(INT8U,INT8U,INT8U,void *,INT8U,void *,INT16U),
	const char *tag,int loglevel)
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

			DBG_Print(loglevel,"-------(link%d)%s-------\r\n"
							"cmd:%02X ack:%02X vin:%s jiami:%02X dataunitlen:%d xor:%02X\r\n",
							linkid,tag,cmd,ack_flag,pvin,jiami_way,contentlen,xor);
			if (deal) deal(linkid,cmd,ack_flag,pvin,jiami_way,content,contentlen);
		}
		else
		{
			DBG_Print(LEVEL1,"reject %s frame, bad BCC\r\n",tag);
		}
		ptr = frame + framelen;
		len -= skipped + framelen;
	}
}

void gb32960_ack_plat(INT8U linkid,void *pdat,unsigned int len)
{
	gb32960_dispatch_frames(linkid,pdat,len,gb32960_deal,"GB32960 server ack",LEVEL3);
}

void sddf_deal(INT8U linkid,INT8U cmd,INT8U ack_flag,void *pvin,INT8U jiami_way,void *content,INT16U contentlen)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;

	ptr = ptr;

	ptr = (unsigned char *)content;
	switch (cmd)
	{
	case 0x01:				//车辆登录响应

	break;

	case 0x05:				//平台登录响应
		set_sddf_login_ok_flag((ack_flag == 1) ? 1 : 0);
	break;

	case 0xC3:				//平台下发充电限制
	{
		unsigned char *pbuf = 0;

		pbuf = os_malloc(256);
		if (pbuf)
		{
			rlen = sddf_ack_charge_limit_pack(pbuf,1024,GetVIN(),get_rtc_time_ptr());
			send_msg(S_MSG_SEND_TO_SERVER,(void *)linkid,pbuf,(void *)rlen);

			os_free(pbuf);
		}
	}
	break;

	default:break;
	}
}


void sddf_ack_plat(INT8U linkid,void *pdat,unsigned int len)
{
	gb32960_dispatch_frames(linkid,pdat,len,sddf_deal,"sddf server ack",LEVEL1);
}

typedef enum tagSOSStatus
{
	SOS_NONE = 0,
	SOS_START,		//开始报警
	SOS_AFTER_REAL_COLLOCET,		//报警发生后 实时采集
	SOS_PRV_BF, 	//补发报警点之前的30秒数据
	SOS_PRV_BF_OK, 	//补发报警点之前的30秒数据完成
	SOS_END,		//报警结束
}SOSStatus;

typedef void (*pf_collect_function)(void);

SOSStatus sos_sta = SOS_NONE;
static unsigned char s_happen_sos_flag = 0;
pf_collect_function pf_collect_func = 0;

SOSStatus d78_sos_sta = SOS_NONE;
static unsigned char s_d78_happen_sos_flag = 0;
pf_collect_function pf_d78_collect_func = 0;

//是否正常采集
char is_usually_collect(void)
{
	if ((sos_sta != SOS_AFTER_REAL_COLLOCET) && (d78_sos_sta != SOS_AFTER_REAL_COLLOCET))
	{
		return 1;
	}
	return 0;
}

void check_sos(void)
{
	static INT32U bc_prv_sos_cnt = 0;
	static INT32U bc_jishi_cnt = 0;
	static INT8U s_after_cnt = 0;
	static INT8U sc_last_sec = 0;
	unsigned int idx = 0;
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	unsigned char time[6] = {0};

	send_msg(S_MSG_GET_RTC_TIME,time,0,0);
	if (s_happen_sos_flag == 0)
	{
		//一秒记录一次
		if (time[5] != sc_last_sec)
		{
			sc_last_sec = time[5];

			ptr = os_malloc(1536);
			if (ptr)
			{
				rlen = xGB32960_BFDataPack(ptr,1536,GetVIN(),time,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0));
				if (rlen)
				{
					send_msg(S_MSG_STORE_CACHE_DATA,0,ptr,(void *)rlen);
		
					DBG_Print(LEVEL4,"store every one sec:%02d-%02d-%02d %02d:%02d:%02d\r\n",
					time[0],time[1],time[2],time[3],time[4],time[5]
					);
				}
				os_free(ptr);
			}
		}
	}

	switch (sos_sta)
	{
	case SOS_NONE:
	//三级报警 启动报警机制
	if (gb32960_can_get_max_sos_level() == 3)
	{
		DBG_Print(LEVEL1,"启动报警:%02d-%02d-%02d %02d:%02d:%02d\r\n",time[0],time[1],time[2],time[3],time[4],time[5]);

		if (pf_collect_func)
		{
			pf_collect_func();
		}
		set_sos_collect_time(GetRunTime());
		sc_last_sec = time[5];
		s_happen_sos_flag = 1;			//报警时间点
		s_after_cnt = 0;

		sos_sta = SOS_AFTER_REAL_COLLOCET;			//状态改变为实时采集
	}
	break;
	case SOS_START:
	break;
	case SOS_AFTER_REAL_COLLOCET:
	//发生报警 实时1hz上传29s
	if (time[5] != sc_last_sec)
	{
		sc_last_sec = time[5];

		s_after_cnt++;

		if (pf_collect_func)
		{
			pf_collect_func();
		}

		DBG_Print(LEVEL1,"发生报警后采集点(%u):%02d-%02d-%02d %02d:%02d:%02d\r\n",
		s_after_cnt,time[0],time[1],time[2],time[3],time[4],time[5]);

		if (s_after_cnt >= 29)
		{
			s_after_cnt = 0;
			bc_prv_sos_cnt = 0;
			sos_sta = SOS_PRV_BF;
		}
	}
	break;
	case SOS_PRV_BF:						//补发报警点之前的30秒数据
	if (1)
	{
		bc_jishi_cnt++;
		if (bc_jishi_cnt >= 10)				//一秒补传一包
		{
			bc_jishi_cnt = 0;
			idx = bc_prv_sos_cnt + 1;

			send_msg(S_MSG_GET_CACHE_DATA,0,(void *)idx,0);

			bc_prv_sos_cnt++;
			if (bc_prv_sos_cnt >= 30)
			{
				bc_prv_sos_cnt = 0;
				s_happen_sos_flag = 0;
				sos_sta = SOS_PRV_BF_OK;
			}
		}
	}
	break;
	case SOS_PRV_BF_OK:
	//等待向前追溯的30包数据补传完成 和 向后采集29秒数据完成 之后 再退出报警机制
	if (gb32960_can_get_max_sos_level() == 0)
	{
		//报警结束
		sos_sta = SOS_END;
	}
	break;
	case SOS_END:
		sos_sta = SOS_NONE;
	break;
	default:break;
	}

}


char is_d78_happen_sos(void)
{
	if ((get_d78_mcu_gz_level() == 3)
		|| (get_d78_vcu_gz_level() == 3)
		|| (get_d78_dcdc_gz_level() == 3)
		|| (get_d78_bms_gz_level() == 3)
		|| (get_d78_mcu_igbt_flag() == 1)
		|| (gb32960_can_get_max_sos_level() == 3)
		)
	{
		return 1;
	}
	return 0;
}

void d78_check_sos(void)
{
	static INT32U bc_prv_sos_cnt = 0;
	static INT32U bc_jishi_cnt = 0;
	static INT8U s_after_cnt = 0;
	static INT8U sc_last_sec = 0;
	unsigned int idx = 0;
	unsigned int rlen = 0;
	unsigned char *ptr = 0;
	unsigned char time[6] = {0};

	send_msg(S_MSG_GET_RTC_TIME,time,0,0);
	if (s_d78_happen_sos_flag == 0)
	{
		//一秒记录一次
		if (time[5] != sc_last_sec)
		{
			sc_last_sec = time[5];

			ptr = os_malloc(1536);
			if (ptr)
			{
				rlen = D78_xGB32960_BFDataPack(ptr,1536,GetVIN(),time,0,
											(INT32U)send_msg(S_MSG_GET_LON,0,0,0),
											(INT32U)send_msg(S_MSG_GET_LAT,0,0,0));
				if (rlen)
				{
					send_msg(S_MSG_STORE_CACHE_DATA,(void *)1,ptr,(void *)rlen);

					DBG_Print(LEVEL4,"store every one sec:%02d-%02d-%02d %02d:%02d:%02d\r\n",
					time[0],time[1],time[2],time[3],time[4],time[5]
					);
				}
				os_free(ptr);
			}
		}
	}

	switch (d78_sos_sta)
	{
	case SOS_NONE:
	//三级报警 启动报警机制
	if (is_d78_happen_sos())
	{
		DBG_Print(LEVEL1,"D78启动报警程序:%02d-%02d-%02d %02d:%02d:%02d\r\n",time[0],time[1],time[2],time[3],time[4],time[5]);

		if (pf_d78_collect_func)
		{
			pf_d78_collect_func();
		}
		set_sos_collect_time(GetRunTime());
		sc_last_sec = time[5];
		s_d78_happen_sos_flag = 1;			//报警时间点
		s_after_cnt = 0;

		d78_sos_sta = SOS_AFTER_REAL_COLLOCET;			//状态改变为实时采集
	}
	break;
	case SOS_START:
	break;
	case SOS_AFTER_REAL_COLLOCET:
	//发生报警 实时1hz上传60s
	if (time[5] != sc_last_sec)
	{
		sc_last_sec = time[5];

		s_after_cnt++;

		if (pf_d78_collect_func)
		{
			pf_d78_collect_func();
		}

		DBG_Print(LEVEL1,"发生报警后采集点(%u):%02d-%02d-%02d %02d:%02d:%02d\r\n",
		s_after_cnt,time[0],time[1],time[2],time[3],time[4],time[5]);

		if (s_after_cnt >= 60)
		{
			s_after_cnt = 0;
			bc_prv_sos_cnt = 0;
			d78_sos_sta = SOS_PRV_BF;
		}
	}
	break;
	case SOS_PRV_BF:						//补发报警点之前的60秒数据
	if (1)
	{
		bc_jishi_cnt++;
		if (bc_jishi_cnt >= 10)				//一秒补传一包
		{
			bc_jishi_cnt = 0;
			idx = bc_prv_sos_cnt + 1;

			send_msg(S_MSG_GET_CACHE_DATA,(void *)1,(void *)idx,0);

			bc_prv_sos_cnt++;
			if (bc_prv_sos_cnt >= 60)
			{
				bc_prv_sos_cnt = 0;
				s_d78_happen_sos_flag = 0;
				d78_sos_sta = SOS_PRV_BF_OK;
			}
		}
	}
	break;
	case SOS_PRV_BF_OK:
	//等待向前追溯的60包数据补传完成 和 向后采集60秒数据完成 之后 再退出报警机制
	if (is_d78_happen_sos() == 0)
	{
		//报警结束
		d78_sos_sta = SOS_END;
	}
	break;
	case SOS_END:
		d78_sos_sta = SOS_NONE;
	break;
	default:break;
	}

}


void gb32960_sos_proc(void (*p_collect_func)(void))
{
	pf_collect_func = p_collect_func;
	check_sos();
}

void d78_gb32960_sos_proc(void (*p_collect_func)(void))
{
	pf_d78_collect_func = p_collect_func;
	d78_check_sos();
}


