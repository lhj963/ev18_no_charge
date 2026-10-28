#ifndef __BACK_PROTOCOL_H__
#define __BACK_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif



extern unsigned int bp_pack(unsigned short msgid,void *psn,unsigned int sn_len,
								void *content,int contentlen,
								void *pOutBuf,unsigned int outbuflen);

/* ½â°ü */
extern int bp_unpack(void *pdat,unsigned int len,
	void (*pCallBackAnalyze)(unsigned int msgid,void *psn,unsigned int sn_len,void *content,unsigned int contentlen),
	void (*pCallBackError)(void *pdat,unsigned int len,int iErrorCode));


#ifdef __cplusplus
}
#endif
#endif


