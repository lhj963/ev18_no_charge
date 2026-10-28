#ifndef __TWO_CPU_COM_PROTOCOL_H__
#define __TWO_CPU_COM_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif

//主机和从机都可以发送的消息
#define TCCP_FILE_SEND_START_MSG_ID 		0x2002	//发送文件消息
#define TCCP_SEND_FILE_MSG_ID 				0x2000	//发送文件消息
#define TCCP_FILE_SEND_FINISH_MSG_ID 		0x2001	//文件发送完成消息
#define TCCP_FILE_SEND_NEXT_MSG_ID 			0x2003	//文件发送下一包

//从机发送的消息
#define TCCP_SLAVE_SEND_CAN_MSG_ID 			0x1000	//从机发送CAN数据消息
#define TCCP_SLAVE_UN_ACK_MSG_ID 			0x1001	//从机机通用应答
#define TCCP_SLAVE_UPDATA_RES_MSG_ID 		0x1002	//从机汇报升级结果
#define TCCP_SLAVE_REPORT_VER_MSG_ID 		0x1003	//从机汇报软件版本号
#define TCCP_SLAVE_ACK_BEIAN_E_MSG_ID 		0x1004	//从机汇报密文备案信息
#define TCCP_SLAVE_ACK_BEIAN_M_MSG_ID 		0x1005	//从机应答明文备案信息
#define TCCP_SLAVE_ACK_SIGNA_MSG_ID 		0x1006	//从机应答签名数据

//主机发送的消息
#define TCCP_HOST_UN_ACK_MSG_ID 					0x0001	//主机通用应答
#define TCCP_HOST_UPDATA_RES_MSG_ID 				0x0002	//主机汇报升级结果
#define TCCP_HOST_QUERY_SLAVE_CPU_VER_MSG_ID 		0x0003	//主机查询从机版本号
#define TCCP_HOST_REQ_BEIAN_E_MSG_ID 				0x0004	//主机请求密文备案信息
#define TCCP_HOST_REQ_BEIAN_M_MSG_ID 				0x0005	//主机请求明文备案信息
#define TCCP_HOST_REQ_SIGNA_MSG_ID 					0x0006	//主机请求签名数据

#define TCCP_GZ_HEART_BEAT_MSG_ID 					0x3000	//工装心跳
#define TCCP_GZ_MASTER_SET_CHANNEL_MSG_ID 			0x3002	//工装主CPU设置通道号
#define TCCP_AUTO_TEST_MSG_ID 						0x3004	//启动/停止自动化测试
#define TCCP_REPORT_DOWNLOAD_PROGRESS_MSG_ID		0x3006	//汇报设备程序下载进度
#define TCCP_ACK_DOWNLOAD_PROGRESS_MSG_ID			0x3007	//上位机响应已收到设备程序下载进度消息

#define TCCP_GZ_MASTER_DISP_INFO_MSG_ID				0x3008	//上位机给显示屏下发显示信息


#define TCCP_DEVICE_HEART_BEAT_MSG_ID 				0x4000	//设备心跳
#define TCCP_DEVICE_HEART_BEAT_MSG_ID1 				0x4001	//设备心跳


#define TCCP_FORWARD_MODE_MSG_ID 					0x5000	//进入/退出 转发模式
#define TCCP_GZ_SLAVE_FORWARD_MODE_MSG_ID 			0x6000	//进入/退出 与工装从机对话模式

#define TCCP_USART_DOWNLOAD_FILE_MSG_ID 			0x7000	//启动串口下载app文件
#define TCCP_USART_DOWNLOAD_BOOT_FILE_MSG_ID 		0x7002	//启动串口下载boot文件

#define TCCP_TRANS_CMD_MSG_ID 						0x9000	//透传设备指令
#define TCCP_ACK_TRANS_CMD_MSG_ID 					0x9001	//响应透传设备指令

#define TCCP_GZ_SLAVE_SPEAK							0x9002	//工装从机反馈调试信息消息ID
#define TCCP_SET_BACKDOOR_HEART_INV_MSG_ID 			0x9004	//设置远程管理心跳时间间隔

#define TCCP_DEVICE_REPORT_LOG_MSG_ID 				0x9006	//上报远程日志


#define TCCP_RP_CAN_CMD_MSG_ID 					0x9101	//上报CAN数据指令

extern unsigned int tccp_pack(unsigned short msgid,void *content,unsigned int contentlen,
							void *pOutBuf,unsigned int outbuflen);

/* 解包 */
extern int tccp_unpack(void *pdat,unsigned int len,
					void (*pCallBackAnalyze)(unsigned int msgid,void *content,unsigned int contentlen),
					void (*pCallBackError)(void *pdat,unsigned int len));


#ifdef __cplusplus
}
#endif
#endif

