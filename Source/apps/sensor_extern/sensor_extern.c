#include "sensor_extern.h"



#define TAEPY_100_UP_ID   0x18F10155



typedef struct tagTAEPY_100Data
{
	INT16U guangxishouxishu;	//光吸收吸收
	INT16U butouguangdu;		//不透光度
	INT16U keliwunongdu;		//颗粒物浓度
}TAEPY_100Data;

TAEPY_100Data stTAEPY_100Data = {0};


INT16U get_guangxishouxishu(void)
{
	return stTAEPY_100Data.guangxishouxishu;
}

INT16U get_butouguangdu(void)
{
	return stTAEPY_100Data.butouguangdu;
}

INT16U get_keliwunongdu(void)
{
	return stTAEPY_100Data.keliwunongdu;
}



void TAEPY_100_Deal(CanRxMsg *prxmsg)
{
	INT32U can_id = prxmsg->IDE ? prxmsg->ExtId : prxmsg->StdId;
	unsigned char *pdat = prxmsg->Data;

	if (can_id == TAEPY_100_UP_ID)
	{
		DBG_PrintBuf(LEVEL4,"烟度传感器:",pdat,8);

		//传感器出来的数据需要*0.001 但是上传平台需要*10 所以*0.01
		stTAEPY_100Data.keliwunongdu = ((pdat[2] << 8) + pdat[1]) * 0.01;

		//传感器出来的数据需要*0.001 但是上传平台需要*100 所以*0.1
		stTAEPY_100Data.guangxishouxishu = ((pdat[4] << 8) + pdat[3]) * 0.1;

		//传感器出来的数据需要*0.1  但是上传平台需要*10  所以抵消
		stTAEPY_100Data.butouguangdu = ((pdat[6] << 8) + pdat[5]);
	}
}





