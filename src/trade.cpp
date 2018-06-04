#include "MdSpi.h"
#include <iostream>
#include <QstringList>
using namespace std;

#pragma warning(disable: 4996)

char* ppInstrumentID []={"IF1609","ni1609","ru1609","SR609","cu1609","TA609"};	
int iInstrumentID=6;
// 请求编号
int iRequestID;

MdSpi::MdSpi(QObject *parent)
	: QObject(parent)
{

}

MdSpi::~MdSpi()
{

}

void MdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
						int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}

void MdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void MdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void MdSpi::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	///用户登录请求
	ReqUserLogin();
}


void MdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, hq.BROKER_ID);
	strcpy(req.UserID, "");
	strcpy(req.Password, "");
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();
		SubscribeMarketData(dm);  //
	}
}
//
void MdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//自动交易模块化行情代码部分
void MdSpi::SubscribeMarketData(QString dm)
{
	QStringList strlist=dm.split(",");
	int iInstrumentID=strlist.length();
	for (int i=0;i<iInstrumentID;i++)
	{
		QString str=strlist.at(i);
		char *ch;
		QByteArray ba=str.toLatin1();
		ch=ba.data();
		char *myppInstrumentID[]={ch};
		int iRequestID=pUserApi->SubscribeMarketData(myppInstrumentID,1); //每次只发送一个
		
	}
	cerr<<"--->>>发送行情订阅请求:"<<((iRequestID==0)?"成功":"失败")<<endl;
}



void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}

void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}
//返回合约代码具体信息
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	//cerr << __FUNCTION__ << endl;
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
	QString openprice=QString::number(pDepthMarketData->OpenPrice);	 //开盘价

	QString HQTick = dm+","+updatetime+","+lastprice+","+buyprice+","+buyvol+","+sellprice+","+sellvol+","+zf+","+vol+","+zt+","+dt+","+openprice;	 //使用信号传递数据
	emit sendData(HQTick); //只是发送了数据，需要接收端接收数据 ，a,在ctp.h头文件定义接收方法 b, 通过SLOT连接
}

bool MdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void MdSpi::Init()
{
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	pUserApi->RegisterSpi(this);
	pUserApi->RegisterFront(hq.FRONT_ADDR);
	pUserApi->Init();
}
