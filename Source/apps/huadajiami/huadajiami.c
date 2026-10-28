#include "huadajiami.h"

//私钥KID = 公钥KID + 1
//公钥KID:0x01 0x03 0x05 0x07 0x09 0x0B 0x0D 0x0F ... 0x1F
//私钥KID:0x02 0x04 0x06 0x08 0x0A 0x0C 0x0E 0x10 ... 0x20

#define PIB_ACTIVE_FRAME 		0x03		//激活帧(包括RESET RATR)
#define PIB_INFO_FRAME 			0x0E		//信息帧
#define PIB_PROCESS_FRAME 		0x09		//过程帧


#define DEFAULT_HDSE_MAX_ONE_PACK_LEN	16	//RATR 帧正确处理完成前，即 FSM/FSS 没有协商成功前，FSM/FSS 默认为 16



SM2EncryVer encry_ver = SM2_VER2;

typedef struct tagHDSE
{
	HDSE_SETGPIO0 pSetGpio0;
	HDSE_DELAYMS pDelayms;
	HDSE_SPI_WRITE pWrite;
	HDSE_SPI_READ pRead;
	unsigned int one_pack_max_len;
}HDSE;

HDSE stHDSE = {0};


SM2EncryVer get_sm2_ver(void)
{
	return encry_ver;
}

void set_sm2_ver(SM2EncryVer ver)
{
	encry_ver = ver;
}

void _hdse_set_gpio0(unsigned char val)
{
	if (stHDSE.pSetGpio0) stHDSE.pSetGpio0(val);
}


void _hdse_delayms(unsigned int dly)
{
	if (stHDSE.pDelayms) stHDSE.pDelayms(dly);
}

void _hdse_spi_write(void *pdat,unsigned int len)
{
	if (stHDSE.pWrite)
	{
		stHDSE.pWrite(pdat,len);
		DBG_PrintBuf(LEVEL4,"mcu send:",pdat,len);
	}
}

void _hdse_spi_read(unsigned int iWantToReadBytes,void *pOutBuf,
							unsigned int outbuflen)
{
	if (stHDSE.pRead)
	{
		stHDSE.pRead(iWantToReadBytes,pOutBuf,outbuflen);
		DBG_PrintBuf(LEVEL4,"mcu read:",pOutBuf,iWantToReadBytes);
	}
}

void _hdse_delayus(unsigned int t)
{
	volatile unsigned int i = 0;
	volatile unsigned int j = 0;

	for (i = 0; i < t; i++)
	{
		for (j = 0; j < 20; j++);
	}
}


void hdse_init(	HDSE_SETGPIO0 pSetGpio0,
					HDSE_DELAYMS pDelayms,
					HDSE_SPI_WRITE pWrite,
					HDSE_SPI_READ pRead)
{
	stHDSE.pSetGpio0 = pSetGpio0;
	stHDSE.pDelayms = pDelayms;
	stHDSE.pWrite = pWrite;
	stHDSE.pRead = pRead;
	stHDSE.one_pack_max_len = DEFAULT_HDSE_MAX_ONE_PACK_LEN;
}


/**
1.上位机 MCU 连接 GPIO0，产生下降沿并延时 至少200us 时间
2.上位机在 GPIO0 输出高电平，产生上升沿并延时 至少500us 时间
3.复位成功，上位机在 GPIO0 输出低电平
4.复位时间内对 SPI 接口的 SS 信号无要求
*/
void hdse_reset(void)
{
	_hdse_set_gpio0(1);
	_hdse_delayms(5);

	_hdse_set_gpio0(0);
	_hdse_delayms(5);
	_hdse_set_gpio0(1);
	_hdse_delayms(5);
	_hdse_set_gpio0(0);
}

//MASTER发送规则:起始域(FRAME+DATA LEN(2Bytes)) + 信息域(nBytes)+ 终止域(1Byte 起始域到信息域的异或校验)
unsigned int _hdse_send_data(unsigned char frame,
							void *pdat,unsigned int datlen,
							void *pOutBuf,unsigned int outbuflen)
{
	unsigned int templen = 0;
	unsigned int slen = 0;
	unsigned int rlen = datlen + 1;		//加一个字节的校验
	unsigned char xor = 0;
	unsigned char *ptr = (unsigned char *)pdat;
	unsigned char firstbag[3] = {0};
	unsigned char last_flag = 0;
	unsigned int cnt = 0;
	unsigned char tempbuf[DEFAULT_HDSE_MAX_ONE_PACK_LEN] = {0};
	unsigned int acklen = 0;

	if ((pOutBuf == 0) || (outbuflen == 0))
	{
		return 0;
	}

	//准备好第一包
	firstbag[0] = frame;
	firstbag[1] = rlen >> 8;
	firstbag[2] = rlen;

	//计算异或校验
	xor = CommSumCheckCode(firstbag,3);
	xor ^= CommSumCheckCode(ptr,datlen);

	_hdse_spi_write(firstbag,3);

	//按照最多一次发送16字节的规则 将数据分割发送出去
	while (rlen)
	{
		if (rlen > stHDSE.one_pack_max_len)
		{
			templen = stHDSE.one_pack_max_len;
		}
		else
		{
			templen = rlen;
			last_flag = 1;
		}

		if (last_flag)
		{
			memcpy(tempbuf,ptr + slen,templen - 1);
			tempbuf[templen - 1] = xor;
		}
		else
		{
			memcpy(tempbuf,ptr + slen,templen);
		}
		_hdse_spi_write(tempbuf,templen);

		rlen -= templen;
		slen += templen;
		//_hdse_delayms(20);
	}

	/***********MASTER尝试接收SLAVE应答************/
	//_hdse_delayms(100);

	while (1)
	{
		//尝试接收第一包
		firstbag[0] = 0;
		firstbag[1] = 0;
		firstbag[2] = 0;

		_hdse_spi_read(3,firstbag,3);

		if ((firstbag[0] == PIB_ACTIVE_FRAME)
		|| (firstbag[0] == PIB_INFO_FRAME)
		|| (firstbag[0] == PIB_PROCESS_FRAME))
		{
			acklen = (firstbag[1] << 8) + firstbag[2];

			if (acklen > (outbuflen + 1))
			{
				DBG_Print(0,"[_hdse_send_data]outbuflen too small.(acklen:%d outbuflen:%d)\r\n",
				acklen,outbuflen);
				return 0;
			}
			break;
		}
		_hdse_delayms(10);
		cnt++;
		if (cnt >= 20)
		{
			DBG_PrintBuf(LEVEL0,"cmd send false:\r\n",pdat,datlen);
			return 0;
		}
	}

	//_hdse_delayms(10);

	ptr = (unsigned char *)pOutBuf;
	slen = 0;
	last_flag = 0;
	while (acklen)
	{
		if (acklen > stHDSE.one_pack_max_len)
		{
			templen = stHDSE.one_pack_max_len;
		}
		else
		{
			templen = acklen;
			last_flag = 1;
		}

		_hdse_spi_read(templen,tempbuf,sizeof(tempbuf));
		//_hdse_delayms(10);

		if (last_flag)	//最后一包 需要去掉最后一个字节 xor
		{
			memcpy(ptr + slen,tempbuf,templen - 1);
		}
		else
		{
			memcpy(ptr + slen,tempbuf,templen);
		}

		acklen -= templen;
		slen += templen;		
	}

	if (slen == 0)
	{
		return slen;
	}
	else
	{
		return (slen - 1);
	}
}


#if 0
根据命令体和命令体中的数据是否存在，命令可以分为四种格式：
格式  命令组成
1  CLA INS P1 P2
2  CLA INS P1 P2 Le
3  CLA INS P1 P2 Lc Data
4  CLA INS P1 P2 Lc Data Le

响应的格式
应答体  	应答尾部
响应数据体  SW1  SW2
#endif
//le为-1时 表示不使用该参数
//lc为0 或者 pdat为0 表示不使用 lc Data参数
unsigned int send_apdu_cmd(  unsigned char cla,
									unsigned char ins,
									unsigned char p1,
									unsigned char p2,
									unsigned char lc,
									unsigned char *pdat,
									int le,	//期望卡返回的长度 ，取值范围 1~256，其中 Le 为 0 表示 256字节
									unsigned char *pOut, unsigned int outbuflen,
									unsigned int *status)
{
	unsigned char *ptr = 0;
	unsigned int slen = 0;
	unsigned int rlen = 0;

	ptr = get_small_mem();
	if (ptr == 0) return 0;

	ptr[slen++] = cla;
	ptr[slen++] = ins;
	ptr[slen++] = p1;
	ptr[slen++] = p2;

	if (lc && pdat)
	{
		ptr[slen++] = lc;
		memcpy(ptr + slen,pdat,lc);
		slen += lc;
	}

	if (le != -1) ptr[slen++] = le;


	rlen = _hdse_send_data(PIB_INFO_FRAME,ptr,slen,pOut,outbuflen);
	free_small_mem(ptr);

	//返回状态
	if (status && (rlen >= 2))
	{
		*(unsigned short *)status = (*((unsigned char *)pOut + rlen - 2) << 8) + (*((unsigned char *)pOut + rlen - 1));
	}
	
	return rlen;
}


//mode:0x51加密  0x52解密  0x53私钥签名  0x54公钥验签
unsigned int _hdse_sm2_calc(unsigned char mode,unsigned char key_id,void *pdat,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *ptr = 0;
	unsigned int bufferoff=0;
	unsigned int p1 = 0;
	unsigned int p2 = 0;
	unsigned char *msg = (unsigned char *)pdat;
	unsigned int msg_len = datlen;
	unsigned int status = 0;
	unsigned int i = 0;
	unsigned int total_len_output = 0;

	if ((pdat == 0) || (datlen == 0) || (pOut == 0) || (outbuflen == 0))
	{
		return 0;
	}

	ptr = get_big_mem();

	if (ptr == 0)
	{
		return 0;
	}
#if 0
	;公钥加密
	;80|F4|00|01|0D|51(加密)|05(SM2_SM3)|0F(KEY ID)|0008(数据长度)|ENCRYPT_DATA(数据)
	APDU=80F400010D  51  05  0F  0008 (ENCRYPT_DATA),0x9000
	;返回前两字节为数据长度
	COPY(DECRYPT,RESP,"5","208");从RESP的第5个字符开始拷贝
#endif
	ptr[0] = mode;	//模式
	ptr[1] = 0x05;	//算法类型  0x05表示SM2 SM3
	ptr[2] = key_id;
	//APDU max data 0xFA
	//数据长度大于250时 要做级联处理 First block 5-bytes+250bytes data,others 255bytes data
	while (msg_len > 250)
	{
		if (p1 == 0)
		{
			ptr[3] = (msg_len >> 8) & 0xFF;
			ptr[4] = msg_len & 0xFF;
			memcpy(ptr + 5,msg + (p1 * 250),0xFA);
			bufferoff = 250;
		}
		else
		{
			memcpy(ptr,msg + 250 + ((p1 - 1) * bufferoff),255);
			bufferoff = 255;
		}

		rlen = send_apdu_cmd(0x80,0xF4,p1++,p2,0xFF,ptr,-1,pOut,outbuflen,&status);
		if (status != 0x9000)
		{
			free_big_mem(ptr);
			DBG_Print(LEVEL0,"[_hdse_sm2_calc]status err:%04X\r\n",status);
			return 0;
		}
	
		msg_len -= bufferoff;
	}
	//the last block
	p2 = 1;
	
	DBG_Print(LEVEL5,"[_hdse_sm2_calc]hdse send last bag(p2:%d remain msg_len:%d).\r\n",p2,msg_len);
	if(p1 == 0)//only
	{
		ptr[3] = msg_len >> 8;
		ptr[4] = msg_len;
		memcpy(ptr + 5,msg + (p1 * 250),msg_len);		
		rlen = send_apdu_cmd(0x80,0xF4,p1,p2,msg_len + 5,ptr,-1,pOut,outbuflen,&status);
	}
	else
	{
		memcpy(ptr,msg + 250 +((p1 - 1) * bufferoff),msg_len);
		rlen = send_apdu_cmd(0x80,0xF4,p1,p2,msg_len,ptr,-1,pOut,outbuflen,&status);
	}

	free_big_mem(ptr);

	DBG_Print(LEVEL4,"[_hdse_sm2_calc]rlen:%u status:%04X\r\n",rlen,status);
	DBG_PrintBuf(LEVEL4,0,pOut,rlen);
	if (rlen > 4)
	{
		total_len_output = (*((unsigned char *)pOut + 0) << 8) + *((unsigned char *)pOut + 1);

		//前两个字节是表示总长度 后面跟着数据和两个字节的状态
		slen = rlen - 4;   //减去(两字节的长度 + 两字节的状态)

		//pOut前两个字节覆盖
		for (i = 0; i < slen; i++)
		{
			*((unsigned char *)pOut + i) = *((unsigned char *)pOut + i + 2);
		}
		DBG_Print(LEVEL4,"first output(%u/%u):\r\n",slen,total_len_output);
		DBG_PrintBuf(LEVEL4,0,pOut,slen);
	}

	//取剩下的数据
	while (status == 0x6310)
	//while (slen != total_len_output)
	{
		unsigned char le = 0;
		p1 = 0x40;
		p2 = 0x00;
		if ((total_len_output - slen) >= 256)
		{
			le = 0;
		}
		else
		{
			le = total_len_output - slen;
		}

		rlen = send_apdu_cmd(0x80,0xF4,p1,p2,0,0,le, 
							(unsigned char *)pOut + slen, outbuflen - slen,&status);

		DBG_Print(LEVEL4,"get remain data ack(%u):\r\n",rlen);
		DBG_PrintBuf(LEVEL4,0,(unsigned char *)pOut + slen,rlen);

		if (rlen > 2)
		{
			//后面跟着数据和两个字节的状态
			slen += (rlen - 2);
			DBG_Print(LEVEL4,"[remain]output(%u/%u):\r\n",slen,total_len_output);
			DBG_PrintBuf(LEVEL4,0,pOut,slen);
		}
	}


	return slen;
}

//se输出的密文，可以先把数据分成三段，64字节c1，32字节c3，剩余字节c2

unsigned int _hdse_pubkey_encryption(unsigned char kid,void *pMingWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	if ((pOut == 0) || (outbuflen <= 96))
	{
		return 0;
	}

	if (get_sm2_ver() == SM2_VER2)
	{
		return _hdse_sm2_calc(0x51,kid,pMingWen, datlen, pOut,outbuflen);
	}
	else
	{
		unsigned char temp[32] = {0};
		unsigned int rlen = 0;
		unsigned int C2_len = 0;

		rlen = _hdse_sm2_calc(0x51,kid,pMingWen, datlen, pOut,outbuflen);
		if (rlen <= 96) return 0;

		DBG_Print(LEVEL4,"[kid:%d]明文数据(%u):\r\n",kid,datlen);
		DBG_PrintBuf(LEVEL4,0,pMingWen,datlen);

		DBG_Print(LEVEL4,"[kid:%d]C1C3C2加密后(%u):\r\n",kid,rlen);
		DBG_PrintBuf(LEVEL4,0,pOut,rlen);

		C2_len = rlen - 96;	//C2等于总长度减去C1 C3的长度
		memcpy(temp,(unsigned char *)pOut + 64,32);	//先把C3保存一下

		memcpy((unsigned char *)pOut + 64,(unsigned char *)pOut + 96,C2_len);	//C2
		memcpy((unsigned char *)pOut + 64 + C2_len,temp,32);		//C3

		DBG_Print(LEVEL4,"[kid:%d]C1C2C3加密后(%u):\r\n",kid,rlen);
		DBG_PrintBuf(LEVEL4,0,pOut,rlen);
		return rlen;
	}
}

unsigned int _hdse_privkey_decrypt(unsigned char kid,void *pMiWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	//密文长度必须大于96
	if ((pMiWen == 0) || (datlen <= 96))
	{
		return 0;
	}

	if (get_sm2_ver() == SM2_VER2)
	{
		return _hdse_sm2_calc(0x52,kid,pMiWen, datlen, pOut,outbuflen);
	}
	else
	{
		//如果是SM2_VER1模式 需要将密文拼成C1C3C2的形式再进行解密
		unsigned int rlen = 0;
		unsigned char *ptr = 0;
		unsigned int C2_len = datlen - 96;

		ptr = get_big_mem();
		if (ptr)
		{
			DBG_PrintBuf(LEVEL4,"C1C2C3 miwen:\r\n",pMiWen,datlen);

			memcpy(ptr,pMiWen,64);						//C1
			memcpy(ptr + 64,(unsigned char *)pMiWen + 64 + C2_len,32);	//C3
			memcpy(ptr + 96,(unsigned char *)pMiWen + 64,C2_len);		//C2

			DBG_PrintBuf(LEVEL4,"C1C3C2 miwen:\r\n",ptr,datlen);
			rlen = _hdse_sm2_calc(0x52,kid,ptr, datlen, pOut,outbuflen);

			free_big_mem(ptr);
		}

		return rlen;
	}
}


unsigned int hdse_public_key_encryption(void *pMingWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_pubkey_encryption(0x0F, pMingWen, datlen, pOut, outbuflen);
}

unsigned int hdse_public_key_encryptionB(void *pMingWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_pubkey_encryption(0x01, pMingWen, datlen, pOut, outbuflen);
	//return _hdse_sm2_calc(0x51,0x01,pMingWen, datlen, pOut,outbuflen);
}

unsigned int hdse_public_key_encryptionC(void *pMingWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_pubkey_encryption(0x03, pMingWen, datlen, pOut, outbuflen);
	//return _hdse_sm2_calc(0x51,0x03,pMingWen, datlen, pOut,outbuflen);
}

//私钥解密
unsigned int hdse_private_key_decrypt(void *pMiWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_privkey_decrypt(0x10, pMiWen, datlen, pOut, outbuflen);
	//return _hdse_sm2_calc(0x52,0x10,pMiWen, datlen, pOut,outbuflen);
}

unsigned int hdse_private_key_decryptB(void *pMiWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_privkey_decrypt(0x02, pMiWen, datlen, pOut, outbuflen);
	//return _hdse_sm2_calc(0x52,0x02,pMiWen, datlen, pOut,outbuflen);
}

unsigned int hdse_private_key_decryptC(void *pMiWen,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_privkey_decrypt(0x04, pMiWen, datlen, pOut, outbuflen);
	//return _hdse_sm2_calc(0x52,0x04,pMiWen, datlen, pOut,outbuflen);
}


//使用A组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
unsigned int hdsm_private_key_decrypt_encrypt(unsigned char type,
		void *data,unsigned int len, void *outdata, unsigned int maxlen)
{
	if (type == 1)
	{
		return hdse_public_key_encryption(data, len,outdata,maxlen);
	}
	else
	{
		return hdse_private_key_decrypt(data,len,outdata,maxlen);
	}
}

//使用B组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
unsigned int hdsm_private_key_decrypt_encryptB(unsigned char type,
				void *data, unsigned int len, void *outdata,unsigned int maxlen)
{
	if (type == 1)
	{
		return hdse_public_key_encryptionB(data, len,outdata,maxlen);
	}
	else
	{
		return hdse_private_key_decryptB(data,len,outdata,maxlen);
	}

}

//使用C组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
unsigned int hdsm_private_key_decrypt_encryptC(unsigned char type,
			void *data, unsigned int len,void *outdata,unsigned int maxlen)
{
	if (type == 1)
	{
		return hdse_public_key_encryptionC(data, len,outdata,maxlen);
	}
	else
	{
		return hdse_private_key_decryptC(data,len,outdata,maxlen);
	}
}



//私钥签名
unsigned int hdse_private_key_sign(void *pdat,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_sm2_calc(0x53,0x10,pdat, datlen, pOut,outbuflen);
}

unsigned int hdse_private_sign_data_with_z(void *chip_id,unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *pdat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen)
{
	unsigned char *ptr = 0;
	unsigned int rlen = 0;
	unsigned int status = 0;
	unsigned char *pza = 0;
	unsigned int zalen = 0;

	if ((chip_id == 0) || (chip_id_len == 0) || (public_key == 0) || (public_key_len == 0)
		|| (pdat == 0) || (datlen == 0) || (pOutSign == 0) || (outbuflen < 64)
	)
	{
		return 0;
	}
	ptr = get_big_mem();
	pza = get_small_mem();

	if ((ptr == 0) || (pza == 0))
	{
		free_big_mem(ptr);
		free_small_mem(pza);

		return 0;
	}

	//获取Z值
	ptr[0] = chip_id_len;
	memcpy(ptr + 1,chip_id,chip_id_len);
	memcpy(ptr + 1 + chip_id_len,public_key,0x40);

	rlen = send_apdu_cmd(0x80,0x4F,0,0, 1 + chip_id_len + 64, ptr,-1,pza,get_small_mem_size(),&status);
	if (status == 0x9000)
	{
		zalen = rlen - 2;
	}
	else
	{
		free_big_mem(ptr);
		free_small_mem(pza);
		DBG_Print(LEVEL0,"get za err(status:%04X)\r\n",status);
		return 0;
	}

	DBG_Print(LEVEL3,"get za ok(%u):\r\n",zalen);
	DBG_PrintBuf(LEVEL3,0,pza,zalen);

	//za+pdat
	memcpy(ptr,pza,zalen);
	memcpy(ptr + zalen,pdat,datlen);
	rlen = hdse_private_key_sign(ptr,zalen + datlen,pOutSign,outbuflen);

	free_big_mem(ptr);
	free_small_mem(pza);
	return rlen;
}

unsigned int hdsm_read_sign_data(void *pdat,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return hdse_private_key_sign( pdat, datlen, pOut, outbuflen);
}


//带Z值的签名 返回值:小于0表示失败 其中-66 表示传入的tempbuff缓冲区太小
//返回值大于零 表示 输出签名的长度
unsigned int hdsm_read_sign_data_with_z(void *chip_id,
							unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *pdat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen)
{
	return hdse_private_sign_data_with_z(chip_id,chip_id_len,public_key,public_key_len,pdat,datlen, pOutSign, outbuflen);
}


//公钥验签
unsigned int hdse_public_key_verity_sign(void *pdat,unsigned int datlen,void *pOut,unsigned int outbuflen)
{
	return _hdse_sm2_calc(0x54,0x0F,pdat, datlen, pOut,outbuflen);
}

#if 0

{0x80,0xF2,0x01,0x00,0x04,04,key_id,0x01,0x00}

#endif

unsigned int _hdse_read_public_key(unsigned char kid,void *pOut,unsigned int outbuflen)
{
	unsigned char cmd[8] = {0x04,0,0x01,0x00};
	unsigned int status = 0;
	unsigned int rlen = 0;
	unsigned char *ptr = 0;

	ptr = get_small_mem();
	if (ptr == 0) return 0;

	cmd[1] = kid;
	rlen = send_apdu_cmd(0x80,0xF2,0x01,0x00,0x04,cmd,-1, ptr, get_small_mem_size(),&status);
	//公钥长度64
	if (pOut && (outbuflen >= 64))
	{
		memcpy(pOut,ptr + 2,32);
		memcpy((unsigned char *)pOut + 32,ptr + 2 + 32 + 2,32);
		rlen = 64;
	}

	free_small_mem(ptr);

	if (status != 0x9000)
	{
		rlen = 0;
		DBG_Print(LEVEL0,"read public err(status:%04X)\r\n",status);
	}

	return rlen;
}

unsigned int hdsm_read_public_key(void *pOut,unsigned int outbuflen)
{
	return _hdse_read_public_key(0x0F,pOut,outbuflen);
}

unsigned int hdsm_read_public_keyB(void *pOut,unsigned int outbuflen)
{
	return _hdse_read_public_key(0x01,pOut,outbuflen);
}

unsigned int hdsm_read_public_keyC(void *pOut,unsigned int outbuflen)
{
	return _hdse_read_public_key(0x03,pOut,outbuflen);
}



//Symmetric Alg mode
#define ET_CBC 1
#define ET_ECB 0

//Mode
#define ENC 0//Encrypt
#define DEC 1//Decrypt


#define ICC_ECB	0
#define ICC_CBC	1

#define EXCUTE_SUC		0x9000
#define CER_HEXSTR		0xff30
#define CER_DECSTR		0xff31
#define CER_BINSTR		0xff32
#define CER_LARGE		0xff33
#define CER_MODE		0xff34
#define CER_KEYLEN		0xff35
#define CER_DATALEN		0xff36
#define CER_CBC			0xff37
#define CER_RESULT		0xff38
#define CER_UNKNOWN     0xffff

#define MBEDTLS_DES_ENCRYPT     1
#define MBEDTLS_DES_DECRYPT     0

#define MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH              -0x0032  /**< The data input has an invalid length. */
#define MBEDTLS_ERR_DES_HW_ACCEL_FAILED                   -0x0033  /**< DES hardware accelerator failed. */

#define MBEDTLS_DES_KEY_SIZE    8

typedef struct
{
    unsigned int sk[32];            /*!<  DES subkeys       */
}
mbedtls_des_context;

/**
 * \brief          Triple-DES context structure
 */
typedef struct
{
    unsigned int sk[96];            /*!<  3DES subkeys      */
}
mbedtls_des3_context;

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = (unsigned char*)v; while( n-- ) *p++ = 0;
}


void mbedtls_des_init( mbedtls_des_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_des_context ) );
}

void mbedtls_des_free( mbedtls_des_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_des_context ) );
}

void mbedtls_des3_init( mbedtls_des3_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_des3_context ) );
}

void mbedtls_des3_free( mbedtls_des3_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_des3_context ) );
}

/*
 * Expanded DES S-boxes
 */
static const unsigned int SB1[64] =
{
    0x01010400, 0x00000000, 0x00010000, 0x01010404,
    0x01010004, 0x00010404, 0x00000004, 0x00010000,
    0x00000400, 0x01010400, 0x01010404, 0x00000400,
    0x01000404, 0x01010004, 0x01000000, 0x00000004,
    0x00000404, 0x01000400, 0x01000400, 0x00010400,
    0x00010400, 0x01010000, 0x01010000, 0x01000404,
    0x00010004, 0x01000004, 0x01000004, 0x00010004,
    0x00000000, 0x00000404, 0x00010404, 0x01000000,
    0x00010000, 0x01010404, 0x00000004, 0x01010000,
    0x01010400, 0x01000000, 0x01000000, 0x00000400,
    0x01010004, 0x00010000, 0x00010400, 0x01000004,
    0x00000400, 0x00000004, 0x01000404, 0x00010404,
    0x01010404, 0x00010004, 0x01010000, 0x01000404,
    0x01000004, 0x00000404, 0x00010404, 0x01010400,
    0x00000404, 0x01000400, 0x01000400, 0x00000000,
    0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const unsigned int SB2[64] =
{
    0x80108020, 0x80008000, 0x00008000, 0x00108020,
    0x00100000, 0x00000020, 0x80100020, 0x80008020,
    0x80000020, 0x80108020, 0x80108000, 0x80000000,
    0x80008000, 0x00100000, 0x00000020, 0x80100020,
    0x00108000, 0x00100020, 0x80008020, 0x00000000,
    0x80000000, 0x00008000, 0x00108020, 0x80100000,
    0x00100020, 0x80000020, 0x00000000, 0x00108000,
    0x00008020, 0x80108000, 0x80100000, 0x00008020,
    0x00000000, 0x00108020, 0x80100020, 0x00100000,
    0x80008020, 0x80100000, 0x80108000, 0x00008000,
    0x80100000, 0x80008000, 0x00000020, 0x80108020,
    0x00108020, 0x00000020, 0x00008000, 0x80000000,
    0x00008020, 0x80108000, 0x00100000, 0x80000020,
    0x00100020, 0x80008020, 0x80000020, 0x00100020,
    0x00108000, 0x00000000, 0x80008000, 0x00008020,
    0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const unsigned int SB3[64] =
{
    0x00000208, 0x08020200, 0x00000000, 0x08020008,
    0x08000200, 0x00000000, 0x00020208, 0x08000200,
    0x00020008, 0x08000008, 0x08000008, 0x00020000,
    0x08020208, 0x00020008, 0x08020000, 0x00000208,
    0x08000000, 0x00000008, 0x08020200, 0x00000200,
    0x00020200, 0x08020000, 0x08020008, 0x00020208,
    0x08000208, 0x00020200, 0x00020000, 0x08000208,
    0x00000008, 0x08020208, 0x00000200, 0x08000000,
    0x08020200, 0x08000000, 0x00020008, 0x00000208,
    0x00020000, 0x08020200, 0x08000200, 0x00000000,
    0x00000200, 0x00020008, 0x08020208, 0x08000200,
    0x08000008, 0x00000200, 0x00000000, 0x08020008,
    0x08000208, 0x00020000, 0x08000000, 0x08020208,
    0x00000008, 0x00020208, 0x00020200, 0x08000008,
    0x08020000, 0x08000208, 0x00000208, 0x08020000,
    0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const unsigned int SB4[64] =
{
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802080, 0x00800081, 0x00800001, 0x00002001,
    0x00000000, 0x00802000, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00800080, 0x00800001,
    0x00000001, 0x00002000, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002001, 0x00002080,
    0x00800081, 0x00000001, 0x00002080, 0x00800080,
    0x00002000, 0x00802080, 0x00802081, 0x00000081,
    0x00800080, 0x00800001, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00000000, 0x00802000,
    0x00002080, 0x00800080, 0x00800081, 0x00000001,
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802081, 0x00000081, 0x00000001, 0x00002000,
    0x00800001, 0x00002001, 0x00802080, 0x00800081,
    0x00002001, 0x00002080, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const unsigned int SB5[64] =
{
    0x00000100, 0x02080100, 0x02080000, 0x42000100,
    0x00080000, 0x00000100, 0x40000000, 0x02080000,
    0x40080100, 0x00080000, 0x02000100, 0x40080100,
    0x42000100, 0x42080000, 0x00080100, 0x40000000,
    0x02000000, 0x40080000, 0x40080000, 0x00000000,
    0x40000100, 0x42080100, 0x42080100, 0x02000100,
    0x42080000, 0x40000100, 0x00000000, 0x42000000,
    0x02080100, 0x02000000, 0x42000000, 0x00080100,
    0x00080000, 0x42000100, 0x00000100, 0x02000000,
    0x40000000, 0x02080000, 0x42000100, 0x40080100,
    0x02000100, 0x40000000, 0x42080000, 0x02080100,
    0x40080100, 0x00000100, 0x02000000, 0x42080000,
    0x42080100, 0x00080100, 0x42000000, 0x42080100,
    0x02080000, 0x00000000, 0x40080000, 0x42000000,
    0x00080100, 0x02000100, 0x40000100, 0x00080000,
    0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const unsigned int SB6[64] =
{
    0x20000010, 0x20400000, 0x00004000, 0x20404010,
    0x20400000, 0x00000010, 0x20404010, 0x00400000,
    0x20004000, 0x00404010, 0x00400000, 0x20000010,
    0x00400010, 0x20004000, 0x20000000, 0x00004010,
    0x00000000, 0x00400010, 0x20004010, 0x00004000,
    0x00404000, 0x20004010, 0x00000010, 0x20400010,
    0x20400010, 0x00000000, 0x00404010, 0x20404000,
    0x00004010, 0x00404000, 0x20404000, 0x20000000,
    0x20004000, 0x00000010, 0x20400010, 0x00404000,
    0x20404010, 0x00400000, 0x00004010, 0x20000010,
    0x00400000, 0x20004000, 0x20000000, 0x00004010,
    0x20000010, 0x20404010, 0x00404000, 0x20400000,
    0x00404010, 0x20404000, 0x00000000, 0x20400010,
    0x00000010, 0x00004000, 0x20400000, 0x00404010,
    0x00004000, 0x00400010, 0x20004010, 0x00000000,
    0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const unsigned int SB7[64] =
{
    0x00200000, 0x04200002, 0x04000802, 0x00000000,
    0x00000800, 0x04000802, 0x00200802, 0x04200800,
    0x04200802, 0x00200000, 0x00000000, 0x04000002,
    0x00000002, 0x04000000, 0x04200002, 0x00000802,
    0x04000800, 0x00200802, 0x00200002, 0x04000800,
    0x04000002, 0x04200000, 0x04200800, 0x00200002,
    0x04200000, 0x00000800, 0x00000802, 0x04200802,
    0x00200800, 0x00000002, 0x04000000, 0x00200800,
    0x04000000, 0x00200800, 0x00200000, 0x04000802,
    0x04000802, 0x04200002, 0x04200002, 0x00000002,
    0x00200002, 0x04000000, 0x04000800, 0x00200000,
    0x04200800, 0x00000802, 0x00200802, 0x04200800,
    0x00000802, 0x04000002, 0x04200802, 0x04200000,
    0x00200800, 0x00000000, 0x00000002, 0x04200802,
    0x00000000, 0x00200802, 0x04200000, 0x00000800,
    0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const unsigned int SB8[64] =
{
    0x10001040, 0x00001000, 0x00040000, 0x10041040,
    0x10000000, 0x10001040, 0x00000040, 0x10000000,
    0x00040040, 0x10040000, 0x10041040, 0x00041000,
    0x10041000, 0x00041040, 0x00001000, 0x00000040,
    0x10040000, 0x10000040, 0x10001000, 0x00001040,
    0x00041000, 0x00040040, 0x10040040, 0x10041000,
    0x00001040, 0x00000000, 0x00000000, 0x10040040,
    0x10000040, 0x10001000, 0x00041040, 0x00040000,
    0x00041040, 0x00040000, 0x10041000, 0x00001000,
    0x00000040, 0x10040040, 0x00001000, 0x00041040,
    0x10001000, 0x00000040, 0x10000040, 0x10040000,
    0x10040040, 0x10000000, 0x00040000, 0x10001040,
    0x00000000, 0x10041040, 0x00040040, 0x10000040,
    0x10040000, 0x10001000, 0x10001040, 0x00000000,
    0x10041040, 0x00041000, 0x00041000, 0x00001040,
    0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * PC1: left and right halves bit-swap
 */
static const unsigned int LHs[16] =
{
    0x00000000, 0x00000001, 0x00000100, 0x00000101,
    0x00010000, 0x00010001, 0x00010100, 0x00010101,
    0x01000000, 0x01000001, 0x01000100, 0x01000101,
    0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const unsigned int RHs[16] =
{
    0x00000000, 0x01000000, 0x00010000, 0x01010000,
    0x00000100, 0x01000100, 0x00010100, 0x01010100,
    0x00000001, 0x01000001, 0x00010001, 0x01010001,
    0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned int) (b)[(i)    ] << 24 )             \
        | ( (unsigned int) (b)[(i) + 1] << 16 )             \
        | ( (unsigned int) (b)[(i) + 2] <<  8 )             \
        | ( (unsigned int) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

#define SWAP(a,b) { unsigned int t = a; a = b; b = t; t = 0; }



void mbedtls_des_setkey( unsigned int SK[32], const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    int i;
    unsigned int X, Y, T;

    GET_UINT32_BE( X, key, 0 );
    GET_UINT32_BE( Y, key, 4 );

    /*
     * Permuted Choice 1
     */
    T =  ((Y >>  4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T <<  4);
    T =  ((Y      ) ^ X) & 0x10101010;  X ^= T; Y ^= (T      );

    X =   (LHs[ (X      ) & 0xF] << 3) | (LHs[ (X >>  8) & 0xF ] << 2)
        | (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ]     )
        | (LHs[ (X >>  5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
        | (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

    Y =   (RHs[ (Y >>  1) & 0xF] << 3) | (RHs[ (Y >>  9) & 0xF ] << 2)
        | (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ]     )
        | (RHs[ (Y >>  4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
        | (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

    X &= 0x0FFFFFFF;
    Y &= 0x0FFFFFFF;

    /*
     * calculate subkeys
     */
    for( i = 0; i < 16; i++ )
    {
        if( i < 2 || i == 8 || i == 15 )
        {
            X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
            Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
        }
        else
        {
            X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
            Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
        }

        *SK++ =   ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
                | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
                | ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
                | ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
                | ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
                | ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
                | ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
                | ((Y >> 14) & 0x00000200) | ((Y      ) & 0x00000100)
                | ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
                | ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
                | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

        *SK++ =   ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
                | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
                | ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
                | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
                | ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
                | ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
                | ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
                | ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
                | ((Y      ) & 0x00000200) | ((Y <<  7) & 0x00000100)
                | ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
                | ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
    }
}

/*
 * Initial Permutation macro
 */
#define DES_IP(X,Y)                                             \
{                                                               \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
    X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
}

/*
 * Final Permutation macro
 */
#define DES_FP(X,Y)                                             \
{                                                               \
    X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
    Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
}

/*
 * DES round macro
 */
#define DES_ROUND(X,Y)                          \
{                                               \
    T = *SK++ ^ X;                              \
    Y ^= SB8[ (T      ) & 0x3F ] ^              \
         SB6[ (T >>  8) & 0x3F ] ^              \
         SB4[ (T >> 16) & 0x3F ] ^              \
         SB2[ (T >> 24) & 0x3F ];               \
                                                \
    T = *SK++ ^ ((X << 28) | (X >> 4));         \
    Y ^= SB7[ (T      ) & 0x3F ] ^              \
         SB5[ (T >>  8) & 0x3F ] ^              \
         SB3[ (T >> 16) & 0x3F ] ^              \
         SB1[ (T >> 24) & 0x3F ];               \
}


static void des3_set2key( unsigned int esk[96],
                          unsigned int dsk[96],
                          const unsigned char key[MBEDTLS_DES_KEY_SIZE*2] )
{
    int i;

    mbedtls_des_setkey( esk, key );
    mbedtls_des_setkey( dsk + 32, key + 8 );

    for( i = 0; i < 32; i += 2 )
    {
        dsk[i     ] = esk[30 - i];
        dsk[i +  1] = esk[31 - i];

        esk[i + 32] = dsk[62 - i];
        esk[i + 33] = dsk[63 - i];

        esk[i + 64] = esk[i    ];
        esk[i + 65] = esk[i + 1];

        dsk[i + 64] = dsk[i    ];
        dsk[i + 65] = dsk[i + 1];
    }
}

static void des3_set3key( unsigned int esk[96],
                          unsigned int dsk[96],
                          const unsigned char key[24] )
{
    int i;

    mbedtls_des_setkey( esk, key );
    mbedtls_des_setkey( dsk + 32, key +  8 );
    mbedtls_des_setkey( esk + 64, key + 16 );

    for( i = 0; i < 32; i += 2 )
    {
        dsk[i     ] = esk[94 - i];
        dsk[i +  1] = esk[95 - i];

        esk[i + 32] = dsk[62 - i];
        esk[i + 33] = dsk[63 - i];

        dsk[i + 64] = esk[30 - i];
        dsk[i + 65] = esk[31 - i];
    }
}


/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int mbedtls_des3_set2key_enc( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] )
{
    unsigned int *sk = 0;

	sk = get_big_mem();
	if (sk == 0)
	{
		return 0;
	}
    des3_set2key( ctx->sk, sk, key );
    mbedtls_zeroize( sk,  sizeof( int ) * 96 );
	free_big_mem(sk);

    return( 0 );
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int mbedtls_des3_set2key_dec( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] )
{
    unsigned int *sk = 0;

	sk = get_big_mem();
	if (sk == 0)
	{
		return 0;
	}
    des3_set2key( sk, ctx->sk, key );
    mbedtls_zeroize( sk,  sizeof( int ) * 96);
	free_big_mem(sk);
    return( 0 );
}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int mbedtls_des3_set3key_enc( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] )
{
    unsigned int *sk = 0;

	sk = get_big_mem();
	if (sk == 0)
	{
		return 0;
	}
    des3_set3key( ctx->sk, sk, key );
    mbedtls_zeroize( sk,  sizeof( int ) * 96 );

	free_big_mem(sk);
    return( 0 );
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int mbedtls_des3_set3key_dec( mbedtls_des3_context *ctx,
                      const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] )
{
    unsigned int *sk = 0;

	sk = get_big_mem();
	if (sk == 0)
	{
		return 0;
	}
    des3_set3key( sk, ctx->sk, key );
    mbedtls_zeroize( sk,  sizeof( int ) * 96);

	free_big_mem(sk);

    return( 0 );
}

/*
 * DES-ECB block encryption/decryption
 */
int mbedtls_des_crypt_ecb( mbedtls_des_context *ctx,
                    const unsigned char input[8],
                    unsigned char output[8] )
{
    int i;
    unsigned int X, Y, T, *SK;

    SK = ctx->sk;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );

    return( 0 );
}


/*
 * 3DES-ECB block encryption/decryption
 */
int mbedtls_des3_crypt_ecb( mbedtls_des3_context *ctx,
                     const unsigned char input[8],
                     unsigned char output[8] )
{
    int i;
    unsigned int X, Y, T, *SK;

    SK = ctx->sk;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( X, Y );
        DES_ROUND( Y, X );
    }

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );

    return( 0 );
}

/*
 * DES key schedule (56-bit, encryption)
 */
int mbedtls_des_setkey_enc( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    mbedtls_des_setkey( ctx->sk, key );

    return( 0 );
}

/*
 * DES key schedule (56-bit, decryption)
 */
int mbedtls_des_setkey_dec( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] )
{
    int i;

    mbedtls_des_setkey( ctx->sk, key );

    for( i = 0; i < 16; i += 2 )
    {
        SWAP( ctx->sk[i    ], ctx->sk[30 - i] );
        SWAP( ctx->sk[i + 1], ctx->sk[31 - i] );
    }

    return( 0 );
}

unsigned int HD_3DES_EncryptByte(unsigned char *svHex, unsigned int svHexLen, unsigned char *svKey, unsigned int svKeyLen, unsigned int ivMode, unsigned char *srHex, unsigned int *srHexLen)
{
#if 1
	unsigned int i = 0;
	unsigned int ptr = 0;
	unsigned char iv[8] = {0};
	unsigned char buf[8] = {0};

	static mbedtls_des3_context ctx3;

	// Check para 
	if((ivMode != ICC_ECB) && (ivMode != ICC_CBC)) 
	{
		return CER_MODE;                         // mode only is 0 or 1
	}
	if((svHexLen % 8) != 0)
	{
		return CER_DATALEN;		                 // len of DATA must be the time of 8
	}
	if((ivMode == ICC_CBC) && (svHexLen < 16))	
	{
		return CER_CBC;                          // len of DATA must be larger than 8
	}

	// Init seed
	if(ivMode == ICC_CBC)
	{
		for(i = 0; i < 8; i++)
		{
			iv[i] = svHex[i];
		}
		ptr = 8;
		svHexLen = svHexLen - 8;
	}
	else
	{
		ptr = 0;
	}
	*srHexLen = svHexLen;

	// Init & Set Key
	mbedtls_des3_init(&ctx3);
	if (svKeyLen == 16)	        // 112 bit
	{		
		mbedtls_des3_set2key_enc(&ctx3, svKey);
	}
	else if (svKeyLen == 24)	// 168 bit
	{
		mbedtls_des3_set3key_enc(&ctx3, svKey);
	}
	else
	{
		return CER_KEYLEN;		// len of KEY must be 16 bytes or 24 byte
	}

	// Loop
	while(svHexLen/8 != 0)
	{
		// get plain data & XOR
		for(i = ptr; i < ptr+8; i++)
		{
			buf[i-ptr] = svHex[i];
		}

		if(ivMode == ICC_CBC)
		{
			for(i = 0; i < 8; i++)
			{
				buf[i] = buf[i] ^ iv[i];
			}
		}

		// Encipher
		mbedtls_des3_crypt_ecb(&ctx3, buf, buf);

		if(ivMode == ICC_CBC)
		{
			memcpy(iv, buf, 8);
		}

		// get result
		for(i = ptr; i < ptr+8; i++)
		{
			if(ivMode == ICC_CBC) 
			{
				srHex[i-8] = buf[i%8];			
			}
			else
			{
				srHex[i] = buf[i%8];		
			}
		}
		svHexLen = svHexLen - 8;
		ptr = ptr + 8;
	}

	mbedtls_des3_free(&ctx3);
#endif
	return EXCUTE_SUC;

}


unsigned int HD_DES_EncryptByte(unsigned char *svHex, unsigned int svHexLen, unsigned char *svKey, unsigned int svKeyLen, unsigned int ivMode, unsigned char *srHex, unsigned int *srHexLen)
{
	unsigned int i = 0;
	unsigned int ptr = 0;
	unsigned char iv[8] = {0};
	unsigned char buf[8] = {0};

	static mbedtls_des_context ctx;

	// Check para 
	if((ivMode != ICC_ECB) && (ivMode != ICC_CBC)) 
	{
		return CER_MODE;                         // mode only is 0 or 1
	}
	if((svHexLen % 8) != 0)
	{
		return CER_DATALEN;		                 // len of DATA must be the time of 8
	}
	if((ivMode == ICC_CBC) && (svHexLen < 16))	
	{
		return CER_CBC;                          // len of DATA must be larger than 8
	}

	// Init seed
	if(ivMode == ICC_CBC)
	{
		for(i = 0; i < 8; i++)
		{
			iv[i] = svHex[i];
		}
		ptr = 8;
		svHexLen = svHexLen - 8;
	}
	else
	{
		ptr = 0;
	}
	*srHexLen = svHexLen;

	// Init & Set Key
	mbedtls_des_init(&ctx);
	if (svKeyLen == 8)	        // 64 bit
	{		
		mbedtls_des_setkey_enc(&ctx, svKey);
	}
	else
	{
		return CER_KEYLEN;		// len of KEY must be 8 byte
	}

	// Loop
	while(svHexLen/8 != 0)
	{
		// get plain data & XOR
		for(i = ptr; i < ptr+8; i++)
		{
			buf[i-ptr] = svHex[i];
		}

		if(ivMode == ICC_CBC)
		{
			for(i = 0; i < 8; i++)
			{
				buf[i] = buf[i] ^ iv[i];
			}
		}

		// Encipher
		mbedtls_des_crypt_ecb(&ctx, buf, buf);

		if(ivMode == ICC_CBC)
		{
			memcpy(iv, buf, 8);
		}

		// get result
		for(i = ptr; i < ptr+8; i++)
		{
			if(ivMode == ICC_CBC) 
			{
				srHex[i-8] = buf[i%8];			
			}
			else
			{
				srHex[i] = buf[i%8];		
			}
		}
		svHexLen=svHexLen-8;
	}

	mbedtls_des_free(&ctx);
	return EXCUTE_SUC;
}


unsigned int HD_3DES_DecryptByte(unsigned char *svHex, unsigned int svHexLen, unsigned char *svKey, unsigned int svKeyLen, unsigned int ivMode, unsigned char *srHex, unsigned int *srHexLen)
{
	unsigned int i = 0;
	unsigned int ptr = 0;
	unsigned char iv[8] = {0};
	unsigned char buf[8] = {0};

	static mbedtls_des3_context ctx3;

	// Check para 
	if((ivMode != ICC_ECB) && (ivMode != ICC_CBC)) 
	{
		return CER_MODE;                         // mode only is 0 or 1
	}
	if((svHexLen % 8) != 0)
	{
		return CER_DATALEN;		                 // len of DATA must be the time of 8
	}
	if((ivMode == ICC_CBC) && (svHexLen < 16))	
	{
		return CER_CBC;                          // len of DATA must be larger than 8
	}

	// Init seed
	if(ivMode == ICC_CBC)
	{
		for(i = 0; i < 8; i++)
		{
			iv[i] = svHex[i];
		}
		ptr = 8;
		svHexLen = svHexLen - 8;
	}
	else
	{
		ptr = 0;
	}
	*srHexLen = svHexLen;

	// Init & Set Key
	mbedtls_des3_init(&ctx3);
	if (svKeyLen == 16)	        // 112 bit
	{		
		mbedtls_des3_set2key_dec(&ctx3, svKey);
	}
	else if (svKeyLen == 24)	// 168 bit
	{
		mbedtls_des3_set3key_dec(&ctx3, svKey);
	}
	else
	{
		return CER_KEYLEN;		// len of KEY must be 16 bytes or 24 byte
	}

	// Loop
	while(svHexLen/8 != 0)
	{
		// get plain data & XOR
		for(i = ptr; i < ptr+8; i++)
		{
			buf[i-ptr] = svHex[i];
		}

		if(ivMode == ICC_CBC)
		{
			for(i = 0; i < 8; i++)
			{
				buf[i] = buf[i] ^ iv[i];
			}
		}

		// Encipher
		mbedtls_des3_crypt_ecb(&ctx3, buf, buf);

		if(ivMode == ICC_CBC)
		{
			memcpy(iv, buf, 8);
		}

		// get result
		for(i = ptr; i < ptr+8; i++)
		{
			if(ivMode == ICC_CBC) 
			{
				srHex[i-8] = buf[i%8];			
			}
			else
			{
				srHex[i] = buf[i%8];		
			}
		}
		svHexLen = svHexLen - 8;
		ptr = ptr + 8;
	}

	mbedtls_des3_free(&ctx3);
	return EXCUTE_SUC;

}

unsigned long calc3DES(unsigned char* sSrc,unsigned int sSrcLen,unsigned char* sKey,unsigned int sKeyLen,unsigned int uTp,unsigned int mode,unsigned char* sDst, unsigned int*sDstLen)
{
#if 1
	unsigned int usRet = 0x9000;

	switch (mode){
		case 0://encrypt
			{
				if(uTp==ET_ECB)
				{
					usRet = HD_3DES_EncryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_ECB,sDst,sDstLen);
				}
				else
				{
					usRet = HD_3DES_EncryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_CBC,sDst,sDstLen);
				}
				break;
			}
		case 1://decrypt
			{
				if(uTp==ET_ECB)
				{
					usRet = HD_3DES_DecryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_ECB,sDst,sDstLen);
				}
				else
				{
					usRet = HD_3DES_DecryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_CBC,sDst,sDstLen);
				}
				break;
			}

	}

	if (usRet != 0x9000)
	{
		return 0xFFFFFFFF;
	}
#endif
	return *sDstLen;
}



unsigned int HD_3DES_MACByte(unsigned char *svHex, unsigned int svHexLen, unsigned char *svKey, unsigned int svKeyLen, unsigned char *srHex, unsigned int *srHexLen)
{
#if 1
	unsigned int i = 0;
	unsigned int ptr = 0;
	unsigned char iv[8] = {0};
	unsigned char buf[8] = {0};

	static mbedtls_des_context ctx;

	// Check para 
	if(svHexLen % 8 != 0)
	{
		return CER_DATALEN;		// len of DATA must be the time of 8
	}
	if(svHexLen < 16)
	{
		return CER_CBC;        // len of DATA must be larger than 16
	}

	// Init seed
	for(i = 0; i < 8; i++)
	{
		iv[i] = svHex[i];
	}
	svHexLen = svHexLen - 8;
	ptr = 8;

	// Init
	mbedtls_des_init(&ctx);

	// Loop XOR & Encipher, use K1
	mbedtls_des_setkey_enc(&ctx, svKey);
	while(svHexLen/8 != 0)
	{
		// get plain data 
		for(i = ptr; i < ptr+8; i++)
		{
			buf[i-ptr] = svHex[i];
		}
		// XOR
		for(i = 0; i < 8; i++)
		{
			buf[i] = buf[i] ^ iv[i];
		}

		// Encipher, use K1
		mbedtls_des_crypt_ecb(&ctx, buf, buf);

		memcpy(iv, buf, 8);
		svHexLen = svHexLen - 8;
		ptr = ptr + 8;
	}

	// Decipher, use K2
	mbedtls_des_setkey_dec(&ctx, svKey + 8);
	mbedtls_des_crypt_ecb(&ctx, buf, buf);

	// Encipher
	if (svKeyLen == 16)	        // 112 bit, use K1
	{
		mbedtls_des_setkey_enc(&ctx, svKey);
	}
	else if (svKeyLen == 24)	// 168 bit, use K3
	{
		mbedtls_des_setkey_enc(&ctx, svKey + 16);
	}
	else
	{
		return CER_KEYLEN;		// len of KEY must be 16 bytes or 24 byte
	}
	mbedtls_des_crypt_ecb(&ctx, buf, buf);

	// get result
	for(i = 0; i < 8; i++)
	{
		srHex[i] = buf[i];
	}
	*srHexLen = i;

	mbedtls_des_free(&ctx);
#endif
	return EXCUTE_SUC;
}

unsigned long calc3MAC(unsigned char* sSrc,unsigned int sSrcLen,unsigned char* sIV,unsigned int sIVLen,unsigned char* sKey,unsigned int svKeyLen,unsigned char* sDst,unsigned int*sDstLen)
{
#if 1
	unsigned char *uBuf = NULL;

	uBuf = (unsigned char*)get_small_mem();
	memcpy(uBuf, sIV, sIVLen);
	memcpy(uBuf+sIVLen, sSrc, sSrcLen);

	if (HD_3DES_MACByte(uBuf,sSrcLen+sIVLen, sKey,svKeyLen,sDst,sDstLen) != 0x9000)
	{
		free_small_mem(uBuf);
		return 0xFFFFFFFF;
	}	
	free_small_mem(uBuf);
#endif
	return 8* 2;
}

unsigned long calcDES(unsigned char* sSrc,unsigned int sSrcLen,unsigned char* sKey,unsigned int sKeyLen,unsigned int uTp,unsigned char* sDst, unsigned int*sDstLen)
{
	unsigned int usRet = 0x9000;
	if(uTp==ET_ECB)//ECB
	{
		usRet = HD_DES_EncryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_ECB,sDst,sDstLen);
	}
	else//CBC
	{
		usRet = HD_DES_EncryptByte(sSrc, sSrcLen,sKey,sKeyLen,ET_CBC,sDst,sDstLen);
	}
	if (usRet != 0x9000)
	{
		return 0xFFFFFFFF;
	}
	return *sDstLen;
}


int scp02_auth(unsigned char *outenc,
	unsigned char *outmac,unsigned char *outdek,
	unsigned char *CMAC)
{
	static unsigned char ptr[128] = {0};
	unsigned char encKey[16]={0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F};
	unsigned char macKey[16]={0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F};
	unsigned char dekKey[16]={0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F};
	unsigned char input[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

	unsigned char level = 0x03;

	unsigned char seqCounter[2]={0}; //2-bytes seq counter
	unsigned char cardRandom[6]={0};//6-bytes card random
	unsigned char cardCrypt[8]={0};//8-bytes card chiper

	//dispersion factor
	unsigned char vspEncData[24]={0};
	unsigned char vspMacData[24]={0};
	unsigned char vspDekData[24]={0};
	//process key
	unsigned char vspEnc[24]={0};
	unsigned char vspMac[24]={0};
	unsigned char vspDek[24]={0};
	unsigned int vspEnclen = 0;
	unsigned int vspMaclen = 0;
	unsigned int vspDeklen = 0;
	unsigned int iDataLen = 0;
	//host chiper
	unsigned char dataforHostCrypt[32]={0};
	unsigned char hdataCrypt[32]={0};
	unsigned char vhostCrypt[8]={0};
	unsigned int hdatalen = 0;
	//C-MAC
	unsigned char dataforMAC[24]={0};
	unsigned char vcMAC[8]={0};
	unsigned int vcMAClen = 0;
	unsigned char IV[8]={0};
	unsigned char dataIn[16]={0};
	unsigned int sta = 0;

	//ret =sendcommand(handle,ISO_CASE_4,0x80,0x50,0x00,0x00,input,0x08,0x1C,output, output_len);
	send_apdu_cmd(0x80,0x50,0x00,0,0x08,input,0x1C,ptr,sizeof(ptr),&sta);
	if (sta != 0x9000)
	{
		DBG_Print(0,"scp02_auth false(%04X)\r\n",sta);
		return 1;
	}

	memcpy(seqCounter,ptr+12,2);
	memcpy(cardRandom,ptr+14,6);
	memcpy(cardCrypt,ptr+20,8);
	
	memset(vspEncData,0x00,8);
	vspEncData[8]=0x01;
	vspEncData[9]=0x82;
	memcpy(vspEncData+10,seqCounter,2);
	memset(vspEncData+12,0x00,12);
	
	memset(vspMacData,0x00,8);
	vspMacData[8]=0x01;
	vspMacData[9]=0x01;
	memcpy(vspMacData+10,seqCounter,2);
	memset(vspMacData+12,0x00,12);

	memset(vspDekData,0x00,8);
	vspDekData[8]=0x01;
	vspDekData[9]=0x81;
	memcpy(vspDekData+10,seqCounter,2);
	memset(vspDekData+12,0x00,12);


	iDataLen=calc3DES(vspEncData,0x18,encKey,0x10,ET_CBC,ENC,vspEnc,&vspEnclen);
	if ( iDataLen ==0xFFFFFFFF )
	{
		return iDataLen;
	}
	iDataLen=calc3DES(vspMacData,0x18,macKey,0x10,ET_CBC,ENC,vspMac,&vspMaclen);
	if ( iDataLen ==0xFFFFFFFF)
	{
		return iDataLen;
	}
	iDataLen=calc3DES(vspDekData,0x18,dekKey,0x10,ET_CBC,ENC,vspDek,&vspDeklen);
	if ( iDataLen ==0xFFFFFFFF)
	{
		return iDataLen;
	}
	//VSPDEK
	memcpy(outenc,vspEnc,vspEnclen);
	memcpy(outdek,vspDek,vspDeklen);
	memcpy(outmac,vspMac,vspMaclen);
	memset(dataforHostCrypt,0x00,8);
	memcpy(dataforHostCrypt+8,seqCounter,2);
	memcpy(dataforHostCrypt+10,cardRandom,6);

	memcpy(dataforHostCrypt+16,input,8);
	dataforHostCrypt[24]=0x80;
	memset(dataforHostCrypt+25,0x00,7);

	iDataLen=calc3DES(dataforHostCrypt,0x20,vspEnc,vspEnclen,ET_CBC,ENC,hdataCrypt,&hdatalen);
	if ( iDataLen ==0xFFFFFFFF)
	{
		return iDataLen;
	}
	memcpy(vhostCrypt,hdataCrypt+16,8);
	dataforMAC[0]=0x84;
	dataforMAC[1]=0x82;
	dataforMAC[2]=level;
	dataforMAC[3]=0x00;
	dataforMAC[4]=0x10;
	memcpy(dataforMAC+5,vhostCrypt,8);
	dataforMAC[13]=0x80;
	dataforMAC[14]=0x00;
	dataforMAC[15]=0x00;
	memset(IV,0x00,8);
	iDataLen=calc3MAC(dataforMAC,16,IV,8, vspMac,vspMaclen,vcMAC,&vcMAClen);
	if ( iDataLen == 0xFFFFFFFF)
	{
		return iDataLen;
	}
	memcpy(dataIn,vhostCrypt,8);
	memcpy(dataIn+8,vcMAC,8);
	memcpy(CMAC,vcMAC,8);

	//ret =sendcommand(handle,ISO_CASE_3,0x84,0x82,level,0x00,input,0x10,0x00,NULL,NULL);
	send_apdu_cmd(0x84,0x82,level,0,0x10,dataIn,-1,ptr,sizeof(ptr),&sta);
	if (sta == 0x9000)
	{
		return 0;
	}
	else
	{
		return 2;
	}
}

//SUCCESS	
#define   E_SUCCESS	                       0x00000000	//OK


unsigned long store_key(unsigned char level,unsigned char*input,unsigned int input_len,unsigned int block_num)
{
	unsigned long ret = 0; 
	unsigned char cla = 0;
	unsigned char p1 = 0;
	unsigned char p2 = 0;
	unsigned char ack[32] = {0};

	//check params
	switch(level)
	{
		case 0x00:
			cla = 0x80;
			break;
		case 0x01:
			cla = 0x84;
			break;
		case 0x02:
			cla = 0x84;
			break;
		case 0x03:
			cla = 0x84;
			break;
		default:
			break;
	}

	if(level>0x03)
	{
		return 0x20000002;
	}
	if(input==NULL)
	{
		return 0x20000003;
	}
	if(input_len<=0)
	{
		return 0x20000004;
	}
	p1 = 0x00;
	p2 = 0x01;

	//ret = store_key_pack(handle,level,input,input_len,block_num);
	//ret =sendcommand(handle,ISO_CASE_3,cla,0xE2,p1,p2,input,input_len,0x00,NULL,NULL);
	send_apdu_cmd(cla,0xE2,p1,p2,input_len,input,-1,ack,32,0);

	if ( ret != E_SUCCESS ) 
	{
		return ret; 
	}

	return  E_SUCCESS;

}

//仅限本地演示使用
/*
* subfunction: storekey
* input params:
*      handle          SE handle
*      level           01-03
*        i             0x15
*      outenc          scp02 process enc key(scp02_test output)
*      outmac          scp02 process mac key(scp02_test output)
*      outdek          scp02 process dek key(scp02_test output)
*      CMAC            scp02 CMAC(scp02_test output)
*      keyinfo         key info or id info refer security storage command data
*      keyinfo_len     key info len
*      block_num       block num

* ruturn: reference errcode.h or  error-0xFFFFFFFF，success-0
*/
unsigned long storekey(unsigned char level,unsigned char i,
	unsigned char* outenc,unsigned char* outmac,unsigned char* outdek,
	unsigned char* CMAC, unsigned char*keyinfo, unsigned int keyinfo_len,
	unsigned int block_num)
{
	/*	
	
		level 0x00:
		call store_key api,update key
		level 0x01:
			a.pad macIn:84E20001lc|keyinfo|8000..,8time length
			b.calculate MAC:3des_mac(macIn,encVMAC)
			c.call store_key api,update key
		level 0x02:
			a.pad encin:00(8bytes IV)|keyinfo|8000...
			b.calculate encdata:3des_enc(encin,outenc)
			c.call store_key api,update key
		level 0x03:
			a.calculate macIn as level 01
			b.calculate encdata as level 02
			c.call store_key api,update key
	
*/
	unsigned char *dataIn = 0;
	unsigned char *macIn = 0;
	unsigned char *encdata = 0;
	unsigned char *encdata1 = 0;
	unsigned char *encin = 0;

	unsigned char *keydata = 0;
	unsigned char *encVMAC = 0;
	unsigned char *MAC = 0;
	unsigned char *MAC1 = 0;
	unsigned char *MAC2 = 0;

	unsigned long ret = 0;
	unsigned int iDataLen = 0;
	unsigned int keylen=0;
	unsigned int encinlen = 0;
	unsigned int encVMAClen = 0;
	unsigned int MAClen=0;
	unsigned int MAClen1=0;
	unsigned int MAClen2=0;
	unsigned int enclen = 0;
	unsigned int enclen1 = 0;
	unsigned int macinlen = 0;
	unsigned int pad = 0;
	unsigned int limitlen = 230;
	unsigned int dataIn_len = 0;
	unsigned char *pbuf = 0;

	pbuf = get_big_mem();
	if (pbuf == 0)
	{
		DBG_Print(LEVEL0,"storekey false.memery error.\r\n");
		return 0;
	}

	dataIn = pbuf + 0;
	macIn = pbuf + 256;
	encdata = pbuf + 512;
	encdata1 = pbuf + 768;
	encin = pbuf + 1024;
	keydata = pbuf + 1024 + 256;
	encVMAC = pbuf + 1024 + 256 + 8;
	MAC     = pbuf + 1024 + 256 + 8 + 16;
	MAC1    = pbuf + 1024 + 256 + 8 + 16 + 8;
	MAC2    = pbuf + 1024 + 256 + 8 + 16 + 8 + 8;

	//chiper data
	if(keyinfo_len<limitlen)
	{
		memset(encin,0x00,8);
		memcpy(encin+8,keyinfo,keyinfo_len);
		//8 times,must padding 80
		if(keyinfo_len%8==0)
		{
			memset(encin+8+keyinfo_len,0x80,1);
			memset(encin+8+keyinfo_len+1,0x00,7);
			encinlen = keyinfo_len+8+8;
		}
		else
		{
			pad = 0;
			memset(encin+8+keyinfo_len,0x80,1);
			while((keyinfo_len+1+pad)%8!=0       )
			{
				memset(encin+8+keyinfo_len+1+pad,0x00,1);
				pad++;
			}
			encinlen = keyinfo_len+8+1+pad;
		}
		iDataLen=calc3DES(encin,encinlen,outenc,16,ET_CBC,ENC,encdata,&enclen);
		if ( iDataLen == 0xFFFFFFFF )
		{
			return iDataLen;
		}
	}
	else{

		memset(encin,0x00,8);
		memcpy(encin+8,keyinfo,limitlen);
		//8 times,must padding 80
		if(limitlen%8==0)
		{
			memset(encin+8+limitlen,0x80,1);
			memset(encin+8+limitlen+1,0x00,7);
			encinlen = limitlen+8+8;
		}
		else
		{
			pad = 0;
			memset(encin+8+limitlen,0x80,1);
			while((limitlen+1+pad)%8!=0       )
			{
				memset(encin+8+limitlen+1+pad,0x00,1);
				pad++;
			}
			encinlen = limitlen+8+1+pad;
		}
		iDataLen=calc3DES(encin,encinlen,outenc,16,ET_CBC,ENC,encdata,&enclen);
		if ( iDataLen == 0xFFFFFFFF)
		{
			return iDataLen;
		}
		memset(encin,0x00,8);
		memcpy(encin+8,keyinfo+limitlen,keyinfo_len-limitlen);
		//8 times,must padding 80
		if((keyinfo_len-limitlen)%8==0)
		{
			memset(encin+8+(keyinfo_len-limitlen),0x80,1);
			memset(encin+8+(keyinfo_len-limitlen)+1,0x00,7);
			encinlen = (keyinfo_len-limitlen)+8+8;
		}
		else
		{
			pad = 0;
			memset(encin+8+(keyinfo_len-limitlen),0x80,1);
			while(((keyinfo_len-limitlen)+1+pad)%8!=0       )
			{
				memset(encin+8+(keyinfo_len-limitlen)+1+pad,0x00,1);
				pad++;
			}
			encinlen = (keyinfo_len-limitlen)+8+1+pad;
		}
		iDataLen=calc3DES(encin,encinlen,outenc,16,ET_CBC,ENC,encdata1,&enclen1);
		if ( iDataLen== 0xFFFFFFFF)
		{
			return iDataLen;
		}


	}

	//MAC
	if(i==0x15){

		//DES encrypt last C-MAC
		iDataLen=calcDES(CMAC,0x08,outmac,0x08,ET_ECB,encVMAC,&encVMAClen);
		if ( iDataLen == 0xFFFFFFFF )
		{
			return iDataLen;
		}
	}
	else if(i==0x05)
	{
		memcpy(encVMAC,CMAC,8);
	}

	if(keyinfo_len<limitlen)
	{
		macIn[0]=0x84;
		macIn[1]=0xE2;
		if(block_num==0)
		{
			 macIn[2]=0x00;
		   macIn[3]=0x01;
		}
	  else
		{
			 macIn[2]=(block_num-1)&0x0F;
			 if((block_num&0x80)==0x00)
			 {
			 		macIn[3]=0x00;
			 }
			 else
			 {
					macIn[3]=0x01;
			 }
		}
		macIn[4]=(keyinfo_len+8)&0xFF;
		memcpy(macIn+5,keyinfo,keyinfo_len);
		//8 times,must padding 80
		if((keyinfo_len+5)%8==0)
		{
			memset(macIn+5+keyinfo_len,0x80,1);
			memset(macIn+5+keyinfo_len+1,0x00,7);
			macinlen = keyinfo_len+5+8;
		}
		else
		{
			pad=0;
			memset(macIn+5+keyinfo_len,0x80,1);
			while((keyinfo_len+5+1+pad)%8!=0)
			{
				memset(macIn+5+keyinfo_len+1+pad,0x00,1);
				pad++;
			}
			macinlen = keyinfo_len+5+1+pad;
		}

		iDataLen=calc3MAC(macIn,macinlen,encVMAC,8, outmac,16,MAC,&MAClen);
		if ( iDataLen == 0xFFFFFFFF)
		{
			return iDataLen;
		}
	}
	else{
		macIn[0]=0x84;
		macIn[1]=0xE2;
		if(block_num==0)
		{
			 macIn[2]=0x00;
		   macIn[3]=0x01;
		}
	  else
		{
			 macIn[2]=(block_num-1)&0x0F;
			 if((block_num&0x80)==0x00)
			 {
			 		macIn[3]=0x00;
			 }
			 else
			 {
					macIn[3]=0x01;
			 }
		}
		macIn[4]=(limitlen+8)&0xFF;
		memcpy(macIn+5,keyinfo,limitlen);
		//8 times,must padding 80
		if((limitlen+5)/8==0)
		{
			memset(macIn+5+limitlen,0x80,1);
			memset(macIn+5+limitlen+1,0x00,7);
			macinlen = limitlen+5+8;
		}
		else
		{
			pad=0;
			memset(macIn+5+limitlen,0x80,1);
			while((limitlen+5+1+pad)%8!=0)
			{
				memset(macIn+5+limitlen+1+pad,0x00,1);
				pad++;
			}
			macinlen = limitlen+5+1+pad;
		}

		iDataLen=calc3MAC(macIn,macinlen,encVMAC,8, outmac,16,MAC1,&MAClen1);
		if ( iDataLen== 0xFFFFFFFF )
		{
			return iDataLen;
		}
		//MAC
		if(i==0x15){

			//DES encrypt last C-MAC
			iDataLen=calcDES(MAC1,0x08,outmac,0x08,ET_ECB,encVMAC,&encVMAClen);
			if ( iDataLen == 0xFFFFFFFF)
			{
				return iDataLen;
			}
		}
		else if(i==0x05)
		{
			memcpy(encVMAC,MAC1,8);
		}
		macIn[0]=0x84;
		macIn[1]=0xE2;
		if(block_num==0)
		{
			 macIn[2]=0x00;
		   macIn[3]=0x01;
		}
	  else
		{
			 macIn[2]=(block_num-1)&0x0F;
			 if((block_num&0x80)==0x00)
			 {
			 		macIn[3]=0x00;
			 }
			 else
			 {
					macIn[3]=0x01;
			 }
		}
		macIn[4]=((keyinfo_len-limitlen)+8)&0xFF;
		memcpy(macIn+5,keyinfo+limitlen,keyinfo_len-limitlen);
		//8 times,must padding 80
		if(((keyinfo_len-limitlen)+5)/8==0)
		{
			memset(macIn+5+(keyinfo_len-limitlen),0x80,1);
			memset(macIn+5+(keyinfo_len-limitlen)+1,0x00,7);
			macinlen = (keyinfo_len-limitlen)+5+8;
		}
		else
		{
			pad=0;
			memset(macIn+5+(keyinfo_len-limitlen),0x80,1);
			while(((keyinfo_len-limitlen)+5+1+pad)%8!=0)
			{
				memset(macIn+5+(keyinfo_len-limitlen)+1+pad,0x00,1);
				pad++;
			}
			macinlen = (keyinfo_len-limitlen)+5+1+pad;
		}

		iDataLen=calc3MAC(macIn,macinlen,encVMAC,8, outmac,16,MAC2,&MAClen2);
		if ( iDataLen ==0xFFFFFFFF )
		{
			return iDataLen;
		}

	}
	

	if(level==0x00)//plain data
	{		
		if(keyinfo_len>255){
			memcpy(dataIn,keyinfo,255);
			ret =store_key(level,dataIn,255,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
			memcpy(dataIn,keyinfo+255,keyinfo_len-255);
			ret =store_key(level,dataIn,keyinfo_len-255,block_num);
			if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
		}
		else{
			memcpy(dataIn,keyinfo,keyinfo_len);
			ret =store_key(level,dataIn,keyinfo_len,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
		}
		
	}
	else if(level==0x01)//plain data+MAC
	{

		if(keyinfo_len>limitlen)
		{

			keylen = limitlen+MAClen1;
			memcpy(keydata,keyinfo,limitlen);
			memcpy(keydata+limitlen,MAC1,MAClen1);
			memcpy(dataIn,keydata,keylen);
			ret =store_key(level,dataIn,keylen,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
			keylen = (keyinfo_len-limitlen)+MAClen2;
			memcpy(keydata,keyinfo+limitlen,keyinfo_len-limitlen);
			memcpy(keydata+(keyinfo_len-limitlen),MAC2,MAClen2);
			memcpy(dataIn,keydata,keylen);
			ret =store_key(level,dataIn,keylen,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
		}
		else{
			keylen = keyinfo_len+MAClen;
			memcpy(keydata,keyinfo,keyinfo_len);
			memcpy(keydata+keyinfo_len,MAC,MAClen);
			memcpy(dataIn,keydata,keylen);
			ret =store_key(level,dataIn,keylen,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
		//update CMAC
		memcpy(CMAC,MAC,8);
		}
	}
	else if(level==0x02)//chiper data
	{
		
		if(keyinfo_len>limitlen)
		{
			memcpy(dataIn,encdata,enclen);
			ret =store_key(level,dataIn,enclen,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }

			memcpy(dataIn,encdata1,enclen1);
			ret =store_key(level,dataIn,enclen1,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }
		}
		else{
			memcpy(dataIn,encdata,enclen);
			ret =store_key(level,dataIn,enclen,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }

		}
	}
	else if(level==0x03)//chiper data+MAC
	{

		if(keyinfo_len>limitlen)
		{
			memcpy(dataIn,encdata,enclen);
			memcpy(dataIn+enclen,MAC1,MAClen1);

			DBG_PrintBuf(LEVEL4,"DataIn31:\r\n",dataIn,enclen+MAClen1);
			ret =store_key(level,dataIn,enclen+MAClen1,block_num);
		    if ( ret != E_SUCCESS )
		    {
			   return ret;
		    }

			memcpy(dataIn,encdata1,enclen1);
			memcpy(dataIn+enclen1,MAC2,MAClen2);
	
			ret =store_key(level,dataIn,enclen1+MAClen2,block_num);
		    if ( ret != E_SUCCESS )
		    {

			   return ret;
		    }
			//update CMAC
		  memcpy(CMAC,MAC2,8);

		}
		else{
	
			memcpy(dataIn,encdata,enclen);
			memcpy(dataIn+enclen,MAC,MAClen);
			dataIn_len = enclen+MAClen;	
			DBG_PrintBuf(0,"DataIn30:\r\n",dataIn,dataIn_len);

			store_key(level,dataIn,dataIn_len,block_num);
			//update CMAC
			memcpy(CMAC,MAC,8);
		}
	}

	free_big_mem(pbuf);

	return 0;
}

int _hdsm_write_public_private_key(unsigned char kid,void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	unsigned char outenc[16]={0};
	unsigned char outdek[16]={0};
	unsigned char outmac[16]={0};
	unsigned char CMAC[8] = {0};
	unsigned char *ptr = 0;
	unsigned int rlen = 0;

	if ((public_key == 0) || (pb_len == 0))
	{
		return 1;
	}

	if (scp02_auth(outenc,outmac,outdek,CMAC))
	{
		DBG_Print(0,"scp02_auth false.\r\n");
		return 2;
	}

	ptr = get_small_mem();
	if (ptr == 0)
	{
		return 3;
	}

	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x04;	//SM2
	ptr[rlen++] = kid;	//KID	

	ptr[rlen++] = 0x60;
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x20;
	memcpy(ptr+rlen,public_key,0x20);
	rlen += 0x20;

	ptr[rlen++] = 0x61;
	ptr[rlen++] = 0x00;
	ptr[rlen++] = 0x20;
	memcpy(ptr+rlen,(unsigned char *)public_key + 0x20,0x20);
	rlen += 0x20;

	storekey(3,0x15,outenc,outmac,outdek,CMAC,
	ptr,rlen,0);
	free_small_mem(ptr);

	return 0;
}
	


//公私钥对有A B C三组 写入公私钥对A
extern int hdsm_write_public_private_key(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hdsm_write_public_private_key(0x0F,public_key,pb_len,private_key,pv_len);
}

//写入公私钥对B
int hdsm_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hdsm_write_public_private_key(0x1,public_key,pb_len,private_key,pv_len);
}

//写入公私钥对C

int hdsm_write_public_private_keyC(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	return _hdsm_write_public_private_key(0x3,public_key,pb_len,private_key,pv_len);
}

int hdse_test_ex(void)
{
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *ptr = 0;
	unsigned char *ptemp = 0;
	unsigned char pk[] = 
	{
		0xFF,0x3B,0xD4,0x7E,0x0D,0x59,0xAE,0xCB,0x4F,0x79,0xB5,0x02,0xAE,0x3E,0xB8,0xC4,0xFE,0xA0,0xBF,0xDE,0x1E,0x11,0x9F,0x83,0xC1,0x97,0x26,0xE8,0x68,0xCB,0xE0,0xE6,0x3D,0x69,0x57,0xA2,0x7F,0xF6,0x62,0x3A,0x79,0xEF,0x79,0x2A,0xCD,0xC8,0x77,0x97,0xFC,0x70,0x01,0xF0,0x32,0x9A,0x9D,0x23,0x77,0x61,0x9D,0x33,0xDE,0xF1,0xEA,0xCF
	};

	ptr = (unsigned char *)get_big_mem();
	ptemp = (unsigned char *)get_big_mem();

	set_sm2_ver(SM2_VER2);

	rlen = hdsm_read_public_key(ptr,1024);
	DBG_PrintBuf(0,"[app]read key:\r\n",ptr,rlen);

	rlen = hdsm_private_key_decrypt_encrypt(1,"123",3,ptr,1024);
	DBG_PrintBuf(0,"encrypt data:\r\n",ptr,rlen);

	slen = hdsm_private_key_decrypt_encrypt(0,ptr,rlen,ptemp,1024);
	DBG_PrintBuf(0,"decrypt data:\r\n",ptemp,slen);

	DBG_Print(0,"write key:\r\n");
	hdsm_write_public_private_keyB(pk,64,0,0);

	rlen = hdsm_read_public_keyB(ptr,1024);
	DBG_PrintBuf(0,"read keyB:\r\n",ptr,rlen);

	rlen = hdsm_private_key_decrypt_encryptB(1,"333",3,ptr,1024);
	DBG_PrintBuf(0,"keyB加密:\r\n",ptr,rlen);
	
	free_big_mem(ptr);
	free_big_mem(ptemp);
	return 0;
}


int hdse_test(void)
{
	unsigned char tempbuf[128] = {0};
	unsigned int rlen = 0;
	unsigned int slen = 0;
	unsigned char *ptr = 0;
	unsigned char *ptemp = 0;
	int ret = 0;
	unsigned char pbk[] = 
	{
		0x86,0x30,0x3C,0xA1,0x69,0x4B,0xC2,0xF4,0xD2,0x71,0xC8,0xE9,0x43,0x8F,0xA9,0x40,0xDC,0xD0,0x86,0x6B,0x2A,0x0D,0x8E,0x45,0xDE,0xFC,0x6A,0xAC,0x9F,0x31,0xAB,0x56,0x8E,0xC7,0x6D,0x33,0x4D,0xE7,0xB2,0xCC,0xC7,0xB3,0xD8,0xD2,0xF5,0x1F,0xA4,0xA5,0x57,0xD4,0xE1,0x7A,0x90,0x06,0xFC,0x77,0x9E,0x97,0xC9,0x77,0x31,0x56,0x2D,0xF2

	};
	ptr = get_big_mem();
	ptemp = get_big_mem();
	if ((ptr == 0) || (ptemp == 0))
	{
		free_big_mem(ptr);
		free_big_mem(ptemp);

		return 0;
	}

	hdse_reset();

	//发送复位请求帧030003D300D3 接收应答帧030003D300D3
	rlen = _hdse_send_data(PIB_ACTIVE_FRAME,"\xD3\x00",2,ptr,1024);
	DBG_PrintBuf(0,"hdse RESET ack:",ptr,rlen);

	//发送RATR请求帧
	rlen = _hdse_send_data(PIB_ACTIVE_FRAME,"\xE2\x01",2,ptr,1024);
	DBG_PrintBuf(0,"hdse RATR ack:",ptr,rlen);

	do
	{
		rlen = hdsm_read_public_key(ptr,get_big_mem_size());
		if (rlen)
		{
			DBG_PrintBuf(LEVEL0,"公钥Key:\r\n",ptr,rlen);
		}
		else
		{
			ret = 0;
			break;
		}


		hdsm_write_public_private_keyB(pbk,64,0,0);

		rlen = hdsm_read_public_keyB(ptr,1024);
		DBG_PrintBuf(0,"public_keyB:\r\n",ptr,rlen);
		rlen = hdsm_read_sign_data_with_z("1234567812345678",16,ptr,64,
		"123",3,
		tempbuf,sizeof(tempbuf));
		if (rlen)
		{

		}
		else
		{
			DBG_Print(LEVEL0,"hdse error.\r\n");
			ret = 0;
			break;
		}

		//加密
		rlen = hdsm_private_key_decrypt_encrypt(1,
		"hello",
		5,ptemp,get_big_mem_size());
		if (rlen)
		{
			DBG_PrintBuf(0,"hdsm encrypt:\r\n",ptemp,rlen);
		}
		else
		{
			ret = 0;
			break;
		}

		//解密
		memset(ptr,0,get_big_mem_size());
		slen = hdsm_private_key_decrypt_encrypt(0,ptemp,rlen,ptr,get_big_mem_size());

		if (slen && (ptr[0] == 'h') && (ptr[1] == 'e') && (ptr[2] == 'l') & (ptr[3] == 'l') & (ptr[4] == 'o'))
		{
		}
		else
		{
			ret = 0;
			break;
		}
		ret = 1;
	}while (0);

	free_big_mem(ptr);
	free_big_mem(ptemp);
	return ret;
}



















