/*************************************************************************
**  名称: Comm_Func.c文件
*   日期: 2019/3/26
*   作者: chensufei
*   描述: 实现公共功能的API接口
*		
*   修改记录: 
*	   日期		修改人		版本 		修改内容
*	2019/3/26 	chensufei	V0.01       软件初始化版本，	
**************************************************************************/
#include "Comm_Func.h"

/**************************************************************************
** 宏定义
**************************************************************************/

/**************************************************************************
** 数据结构定义
**************************************************************************/

/**************************************************************************
** 全局变量定义
**************************************************************************/
/* 每月天数 */
const u8 g_commDaysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**************************************************************************
** 函数定义
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
char *CommGetSubStr(char *src, char *substr1, char *substr2, char *outstr, u32 maxlen)
{
	char *pTemp1 = 0;
	char *pTemp2 = 0;
	u32 templen = 0;

	if ((0 == src) || (0 == substr1) || (0 == substr2) || (0 == outstr) || (0 == maxlen))
	{
		return 0;
	}

	/* 未查找到字符串1 */
	pTemp1 = strstr(src, substr1);
	if (!pTemp1)
	{
		return 0;
	}

	/* 未查找到字符串2 */
	pTemp2 = strstr(pTemp1 + strlen((char *)substr1), substr2);
	if (!pTemp2)
	{
		return 0;
	}

	templen = pTemp2 - (pTemp1 + strlen((char *)substr1));
	templen = (maxlen >= templen) ? templen : maxlen;
	if (templen > 0)
	{
		memcpy(outstr, pTemp1 + strlen((char *)substr1), templen);
	}

	return pTemp1;
}

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
u32 CommGetSubStrCnt(char *src, u32 srclen, char *substr, u32 sublen)
{
	u32 cnt = 0;
	char *pStart = 0;
	char *pSrc = src;
	u32 index = 0;

	if ((0 == src) || (0 == substr) || (0 == srclen) || (0 == sublen))
	{
		return 0;
	}

	/* 子字符长度过长 */
	if (sublen > srclen)
	{
		return 0;
	}

	while (pSrc)
	{
		pStart = strstr(pSrc, substr);
		if (!pStart)
		{
			break;
		}

		index += (pStart - pSrc) + sublen;
		if (index > srclen)
		{
			break;
		}

		cnt++;
		pSrc += (pStart - pSrc) + sublen;
	}

	return cnt;
}

/**************************************************************************
* 函  数: u8 ModemAtIsLeapYear(u16 year)
* 描  述: 判断是否时闰年
* 举  例: 
* 入  参: u16 year
* 出  参: void
* 返回值: u8
**************************************************************************/
static u8 CommIsLeapYear(u16 year)
{
    if ((0 == (year % 400)) || ((0 == (year % 4)) && (0 != (year % 100))))
    {
        return 1;
    }

    return 0;
}

/**************************************************************************
* 函  数: u32 CommSystimeToSec(CommDateTimeSt *tm)
* 描  述: 把系统时间转成秒数,从2000年开始算起
* 举  例: 
* 入  参: CommDateTimeSt *tm :系统时间
* 出  参: void
* 返回值: u32 - 返回秒数
**************************************************************************/
u32 CommSystimeToSec(CommDateTimeSt *tm)
{
    u8 i = 0;
	u16 days = 0;

	if (0 == tm)
	{
		return 0;
	}

    if (tm->date.year == 0)
    {
        days = 0;
    }
    else
    {
        days = tm->date.year * 365 + ((tm->date.year - 1)/4 + 1); /* 从2000年开始，闰年的天数 */
    }

    for (i = 1; i < tm->date.month; i++)
    {
        days += g_commDaysOfMonth[i - 1];
    }

    if ((tm->date.month > 2) && (0 == (tm->date.year % 4)))
    {
        days++;
    }
    
    days += tm->date.day - 1;	/* 需要减去当天 */
    
    return (days * (24 * 3600) + (tm->time.hour * 3600) + (tm->time.min * 60) + tm->time.sec);
}

/**************************************************************************
* 函  数: void CommSecToSystime(u32 sec, CommDateTimeSt *tm)
* 描  述: 把秒数转成系统时间,超过2000年后
* 举  例: 
* 入  参: u32 sec: 秒数
* 出  参: CommDateTimeSt *tm :系统时间
* 返回值: void
**************************************************************************/
void CommSecToSystime(u32 sec, CommDateTimeSt *tm)
{
	u32 days = 0;
	u32 seconds = 0;
	u16 year = 0;
	u16 index = 0;

	if ((0 == sec) || (0 == tm))
	{
		return;
	}

	days = sec / 86400; // 得到天数(秒钟数对应的)

	year = 2000; 		// 从2000年开始算起
	while(days >= 365)
	{
		if(CommIsLeapYear(year))
		{
			if(days >= 366)
			{
				/* 润年 */
                days -= 366;
			}
			else
			{
			    break;
			}
		}
		else
		{
			/* 平年 */
            days -= 365;
		}
		year++;
	}
	tm->date.year = (year % 100); /* 得到年份 */
	
	while(days >= 28) 		/* 超过了一个月 */
	{
		if(CommIsLeapYear(year) && (1 == index))
		{
			if(days >= 29)
			{
				/* 润年 */
                days -= 29;
			}
			else
			{
                break;
			}
		}
		else
		{
			if(days >= g_commDaysOfMonth[index])
			{
				/* 平年 */
                days -= g_commDaysOfMonth[index];
			}
			else
			{
                break;
			}
		}
		index++;
	}
	tm->date.month = index + 1;		/* 得到月份 */
	tm->date.day = days + 1; 		/* 得到日期 */

	seconds = sec % 86400; 		/* 得到秒钟数 */
	tm->time.hour = seconds / 3600; 	   /* 小时 */
	tm->time.min = (seconds % 3600) / 60; /* 分钟 */
	tm->time.sec = (seconds % 3600) % 60; /* 秒钟 */
}

/**************************************************************************
* 函  数: int CommSumCheckCode(u8 *data, u32 datalen)
* 描  述: 计算校验和，异或累加计算法
* 举  例: 
* 入  参: u8 *data:数据流
*		  u32 datalen:数据流长度
* 出  参: void
* 返回值: u8 : 校验码值
**************************************************************************/
u8 CommSumCheckCode(u8 *data, u32 datalen)
{
	u8 code = 0;
	u32 index = 0;

	for (index = 0; index < datalen; index++)
	{
		code ^= data[index];
	}

	return code;
}

/**************************************************************************
* 函  数: void CommToLowerAlpha(char *data, int len)
* 描  述: 对字符串中出现的大写字母全部全部转化成小写字母
* 举  例: 
* 入  参: char *data:数据
*		  int len:数据长度
* 出  参: void
* 返回值: void
**************************************************************************/
void CommToLowerAlpha(char *data, int len)
{
	int i = 0;

	/* 进行大小写转换 */
	for (i = 0; i < len; i++)
	{
		if ((data[i] >= 'A') && (data[i] <= 'Z'))
		{
			data[i] = data[i] - 'A' + 'a';
		}
	}
}

/**************************************************************************
* 函  数: void CommToUpperAlpha(char *data, int len)
* 描  述: 对字符串中出现的小写字母全部全部转化成大写字母
* 举  例: 
* 入  参: char *data:数据
*		  int len:数据长度
* 出  参: void
* 返回值: void
**************************************************************************/
void CommToUpperAlpha(char *data, int len)
{
	int i = 0;

	/* 进行大小写转换 */
	for (i = 0; i < len; i++)
	{
		if ((data[i] >= 'a') && (data[i] <= 'z'))
		{
			data[i] = data[i] - 'a' + 'A';
		}
	}
}

/**************************************************************************
* 函  数: u8* CommCheckSubStr(u8 *srcData, u32 srcLen, u8 *subData, u32 subLen)
* 描  述: 判断源数据中，是否有子数据信息，输出子数据中第一个字符
* 举  例: 
* 入  参: u8 *srcData：
*		  u32 srcLen:
*		  u8 *subData：
*		  u32 subLen
* 出  参: void
* 返回值: u8*: 0 - 失败， 子数据开始的位置
**************************************************************************/
u8* CommCheckSubStr(u8 *srcData, u32 srcLen, u8 *subData, u32 subLen)
{
	int ret = M_COMM_FUNC_FAIL;
	int startpos = 0;
	u32 i = 0;
	u32 j = 0;

	do
	{
		if ((0 == srcData) || (0 == srcLen) || (0 == subData) || (0 == subLen))
		{
			break;
		}

		for (i = 0; i < srcLen; )
		{
			startpos = i;
			for (j = 0; j < subLen; j++)
			{
				if (srcData[i] != subData[j])
				{
					if (0 == j)
					{
						i++;
					}
					break;
				}
				i++;
			}

			if (j == subLen)
			{
				ret = M_COMM_FUNC_SUCC;
				//M_COMM_DEBUG("COMM", "find str[%d]:%s, i:%d, j:%d", subLen, subData, i, j);
				break;
			}
			startpos = 0;
		}
	} while(0);

	if (0 == ret)
	{
		return (void *)&srcData[startpos];
	}

	return 0;
}

/**************************************************************************
* 函  数: int CommCharToDec(char *chr)
* 描  述: 将字符转换成10进制的数值
* 举  例: 
* 入  参: char *chr：字符值
* 出  参: void
* 返回值: int ： M_COMM_FUNC_FAIL - 失败, >= 0 成功
**************************************************************************/
int CommCharToDec(char chr)
{
	if ((chr >= '0') && (chr <= '9'))
	{
		return (chr - 0x30);
	}

	if ((chr >= 'A') && (chr <= 'F'))
	{
		return ((chr - 'A') + 10);
	}

	if ((chr >= 'a') && (chr <= 'f'))
	{
		return ((chr - 'a') + 10);
	}

	return M_COMM_FUNC_FAIL;
}

/**************************************************************************
* 函  数: int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
* 描  述: 16进制字符串转换成16进制数据流
* 举  例: 123456 - 0x12 0x34 0x56
* 入  参: char *hexStr：16进制字符串
*	      int hexlen：16进制字符串长度
* 出  参: u8 *data:16进制数据
* 返回值: int ： M_COMM_FUNC_FAIL - 失败, > 0 长度
**************************************************************************/
int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
{
	int ret = M_COMM_FUNC_FAIL;
	int i = 0;
	int datalen = 0;
	u8 high = 0;
	u8 low = 0;

	do
	{
		if ((0 == hexStr) || (0 == hexlen) || (0 == data))
		{
			M_COMM_ERROR("Comm", "input param error.");
			break;
		}

		if (0 != (hexlen % 2))
		{
			M_COMM_ERROR("Comm", "hexstr len[%d] is error", hexlen);
			break;
		}

		CommToUpperAlpha(hexStr, hexlen);

		for (i = 0; i < hexlen;)
		{
			ret = CommCharToDec(hexStr[i]);
			if (M_COMM_FUNC_FAIL == ret)
			{
				break;
			}
			high = ret;

			ret = CommCharToDec(hexStr[i + 1]);
			if (M_COMM_FUNC_FAIL == ret)
			{
				break;
			}
			low = ret;

			data[datalen++] = (high << 4) + low;
			i += 2;
		}

		if (M_COMM_FUNC_FAIL == ret)
		{
			break;
		}

		ret = datalen;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: int CommGetBcdData(u8 *outBcd, char *inStr, int len)
* 描  述: 16进制字符串转换成16进制数据流
* 举  例: "123456" - 0x12 0x34 0x56
          12345 - 0x01, 0x23, 0x45
* 入  参: char *inStr：数字字符串
*	      int hexlen：16进制字符串长度
* 出  参: u8 *outBcd:16进制BCD码数据
* 返回值: int ： M_COMM_FUNC_FAIL - 失败, > 0  BCD码数据长度
**************************************************************************/
int CommGetBcdData(u8 *outBcd, char *inStr, int len)
{
	int i = 0;
	int length = 0;

	for (i = 0; i < len; i++)
	{
		if ((inStr[i] < '0') || (inStr[i] > '9'))
		{
			return M_COMM_FUNC_FAIL;
		}
	}

	if (1 == (len % 2))
	{
		outBcd[length++] = inStr[0] - '0';
		for (i = 1; i <= (len/2); i++)
		{
			outBcd[length++] = ((inStr[2 * i - 1] - '0') << 4) | (inStr[2 * i] - '0');
		}
	}
	else
	{
		for (i = 0; i < (len/2); i++)
		{
			outBcd[length++] = ((inStr[2 * i] - '0') << 4) | (inStr[2 * i + 1] - '0');
		}
	}

	return length;
}

/**************************************************************************
* 函  数: u8 CommDecToBcd(u8 dec)
* 描  述: 将10进制数换成BCD码
* 举  例: 12 -> 0x12
* 入  参: u8 dec：10进制数值,取值范围0 - 99
* 出  参: void
* 返回值: u8 : bcd码的16进制数据
**************************************************************************/
u8 CommDecToBcd(u8 dec)
{
	if (dec > 100)
	{
		return 0;
	}

	return (u8)(((dec / 10) << 4) | (dec % 10));
}


