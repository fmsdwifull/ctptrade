#include "trade.h"
#include <iostream>
using namespace std;

#pragma warning(disable: 4996)



 typedef struct TdStruct
  {
            char FRONT_ADDR[100];
            TThostFtdcBrokerIDType BROKER_ID;
           TThostFtdcInvestorIDType INVESTOR_ID;
           TThostFtdcPasswordKeyType PASSWORD;
  };
 TdStruct jy;



char INSTRUMENT_ID[100];	// 合约代码
TThostFtdcPriceType	LIMIT_PRICE;	// 价格
TThostFtdcDirectionType	DIRECTION;	// 买卖方向

// 请求编号
int iRequestTdID;

// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用

char hyarray[500][4];   //定义一个合约的二纬数组
int k=0;



CTradeHandler::CTradeHandler()
{
	printf("this is CTradeHandler");
}

CTradeHandler::~CTradeHandler()
{

}


void CTradeHandler::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CTradeHandler::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, jy.BROKER_ID);
	strcpy(req.UserID, jy.INVESTOR_ID);
	strcpy(req.Password, jy.PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestTdID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///获取当前交易日

		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		///投资者结算结果确认
		ReqSettlementInfoConfirm();
	}
}

void CTradeHandler::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, jy.BROKER_ID);
	strcpy(req.InvestorID, jy.INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestTdID);
	cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		ReqQryInstrument();
	}
}

void CTradeHandler::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestTdID);
	cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}

void CTradeHandler::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, jy.BROKER_ID);
	strcpy(req.InvestorID, jy.INVESTOR_ID);
	int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestTdID);
	cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}

void CTradeHandler::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, jy.BROKER_ID);
	strcpy(req.InvestorID, jy.INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestTdID);
	cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  
}


void CTradeHandler::ReqOrderInsert(char* dm,char* lx,int lots,double price)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, jy.BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, jy.INVESTOR_ID);
	///合约代码
	//strcpy(req.InstrumentID, dm.toStdString().data());
	///报单引用
    sprintf(ORDER_REF,"%d",iRequestTdID);	  //-------------
	strcpy(req.OrderRef, ORDER_REF);  
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;

	if (lx=="kd")
	{
		req.Direction=THOST_FTDC_D_Buy;
		req.CombOffsetFlag[0]= THOST_FTDC_OF_Open;
	}
	if (lx=="pd")
	{
		req.Direction=THOST_FTDC_D_Sell;
		req.CombOffsetFlag[0]=THOST_FTDC_OF_CloseToday;
	}
	if (lx=="kk")
	{
		  req.Direction=THOST_FTDC_D_Sell;
		  req.CombOffsetFlag[0]=THOST_FTDC_OF_Open;
	}
	if (lx=="pk")
	{
		req.Direction=THOST_FTDC_D_Buy;
		req.CombOffsetFlag[0] =THOST_FTDC_OF_CloseToday;
	}

	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	req.LimitPrice = price;
	///数量: 1
	req.VolumeTotalOriginal = lots;
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;

	int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestTdID);
	cerr << "--->>> 报单录入请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTradeHandler::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
//报单录入请求,撤单
void CTradeHandler::ReqOrderAction(char* brokerid,char* wth,char* jys)
{
/*
	QByteArray ba=brokerid.toLatin1();
	char *pbid=ba.data();
	QByteArray bw=wth.toLatin1();
	char* pwth=bw.data();
	QByteArray bj=jys.toLatin1();
	char *pjys=	bj.data();

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, pbid);
	strcpy(req.OrderSysID,pwth); //委托号
	strcpy(req.ExchangeID,pjys); //交易所

	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;
	int iResult = pUserApi->ReqOrderAction(&req, ++iRequestTdID);
*/
}

void CTradeHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

//问题模块
///报单通知	   ----委托回报
void CTradeHandler::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	
}

///成交通知
void CTradeHandler::OnRtnTrade(CThostFtdcTradeField *pTrade)
{


}

void CTradeHandler:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CTradeHandler::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CTradeHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

bool CTradeHandler::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

bool CTradeHandler::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
		(pOrder->SessionID == SESSION_ID) &&
		(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTradeHandler::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

void CTradeHandler::Init()
{
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	pUserApi->RegisterSpi(this);			// 注册事件类
	pUserApi->SubscribePublicTopic(THOST_TERT_RESTART);					// 注册公有流
	pUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);					// 注册私有流
	pUserApi->RegisterFront(jy.FRONT_ADDR);							// connect
	pUserApi->Init();


}

/* 
CTradeHandler::HYStruct CTradeHandler::hy(char* dm)
 {
 	HYStruct myhy;
	for (int i=0;i<500;i++)
	{
		if (dm==hyarray[i][0])
		{
			myhy.hycs=hyarray[i][2].toInt();
			myhy.hyds=hyarray[i][3].toDouble();
			break;
		}
	}
 	return myhy;
 }
*/
