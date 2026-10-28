#ifndef __HHSM_H__
#define __HHSM_H__

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************
** 回调函数定义
**************************************************************************/
typedef void (*SPIInitFunc)(void);
typedef void (*SPIWriteFunc)(void *pdat,unsigned int len);
typedef unsigned int (*SPIReadFunc)(unsigned int iWantToReadBytes,void *pOutBuf,unsigned int outbufsize);

typedef void (*CE_SetHigFunc)(void);
typedef void (*CE_SetLowFunc)(void);
typedef void (*DelayMsFunc)(unsigned int ms);
typedef int (*IsHHSMBusy)(void);

/**************************************************************************
** 函数声明
**************************************************************************/
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
					  IsHHSMBusy _is_hhsm_busy);


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
int hhsm_read_chip_id(u8 *data, int maxlen);

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
int hhsm_read_public_key(u8 *data, int maxlen);

extern int hhsm_read_public_keyB(u8 *data, int maxlen);
extern int hhsm_read_public_keyC(u8 *data, int maxlen);


//公私钥对有A B C三组 写入公私钥对A
extern int hhsm_write_public_private_key(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);

//写入公私钥对B
extern int hhsm_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);
	

//写入公私钥对C
extern int hhsm_write_public_private_keyC(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);




/**************************************************************************
* 函  数: int hhsm_read_sign_data(u8 *data, u32 len, u8 *outdata, int maxlen)
* 描  述: 读取签名数据
* 举  例: 	
	M->S 802500 00 22 1000 7de6843fcd2dd2b75912e8b5d396e78aff58a10ee3d4dd285862720c0772909b
	M<-S xxxxxxxxx9000
	
* 入  参: u8 data:输入需要前面的数据
*		  u32 len:长度
*		  int maxlen:得到签名数据的最大允许长度
* 出  参: u8 *outdata：得到签名数据
* 返回值: int ， -1 : 失败， > 0 芯片ID的长度
**************************************************************************/
int hhsm_read_sign_data(u8 *data, u32 len, u8 *outdata, int maxlen);


//带Z值的签名 返回值:小于0表示失败 其中-66 表示传入的tempbuff缓冲区太小
//返回值大于零 表示 输出签名的长度
int hhsm_read_sign_data_with_z(void *chip_id,unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *dat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen);


//使用A组公私钥对加解密
extern int hhsm_private_key_decrypt_encrypt(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen);

//使用B组公私钥对加解密
extern int hhsm_private_key_decrypt_encryptB(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen);

//使用C组公私钥对加解密
extern int hhsm_private_key_decrypt_encryptC(u8 type, u8 *data, u32 len, u8 *outdata, int maxlen);


//0成功 非零失败
int hhsm_test(void);




extern int hhsm_read_calc_hash(u8 *data, u32 len, u8 *outdata, int maxlen);


#ifdef __cplusplus
}
#endif
#endif


