#ifndef _BROADCAST_HANDLER_BASE_H_
#define _BROADCAST_HANDLER_BASE_H_

#include "CMessageTypes.h"

class CBroadCastHandlerBase
{
public:
    virtual            ~CBroadCastHandlerBase(){}
    virtual bool        Process(char *msg) = 0;
    virtual uint16      GetMessageSize() = 0;
    virtual const char* GetSerilizeString()=0;
    virtual uint16      GetSerilizeStringSize()=0;
protected:
    MapOrderExecutedPrice m_mapOrderExecutedPrice;
};
#endif 
