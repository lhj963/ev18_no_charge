#include "HardwareCfg.h"


char APP_VERSION[50] = {0};

unsigned int GetCpuID(void *pOutBuf,unsigned int buflen)
{
	INT8U len = 0;
	INT32U temp0,temp1,temp2;
	unsigned short ret = 0;
	unsigned char *cpuid = (unsigned char *)pOutBuf;

	temp0 = *(volatile unsigned int *)(0x1FFFF7E8);
	temp1 = *(volatile unsigned int *)(0x1FFFF7EC);
	temp2 = *(volatile unsigned int *)(0x1FFFF7F0);

	if (cpuid && (buflen >= 12))
	{
		memset(pOutBuf,0,buflen);
		cpuid[len++] = temp0 >> 24;
		cpuid[len++] = temp0 >> 16;
		cpuid[len++] = temp0 >> 8;
		cpuid[len++] = temp0;
		
		cpuid[len++] = temp1 >> 24;
		cpuid[len++] = temp1 >> 16;
		cpuid[len++] = temp1 >> 8;
		cpuid[len++] = temp1;
		
		cpuid[len++] = temp2 >> 24;
		cpuid[len++] = temp2 >> 16;
		cpuid[len++] = temp2 >> 8;
		cpuid[len++] = temp2;
		ret = len;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

unsigned int GetCpuFlashSize(void)
{
	return 512;
}


INT8U get_software_version(void)
{
	char *pver = 0;
	INT8U ver = 1;

	pver = ReverseFindSubStr(APP_VERSION,strlen(APP_VERSION),".",1);
	if (pver)
	{
		ver = atoi(pver + 1);
		DBG_Print(LEVEL3,"ver:%d\r\n",ver);
	}
	return ver;
}

INT8U get_bps_version(void)
{
	char *pver = 0;
	INT8U ver = 1;

	pver = _find_sub_str(APP_VERSION,strlen(APP_VERSION),".",1);	
	if (pver)
	{
		ver = atoi(pver + 1);
		DBG_Print(LEVEL5,"bps ver:%d\r\n",ver);
	}
	return ver;
}




