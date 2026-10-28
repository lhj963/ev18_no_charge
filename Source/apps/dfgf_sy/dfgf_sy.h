#ifndef __DFGF_SY_H__
#define __DFGF_SY_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "sysapi.h"
#define DFGFHEAD0 0x23
#define DFGFHEAD1 0x23

#define DFGF_VER_MAJ 0x05
#define DFGF_VER_MIN 0x08

#define DFGF_CONTENT_OFFSET 2+2+1+1+GB_LEN_VIN+1+1+2+2

#define DFGF_MIN_PACK_LEN (DFGF_CONTENT_OFFSET+1)

//起始符(2B)+协议版本(2B)+命令字(1B)+应答标志(1B)+VIN(17B)+数据压缩（1B）
//+加密方式(1B)+指令流水号（2B）+数据单元长度(2B)+数据单元内容(usDataUnitLen)+校验(1B)

#define DFGF_VERSION_OFFSET     			2
#define DFGF_CMD_OFFSET     				4
#define DFGF_CMD_ACK_FLAG_OFFSET    		5
#define DFGF_VIN_OFFSET						6
#define DFGF_YASO_WAY_OFFSET				23
#define DFGF_JIAMI_WAY_OFFSET				24
#define DFGF_CMD_FLOWNUM_OFFSET				25
#define DFGF_CONTENT_LEN_OFFSET				27


#define DFGF_LOGIN_CMD 						0x01
#define DFGF_REAL_DATA_CMD 					0x02
#define DFGF_BF_DATA_CMD 					0x03
#define DFGF_LOGOUT_CMD 					0x04
#define DFGF_CTRL_CMD 						0x05
#define DFGF_CTRL_CMD_ACK 					0x06
#define DFGF_WARNING_UP		 				0x07
#define DFGF_HEART_BEAT_CMD					0x08

#define DFGF_PLAT_QUERY_PARAM_CMD	0x80		//平台查询参数
#define DFGF_PLAT_SET_PARAM_CMD		0x81		//平台设置参数

#define MAX_QUDONG_DIANJI_NUM       0x02		//最大驱动电机个数
#define MAX_UPLOAD_SEC_NUM          0x08		//所需上传的内容
#define MAX_WARN_INFO_NUM			0x08		//故障码信息结构体个数
#define MAX_LINGJIAN_CNT 			0x04		//最大零件个数

#define DFGF_TBOX_STATUS			0x01		//TBOX状态
#define DFGF_DFAC_INFO 				0x02		//新能源业务拓展数据
#define DFGF_WARN_INFO 				0x03		//故障码信息
#define DFGF_LINGJIAN_INFO 			0x04		//零件属性数据
#define DFGF_YANFA_INFO 			0x05		//研发拓展数据
#define DFGF_CAR_CONNECT_INFO 		0x06		//智能网联车辆数据
#define DFGF_CAR_V2X_INFO 			0x07		//智能网联汽车V2X数据
#define DFGF_CAR_STATUS_INFO 		0x08		//车辆状态状态监控数据

#define setbit(x,y) x|=(1<<y) //将X的第Y位置1
#define clrbit(x,y) x&=!(1<<y) //将X的第Y位清0

typedef struct tagDFGFTboxStatus
{
	INT8U is_position;						//定位状态
	INT8U stars_count;						//GPS卫星数量
	INT32U gps_lon;							//经度
	INT32U gps_lat;							//维度
	INT16U gps_speed;						//GPS速度
	INT8U gps_angle;						//方向
	INT16U gps_haiab;						//海拔
	INT8U gps_signal;						//GPS信号强度
	INT16U jizhan_lac;						//基站LAC
	INT16U jizhan_cid;						//基站CID
	INT32U main_dianya;						//B+电压
	INT8U signal_4g;						//4G信号强度
	INT8U dev_num_len;						//设备号长度
	INT8U dev_num[32];						//设备号
	INT8U imei_num_len;						//设备IMEI号长度
	INT8U imei_num[32];						//设备IMEI
	INT8U budian_status;					//补电状态
}DFGFTboxStatus;

DFGFTboxStatus s_DFGFTboxStatu = {0};

typedef struct tagDFGFDfacInfoPower_dianji
{
//	INT8U dianji_num;						//驱动电机序号
	INT8U dianji_status;					//驱动电机状态
	
	INT8U dianji_ctrl_temp;					//驱动电机控制器温度
	INT16U dianji_rpm;						//驱动电机转速
	INT16U dianji_tor;						//驱动电机转矩
	INT8U dianji_temp;						//驱动电机温度
	
	INT16U dianji_dianya;					//电机控制器输入电压
	INT16U dianji_dianliu;					//电机控制器直流母线电流
	
}DFGFDfacInfoPower_dianji;

typedef struct tagDFGFDfacInfoPower
{
	INT8U car_status;						//车辆状态
	INT8U key_dangwei;						//钥匙档位
	INT8U dangwei;							//档位
	INT8U charge_in;						//充电
	INT16U car_speed;						//车速
	INT8U jiasu_taban;						//加速踏板开度
	INT8U zhidong_taban;					//制动踏板开度
	INT8U hand_brk;							//手刹
	INT32U 	run_time;						//总运行时长
	INT32U 	licheng;						//累计里程
	INT16U dianchi_yulaing;					//电池容量
	INT8U 	soc_value;						//SOC
	INT8U 	dcdc_status;					//DCDC工作状态
	INT16U 	zong_dianya;					//电池系统总电压
	INT16U 	zong_dianliu;					//电池系统总电流
	INT8U charge_connect_status;			//充电连接状态
	INT8U charging_status;					//充电时状态
	INT16U charge_dianya;					//充电电压
	INT16U charge_dianliu;					//充电电流
	INT16U 	jueyuan_dianzu;					//绝缘电阻
	//INT8U engine_status;							//发动机状态
	INT16U quzhou_rpm;							//曲轴转速
	INT8U 	dianji_cnt;						//驱动电机个数
	DFGFDfacInfoPower_dianji stDFGFDfacInfoPower_dianji[MAX_QUDONG_DIANJI_NUM];
	
	INT32U xuhang_mail ;					//续航里程
	INT8U dianchi_shuaijian ;				//电池衰减度
	INT16U shunshi_dianhao;					//瞬时电耗
	INT32U bms_xuhang_mail;					//BMS续航里程
	INT32U zong_charge_power;				//累计充入电量
	INT32U zong_loss_power;					//累计放电电量
	INT16U charge_power;					//单次充入电量
	INT32U zong_huikui_power;				//累计回馈电量
	
	INT32U charge_gonglv;					//充电功率
	
	INT16U charge_time_shengyu;				//充电剩余时间
	
	INT8U charge_end_level;					//充电结束优先级
	INT8U dianchi_jiare_status;				//电池加热状态
	INT8U max_dianchi_wendu;				//最高电池温度
}DFGFDfacInfoPower;

DFGFDfacInfoPower stDFGFDfacInfoPower = {0xFF};

typedef struct tagDFGFDfacInfoChassis
{
	//INT16U zaizhong_value;				//货车载重
	INT16U fangxaingpan_angle;				//方向盘角度信号
	INT8U shache_warn_status;				//刹车磨损报警状态
	INT8U xunhang_status;					//定速巡航
	INT8U xunhang_speed;					//巡航速度
	INT8U taiya_data[6];					//胎压数据
	INT8U aeb_status;						//AEB状态
}DFGFDfacInfoChassis;

DFGFDfacInfoChassis stDFGFDfacInfoChassis = {0xFF};

typedef struct tagDFGFDfacInfoCarBody
{
	INT8U door_lock_status;					//门锁开关状态
	INT8U safe_bind_status;					//安全带状态
	INT8U windows_status;					//车窗状态
	INT16U light_status;					//车灯状态
	INT16U air_cond_status;					//空调状态
	INT8U feng_mode;						//吹风模式
	INT8U feng_speed;						//风速
	INT8U air_cond_temper;					//空调温度
	INT8U air_cond_temper_disp;					//空调温度显示
	INT8U env_temper;						//环境温度
	INT8U driver_env_temper;				//驾驶室内温度
	
	INT8U cond_fan_pwm;						//风扇控制占空比
	INT16U cond_fan_rpm;					//压缩机转速
	INT8U fangxaingpan_jiare_status;		//方向盘加热状态
	INT8U fan_error_status;				//风扇故障状态
	
}DFGFDfacInfoCarBody;

DFGFDfacInfoCarBody stDFGFDfacInfoCarBody = {0xFF};
/*
typedef struct tagDFGFDfacInfoCabin//座舱 预留
{
}DFGFDfacInfoCabin;

typedef struct tagDFGFDfacInfoDriv//智能驾驶 预留
{
}DFGFDfacInfoDriv;
*/

typedef struct tagDFGFDfacInfoLockStatus//车辆锁车状态
{	
	//电池电压
	INT8U acc_status;//ACC状态 send_msg(S_MSG_GET_ACC_STATUS,0,0,0)
	//手刹状态
	INT8U zhidong_status;//制动开关
	INT8U youmen_status;//油门开度
	INT8U driver_zuoyi_status;//驾驶位座椅状态
	INT8U jihuo_status;///当前激活状态
	INT16U lockcar_status;//当前锁车状态

}DFGFDfacInfoLockStatus;

DFGFDfacInfoLockStatus stDFGFDfacInfoLockStatus = {0xFF};
/*
typedef struct tagDFGFDfacInfoLockCarStatus//锁车发送机及车辆数据
{
	INT8U zhudong_lockcar;//主动锁车
	INT8U beidong_lockcar;//被动锁车
	INT16U limit_speed;//转速限制
	INT16U limit_time;//锁车倒计时
	INT16U unlimit_time;//临时解锁倒计时
	INT8U lockcar_manul_key;//ManulKey有效数
	INT8U lockcar_pro_ver;//已激活锁车版本
	
}DFGFDfacInfoLockCarStatus;
DFGFDfacInfoLockCarStatus stDFGFDfacInfoLockCarStatus = {0xFF};
*/
/*
typedef struct tagDFGFDfacInfo
{
}DFGFDfacInfo;
*/

/*
if(pdat[0] > 0)
{
	stDFGFWarnInfo[0].uds_addr = canid;
	stDFGFWarnInfo[0].protocal = 0;
	stDFGFWarnInfo[0].sos_level = ;
	stDFGFWarnInfo[0].sos_num = ;
	stDFGFWarnInfo[0].dtc_num = ;
	stDFGFWarnInfo[0].dtc_data_len = 64;
	memcpy(stDFGFWarnInfo[0].dtc_data,pdat,8); 
	
}
*/
typedef struct tagDFGFWarnInfo
{
	INT8U dtc_flag;
	INT32U dtc_code[MAX_WARN_INFO_NUM];//故障码
}DFGFWarnInfo;

DFGFWarnInfo stDFGFWarnInfo = {0};
/*
#define MAX_LEN_JIANCHENG				0x08
#define MAX_LEN_GONGYINGSHANG 			0x08
#define MAX_LEN_ZONGCHENG 				0x08
#define MAX_LEN_YINGJIAN 				0x08
#define MAX_LEN_RUANJIAN 				0x08
typedef struct tagDFGFLingJianInfoPart
{
	INT8U lingjian_uds_addr;//UDS物理地址
	INT8U jiancheng_len;//零件简称长度
	INT8U jiancheng[MAX_LEN_JIANCHENG];//零件简称
	INT8U gongyingshang_code_len;//供应商编码长度
	INT8U gongyingshang_code[MAX_LEN_GONGYINGSHANG];//供应商编码
	INT8U zongcheng_code_len;//零件号总成号长度
	INT8U zongcheng_code[MAX_LEN_ZONGCHENG];//零件总成号
	INT8U yingjian_ver_len;//硬件版本长度
	INT8U yingjian_ver[MAX_LEN_YINGJIAN];//硬件版本
	INT8U soft_ver_len;//软件版本号长度 
	INT8U soft_ver[MAX_LEN_RUANJIAN];//软件零件号 
	
}DFGFLingJianInfoPart;

typedef struct tagDFGFLingJianInfo
{
	INT8U lingjian_cnt;
	DFGFLingJianInfoPart stDFGFLingJianInfoPart[MAX_LINGJIAN_CNT];
}DFGFLingJianInfo;

DFGFLingJianInfo stDFGFLingJianInfo = {0};
*/
typedef struct tagDFGFYanfaInfo
{
	INT8U connect_comfirm ;					//快充1连接确认信号
	INT16U connect_status;					//CC连接状态
	INT16U dcdc_dianya;						//DCDC输出电压
	INT8U dcdc_dianliu;						//DCDC输出电流
	INT8U dcdc_temper;						//DCDC温度
	INT16U dcdc_gonglv;						//DCDC输入功率
	INT8U cond_dianya;						//压缩机母线电压
	INT8U cond_dianliu;						//压缩机母线电流
	INT8U cond_temper;						//压缩机内部温度
	INT8U cond_gonglv;						//压缩机的实际功率
	INT8U wptc_wendu;						//WPTC出口温度
	INT8U wptc_dianliu;						//WPTC电流值
	INT8U wptc_dianya;						//WPTC高压值
	INT8U wptc_gonglv;						//WPTC功率值
	
}DFGFYanfaInfo;
DFGFYanfaInfo stDFGFYanfaInfo;
/*
typedef struct tagDFGFCarConnectInfo
{
}DFGFCarConnectInfo;


typedef struct tagDFGFCarV2xInfo
{
}DFGFCarV2xInfo;


typedef struct tagDFGFCarStatusInfo
{
}DFGFCarStatusInfo;
*/
//轮胎报警数据
typedef struct tagDFGFCarTireInfo
{
	INT8U tire_weizhi;//位置
	INT8U tire_value;//胎压
	INT8U tire_status;//阈值状态
	
}DFGFCarTireInfo;

typedef struct tagDFGFCarTireSOSInfo
{
	DFGFCarTireInfo stDFGFCarTireInfo[6];//轮胎信息
	
}DFGFCarTireSOSInfo;

DFGFCarTireSOSInfo stDFGFCarSOSInfo = {0xFF};

typedef struct tagDFGFDataInfo
{
	unsigned char login_flow_cnt;
}DFGFDataInfo;

typedef struct tagDFGFDataIUpload
{
	unsigned char id;
	unsigned char is_upload;
	unsigned char upload_freq;
}DFGFDataUpload;

typedef struct tagDFGFLOGUpload
{
	unsigned char cmd;
	unsigned char ftp_path[128];
	unsigned char ftp_user_name[32];
	unsigned char ftp_user_passwd[16];
}DFGFLOGUpload;

typedef struct tagDFGFCANUpload
{
	unsigned char file_format;
	unsigned char protocol;
	unsigned char passageway;
	unsigned char file_mode;
	unsigned char dtc_code;
	unsigned char ftp_path[128];
	unsigned char ftp_user_name[32];
	unsigned char ftp_user_passwd[16];
}DFGFCANUpload;

typedef struct tagDFGFCameraUpload
{
	unsigned char switch_camera;
	unsigned char camera_path[128];
}DFGFCameraUpload;



typedef struct tagDFGFEDRUpload
{
	unsigned char start_time[6];
	unsigned char end_time[6];
	unsigned char ftp_path[128];
	unsigned char ftp_user_name[32];
	unsigned char ftp_user_passwd[16];
	unsigned char edr_path[128];
}DFGFEDRUpload;

typedef struct tagDFGFAirCtrl
{
	unsigned char start_time[6];
	unsigned char AC_switch;
	unsigned char ptc_switch;
	unsigned char shuang_switch;
	unsigned char xunhuan_mode;
	unsigned char temper_value;
	unsigned char temper_dangwei;
	unsigned char feng_dangwei;
	unsigned char shushi_mode;
}DFGFAirCtrl;

typedef struct tagDFGFDianchiCtrl
{
	unsigned char start_time[6];
	unsigned char jiare_switch;
}DFGFDianchiCtrl;

typedef struct tagDFGFChargeCtrl
{
	unsigned char start_time[6];
	unsigned char end_time[6];
	unsigned char charge_switch;
	unsigned char charge_time;//充电时长，分钟
	unsigned char charge_value;
	unsigned char charge_youxainji;//充电结束优先级
	unsigned short charge_limit;//充电功率限制
}DFGFChargeCtrl;

typedef struct tagDFGFShangzhuangCtrl
{
	unsigned char tmeper_value;
	unsigned char shidu_value;
}DFGFShangzhuangCtrl;

typedef struct tagDFGFFangxaingpanCtrl
{
	unsigned char start_time[6];
	unsigned char jiare_switch;
}DFGFFangxaingpanCtrl;

typedef struct tagDFGFZuoyiCtrl
{
	unsigned char start_time[6];
	unsigned char jiare_switch;
	unsigned char tongfeng_switch;
}DFGFZuoyiCtrl;

typedef struct tagDFGFCtrlStatus
{
	//unsigned char ON_status;// 远程ON档启动/关闭 可立即实现
	unsigned char door_window_status[3];// 远程门窗上锁/解锁
	DFGFAirCtrl stDFGFAirCtrl;// 远程空调
	unsigned char lookup_car;
	DFGFDianchiCtrl stDFGFDianchiCtrl;
	DFGFChargeCtrl stDFGFChargeCtrl;
	unsigned char horn_ctrl;
	unsigned char light_ctrl;
	DFGFShangzhuangCtrl stDFGFShangzhuangCtrl;
	DFGFFangxaingpanCtrl stDFGFFangxaingpanCtrl;
	DFGFZuoyiCtrl stDFGFZuoyiCtrl;
	unsigned char driver_ctrl;
}DFGFCtrlStatus;

typedef struct tagDFGFServerCmd
{
	DFGFDataUpload stDFGFDataUpload[MAX_UPLOAD_SEC_NUM];
	DFGFLOGUpload stDFGFLOGUpload;
	DFGFCANUpload stDFGFCANUpload;
	DFGFCameraUpload stDFGFCameraUpload;
	DFGFEDRUpload stDFGFEDRUpload;
	DFGFCtrlStatus stDFGFCtrlStatus;
}DFGFServerCmd;

DFGFServerCmd stDFGFServerCmd = {0};
/*
void dfgf_can_set_dianji_num(void);
INT8U dfgf_can_get_dianji_num(INT8U num);
*/
void dfgf_can_set_dianji_status(INT8U num, INT8U status);
INT8U dfgf_can_get_dianji_status(INT8U num);

void dfgf_can_set_dianji_ctrl_temp(INT8U num, INT8U status);
INT8U dfgf_can_get_dianji_ctrl_temp(INT8U num);

void dfgf_can_set_dianji_rpm(INT8U num, INT16U status);
INT16U dfgf_can_get_dianji_rpm(INT8U num);

void dfgf_can_set_dianji_tor(INT8U num, INT16U status);
INT16U dfgf_can_get_dianji_tor(INT8U num);

void dfgf_can_set_dianji_temp(INT8U num, INT8U status);
INT8U dfgf_can_get_dianji_temp(INT8U num);

void dfgf_can_set_dianji_dianya(INT8U num, INT16U status);
INT16U dfgf_can_get_dianji_dianya(INT8U num);

void dfgf_can_set_dianji_dianliu(INT8U num, INT16U status);
INT16U dfgf_can_get_dianji_dianliu(INT8U num);

void dfgf_can_set_car_status(INT8U status);
INT8U dfgf_can_get_car_status(void);

void dfgf_can_set_dangwei(INT8U status);
INT8U dfgf_can_get_dangwei(void);

void dfgf_can_set_car_speed(INT16U status);
INT16U dfgf_can_get_car_speed(void);

void dfgf_can_set_jiasu_taban(INT16U status);
INT8U dfgf_can_get_jiasu_taban(void);

void dfgf_can_set_zhidong_taban(INT8U status);
INT8U dfgf_can_get_zhidong_taban(void);

INT32U dfgf_can_get_run_time(void);

void dfgf_can_set_licheng(INT32U status);
INT32U dfgf_can_get_licheng(void);

void dfgf_can_set_soc_value(INT8U status);
INT8U dfgf_can_get_soc_value(void);

void dfgf_can_set_dcdc_status(INT8U status);
INT8U dfgf_can_get_dcdc_status(void);

void dfgf_can_set_zong_dianya(INT16U status);
INT16U dfgf_can_get_zong_dianya(void);

void dfgf_can_set_zong_dianliu(INT16U status);
INT16U dfgf_can_get_zong_dianliu(void);

void dfgf_can_set_jueyuan_dianzu(INT16U status);
INT16U dfgf_can_get_jueyuan_dianzu(void);

void dfgf_can_set_dianji_cnt(INT8U status);
INT8U dfgf_can_get_dianji_cnt(void);

//动能回收状态
void dfgf_can_set_power_recovery_status(INT8U status);
INT8U dfgf_can_get_power_recovery_status(void);

//动能转化率
void dfgf_can_set_power_recovery_value(INT8U status);
INT8U dfgf_can_get_power_recovery_value(void);

void dfgf_can_set_key_dangwei(INT8U status);
INT8U dfgf_can_get_key_dangwei(void);

void dfgf_can_set_hand_brk(INT8U status);
INT8U dfgf_can_get_hand_brk(void);

void dfgf_can_set_dianchi_yulaing(INT16U status);
INT16U dfgf_can_get_dianchi_yulaing(void);

void dfgf_can_set_charge_connect_status(INT8U status);
INT8U dfgf_can_get_charge_connect_status(void);

void dfgf_can_set_charging_status(INT8U status);
INT8U dfgf_can_get_charging_status(void);

void dfgf_can_set_charge_dianya(INT16U status);
INT16U dfgf_can_get_charge_dianya(void);

void dfgf_can_set_charge_dianliu(INT16U status);
INT16U dfgf_can_get_charge_dianliu(void);

void dfgf_can_set_fangxaingpan_angle(INT16U status);
INT16U dfgf_can_get_fangxaingpan_angle(void);

void dfgf_can_set_shache_warn_status(INT8U status);
INT8U dfgf_can_get_shache_warn_status(void);

void dfgf_can_set_xunhang_status(INT8U status);
INT8U dfgf_can_get_xunhang_status(void);

void dfgf_can_set_xunhang_speed(INT8U status);
INT8U dfgf_can_get_xunhang_speed(void);

void dfgf_can_set_door_lock_status(INT8U status);
INT8U dfgf_can_get_door_lock_status(void);

void dfgf_can_set_safe_bind_status(INT8U status);
INT8U dfgf_can_get_safe_bind_status(void);

void dfgf_can_set_windows_status(INT8U status);
INT8U dfgf_can_get_windows_status(void);

void dfgf_can_set_light_status_0(INT16U wei, INT8U value);
void dfgf_can_set_light_status(INT16U status);
INT16U dfgf_can_get_light_status(void);

void dfgf_can_set_air_cond_status(INT16U status);
INT16U dfgf_can_get_air_cond_status(void);

void dfgf_can_set_feng_mode(INT8U status);
INT8U dfgf_can_get_feng_mode(void);

void dfgf_can_set_feng_speed(INT8U status);
INT8U dfgf_can_get_feng_speed(void);

void dfgf_can_set_air_cond_temper(INT8U status);
INT8U dfgf_can_get_air_cond_temper(void);

void dfgf_can_set_env_temper(INT8U status);
INT8U dfgf_can_get_env_temper(void);

void dfgf_can_set_driver_env_temper(INT8U status);
INT8U dfgf_can_get_driver_env_temper(void);

INT8U dfgf_can_get_acc_status(void);

void dfgf_can_set_zhidong_status(INT8U status);
INT8U dfgf_can_get_zhidong_status(void);

void dfgf_can_set_youmen_status(INT8U status);
INT8U dfgf_can_get_youmen_status(void);

void dfgf_can_set_driver_zuoyi_status(INT8U status);
INT8U dfgf_can_get_driver_zuoyi_status(void);

void dfgf_can_set_jihuo_status(INT8U status);
INT8U dfgf_can_get_jihuo_status(void);

void dfgf_can_set_lockcar_status(INT8U status);
INT8U dfgf_can_get_lockcar_status(void);

void dfgf_can_set_zhudong_lockcar(INT8U status);
INT8U dfgf_can_get_zhudong_lockcar(void);

void dfgf_can_set_beidong_lockcar(INT8U status);
INT8U dfgf_can_get_beidong_lockcar(void);

void dfgf_can_set_limit_speed(INT16U status);
INT16U dfgf_can_get_limit_speed(void);

void dfgf_can_set_limit_time(INT16U status);
INT16U dfgf_can_get_limit_time(void);

void dfgf_can_set_unlimit_time(INT16U status);
INT16U dfgf_can_get_unlimit_time(void);

void dfgf_can_set_lockcar_manul_key(INT8U status);
INT8U dfgf_can_get_lockcar_manul_key(void);

void dfgf_can_set_lockcar_pro_ver(INT8U status);
INT8U dfgf_can_get_lockcar_pro_ver(void);

void dfgf_can_set_uds_addr(INT16U status);
INT16U dfgf_can_get_uds_addr(void);

void dfgf_can_set_protocal(INT8U status);
INT8U dfgf_can_get_protocal(void);

void dfgf_can_set_sos_level(INT8U status);
INT8U dfgf_can_get_sos_level(void);

void dfgf_can_set_sos_num(INT8U status);
INT8U dfgf_can_get_sos_num(void);

void dfgf_can_set_dtc_num(INT8U status);
INT8U dfgf_can_get_dtc_num(void);

void dfgf_can_set_dtc_data_len(INT8U status);
INT8U dfgf_can_get_dtc_data_len(void);

void dfgf_can_set_dtc_data(INT8U* status);
INT8U* dfgf_can_get_dtc_data(void);

void dfgf_can_set_lingjian_uds_addr(INT8U i, INT8U status);
INT8U dfgf_can_get_lingjian_uds_addr(INT8U i);

void dfgf_can_set_jiancheng_len(INT8U i, INT8U status);
INT8U dfgf_can_get_jiancheng_len(INT8U i);

void dfgf_can_set_jiancheng(INT8U i, INT8U* status);
INT8U* dfgf_can_get_jiancheng(INT8U i);

void dfgf_can_set_gongyingshang_code_len(INT8U i, INT8U status);
INT8U dfgf_can_get_gongyingshang_code_len(INT8U i);

void dfgf_can_set_gongyingshang_code(INT8U i, INT8U* status);
INT8U* dfgf_can_get_gongyingshang_code(INT8U i);

void dfgf_can_set_zongcheng_code_len(INT8U i, INT8U status);
INT8U dfgf_can_get_zongcheng_code_len(INT8U i);

void dfgf_can_set_zongcheng_code(INT8U i, INT8U* status);
INT8U* dfgf_can_get_zongcheng_code(INT8U i);

void dfgf_can_set_yingjian_ver_len(INT8U i, INT8U status);
INT8U dfgf_can_get_yingjian_ver_len(INT8U i);

void dfgf_can_set_yingjian_ver(INT8U i, INT8U* status);
INT8U* dfgf_can_get_yingjian_ver(INT8U i);

void dfgf_can_set_soft_ver_len(INT8U i, INT8U status);
INT8U dfgf_can_get_soft_ver_len(INT8U i);

void dfgf_can_set_soft_ver(INT8U i, INT8U* status);
INT8U* dfgf_can_get_soft_ver(INT8U i);

void dfgf_can_set_connect_comfirm(INT8U status);
INT8U dfgf_can_get_connect_comfirm(void);

void dfgf_can_set_connect_status(INT16U status);
INT16U dfgf_can_get_connect_status(void);

void dfgf_can_set_dcdc_dianya(INT16U status);
INT16U dfgf_can_get_dcdc_dianya(void);

void dfgf_can_set_dcdc_dianliu(INT8U status);
INT8U dfgf_can_get_dcdc_dianliu(void);

void dfgf_can_set_dcdc_temper(INT8U status);
INT8U dfgf_can_get_dcdc_temper(void);

void dfgf_can_set_dcdc_gonglv(INT16U status);
INT16U dfgf_can_get_dcdc_gonglv(void);

void dfgf_can_set_cond_dianya(INT8U status);
INT8U dfgf_can_get_cond_dianya(void);

void dfgf_can_set_cond_dianliu(INT8U status);
INT8U dfgf_can_get_cond_dianliu(void);

void dfgf_can_set_cond_temper(INT8U status);
INT8U dfgf_can_get_cond_temper(void);

void dfgf_can_set_cond_gonglv(INT8U status);
INT8U dfgf_can_get_cond_gonglv(void);

void dfgf_can_set_wptc_wendu(INT8U status);
INT8U dfgf_can_get_wptc_wendu(void);

void dfgf_can_set_wptc_dianliu(INT8U status);
INT8U dfgf_can_get_wptc_dianliu(void);

void dfgf_can_set_wptc_dianya(INT8U status);
INT8U dfgf_can_get_wptc_dianya(void);

void dfgf_can_set_wptc_gonglv(INT8U status);
INT8U dfgf_can_get_wptc_gonglv(void);

void dfgf_can_set_tire(INT8U weizhi, INT8U value, INT8U status);

void DFGFParam_Init(void);
unsigned int _dfgf_DataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,INT8U ack_flag,INT16U cmdNumber,void *pVIN,void *pDataUnit,INT16U usDataUnitLen);
unsigned int DFGF_LoginPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,INT8U time[6]);

unsigned int get_dfgf_data_unit(void *pOut,unsigned int outbuflen,
												INT8U time[6]);
void collect_dfgf_pack(void);

void anlyze_dfgf_sy_server_data(unsigned int linkid,void *pbuf,unsigned int len);

void dfgf_sy_analyze(unsigned char channel,CanRxMsg *prxmsg);

#ifdef __cplusplus
}
#endif

#endif


