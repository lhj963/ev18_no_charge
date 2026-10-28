#ifndef __SJZ_PLAT_H__
#define __SJZ_PLAT_H__
#ifdef __cplusplus
extern "C" {
#endif




//上行命令字
#define HD_PLAT_CMD_REQUEST_JS  		0xC0	//设备请求校时
#define HD_PLAT_CMD_BEIAN  				0x07	//备案
#define HD_PLAT_CMD_LOGIN  				0xC2 	//登录命令字
#define HD_PLAT_CMD_REAL_DATA  			0x02	//实时数据
#define HD_PLAT_CMD_BF_DATA  			0x03	//补发数据
#define HD_PLAT_CMD_SOS  				0x06	//拆除报警



//下行命令字
#define HD_PLAT_CMD_ACK_BEIAN  				0x08	//备案应答
#define HD_PLAT_CMD_ACK_LOGIN  				0xC3	//登录应答
#define HD_PLAT_CMD_ACK_DATA  				0xF0	//实时数据流信息应答


//信息类型
#define HD_PLAT_INFO_TYPE_OBD 				0x01  //OBD信息
#define HD_PLAT_INFO_TYPE_DATA_STREAM 		0x02  //实时数据流
#define HD_PLAT_INFO_TYPE_NO_ROAD 			0x81  //非道路数据流信息


typedef struct tagSJZStream
{
	unsigned char time[6];
	unsigned short speed;
	unsigned char kpa;
	unsigned char cActualTorquePercent;	//发动机实际扭矩与指示扭矩的百分比 范围:-125~125%
	unsigned char cFrictionTorquePercent;	//摩擦扭矩百分比 范围:-125~125%
	unsigned short rpm;
	unsigned short fuelflow;
	//unsigned int Nox_out;

	unsigned short up_Nox_out;
	unsigned short down_Nox_out;
	

	unsigned int sSCRInletTemperature;	//SCR入口温度 0.03125deg C/bit
	unsigned int sSCROutletTemperature;	//SCR出口温度

	unsigned int dpf;

	unsigned int usIntakeVolume;	//进气量 0.05Kg/h per bit
	unsigned char ucRemain; 	//反应剂余量 0~100%

	unsigned char oil_level;	//油箱液位

	unsigned char cCoolantTemperature;	//发动机冷却液温度

	unsigned char gps_status;		//0表示定位
	unsigned long lon;
	unsigned long lat;
	unsigned long milage;

	unsigned short guang_xishou_xishu;	//光吸收系数
	unsigned short bu_tou_guang_du;		//不透光度
	unsigned short klw_nd;			//颗粒物浓度
	
	unsigned char mil_status;		//mil状态


	unsigned char eng_status;//发动机状态 数据范围 0~2，“0”代表待机，“1”代表怠速，“2”代表行驶。“0xFF”表示无效

	unsigned short drive_cycle_cnt;	//驾驶循环数

	unsigned char obd_pro;		//诊断协议
	unsigned int support_status;

	unsigned char *pvin;
	unsigned char *pcalid;
	unsigned char *pcvn;
	unsigned char *piupr;

	unsigned char dtc;		//故障码总数
	unsigned char *gzm_list;

}SJZStream;


//模块初始化
extern void sjz_plat_init(void *vin,unsigned int vin_len,void *chipid,unsigned int chipid_len,
					void *pubkey,unsigned int pubkey_len);

//备案
extern unsigned int sjz_plat_beian_pack(unsigned char rtime[6],void *pOut,unsigned int outbuflen);


//登录
extern unsigned int sjz_plat_login_pack(unsigned char rtime[6],
							void *iccid,unsigned int iccid_len,
							void *imei,unsigned int imei_len,
							void *pOut,unsigned int outbuflen);


//实时多包
extern unsigned int sjz_plat_real_stream_report_multi_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,unsigned int cnt,
		void *pOut,unsigned int outbuflen);

//实时一包
extern unsigned int sjz_plat_real_stream_report_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,void *pOut,unsigned int outbuflen);


//补传多包
extern unsigned int sjz_plat_supply_stream_report_multi_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,unsigned int cnt,
		void *pOut,unsigned int outbuflen);

//补传一包
extern unsigned int sjz_plat_supply_stream_report_pack(unsigned char send_time[6],
		SJZStream *pSJZStream,void *pOut,unsigned int outbuflen);

//请求校时包
extern unsigned int sjz_plat_dev_request_js_pack(void *pOut,unsigned int outbuflen);


extern unsigned int sjz_plat_sos_pack(unsigned char rtime[6],unsigned char pos_status,INT32U lon,INT32U lat,void *pOut,unsigned int outbuflen);



//解包
extern unsigned int sjz_plat_unpack(void *pdat,unsigned int len,
			void (*pHandler)(unsigned char cmd,void *vin,unsigned char ver,unsigned char entype,void *content,unsigned int contentlen),
			void (*pErrorHandler)(void *,unsigned int));


extern void sjz_plat_analyze(unsigned char cmd,void *vin,unsigned char ver,unsigned char entype,void *content,unsigned int contentlen);



extern void sjz_set_beian_flag(unsigned char val);

extern unsigned char sjz_get_beian_flag(void);

extern void sjz_set_login_flag(unsigned char val);

extern unsigned char sjz_get_login_flag(void);



#ifdef __cplusplus
}
#endif
#endif



