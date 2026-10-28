#include "CanProtocol.h"

#define EMS4_CAN_ID 0x0545


//自定义协议解析
void can_protocol_analysis(char channel,CanRxMsg *pMsg)
{
	INT32U can_id = 0;
	INT8U *pdat = 0;

	if (pMsg == 0) return ;

	can_id = pMsg->IDE ? pMsg->ExtId : pMsg->StdId;
	pdat = pMsg->Data;

	switch(can_id)
	{
	case EMS4_CAN_ID:
	pdat[0] = 0;
	break;

	default:break;
	}

}

















