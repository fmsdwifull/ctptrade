#include "market.h"

// 服务器地址
//char * serverAddress = (char *)"tcp://101.231.96.18:51205";
char * serverAddress = (char *)"tcp://180.168.146.187:10010";
// 登录请求结构体
CThostFtdcReqUserLoginField userLoginField;
// 用户请求结构体
CThostFtdcUserLogoutField userLogoutField;
// 线程同步标志
sem_t sem;




int main(int argc char *argv[])
{
	CMarketHandler *cmd=new CMarketHandler();
    cmd->init();

	printf("主线程执行完毕!\n");
    return(0);
}
