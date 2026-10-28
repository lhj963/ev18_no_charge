#include "J1939.h"

char is_can_id_valid(INT32U id)
{
	if ((id == 0) || (id == 0xFFFFFFFF))
	{
		return 0;
	}
	return 1;
}

//J1939协议解析
void j1939_analysis(char channel,CanRxMsg *pMsg)
{
	unsigned long can_id = 0;
	unsigned char *pdat = 0;

	if (pMsg == 0)
	{
		return ;
	}

    //DBG_Print(2,"CanPrio: Milage(%d) Speed(%d) Kpa(%d).\r\n", ucMilagePgnPrio, ucSpeedPgnPrio, ucKpaPgnPrio);
	can_id = pMsg->IDE ? pMsg->ExtId : pMsg->StdId;
	pdat = pMsg->Data;

	//车型判断
	switch (can_id)
	{
	/***********************************ZD30********************************************/
	case 0x18FF0100:
	case 0x023F:
	set_ryc_engine_type(EngineType_ZD30);
	break;
	case 0x18FEC1EE:
	case 0x18FEFC17:
	case 0x0CFE6CEE:
	set_ryc_engine_type(EngineType_YC);
	break;
	default:break;
	}

	if (get_ryc_engine_type() == EngineType_ZD30)
	{
		switch (can_id)
		{
		/***********************************ZD30********************************************/
		case 0x02D3:
		can_set_speed(channel,byte_to_int16u(pdat[2],pdat[1]));
		break;
		case 0x0235:
		can_set_kpa(channel,pdat[2]);
		break;
		case 0x0266:
		can_set_actualtorquepercent(channel,(byte_to_int16u(pdat[3],pdat[2]) * 0.1) - 500 + 125);
		break;
		case 0x18FEDF00:
		can_set_frictiontorquepercent(channel,pdat[0]);
		break;
		case 0x023F:
		can_set_rpm(channel,byte_to_int16u(pdat[4],pdat[3]) * 4);
		can_set_coolanttemperature(channel,pdat[6]);
		break;	
		case 0x0232:
		can_set_fuelflow(channel,byte_to_int16u(pdat[1],pdat[0]) * 0.01 * 20);
		break;
		case 0x18F00E51:
		can_set_upperNOx(channel,byte_to_int16u(pdat[1],pdat[0]));
		break;
		case 0x18F00F52:
		can_set_lowerNOx(channel,byte_to_int16u(pdat[1],pdat[0]));
		break;
		case 0x0381:
		can_set_remain(channel,pdat[1]);
		break;
		case 0x02DC:
		can_set_intakevolume(channel,byte_to_int16u(pdat[1],pdat[0]));
		can_set_SCRInletTemperature(channel,byte_to_int16u(pdat[3],pdat[2]));
		can_set_SCROutletTemperature(channel,byte_to_int16u(pdat[5],pdat[4]));
		can_set_DPF(channel,byte_to_int16u(pdat[7],pdat[6]));
		break;
		case 0x03A3:
		can_set_oilboxlevel(channel,pdat[0]);
		break;
		case 0x03A1:
		can_set_milage(channel,byte_to_int32u(pdat[3],pdat[2],pdat[1],pdat[0]) * 0.005 * 10);
		break;				
		default:break;
		}
	}
	else
	{
		switch (can_id)
		{
		/***********************************M9T********************************************/
		
		/***********************************YUCHAI********************************************/
		case 0x18FEDF00:
		can_set_frictiontorquepercent(channel,pdat[0]);
		break;
		case 0x0CFE6CEE:
		can_set_speed(channel,byte_to_int16u(pdat[7],pdat[6]));
		break;
		case 0x18FEF500:
		can_set_kpa(channel,pdat[0]);
		break;
		case 0x0CF00400:
		can_set_actualtorquepercent(channel,pdat[2]);
		can_set_rpm(channel,byte_to_int16u(pdat[4],pdat[3]));
		break;
		case 0x18FEF200:
		can_set_fuelflow(channel,byte_to_int16u(pdat[1],pdat[0]));
		break;
		case 0x18F00E00:
		can_set_upperNOx(channel,byte_to_int16u(pdat[1],pdat[0]));
		break;
		case 0x18F00F00:
		can_set_lowerNOx(channel,byte_to_int16u(pdat[1],pdat[0]));
		break;
		case 0x18FE5600:
		can_set_remain(channel,pdat[0]);
		break;
		case 0x0CF00A00:
		can_set_intakevolume(channel,byte_to_int16u(pdat[3],pdat[2]));
		break;
		case 0x14FD3E00:
		can_set_SCRInletTemperature(channel,byte_to_int16u(pdat[1],pdat[0]));
		can_set_SCROutletTemperature(channel,byte_to_int16u(pdat[4],pdat[3]));
		break;
		case 0x18FDB200:
		can_set_DPF(channel,byte_to_int16u(pdat[5],pdat[4]));
		break;
		case 0x18FEEE00:
		can_set_coolanttemperature(channel,pdat[0]);
		break;
		case 0x18FEFC17:
		can_set_oilboxlevel(channel,pdat[1]);
		break;
		case 0x18FEC1EE:
		can_set_milage(channel,byte_to_int32u(pdat[7],pdat[6],pdat[5],pdat[4]) * 0.005 * 10);
		break;
		/***********************************QUANCHAI********************************************/
		
		/***********************************CHAOCHAI********************************************/
		
		/***********************************KANGMINGSI********************************************/
		
		default:break;
		}
	}
}

