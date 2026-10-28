#ifndef __USERCMD_H__
#define __USERCMD_H__

#ifdef __cplusplus
extern "C" {
#endif


extern INT8U get_vin(void *pOut,int outbuflen);

extern unsigned int analyze_usercmd(void *ptr,unsigned int len,void *pOut,unsigned int outbuflen);


extern INT32U _get_sw_status(void);



extern void ReportStatus(void);

extern void report_can_data(void);

extern void device_heart_beat(void);

extern void device_heart_beat1(void);

extern void test_device_heart_beat(void);

extern void send_status_to_canbus(void);

#ifdef __cplusplus
}
#endif

#endif


