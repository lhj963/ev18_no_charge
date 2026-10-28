#ifndef __ADHD_H__
#define __ADHD_H__

#ifdef __cplusplus
extern "C" {
#endif


extern void adhd_analyze(unsigned char linkid,void *pdat,unsigned int datlen);



extern unsigned int adhd_request_radom_pack(void *pOut,unsigned int outbuflen,void *pvin);
extern unsigned int adhd_send_radom_pack(void *pOut,unsigned int outbuflen,void *pvin,void *pRecvRadom,unsigned int recvlen);
extern unsigned int adhd_car_status_pack(void *pOut,unsigned int outbuflen,void *pvin);
extern unsigned int adhd_connector_status_pack(void *pOut,unsigned int outbuflen,void *pvin);
extern unsigned int adhd_bms_pack(void *pOut,unsigned int outbuflen,void *pvin);
extern unsigned int adhd_responese_hdstatus_pack(void *pOut,unsigned int outbuflen,void *pvin);


extern void adhd_proc(void);




#ifdef __cplusplus
}
#endif

#endif

