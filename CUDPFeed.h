#ifndef _BROADCAST_HANDLER_H_
#define _BROADCAST_HANDLER_H_

#include "CBroadCastHandlerBase.h"

typedef shared_ptr<CBroadCastHandlerBase>              CBroadCastHandlerBasePtr;
typedef map<enMessageTypeIn, CBroadCastHandlerBasePtr>  MapCharToBroadCastHdrTypes;

typedef map<uint32,shared_ptr<MissingSequenceMsg> >    MapMissingSequenceMessage;
class CUDPFeed
{
public:
    CUDPFeed():m_eCurrProcessingMsgType(ORDER_UNKNOWN),m_StraddleMessageFound(false),m_nStraddleMsgLen(-1),m_lLastSequenceNumber(0)
    {
    }
    virtual ~CUDPFeed()
    {
        m_oStreamOut.close();
    }
    bool Init();
    void ProcessMessage(const char *buf, size_t len);
    void ProcessPacket(char *buf, size_t len);
    void WriteIntoFile( const char *buf, size_t len);
    std::shared_ptr<CBroadCastHandlerBase> GetHandler(enMessageTypeIn eMsgType);
    bool CheckStraddlePacket(char *msg, uint16 nInPacketLen, uint16 nUsedPacketLen );
    char* ProcessStraddleMessage(char *msg, uint16 &nInPcketLen, uint16 &nMsgLength);
    void ProcessInSequnceMsgOrder(char *buf, uint32 nSeqNum,size_t len);
private:
    MapCharToBroadCastHdrTypes m_mapCharToBroadCastHdrTypes;
    ofstream                   m_oStreamOut;
    enMessageTypeIn            m_eCurrProcessingMsgType;
    bool                       m_StraddleMessageFound;
    char                       m_szStraddleBuffer[STRADDLE_MSG_LEN];
    uint16                     m_nStraddleMsgLen;
    MapMissingSequenceMessage  m_mapMissingSequenceMessage;
    uint32                     m_lLastSequenceNumber;

};
#endif //_BROADCAST_HANDLER_H_
