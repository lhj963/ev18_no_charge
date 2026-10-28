#include "sysapi.h"


/**
函数功能:以16进制形式将缓冲区的内容打印出来
入口参数:
	iLevel:打印等级
	tip:提示语
	pdat:要打印的缓冲区指针
	len:要打印的数据长度
出口参数:
	无
返回值:
	无
例如:
	假定 buf[3] = {0x11,0x22,0x33};
	DBG_PrintBuf(LEVEL0,"Read Hex:\r\n",buf,3);
	执行结果:
	Read Hex:
	11 22 33
*/
void DBG_PrintBuf(int iLevel,void *tip,void *pdat,unsigned int len)
{
	__lffunc__(_DBG_PrintBuf,(DRIVER_API_START_ADDR + 4))(iLevel,tip,pdat,len);
}

/**
函数功能:设置打印等级
入口参数:
	iLevel:打印等级
出口参数:
	无
返回值:
	无
例如:
*/
void Set_DBG_Level(int iLevel)
{
	__lffunc__(_Set_DBG_Level,(DRIVER_API_START_ADDR + 8))(iLevel);
}

/**
函数功能:获取当前打印等级
入口参数:
	无
出口参数:
	无
返回值:
	当前打印等级
例如:
*/
int Get_DBG_Level(void)
{
	return __lffunc__(_Get_DBG_Level,(DRIVER_API_START_ADDR + 12))();
}

/**
函数功能:创建一个任务
入口参数:
	thread_id:线程ID
	thread_proc:线程处理函数
	sp:该线程的堆栈栈顶
	bottom:该线程的栈底
出口参数:
	无
返回值:
	0成功  非0失败
例如:
*/
int os_create_thread(INT8U thread_id,void (*thread_proc)(void),OS_STK *sp,OS_STK *bottom)
{
	return __lffunc__(_os_create_thread,(DRIVER_API_START_ADDR + 20))(thread_id,thread_proc,sp,bottom);
}


/**
函数功能:延时函数
入口参数:
	t:延时时长 ms
出口参数:
	无
返回值:
	无
例如:
*/
void os_sleep(unsigned int t)
{
	__lffunc__(_os_sleep,(DRIVER_API_START_ADDR + 24))(t);
}


/**
函数功能:创建信号量
入口参数:
	pSem:信号量指针
	pName:信号量名称
	cnt:信号量初始值
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_create_sem(&sem1,"test",1);
*/
int os_create_sem(OS_SEM *pSem,void *pName,INT8U cnt)
{
	return __lffunc__(_os_create_sem,(DRIVER_API_START_ADDR + 28))(pSem,pName,cnt);
}

/**
函数功能:等待信号量
入口参数:
	pSem:信号量指针
	timeout:超时时间  0表示死等
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_sem_pend(&sem1,0);
*/
int os_sem_pend(OS_SEM *pSem,INT32U timeout)
{
	return __lffunc__(_os_sem_pend,(DRIVER_API_START_ADDR + 32))(pSem,timeout);
}


/**
函数功能:释放信号量
入口参数:
	pSem:信号量指针
出口参数:
	无
返回值:
	0成功 非0失败
例如:
	OS_SEM sem;

	os_sem_post(&sem1);
*/
int os_sem_post(OS_SEM *pSem)
{
	return __lffunc__(_os_sem_post,(DRIVER_API_START_ADDR + 36))(pSem);
}

void *os_malloc(INT32U size)
{
	return __lffunc__(_os_malloc,(DRIVER_API_START_ADDR + 40))(size);
}

int os_free(void *ptr)
{
	return __lffunc__(_os_free,(DRIVER_API_START_ADDR + 44))(ptr);
}


/**
函数功能:can初始化
入口参数:
	channel:第几路CAN  0,1,2等
	bps:CAN速率  125 250 500 1000等
出口参数:
	无
返回值:
	无
例如:
*/
void can_init(unsigned char channel,unsigned int bps)
{
	__lffunc__(_can_init,(DRIVER_API_START_ADDR + 44 + 36))(channel,bps);
}

/**
函数功能:发送CAN消息
入口参数:
	channel:第几路CAN
	CanData:CanTxMsg结构的指针
出口参数:
	无
返回值:
	0表示发送成功 非零表示发送失败
例如:
*/
int can_send_data(unsigned char channel,CanTxMsg* CanData)
{
	return __lffunc__(_can_send_data,(DRIVER_API_START_ADDR + 84))(channel,CanData);
}


/**
函数功能:发送消息给驱动
入口参数:
	msgtype:消息类型
	lparam:参数1
	wparam:参数2
出口参数:
	无
返回值:根据不同的消息类型 返回值视情况而定

说明:例如
	拨打电话:
	send_msg(S_MSG_DIAL,"13902910201",0);

	发送短信
	send_msg(S_MSG_SEND_SMS,"13902910201","hello world");

	//获取CPU型号
	char *cpu_type = send_msg(S_MSG_GET_CPU_TYPE,0,0);
	DBG_Print(0,"CPU:%s\r\n",cpu_type);
*/
void *send_msg(S_MSG_TYPE msgtype,void *param,void *lparam,void *wparam)
{
	return __lffunc__(_send_msg,(DRIVER_API_START_ADDR + 84 + 28))(msgtype,param,lparam,wparam);
}

void install_hook_msg(HOOK_MSG_TYPE msgtype,void *pf)
{
	__lffunc__(_install_hook_msg,(DRIVER_API_START_ADDR + 116))(msgtype,pf);
}

void reg_callback_msg(CALLBACK_MSG_TYPE msgtype,void *pf)
{
	__lffunc__(_reg_callback_msg,(DRIVER_API_START_ADDR + 120))(msgtype,pf);
}

/**
函数功能:获取系统运行时间 秒为单位
入口参数:
	无
出口参数:
	无
返回值:
	系统运行时间
例如:
*/
unsigned long GetRunTime(void)
{
	return __lffunc__(_GetRunTime,(DRIVER_API_START_ADDR + 124))();
}


