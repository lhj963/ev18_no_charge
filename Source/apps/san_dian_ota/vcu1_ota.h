#ifndef __VCU1_OTA_H__
#define __VCU1_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif



extern void vcu1_ota_proc(char channel,CanRxMsg *pMsg);


extern unsigned int get_sandian_vcu1_ota_progress(void);

extern unsigned int get_sandian_vcu1_ota_step(void);

extern void sandian_vcu1_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len);

extern void sandian_vcu1_restart_ota(void);


#ifdef __cplusplus
}
#endif

#endif
