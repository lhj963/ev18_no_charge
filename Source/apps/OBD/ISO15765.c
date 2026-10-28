#include "ISO15765.h"


//ISO15765协议解析
void ISO15765_Analysis(CanRxMsg *pMsg)
{
	static u8 temp = 0;
	static u8 VIN_DTC_Flag = 0;
	u32 can_id = 0;
	u8 *data = 0;

	if (pMsg == 0)
	{
		return ;
	}

	can_id = pMsg->IDE ? pMsg->ExtId : pMsg->StdId;
	if(((can_id>>4)==0x7E)||((can_id>>8)==0x18DAF1))
	{

	}
	else
	{
		return ;
	}

	data = pMsg->Data;

	switch((data[0] >> 4) & 0x0F)
	{
		case 0x00:	//收到数据小于7个字节					
			switch(data[1])
			{
				case 0x01: 
					break;
				case 0x41: //数据解析
					if(data[2]==0x01)//故障码数量
					{
						OBD_RT.DTC_CNT 	= (data[3] & 0x7f);
						OBD_RT.MIL_STA 	= (data[3] & 0x80)?1:0;	//发动机故障灯
						OBD_RT.DST_STA  = ((data[4]>>6)&0x01);//支持列表
						OBD_RT.DST_STA |= (u32)((data[4]>>7)&0x01)<<1;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>0)&0x01)<<12;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>1)&0x01)<<5;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>3)&0x01)<<9;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>5)&0x01)<<11;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>6)&0x01)<<10;//支持列表
						OBD_RT.DST_STA |= (u32)((data[5]>>7)&0x01)<<7;//支持列表
					}
					break;
				case 0x43: //故障码
					if(OBD_RT.DTC_CNT==0)
					OBD_RT.DTC_CNT = data[2];//故障码个数
					OBD_RT.DTC_BUF[0] = OBD_Hex_To_DTC(data[3],data[4]);
					OBD_RT.DTC_BUF[1] = OBD_Hex_To_DTC(data[5],data[6]);
					break;
				default:break;
			}
			break;
		case 0x01:	//收到连续帧
			//VIN
			if((data[2]==0x49)&&(data[3]==0x02))
			{
				VIN_DTC_Flag = 1;
				OBD_RT.VIN[0] = data[5];
				OBD_RT.VIN[1] = data[6];
				OBD_RT.VIN[2] = data[7];
			}
			else if(data[2]==0x43)//读取故障码
			{
				VIN_DTC_Flag = 2;
				if(OBD_RT.DTC_CNT==0)
				OBD_RT.DTC_CNT = data[3];//故障码个数
				OBD_RT.DTC_BUF[0] = OBD_Hex_To_DTC(data[4],data[5]);
				OBD_RT.DTC_BUF[1] = OBD_Hex_To_DTC(data[6],data[7]);
			}
			//Can_Send_Data(&CAR_CAN_Bro[CAN_FCD+(CAR_CAN_PID_Flag-1)*8]);//请求0x30
			break;
		case 0x02:	//多帧数据返回
			switch(data[0]&0x0F)
			{
				case 0x01:
					if(VIN_DTC_Flag==1) //VIN
					{
						OBD_RT.VIN[3] = data[1];
						OBD_RT.VIN[4] = data[2];
						OBD_RT.VIN[5] = data[3];
						OBD_RT.VIN[6] = data[4];
						OBD_RT.VIN[7] = data[5];
						OBD_RT.VIN[8] = data[6];
						OBD_RT.VIN[9] = data[7];
					}
					else if(VIN_DTC_Flag==2) //DTC
					{
						OBD_RT.DTC_BUF[2] = OBD_Hex_To_DTC(data[1],data[2]);
						OBD_RT.DTC_BUF[3] = OBD_Hex_To_DTC(data[3],data[4]);
						OBD_RT.DTC_BUF[4] = OBD_Hex_To_DTC(data[5],data[6]);
						temp = data[7];
					}
					break;
				case 0x02:
					if(VIN_DTC_Flag==1) //VIN
					{					
						OBD_RT.VIN[10] = data[1];
						OBD_RT.VIN[11] = data[2];
						OBD_RT.VIN[12] = data[3];
						OBD_RT.VIN[13] = data[4];
						OBD_RT.VIN[14] = data[5];
						OBD_RT.VIN[15] = data[6];
						OBD_RT.VIN[16] = data[7];
						OBD_RT.VIN[17] = 0;
					}
					else if(VIN_DTC_Flag == 2)	//DTC
					{
						OBD_RT.DTC_BUF[5] = OBD_Hex_To_DTC(temp,data[1]);
						OBD_RT.DTC_BUF[6] = OBD_Hex_To_DTC(data[2],data[3]);
						OBD_RT.DTC_BUF[7] = OBD_Hex_To_DTC(data[4],data[5]);
						OBD_RT.DTC_BUF[8] = OBD_Hex_To_DTC(data[6],data[7]);
					}
					break;
				case 0x03:
					if(VIN_DTC_Flag==2) //DTC
					{
						OBD_RT.DTC_BUF[9] = OBD_Hex_To_DTC(data[1],data[2]);
						OBD_RT.DTC_BUF[10] = OBD_Hex_To_DTC(data[3],data[4]);
						OBD_RT.DTC_BUF[11] = OBD_Hex_To_DTC(data[5],data[6]);
						temp = data[7];
					}
					break;
			}
			break;
		case 0x03:
			
			break;
		default:break;
	}
}



