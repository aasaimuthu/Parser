#ifndef _MESSAGE_TYPES_H_
#define _MESSAGE_TYPES_H_

#include<iostream>
#include<cstdint>
#include<map>
#include<string>
#include<memory>

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <cstring>
#include <endian.h>
#include <fstream>
#include <chrono>
#include <sstream>

using namespace std::chrono;
using namespace std;

#pragma pack(push, 1)

#define IN_COMING_MSG_LEN  5000
#define STRADDLE_MSG_LEN   5500
#define STOCK_TICKER_LEN  8
#define LEN_PAD3          3

typedef int8_t    int8;
typedef uint8_t   uint8;
typedef int16_t   int16;
typedef uint16_t  uint16;
typedef int32_t   int32;
typedef uint32_t  uint32;
typedef int64_t   int64;
typedef uint64_t  uint64;

typedef  int32    InPrice;
typedef  double   OutPrice;
typedef  uint32   Size;

const double PRICE_MULTIPLIER=10000;

#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

static uint64 GetTillMidNightEpochTime()
{
    using days = duration<int, ratio<86400>>;
    nanoseconds last_midnight = time_point_cast<days>(system_clock::now()).time_since_epoch();
    return last_midnight.count();
}

static const char *szOutPutFile = "test.out";

typedef map<uint64,InPrice> MapOrderExecutedPrice;

enum enMessageTypeIn
{
	ORDER_UNKNOWN='0',
    NEW_ORDER='A',
    ORDER_EXECUTION='E',
    CANCEL_ORDER='X',
    REPLACE_ORDER='R',
};
enum enMessageTypeOut
{
    NEW_ORDER_OUT=0x01,
    ORDER_EXECUTION_OUT=0x02,
    REDUCED_ORDER_OUT=0x03,
    REPLACE_ORDER_OUT=0x04,
};

struct MissingSequenceMsg
{
    uint16                PacketSize;
    char                  Buffer[IN_COMING_MSG_LEN];
    MissingSequenceMsg(uint16 nlen):PacketSize(nlen)
    {

    }
};

typedef struct
{
    uint16                PacketSize;
    uint32                SequenceNumber;
} MessageHeaderIn;

typedef struct
{
    char                  MessageType;
} MessageTypeIn;


typedef struct
{
    uint16                MessageType;
    uint16                MessageSize;
} MessageHeaderOut;

typedef struct
{

    uint64                TimeStamp;
    uint64                OrderReferenceNumber;
} OrderCommonIn;

typedef struct
{
    MessageHeaderOut       oMessageHeaderOut;
    char                   StockTicker[STOCK_TICKER_LEN];
    uint64                 TimeStamp;
    uint64                 OrderReferenceNumber;
} OrderCommonOut;

typedef struct
{
    OrderCommonIn          oOrderCommonIn;
    char                   Side;
    Size                   oSize;
    char                   StockTicker[STOCK_TICKER_LEN];
    InPrice                oPrice;
} NewOrderIn;

typedef struct
{
    static const enMessageTypeOut MESSAGEID=NEW_ORDER_OUT;
    OrderCommonOut         oOrderCommonOut;
    char                   Side;
    char                   Pad3[LEN_PAD3];
    Size                   oSize;
    OutPrice               oPrice;
} NewOrderOut;

typedef struct
{
    OrderCommonIn          oOrderCommonIn;
    Size                   oSize;
} OrderExecutionIn;

typedef struct
{
    static const enMessageTypeOut MESSAGEID=ORDER_EXECUTION_OUT;
    OrderCommonOut         oOrderCommonOut;
    Size                   oSize;
    OutPrice               oPrice;
} OrderExecutionOut;

typedef struct
{
    OrderCommonIn          oOrderCommonIn;
    Size                   oSize;
} CancelOrderIn;

typedef struct
{
    static const enMessageTypeOut MESSAGEID=REDUCED_ORDER_OUT;
    OrderCommonOut         oOrderCommonOut;
    Size                   oSize;
} ReducedOrderOut;


typedef struct
{
    OrderCommonIn          oOrderCommonIn;
    uint64                 ReplacedOrderReferenceNumber;
    Size                   oSize;
    InPrice                oPrice;
} ReplaceOrderIn;

typedef struct
{
    static const enMessageTypeOut MESSAGEID=REPLACE_ORDER_OUT;
    OrderCommonOut         oOrderCommonOut;
    uint64                 ReplacedOrderReferenceNumber;
    Size                   oSize;
    OutPrice               oPrice;
} ReplaceOrderOut;

#endif //


