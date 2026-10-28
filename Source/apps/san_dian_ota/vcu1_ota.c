#include "vcu1_ota.h"

//Poly 	: 0x04C11DB7
//Init 	: 0xFFFFFFFF
//RefIn	: True
//RefOut	: True
//XorOut  : 0xFFFFFFFF

// 使用说明：先初始化，再调用计算函数，最后异或输出结果
// 查表
const unsigned int CRC32TABLE1[] = 
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,\
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,\
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,\
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,\
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,\
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,\
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,\
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,\
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,\
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,\
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,\
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,\
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,\
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,\
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,\
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,\
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,\
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,\
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,\
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,\
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,\
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,\
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,\
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,\
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,\
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,\
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,\
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,\
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,\
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,\
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,\
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D\
};

unsigned int gs_bzip2CrcRemainder1;


#define CRC32_BZIP2_INIT_VALUE1              (0xFFFFFFFFUL)
#define CRC32_BZIP2_FINAL_XOR_VALUE1         (0xFFFFFFFFUL)



#define CRC04C11DB7CacRefInAndOut1(buf) _crc32 = CRC32TABLE1[(_crc32 ^ (*buf++)) & 0xff] ^ (_crc32 >> 8)

#define CRC_Cac_UnifyCfg1(buf)         CRC04C11DB7CacRefInAndOut1(buf)

#define _DO1(buf)  CRC_Cac_UnifyCfg1(buf);
#define _DO2(buf)  _DO1(buf) _DO1(buf)
#define _DO4(buf)  _DO2(buf) _DO2(buf)
#define _DO8(buf)  _DO4(buf) _DO4(buf)



// 计算CRC
void Bzip2_Update1(unsigned int size, unsigned char *data)
{
    unsigned int _crc32 = gs_bzip2CrcRemainder1;

	while (size >= 8)
	{
		_DO8(data)
		size -= 8;
	}
	
	if (size>0)
	{
		do
		{
			_DO1(data)
		}while(--size);
	}

    gs_bzip2CrcRemainder1 = _crc32;
}



// 初始化
void Bzip2_Start1(void)
{
    gs_bzip2CrcRemainder1 = CRC32_BZIP2_INIT_VALUE1;
}


// 异或结果
unsigned int Bzip2_XOR1(void)
{
  return gs_bzip2CrcRemainder1 ^ CRC32_BZIP2_FINAL_XOR_VALUE1;
}



/******************************************************************************************************
 * 安全认证算法
 * mask_UnifyCfg                              ：安全算法掩码
 * _Acc_UdsDemoCalculateKey_UnifyCfg(void)    ：安全算法
 ******************************************************************************************************/

#define MASK_UNIFYCFG1  0xD9410702						// TM
unsigned long _Acc_UdsDemoCalculateKey_UnifyCfg1(unsigned long seed)
{
    unsigned short key[4];
    unsigned short data[4];
    unsigned short i;
    unsigned long seedlokalwort;
    seedlokalwort  = seed;

    for (i=0; i<40; i++)
    {
        if(seedlokalwort&0x80000000)
        {
            seedlokalwort= seedlokalwort<<1;
            seedlokalwort= seedlokalwort^MASK_UNIFYCFG1;
        }
        else
        {
            seedlokalwort= seedlokalwort<<1;
        }
    }
    data[0] = (seedlokalwort>>24) & 0xff;
    data[1] = (seedlokalwort>>16) & 0xff;
    data[2] = (seedlokalwort>>8) & 0xff;
    data[3] = (seedlokalwort>>0) & 0xff;

    for(i=0;i<4;i++)
    {
        data[i] = ((data[i] & 0x0F) << 4) + (data[i] >> 4 & 0x0F);
        data[i] = ((data[i] & 0x33) << 2) + (data[i] >> 2 & 0x33);
        data[i] = ((data[i] & 0x55) << 1) + (data[i]>> 1 & 0x55);
        key[3-i] = (data[i] & 0x00ff);
    }

    seedlokalwort = (unsigned long)( (((unsigned long)(key)[0] & 0xFFUL) << 24u)\
                                 | (((unsigned long)(key)[1] & 0xFFUL) << 16u)\
                                 | (((unsigned long)(key)[2] & 0xFFUL) << 8u)\
                                 | (((unsigned long)(key)[3] & 0xFFUL) << 0u));
    return (seedlokalwort);
}


unsigned long SeedToKey(unsigned int num_rounds,unsigned long seed)
{
	unsigned long seedTemp1 = seed;
	unsigned long seedTemp2 = ~seed;
	unsigned long i = 0;
	unsigned long sum = 0,delta = 0x0ab16000;
	unsigned long k[4],key;

	k[0] = 0x4c495551;
	k[1] = 0x49534545;
	k[2] = 0x444b4559;
	k[3] = 0x414c474f;

	for (i = 0; i < num_rounds; i++)
	{
		seedTemp1 += (((seedTemp2 << 4) ^ (seedTemp2 >> 5)) + seedTemp2 ) ^ (sum + k[sum & 3]);
		sum += delta;
		seedTemp2 += (((seedTemp1 << 4) ^ (seedTemp1 >> 5)) + seedTemp1) ^ (sum + k[(sum >> 11)&3]);
	}
	key = seedTemp1;
	return key;
}


typedef enum tagCAN_VCU_OTA_STEP
{
	CAN_VCU_OTA_NONE = 0,

	CAN_VCU_OTA_STEP_1,
	CAN_VCU_OTA_STEP_1_WAIT_ACK,

	CAN_VCU_OTA_STEP_2,
	CAN_VCU_OTA_STEP_2_WAIT_ACK,


	CAN_VCU_OTA_STEP_3,
	CAN_VCU_OTA_STEP_3_WAIT_ACK,


	CAN_VCU_OTA_STEP_4,
	CAN_VCU_OTA_STEP_4_WAIT_ACK,


	CAN_VCU_OTA_STEP_5,
	CAN_VCU_OTA_STEP_5_WAIT_ACK,


	CAN_VCU_OTA_STEP_6,
	CAN_VCU_OTA_STEP_6_WAIT_ACK,


	CAN_VCU_OTA_STEP_7,
	CAN_VCU_OTA_STEP_7_WAIT_ACK,


	CAN_VCU_OTA_STEP_8,
	CAN_VCU_OTA_STEP_8_WAIT_ACK,

	CAN_VCU_OTA_STEP_9,
	CAN_VCU_OTA_STEP_9_WAIT_ACK,

	CAN_VCU_OTA_STEP_10,
	CAN_VCU_OTA_STEP_10_WAIT_ACK,

	CAN_VCU_OTA_STEP_11,
	CAN_VCU_OTA_STEP_11_WAIT_ACK,

	CAN_VCU_OTA_STEP_12,
	CAN_VCU_OTA_STEP_12_WAIT_ACK,

	CAN_VCU_OTA_STEP_13,
	CAN_VCU_OTA_STEP_13_WAIT_ACK,

	CAN_VCU_OTA_STEP_14,
	CAN_VCU_OTA_STEP_14_WAIT_ACK,

	CAN_VCU_OTA_STEP_15,
	CAN_VCU_OTA_STEP_15_WAIT_ACK,

	CAN_VCU_OTA_STEP_16,
	CAN_VCU_OTA_STEP_16_WAIT_ACK,

	CAN_VCU_OTA_STEP_17,
	CAN_VCU_OTA_STEP_17_WAIT_ACK,

	CAN_VCU_OTA_STEP_18,
	CAN_VCU_OTA_STEP_18_WAIT_ACK,


}CAN_VCU_OTA_STEP;


typedef struct tagVCU1OTA
{
	unsigned char channel;					//CAN通道
	CAN_VCU_OTA_STEP mcu_ota_step;			//当前的升级步骤
	unsigned long seed;						//安全访问的种子

	unsigned char next_drv_bag_flag : 1;
	unsigned char next_app_bag_flag : 1;
	unsigned char drv_data_arrive_flag : 1;
	unsigned char app_data_arrive_flag : 1;

	unsigned int last_send_cmd_time;

	unsigned int drv_check_sum;				//驱动文件checksum
	unsigned int app_check_sum;				//app文件checksum

	unsigned int drv_offset;
	unsigned int app_offset;
	unsigned int drv_tx_len;
	unsigned int app_tx_len;

	unsigned int drv_cur_tx_len;
	unsigned int app_cur_tx_len;

	unsigned int drv_addr;					//驱动地址
	unsigned int drv_len;
	unsigned int drv_bag_id;

	unsigned int app_addr;					//app地址
	unsigned int app_len;
	unsigned int app_bag_id;

	unsigned int transfer_max_len;			//一次最大传输长度

	unsigned char buf[512];
}VCU1OTA;

VCU1OTA s_stVCU1OTA = {0};


void sandian_vcu1_data_arrive(unsigned int file_type,unsigned int offset,void *pdat,unsigned int datlen)
{
	Bzip2_Update1(datlen,pdat);

	if (file_type == 21)
	{
		s_stVCU1OTA.drv_data_arrive_flag = 1;
	}
	else if (file_type == 22)
	{
		s_stVCU1OTA.app_data_arrive_flag = 1;
	}

	memcpy(s_stVCU1OTA.buf,pdat,(datlen >= sizeof(s_stVCU1OTA.buf)) ? sizeof(s_stVCU1OTA.buf) : datlen);
}


unsigned int get_sandian_vcu1_ota_step(void)
{
	return (unsigned int)(s_stVCU1OTA.mcu_ota_step);
}

unsigned int get_sandian_vcu1_ota_progress(void)
{
	if (get_sandian_vcu1_ota_step() <= CAN_VCU_OTA_NONE) return 0;

	if ((s_stVCU1OTA.drv_len + s_stVCU1OTA.app_len) > 0)
	{
		return ((s_stVCU1OTA.drv_tx_len + s_stVCU1OTA.app_tx_len) * 100) / (s_stVCU1OTA.drv_len + s_stVCU1OTA.app_len);
	}
	else
	{
		return 0;
	}
}

void sandian_vcu1_start_ota_step(unsigned int step,unsigned int drv_addr,unsigned int drv_len,unsigned int app_addr,unsigned int app_len)
{
	memset(&s_stVCU1OTA,0,sizeof(s_stVCU1OTA));

	s_stVCU1OTA.mcu_ota_step = (CAN_VCU_OTA_STEP)step;
	if (s_stVCU1OTA.mcu_ota_step == CAN_VCU_OTA_NONE) sandian_set_update_status(SANDIAN_UPDATE_STATUS_NONE);

	s_stVCU1OTA.drv_addr = drv_addr;
	s_stVCU1OTA.drv_len = drv_len;
	s_stVCU1OTA.app_addr = app_addr;
	s_stVCU1OTA.app_len = app_len;
}

void sandian_vcu1_restart_ota(void)
{
	unsigned int drv_addr = 0;
	unsigned int drv_len = 0;
	unsigned int app_addr = 0;
	unsigned int app_len = 0;

	drv_addr = s_stVCU1OTA.drv_addr;
	drv_len = s_stVCU1OTA.drv_len;
	app_addr = s_stVCU1OTA.app_addr;
	app_len = s_stVCU1OTA.app_len;	
	memset(&s_stVCU1OTA,0,sizeof(s_stVCU1OTA));
	s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_1;
	s_stVCU1OTA.drv_addr = drv_addr;
	s_stVCU1OTA.drv_len = drv_len;
	s_stVCU1OTA.app_addr = app_addr;
	s_stVCU1OTA.app_len = app_len; 
}


void vcu1_transfer_file_proc(CanRxMsg *pMsg,unsigned int file_type,CAN_VCU_OTA_STEP next_step)
{
	unsigned int canid = 0;
	unsigned int templen = 0;
	unsigned char dat[8] = {0};

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}


	if (file_type == 21)						//drv 下载
	{
		if (s_stVCU1OTA.drv_len == 0)
		{
			s_stVCU1OTA.mcu_ota_step = next_step;
			return ;
		}

		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x04) && (pMsg->Data[1] == 0x74) && (pMsg->Data[2] == 0x20))
		{
			s_stVCU1OTA.transfer_max_len = (pMsg->Data[3] << 8) + pMsg->Data[4];
			DBG_Print(LEVEL2,"onece max transfer len:%uByes\r\n",s_stVCU1OTA.transfer_max_len);		//MCU响应数据一次最大传输长度
		
			if (s_stVCU1OTA.transfer_max_len >= 512)
			{
				s_stVCU1OTA.transfer_max_len = 512;
			}
			else if (s_stVCU1OTA.transfer_max_len >= 256)
			{
				s_stVCU1OTA.transfer_max_len = 256;
			}
			else
			{
				s_stVCU1OTA.transfer_max_len = 128;
			}

			s_stVCU1OTA.next_drv_bag_flag = 1;
			send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(s_stVCU1OTA.drv_offset),(void *)(s_stVCU1OTA.transfer_max_len));
			s_stVCU1OTA.drv_offset += s_stVCU1OTA.transfer_max_len;
		}
		
		if (s_stVCU1OTA.transfer_max_len && (s_stVCU1OTA.drv_tx_len < s_stVCU1OTA.drv_len))
		{
			unsigned int i = 0;
		
			if ((canid == 0x7EF) && (pMsg->Data[1] == 0x76))
			{
				s_stVCU1OTA.next_drv_bag_flag = 1;
		
				s_stVCU1OTA.drv_tx_len += s_stVCU1OTA.drv_cur_tx_len;
		
				if (s_stVCU1OTA.drv_tx_len >= s_stVCU1OTA.drv_len)											//传输完毕 退出
				{
					sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x01\x37\xFF\xFF\xFF\xFF\xFF\xFF",8);
		
					s_stVCU1OTA.mcu_ota_step = next_step;
		
					DBG_Print(LEVEL2,"drv transfer finish\r\n");
					s_stVCU1OTA.next_drv_bag_flag = 0;
					s_stVCU1OTA.transfer_max_len = 0;
				}
			}
		
			if (s_stVCU1OTA.next_drv_bag_flag && s_stVCU1OTA.drv_data_arrive_flag)
			{
				s_stVCU1OTA.next_drv_bag_flag = 0;
		
				if ((s_stVCU1OTA.drv_len - s_stVCU1OTA.drv_tx_len) <= 5)
				{
					s_stVCU1OTA.drv_cur_tx_len = s_stVCU1OTA.drv_len - s_stVCU1OTA.drv_tx_len;
		
					dat[0] = s_stVCU1OTA.drv_cur_tx_len + 2;
					dat[1] = 0x36;
					dat[2] = 0x01 + s_stVCU1OTA.drv_bag_id;
					
					dat[3] = s_stVCU1OTA.buf[0];								//data										
					dat[4] = s_stVCU1OTA.buf[1];
					dat[5] = s_stVCU1OTA.buf[2];
					dat[6] = s_stVCU1OTA.buf[3];
					dat[7] = s_stVCU1OTA.buf[4];
				}
				else
				{
					if ((s_stVCU1OTA.drv_len - s_stVCU1OTA.drv_tx_len) > s_stVCU1OTA.transfer_max_len)
					{
						s_stVCU1OTA.drv_cur_tx_len = s_stVCU1OTA.transfer_max_len;
					}
					else
					{
						s_stVCU1OTA.drv_cur_tx_len = s_stVCU1OTA.drv_len - s_stVCU1OTA.drv_tx_len;
					}
					
					dat[0] = ((s_stVCU1OTA.drv_cur_tx_len + 2) >> 8) | 0x10;
					dat[1] = s_stVCU1OTA.drv_cur_tx_len + 2;
					dat[2] = 0x36;
					dat[3] = 0x01 + s_stVCU1OTA.drv_bag_id;
		
					dat[4] = s_stVCU1OTA.buf[0];								//data										
					dat[5] = s_stVCU1OTA.buf[1];
					dat[6] = s_stVCU1OTA.buf[2];
					dat[7] = s_stVCU1OTA.buf[3];
				}
		
				sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);
				s_stVCU1OTA.drv_bag_id++;
			}
		
			if ((canid == 0x7EF) && ((pMsg->Data[0] & 0xF0) == 0x30))
			{
				templen = 4;
				i = 0;
				while (templen < s_stVCU1OTA.drv_cur_tx_len)
				{
					os_sleep(2);
					dat[0] = 0x20 + ((i+1) % 16);
					dat[1] = s_stVCU1OTA.buf[4 + (i * 7) + 0];
					dat[2] = s_stVCU1OTA.buf[4 + (i * 7) + 1];
					dat[3] = s_stVCU1OTA.buf[4 + (i * 7) + 2];
					dat[4] = s_stVCU1OTA.buf[4 + (i * 7) + 3]; 									
					dat[5] = s_stVCU1OTA.buf[4 + (i * 7) + 4];
					dat[6] = s_stVCU1OTA.buf[4 + (i * 7) + 5];
					dat[7] = s_stVCU1OTA.buf[4 + (i * 7) + 6];
		
					sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);
		
					templen += 7;
					i++;

					if ((i % 16) == 0) os_sleep(5);
				}
				
				s_stVCU1OTA.drv_data_arrive_flag = 0;
				send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(s_stVCU1OTA.drv_offset),(void *)(s_stVCU1OTA.transfer_max_len));
				s_stVCU1OTA.drv_offset += s_stVCU1OTA.transfer_max_len;
			}
		}

	}
	else if (file_type == 22)
	{
		if (s_stVCU1OTA.app_len == 0)
		{
			s_stVCU1OTA.mcu_ota_step = next_step;
			return ;
		}

		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x04) && (pMsg->Data[1] == 0x74) && (pMsg->Data[2] == 0x20))
		{
			s_stVCU1OTA.transfer_max_len = (pMsg->Data[3] << 8) + pMsg->Data[4];
			DBG_Print(LEVEL2,"onece max transfer len:%uByes\r\n",s_stVCU1OTA.transfer_max_len);		//MCU响应数据一次最大传输长度
		
			if (s_stVCU1OTA.transfer_max_len >= 512)
			{
				s_stVCU1OTA.transfer_max_len = 512;
			}
			else if (s_stVCU1OTA.transfer_max_len >= 256)
			{
				s_stVCU1OTA.transfer_max_len = 256;
			}
			else
			{
				s_stVCU1OTA.transfer_max_len = 128;
			}
		
			s_stVCU1OTA.next_app_bag_flag = 1;
			send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(s_stVCU1OTA.app_offset),(void *)(s_stVCU1OTA.transfer_max_len));
			s_stVCU1OTA.app_offset += s_stVCU1OTA.transfer_max_len;
		}
		
		if (s_stVCU1OTA.transfer_max_len && (s_stVCU1OTA.app_tx_len < s_stVCU1OTA.app_len))
		{
			unsigned int i = 0;
		
			if ((canid == 0x7EF) && (pMsg->Data[1] == 0x76))
			{
				s_stVCU1OTA.next_app_bag_flag = 1;
		
				s_stVCU1OTA.app_tx_len += s_stVCU1OTA.app_cur_tx_len;

				if (s_stVCU1OTA.app_tx_len >= s_stVCU1OTA.app_len)											//传输完毕 退出
				{
					sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x01\x37\xFF\xFF\xFF\xFF\xFF\xFF",8);
		
					s_stVCU1OTA.mcu_ota_step = next_step;
		
					DBG_Print(LEVEL2,"app transfer finish\r\n");
					s_stVCU1OTA.next_app_bag_flag = 0;
					s_stVCU1OTA.transfer_max_len = 0;
				}
			}

			if ((GetRunTime() - s_stVCU1OTA.last_send_cmd_time) >= 8)
			{
				DBG_Print(LEVEL2,"sandian vcu ack timeout need restart\r\n");
				//sandian_mcu_restart_ota();
				sandian_vcu1_restart_ota();
				return ;
			}

			if (s_stVCU1OTA.next_app_bag_flag && s_stVCU1OTA.app_data_arrive_flag)
			{
				s_stVCU1OTA.next_app_bag_flag = 0;
		
				if ((s_stVCU1OTA.app_len - s_stVCU1OTA.app_tx_len) <= 5)
				{
					s_stVCU1OTA.app_cur_tx_len = s_stVCU1OTA.app_len - s_stVCU1OTA.app_tx_len;
		
					dat[0] = s_stVCU1OTA.app_cur_tx_len + 2;
					dat[1] = 0x36;
					dat[2] = 0x01 + s_stVCU1OTA.app_bag_id;
					
					dat[3] = s_stVCU1OTA.buf[0];								//data										
					dat[4] = s_stVCU1OTA.buf[1];
					dat[5] = s_stVCU1OTA.buf[2];
					dat[6] = s_stVCU1OTA.buf[3];
					dat[7] = s_stVCU1OTA.buf[4];
				}
				else
				{
					if ((s_stVCU1OTA.app_len - s_stVCU1OTA.app_tx_len) > s_stVCU1OTA.transfer_max_len)
					{
						s_stVCU1OTA.app_cur_tx_len = s_stVCU1OTA.transfer_max_len;
					}
					else
					{
						s_stVCU1OTA.app_cur_tx_len = s_stVCU1OTA.app_len - s_stVCU1OTA.app_tx_len;
					}
					
					dat[0] = ((s_stVCU1OTA.app_cur_tx_len + 2) >> 8) | 0x10;
					dat[1] = s_stVCU1OTA.app_cur_tx_len + 2;
					dat[2] = 0x36;
					dat[3] = 0x01 + s_stVCU1OTA.app_bag_id;
		
					dat[4] = s_stVCU1OTA.buf[0];								//data										
					dat[5] = s_stVCU1OTA.buf[1];
					dat[6] = s_stVCU1OTA.buf[2];
					dat[7] = s_stVCU1OTA.buf[3];
				}

				sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);
				s_stVCU1OTA.last_send_cmd_time = GetRunTime();
				s_stVCU1OTA.app_bag_id++;
			}
		
			if ((canid == 0x7EF) && ((pMsg->Data[0] & 0xF0) == 0x30))
			{
				templen = 4;
				i = 0;
				while (templen < s_stVCU1OTA.app_cur_tx_len)
				{
					os_sleep(2);
					dat[0] = 0x20 + ((i+1) % 16);
					dat[1] = s_stVCU1OTA.buf[4 + (i * 7) + 0];
					dat[2] = s_stVCU1OTA.buf[4 + (i * 7) + 1];
					dat[3] = s_stVCU1OTA.buf[4 + (i * 7) + 2];
					dat[4] = s_stVCU1OTA.buf[4 + (i * 7) + 3]; 									
					dat[5] = s_stVCU1OTA.buf[4 + (i * 7) + 4];
					dat[6] = s_stVCU1OTA.buf[4 + (i * 7) + 5];
					dat[7] = s_stVCU1OTA.buf[4 + (i * 7) + 6];
		
					sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);
		
					templen += 7;
					i++;

					if ((i % 16) == 0) os_sleep(5);
				}

				s_stVCU1OTA.app_data_arrive_flag = 0;
				send_msg(S_MSG_GET_FILE_DATA,(void *)file_type,(void *)(s_stVCU1OTA.app_offset),(void *)(s_stVCU1OTA.transfer_max_len));
				s_stVCU1OTA.app_offset += s_stVCU1OTA.transfer_max_len;
			}
		}

	}

}


void vcu1_ota_proc(char channel,CanRxMsg *pMsg)
{
	unsigned int canid = 0;
	unsigned char dat[8] = {0};

	if (pMsg->IDE)
	{
		canid = pMsg->ExtId;
	}
	else
	{
		canid = pMsg->StdId;
	}

	if (canid == 0x7EF)
	{
		s_stVCU1OTA.channel = channel;
	}

	switch (s_stVCU1OTA.mcu_ota_step)
	{
	case CAN_VCU_OTA_STEP_1:
	{
		sandian_set_update_status(SANDIAN_UPDATE_STATUS_ING);
		sandian_tx(0,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		sandian_tx(1,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		sandian_tx(2,0x7DF,"\x02\x10\x03\xFF\xFF\xFF\xFF\xFF",8);
		s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_1_WAIT_ACK;
	}
	break;
	case CAN_VCU_OTA_STEP_1_WAIT_ACK:
	case CAN_VCU_OTA_STEP_2:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x03))
		{
			sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x04\x31\x01\xFF\x02\xFF\xFF\xFF",8);
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_2_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_2_WAIT_ACK:
	case CAN_VCU_OTA_STEP_3:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01))
		{
			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x02\x85\x82\xFF\xFF\xFF\xFF\xFF",8);
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_3_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_3_WAIT_ACK:
	case CAN_VCU_OTA_STEP_4:
		sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x03\x28\x83\x03\xFF\xFF\xFF\xFF",8);
		s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_4_WAIT_ACK;
	break;

	case CAN_VCU_OTA_STEP_4_WAIT_ACK:
	case CAN_VCU_OTA_STEP_5:
		s_stVCU1OTA.last_send_cmd_time = GetRunTime();
		sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x02\x10\x02\xFF\xFF\xFF\xFF\xFF",8);
		s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_5_WAIT_ACK;
	break;

	case CAN_VCU_OTA_STEP_5_WAIT_ACK:
	case CAN_VCU_OTA_STEP_6:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x50) && (pMsg->Data[2] == 0x02))
		{
			os_sleep(5);
			sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x02\x27\x09\xFF\xFF\xFF\xFF\xFF",8);		//获取安全访问种子
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_6_WAIT_ACK;
		}

		if ((GetRunTime() - s_stVCU1OTA.last_send_cmd_time) >= 3)
		{
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_5;
		}
	break;

	case CAN_VCU_OTA_STEP_6_WAIT_ACK:
	case CAN_VCU_OTA_STEP_7:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x67) && (pMsg->Data[2] == 0x09))				//发送安全密钥
		{
			unsigned int temp = 0;

			DBG_Print(LEVEL2,"mcu seed:%02X %02X %02X %02X\r\n",pMsg->Data[3],pMsg->Data[4],pMsg->Data[5],pMsg->Data[6]);
			s_stVCU1OTA.seed = (pMsg->Data[3] << 24) + (pMsg->Data[4] << 16) + (pMsg->Data[5] << 8) +pMsg->Data[6];

			//temp = _Acc_UdsDemoCalculateKey_UnifyCfg1(s_stVCU1OTA.seed);
			temp = SeedToKey(2,s_stVCU1OTA.seed);

			dat[0] = 0x06;
			dat[1] = 0x27;
			dat[2] = 0x0A;
			dat[3] = temp >> 24;
			dat[4] = temp >> 16;
			dat[5] = temp >> 8;
			dat[6] = temp;
			dat[7] = 0xFF;

			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);

			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_7_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_7_WAIT_ACK:
	case CAN_VCU_OTA_STEP_8:														//下载驱动
		Bzip2_Start1();
		if (s_stVCU1OTA.drv_len)
		{
			if ((canid == 0x7EF) && (pMsg->Data[1] == 0x67) && (pMsg->Data[2] == 0x0A))
			{
				dat[0] = 0x10;
				dat[1] = 0x0B;
				dat[2] = 0x34;
				dat[3] = 0x00;
				dat[4] = 0x44;
				dat[5] = s_stVCU1OTA.drv_addr >> 24;
				dat[6] = s_stVCU1OTA.drv_addr >> 16;
				dat[7] = s_stVCU1OTA.drv_addr >> 8;
				sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);					//连续帧 内存地址+长度
			}
			
			if ((canid == 0x7EF) && (pMsg->Data[0] == 0x30))
			{
				dat[0] = 0x21;
				dat[1] = s_stVCU1OTA.drv_addr;
				dat[2] = s_stVCU1OTA.drv_len >> 24;
				dat[3] = s_stVCU1OTA.drv_len >> 16;
				dat[4] = s_stVCU1OTA.drv_len >> 8;
				dat[5] = s_stVCU1OTA.drv_len;
				dat[6] = 0xFF;
				dat[7] = 0xFF;
				sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);					//连续帧 内存地址+长度
				s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_8_WAIT_ACK;
			}
		}
		else
		{
			dat[0] = 0x10;
			dat[1] = 0x0D;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xFF;
			dat[5] = 0x00;
			dat[6] = 0x44;
			dat[7] = s_stVCU1OTA.app_addr >> 24;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行依赖性检查		
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_11;
		}
	break;

	case CAN_VCU_OTA_STEP_8_WAIT_ACK:
	case CAN_VCU_OTA_STEP_9:
		vcu1_transfer_file_proc(pMsg,21,CAN_VCU_OTA_STEP_9_WAIT_ACK);		//分段文件传输
	break;

	case CAN_VCU_OTA_STEP_9_WAIT_ACK:
	case CAN_VCU_OTA_STEP_10:																
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x77))
		{
			s_stVCU1OTA.drv_check_sum = Bzip2_XOR1();

			DBG_Print(LEVEL2,"drv transfer finish exit.checksum:%08X\r\n",s_stVCU1OTA.drv_check_sum);

			dat[0] = 0x10;
			dat[1] = 0x08;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xF0;
			dat[5] = 0x01;
			dat[6] = s_stVCU1OTA.drv_check_sum >> 24;
			dat[7] = s_stVCU1OTA.drv_check_sum >> 16;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行完整性检测
		}

		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = s_stVCU1OTA.drv_check_sum >> 8;
			dat[2] = s_stVCU1OTA.drv_check_sum;
			dat[3] = 0xFF;
			dat[4] = 0xFF;
			dat[5] = 0xFF;
			dat[6] = 0xFF;
			dat[7] = 0xFF;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行完整性检测
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_10_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_10_WAIT_ACK:
	case CAN_VCU_OTA_STEP_11:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xF0) && (pMsg->Data[4] == 0x01))
		{
			dat[0] = 0x10;
			dat[1] = 0x0D;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xFF;
			dat[5] = 0x00;
			dat[6] = 0x44;
			dat[7] = s_stVCU1OTA.app_addr >> 24;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行依赖性检查			
		}

		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = s_stVCU1OTA.app_addr >> 16;
			dat[2] = s_stVCU1OTA.app_addr >> 8;
			dat[3] = s_stVCU1OTA.app_addr;
			dat[4] = s_stVCU1OTA.app_len >> 24;
			dat[5] = s_stVCU1OTA.app_len >> 16;
			dat[6] = s_stVCU1OTA.app_len >> 8;
			dat[7] = s_stVCU1OTA.app_len;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_11_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_11_WAIT_ACK:
	case CAN_VCU_OTA_STEP_12:
		Bzip2_Start1();
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xFF))
		{
			dat[0] = 0x10;
			dat[1] = 0x0B;
			dat[2] = 0x34;
			dat[3] = 0x00;
			dat[4] = 0x44;
			dat[5] = s_stVCU1OTA.app_addr >> 24;
			dat[6] = s_stVCU1OTA.app_addr >> 16;
			dat[7] = s_stVCU1OTA.app_addr >> 8;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);					//连续帧 内存地址+长度
		}

		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = s_stVCU1OTA.app_addr;
			dat[2] = s_stVCU1OTA.app_len >> 24;
			dat[3] = s_stVCU1OTA.app_len >> 16;
			dat[4] = s_stVCU1OTA.app_len >> 8;
			dat[5] = s_stVCU1OTA.app_len;
			dat[6] = 0xFF;
			dat[7] = 0xFF;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);					//连续帧 内存地址+长度
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_12_WAIT_ACK;
			s_stVCU1OTA.last_send_cmd_time = GetRunTime();
		}
		
	break;

	case CAN_VCU_OTA_STEP_12_WAIT_ACK:
	case CAN_VCU_OTA_STEP_13:
		vcu1_transfer_file_proc(pMsg,22,CAN_VCU_OTA_STEP_13_WAIT_ACK);
	break;

	case CAN_VCU_OTA_STEP_13_WAIT_ACK:
	case CAN_VCU_OTA_STEP_14:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x77))
		{
			s_stVCU1OTA.app_check_sum = Bzip2_XOR1();
			DBG_Print(LEVEL2,"app transfer finish exit.check sum:%08X\r\n",s_stVCU1OTA.app_check_sum);

			dat[0] = 0x10;
			dat[1] = 0x08;
			dat[2] = 0x31;
			dat[3] = 0x01;
			dat[4] = 0xF0;
			dat[5] = 0x01;
			dat[6] = s_stVCU1OTA.app_check_sum >> 24;
			dat[7] = s_stVCU1OTA.app_check_sum >> 16;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行完整性检测
		}


		if ((canid == 0x7EF) && (pMsg->Data[0] == 0x30))
		{
			dat[0] = 0x21;
			dat[1] = s_stVCU1OTA.app_check_sum >> 8;
			dat[2] = s_stVCU1OTA.app_check_sum;
			dat[3] = 0xFF;
			dat[4] = 0xFF;
			dat[5] = 0xFF;
			dat[6] = 0xFF;
			dat[7] = 0xFF;
			sandian_tx(s_stVCU1OTA.channel,0x7E7,dat,8);									//例行完整性检测
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_14_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_14_WAIT_ACK:
	case CAN_VCU_OTA_STEP_15:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xF0) && (pMsg->Data[4] == 0x01))
		{
			sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x04\x31\x01\xFF\x01\xFF\xFF\xFF",8);	//例行依赖性检查

			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_15_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_15_WAIT_ACK:
	case CAN_VCU_OTA_STEP_16:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x71) && (pMsg->Data[2] == 0x01) && (pMsg->Data[3] == 0xFF) && (pMsg->Data[4] == 0x01))
		{
			DBG_Print(LEVEL1,"send mcu reset cmd\r\n");
			sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x02\x11\x01\xFF\xFF\xFF\xFF\xFF",8);	//ECU复位

			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_16_WAIT_ACK;
		}
	break;
	case CAN_VCU_OTA_STEP_16_WAIT_ACK:
	case CAN_VCU_OTA_STEP_17:
		if ((canid == 0x7EF) && (pMsg->Data[1] == 0x51) && (pMsg->Data[2] == 0x01))
		{
			os_sleep(1500);

			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x02\x10\x83\xFF\xFF\xFF\xFF\xFF",8);	//诊断会话控制
			os_sleep(100);
			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x03\x28\x80\x03\xFF\xFF\xFF\xFF",8);	//通讯控制 允许
			os_sleep(100);
			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x02\x85\x81\xFF\xFF\xFF\xFF\xFF",8);	//开启故障码

			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x02\x10\x81\xFF\xFF\xFF\xFF\xFF",8);	//进入诊断会话模式
			os_sleep(100);

			sandian_tx(s_stVCU1OTA.channel,0x7E7,"\x04\x14\xFF\xFF\xFF\xFF\xFF\xFF",8);
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_17_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_17_WAIT_ACK:
	case CAN_VCU_OTA_STEP_18:
		//if ((canid == 0x7EF) && (pMsg->Data[1] == 0x54))
		{
			DBG_Print(LEVEL1,"sandian mcu ota finish\r\n");

			sandian_set_update_status(SANDIAN_UPDATE_STATUS_SUCCESS);

			s_stVCU1OTA.last_send_cmd_time = GetRunTime();
			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_STEP_18_WAIT_ACK;
		}
	break;

	case CAN_VCU_OTA_STEP_18_WAIT_ACK:
		if ((GetRunTime() - s_stVCU1OTA.last_send_cmd_time) >= 7)
		{
			sandian_tx(s_stVCU1OTA.channel,0x7DF,"\x03\x22\xF1\x95\xFF\xFF\xFF\xFF",8);

			s_stVCU1OTA.mcu_ota_step = CAN_VCU_OTA_NONE;
		}
	break;
	default:break;
	}
}

void vcu_test(void)
{
	unsigned int key = 0;

	//_Acc_UdsDemoCalculateKey_UnifyCfg1(0x0BB58FF9);

	key = SeedToKey(2,0x0BB58FF9);

	DBG_Print(LEVEL2,"seedTokey:%08X\r\n",key);
	
}


