#ifndef __SM2_CHIP_H__
#define __SM2_CHIP_H__
#ifdef __cplusplus
extern "C" {
#endif

extern void sm2_chip_init(void);

extern unsigned int sm2_chip_read_public_key(void *pOut,unsigned int outbuflen);
extern unsigned int sm2_chip_read_public_keyB(void *pOut,unsigned int outbuflen);


extern unsigned int sm2_chip_read_sign_data_with_z(void *chip_id,
							unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *pdat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen);

//写入公私钥对B
extern int sm2_chip_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len);


//使用B组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
extern unsigned int sm2_chip_private_key_decrypt_encryptB(unsigned char type,
				void *data, unsigned int len, void *outdata,unsigned int maxlen);







#ifdef __cplusplus
}
#endif

#endif

