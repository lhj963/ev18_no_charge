#include "car_uds.h"


typedef struct _UdsDtcDataInfo
{
    u8 AntennaDtcStatus; //天线开路DTC状态
    u8 SimCardDtcStatus; //SIM卡未插DTC状态
    u8 CanbusDtcStatus;  //busoff故障DTC状态
	u32 AntennaDTC ;  //天线开路DTC故障码
    u32 SimCardDTC;	 //SIM卡未插DTC故障码
	u32 CanbusDTC;  //busoff故障DTC
}UdsDtcDataInfo;

u8 DTCStatusAvailabilityMask =0x49 ; //ECU支持的DTC状态
u8 AntennaDTCStatus =0x00;
u8 SimCardDTCStatus =0x00;
u8 CanbusDTCStatus  =0x00;


UdsDtcDataInfo s_UdsDtcInfo;


/**************************************************************************************************
* 函  数: u8 CanSendMsg(CAN_TypeDef* CANx,u8 CAN_IDE,u32 CAN_Id,u8* msg,u8 len)
* 描  述: CAN发送一组数据
* 举  例: 
* 入  参: uCAN_TypeDef* CANx:选择CAN为哪路，CAN1或CAN2
*         u8 CAN_IDE:标识符类型选择，标准标识符（CAN_Id_Standard）或扩展标识符（CAN_Id_Extended)
*         u32 CAN_Id:设置标识符Id值
*         u8* msg   :数据指针,最大为8个字节
*         u8 len    :数据长度(最大为8)
* 出  参: u8
* 返回值: 0,成功,其他,失败;
****************************************************************************************************/
u8 CanSendMsg(CAN_TypeDef* CANx,u8 CAN_IDE,u32 CAN_Id,u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=CAN_Id;			// 设置标准标识符Id值
	TxMessage.ExtId=CAN_Id;			// 设置扩展标示符（29位）
	TxMessage.IDE=CAN_IDE;          // 标识符类型选择，标准标识符（CAN_Id_Standard）或扩展标识符（CAN_Id_Extended)
	TxMessage.RTR=0;		 		// 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;				// 发送帧信息的数量
	DBG_Print(LEVEL4,"Send CanID: %08X\r\n",CAN_Id);
	for(i=0;i<len;i++)
	{
	 TxMessage.Data[i]=msg[i];		// 第一帧信息 
	}		
	mbox= CAN_Transmit(CANx, &TxMessage);  	
	i=0;
	while((CAN_TransmitStatus(CANx, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;		
}


/**************************************************************************************************
* 函  数: void CarIS014229UdsAnalysis(CAN_TypeDef* CANx,u32 can_id,u8 data[8])
* 描  述: 基于ISO14229协议的UDS诊断数据解析
* 举  例: 
* 入  参: CAN_TypeDef* CANx:选择CAN的编号
*         u32 can_id:收到的CAN ID号
*         u8 data[8]:收到的数据
* 出  参: u8
* 返回值: 0,成功,其他,失败;
****************************************************************************************************/
void CarIS014229UdsAnalysis(CAN_TypeDef* CANx,u32 can_id,u8 data[8])
{
    CanProInfo  CAR_UDSBro;
	static u8 ExtendDSFlag =0;
	static u8 Write2EEnableFlag =0;
	static u8 UDS_FCDFlag =0;
	static u8 UdsDtcFCDFlag =0;  //DTC流控制标志
	u8 UdsDtcCount =0;
	u8 DTCStatusMaskFlag =0; //通过状态掩码得到DTC的个数标志
	unsigned int PGN = 0;
	s_UdsDtcInfo.AntennaDTC =0xA39029;
	s_UdsDtcInfo.SimCardDTC =0xA3C000;
	s_UdsDtcInfo.CanbusDTC = 0xA39129;
	PGN =  can_id & 0xFFFF;
    if ((PGN != 0x0793)&&(PGN!= 0x07DF))
	{
		return ;
	}
	DBG_PrintBuf(LEVEL4,"CarIS014229UdsAnalysis:",data,8);

	if(data[0]&0x0F) //单帧请求
	{
		switch(data[1])
		{
			case 0x10://诊断模式控制 
				if(data[2]==0x01)//默认模式
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x50;
					CAR_UDSBro.msg[2] = 0x01;
					CAR_UDSBro.len = 3;
					ExtendDSFlag =0;
				}
				else if(data[2]==0x03)//扩展诊断模式
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x50;
					CAR_UDSBro.msg[2] = 0x03;
					CAR_UDSBro.len = 3;
					ExtendDSFlag =1;
				}
				else   //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x10;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
				}
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);			
			break;
			case 0x27://安全访问
				if((data[2]==0x01)&&(ExtendDSFlag == 1))
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x67;
					CAR_UDSBro.msg[2] = 0x01;
					CAR_UDSBro.msg[3] = 0x07;
					CAR_UDSBro.msg[4] = 0x10;
					CAR_UDSBro.msg[5] = 0x56;
					CAR_UDSBro.msg[6] = 0x01;
					CAR_UDSBro.msg[7] = 0xAA;
					CAR_UDSBro.len = 8;
				}
				else if((data[2]==0x02)&&(ExtendDSFlag == 1))
				{
					if((data[3]==0x02)&&(data[4]==0x02)&&(data[5]==0x03)&&(data[6]==0x03)) //密码配对
					{
						CAR_UDSBro.CANx =CANx;
						CAR_UDSBro.canide =CAN_Id_Standard;
						CAR_UDSBro.canid =0x79B;
						CAR_UDSBro.msg[0] = 0x02;	
						CAR_UDSBro.msg[1] = 0x67;
						CAR_UDSBro.msg[2] = 0x02;
						CAR_UDSBro.len = 3;
						Write2EEnableFlag = 1;  //允许写入数据标志
					}
					else
					{
					    CAR_UDSBro.CANx =CANx;
					    CAR_UDSBro.canide =CAN_Id_Standard;
					    CAR_UDSBro.canid =0x79B;
					    CAR_UDSBro.msg[0] = 0x7F;	
					    CAR_UDSBro.msg[1] = 0x27;
					    CAR_UDSBro.msg[2] = 0x35;
					    CAR_UDSBro.len = 3;
					
					}
				
				}
				else      //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x27;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
				}
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
			break;
			case 0x3E://诊断设备在线
				if(data[2]==0x00)
				{ 
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x7E;
					CAR_UDSBro.msg[2] = 0x00;
					CAR_UDSBro.len = 3;
				}
				else      //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x3E;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
				}
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
			break;
			case 0x28://通信控制
				if(data[2]==0x00)//使能收发
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x68;
					CAR_UDSBro.msg[2] = 0x00;
					CAR_UDSBro.len = 3;
				}
				else if(data[2]==0x03)//禁止收发
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0x68;
					CAR_UDSBro.msg[2] = 0x03;
					CAR_UDSBro.len = 3;
				
				}
				else      //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x28;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
				}
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
			break;
			case 0x85://控制DTC设置
				if(data[2]==0x01)  //打开DTC设置
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0xC5;
					CAR_UDSBro.msg[2] = 0x01;
					CAR_UDSBro.len = 3;
				}
				else if(data[2]==0x02)  //关闭DTC设置
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x02;	
					CAR_UDSBro.msg[1] = 0xC5;
					CAR_UDSBro.msg[2] = 0x02;
					CAR_UDSBro.len = 3;
				
				}
				else      //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x85;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
				}
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
			break;
			case 0x22://读取数据
				if((data[2]==0xF1)&&(data[3]==0x84))  //application Software Fingerprint Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x0B;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x84;
					CAR_UDSBro.msg[5] = 0x01;
					CAR_UDSBro.msg[6] = 0x02;
					CAR_UDSBro.msg[7] = 0x06;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 1;
				}
				else if((data[2]==0xF1)&&(data[3]==0x87))  //vehicle Manufacturer Spare Part Number Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x0E;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x87;
					CAR_UDSBro.msg[5] = 0x11;
					CAR_UDSBro.msg[6] = 0x12;
					CAR_UDSBro.msg[7] = 0x16;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 2;
				
				}
				else if((data[2]==0xF1)&&(data[3]==0x8A))  //System Supplier Identifier Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x0B;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x8A;
					CAR_UDSBro.msg[5] = 0x01;
					CAR_UDSBro.msg[6] = 0x02;
					CAR_UDSBro.msg[7] = 0x06;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 3;
				
				}
				else if((data[2]==0xF1)&&(data[3]==0x8C)) //ECU Serial Number Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x08;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x8C;
					CAR_UDSBro.msg[5] = 0x01;
					CAR_UDSBro.msg[6] = 0x02;
					CAR_UDSBro.msg[7] = 0x06;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 4;
				}
				else if((data[2]==0xF1)&&(data[3]==0x90)) //VIN Data Identifier
				{
				   CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x13;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x90;
					CAR_UDSBro.msg[5] = 0x01;
					CAR_UDSBro.msg[6] = 0x02;
					CAR_UDSBro.msg[7] = 0x06;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 5;
				}
				else if((data[2]==0xF1)&&(data[3]==0x92)) //system Supplier ECU Hardware Number Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x05;	
					CAR_UDSBro.msg[1] = 0x62;
					CAR_UDSBro.msg[2] = 0xF1;
					CAR_UDSBro.msg[3] = 0x92;
					CAR_UDSBro.msg[4] = 0x01;
					CAR_UDSBro.msg[5] = 0x02;
					CAR_UDSBro.msg[6] = 0xAA;
					CAR_UDSBro.msg[7] = 0xAA;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					
				}
				else if((data[2]==0xF1)&&(data[3]==0x93)) //ECU Hardware Version
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x10;	
					CAR_UDSBro.msg[1] = 0x08;
					CAR_UDSBro.msg[2] = 0x62;
					CAR_UDSBro.msg[3] = 0xF1;
					CAR_UDSBro.msg[4] = 0x93;
					CAR_UDSBro.msg[5] = 0x01;
					CAR_UDSBro.msg[6] = 0x02;
					CAR_UDSBro.msg[7] = 0x06;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
					UDS_FCDFlag = 6;
				}
				else if((data[2]==0xF1)&&(data[3]==0x94))  //system Supplier ECU Software Number Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x05;	
					CAR_UDSBro.msg[1] = 0x62;
					CAR_UDSBro.msg[2] = 0xF1;
					CAR_UDSBro.msg[3] = 0x94;
					CAR_UDSBro.msg[4] = 0x01;
					CAR_UDSBro.msg[5] = 0x02;
					CAR_UDSBro.msg[6] = 0xAA;
					CAR_UDSBro.msg[7] = 0xAA;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);                        
				
				}
				else if((data[2]==0xF1)&&(data[3]==0x95)) //system Supplier ECU Software Version Number Data Identifier
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x05;	
					CAR_UDSBro.msg[1] = 0x62;
					CAR_UDSBro.msg[2] = 0xF1;
					CAR_UDSBro.msg[3] = 0x95;
					CAR_UDSBro.msg[4] = 0x01;
					CAR_UDSBro.msg[5] = 0x02;
					CAR_UDSBro.msg[6] = 0xAA;
					CAR_UDSBro.msg[7] = 0xAA;
					CAR_UDSBro.len = 8;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len);
				}
				else      //否定的响应
				{
					CAR_UDSBro.CANx =CANx;
					CAR_UDSBro.canide =CAN_Id_Standard;
					CAR_UDSBro.canid =0x79B;
					CAR_UDSBro.msg[0] = 0x7F;	
					CAR_UDSBro.msg[1] = 0x22;
					CAR_UDSBro.msg[2] = 0x12;
					CAR_UDSBro.len = 3;
					CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len); 
				}
			break;
//			case 0x2E://写入数据
//				 
//			break;
			case 0x19://读DTC信息
				if(data[2] ==0x01) //通过状态掩码报告DTC的数量
				{
				     if(s_UdsDtcInfo.AntennaDtcStatus &(data[2]&DTCStatusAvailabilityMask))
					 {
					     UdsDtcCount ++;
					 }
					 else if(s_UdsDtcInfo.SimCardDtcStatus &(data[2]&DTCStatusAvailabilityMask))
					 {
					 
					     UdsDtcCount ++;
					 }
					 else if(s_UdsDtcInfo.CanbusDtcStatus &(data[2]&DTCStatusAvailabilityMask))
					 {
					     UdsDtcCount ++;
					 }
					 CAR_UDSBro.CANx =CANx;
					 CAR_UDSBro.canide =CAN_Id_Standard;
					 CAR_UDSBro.canid =0x79B;
					 CAR_UDSBro.msg[0] = 0x05;	
					 CAR_UDSBro.msg[1] = 0x59;
					 CAR_UDSBro.msg[2] = 0x01;
					 CAR_UDSBro.msg[3] = DTCStatusAvailabilityMask;
					 CAR_UDSBro.msg[4] = 0x00;
					 CAR_UDSBro.msg[5] = UdsDtcCount;
					 CAR_UDSBro.len = 6;
					 CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len); 
				
				}
				else if(data[2] ==0x02)  //通过状态掩码报告DTC
				{
				     if(s_UdsDtcInfo.AntennaDtcStatus &(data[2] & DTCStatusAvailabilityMask))
					 {
						 DTCStatusMaskFlag =1;  //一个故障码：天线DTC
						 if(s_UdsDtcInfo.SimCardDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
						 {
						     DTCStatusMaskFlag = 2;  //两个故障码：天线DTC和SIM卡DTC
							 if(s_UdsDtcInfo.CanbusDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
							 {
							     DTCStatusMaskFlag = 7; //三个故障码：天线DTC，SIM卡DTC和canbusDTC
							 }
						 }
						 else if(s_UdsDtcInfo.CanbusDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
						 {
						     DTCStatusMaskFlag = 3;  //两个故障码：天线DTC和canbusDTC
						 }
					     
					 }
					 else if(s_UdsDtcInfo.SimCardDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
					 {
					     DTCStatusMaskFlag =4; //一个故障码：SIM卡DTC
						 if(s_UdsDtcInfo.CanbusDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
						 {
						     DTCStatusMaskFlag = 5;  //两个故障码：SIM卡DTC和canbusDTC
						 }
					 }
					 else if(s_UdsDtcInfo.CanbusDtcStatus &&(data[2] && DTCStatusAvailabilityMask))
					 {
					     DTCStatusMaskFlag = 6;  //一个故障码：canbusDTC
					 }
					 
					 switch(DTCStatusMaskFlag)
					 {
					     case 1:   //一个故障码：天线DTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x07;	
						     CAR_UDSBro.msg[1] = 0x59;
						     CAR_UDSBro.msg[2] = 0x02;
						     CAR_UDSBro.msg[3] = DTCStatusAvailabilityMask;  
						     CAR_UDSBro.msg[4] = (u8)(s_UdsDtcInfo.AntennaDTC&0xFF);
						     CAR_UDSBro.msg[5] =(u8)((s_UdsDtcInfo.AntennaDTC >>8)&0xFF);
						     CAR_UDSBro.msg[6] = (u8)((s_UdsDtcInfo.AntennaDTC >>16)&0xFF);
						     CAR_UDSBro.msg[7] = s_UdsDtcInfo.AntennaDtcStatus;
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
							 break;
					     case 2:   //两个故障码：天线DTC和SIM卡DTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x10;	
						     CAR_UDSBro.msg[1] = 0x08;
						     CAR_UDSBro.msg[2] = 0x59;
					         CAR_UDSBro.msg[3] = 0x0A;
						     CAR_UDSBro.msg[4] = DTCStatusAvailabilityMask; 
						     CAR_UDSBro.msg[5] = (u8)(s_UdsDtcInfo.AntennaDTC&0xFF);
						     CAR_UDSBro.msg[6] =(u8)((s_UdsDtcInfo.AntennaDTC >>8)&0xFF);
						     CAR_UDSBro.msg[7] = (u8)((s_UdsDtcInfo.AntennaDTC >>16)&0xFF);
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
					         UdsDtcFCDFlag =2;
							 break;
						 case 3:   //两个故障码：天线DTC和canbusDTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x10;	
						     CAR_UDSBro.msg[1] = 0x08;
						     CAR_UDSBro.msg[2] = 0x59;
					         CAR_UDSBro.msg[3] = 0x0A;
						     CAR_UDSBro.msg[4] = DTCStatusAvailabilityMask; 
						     CAR_UDSBro.msg[5] = (u8)(s_UdsDtcInfo.AntennaDTC&0xFF);
						     CAR_UDSBro.msg[6] =(u8)((s_UdsDtcInfo.AntennaDTC >>8)&0xFF);
						     CAR_UDSBro.msg[7] = (u8)((s_UdsDtcInfo.AntennaDTC >>16)&0xFF);
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
					         UdsDtcFCDFlag =3;
							 break;
						 case 4:   //一个故障码：SIM卡DTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x07;	
						     CAR_UDSBro.msg[1] = 0x59;
						     CAR_UDSBro.msg[2] = 0x02;
						     CAR_UDSBro.msg[3] = DTCStatusAvailabilityMask;  
						     CAR_UDSBro.msg[4] = (u8)(s_UdsDtcInfo.SimCardDTC&0xFF);
						     CAR_UDSBro.msg[5] =(u8)((s_UdsDtcInfo.SimCardDTC >>8)&0xFF);
						     CAR_UDSBro.msg[6] = (u8)((s_UdsDtcInfo.SimCardDTC >>16)&0xFF);
						     CAR_UDSBro.msg[7] = s_UdsDtcInfo.SimCardDtcStatus;
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
							 break;
						 case 5:  //两个故障码：SIM卡DTC和canbusDTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x10;	
						     CAR_UDSBro.msg[1] = 0x08;
						     CAR_UDSBro.msg[2] = 0x59;
					         CAR_UDSBro.msg[3] = 0x0A;
						     CAR_UDSBro.msg[4] = DTCStatusAvailabilityMask; 
						     CAR_UDSBro.msg[5] = (u8)(s_UdsDtcInfo.SimCardDTC&0xFF);
						     CAR_UDSBro.msg[6] =(u8)((s_UdsDtcInfo.SimCardDTC >>8)&0xFF);
						     CAR_UDSBro.msg[7] = (u8)((s_UdsDtcInfo.SimCardDTC >>16)&0xFF);
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
					         UdsDtcFCDFlag =4;
							 break;
						 case 6:   //一个故障码：canbusDTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x07;	
						     CAR_UDSBro.msg[1] = 0x59;
						     CAR_UDSBro.msg[2] = 0x02;
						     CAR_UDSBro.msg[3] = DTCStatusAvailabilityMask;  
						     CAR_UDSBro.msg[4] = (u8)(s_UdsDtcInfo.CanbusDTC&0xFF);
						     CAR_UDSBro.msg[5] =(u8)((s_UdsDtcInfo.CanbusDTC >>8)&0xFF);
						     CAR_UDSBro.msg[6] = (u8)((s_UdsDtcInfo.CanbusDTC >>16)&0xFF);
						     CAR_UDSBro.msg[7] = s_UdsDtcInfo.CanbusDtcStatus;
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
						 case 7:  //三个故障码：天线DTC，SIM卡DTC和canbusDTC
							 CAR_UDSBro.CANx =CANx;
						     CAR_UDSBro.canide =CAN_Id_Standard;
						     CAR_UDSBro.canid =0x79B;
						     CAR_UDSBro.msg[0] = 0x10;	
						     CAR_UDSBro.msg[1] = 0x0E;
						     CAR_UDSBro.msg[2] = 0x59;
					         CAR_UDSBro.msg[3] = 0x0A;
						     CAR_UDSBro.msg[4] = DTCStatusAvailabilityMask; 
						     CAR_UDSBro.msg[5] = (u8)(s_UdsDtcInfo.AntennaDTC&0xFF);
						     CAR_UDSBro.msg[6] =(u8)((s_UdsDtcInfo.AntennaDTC >>8)&0xFF);
						     CAR_UDSBro.msg[7] = (u8)((s_UdsDtcInfo.AntennaDTC >>16)&0xFF);
						     CAR_UDSBro.len = 8;
						     CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
					         UdsDtcFCDFlag =1;
							 break;
						 default:
							 break;
					 
					 
					 
					 
					 }
				
				}
				else if(data[2] ==0x0A) //报告支持的DTC
				{
				    CAR_UDSBro.CANx =CANx;
				    CAR_UDSBro.canide =CAN_Id_Standard;
				    CAR_UDSBro.canid =0x79B;
				    CAR_UDSBro.msg[0] = 0x10;	
				    CAR_UDSBro.msg[1] = 0x0E;
				    CAR_UDSBro.msg[2] = 0x59;
				    CAR_UDSBro.msg[3] = 0x0A;
				    CAR_UDSBro.msg[4] = DTCStatusAvailabilityMask; 
				    CAR_UDSBro.msg[5] = (u8)(s_UdsDtcInfo.AntennaDTC&0xFF);
				    CAR_UDSBro.msg[6] =(u8)((s_UdsDtcInfo.AntennaDTC >>8)&0xFF);
				    CAR_UDSBro.msg[7] = (u8)((s_UdsDtcInfo.AntennaDTC >>16)&0xFF);
				    CAR_UDSBro.len = 8;
				    CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				    UdsDtcFCDFlag =1;
				}
			break;
			case 0x34://请求下载
			break;
			case 0x36://发送数据
			break;
			case 0x37://请求退出发送
			break;
			default:
			break;
		
		
		
		}
	}
	else if(data[0] ==0x10) //连续帧请求第一帧
	{
	    if(data[2]==0x2E)
		{
		    if(Write2EEnableFlag == 1)
			{
				if((data[3]==0xF1)&&(data[4]==0x90))
				{
				    
				
				}
			
			}
			else
			{
			
			
			}
	
		
		}
	
	
	}
	else if(((data[0]>>4)&0x0F) == 0x02) //多帧请求后续帧
	{
	
	
	}
	else if(data[0] ==0x30) ////收到流控帧
	{
	    switch(UDS_FCDFlag)
		{
			case 1: //application Software Fingerprint Data Identifier
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x07;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0x05;
				CAR_UDSBro.msg[6] = 0x09;
				CAR_UDSBro.msg[7] = 0x02;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				break;
			case 2: //vehicle Manufacturer Spare Part Number Data Identifier
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x17;
				CAR_UDSBro.msg[2] = 0x21;
				CAR_UDSBro.msg[3] = 0x22;
				CAR_UDSBro.msg[4] = 0x23;
				CAR_UDSBro.msg[5] = 0x25;
				CAR_UDSBro.msg[6] = 0x29;
				CAR_UDSBro.msg[7] = 0x26;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x22;	
				CAR_UDSBro.msg[1] = 0x25;
				CAR_UDSBro.msg[2] = 0x27;
				CAR_UDSBro.msg[3] = 0xAA;
				CAR_UDSBro.msg[4] = 0xAA;
				CAR_UDSBro.msg[5] = 0xAA;
				CAR_UDSBro.msg[6] = 0xAA;
				CAR_UDSBro.msg[7] = 0xAA;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				break;
			case 3: //System Supplier Identifier Data Identifier
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x07;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0x05;
				CAR_UDSBro.msg[6] = 0x09;
				CAR_UDSBro.msg[7] = 0x02;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				break;
			case 4: //ECU Serial Number Data Identifier
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x07;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0xAA;
				CAR_UDSBro.msg[6] = 0xAA;
				CAR_UDSBro.msg[7] = 0xAA;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				break;
			case 5: //VIN Data Identifier
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x07;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0x40;
				CAR_UDSBro.msg[6] = 0x50;
				CAR_UDSBro.msg[7] = 0x60;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x22;	
				CAR_UDSBro.msg[1] = 0x30;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0x05;
				CAR_UDSBro.msg[6] = 0x09;
				CAR_UDSBro.msg[7] = 0x01;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
			
				break;
			case 6: //ECU Hardware Version
				CAR_UDSBro.CANx =CANx;
				CAR_UDSBro.canide =CAN_Id_Standard;
				CAR_UDSBro.canid =0x79B;
				CAR_UDSBro.msg[0] = 0x21;	
				CAR_UDSBro.msg[1] = 0x07;
				CAR_UDSBro.msg[2] = 0x01;
				CAR_UDSBro.msg[3] = 0x02;
				CAR_UDSBro.msg[4] = 0x03;
				CAR_UDSBro.msg[5] = 0xAA;
				CAR_UDSBro.msg[6] = 0xAA;
				CAR_UDSBro.msg[7] = 0xAA;
				CAR_UDSBro.len = 8;
				CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
				break;
			default:
				break;
				
		}
		if(UdsDtcFCDFlag ==1)  //多帧发送3个DTC
		{
	        CAR_UDSBro.CANx =CANx;
			CAR_UDSBro.canide =CAN_Id_Standard;
			CAR_UDSBro.canid =0x79B;
			CAR_UDSBro.msg[0] = 0x21;
			CAR_UDSBro.msg[1] =s_UdsDtcInfo.AntennaDtcStatus;			
			CAR_UDSBro.msg[2] = (u8)(s_UdsDtcInfo.SimCardDTC&0xFF);
			CAR_UDSBro.msg[3] = (u8)((s_UdsDtcInfo.SimCardDTC >>8)&0xFF);
			CAR_UDSBro.msg[4] = (u8)((s_UdsDtcInfo.SimCardDTC >>16)&0xFF);
			CAR_UDSBro.msg[5] = s_UdsDtcInfo.SimCardDtcStatus;
			CAR_UDSBro.msg[6] =(u8)(s_UdsDtcInfo.CanbusDTC&0xFF);
			CAR_UDSBro.msg[7] =(u8)((s_UdsDtcInfo.CanbusDTC >>8)&0xFF);
			CAR_UDSBro.len = 8;
			CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
			CAR_UDSBro.CANx =CANx;
			CAR_UDSBro.canide =CAN_Id_Standard;
			CAR_UDSBro.canid =0x79B;
			CAR_UDSBro.msg[0] = 0x22;	
			CAR_UDSBro.msg[1] = (u8)((s_UdsDtcInfo.CanbusDTC >>16)&0xFF);
			CAR_UDSBro.msg[2] = s_UdsDtcInfo.CanbusDtcStatus;
			CAR_UDSBro.msg[3] = 0xAA;
			CAR_UDSBro.msg[4] = 0xAA;
			CAR_UDSBro.msg[5] = 0xAA;
			CAR_UDSBro.msg[6] = 0xAA;
			CAR_UDSBro.msg[7] = 0xAA;
			CAR_UDSBro.len = 8;
			CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
		}
		else if(UdsDtcFCDFlag ==2) //多帧发送2个DTC
		{
		    CAR_UDSBro.CANx =CANx;
			CAR_UDSBro.canide =CAN_Id_Standard;
			CAR_UDSBro.canid =0x79B;
			CAR_UDSBro.msg[0] = 0x21;
			CAR_UDSBro.msg[1] =s_UdsDtcInfo.AntennaDtcStatus;			
			CAR_UDSBro.msg[2] = (u8)(s_UdsDtcInfo.SimCardDTC&0xFF);
			CAR_UDSBro.msg[3] = (u8)((s_UdsDtcInfo.SimCardDTC >>8)&0xFF);
			CAR_UDSBro.msg[4] = (u8)((s_UdsDtcInfo.SimCardDTC >>16)&0xFF);
			CAR_UDSBro.msg[5] = s_UdsDtcInfo.SimCardDtcStatus;
			CAR_UDSBro.msg[6] = 0xAA;
			CAR_UDSBro.msg[7] = 0xAA;
			CAR_UDSBro.len = 8;
			CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ;         
		}
	    else if(UdsDtcFCDFlag ==3) //多帧发送2个DTC
		{
		    CAR_UDSBro.CANx =CANx;
			CAR_UDSBro.canide =CAN_Id_Standard;
			CAR_UDSBro.canid =0x79B;
			CAR_UDSBro.msg[0] = 0x21;
			CAR_UDSBro.msg[1] =s_UdsDtcInfo.AntennaDtcStatus;			
			CAR_UDSBro.msg[2] = (u8)(s_UdsDtcInfo.CanbusDTC&0xFF);
			CAR_UDSBro.msg[3] = (u8)((s_UdsDtcInfo.CanbusDTC >>8)&0xFF);
			CAR_UDSBro.msg[4] = (u8)((s_UdsDtcInfo.CanbusDTC >>16)&0xFF);
			CAR_UDSBro.msg[5] = s_UdsDtcInfo.CanbusDtcStatus;
			CAR_UDSBro.msg[6] = 0xAA;
			CAR_UDSBro.msg[7] = 0xAA;
			CAR_UDSBro.len = 8;
			CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
		
		}
		else if(UdsDtcFCDFlag ==4) //多帧发送2个DTC
		{
		    CAR_UDSBro.CANx =CANx;
			CAR_UDSBro.canide =CAN_Id_Standard;
			CAR_UDSBro.canid =0x79B;
			CAR_UDSBro.msg[0] = 0x21;
			CAR_UDSBro.msg[1] =s_UdsDtcInfo.AntennaDtcStatus;			
			CAR_UDSBro.msg[2] = (u8)(s_UdsDtcInfo.CanbusDTC&0xFF);
			CAR_UDSBro.msg[3] = (u8)((s_UdsDtcInfo.CanbusDTC >>8)&0xFF);
			CAR_UDSBro.msg[4] = (u8)((s_UdsDtcInfo.CanbusDTC >>16)&0xFF);
			CAR_UDSBro.msg[5] = s_UdsDtcInfo.CanbusDtcStatus;
			CAR_UDSBro.msg[6] = 0xAA;
			CAR_UDSBro.msg[7] = 0xAA;
			CAR_UDSBro.len = 8;
			CanSendMsg(CAR_UDSBro.CANx,CAR_UDSBro.canide,CAR_UDSBro.canid,(u8 *)CAR_UDSBro.msg,CAR_UDSBro.len) ; 
		}
	
	
	
	}

}


