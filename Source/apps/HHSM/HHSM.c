#include "HHSM.h"

/**************************************************************************
** 宏定义
**************************************************************************/
#define M_HHSM_FRAME_HEAD0			(0xAA)	//帧头1
#define M_HHSM_FRAME_HEAD1			(0x55)	//帧头2
#define M_HHSM_FRAME_HEAD_LEN		(2)		//帧头的长度
#define M_HHSM_MAX_DATA_LEN			(1024)	//最大1k数据
#define M_HHSM_SINGLE_DATA_LEN		(16)	//单段数据长度

#define M_HHSM_RECV_TRANSMIT_ERR	(0x3B)	//接收传输错误
#define M_HHSM_SEND_RE_HANDSHAKE	(0x3C)	//发送重新握手
#define M_HHSM_SEND_TANSMIT_FAIL	(0x3D)	//多次发送数据出错，发送传输失败

#define M_HHSM_RE_SEND_FAIL_CNT		(3)		//重发的最大次数
#define M_HHSM_RE_RECV_FAIL_CNT		(3)		//重接收的最大次数

#define M_HHSM_ACK_SUCC_H			(0x90)
#define M_HHSM_ACK_SUCC_L			(0x00)
/**************************************************************************
** 结构体定义
**************************************************************************/
/* 驱动接口结构体 */
typedef struct _hhsm_driver_func_st
{
	SPIInitFunc init;
	SPIWriteFunc write;
	SPIReadFunc read;
	CE_SetHigFunc enhigh;
	CE_SetLowFunc enlow;
	DelayMsFunc delayms;
	IsHHSMBusy _is_hhsm_busy;
}hhsm_driver_func_st;

/**************************************************************************
** 全局变量定义
**************************************************************************/
//驱动接口
static hhsm_driver_func_st s_hhsm_func;

//发送失败次数
static u8 s_hhsm_send_fail_cnt = 0;

/**************************************************************************
** 函数声明
**************************************************************************/

/**************************************************************************
* 函  数: void hhsm_wait_busy_to_free(void)
* 描  述: 等待HHSM芯片总线空闲
* 举  例: 
* 入  参: void
* 出  参: void
* 返回值: void
**************************************************************************/
static void hhsm_wait_busy_to_free(void)
{
	u8 cnt = 0;
	
	while (s_hhsm_func._is_hhsm_busy ? s_hhsm_func._is_hhsm_busy() : 1)
	{
		if (s_hhsm_func.delayms)
		{
			s_hhsm_func.delayms(10);
		}

		cnt++;
		if (cnt >= 6)
		{
			break;
		}
	}

	//DBG_Print(LEVEL_GPRS, "hhsm_wait_busy_to_free cnt:%d\r\n", cnt);
}

/**************************************************************************
* 函  数: int hhsm_send_cmd(u8 *data, u32 len)
* 描  述: 发送指令数据
* 举  例: 
* 入  参: u8 *data：发送数据信息
*		  u32 len:发送数据信息长度
* 出  参: void
* 返回值: int ， -1 : 失败， 0 发送成功
**************************************************************************/
static int hhsm_send_cmd(u8 *data, u32 len)
{
	int ret = -1;
	int index = 0;

	do
	{
		if ((0 == data) || (0 == len) || (len > get_big_mem_size()))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_send_cmd input data error\r\n");
			break;
		}

		if (s_hhsm_func.enlow)
		{
			s_hhsm_func.enlow();
		}

		if (s_hhsm_func.delayms)
		{
			s_hhsm_func.delayms(10);
		}

		if (s_hhsm_func.write)
		{
			s_hhsm_func.write(data, M_HHSM_FRAME_HEAD_LEN);
		}

		//DBG_PrintBuf(LEVEL0, "send Frame head:\r\n", data, M_HHSM_FRAME_HEAD_LEN);
		hhsm_wait_busy_to_free();

		index = 2;

		while(len > index)
		{
			if ((len - index) <= M_HHSM_SINGLE_DATA_LEN)
			{
				if (s_hhsm_func.write)
				{
					s_hhsm_func.write(&data[index], len - index);
					//DBG_PrintBuf(LEVEL0, "send Frame data:\r\n", &data[index], len - index);
				}
				hhsm_wait_busy_to_free();
				break;
			}

			if (s_hhsm_func.write)
			{
				s_hhsm_func.write(&data[index], M_HHSM_SINGLE_DATA_LEN);
				//DBG_PrintBuf(LEVEL0, "send Frame data[16]:\r\n", &data[index], M_HHSM_SINGLE_DATA_LEN);
				index += M_HHSM_SINGLE_DATA_LEN;
			}
			hhsm_wait_busy_to_free();
		}

		DBG_Print(LEVEL_GPRS, "hhsm_send_cmd ok\r\n");

		ret = 0;
	} while(0);

	return ret;
}

/**************************************************************************
* 函  数: int hhsm_recv_data(u8 *data, u32 len)
* 描  述: 接收到数据
* 举  例: 
* 入  参: u32 maxlen:允许读到数据的最大数据长度
* 出  参: u8 *data:读取到数据信息
* 返回值: int 
			-1 : 发送过程中出错，支持重传，最大支持3次重传
			-2 : 接收数据长度太大
			-3 : 其他错误
			 > 0 读取到数据长度
**************************************************************************/
static int hhsm_recv_data(u8 *data, u32 maxlen)
{
	int ret = -2;
	unsigned short recv_msg_len = 0;	//有效数据的长度
	unsigned short recv_len = 0;					//接收所有数据的长度
	u8 temp[M_HHSM_SINGLE_DATA_LEN] = {0};

	//crc错误重新接收的错误次数
	u8 crc = 0;
	static u8 crc_err_cnt = 0;

	do
	{
Recv:
		if (s_hhsm_func.read)
		{
			s_hhsm_func.read(M_HHSM_FRAME_HEAD_LEN, temp, sizeof(temp));
		}
		hhsm_wait_busy_to_free();

		//DBG_PrintBuf(LEVEL_GPRS, "Ack Frame Head:\r\n", temp,M_HHSM_FRAME_HEAD_LEN);

		if ((M_HHSM_FRAME_HEAD0 == temp[0]) && (M_HHSM_RECV_TRANSMIT_ERR == temp[1]))
		{
			//发送过程数据有误
			ret = -1;
			crc_err_cnt = 0;
			break;
		}
		else if ((M_HHSM_FRAME_HEAD0 == temp[0]) && (M_HHSM_SEND_RE_HANDSHAKE == temp[1]))
		{
			//发送过程数据有误
			ret = -1;
			crc_err_cnt = 0;
			break;
		}
		else if ((M_HHSM_FRAME_HEAD0 == temp[0]) && (M_HHSM_FRAME_HEAD1 == temp[1]))
		{
			//应答成功
			memset(temp, 0, sizeof(temp));
			if (s_hhsm_func.read)
			{
				s_hhsm_func.read(M_HHSM_SINGLE_DATA_LEN, temp, sizeof(temp));
			}
			hhsm_wait_busy_to_free();

			//DBG_PrintBuf(LEVEL_GPRS, "Ack Data:\r\n", temp, M_HHSM_SINGLE_DATA_LEN);
		
			recv_msg_len = (((unsigned short)temp[0]) << 8) | temp[1];
			recv_msg_len += 1; //+1 是还有一个校验码
			if (recv_msg_len > get_big_mem_size())
			{
				ret = -2;
				crc_err_cnt = 0;
				break;
			}

			//DBG_Print(LEVEL_GPRS, "recv msg len:%x\r\n", recv_msg_len);

			if (recv_msg_len <= (M_HHSM_SINGLE_DATA_LEN - M_HHSM_FRAME_HEAD_LEN))
			{
				memcpy(&data[recv_len], &temp[M_HHSM_FRAME_HEAD_LEN], recv_msg_len);
				recv_len += recv_msg_len;
			}
			else
			{
				memcpy(&data[recv_len], &temp[M_HHSM_FRAME_HEAD_LEN], M_HHSM_SINGLE_DATA_LEN - M_HHSM_FRAME_HEAD_LEN);
				recv_len += M_HHSM_SINGLE_DATA_LEN - M_HHSM_FRAME_HEAD_LEN;

				//循环读取接收的数据
				while (recv_msg_len > recv_len)
				{
					memset(temp, 0, sizeof(temp));
					//超过16个字节
					if ((recv_msg_len - recv_len) >= M_HHSM_SINGLE_DATA_LEN)
					{
						if (s_hhsm_func.read)
						{
							s_hhsm_func.read(M_HHSM_SINGLE_DATA_LEN, temp, sizeof(temp));
						}
						hhsm_wait_busy_to_free();

						memcpy(&data[recv_len], temp, M_HHSM_SINGLE_DATA_LEN);
						recv_len += M_HHSM_SINGLE_DATA_LEN;
					}
					else
					{
						if (s_hhsm_func.read)
						{
							s_hhsm_func.read((recv_msg_len - recv_len), temp, sizeof(temp));
						}
						hhsm_wait_busy_to_free();

						memcpy(&data[recv_len], temp, recv_msg_len - recv_len);
						recv_len = recv_msg_len;
						break;
					}
				}
			}

			//DBG_PrintBuf(LEVEL_GPRS, "recv hex data:\r\n", data, recv_len);

			//进行校验
			crc = CommSumCheckCode(data, (recv_len - 1));
			crc = ~crc;

			//DBG_Print(LEVEL_GPRS, "calc cr:%x, recv crc:%x\r\n", crc, data[recv_len - 1]);
			
			if (crc != data[recv_len - 1])
			{
				//传输过程中出错，需要重新启动接收流程
				if (s_hhsm_func.write)
				{
					memset(temp, 0, sizeof(temp));
					temp[0] = M_HHSM_FRAME_HEAD0;
					temp[1] = M_HHSM_SEND_RE_HANDSHAKE;
					s_hhsm_func.write(temp, M_HHSM_FRAME_HEAD_LEN);
				}
				hhsm_wait_busy_to_free();

				crc_err_cnt++;
				if (crc_err_cnt >= M_HHSM_RE_RECV_FAIL_CNT)
				{
					//超过3次接收都失败，就终止本次
					if (s_hhsm_func.write)
					{
						memset(temp, 0, sizeof(temp));
						temp[0] = M_HHSM_FRAME_HEAD0;
						temp[1] = M_HHSM_SEND_TANSMIT_FAIL;
						s_hhsm_func.write(temp, M_HHSM_FRAME_HEAD_LEN);
					}
					hhsm_wait_busy_to_free();

					ret = -3;
					break;
				}

				//需要重新接收
				memset(data, 0, maxlen);
				recv_len = 0;
				goto Recv;
			}

			//接收成功
			ret = recv_len - 1;
		}
	} while(0);

	crc_err_cnt = 0;

	return ret;
}

/**************************************************************************
* 函  数: int hhsm_send_cmd_data(u8 *data, u32 len, u8 *outdata, u32 maxlen)
* 描  述: 发送指令数据得到数据
* 举  例: 
* 入  参: u8 *data：发送数据信息
*		  u32 len:发送数据信息长度
*		  u32 maxlen:允许读到数据的最大数据长度
* 出  参: u8 *outdata:读取到数据信息
* 返回值: int ， -1 : 失败， > 0 读取到数据长度
**************************************************************************/
static int hhsm_send_cmd_data(u8 *data, u32 len, u8 *outdata, u32 maxlen)
{
	int ret = -1;
	u8 crc = 0;
	static u8 *send_cmd = 0;
	u32 send_len = 0;
	u8 temp[M_HHSM_FRAME_HEAD_LEN] = {0};

	send_cmd = (u8 *)get_big_mem();
	if (send_cmd == 0)
	{
		return -1;
	}

	do
	{
		if ((0 == data) || (0 == len) || (0 == outdata) || (len > M_HHSM_MAX_DATA_LEN)
			|| (0 == maxlen) || (maxlen > get_big_mem_size()))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_send_cmd_data input data error\r\n");
			break;
		}

Send:
		//同步字节
		send_cmd[send_len++] = M_HHSM_FRAME_HEAD0;
		send_cmd[send_len++] = M_HHSM_FRAME_HEAD1;

		//长度
		send_cmd[send_len++] = (u8)((len >> 8) & 0xff);
		send_cmd[send_len++] = (u8)(len & 0xff);

		//数据项
		memcpy(&send_cmd[send_len], data, len);
		send_len += len;

		//校验位
		crc = CommSumCheckCode(&send_cmd[4], send_len - 4);
		send_cmd[send_len++] = ~crc;

		//DBG_PrintBuf(LEVEL_GPRS, "hhsm_send_cmd_data:\r\n", send_cmd, send_len);

		//进行发送
		ret = hhsm_send_cmd(send_cmd, send_len);
		if (-1 == ret)
		{
			DBG_Print(LEVEL4, "hhsm_send_cmd_data send cmd fail\r\n");
			break;
		}
		
		//接收数据
		ret = hhsm_recv_data(outdata, maxlen);
		if (ret > 0)
		{
			//接收成功
			//DBG_PrintBuf(LEVEL_GPRS, "hhsm recv data:\r\n", outdata, ret);

			//发送接收成功的应答指令
			if (s_hhsm_func.write)
			{
				memset(temp, 0, sizeof(temp));
				temp[0] = M_HHSM_FRAME_HEAD0;
				temp[1] = M_HHSM_FRAME_HEAD0;
				s_hhsm_func.write(temp, M_HHSM_FRAME_HEAD_LEN);
			}
			hhsm_wait_busy_to_free();

			break;
		}
		else if (-1 == ret)
		{
			DBG_Print(LEVEL4, "hhsm_send_cmd_data need resend data\r\n");

			s_hhsm_send_fail_cnt++;
			if (s_hhsm_send_fail_cnt >= M_HHSM_RE_SEND_FAIL_CNT)
			{
				//超过3次就主动发送失败，不再重传
				if (s_hhsm_func.write)
				{
					memset(temp, 0, sizeof(temp));
					temp[0] = M_HHSM_FRAME_HEAD0;
					temp[1] = M_HHSM_SEND_TANSMIT_FAIL;
					s_hhsm_func.write(temp, M_HHSM_FRAME_HEAD_LEN);
				}
				hhsm_wait_busy_to_free();

				ret = -1;
				break;
			}

			send_len = 0;
			goto Send;
		}
		else
		{
			DBG_Print(LEVEL4, "hhsm_send_cmd_data recv data fail\r\n");
			ret = -1;
			break;
		}
	} while(0);

	free_big_mem(send_cmd);

	if (s_hhsm_func.enhigh)
	{
		s_hhsm_func.enhigh();
	}

	s_hhsm_send_fail_cnt = 0;

	return ret;
}

/**************************************************************************
* 函  数: void hhsm_select_app(void)
* 描  述: 选择应用
* 举  例: 
		//M->S 00a4 04 00 0E 6170706C65742E63746170703031
		//M<-S 9000
* 入  参: void
* 出  参: void
* 返回值: int ， -1 : 失败， 0 成功
**************************************************************************/
static int hhsm_select_app(void)
{
	int ret = -1;
	u8 temp[] ={
			0x00, 0xA4, 0x04, 0x00, 0x0E,
			0x61, 0x70, 0x70, 0x6C, 0x65,
			0x74, 0x2E, 0x63, 0x74, 0x61,
			0x70, 0x70, 0x30, 0x31};
	u8 ack[M_HHSM_SINGLE_DATA_LEN] = {0};

	ret = hhsm_send_cmd_data(temp, sizeof(temp), ack, sizeof(ack));
	if (-1 != ret)
	{
		if ((M_HHSM_ACK_SUCC_H == ack[0]) && (M_HHSM_ACK_SUCC_L == ack[1]))
		{
			DBG_Print(LEVEL4, "hhsm_select_app Ok.\r\n");
			return 0;
		}
	}

	return -1;
}

/**************************************************************************
* 函  数: void hhsm_open_app(void)
* 描  述: 打开应用
* 举  例: 
		//M->S 80110000021000
		//M<-S 9000
		
* 入  参: void
* 出  参: void
* 返回值: int ， -1 : 失败， 0 成功
**************************************************************************/
static int hhsm_open_app(void)
{
	int ret = -1;
	u8 temp[] ={0x80, 0x11, 0x00, 0x00, 0x02, 0x10, 0x00};
	u8 ack[M_HHSM_SINGLE_DATA_LEN] = {0};

	ret = hhsm_send_cmd_data(temp, sizeof(temp), ack, sizeof(ack));
	if (-1 != ret)
	{
		if ((M_HHSM_ACK_SUCC_H == ack[0]) && (M_HHSM_ACK_SUCC_L == ack[1]))
		{
			DBG_Print(LEVEL4, "hhsm_open_app Ok.\r\n");
			return 0;
		}
	}

	return -1;
}

/**************************************************************************
* 函  数: void hhsm_open_container(void)
* 描  述: 打开容器
* 举  例: 
		//M->S 80170000021000
		//M<-S 9000	
* 入  参: void
* 出  参: void
* 返回值: int ， -1 : 失败， 0 成功
**************************************************************************/
static int hhsm_open_container(void)
{
	int ret = -1;
	u8 temp[] ={0x80, 0x17, 0x00, 0x00, 0x02,0x10, 0x00};
	u8 ack[M_HHSM_SINGLE_DATA_LEN] = {0};

	ret = hhsm_send_cmd_data(temp, sizeof(temp), ack, sizeof(ack));
	if (-1 != ret)
	{
		if ((M_HHSM_ACK_SUCC_H == ack[0]) && (M_HHSM_ACK_SUCC_L == ack[1]))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_open_container Ok.\r\n");
			return 0;
		}
	}

	return -1;
}

/**************************************************************************
* 函  数: void hhsm_open_container(void)
* 描  述: 打开容器
* 举  例: 
		//M->S 80170000021000
		//M<-S 9000	
* 入  参: void
* 出  参: void
* 返回值: int ， -1 : 失败， 0 成功
**************************************************************************/
static int _hhsm_open_container(unsigned char idx)
{
	int ret = -1;
	u8 temp[] ={0x80, 0x17, 0x00, 0x00, 0x02,0x10, 0x00};
	u8 ack[M_HHSM_SINGLE_DATA_LEN] = {0};

	temp[6] = idx;
	ret = hhsm_send_cmd_data(temp, sizeof(temp), ack, sizeof(ack));
	if (-1 != ret)
	{
		if ((M_HHSM_ACK_SUCC_H == ack[0]) && (M_HHSM_ACK_SUCC_L == ack[1]))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_open_container Ok.\r\n");
			return 0;
		}
	}

	return -1;
}



/**************************************************************************
* 函  数: int hhsm_read_chip_id(u8 *data, int maxlen)
* 描  述: 读取芯片ID
* 举  例: 	
	M->S 8028 0000 0A
	M<-S 0010000000000000002B 9000
* 入  参: int maxlen:最大长度限制
* 出  参: u8 *data：读取芯片ID
* 返回值: int ， -1 : 失败， > 0 芯片ID的长度
**************************************************************************/
int hhsm_read_chip_id(u8 *data, int maxlen)
{
	int ret = -1;
	u8 temp[] ={0x80, 0x28, 0x00, 0x00, 0x0A};

	do
	{
		if ((0 == data) || (0 == maxlen))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_chip_id input fail.\r\n");
			break;
		}
	
		if (-1 == hhsm_select_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_chip_id hhsm_select_app fail.\r\n");
			break;
		}

		if (-1 == hhsm_open_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_chip_id hhsm_open_app fail.\r\n");
			break;
		}

		if (-1 == hhsm_open_container())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_chip_id hhsm_open_container fail.\r\n");
			break;
		}

		ret = hhsm_send_cmd_data(temp, sizeof(temp), data, maxlen);
		if (-1 != ret)
		{
			if ((M_HHSM_ACK_SUCC_H == data[ret - 2]) && (M_HHSM_ACK_SUCC_L == data[ret - 1]))
			{
				ret = ret - 2;
				break;
			}
		}

		ret = -1;
	} while(0);

	return ret;
}

//0表示成功 非零表示失败
int _hhsm_write_public_private_key(unsigned char idx,
	void *public_key,unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	int ret = -1;
	unsigned char data[16] = {0};
	//u8 temp[] ={0x80, 0x24, 0x01, 0x00, 0x00};
	unsigned int rlen = 0;
	unsigned char *temp = 0;


	temp = get_small_mem();
	if (temp == 0) return -1;

	do
	{
		if ((public_key == 0) || (pb_len == 0) 
		|| (private_key == 0) || (pv_len == 0))
		{
			break;
		}
		if (-1 == hhsm_select_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		if (-1 == hhsm_open_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		if (-1 == _hhsm_open_container(idx))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		//80328000 63 1001 00 8ee63df7e84f195f6a3ae62c90db62b6aa30e09a52d7e9b05e9c23085e6ddef8059b11174b959b81c85a1e010e189a836d81342d6873566d3f1ecd7b06b423f0 58e0d61e89b69cf477199131d3e6c37bae5990fd865cca48f44382e6628a89c1
		//准备数据
		memcpy(temp,"\x80\x32\x80\x00",4);
		rlen += 4;
		temp[rlen++] = 3 + pv_len + pb_len;
		
		temp[rlen++] = 0x10;
		temp[rlen++] = idx;
		temp[rlen++] = 0x00;

		memcpy(temp+rlen,public_key,pb_len);
		rlen += pb_len;
		memcpy(temp+rlen,private_key,pv_len);
		rlen += pv_len;

		ret = hhsm_send_cmd_data(temp, rlen, data, sizeof(data));
		if (-1 != ret)
		{
			if ((M_HHSM_ACK_SUCC_L == data[1]) && (M_HHSM_ACK_SUCC_H == data[0]))
			{
				ret = 0;
				break;
			}
		}

		ret = -1;
	} while(0);

	free_small_mem(temp);
	return ret;
}


//公私钥对有A B C三组 写入公私钥对A
int hhsm_write_public_private_key(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hhsm_write_public_private_key(0,public_key,pb_len,private_key,pv_len);
}

//写入公私钥对B
int hhsm_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hhsm_write_public_private_key(1,public_key,pb_len,private_key,pv_len);
}

//写入公私钥对C
int hhsm_write_public_private_keyC(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hhsm_write_public_private_key(2,public_key,pb_len,private_key,pv_len);
}



/**************************************************************************
* 函  数: int hhsm_read_public_key(u8 *data, int maxlen)
* 描  述: 读取公钥
* 举  例: 	
	M->S 8024 01 00 00
	M<-S 00EA571DC72EABEBEF7953505C9CBB11875E68C16CF1ACA2BB087C
		 A91A00F4A52989F4D6351BC4A287CBE8431A119CE2B13013EF76E1ACB28B356E9C801B6AB47D 9000
	
* 入  参: int maxlen：最大长度限制
* 出  参: u8 *data:读取的公钥
* 返回值: int ， -1 : 失败， > 0 公钥的长度
**************************************************************************/
int _hhsm_read_public_key(unsigned char idx,u8 *data, int maxlen)
{
	int ret = -1;
	u8 temp[] ={0x80, 0x24, 0x01, 0x00, 0x00};

	do
	{
		if ((0 == data) || (0 == maxlen))
		{
			break;
		}
	
		if (-1 == hhsm_select_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		if (-1 == hhsm_open_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		if (-1 == _hhsm_open_container(idx))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_read_public_key input fail.\r\n");
			break;
		}

		ret = hhsm_send_cmd_data(temp, sizeof(temp), data, maxlen);
		if (-1 != ret)
		{
			if ((M_HHSM_ACK_SUCC_L == data[0]) && (M_HHSM_ACK_SUCC_H == data[ret - 2])
				&& (M_HHSM_ACK_SUCC_L == data[ret - 1]))
			{
				int i = 0;

				for (i = 0; i < (ret - 3); i++)
				{
					data[i] = data[i + 1];
				}

				ret = ret - 3;
				break;
			}
		}

		ret = -1;
	} while(0);

	return ret;
}


/**************************************************************************
* 函  数: int hhsm_read_public_key(u8 *data, int maxlen)
* 描  述: 读取公钥
* 举  例: 	
	M->S 8024 01 00 00
	M<-S 00EA571DC72EABEBEF7953505C9CBB11875E68C16CF1ACA2BB087C
		 A91A00F4A52989F4D6351BC4A287CBE8431A119CE2B13013EF76E1ACB28B356E9C801B6AB47D 9000
	
* 入  参: int maxlen：最大长度限制
* 出  参: u8 *data:读取的公钥
* 返回值: int ， -1 : 失败， > 0 公钥的长度
**************************************************************************/
int hhsm_read_public_key(u8 *data, int maxlen)
{
	return _hhsm_read_public_key(0,data,maxlen);
}


/**************************************************************************
* 函  数: int hhsm_read_public_keyB(u8 *data, int maxlen)
* 描  述: 读取公钥B
* 举  例: 	
	M->S 8024 01 00 00
	M<-S 00EA571DC72EABEBEF7953505C9CBB11875E68C16CF1ACA2BB087C
		 A91A00F4A52989F4D6351BC4A287CBE8431A119CE2B13013EF76E1ACB28B356E9C801B6AB47D 9000
	
* 入  参: int maxlen：最大长度限制
* 出  参: u8 *data:读取的公钥
* 返回值: int ， -1 : 失败， > 0 公钥的长度
**************************************************************************/
int hhsm_read_public_keyB(u8 *data, int maxlen)
{
	return _hhsm_read_public_key(1,data,maxlen);
}


/**************************************************************************
* 函  数: int hhsm_read_public_keyC(u8 *data, int maxlen)
* 描  述: 读取公钥C
* 举  例: 	
	M->S 8024 01 00 00
	M<-S 00EA571DC72EABEBEF7953505C9CBB11875E68C16CF1ACA2BB087C
		 A91A00F4A52989F4D6351BC4A287CBE8431A119CE2B13013EF76E1ACB28B356E9C801B6AB47D 9000
	
* 入  参: int maxlen：最大长度限制
* 出  参: u8 *data:读取的公钥
* 返回值: int ， -1 : 失败， > 0 公钥的长度
**************************************************************************/
int hhsm_read_public_keyC(u8 *data, int maxlen)
{
	return _hhsm_read_public_key(2,data,maxlen);
}


/**************************************************************************
* 函  数: int hhsm_send_singal_data(u8 *data, u32 len, u8 last_flag, u8 *outdata)
* 描  述: 发送单包数据
* 举  例: 	
	
* 入  参: u8 *data:一包数据
*	      u8 len:一包长度
*		  u8 last_flag：最后一包
* 出  参: u8 *outdata：如果是最后一包，就需要输出结果数据
* 返回值: int ， -1 : 失败，>= 0 成功
**************************************************************************/
static int hhsm_send_singal_data(u8 *data, u32 len, u8 last_flag, u8 *outdata)
{
	int ret = -1;
	static u8 ack[80] = {0};

	//发送数据
	ret = hhsm_send_cmd_data(data, len, ack, sizeof(ack));
	if (-1 != ret)
	{
		if ((M_HHSM_ACK_SUCC_H == ack[ret - 2]) && (M_HHSM_ACK_SUCC_L == ack[ret - 1]))
		{
			if (1 == last_flag)
			{
				memcpy(outdata, ack, ret - 2);
				return ret - 2;
			}
			else
			{
				return 0;
			}
		}
	}

	return -1;
}

/**************************************************************************
* 函  数: int hhsm_read_calc_hash(u8 *data, u32 len, u8 *outdata, int maxlen)
* 描  述: 计算哈希值
* 举  例: 	
	
* 入  参: u8 *data, u32 len, int maxlen
* 出  参: u8 *outdata：
* 返回值: int ， -1 : 失败， > 0 哈希值的长度
**************************************************************************/
int hhsm_read_calc_hash(u8 *data, u32 len, u8 *outdata, int maxlen)
{
	int ret = -1;
	u8 *temp = 0;
	int templen = 0;
	int msglen = 0;
	u8 index = 0;

	do
	{
		temp = (u8 *)get_big_mem();
		if (!temp)
		{
			break;
		}		

		if (len <= 0x7f)
		{
			temp[templen++] = 0x80;
			temp[templen++] = 0x31;

			temp[templen++] = 0x80;	//最后一块
			temp[templen++] = 0x00;
			temp[templen++] = (u8)(len & 0x7f);

			memcpy(&temp[templen], data, len);
			templen += (u8)(len & 0x7f);

			ret = hhsm_send_singal_data(temp, templen, 1, outdata);
			if (ret > 0)
			{
				break;
			}
		}
		else
		{
			//分段发送
			while (len > msglen)
			{
				memset(temp, 0, get_big_mem_size());
				templen = 0;
				temp[templen++] = 0x80;
				temp[templen++] = 0x31;

				if ((len - msglen) <= 0x7f)
				{
					temp[templen++] = 0x80;	//最后一块
					temp[templen++] = index++;
					temp[templen++] = (u8)((len - msglen) & 0x7f);

					memcpy(&temp[templen], &data[msglen], len - msglen);
					templen += (u8)((len - msglen) & 0x7f);

					//发送
					ret = hhsm_send_singal_data(temp, templen, 1, outdata);
					if (ret > 0)
					{
						DBG_Print(LEVEL_GPRS, "hhsm_send_singal_data, 1, succ\r\n");
						msglen = len;
						break;
					}
					else
					{
						DBG_Print(LEVEL_GPRS, "hhsm_send_singal_data, 1, Fail\r\n");
						ret = -1;
						break;
					}
				}
				else
				{
					temp[templen++] = 0x00;//分段
					temp[templen++] = index++;
					temp[templen++] = 0x7f;

					memcpy(&temp[templen], &data[msglen], 0x7f);
					templen += 0x7f;

					//发送
					ret = hhsm_send_singal_data(temp, templen, 0, 0);
					if (0 == ret)
					{
						DBG_Print(LEVEL_GPRS, "hhsm_send_singal_data, 0, succ\r\n");
						msglen += 0x7f;
					}
					else
					{
						DBG_Print(LEVEL_GPRS, "hhsm_send_singal_data, 0, Fail\r\n");
						ret = -1;
						break;
					}
				}
			}
		}
	} while(0);

	if (temp)
	{
		free_big_mem(temp);
		temp = 0;
	}

	return ret;
}

/**************************************************************************
* 函  数: int hhsm_read_sign_data(u8 *data, u32 len, u8 *outdata, int maxlen)
* 描  述: 读取签名数据
* 举  例: 	
	M->S 8024 01 00 00
	M<-S 00EA571DC72EABEBEF7953505C9CBB11875E68C16CF1ACA2BB087C
		 A91A00F4A52989F4D6351BC4A287CBE8431A119CE2B13013EF76E1ACB28B356E9C801B6AB47D 9000
	
* 入  参: u8 data:输入需要前面的数据
*		  u32 len:长度
*		  int maxlen:得到签名数据的最大允许长度
* 出  参: u8 *outdata：得到签名数据
* 返回值: int ， -1 : 失败， > 0 芯片ID的长度
**************************************************************************/
int hhsm_read_sign_data(u8 *data, u32 len, u8 *outdata, int maxlen)
{
	int ret = -1;
	//static u8 hash_val[33] = {0};
	u8 *hash_val = 0;
	u8 *temp = 0;
	u32 templen = 0;

	do
	{
		if ((0 == data) || (0 == maxlen) || (0 == outdata) || (0 == maxlen))
		{
			break;
		}
	
		if (-1 == hhsm_select_app())
		{
			break;
		}

		if (-1 == hhsm_open_app())
		{
			break;
		}

		if (-1 == hhsm_open_container())
		{
			break;
		}

		hash_val = (u8 *)get_micro_mem();
		if (!hash_val)
		{
			DBG_Print(LEVEL_GPRS, "hhsm_send_cmd_data get_micro_mem fail .\r\n");
			break;
		}

		//先计算的都哈希值
		ret = hhsm_read_calc_hash(data, len, hash_val, sizeof(hash_val));
		if (-1 == ret)
		{
			DBG_Print(LEVEL_GPRS, "hhsm_send_cmd_data hhsm_read_calc_hash fail.\r\n");
			break;
		}

		temp = (u8 *)get_small_mem();
		if (!temp)
		{
			break;
		}

		DBG_PrintBuf(LEVEL_GPRS, "哈希值:\r\n", hash_val, ret);

		//根据哈希值获取签名值
		temp[templen++] = 0x80;
		temp[templen++] = 0x25;
		temp[templen++] = 0x00;
		temp[templen++] = 0x00;
		temp[templen++] = (u8)((ret + 2) & 0xff);

		//容器ID
		temp[templen++] = 0x10;
		temp[templen++] = 0x00;

		//哈希值
		memcpy(&temp[templen], hash_val, (ret & 0xff));
		templen += (ret & 0xff);
		
		ret = hhsm_send_cmd_data(temp, templen, outdata, maxlen);
		if (-1 != ret)
		{
			if ((M_HHSM_ACK_SUCC_H == outdata[ret - 2]) && (M_HHSM_ACK_SUCC_L == outdata[ret - 1]))
			{
				ret = ret - 2;
				break;
			}
		}

		ret = -1;
	} while(0);

	if (hash_val)
	{
		free_micro_mem(hash_val);
		hash_val = 0;
	}

	if (temp)
	{
		free_small_mem(temp);
		temp = 0;
	}

	return ret;
}

/* 计算起那么信息的ZA值 */
const u8 g_protocol_za_val[] =
{
 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFC, 0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34,
 0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7, 0xF3, 0x97,
 0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92, 0xDD, 0xBC, 0xBD, 0x41,
 0x4D, 0x94, 0x0E, 0x93, 0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19,
 0x81, 0x19, 0x5F, 0x99, 0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94,
 0x8F, 0xE3, 0x0B, 0xBF, 0xF2, 0x66, 0x0B, 0xE1, 0x71, 0x5A,
 0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7, 0xBC, 0x37, 0x36, 0xA2,
 0xF4, 0xF6, 0x77, 0x9C, 0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69,
 0x21, 0x53, 0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40,
 0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0
};



static int _ProtocolCalcZaHashVal(void *chip_id,unsigned int chip_id_len,
									void *public_key,unsigned int public_key_len,
									u8 *za_hash, u32 maxlen)
{
	int ret = -1;
	u8 *temp = 0;
	int templen = 0;

	do
	{
		temp = get_small_mem();
		if (!temp)
		{
			break;
		}

		//00 80 + ChipID + ZA_val + PubKey
		temp[templen++] = 0x00;
		temp[templen++] = 0x80;
		memcpy(&temp[templen], chip_id,chip_id_len);
		templen += chip_id_len;

		memcpy(&temp[templen], g_protocol_za_val, sizeof(g_protocol_za_val));
		templen += sizeof(g_protocol_za_val);

		memcpy(&temp[templen],public_key, public_key_len);
		templen += public_key_len;

		DBG_PrintBuf(LEVEL1, "固定值+公钥\r\n", temp, templen);
		ret = hhsm_read_calc_hash(temp, templen, za_hash, maxlen);
		if (-1 == ret)
		{
			DBG_Print(LEVEL0, "ProtocolFilingData hhsm_read_calc_hash fail.\r\n");
			break;
		}

		DBG_PrintBuf(LEVEL1, "ZA(哈希值)\r\n", za_hash, ret);
	} while(0);

	free_small_mem(temp);

	return ret;
}



//带Z值的签名 返回值:小于0表示失败 其中-66 表示传入的tempbuff缓冲区太小
//返回值大于零 表示 输出签名的长度
int hhsm_read_sign_data_with_z(void *chip_id,unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *dat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen)
{
	int templen = 0;
	unsigned char *pTempBuff = 0;

	if ((pOutSign == 0) || (outbuflen ==0)
	|| (chip_id == 0) || (chip_id_len == 0) || (public_key == 0)
	|| (public_key_len == 0) || (dat == 0) || (datlen == 0)
	)
	{
		return -1;
	}
	pTempBuff = get_big_mem();

	if (pTempBuff == 0)
	{
		return -2;
	}
	templen = _ProtocolCalcZaHashVal(chip_id,chip_id_len,public_key,
									public_key_len,pTempBuff,
									get_big_mem_size());

	if (templen > 0)
	{
		memcpy(pTempBuff + templen,dat,datlen);
	}
	else
	{
		memcpy(pTempBuff,dat,datlen);
		templen = 0;
	}
	templen = hhsm_read_sign_data(pTempBuff,templen + datlen,pOutSign,outbuflen);
	free_big_mem(pTempBuff);
	return templen;
}


/**************************************************************************
* 函  数: int hhsm_private_key_decrypt_encrypt(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen)
* 描  述: 输入密文数据，用私密进行解密,加密
* 举  例: 	
* 入  参: u8 type: 数据类型， 0 - 私钥解密，1 - 公钥加密
*		  u8 data:密文数据
*		  u32 len:密文数据长度
*		  int maxlen:最大明文数据长度
* 出  参: u8 *outdata：输出明文数据
* 返回值: int ， -1 : 失败， > 0 输出明文数据长度
**************************************************************************/
int _hhsm_private_key_decrypt_encrypt(unsigned char idx,u8 type, u8 *data, u32 len, u8 *outdata, int maxlen)
{
	int ret = -1;
	static u8 temp[270] = {0};
	int templen = 0;
	u8 ack[16] = {0};
	u8 index = 0;			//段序号
	int msg_len = 0;
	u8 flag = 0;
	static u8 recv_data[512] = {0};

	do
	{
		if ((0 == data) || (0 == len) || (len > M_HHSM_MAX_DATA_LEN)
			|| (0 == outdata) || (0 == maxlen) || (maxlen > get_big_mem_size()))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt input error.\r\n");
			break;
		}
		
		if (-1 == hhsm_select_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt input fail.\r\n");
			break;
		}

		if (-1 == hhsm_open_app())
		{
			DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt input fail.\r\n");
			break;
		}

		if (-1 == _hhsm_open_container(idx))
		{
			DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt input fail.\r\n");
			break;
		}

		if (len <= (0xff - 5))
		{
			temp[templen++] = 0x80;
			temp[templen++] = (0 == type) ? (0x26) : ((1 == type) ? (0x36) : (0x32));

			temp[templen++] = 0x82;
			temp[templen++] = 0x00;
			temp[templen++] = (u8)((len + 5) & 0xff);

			//容器号
			temp[templen++] = 0x10;
			temp[templen++] = idx;

			//AlgFlag
			temp[templen++] = 0x00;

			//密文长度
			temp[templen++] = (u8)((len >> 8) & 0xff);
			temp[templen++] = (u8)(len & 0xff);

			//密文信息
			memcpy(&temp[templen], data, len);
			templen += len;

			//发送数据
			ret = hhsm_send_cmd_data(temp, templen, ack, sizeof(ack));
			if (-1 == ret)
			{
				DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt hhsm_send_cmd_data fail.\r\n");
				break;
			}

			if (!((M_HHSM_ACK_SUCC_H == ack[ret - 2]) && (M_HHSM_ACK_SUCC_L == ack[ret - 1])))
			{
				DBG_Print(LEVEL_GPRS, "hhsm_private_key_decrypt_encrypt ack data[%x][%x].\r\n",
					ack[ret - 2], ack[ret - 1]);
				break;
			}

			//取报文
			templen = 0;
			memset(temp, 0, sizeof(temp));
			temp[templen++] = 0x80;
			temp[templen++] = (0 == type) ? (0x26) : (0x36);

			temp[templen++] = 0xC2;
			temp[templen++] = 0x00;
			temp[templen++] = 0x00;

			memset(recv_data, 0, sizeof(recv_data));
			ret = hhsm_send_cmd_data(temp, templen, recv_data, sizeof(recv_data));
			if (ret > 0)
			{
				if ((M_HHSM_ACK_SUCC_H == recv_data[ret - 2]) && (M_HHSM_ACK_SUCC_L == recv_data[ret - 1]))
				{
					ret = (int)(((unsigned short)(recv_data[0]) << 8) | recv_data[1]);
					memcpy(outdata, &recv_data[2], ret);
					break;
				}
			}
			ret = -1;
			break;
		}

		//长度大于0xff - 5个字节，需要分段发送
		while (len > msg_len)
		{
			templen = 0;
			memset(temp, 0, sizeof(temp));

			if ((0 == index) || ((index >= 1) && ((len - msg_len) >= 0xff)))
			{
				//前面的数据，分段处理
				temp[templen++] = 0x80;
				temp[templen++] = (0 == type) ? (0x26) : (0x36);

				temp[templen++] = 0x02;
				temp[templen++] = index;
				temp[templen++] = 0xff;

				if (0 == index)
				{
					//容器号
					temp[templen++] = 0x10;
					temp[templen++] = idx;

					//AlgFlag
					temp[templen++] = 0x00;

					//密文长度
					temp[templen++] = (u8)((len >> 8) & 0xff);
					temp[templen++] = (u8)(len & 0xff);

					//密文信息
					memcpy(&temp[templen], &data[msg_len], (0xff - 5));
					templen += (0xff - 5);
				}
				else
				{
					//密文信息
					memcpy(&temp[templen], &data[msg_len], 0xff);
					templen += 0xff;
				}

				//发送数据
				ret = hhsm_send_cmd_data(temp, templen, ack, sizeof(ack));
				if (-1 != ret)
				{
					if ((M_HHSM_ACK_SUCC_H == ack[ret - 2]) && (M_HHSM_ACK_SUCC_L == ack[ret - 1]))
					{
						if (0 == index)
						{
							DBG_Print(LEVEL_GPRS, "send first package[%d], msg_len:%d, len:%d, ok\r\n", index, msg_len, len);
							msg_len += (0xff - 5);
						}
						else if ((index >= 1) && ((len - msg_len) >= 0xff))
						{
							DBG_Print(LEVEL_GPRS, "send package[%d], msg_len:%d, len:%d, ok\r\n", index, msg_len, len);
							msg_len += 0xff;
						}
						index++;
						continue;
					}
				}

				ret = -1;
				break;
			}
			else if ((len - msg_len) <= 0xff)
			{
				//最后一包数据
				temp[templen++] = 0x80;
				temp[templen++] = (0 == type) ? (0x26) : (0x36);

				temp[templen++] = 0x82;
				temp[templen++] = index;
				temp[templen++] = (u8)((len - msg_len) & 0xff);

				//密文信息
				memcpy(&temp[templen], &data[msg_len], (len - msg_len));
				templen += (len - msg_len);

				//发送数据
				ret = hhsm_send_cmd_data(temp, templen, ack, sizeof(ack));
				if (-1 != ret)
				{
					if ((M_HHSM_ACK_SUCC_H == ack[ret - 2]) && (M_HHSM_ACK_SUCC_L == ack[ret - 1]))
					{
						DBG_Print(LEVEL_GPRS, "send last package[%d], msg_len:%d, len:%d, ok\r\n", index, msg_len, len);
						msg_len = len;
						index++;
						flag = 1;
						break;
					}
				}

				ret = -1;
				break;
			}
		}

		if (flag)
		{
			u8 i = 0;
			int recv_len = 0;
			u16 recv_msg_len = 0;

			//取密文/明文数据
			for (i = 0; i < index; i++)
			{
				templen = 0;
				memset(temp, 0, sizeof(temp));

				temp[templen++] = 0x80;
				temp[templen++] = (0 == type) ? (0x26) : (0x36);

				temp[templen++] = (0 == type) ? (0xC2) : (0x42);

				temp[templen++] = i;
				temp[templen++] = 0x00;
				//temp[templen++] = 0x00;

				memset(recv_data, 0, sizeof(recv_data));

				//发送读取报文指令
				ret = hhsm_send_cmd_data(temp, templen, recv_data, sizeof(recv_data));
				if (-1 != ret)
				{
					if ((M_HHSM_ACK_SUCC_H == recv_data[ret - 2]) && (M_HHSM_ACK_SUCC_L == recv_data[ret - 1]))
					{
						if (0 == i)
						{
							//第一次就把所有数据的长度传出来
							recv_msg_len = (u16)(((u16)recv_data[0]) << 8 | recv_data[1]);
							if (recv_msg_len > maxlen)
							{
								ret = -1;
								DBG_Print(LEVEL_GPRS, "read indexx[%d], recv_msg_len:%d > maxlen\r\n",
									    index, recv_msg_len, maxlen);
								break;
							}

							memcpy(&outdata[recv_len], &recv_data[2], ret - 4);
							recv_len += ret - 4;
						}
						else
						{
							//后面只传数据
							memcpy(&outdata[recv_len], &recv_data[0], ret - 2);
							recv_len += ret - 2;
						}
					}
				}

				if (i == (index - 1))
				{
					DBG_Print(LEVEL_GPRS, "read indexx[%d], recv_msg_len:%d, recv_len\r\n",
									    		index, recv_msg_len, recv_len);
					if (recv_msg_len == recv_len)
					{
						ret = recv_len;
					}
					else
					{
						ret = -1;
					}
					break;
				}
			}
		}
	} while(0);

	return ret;
}

//使用A组公私钥对加解密
int hhsm_private_key_decrypt_encrypt(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen)
{
	return _hhsm_private_key_decrypt_encrypt(0,type,data,len,outdata,maxlen);
}

//使用B组公私钥对加解密
int hhsm_private_key_decrypt_encryptB(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen)
{
	return _hhsm_private_key_decrypt_encrypt(1,type,data,len,outdata,maxlen);
}

//使用C组公私钥对加解密
int hhsm_private_key_decrypt_encryptC(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen)
{
	return _hhsm_private_key_decrypt_encrypt(2,type,data,len,outdata,maxlen);
}



//测试接口
int hhsm_test(void)
{
	int ret = -1;
	u8 *data = 0;
	int ret1 = -1;
	u8 count = 0;
	char key_ok_flag = 0;
	char chipid_ok_flag = 1;

	data = (u8 *)get_small_mem();
	if (data == 0)
	{
		DBG_Print(LEVEL0, "hhsm_test data get_big_mem fail.\r\n");
		return -1;
	}

	do
	{
		memset(data, 0, get_small_mem_size());
		ret = hhsm_read_public_key(data, get_small_mem_size());
		if (ret > 0)
		{
			DBG_PrintBuf(LEVEL0, "公钥Key:\r\n", data, ret);
			key_ok_flag = 1;
		}

		memset(data, 0, get_small_mem_size());
		ret = hhsm_read_chip_id(data, get_small_mem_size());
		if (ret > 0)		//读到了芯片ID 证明是旧芯片 芯片错误 立即返回错误
		{
			DBG_PrintBuf(LEVEL0, "芯片ID:\r\n", data, ret);
			chipid_ok_flag = 0;
			ret1 = -10;
			break;
		}
		else			//读不到芯片ID 反而是正确的
		{
			//chipid_ok_flag = 1;
		}

		//芯片ID和PubKey都OK标志 才正确返回
		if (chipid_ok_flag && key_ok_flag)
		{
			ret1 = 0;
			break;
		}
		count++;			
	}while(count < 3);

	free_small_mem(data);

	return ret1;
}

/**************************************************************************
* 函  数: void hhsm_init
* 描  述: 初始化加密芯片的驱动接口
* 举  例: 
* 入  参: SPIInitFunc pSPIInit:初始化
	      SPIWriteFunc pSPIWrite:写
		  SPIReadFunc pSPIRead:读
		  CE_SetHigFunc pSetCeHigh:使能拉高
		  CE_SetLowFunc pSetCeLow:使能拉低
		  DelayMsFunc pDelayms:延时
* 出  参: void
* 返回值: void
**************************************************************************/
void hhsm_init(SPIInitFunc pSPIInit,
					  SPIWriteFunc pSPIWrite,
					  SPIReadFunc pSPIRead,
					  CE_SetHigFunc pSetCeHigh,
					  CE_SetLowFunc pSetCeLow,
					  DelayMsFunc pDelayms,
					  IsHHSMBusy _is_hhsm_busy)
{
	memset(&s_hhsm_func, 0, sizeof(s_hhsm_func));
	s_hhsm_func.init = pSPIInit;
	s_hhsm_func.write = pSPIWrite;
	s_hhsm_func.read = pSPIRead;	
	s_hhsm_func.enhigh = pSetCeHigh;
	s_hhsm_func.enlow = pSetCeLow;	
	s_hhsm_func.delayms = pDelayms;
	s_hhsm_func._is_hhsm_busy = _is_hhsm_busy;

	if (s_hhsm_func.init)
	{
		s_hhsm_func.init();
	}
}


