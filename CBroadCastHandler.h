#ifndef _BROADCAST_HANDLER_IN_H__
#define _BROADCAST_HANDLER_IN_H__

#include "CBroadCastHandlerBase.h"

class CNewOrderMsgIn: public CBroadCastHandlerBase
{
public:
    static const enMessageTypeIn MESSAGEID=NEW_ORDER;
    static const uint16 INMESSAGESIZE=sizeof(NewOrderIn);
    virtual bool Process(char *buff);
    virtual uint16 GetMessageSize()
    {
        return INMESSAGESIZE;
    }
    const char* GetSerilizeString()
    {
        return (const char*)&m_NewOrderOut;
    }
    uint16 GetSerilizeStringSize()
    {
        return sizeof(NewOrderOut);
    }
private:
    NewOrderOut m_NewOrderOut;
};

class COrderExecutionIn: public CBroadCastHandlerBase
{
public:
    static const enMessageTypeIn MESSAGEID=ORDER_EXECUTION;
    static const uint16 INMESSAGESIZE=sizeof(OrderExecutionIn);

    virtual bool Process(char *buff);
    virtual uint16 GetMessageSize()
    {
        return INMESSAGESIZE;
    }
    const char* GetSerilizeString()
    {
        return (char*)&m_OrderExecutionOut;
    }
    uint16 GetSerilizeStringSize()
    {
        return sizeof(OrderExecutionOut);
    }
private:
    OrderExecutionOut m_OrderExecutionOut;
};

class CCancelOrderIn: public CBroadCastHandlerBase
{
public:
    static const enMessageTypeIn MESSAGEID=CANCEL_ORDER;
    static const uint16 INMESSAGESIZE=sizeof(CancelOrderIn);

    virtual bool Process(char *buff);
    virtual uint16 GetMessageSize()
    {
        return INMESSAGESIZE;
    }
    const char* GetSerilizeString()
    {
        return (const char*)&m_ReducedOrderOut;
    }
    uint16 GetSerilizeStringSize()
    {
        return sizeof(ReducedOrderOut);
    }
private:
    ReducedOrderOut m_ReducedOrderOut;
};


class CReplaceOrderIn: public CBroadCastHandlerBase
{
public:
    static const enMessageTypeIn MESSAGEID=REPLACE_ORDER;
    static const uint16 INMESSAGESIZE=sizeof(ReplaceOrderIn);

    virtual bool Process(char *buff);
    virtual uint16 GetMessageSize()
    {
        return INMESSAGESIZE;
    }
    const char* GetSerilizeString()
    {
        return (char*)&m_ReplaceOrderOut;
    }
    uint16 GetSerilizeStringSize()
    {
        return sizeof(ReplaceOrderOut);
    }
private:
    ReplaceOrderOut m_ReplaceOrderOut;
};

#endif //_BROADCAST_HANDLER_IN_H__
