#ifndef __NINGDE_BMS_OTA_H__
#define __NINGDE_BMS_OTA_H__
#ifdef __cplusplus
extern "C" {
#endif


extern unsigned int get_sandian_bms_ota_step(void);

extern void sandian_bms_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len);

extern void ningde_bms_ota_proc(char channel,CanRxMsg *pMsg);



#ifdef __cplusplus
}
#endif


#endif
