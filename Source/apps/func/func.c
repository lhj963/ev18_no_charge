#include "func.h"


typedef long long unsigned int INT64U;
typedef long long int INT64S;

typedef struct CAN_BYTES_DATA_t
{
	 INT64U BYTE0:8;
	 INT64U BYTE1:8;
	 INT64U BYTE2:8;
	 INT64U BYTE3:8;
	 INT64U BYTE4:8;
	 INT64U BYTE5:8;
	 INT64U BYTE6:8;
	 INT64U BYTE7:8;
} CAN_BYTES_DATA;

typedef union
{
	INT64U all;
	CAN_BYTES_DATA bit;
}CAN_UINT64_DATA;

unsigned int DecodeCANFrameData(INT8U frame_data[], INT8U bit_start, INT8U bit_len, INT8U motorola)
{
	INT64U index, mask,value;
	CAN_UINT64_DATA long_data;

	if(motorola)
	{
		index = 56 + (bit_start % 8) - (bit_start / 8 * 8);
		mask = (0x1ULL << bit_len)-1;
		long_data.bit.BYTE0 = frame_data[7];
		long_data.bit.BYTE1 = frame_data[6];
		long_data.bit.BYTE2 = frame_data[5];
		long_data.bit.BYTE3 = frame_data[4];
		long_data.bit.BYTE4 = frame_data[3];
		long_data.bit.BYTE5 = frame_data[2];
		long_data.bit.BYTE6 = frame_data[1];
		long_data.bit.BYTE7 = frame_data[0];
		long_data.all >>= index;
		value = long_data.all & mask;
	}
	else
	{
		index = bit_start;
		mask = (0x1ULL << bit_len) - 1;
		long_data.bit.BYTE0 = frame_data[0];
		long_data.bit.BYTE1 = frame_data[1];
		long_data.bit.BYTE2 = frame_data[2];
		long_data.bit.BYTE3 = frame_data[3];
		long_data.bit.BYTE4 = frame_data[4];
		long_data.bit.BYTE5 = frame_data[5];
		long_data.bit.BYTE6 = frame_data[6];
		long_data.bit.BYTE7 = frame_data[7];
		long_data.all >>= index;
		value = long_data.all & mask;
	}
	return value;
}

#define get_one_byte_of_bit(dat,start_bit,bit_len) (((dat) >> (start_bit)) & ((1 << (bit_len)) - 1))

unsigned int decode_can_frame(void *frame_data,unsigned char bit_start,unsigned char bit_len,unsigned char motorola)
{
	unsigned char temp_bit = bit_start;
	unsigned char *pdat = (unsigned char *)frame_data;
	unsigned int dat = 0;
	unsigned char templen = 0;
	unsigned char which_byte = 0;
	unsigned char sumlen = 0;

	while (bit_len)
	{
		which_byte = temp_bit / 8;
		templen = (which_byte + 1) * 8 - temp_bit;
		if (templen >= bit_len)
		{
			templen = bit_len;
		}

		if (which_byte >= 8)
		{
			break;
		}
		dat += (get_one_byte_of_bit(pdat[which_byte],temp_bit % 8,templen) << sumlen);

		sumlen += templen;
		bit_len -= templen;
		if (motorola)
		{
			temp_bit = (which_byte - 1) * 8;
		}
		else
		{
			temp_bit = (which_byte + 1) * 8;
		}		
	}

	return dat;
}


void iccid_str_to_8byte(void *iccid,void *pbuf)
{
	double d = 0;
	unsigned char iccid_buf[22] = {0};
	unsigned char *ptr = (unsigned char *)pbuf;
	unsigned char i = 0;
	unsigned char temp = 0;
	unsigned char cnt = 0;

	for (i = 0; i < 16; i++)
	{
		temp = *((unsigned char *)iccid + 4 + i);
		if ((temp >= '0') && (temp <= '9'))
		{
			iccid_buf[cnt++] = temp;
		}
		else
		{
			iccid_buf[cnt++] = (temp >> 4) + '0';			
			iccid_buf[cnt++] = (temp & 0x0F) + '0';
		}

		if (cnt >= 21)
		{
			break;
		}
	}

	d = atof((char *)iccid_buf);
	ptr[0] = (INT32U)d;
	ptr[1] = d / 256;
	ptr[2] = d / 65536;
	ptr[3] = d / (256.0*256*256);
	ptr[4] = d / (256.0*256*256*256);
	ptr[5] = d / (256.0*256*256*256*256);
	ptr[6] = d / (256.0*256*256*256*256*256);
	ptr[7] = d / (256.0*256*256*256*256*256*256);
}

char is_bcd_time_format_ok(INT8U bcd_time[6])
{
	INT8U time[6] = {0};

	memcpy(time,bcd_time,6);

	//年是否合法
	if (time[0] > 0x99)
	{
		return 0;
	}
	//月是否合法
	if ((time[1] > 0x12) || (time[1] == 0))
	{
		return 0;
	}
	//日是否合法
	if ((time[2] > 0x31) || (time[2] == 0))
	{
		return 0;
	}
	//时是否合法
	if (time[3] > 0x23)
	{
		return 0;
	}

	//分是否合法
	if (time[4] > 0x59)
	{
		return 0;
	}

	//秒是否合法
	if (time[5] > 0x59)
	{
		return 0;
	}

	return 1;
}


char is_time_format_ok(INT8U time[6])
{
	//年是否合法
	if ((time[0] > 60) || (time[0] < 22))
	{
		return 0;
	}
	//月是否合法
	if ((time[1] > 12) || (time[1] == 0))
	{
		return 0;
	}
	//日是否合法
	if ((time[2] > 31) || (time[2] == 0))
	{
		return 0;
	}
	//时是否合法
	if (time[3] > 23)
	{
		return 0;
	}

	//分是否合法
	if (time[4] > 59)
	{
		return 0;
	}

	//秒是否合法
	if (time[5] > 59)
	{
		return 0;
	}

	return 1;
}

int timecmp(INT8U time1[6],INT8U time2[6])
{
	char buf1[16] = {0};
	char buf2[16] = {0};
	int ret1 = 0;

	snprintf(buf1,sizeof(buf1),"%02d%02d%02d%02d%02d%02d",time1[0],time1[1],time1[2],time1[3],time1[4],time1[5]);
	snprintf(buf2,sizeof(buf2),"%02d%02d%02d%02d%02d%02d",time2[0],time2[1],time2[2],time2[3],time2[4],time2[5]);

	ret1 = memcmp(buf1,buf2,12);
	return ret1;
}


//判断目标时间desTime 是否介于起始时间和结束时间之内 包含起始时间和结束时间
//201911333333  202001  20200232332233
char is_between_time(INT8U desTime[6],INT8U start_time[6],INT8U end_time[6])
{
	char buf1[16] = {0};
	char buf2[16] = {0};
	char buf3[16] = {0};
	int ret1 = 0;
	int ret2 = 0;

	if ((is_time_format_ok(desTime) == 0)
		|| (is_time_format_ok(start_time) == 0)
		|| (is_time_format_ok(end_time) == 0)
	)
	{
		return 0;
	}

	snprintf(buf1,sizeof(buf1),"%02d%02d%02d%02d%02d%02d",desTime[0],desTime[1],desTime[2],desTime[3],desTime[4],desTime[5]);
	snprintf(buf2,sizeof(buf2),"%02d%02d%02d%02d%02d%02d",start_time[0],start_time[1],start_time[2],start_time[3],start_time[4],start_time[5]);
	snprintf(buf3,sizeof(buf3),"%02d%02d%02d%02d%02d%02d",end_time[0],end_time[1],end_time[2],end_time[3],end_time[4],end_time[5]);

	ret1 = memcmp(buf1,buf2,12);
	ret2 = memcmp(buf1,buf3,12);

	if ((ret1 >= 0) && (ret2 <= 0))
	{
		return 1;
	}
	return 0;
}

#include "time.h"

//计算两个时间直接的间隔有多少秒
unsigned int two_time_inv(INT8U start_time[6],INT8U end_time[6])
{
	struct tm time_cha;
	unsigned long t1 = 0;

	struct tm time_cha1;
	unsigned long t2 = 0;

	if ((is_time_format_ok(start_time) == 0) || (is_time_format_ok(end_time) == 0))
	{
		return 0;
	}

	time_cha.tm_year = (start_time[0] + 2000) - 1900; //tm结构记录年为实际-1900
	time_cha.tm_mon = start_time[1] - 1;
	time_cha.tm_mday = start_time[2];
	time_cha.tm_hour = start_time[3];
	time_cha.tm_min = start_time[4];
	time_cha.tm_sec = start_time[5];
	t1 = mktime(&time_cha);//获得从1970年1月1日0时0分0秒以来过去的时间，秒

	time_cha1.tm_year = (end_time[0] + 2000) - 1900; //tm结构记录年为实际-1900
	time_cha1.tm_mon = end_time[1] - 1;
	time_cha1.tm_mday = end_time[2];
	time_cha1.tm_hour = end_time[3];
	time_cha1.tm_min = end_time[4];
	time_cha1.tm_sec = end_time[5];
	t2 = mktime(&time_cha1);//获得从1970年1月1日0时0分0秒以来过去的时间，秒

	return (t2 - t1);
}

unsigned char is_string(void *ptr,unsigned int len)
{
	unsigned int i = 0;
	unsigned char dat = 0;

	if ((ptr == 0) || (len == 0))
	{
		return 0;
	}

	for (i = 0; i < len; i++)
	{
		dat = *((unsigned char *)ptr + i);
		if (((dat >= 'a') && (dat <= 'z')) || ((dat >= 'A') && (dat <= 'Z')) || ((dat >= '0') && (dat <= '9')))
		{

		}
		else
		{
			return 0;
		}
	}

	return 1;
}

//全0 或 全 FF 的判定为空
int IsObjNULL(void *pdat,INT32U len)
{
	INT32U i = 0;
	INT8U temp = 0;

	for (i = 0; i < len; i++)
	{
		temp = *((INT8U *)pdat + i);
		
		if ((temp != 0x00) && (temp != 0xFF))
		{
			return 0;
		}
	}
	return 1;
}

unsigned char ToBcd(unsigned char dec)
{
	if (dec >= 100)
	{
		return 0;
	}

	return (unsigned char)(((dec / 10) << 4) | (dec % 10));
}

INT8U bcd_to_dec(INT8U bcd)
{
	INT8U high4 = (bcd >> 4) & 0x0F;
	INT8U low4 = bcd & 0x0F;

	return high4 * 10 + low4;
}


#if ENABLE_PLATFORM_REMOTE_WGET
static int platform_url_char_ok(unsigned char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
	{
		return 1;
	}
	switch (c)
	{
	case '.':
	case '-':
	case '_':
	case '/':
	case ':':
	case '?':
	case '=':
	case '%':
	case '~':
	case '@':
		return 1;
	default:
		return 0;
	}
}
#endif

int platform_remote_wget_allowed(const char *cmd)
{
#if !ENABLE_PLATFORM_REMOTE_WGET
	(void)cmd;
	return 0;
#else
	static const char prefix[] = "ATEX=<linuxcmd:wget -O /data/";
	static const char *files[] = {
		"box.bin ",
		"sandian_mcu.zip ",
		"sandian_bms.zip ",
		"sandian_vcu.zip "
	};
	const char *p = 0;
	unsigned int i = 0;
	unsigned int n = 0;
	int file_ok = 0;

	if (cmd == 0)
	{
		return 0;
	}
	n = (unsigned int)strlen(prefix);
	if (strncmp(cmd,prefix,n) != 0)
	{
		return 0;
	}
	p = cmd + n;
	for (i = 0; i < (unsigned int)(sizeof(files) / sizeof(files[0])); i++)
	{
		unsigned int fl = (unsigned int)strlen(files[i]);
		if (strncmp(p,files[i],fl) == 0)
		{
			p += fl;
			file_ok = 1;
			break;
		}
	}
	if (!file_ok)
	{
		return 0;
	}
	if ((strncmp(p,"http://",7) != 0) && (strncmp(p,"ftp://",6) != 0))
	{
		return 0;
	}
	n = (unsigned int)strlen(p);
	if ((n < 8) || (n > 240) || (p[n - 1] != '>'))
	{
		return 0;
	}
	for (i = 0; i + 1 < n; i++)
	{
		if (!platform_url_char_ok((unsigned char)p[i]))
		{
			return 0;
		}
	}
	return 1;
#endif
}

INT8U XOR_Check(INT8U *pdat,INT32U len)
{
	INT32U i = 0;
	INT8U temp = pdat[0];

	if (len == 1) return pdat[0];
	
	for (i = 1; i < len; i++)
	{
		temp ^= pdat[i];
	}
	return temp;
}


/**
函数功能：获取pSubStr1和pSubStr2之间的字符串 
入口参数:
		str:源字符串
		pSubStr1:起始子字符串
		pSubStr2:结尾子字符串
		outbuflen:传入的缓冲区大小
出口参数:
		pOutBuf:调用者传入的缓冲区 用于接收pSubStr1与pSubStr2之间的内容
返回值:如果存在则返回pSubStr1的指针 否则返回NULL
调用示例：GetSubString("<ServerIP:192.168.1.19>","<ServerIP:",">",sub);
		执行结果：sub = "192.168.1.19"
 */
char *GetSubString(char *str,char *pSubStr1,char *pSubStr2,char *pOutBuf,unsigned int outbuflen)
{
	char *pTemp1 = 0;
	char *pTemp2 = 0;
	unsigned int templen = 0;

	if ((str == 0) || (pSubStr1 == 0) || (pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	pTemp1 = strstr(str,pSubStr1);
	//未找到第一个字符串
	if (pTemp1 == 0) return 0;

	//如果第二个子字符串传入的是0 或者 为空值 那么返回结果就默认为取第一个字符串到结尾之间的数据
	if (pSubStr2 && strlen(pSubStr2))
	{
		pTemp2 = strstr(pTemp1 + strlen((char *)pSubStr1),pSubStr2);
	}
	else
	{
		pTemp2 = str + strlen(str);
	}
	if (pTemp2)
	{
		templen = pTemp2 - (pTemp1 + strlen((char *)pSubStr1));
		templen = (outbuflen > templen) ? templen : (outbuflen - 1);
		memset(pOutBuf,0,outbuflen);
		if (templen) memcpy(pOutBuf,pTemp1 + strlen((char *)pSubStr1),templen);

		return pTemp1;
	}
	return 0;
}



/**
函数功能:从pSrc缓冲区中 反方向查找pDes目标串
人口参数:pSrc 源缓冲区
		ulSrcLen 源缓冲区长度
		pDes 目标字符串
		ulDesLen 目标字符串长度
出口参数:无
返回值:查找成功 返回目标串头指针 查找失败 返回NULL
说明:
源缓冲区 aucSrc[] = "\x55\x32\x33\x00\x34\x33\x00\x34";
目标缓冲区 aucDes[] = "\x33\x00\x34";
	ReverseMemstr(aucSrc,8,"\x00\x34",2);
	执行结果返回aucSrc+6
*/
void *ReverseFindSubStr(void *pSrc,int ulSrcLen,void *pDes,int ulDesLen)
{
	INT32S i = 0;
	INT32S j = 0;
	INT8U *pSource = (INT8U *)pSrc;
	INT8U *pTarget = (INT8U *)pDes;

	if ((pSrc == 0) || (ulSrcLen == 0)
		|| (pDes == 0) || (ulDesLen == 0))
	{
		return 0;
	}

	for (i = (ulSrcLen - 1); i >= 0; i--)
	{
		//找到第一个字节
		if (pSource[i] == *pTarget)
		{
			for (j = 0; j < ulDesLen; j++)
			{
				if (
					((i + j) >= ulSrcLen)
					|| (pSource[i + j] != pTarget[j])
					)
				{
					break;
				}
			}
			if (j == ulDesLen)
			{
				return (pSource + i);
			}
		}
	}

	return 0;
}

void *_find_sub_str(void *pbuf,unsigned int ulBufLen,
							 void *SubStr,unsigned int ulSubStrLen)
{
	unsigned char *pSrc = (unsigned char *)pbuf;
	unsigned char *pSubStr = (unsigned char *)SubStr;
	unsigned int i = 0;
	unsigned int j = 0;

	if ((pbuf == 0) || (ulBufLen == 0) || (SubStr == 0) || (ulSubStrLen == 0))
	{
		return 0;
	}

	for (i = 0; (i + ulSubStrLen) <= ulBufLen; i++)
	{
		if (*(pSrc + i) == *pSubStr)	//找到第一个字节
		{
			for (j = 0; j < ulSubStrLen; j++)
			{
				if (*(pSubStr + j) != *(pSrc + i + j))
				{
					//return 0;
					break;
				}
			}
			if (j == ulSubStrLen)
			{
				return (pSrc + i);
			}
		}
	}
	return 0;
}


/**

  返回值：0 转换成功
		非0 转换失败
"89AF3300" 转换结果为 0x89af3300 保存在result中
*/
INT32U StrToHex(INT8U *str,INT8U len,INT32U *result)
{
	INT32U dat = 0;
	INT8U temp = 0;
	INT8U i = 0;
	INT8U *pStr = str;

	if (str == 0) return 1;
	if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
	{
		pStr = str + 2;
	}

	if (len == 0)	//如果等于零 表示传入的是'\0'结尾的字符串
	{
		len = strlen((char *)pStr);
	}
	for (i = 0; i < len; i++)
	{
		
		dat <<= 4;
		if ((pStr[i] >= '0') && (pStr[i] <= '9'))
		{
			temp = pStr[i] - '0';
		}
		else if ((pStr[i] >= 'a') && (pStr[i] <= 'f'))
		{
			temp = pStr[i] - 'a' + 10;
		}
		else if ((pStr[i] >= 'A') && (pStr[i] <= 'F'))
		{
			temp = pStr[i] - 'A' + 10;
		}
		else		//不合法的字符
		{
			return 2;
		}
		dat |= temp;
	}
	if (result) *result = dat;
	return 0;
}

int IsHexChar(INT8U ch)
{
	if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
	{
		return 1;
	}
	return 0;
}

/**
*str = "0203ed"
或者
*str = "02 03 ed";

pdat[0] = 0x02;pdat[1] = 0x03;pdat[2] = 0xed;

返回值：3

返回值小于或等于零 表示转换错误
*/
int HexStrToArray(INT8U *str,INT8U *pdat,INT32U pdatlen)
{
	INT8U aucTemp[3] = {0};
	INT8U ucTempLen = 0;
	INT32U len = 0;
	if (str == 0) return -1;

	while (1)
	{
		if (*str == 0)
		{
			break;
		}
		else if (!IsHexChar(*str))
		{
			
		}
		else
		{
			aucTemp[ucTempLen++] = *str;
			if (ucTempLen == 2)
			{
				INT32U dat = 0;

				if (StrToHex(aucTemp,0,&dat) == 0)
				{
					pdat[len++] = dat;
					if (pdatlen && (len >= pdatlen)) break;
				}
				ucTempLen = 0;
			}
		}
		str++;
	}
	return len;
}

/**
数组a[] = {0x11,0x22,0x33};
转换为"11 22 33" 存到buf中
*/
int ArrayToHexStr(void *pdat,INT32U ulDatLen,void *buf,INT32U ulBufSize)
{
	INT8U *pSrc = (INT8U *)pdat;
	INT8U *pDes = (INT8U *)buf;
	INT32U i = 0;
	INT32U len = 0;

	for (i = 0; i < ulDatLen; i++)
	{
		INT8U ucHigh4 = (*(pSrc + i)) / 16;
		INT8U ucLow4 = (*(pSrc + i)) % 16;

		*(pDes + len++) = (ucHigh4 >= 10) ? (ucHigh4 - 10 + 'A') : (ucHigh4 + '0');
		*(pDes + len++) = (ucLow4 >= 10) ? (ucLow4 - 10 + 'A') : (ucLow4 + '0');

		*(pDes + len++) = ' ';
		if ((len >= ulBufSize) && ulBufSize) break;
	}
	return len;
}

/**
数组a[] = {0x11,0x22,0x33};
转换为"112233" 存到buf中
*/
int ArrayToHexStr1(void *pdat,INT32U ulDatLen,void *buf,INT32U ulBufSize)
{
	INT8U *pSrc = (INT8U *)pdat;
	INT8U *pDes = (INT8U *)buf;
	INT32U i = 0;
	INT32U len = 0;

	for (i = 0; i < ulDatLen; i++)
	{
		INT8U ucHigh4 = (*(pSrc + i)) / 16;
		INT8U ucLow4 = (*(pSrc + i)) % 16;

		*(pDes + len++) = (ucHigh4 >= 10) ? (ucHigh4 - 10 + 'A') : (ucHigh4 + '0');
		*(pDes + len++) = (ucLow4 >= 10) ? (ucLow4 - 10 + 'A') : (ucLow4 + '0');

		//*(pDes + len++) = ' ';
		if ((len >= ulBufSize) && ulBufSize) break;
	}
	return len;
}


#define MAX_Q_CNT 8

typedef struct tagQueue
{
	void *ptr;		//队列缓冲区
	INT32U ulWrite;
	INT32U ulRead;

	INT32U ulOneUnitDataSize;	//一个数据单元的大小
	INT32U ulTotalCnt;			//总共有多少个数据单元
}Queue;

static Queue s_astQ[MAX_Q_CNT] = {0};


int FindFreeQueue(void)
{
	INT8U i = 0;

	for (i = 0; i < MAX_Q_CNT; i++)
	{
		if (s_astQ[i].ptr == 0)
		{
			return i;
		}
	}
	return -1;
}

/**
函数功能:入队 将buf指针所指的内容 拷贝到队列中
入口参数:
	Qnum:队列编号 所有队列编号不能重复
	ptr:队列缓冲区
	ulOneUnitDataSize:一个数据单元的大小
	cnt:数据单元的总个数
出口参数:
	无
返回值:返回队列编号 小于0 则创建队列失败
*/
int CreateQueue(int Qnum,void *ptr,INT32U ulOneUnitDataSize,INT32U cnt)
{
	int idx = 0;

	//检查参数
	if ((ptr == 0) || (ulOneUnitDataSize == 0) || (cnt == 0))
	{
		return -1;
	}

	if (Qnum < 0)
	{
		idx = FindFreeQueue();
		if (idx < 0)
		{
			return -2;
		}
	}
	else
	{
		idx = Qnum;
		if ((idx >= MAX_Q_CNT) || s_astQ[idx].ptr)
		{
			return -2;
		}
	}

	memset(s_astQ + idx,0,sizeof(s_astQ[idx]));
	s_astQ[idx].ptr = ptr;
	s_astQ[idx].ulOneUnitDataSize = ulOneUnitDataSize;
	s_astQ[idx].ulTotalCnt = cnt;

	return idx;
}

/**
函数功能:入队 将buf指针所指的内容 拷贝到队列中
入口参数:
	Qnum:队列编号
	buf:入队的缓冲区
出口参数:
	无
返回值:

*/
int QueueIn(INT8U Qnum,void *buf)
{
	INT32U idx = 0;

	//检查参数
	if ((buf == 0) || (Qnum >= MAX_Q_CNT))
	{
		return -1;
	}

	if ((s_astQ[Qnum].ptr == 0) || (s_astQ[Qnum].ulOneUnitDataSize == 0) || 
	(s_astQ[Qnum].ulTotalCnt == 0))
	{
		return -2;
	}

	//判断队列是否满了
	if (s_astQ[Qnum].ulWrite >= (s_astQ[Qnum].ulRead + s_astQ[Qnum].ulTotalCnt))
	{
		return -3;
	}

	idx = s_astQ[Qnum].ulWrite % s_astQ[Qnum].ulTotalCnt;
	//DBG_Print(LEVEL3,"Queue%d Msg In(idx:%d)\r\n",Qnum,idx);
	memcpy((INT8U *)(s_astQ[Qnum].ptr) + idx * s_astQ[Qnum].ulOneUnitDataSize,buf,s_astQ[Qnum].ulOneUnitDataSize);
	s_astQ[Qnum].ulWrite++;

	return 0;
}

//获取队列入队的元素个数
INT32U GetQueueElementCnt(INT8U Qnum)
{
	//检查参数
	if (Qnum >= MAX_Q_CNT)
	{
		return 0;
	}

	if (s_astQ[Qnum].ulWrite < s_astQ[Qnum].ulRead)
	{
		s_astQ[Qnum].ulWrite = s_astQ[Qnum].ulRead;
		return 0;
	}

	return (s_astQ[Qnum].ulWrite - s_astQ[Qnum].ulRead);
}

//出队
int QueueOut(INT8U Qnum,void *buf)
{
	INT32U idx = 0;

	//检查参数
	if (Qnum >= MAX_Q_CNT)
	{
		return -1;
	}

	if ((s_astQ[Qnum].ptr == 0) || (s_astQ[Qnum].ulOneUnitDataSize == 0) || 
	(s_astQ[Qnum].ulTotalCnt == 0))
	{
		return -2;
	}

	//判断队列是否为空
	if (GetQueueElementCnt(Qnum) == 0)
	{
		return -3;
	}

	idx = s_astQ[Qnum].ulRead % s_astQ[Qnum].ulTotalCnt;
	//DBG_Print(LEVEL3,"Queue%d Msg Out(idx:%d)\r\n",Qnum,idx);
	if (buf) memcpy(buf,(INT8U *)(s_astQ[Qnum].ptr) + idx * s_astQ[Qnum].ulOneUnitDataSize,s_astQ[Qnum].ulOneUnitDataSize);
	QueueMsgDealOk(Qnum);

	return 0;
}



//出队不自动加读指针
void *QueueGetCurMsg(INT8U Qnum)
{
	INT32U idx = 0;

	//检查参数
	if (Qnum >= MAX_Q_CNT)
	{
		return 0;
	}

	if ((s_astQ[Qnum].ptr == 0) || (s_astQ[Qnum].ulOneUnitDataSize == 0) || 
	(s_astQ[Qnum].ulTotalCnt == 0))
	{
		return 0;
	}

	//判断队列是否为空
	if (GetQueueElementCnt(Qnum) == 0)
	{
		return 0;
	}

	idx = s_astQ[Qnum].ulRead % s_astQ[Qnum].ulTotalCnt;

	return (INT8U *)(s_astQ[Qnum].ptr) + idx * s_astQ[Qnum].ulOneUnitDataSize;
}

//通知队列 出队结束
int QueueMsgDealOk(INT8U Qnum)
{
	if (s_astQ[Qnum].ulRead < s_astQ[Qnum].ulWrite)
	{
		//DBG_Print(LEVEL3,"Queue%d Msg(idx:%u) Deal Finish.\r\n",Qnum,s_astQ[Qnum].ulRead);
		s_astQ[Qnum].ulRead++;
	}
	return 0;
}




