#ifndef __DST_H__
#define __DST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum tagDST_TBOX_STATUS
{
	DST_TBOX_STATUS_WORK = 1,					//工作 
	DST_TBOX_STATUS_SLEEP = 2,					//休眠 
	DST_TBOX_STATUS_LOW_VOL = 3,				//低电保护
	DST_TBOX_STATUS_MAIN_POWER_OFF = 4,			//主电源断开


	DST_TBOX_STATUS_MAX
}DST_TBOX_STATUS;


extern void set_dst_tbox_work_status(DST_TBOX_STATUS val);
extern DST_TBOX_STATUS get_dst_tbox_work_status(void);
extern char *get_dst_tbox_work_status_tip(void);


extern unsigned int dst_cddl_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U start_time[6],INT8U end_time[6]);

extern unsigned int dst_vin_data_pack(void *pOut,unsigned int outbuflen,void *pVIN,INT8U start_time[6]);

extern unsigned int dst_zdy_data_pack(void *pOut,unsigned int outbuflen,
												void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int dst_RealDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);

extern unsigned int dst_BFDataPack(void *pOut,unsigned int outbuflen,void *pVIN,
												INT8U time[6],
												INT8U pos_status,INT32U lon,INT32U lat);


extern void dst_ack_plat(INT8U linkid,void *pdat,unsigned int len);

extern void clr_charge_dl(void);
extern void dst_stat_charge_dl(void);
extern unsigned int _get_dst_charge_dl(void);

extern void test_calc_charge_dl(void);

#ifdef __cplusplus
}
#endif

#endif


