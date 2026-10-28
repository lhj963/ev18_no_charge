#ifndef __PUB_H__
#define __PUB_H__
#ifdef __cplusplus
extern "C" {
#endif

extern void can_data_in_queue(unsigned char channel,unsigned char is_extid,unsigned int id,void *pdat,unsigned char datlen);
extern void can0_hook(CanRxMsg *prxmsg);
extern void can1_hook(CanRxMsg *prxmsg);
extern void can2_hook(CanRxMsg *prxmsg);
extern void auto_detect_can_bps_proc(void);
extern void can_send_proc(void);

extern void one_ms_hook(void);

extern void store_can_msg(void);




extern void usart_data_arrive(void *buf,unsigned int rlen);


extern void server_connect_ok(INT8U linkid);
extern void server_disconnect(INT8U linkid);


extern void server_data_arrive(unsigned char channel,void *pdat,unsigned int datlen);


extern void linux_cmd_ack_data_arrive(void *pdat,unsigned int datlen);
extern void sms_msg_arrive(unsigned char *ptime,void *phone_num,void *sms_content,unsigned int sms_content_len);

extern void cache_data_arrive(unsigned char cache_data_type_id,void *pdat,unsigned int rlen);

extern void app_main_power_off_callback(void);

extern void reset_main_off_flag(void);

extern void collect_data_proc(void);


extern void led_proc(void);

extern void disp_dbg_info(void);


extern void _send_to_can_bus(CanTxMsg *ptxmsg);

extern void collect_one_32960_pack(void);

extern void report_logout_to_server(unsigned int sleep_mode,unsigned int auto_wake_time_inv);

//地上铁专项处理
extern void dst_lock_car_proc(void);
extern void dst_proc(void);

//时代电服专项处理
extern void sddf_proc(void);

extern void mis_proc(void);

extern void user_mpu_msg_arrive(unsigned int msgid,void *pdat,unsigned int datlen);

extern void can_data_in_queue(unsigned char channel,unsigned char is_extid,unsigned int id,void *pdat,unsigned char datlen);

#ifdef __cplusplus
}
#endif

#endif


