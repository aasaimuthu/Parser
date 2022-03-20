#include "CBroadCastHandler.h"


bool CNewOrderMsgIn::Process(char *msg)
{
    NewOrderIn *pIn =  reinterpret_cast<NewOrderIn*>(msg);
    m_NewOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageType = NewOrderOut::MESSAGEID;
    m_NewOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageSize =  sizeof(NewOrderOut);
    strncpy(m_NewOrderOut.oOrderCommonOut.StockTicker ,pIn->StockTicker,STOCK_TICKER_LEN);
    m_NewOrderOut.oOrderCommonOut.TimeStamp = htonll(pIn->oOrderCommonIn.TimeStamp) + GetTillMidNightEpochTime();
    m_NewOrderOut.oOrderCommonOut.OrderReferenceNumber = htonll(pIn->oOrderCommonIn.OrderReferenceNumber);
    m_NewOrderOut.Side = 'B';
    m_NewOrderOut.oSize = htonl(pIn->oSize);
    m_NewOrderOut.oPrice = htonl(pIn->oPrice)/PRICE_MULTIPLIER;
    m_mapOrderExecutedPrice.insert(make_pair(m_NewOrderOut.oOrderCommonOut.OrderReferenceNumber, htonl(pIn->oPrice)));
    return true;
}

bool COrderExecutionIn::Process(char *msg)
{
    OrderExecutionIn *pIn =  reinterpret_cast<OrderExecutionIn*>(msg);
    m_OrderExecutionOut.oOrderCommonOut.oMessageHeaderOut.MessageType = OrderExecutionOut::MESSAGEID;
    m_OrderExecutionOut.oOrderCommonOut.oMessageHeaderOut.MessageSize =  sizeof(OrderExecutionOut);
    strncpy(m_OrderExecutionOut.oOrderCommonOut.StockTicker ,"SPY",STOCK_TICKER_LEN);
    m_OrderExecutionOut.oOrderCommonOut.TimeStamp =  htonll(pIn->oOrderCommonIn.TimeStamp) + GetTillMidNightEpochTime();
    m_OrderExecutionOut.oOrderCommonOut.OrderReferenceNumber = htonll(pIn->oOrderCommonIn.OrderReferenceNumber);

    m_OrderExecutionOut.oSize = htonl(pIn->oSize);

    MapOrderExecutedPrice::const_iterator it = m_mapOrderExecutedPrice.find( m_OrderExecutionOut.oOrderCommonOut.OrderReferenceNumber);
    if(it != m_mapOrderExecutedPrice.end())
    {
        m_OrderExecutionOut.oPrice = (it->second/PRICE_MULTIPLIER);
    }
    else
        m_OrderExecutionOut.oPrice = 0.0;
    return true;
}
bool CCancelOrderIn::Process(char *msg)
{
    CancelOrderIn *pIn =  reinterpret_cast<CancelOrderIn*>(msg);
    m_ReducedOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageType = ReducedOrderOut::MESSAGEID;
    m_ReducedOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageSize =  sizeof(ReducedOrderOut);
    strncpy(m_ReducedOrderOut.oOrderCommonOut.StockTicker ,"SPY",STOCK_TICKER_LEN);
    m_ReducedOrderOut.oOrderCommonOut.TimeStamp =  htonll(pIn->oOrderCommonIn.TimeStamp) + GetTillMidNightEpochTime();
    m_ReducedOrderOut.oOrderCommonOut.OrderReferenceNumber = htonll(pIn->oOrderCommonIn.OrderReferenceNumber);
    m_ReducedOrderOut.oSize = htonl(pIn->oSize);
    return true;
}
bool CReplaceOrderIn::Process(char *msg)
{
    ReplaceOrderIn *pIn =  reinterpret_cast<ReplaceOrderIn*>(msg);
    m_ReplaceOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageType = ReplaceOrderOut::MESSAGEID;
    m_ReplaceOrderOut.oOrderCommonOut.oMessageHeaderOut.MessageSize =  sizeof(ReplaceOrderOut);
    strncpy(m_ReplaceOrderOut.oOrderCommonOut.StockTicker ,"SPY",STOCK_TICKER_LEN);
    m_ReplaceOrderOut.oOrderCommonOut.TimeStamp =  htonll(pIn->oOrderCommonIn.TimeStamp) + GetTillMidNightEpochTime();
    m_ReplaceOrderOut.oOrderCommonOut.OrderReferenceNumber = htonll(pIn->oOrderCommonIn.OrderReferenceNumber);
    m_ReplaceOrderOut.ReplacedOrderReferenceNumber = htonll(pIn->ReplacedOrderReferenceNumber);
    m_ReplaceOrderOut.oSize = htonl(pIn->oSize);
    m_ReplaceOrderOut.oPrice = htonl(pIn->oPrice)/PRICE_MULTIPLIER;;
    m_mapOrderExecutedPrice[m_ReplaceOrderOut.oOrderCommonOut.OrderReferenceNumber]=htonl(pIn->oPrice);
    return true;
}
