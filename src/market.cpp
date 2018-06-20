#include "market.h"
#include <iostream>
using namespace std;

char* ppInstrumentID[]={"IF1809","ni1809","ru1809","SR809","cu1809","TA809"};	
int iInstrumentID=6;
char* serverAddress = (char *)"tcp://180.168.146.187:10010";

// 请求编号
int iRequestID;

CMarketHandler::CMarketHandler()
{
	printf("this is CMarketHandler");
}

void CMarketHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo,
						int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMarketHandler::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMarketHandler::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMarketHandler::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	///用户登录请求
	ReqUserLogin();
}


void CMarketHandler::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, "9999");
	strcpy(req.UserID, "116372");
	strcpy(req.Password, "123456");
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMarketHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();	
	}
}

void CMarketHandler::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMarketHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
	cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	cout << pRspInfo->ErrorMsg <<endl;
}

void CMarketHandler::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}
//返回合约代码具体信息
void CMarketHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//cout>>pDepthMarketData->InstrumentID;;;
	cerr << __FUNCTION__ << endl;
	//cout << __FUNCTION__ <<endl;
        cout << pDepthMarketData->InstrumentID << endl; 
        
	cout << pDepthMarketData->UpdateTime << endl; 
        cout << pDepthMarketData->LastPrice << endl; 
        cout << pDepthMarketData->BidPrice1 << endl; 
        cout << pDepthMarketData->BidVolume1 << endl; 

        cout << pDepthMarketData->AskPrice1 << endl; 
        cout << pDepthMarketData->AskVolume1 << endl; 
         
	/*
	QString dm = pDepthMarketData->InstrumentID;   //
	QString updatetime = pDepthMarketData->UpdateTime;	  // 更新时间
	QString lastprice = QString::number(pDepthMarketData->LastPrice);	  //最新价
	QString buyprice = QString::number(pDepthMarketData->BidPrice1); //买一价
	QString buyvol=QString::number(pDepthMarketData->BidVolume1);//买一量
	QString sellprice =QString::number(pDepthMarketData->AskPrice1); //卖一价
	QString sellvol = QString::number(pDepthMarketData->AskVolume1); //卖一量
	QString preLastPrice = QString::number(pDepthMarketData->PreClosePrice); //昨收盘价
	QString zf = QString::number(((pDepthMarketData->LastPrice-pDepthMarketData->PreClosePrice)*100/pDepthMarketData->PreClosePrice),'f',2);  //涨幅的计算
	QString vol = QString::number(pDepthMarketData->Volume); //成交量
	QString zt = QString::number(pDepthMarketData->UpperLimitPrice); //涨停价
	QString dt = QString::number(pDepthMarketData->LowerLimitPrice); //跌停价

	QString HQTick = dm+","+updatetime+","+lastprice+","+buyprice+","+buyvol+","+sellprice+","+sellvol+","+zf+","+vol+","+zt+","+dt;	 //使用信号传递数据
	emit sendData(HQTick); //只是发送了数据，需要接收端接收数据 ，a,在ctp.h头文件定义接收方法 b, 通过SLOT连接
	*/
}

bool CMarketHandler::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMarketHandler::Init()
{
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	pUserApi->RegisterSpi(this);
	pUserApi->RegisterFront(serverAddress);
	pUserApi->Init();
}
