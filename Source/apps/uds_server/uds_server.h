#ifndef __UDS_SERVER_H__
#define __UDS_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif


#define UDS_SERVER_PHY_ID 				0x77C				//物理ID
#define UDS_SERVER_FUNC_ID 				0x7FF				//功能ID
#define UDS_SERVER_RESPONSE_ID 			0x67C				//响应ID




extern void uds_server_tx(unsigned char channel,unsigned int canid,unsigned char *pdat,unsigned char datlen);


extern void uds_server_proc(unsigned char channel,CanRxMsg *pMsg);




#ifdef __cplusplus
}
#endif

#endif


