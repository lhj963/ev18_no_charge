#ifndef __SAN_DIAN_OTA_H__
#define __SAN_DIAN_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif

//三电升级类型
typedef enum tagSANDIAN_UPDATE_TYPE
{
	SANDIAN_UPDATE_TYPE_NONE = 0,	//无效
	SANDIAN_UPDATE_TYPE_SHENGJI, 	//升级
	SANDIAN_UPDATE_TYPE_HUIGUN, 	//回滚
}SANDIAN_UPDATE_TYPE;

//三电升级方式
typedef enum tagSANDIAN_UPDATE_WAY
{
	SANDIAN_UPDATE_WAY_NONE = 0,	//无效
	SANDIAN_UPDATE_WAY_PUTONG, 		//普通升级
	SANDIAN_UPDATE_WAY_QIANGZHI, 	//强制升级
}SANDIAN_UPDATE_WAY;

//三电升级状态
typedef enum tagSANDIAN_UPDATE_STATUS
{
	SANDIAN_UPDATE_STATUS_NONE = 0,	//无效
	SANDIAN_UPDATE_STATUS_ING, 		//升级中
	SANDIAN_UPDATE_STATUS_SUCCESS,	//升级成功
	SANDIAN_UPDATE_STATUS_FALSE,	//升级失败
}SANDIAN_UPDATE_STATUS;

//零件代号
typedef enum tagSANDIAN_UPDATE_LINGJIAN_ID
{
	SANDIAN_UPDATE_LINGJIAN_ID_NONE = 0,	//无效
	SANDIAN_UPDATE_LINGJIAN_ID_VCU, 		//整车控制器
	SANDIAN_UPDATE_LINGJIAN_ID_MCU,			//电机控制器
	SANDIAN_UPDATE_LINGJIAN_ID_BMS, 		//BMS
	SANDIAN_UPDATE_LINGJIAN_ID_TERMINAL, 	//远程终端
	SANDIAN_UPDATE_LINGJIAN_ID_YIBIAO, 		//组合仪表

}SANDIAN_UPDATE_LINGJIAN_ID;


typedef enum tagCAN_BUS_OTA_STEP
{
	CAN_BUS_OTA_NONE = 0,

	CAN_BUS_OTA_STEP_1,
	CAN_BUS_OTA_STEP_1_WAIT_ACK,

	CAN_BUS_OTA_STEP_2,
	CAN_BUS_OTA_STEP_2_WAIT_ACK,

	CAN_BUS_OTA_STEP_3,
	CAN_BUS_OTA_STEP_3_WAIT_ACK,

	CAN_BUS_OTA_STEP_4,
	CAN_BUS_OTA_STEP_4_WAIT_ACK,

	CAN_BUS_OTA_STEP_5,
	CAN_BUS_OTA_STEP_5_WAIT_ACK,

	CAN_BUS_OTA_STEP_6,
	CAN_BUS_OTA_STEP_6_WAIT_ACK,

	CAN_BUS_OTA_STEP_7,
	CAN_BUS_OTA_STEP_7_WAIT_ACK,

	CAN_BUS_OTA_STEP_8,
	CAN_BUS_OTA_STEP_8_WAIT_ACK,

	CAN_BUS_OTA_STEP_9,
	CAN_BUS_OTA_STEP_9_WAIT_ACK,

	CAN_BUS_OTA_STEP_10,
	CAN_BUS_OTA_STEP_10_A,
	CAN_BUS_OTA_STEP_10_WAIT_ACK,

	CAN_BUS_OTA_STEP_11,
	CAN_BUS_OTA_STEP_11_WAIT_ACK,

	CAN_BUS_OTA_STEP_12,
	CAN_BUS_OTA_STEP_12_WAIT_ACK,

	CAN_BUS_OTA_STEP_13,
	CAN_BUS_OTA_STEP_13_WAIT_ACK,

	CAN_BUS_OTA_STEP_14,
	CAN_BUS_OTA_STEP_14_WAIT_ACK,

	CAN_BUS_OTA_STEP_15,
	CAN_BUS_OTA_STEP_15_WAIT_ACK,

	CAN_BUS_OTA_STEP_16,
	CAN_BUS_OTA_STEP_16_WAIT_ACK,

	CAN_BUS_OTA_STEP_17,
	CAN_BUS_OTA_STEP_17_WAIT_ACK,

	CAN_BUS_OTA_STEP_18,
	CAN_BUS_OTA_STEP_18_WAIT_ACK,
}CAN_BUS_OTA_STEP;

typedef struct tagFileBlockInfo
{
	unsigned int start_addr;
	unsigned int block_size;
}FileBlockInfo;

typedef struct tagCAN_BUS_OTA
{
	unsigned int phy_id;					//物理寻址标识符
	unsigned int response_id;				//响应标识符
	unsigned int (*p_func_calc_key)(unsigned int);

	unsigned char channel;					//CAN通道
	CAN_BUS_OTA_STEP can_ota_step;			//当前的升级步骤
	unsigned long seed;						//安全访问的种子

	unsigned char next_drv_bag_flag : 1;
	unsigned char next_app_bag_flag : 1;
	unsigned char drv_data_arrive_flag : 1;
	unsigned char app_data_arrive_flag : 1;

	unsigned int last_send_cmd_time;
	unsigned int cnt;

	unsigned int drv_check_sum;				//驱动文件checksum
	unsigned int app_check_sum;				//app文件checksum

	unsigned int drv_offset;
	unsigned int app_offset;
	unsigned int drv_tx_len;
	unsigned int app_tx_len;

	unsigned int last_bag_arrive_offset;		//上一包数据到达的偏移量
	unsigned int last_bag_arrive_len;			//上一包数据到达的长度

	unsigned int drv_cur_tx_len;
	unsigned int app_cur_tx_len;

	unsigned int drv_addr;					//驱动地址
	unsigned int drv_len;
	unsigned int drv_bag_id;

	unsigned int app_addr;					//app地址
	unsigned int app_len;
	unsigned int app_bag_id;

	unsigned int transfer_max_len;			//一次最大传输长度

	unsigned int drv_block_cnt;
	FileBlockInfo drv_blk[6];

	unsigned int app_block_cnt;
	FileBlockInfo app_blk[12];

	unsigned char cur_tx_app_block_idx;
	unsigned char cur_status;

	unsigned char buf[2048];
}CAN_BUS_OTA;

extern void sandian_start_ota_step1(CAN_BUS_OTA *p_can_bus_ota,unsigned int step,FileBlockInfo *drv_blk,unsigned int drv_blk_cnt,FileBlockInfo *app_blk,unsigned int app_blk_cnt);
extern void sandian_start_ota_step(CAN_BUS_OTA *p_can_bus_ota,unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len);
extern void sandian_data_arrive(CAN_BUS_OTA *p_can_bus_ota,unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen);
extern void sandian_restart_ota(CAN_BUS_OTA *p_can_bus_ota);

extern unsigned int can_bus_get_sandian_ota_progress(CAN_BUS_OTA *p_can_bus_ota);
extern void can_bus_ota_proc(CAN_BUS_OTA *p_can_bus_ota,char channel,CanRxMsg *pMsg);


extern unsigned int sandian_get_upgrade_progress(void);
extern void sandian_set_update_type(SANDIAN_UPDATE_TYPE typ);
extern void sandian_set_update_way(SANDIAN_UPDATE_WAY typ);
extern void sandian_set_update_status(SANDIAN_UPDATE_STATUS typ);
extern void sandian_set_update_lingjian_id(SANDIAN_UPDATE_LINGJIAN_ID typ);
extern SANDIAN_UPDATE_LINGJIAN_ID sandian_get_update_lingjian_id(void);
extern void sandian_set_update_request_flag(unsigned char val);
extern unsigned char sandian_get_update_request_flag(void);

extern SANDIAN_UPDATE_WAY sandian_get_update_way(void);



extern void send_info_to_yibiao(void);


extern void sandian_tx(unsigned char channel,unsigned int canid,unsigned char *pdat,unsigned char datlen);

extern void file_data_arrive(unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen);

extern void san_dian_ota_analyze(char channel,CanRxMsg *pMsg);


extern void sandian_flow_ctrl_ack(unsigned char can_channel,unsigned int canid,void *pdat,unsigned int datlen);

extern void sandian_proc(void);

#ifdef __cplusplus
}
#endif


#endif
