#ifndef __HUICHUAN_MCU_OTA_H__
#define __HUICHUAN_MCU_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif



extern void huichuan_mcu_ota_proc(char channel,CanRxMsg *pMsg);


extern unsigned int get_sandian_mcu_ota_progress(void);

extern unsigned int get_sandian_mcu_ota_step(void);

extern void sandian_mcu_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len);

extern void sandian_mcu_restart_ota(void);

extern void can_transfor_test(void);

#ifdef __cplusplus
}
#endif

#endif
