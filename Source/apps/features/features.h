#ifndef __FEATURES_H__
#define __FEATURES_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SEND_DATA_SERVER_NUM   0

typedef struct tagSendServerDataBuf
{
	void *ptr;
	unsigned int len;
}SendServerDataBuf;

extern void features_init(void);
extern void features_proc(void);


extern void send_data_to_server(void);



extern char is_car_off(void);





#ifdef __cplusplus
}
#endif

#endif

