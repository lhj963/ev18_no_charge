
unsigned char is_sm2_chip_ok_flag = 0;
unsigned char chip_type = 0;	//0华大 1 华虹

LFOS_SEM sm2_sem;

void create_sm2_chip_sem(void)
{
	BSP_OS_SemCreate(&sm2_sem,"sm2",1);
}

void wait_sm2_chip_sem(void)
{
	BSP_OS_SemWait(&sm2_sem,0);
}

void post_sm2_chip_sem(void)
{
	BSP_OS_SemPost(&sm2_sem);
}

char is_sm2_chip_ok(void)
{
	return is_sm2_chip_ok_flag;
}

unsigned char get_chip_type(void)
{
	return chip_type;
}

void sm2_chip_init(void)
{
	DRV_RUN_DATA_TYPE *drv_ptr = 0;


	drv_ptr = get_drv_run_data_ptr();

	is_sm2_chip_ok_flag = drv_ptr->is_jiami_chip_ok;

	if (drv_ptr->is_jiami_chip_ok == 0)
	{
		return ;
	}
	if ((drv_ptr->status & 0x0003) == 1)
	{
		chip_type = 1;
		hhsm_init(0,hhsm_spi_write,hhsm_spi_read,hhsm_ce_high,
		hhsm_ce_low,BSP_OS_TimeDlyMs,is_hhsm_busy);
		DBG_Print(0,"hua hong sm2 chip\r\n");
	}
	else
	{
		chip_type = 0;
		DBG_Print(0,"hua da sm2 chip\r\n");
		hdse_init(hd_set_gpio0,BSP_OS_TimeDlyMs,hhsm_spi_write,hhsm_spi_read);
		set_sm2_ver(SM2_VER2);
	}

	create_sm2_chip_sem();
}

unsigned int sm2_chip_read_public_key(void *pOut,unsigned int outbuflen)
{
	unsigned int xlen = 0;

	if (is_sm2_chip_ok() == 0)
	{
		return 0;
	}
	wait_sm2_chip_sem();
	if (get_chip_type() == 0)
	{
		xlen = hdsm_read_public_key(pOut,outbuflen);	
	}
	else
	{
		xlen = hhsm_read_public_key(pOut,outbuflen);
	}
	post_sm2_chip_sem();
	return xlen;
}


unsigned int sm2_chip_read_public_keyB(void *pOut,unsigned int outbuflen)
{
	unsigned int xlen = 0;

	if (is_sm2_chip_ok() == 0)
	{
		return 0;
	}
	wait_sm2_chip_sem();
	if (get_chip_type() == 0)
	{
		xlen = hdsm_read_public_keyB(pOut,outbuflen);	
	}
	else
	{
		xlen = hhsm_read_public_keyB(pOut,outbuflen);
	}
	post_sm2_chip_sem();
	return xlen;
}

unsigned int sm2_chip_read_sign_data_with_z(void *chip_id,
							unsigned int chip_id_len,
							void *public_key,unsigned int public_key_len,
							void *pdat, unsigned int datlen,
							void *pOutSign,unsigned int outbuflen)
{
	unsigned int xlen = 0;

	if (is_sm2_chip_ok() == 0)
	{
		return 0;
	}
	wait_sm2_chip_sem();
	if (get_chip_type() == 0)
	{
		xlen = hdsm_read_sign_data_with_z(chip_id,
							chip_id_len,
							public_key,public_key_len,
							pdat, datlen,
							pOutSign,outbuflen);
	}
	else
	{
		xlen = hhsm_read_sign_data_with_z(chip_id,
							chip_id_len,
							public_key,public_key_len,
							pdat, datlen,
							pOutSign,outbuflen);

	}
	post_sm2_chip_sem();
	return xlen;
}

//写入公私钥对B
int sm2_chip_write_public_private_keyB(void *public_key,
					unsigned int pb_len,void *private_key,unsigned int pv_len)
{
	int xlen = 0;

	if (is_sm2_chip_ok() == 0)
	{
		return 0;
	}
	wait_sm2_chip_sem();
	if (get_chip_type() == 0)
	{
		xlen = hdsm_write_public_private_keyB(public_key,pb_len,private_key,pv_len);
	}
	else
	{
		xlen = hhsm_write_public_private_keyB(public_key,pb_len,private_key,pv_len);
	}
	post_sm2_chip_sem();
	return xlen;
}


//使用B组公私钥对加解密 0 - 私钥解密，1 - 公钥加密
unsigned int sm2_chip_private_key_decrypt_encryptB(unsigned char type,
				void *data, unsigned int len, void *outdata,unsigned int maxlen)
{
	unsigned int xlen = 0;

	if (is_sm2_chip_ok() == 0)
	{
		(void)type;
		(void)len;
		return 0;
	}
	wait_sm2_chip_sem();
	if (get_chip_type() == 0)
	{
		xlen = hdsm_private_key_decrypt_encryptB(type,data,len,outdata,maxlen);
	}
	else
	{
		xlen = hhsm_private_key_decrypt_encryptB(type,data,len,outdata,maxlen);
	}
	post_sm2_chip_sem();
	return xlen;
}


