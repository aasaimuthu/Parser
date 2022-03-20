#include "CUDPFeed.h"
#include "CBroadCastHandler.h"

bool CUDPFeed::Init()
{
	strncpy(m_szStraddleBuffer,"",sizeof(m_szStraddleBuffer));
    m_oStreamOut.open(szOutPutFile,ios_base::binary | std::ofstream::app);
    if(!m_oStreamOut.is_open())
    {
        cout <<"Couldn't open the file " <<  szOutPutFile << endl;
        return false;
    }

    shared_ptr<CNewOrderMsgIn> pNewOrdIn(new CNewOrderMsgIn());
    m_mapCharToBroadCastHdrTypes.insert(make_pair(CNewOrderMsgIn::MESSAGEID, pNewOrdIn));

    shared_ptr<COrderExecutionIn> pOrdExeIn(new COrderExecutionIn());
    m_mapCharToBroadCastHdrTypes.insert(make_pair(COrderExecutionIn::MESSAGEID, pOrdExeIn));

    shared_ptr<CReplaceOrderIn> pReplaceOrdIn(new CReplaceOrderIn());
    m_mapCharToBroadCastHdrTypes.insert(make_pair(CReplaceOrderIn::MESSAGEID, pReplaceOrdIn));

    shared_ptr<CCancelOrderIn> pCancelOrdIn(new CCancelOrderIn());
    m_mapCharToBroadCastHdrTypes.insert(make_pair(CCancelOrderIn::MESSAGEID, pCancelOrdIn));
    return true;
}
std::shared_ptr<CBroadCastHandlerBase> CUDPFeed::GetHandler(enMessageTypeIn eMsgType)
{
    auto it = m_mapCharToBroadCastHdrTypes.find(eMsgType);
    if(it != m_mapCharToBroadCastHdrTypes.end())
    {
        return it->second;
    }
    return CBroadCastHandlerBasePtr();
}
void CUDPFeed::WriteIntoFile( const char *buf, size_t len)
{
    m_oStreamOut.write(buf, len);
}

char* CUDPFeed::ProcessStraddleMessage( char *msg, uint16 &nInPcketLen, uint16 &nMsgLength )
{
    char *temp = m_szStraddleBuffer;
    memcpy (m_szStraddleBuffer+m_nStraddleMsgLen, msg, (nInPcketLen - nMsgLength));
    nInPcketLen = nInPcketLen + m_nStraddleMsgLen;
    msg = temp;
    CBroadCastHandlerBasePtr ptrMessage = GetHandler(m_eCurrProcessingMsgType);
    if(ptrMessage)
    {
        ptrMessage->Process(msg);
        WriteIntoFile(ptrMessage->GetSerilizeString(),ptrMessage->GetSerilizeStringSize());
        msg = msg+ptrMessage->GetMessageSize();
        nMsgLength +=ptrMessage->GetMessageSize();
    }
    else
    {
        cout << __LINE__  << "Unknown Mesasge and don't know the length so not processing further packets from this message" << endl;
    }
    m_StraddleMessageFound = false;
    m_nStraddleMsgLen      = -1;
    return msg;
}

bool CUDPFeed::CheckStraddlePacket(char *msg, uint16 nInPacketLen, uint16 nUsedPacketLen )
{
    uint16 nAvailPacket = nInPacketLen - nUsedPacketLen;
    if( nAvailPacket < COrderExecutionIn::INMESSAGESIZE) //minimum expected size is 20
    {
        memcpy(m_szStraddleBuffer, msg , nAvailPacket);
        m_nStraddleMsgLen      = nAvailPacket;
        m_StraddleMessageFound = true;
        return true;
    }
    return false;
}

void CUDPFeed::ProcessInSequnceMsgOrder(char *buf, uint32 nCurrentSeqNum ,size_t len)
{
    if (nCurrentSeqNum <= m_lLastSequenceNumber)
        return;

    bool nMismatchFound=false;
    if(nCurrentSeqNum > m_lLastSequenceNumber)
    {
        //is first msg sequence num
        if(m_lLastSequenceNumber == 0 )
        {
            if(m_lLastSequenceNumber+1 == nCurrentSeqNum)
            {
                m_lLastSequenceNumber = nCurrentSeqNum;
                nMismatchFound = false;
            }
            else
            {
                cout <<__LINE__ << " Found sequence mismathing " << nCurrentSeqNum << " m_lLastSequenceNumber:" << m_lLastSequenceNumber << " len:" << len  << endl;
                shared_ptr<MissingSequenceMsg> pMsg(new MissingSequenceMsg(len));
                if(pMsg)
                {
                    memset (pMsg->Buffer,0,IN_COMING_MSG_LEN);
                    memcpy(pMsg->Buffer,buf,IN_COMING_MSG_LEN);
                    m_mapMissingSequenceMessage.insert(make_pair(nCurrentSeqNum, pMsg));
                    nMismatchFound = true;
                }
            }
        }
        else if(nCurrentSeqNum == m_lLastSequenceNumber+1)
        {
            m_lLastSequenceNumber = nCurrentSeqNum;
            nMismatchFound =  false;
        }
        else if(nCurrentSeqNum > m_lLastSequenceNumber+1)
        {
            cout <<__LINE__ << " Found sequence mismathing " << nCurrentSeqNum << " m_lLastSequenceNumber:" << m_lLastSequenceNumber  << " len:" << len << endl;
            shared_ptr<MissingSequenceMsg> pMsg(new MissingSequenceMsg(len));
            if(pMsg)
            {
                memset (pMsg->Buffer,0,IN_COMING_MSG_LEN);
                memcpy(pMsg->Buffer,buf,IN_COMING_MSG_LEN);
                m_mapMissingSequenceMessage.insert(make_pair(nCurrentSeqNum, pMsg));
                nMismatchFound =  true;
            }
        }
    }
    if(!nMismatchFound)
    {
        ProcessPacket(buf,len);
        size_t nTotalMssingMsg = m_mapMissingSequenceMessage.size();
        if(nTotalMssingMsg > 0)
        {
            for(uint32 nMissingMsg=1; nMissingMsg <=nTotalMssingMsg; ++nMissingMsg)
            {
                MapMissingSequenceMessage::const_iterator it = m_mapMissingSequenceMessage.find(nCurrentSeqNum+nMissingMsg);
                if(it != m_mapMissingSequenceMessage.end())
                {
                    ProcessPacket(it->second->Buffer,it->second->PacketSize);
                    m_mapMissingSequenceMessage.erase(it);
                    m_lLastSequenceNumber = m_lLastSequenceNumber + nCurrentSeqNum + nMissingMsg;
                }
                else
                {
                    //If next sequence missing then ignore further checking
                    break;
                }
            }
        }
    }
}

void CUDPFeed::ProcessMessage(const char *buf, size_t len)
{
    if(len <=0 )
        return;
    char  *msg = const_cast<char*>(buf);
    MessageHeaderIn *pInMsg = reinterpret_cast<MessageHeaderIn*>(msg);
    if(!pInMsg)
        return;
    uint32 nSequenceNumber     = htonl(pInMsg->SequenceNumber);
    ProcessInSequnceMsgOrder(msg, nSequenceNumber, len);
}

void CUDPFeed::ProcessPacket(char *buf, size_t len)
{
    if(len <=0 )
       return;

    char  *msg = const_cast<char*>(buf);
    MessageHeaderIn *pInMsg = reinterpret_cast<MessageHeaderIn*>(msg);
    if(!pInMsg)
        return;

    msg = msg+sizeof(MessageHeaderIn);
    uint32 nSequenceNumber     =  htonl(pInMsg->SequenceNumber);
    uint16 nInPcketSize        =  htons(pInMsg->PacketSize);

    uint16          nMsgLength           = sizeof(MessageHeaderIn);

    while(nMsgLength < nInPcketSize)
    {
        MessageTypeIn *pMsgType =  reinterpret_cast<MessageTypeIn*>(msg);
        if(pMsgType)
        {
            msg = msg+sizeof(MessageTypeIn);
            nMsgLength +=sizeof(MessageTypeIn);
            if( m_StraddleMessageFound &&  m_nStraddleMsgLen > 0 && m_eCurrProcessingMsgType == (enMessageTypeIn)pMsgType->MessageType )
            {
                char *tmp=msg;
                msg = ProcessStraddleMessage(tmp,nInPcketSize,nMsgLength);
                continue;
            }
        }
        m_eCurrProcessingMsgType = (enMessageTypeIn)pMsgType->MessageType;

        if(CheckStraddlePacket(msg,nInPcketSize,nMsgLength))
        {
            cout << __LINE__ << "Found straddle message handling in next UDP message:" << (char)m_eCurrProcessingMsgType << " packetdiff:" << (nInPcketSize - nMsgLength)  << endl;
            break;
        }
        CBroadCastHandlerBasePtr ptrMessage = GetHandler(m_eCurrProcessingMsgType);
        if(ptrMessage)
        {
            ptrMessage->Process(msg);
            WriteIntoFile(ptrMessage->GetSerilizeString(),ptrMessage->GetSerilizeStringSize());
            msg = msg+ptrMessage->GetMessageSize();;
            nMsgLength +=ptrMessage->GetMessageSize();
        }
        else
        {
            cout << __LINE__ << "Unknown Mesasge and don't know the length so not processing further packets from this message" << endl;
            break;
        }
    }
}
