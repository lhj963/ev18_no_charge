/*************************************************************************
**  名称: Config.h文件
*   日期: 2019/3/18
*   作者: chensufei
*   描述: 提供公共的数据结构及API接口
*		
*   修改记录: 
*	   日期						修改内容
*	2019/3/18 		|		软件初始化版本，实现了以下接口:
					|			
**************************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "apps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
** 版本定义
**************************************************************************/

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;



/**************************************************************************
** 宏定义
**************************************************************************/
#define M_COMM_FUNC_SUCC				(0)				/* 返回成功 */
#define M_COMM_FUNC_FAIL				(-1)			/* 返回失败 */

#define M_GPS_MODULE					"GPS"			/* GPS模块 */
#define M_PROTOCOL_MODULE				"PROTOCOL"		/* 协议模块 */
#define M_GSM_AT_MODULE					"GSMAT"
#define M_TCP_MODULE					"TCP"
#define M_GPRS_MODULE					"GPRS"
#define M_FTP_MODULE					"FTP"
#define M_SMS_MODULE					"SMS"
#define M_CALL_MODULE					"CALL"

/* EC20--GSM模块 */
#define M_EC20_SUPPORT					(1)
#if M_EC20_SUPPORT > 0
#define M_EC20_GSM						"EC20"
#endif

/* SIM800--GSM模块 */
#define M_SIM800_SUPPORT				(0)
#if M_SIM800_SUPPORT > 0
#define M_SIM800_GSM					"SIM800"
#endif

/* M26--GSM模块 */
#define M_M26_SUPPORT					(0)
#if M_M26_SUPPORT > 0
#define M_M26_GSM						"M26"
#endif

#ifndef True
#define True							(1)
#endif

#ifndef False
#define False							(0)
#endif

#define M_DEBUG							(1)				/* debug开关 */

/* 打印重定义 */
#if M_DEBUG > 0
#define M_COMM_DEBUG(module, format,...)  	DBG_Print(LEVEL_GPRS, "[%s][%s][%d] debug: "format"\r\n", module, __func__, __LINE__, ##__VA_ARGS__)
#define M_COMM_WARN(module, format,...)  	DBG_Print(LEVEL_GPRS, "[%s][%s][%d] warn: "format"\r\n", module, __func__, __LINE__, ##__VA_ARGS__)
#define M_COMM_ERROR(module, format,...)  	DBG_Print(LEVEL_GPRS, "[%s][%s][%d] error: "format"\r\n", module, __func__, __LINE__, ##__VA_ARGS__)
#define M_COMM_INFO(module, format,...)  	DBG_Print(LEVEL_GPRS, "[%s][%s][%d] info: "format"\r\n", module, __func__, __LINE__, ##__VA_ARGS__)
#define M_COMM_TRACE_IN(module)				DBG_Print(LEVEL_GPRS, "[%s][%s][%d] trace in\r\n", module, __func__, __LINE__)
#define M_COMM_TRACE_OUT(module)			DBG_Print(LEVEL_GPRS, "[%s][%s][%d] trace out\r\n", module, __func__, __LINE__)
#else
#define M_COMM_DEBUG(module, format,...)
#define M_COMM_WARN(module, format,...)
#define M_COMM_ERROR(module, format,...)
#define M_COMM_INFO(module, format,...)
#define M_COMM_TRACE_IN(module)
#define M_COMM_TRACE_OUT(module)
#endif


/* 32位大端转换 */
#define M_BIG_ENDIAN_U32(x, y)		do\
									{\
									 y = ((x >> 24) & 0xff);\
									 y |= (((x >> 16) & 0xff) << 8);\
									 y |= (((x >> 8) & 0xff) << 16);\
									 y |= ((x & 0xff) << 24);\
									} while(0)
/* 16位大端转换 */
#define M_BIG_ENDIAN_U16(x, y)		do\
									{\
									 y = ((x >> 8) & 0xff);\
									 y |= ((x & 0xff) << 8);\
									} while(0)
/* b-从0开始取值，0-代表第一位 */
#define M_SET_BIT(x, b)			(x |= ((unsigned long)1 << b))
#define M_CLR_BIT(x, b)			(x &= (~((unsigned long)1 << b)))
#define M_GET_BIT(x, b)			(((unsigned long)x >> b) & 0x01)

#define M_DATA_SIZE(x)			(sizeof(x) / sizeof(x[0]))


/*
* 位带操作,实现51类似的GPIO控制功能
* 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
* IO口操作宏定义
*/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 



#ifdef __cplusplus
}
#endif

#endif

