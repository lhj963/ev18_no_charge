/****************************************************************************
**  名称: car_uds.h文件
*   日期: 2019/03/20
*   作者: Alex Chen
*   版本: V0.0.0
*   描述: 
*		1. ------
*		2. ------
*   修改记录: 
*	   日期		     修改人		         版本 		           修改内容
*	2019/03/20 		Alex Chen         V0.0.0          软件初始化版本，实现了xxx接口，

******************************************************************************/
#ifndef __CAR_UDS_H__
#define __CAR_UDS_H__ 

//#include "stm32f10x.h"
//#include "obd_can.h"

/*结构体定义*/
typedef struct _CanProInfo
{
	CAN_TypeDef* CANx;  //can的选择，CAN1或CAN2
    u8 	canide;         //标识符类型的设置，扩展标识符或标准标识符  
	u32 canid;          //标识符值的设定
	u8 	msg[8];         //发送的数据
	u8 	len;            //发送数据的长度
}CanProInfo;


void CarIS014229UdsAnalysis(CAN_TypeDef* CANx,u32 can_id,u8 data[8]);

#endif

