#ifndef __HUADAJIAMI_H__
#define __HUADAJIAMI_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum tagSM2EncryVer
{
	SM2_VER1 = 0,	//VER1 是C1C2C3排序的加密方式
	SM2_VER2,		//VER2 是C1C3C2的排序加密方式
}SM2EncryVer;

typedef void (*HDSE_SETGPIO0)(unsigned char);
typedef void (*HDSE_DELAYMS)(unsigned int);

typedef void (*HDSE_SPI_WRITE)(void *,unsigned int);
typedef unsigned int (*HDSE_SPI_READ)(unsigned int iWantToReadBytes,void *pOutBuf,unsigned int outbuflen);



extern void hdse_init(HDSE_SETGPIO0 pSetGpio0,
					HDSE_DELAYMS pDelayms,
					HDSE_SPI_WRITE pWrite,
					HDSE_SPI_READ pRead);


extern int hdse_test(void);
extern int hdse_test_ex(void);

extern unsigned int hdsm_read_public_key(void *pOut,unsigned int outbuflen);

extern unsigned int hdsm_read_public_keyB(void *pOut,unsigned int outbuflen);

extern unsigned int hdsm_read_public_keyC(void *pOut,unsigned int outbuflen);



//公私钥对有A B C三组 写入公私钥对A
extern int hdsm_write_public_private_key(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);

//写入公私钥对B
int hdsm_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);

//写入公私钥对C

int hdsm_write_public_private_keyC(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);



extern unsigned int hdsm_read_sign_data(void *pdat,unsigned int datlen,void *pOut,unsigned int outbuflen);

//带Z值的签名 返回值:小于0表示失败 其中-66 表示传入的tempbuff缓冲区太小
//返回值大于零 表示 输出签名的长度
extern unsigned int hdsm_read_sign_data_with_z(void *chip_id,
							unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *pdat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen);

//使用A组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
extern unsigned int hdsm_private_key_decrypt_encrypt(unsigned char type, 
		void *data, unsigned int len, void *outdata, unsigned int maxlen);

//使用B组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
extern unsigned int hdsm_private_key_decrypt_encryptB(unsigned char type, 
		void *data,unsigned int len,void *outdata,unsigned int maxlen);

//使用C组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
extern unsigned int hdsm_private_key_decrypt_encryptC(unsigned char type,
						void *data, unsigned int len,void *outdata,unsigned int maxlen);



extern SM2EncryVer get_sm2_ver(void);
extern void set_sm2_ver(SM2EncryVer ver);


#ifdef __cplusplus
}
#endif

#endif


