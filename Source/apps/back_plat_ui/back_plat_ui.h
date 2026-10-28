#ifndef __BACK_PLAT_UI_H__
#define __BACK_PLAT_UI_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagRecvCanMsg
{
	INT32U decs;		//描述符  
						//0-5bit:秒
						//6-11bit:分
						//12-16bit:时
						//17-21bit:日
						//22-25bit:月
						//26-29bit:年 (2020年为基数)  0表示2020年 1表示2021年 2表示2022年
						//30-31:CAN通道
	CanRxMsg msg;
}RecvCanMsg;


extern unsigned int get_heart_beat_time_interval(void);
extern void set_heart_beat_time_interval(unsigned int val);
extern void set_report_can_raw_sw(unsigned char val);
extern void report_to_server_temp(unsigned char can_channel,CanRxMsg *prxmsg);





extern void server4_connect_ok(void);
extern void link4_data_arrive(void *pdat,unsigned int rlen);
extern void server4_connected(void);

extern void remote_log(void *pdat,unsigned int datlen);

#if 1
extern void server_test_connect_ok(void);
extern void link_test_data_arrive(void *pdat,unsigned int rlen);
extern void server_test_connected(void);
#endif

#ifdef __cplusplus
}
#endif

#endif


