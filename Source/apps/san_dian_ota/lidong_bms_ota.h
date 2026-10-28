#ifndef __LIDONG_BMS_OTA_H__
#define __LIDONG_BMS_OTA_H__


#ifdef __cplusplus
extern "C" {
#endif



extern void lidong_bms_ota_proc(char channel,CanRxMsg *pMsg);

extern void sandian_bms1_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len);
extern void sandian_bms1_start_ota_step1(unsigned int step,FileBlockInfo *drv_blk,unsigned int drv_blk_cnt,FileBlockInfo *app_blk,unsigned int app_blk_cnt);


extern void sandian_bms1_data_arrive(unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen);

extern void sandian_bms1_restart_ota(void);

extern unsigned int get_sandian_bms1_ota_progress(void);

extern unsigned int get_sandian_bms1_ota_step(void);



#ifdef __cplusplus
}
#endif
#endif


