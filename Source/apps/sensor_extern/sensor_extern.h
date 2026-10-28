#ifndef __SENSOR_EXTERN_H__
#define __SENSOR_EXTERN_H__

#ifdef __cplusplus
extern "C" {
#endif



extern INT16U get_guangxishouxishu(void);
extern INT16U get_butouguangdu(void);
extern INT16U get_keliwunongdu(void);



extern void TAEPY_100_Deal(CanRxMsg *prxmsg);





#ifdef __cplusplus
}
#endif


#endif

