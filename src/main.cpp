#include "market.h"

// 服务器地址
//char * serverAddress = (char *)"tcp://101.231.96.18:51205";
// 登录请求结构体
CThostFtdcReqUserLoginField userLoginField;
// 用户请求结构体
CThostFtdcUserLogoutField userLogoutField;
// 线程同步标志
sem_t sem;




int main(int argc, char *argv[])
{
	sem_init(&sem,0,0);
	CMarketHandler *cmd=new CMarketHandler();
    cmd->Init();
	// 等待登出成功
	sem_wait(&sem);
	printf("主线程执行完毕!\n");
    return 0;
}
