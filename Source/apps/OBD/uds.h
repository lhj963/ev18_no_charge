#ifndef __UDS_H__
#define __UDS_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*SENDCANMSGPROC)(CanTxMsg *pMsg);

extern char is_calid_request_ok(void);

extern void uds_init(SENDCANMSGPROC SendCanMsgFunc);

extern void uds_can_send_proc(void);

extern void uds_analysis(char channel,CanRxMsg *pMsg);

extern void set_need_send_flow_ctrl_flag(unsigned char val);




#ifdef __cplusplus
}
#endif

#endif
