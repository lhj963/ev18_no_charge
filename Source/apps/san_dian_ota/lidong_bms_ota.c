#include "lidong_bms_ota.h"

CAN_BUS_OTA stldbms = {0};

unsigned int ld_bms_seedToKey(unsigned int gs_UdsDemoSeed)
{
	unsigned char Seed[4];
	unsigned char Key[4];
	unsigned char xorArray[4] = {0xAD,0xE2,0x25,0x52};
	unsigned int NewKey;

	Seed[0] = (gs_UdsDemoSeed >> 24);
	Seed[1] = (gs_UdsDemoSeed >> 16);
	Seed[2] = (gs_UdsDemoSeed >> 8);
	Seed[3] = gs_UdsDemoSeed;

	Key[3] = (((Seed[0] ^ xorArray[0]) & 0x0F) << 4) | ((Seed[2] ^ xorArray[2]) & 0x0F);
	Key[2] = ((Seed[1] ^ xorArray[1]) & 0xF0) | (((Seed[2] ^ xorArray[2]) & 0xF0 >> 4));
	Key[1] = (((Seed[1] ^ xorArray[1]) & 0x0F) << 4) | (((Seed[0] ^ xorArray[0]) >> 4));
	Key[0] = (((Seed[3] ^ xorArray[3]) & 0x0F) << 4) | ((Seed[3] ^ xorArray[3]) & 0xF0);

	NewKey = ((Key[0] & 0xff) << 24) + ((Key[1] & 0xff) << 16) + ((Key[2] & 0xff) << 8) + ((Key[3] & 0xff));
	return NewKey;
}


unsigned int ld_bms_seedToKey1(unsigned int gs_UdsDemoSeed)
{
	return 0xE05F7888;
}


unsigned int ComputeKey1(unsigned int seed) 
{ 
	 unsigned char key[4]; 
	 unsigned int retKey; 
	 unsigned char i=0,j=0; 
	 unsigned char result=0x00; 
	 union 
	 { 
	 	char byte[4]; 
	 	unsigned int wort; 
	 }seedlokal; 

	 seedlokal.wort=seed; 
	 if(seedlokal.byte[0]==0&&seedlokal.byte[1]==0) 
	 { 
	 /* Do Nothing*/ 

	 } 
	 else
	 { 
		 for(i=0;i<40;i++) 
		 { 
			 if(seedlokal.wort&0x80000000) 
			 { 
				 seedlokal.wort=seedlokal.wort<<1; 
				 seedlokal.wort=seedlokal.wort^0xAAAAAAAA; 
			 } 
			 else
			 { 
				 seedlokal.wort=seedlokal.wort<<1; 
			 } 
			 
			 } 
			 for(i=0;i<4;i++) 
			 { 
				result=0x00; 
				j=0; 
				for(;j<8;j++) 
				{ 
				if(j<4) 
				{ 
				result|=(seedlokal.byte[i]& (0x01 << (7-j) ) ) >> (7-2*j) ; 
				} 
				else{ 
				result|=(seedlokal.byte[i]& (0x01 << (7-j) ) ) << (2*j-7) ; 
				} 
			 } 
			 key[3-i]=result; 
		 } 
	 } 
	 retKey=((unsigned int)key[3] << 24)+((unsigned int)key[2]<<16)+((unsigned int)key[1]<<8)+(unsigned int)key[0]; 
	 return retKey;
} 


void lidong_bms_ota_proc(char channel,CanRxMsg *pMsg)
{
	stldbms.phy_id = 0x7C0;
	stldbms.response_id = 0x7C8;

	stldbms.p_func_calc_key = (unsigned int (*)(unsigned int))ld_bms_seedToKey;

	can_bus_ota_proc(&stldbms, channel,pMsg);
}


void sandian_bms1_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len)
{
	sandian_start_ota_step(&stldbms,step,drv_addr,drv_len,app_addr,app_len);
}

void sandian_bms1_start_ota_step1(unsigned int step,FileBlockInfo *drv_blk,unsigned int drv_blk_cnt,FileBlockInfo *app_blk,unsigned int app_blk_cnt)
{
	sandian_start_ota_step1(&stldbms,step,drv_blk,drv_blk_cnt,app_blk,app_blk_cnt);
}


void sandian_bms1_data_arrive(unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen)
{
	sandian_data_arrive(&stldbms,file_type,offset,pdat,datlen);
}

void sandian_bms1_restart_ota(void)
{
	sandian_restart_ota(&stldbms);
}


unsigned int get_sandian_bms1_ota_progress(void)
{
	return can_bus_get_sandian_ota_progress(&stldbms);
}


unsigned int get_sandian_bms1_ota_step(void)
{
	return stldbms.can_ota_step;
}


