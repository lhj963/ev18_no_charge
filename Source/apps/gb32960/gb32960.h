#ifndef __GB32960_H__
#define __GB32960_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GBHEAD0 0x23
#define GBHEAD1 0x23

#define GB32960_CONTENT_OFFSET (2+1+1+GB_LEN_VIN+1+2)


#define GB32960_MIN_PACK_LEN (2+2+17+1+2+1)

#define GB32960_CMD_OFFSET     				2
#define GB32960_CMD_ACK_FLAG_OFFSET    		3
#define GB32960_VIN_OFFSET					4
#define GB32960_JIAMI_WAY_OFFSET			21
#define GB32960_CONTENT_LEN_OFFSET			22


#define GB32960_LOGIN_CMD 				0x01
#define GB32960_REAL_DATA_CMD 			0x02
#define GB32960_BF_DATA_CMD 			0x03
#define GB32960_LOGOUT_CMD 				0x04
#define GB32960_HEART_BEAT_CMD 			0x07
#define GB32960_JS_CMD 					0x08
#define GB32960_ZC_CMD 					0x37

#define GB32960_PLAT_QUERY_PARAM_CMD	0x80		//平台查询参数
#define GB32960_PLAT_SET_PARAM_CMD		0x81		//平台设置参数
#define GB32960_PLAT_CTRL_CMD			0x82		//平台控制命令

#define ZUOCANG_DATA_INFO			0x61		//座舱数据信息类型

#define ZC_DATA_INFO			0x01		//整车数据信息类型
#define QDDJ_INFO 				0x02			//驱动电机数据
#define RLDC_INFO 				0x03			//燃料电池信息类型
#define ECU_INFO 				0x04			//发动机信息类型
#define CAR_POS_INFO 			0x05		//位置信息类型
#define JIZHI_INFO 				0x06		//极值信息类型
#define SOS_INFO 				0x07		//报警信息类型
#define VOL_INFO 				0x08		//电压数据类型
#define TEMP_INFO 				0x09		//温度数据类型


#define	GB_LEN_VIN			17		//VIN长度
#define GB_LEN_SOFTVER		18		//软件标定识别号长度
#define GB_LEN_CVN			18		//标定验证码
#define GB_LEN_IUPR         36
#define GB_LEN_ICCID        20



#define MAX_DJ_CNT  					2
#define MAX_TAN_ZHEN_CNT 				255		//温度探针总数
#define MAX_CAN_CHARGE_DTC_CNT   		5		//最大可充电装置故障
#define MAX_QDDJ_DTC_CNT   				5		//驱动电机故障
#define MAX_ECU_DTC_CNT   				4		//发动机故障
#define MAX_OTHER_DTC_CNT   			20		//其他故障总数
#define MAX_DIANCHI_CNT    				2		//电池个数 储能系统个数
#define DANTI_DIANCHI_CNT_EVERY_DIANCHI 255		//每个电池里面最多有多少个单体电池
#define WENDU_TANZHEN_CNT_EVERY_DIANCHI 255		//每个电池里面最多有多少个温度探针


extern void gb32960_analyze(unsigned char channel,CanRxMsg *prxmsg);

extern void gb32960_fresh(void);

extern void GB32960_Init(void *pvin,INT8U way,INT16U *pLoginFlow,unsigned int flow_cnt);


//登录
extern unsigned int GB32960_LoginPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,
			INT8U time[6],void *iccid,INT8U charge_cnt,
			void *pCode,INT8U code_len);


//登出
extern unsigned int GB32960_LogoutPack(void *pOutBuf,unsigned int outbuflen,unsigned char linkid,INT8U time[6]);

//心跳
extern unsigned int GB32960_HeartBeatPack(void *pOutBuf,unsigned int outbuflen);

//校时
extern unsigned int GB32960_JSPack(void *pOutBuf,unsigned int outbuflen);



extern unsigned int xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);


extern unsigned int xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
													INT8U time[6],
													INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int D78_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int D78_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);



/*--------------------时代电服相关---------------------------*/
extern unsigned int sddf_xGB32960_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int sddf_xGB32960_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int sddf_BasicDataPack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6]);

extern unsigned int sddf_charge_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6]);

extern unsigned int sddf_ack_charge_limit_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U time[6]);

extern unsigned int sddf_LoginPlatPack(void *pVIN,void *puser,void *psw,void *pOut,unsigned int outbuflen);

extern unsigned char get_sddf_login_ok_flag(void);

extern void set_sddf_login_ok_flag(unsigned char val);

extern unsigned char get_gb32960_login_ok_flag(unsigned char linkid);
extern void set_gb32960_login_ok_flag(unsigned char linkid, unsigned char val);


extern void set_0c0728d0_flag(unsigned char val);
extern unsigned char get_0c0728d0_flag(void);



extern unsigned int xDataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,void *pDataUnit,INT16U usDataUnitLen);

extern unsigned int _xDataBag(void *pOutBuf,INT16U usOutBufSize,INT8U cmd,INT8U ack_flag,void *pVIN,void *pDataUnit,INT16U usDataUnitLen);


extern INT8U is_get_milage_id(void);
extern INT8U get_d78_huandian_ver(void);
extern void set_d78_huandian_ver(INT8U val);


extern INT8U get_dst_want_bms_charge_flag(void);
extern void set_dst_want_bms_charge_flag(INT8U val);
extern INT8U get_dst_fact_bms_charge_flag(void);
extern void set_dst_fact_bms_charge_flag(INT8U val);
extern void set_dst_budian_init_vol(unsigned int val);
extern unsigned int get_dst_budian_init_vol(void);

extern INT8U get_lock_car_can_channel(void);
extern void set_lock_car_can_channel(INT8U val);
extern INT8U *get_iccid_bytes_in_ecu(void);
extern void set_iccid_bytes_in_ecu(INT8U *piccid_bytes,unsigned int datlen);
extern INT8U get_iccid_head_flag_in_ecu(void);
extern void set_iccid_head_flag_in_ecu(INT8U val);
extern INT8U *get_iccid_head_bytes_in_ecu(void);
extern void set_iccid_head_bytes_in_ecu(INT8U *piccid_head_bytes,unsigned int datlen);
extern INT8U get_want_bind_status(void);
extern INT8U get_fact_bind_status(void);
extern INT8U get_fact_pro_ver_about_lock(void);
extern INT8U get_fact_lock_status(void);
extern void set_want_bind_status(INT8U val);
extern void set_fact_bind_status(INT8U val);
extern void set_fact_pro_ver_about_lock(INT8U val);
extern void set_fact_lock_status(INT8U val);

extern INT16U gb32960_can_get_cnxt_dianya(INT8U idx);
extern INT16U gb32960_can_get_cnxt_dianliu(INT8U idx);
extern INT8U gb32960_can_get_soc(void);
extern INT8U gb32960_can_get_qddj_cnt(void);

extern void gb32960_can_set_milage(INT32U val);
extern INT32U gb32960_can_get_milage(void);
extern void gb32960_can_set_speed(INT16U val);
extern INT16U gb32960_can_get_speed(void);
extern INT16U gb32960_can_get_total_dianliu(void);
extern INT16U gb32960_can_get_total_dianya(void);


//车辆状态
extern INT8U gb32960_can_get_car_status(void);
extern INT8U gb32960_can_get_charge_status(void);
extern INT8U gb32960_can_get_dianchi_cnt(void);

extern void gb32960_can_set_max_sos_level(INT8U val);
extern INT8U gb32960_can_get_max_sos_level(void);
extern void gb32960_can_set_sos_flag_by_bit(INT8U whitch_bit,INT8U val);
extern void gb32960_can_set_sos_flag(INT32U val);
extern INT32U gb32960_can_get_sos_flag(void);

extern void gb32960_can_set_sos_test(INT8U val);

extern char *gb32960_get_car_status_str(INT8U val);
extern char *gb32960_get_charge_status_str(INT8U val);




//采集时间间隔
extern INT16U gb32960_get_caiji_time_inteval(void);

extern void gb32960_set_caiji_time_inteval(INT16U val);

extern INT8U gb32960_get_moni_enter_mq_flag(void);

extern void gb32960_set_moni_enter_mq_flag(INT8U val);



extern void gb32960_moni_data_proc(void);


extern INT32U get_sos_collect_time(void);

extern void set_sos_collect_time(INT32U val);

extern char is_car_xh(void);

extern void gb32960_proc(void);

extern void gb32960_ack_plat(INT8U linkid,void *pdat,unsigned int len);

extern void sddf_ack_plat(INT8U linkid,void *pdat,unsigned int len);

extern char is_usually_collect(void);

extern void gb32960_sos_proc(void (*p_collect_func)(void));

extern void d78_gb32960_sos_proc(void (*p_collect_func)(void));


extern void set_s_32960_zuocang_data_change(unsigned char val);

extern unsigned char get_s_32960_zuocang_data_change(void);

extern int get_send_18X4D0D5_times(void);

extern void set_send_18X4D0D5_times(int temp);

extern void set_collect_32960_acc_status(unsigned char val);

extern unsigned char get_collect_32960_acc_status(void);

void set_want_startup_status(INT8U val);

INT8U get_want_startup_status(void);

void set_want_lockdoor_status(INT8U val);

INT8U get_want_lockdoor_status(void);

void set_lockdoor_commond_flag(INT8U val);

INT8U get_lockdoor_commond_flag(void);

INT8U get_lose_power_fail_status(void);

void set_lose_power_fail_status(INT8U val);

void set_lock_door_fail_status(INT8U val);

INT8U get_lock_door_fail_status(void);

extern void set_bcm_online_status(unsigned char val);

extern unsigned char get_bcm_online_status(void);
#ifdef __cplusplus
}
#endif

#endif



