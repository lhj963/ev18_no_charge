#ifndef __HARDWARECFG_H__
#define __HARDWARECFG_H__

//app层
#define APP_START_ADDR 							(0x00000000 + (64*1024))
#define APP_END_ADDR 							0x00010000

#define SOFTWARE_INFO_OFFSET					0x1000
#define SOFTWARE_INFO_ADDR 						(APP_START_ADDR + SOFTWARE_INFO_OFFSET)

#define MANUFACTURER_FLAG       				"BYSD"				//厂商标识
#define LF_DRIVER_FILE_FLAG						"BYSD_DRIVER"		//驱动文件标识

#define APPLICATION_VERSION 					"BYSD_N2021_MCU_V3.2.46"

extern char APP_VERSION[50];

extern unsigned int GetCpuID(void *pOutBuf,unsigned int buflen);

extern unsigned int GetCpuFlashSize(void);

extern INT8U get_software_version(void);

extern INT8U get_bps_version(void);

#endif

