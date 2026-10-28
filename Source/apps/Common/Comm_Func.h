/*************************************************************************
**  名称: Comm_Func.h文件
*   日期: 2019/3/26
*   作者: chensufei
*   描述: 提供公共功能的数据结构及API接口
*		
*   修改记录: 
*	   日期						修改内容
*	2019/3/18 		|		软件初始化版本，实现了以下接口:
					|			
**************************************************************************/
#ifndef __COMM_FUNC_H__
#define __COMM_FUNC_H__

#include "./SysParam/Config.h"

#ifdef __cplusplus
extern "C" {
#endif /* End __cplusplus */


/**************************************************************************
** 宏定义
**************************************************************************/


/**************************************************************************
** 结构体定义
**************************************************************************/
/* utc时分秒 */
typedef struct _CommTimeDataSt
{
	u8 hour;							/* 时，取值2位数 */
	u8 min; 							/* 分，取值2位数 */
	u8 sec; 							/* 秒，取值2位数 */
} __attribute__ ((packed)) CommTimeDataSt;

/* utc年月日 */
typedef struct _CommDateDataSt
{
	u8 year;							/* 年，取值2位数 */
	u8 month;							/* 月，取值2位数 */
	u8 day; 							/* 日，取值2位数 */
} __attribute__ ((packed)) CommDateDataSt;

/* utc年月日时分秒 */
typedef struct _CommDateTimeSt
{
	CommDateDataSt date;				/* 日期 */
	CommTimeDataSt time;				/* 时间 */
} __attribute__ ((packed)) CommDateTimeSt;

/**************************************************************************
** 函数声明
**************************************************************************/
/**************************************************************************
* 函  数: char *CommGetSubStr(char *src, char *substr1, char *substr2, char *outstr, u32 maxlen)
* 描  述: 获取pSubStr1和pSubStr2之间的字符串 
* 举  例: 
*		  CommGetSubStr("<ServerIP:192.168.1.19>", "<ServerIP:", ">", sub, sizeof(sub));
*		  执行结果：sub = "192.168.1.19"
* 入  参: char *src: 源字符串
*		  char *substr1: 起始子字符串
*		  char *substr2: 结尾子字符串
*		  u32 maxlen: 传入的缓冲区大小
* 出  参: char *outstr: 调用者传入的缓冲区 用于接收pSubStr1与pSubStr2之间的内容
* 返回值: 如果存在,则返回substr1的指针；否则返回NULL
**************************************************************************/
char *CommGetSubStr(char *src, char *substr1, char *substr2, char *outstr, u32 maxlen);

/**************************************************************************
* 函  数: u32 CommGetSubStrCnt(char *src, u32 srclen, char *substr, u32 sublen)
* 描  述: 统计子在源字符串中src中查找子字符串substr出现的次数
* 举  例: 
*		  u32 cnt = CommGetSubStrCnt("1212123456", 10, "12", 2);
*		  cnt = 3次
* 入  参: char *src: 源字符串
*         u32 srclen: 源字符串长度
*		  char *substr: 子字符串
*		  u32 sublen: 子字符串长度
* 出  参: void
* 返回值: u32 : 返回出现的次数
**************************************************************************/
u32 CommGetSubStrCnt(char *src, u32 srclen, char *substr, u32 sublen);


/**************************************************************************
* 函  数: u32 CommSystimeToSec(CommDateTimeSt *tm)
* 描  述: 把系统时间转成秒数,从2000年开始算起
* 举  例: 
* 入  参: CommDateTimeSt *tm :系统时间
* 出  参: void
* 返回值: u32 - 返回秒数
**************************************************************************/
u32 CommSystimeToSec(CommDateTimeSt *tm);

/**************************************************************************
* 函  数: void CommSecToSystime(u32 sec, CommDateTimeSt *tm)
* 描  述: 把秒数转成系统时间,超过2000年后
* 举  例: 
* 入  参: u32 sec: 秒数
* 出  参: CommDateTimeSt *tm :系统时间
* 返回值: void
**************************************************************************/
void CommSecToSystime(u32 sec, CommDateTimeSt *tm);

/**************************************************************************
* 函  数: int CommSumCheckCode(u8 *data, u32 datalen)
* 描  述: 计算校验和，异或累加计算法
* 举  例: 
* 入  参: u8 *data:数据流
*		  u32 datalen:数据流长度
* 出  参: void
* 返回值: u8 : 校验码值
**************************************************************************/
u8 CommSumCheckCode(u8 *data, u32 datalen);

/**************************************************************************
* 函  数: void CommToLowerAlpha(char *data, int len)
* 描  述: 对字符串中出现的大写字母全部全部转化成小写字母
* 举  例: 
* 入  参: char *data:数据
*		  int len:数据长度
* 出  参: void
* 返回值: void
**************************************************************************/
void CommToLowerAlpha(char *data, int len);

/**************************************************************************
* 函  数: void CommToUpperAlpha(char *data, int len)
* 描  述: 对字符串中出现的小写字母全部全部转化成大写字母
* 举  例: 
* 入  参: char *data:数据
*		  int len:数据长度
* 出  参: void
* 返回值: void
**************************************************************************/
void CommToUpperAlpha(char *data, int len);

/**************************************************************************
* 函  数: u8* CommCheckSubStr(u8 *srcData, u32 srcLen, u8 *subData, u32 subLen)
* 描  述: 判断源数据中，是否有子数据信息
* 举  例: 
* 入  参: char *srcData：
*		  u32 srcLen:
*		  char *subData：
*		  u32 subLen
* 出  参: void
* 返回值: u8*: 0 - 失败， 子数据开始的位置
**************************************************************************/
u8* CommCheckSubStr(u8 *srcData, u32 srcLen, u8 *subData, u32 subLen);

/**************************************************************************
* 函  数: int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
* 描  述: 16进制字符串转换成16进制数据流
* 举  例: 
* 入  参: char *hexStr：16进制字符串
*	      int hexlen：16进制字符串长度
* 出  参: u8 *data:16进制数据
* 返回值: int ： M_COMM_FUNC_FAIL - 失败, > 0 长度
**************************************************************************/
int CommHexStrToHex(char *hexStr, int hexlen, u8 *data);
/**************************************************************************
* 函  数: int CommGetBcdData(u8 *outBcd, char *inStr, int len)
* 描  述: 16进制字符串转换成16进制数据流
* 举  例: 123456 - 0x12 0x34 0x56
          12345 - 0x01, 0x23, 0x45
* 入  参: char *inStr：数字字符串
*	      int hexlen：16进制字符串长度
* 出  参: u8 *outBcd:16进制BCD码数据
* 返回值: int ： M_COMM_FUNC_FAIL - 失败, > 0  BCD码数据长度
**************************************************************************/
int CommGetBcdData(u8 *outBcd, char *inStr, int len);

/**************************************************************************
* 函  数: u8 CommDecToBcd(u8 dec)
* 描  述: 将10进制数换成BCD码
* 举  例: 12 -> 0x12
* 入  参: u8 dec：10进制数值,取值范围0 - 99
* 出  参: void
* 返回值: u8 : bcd码的16进制数据
**************************************************************************/
u8 CommDecToBcd(u8 dec);


#ifdef __cplusplus
}
#endif	/* End __cplusplus */

#endif  /* End __COMM_FUNC_H__ */

