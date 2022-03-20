#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser(int date, const std::string &outputFilename):m_bUDFInit(false)
{
	m_bUDFInit = m_CUDPFeed.Init();
    if(m_bUDFInit)
        printf("ERROr:CUDPFeed is not initialized\n");
}


void Parser::onUDPPacket(const char *buf, size_t len)
{
    printf("Received packet of size %zu\n", len);
    if(m_bUDFInit)
        m_CUDPFeed.ProcessMessage(buf,len);
}
