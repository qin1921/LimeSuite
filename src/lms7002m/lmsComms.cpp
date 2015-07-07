/**
@file   LMScomms.cpp
@author Lime Microsystems (www.limemicro.com)
@brief  Implementation of data transmission to LMS boards
*/

#include "lmsComms.h"

LMScomms::LMScomms()
{
    unsigned short test = 0x1234;
    unsigned char* bytes = (unsigned char*)&test;
    if(bytes[0] == 0x12 && bytes[1] == 0x34)
        mSystemBigEndian = true;
    else
        mSystemBigEndian = false;
}

LMScomms::~LMScomms()
{
}

/** @brief Transfers data between packet and connected device
    @param pkt packet containing output data and to receive incomming data
    @return 1: success, 0:failed, -1:Port not opened, -2:Connection Manager not initialized
*/
LMScomms::TransferStatus LMScomms::TransferPacket(GenericPacket& pkt)
{
	TransferStatus status = TRANSFER_SUCCESS;
    if(IsOpen() == false)
        return NOT_CONNECTED;

    int packetLen;
    eLMS_PROTOCOL protocol = LMS_PROTOCOL_LMS64C;
    switch(protocol)
    {
    case LMS_PROTOCOL_UNDEFINED:
        return TRANSFER_FAILED;
    case LMS_PROTOCOL_LMS64C:
        packetLen = ProtocolLMS64C::pktLength;
        break;
    }
    int outLen = 0;
    unsigned char* outBuffer = NULL;
    outBuffer = PreparePacket(pkt, outLen, protocol);
    unsigned char* inBuffer = new unsigned char[outLen];
    memset(inBuffer, 0, outLen);

    int outBufPos = 0;
    int inDataPos = 0;
    if(outLen == 0)
    {
        //printf("packet outlen = 0\n");
        outLen = 1;
    }

    for(int i=0; i<outLen; i+=packetLen)
    {
        int bytesToSend = packetLen;
        if( Write(&outBuffer[outBufPos], bytesToSend) )
        {
            outBufPos += packetLen;
            long readLen = packetLen;
            int bread = Read(&inBuffer[inDataPos], readLen);
            if(bread != readLen)
            {
                status = TRANSFER_FAILED;
                break;
            }
            inDataPos += readLen;
        }
        else
        {
            status = TRANSFER_FAILED;
            break;
        }
    }
    pkt.inLen = inDataPos;
    ParsePacket(pkt, inBuffer, inDataPos, protocol);
    delete outBuffer;
    delete inBuffer;
//    lmSem_post(m_lock);
    return status;
}

LMSinfo LMScomms::GetInfo()
{
    GenericPacket pkt;
    pkt.cmd = CMD_GET_INFO;
    pkt.outLen = 1;
    TransferPacket(pkt);
    LMSinfo info;
    info.firmware = pkt.inBuffer[0];
    info.device = pkt.inBuffer[1] < LMS_DEV_COUNT ? (eLMS_DEV)pkt.inBuffer[1] : LMS_DEV_UNKNOWN;
    info.protocol = pkt.inBuffer[2];
    info.hardware = pkt.inBuffer[3];
    info.expansion = pkt.inBuffer[4] < EXP_BOARD_COUNT ? (eEXP_BOARD)pkt.inBuffer[4]: EXP_BOARD_UNKNOWN;
    return info;
}

/** @brief Takes generic packet and converts to specific protocol buffer
    @param pkt generic data packet to convert
    @param length returns length of returned buffer
    @param protocol which protocol to use for data
    @return pointer to data buffer, must be manually deleted after use
*/
unsigned char* LMScomms::PreparePacket(const GenericPacket& pkt, int& length, const eLMS_PROTOCOL protocol)
{
    unsigned char* buffer = NULL;
    if(protocol == LMS_PROTOCOL_UNDEFINED)
        return NULL;

    if(protocol == LMS_PROTOCOL_LMS64C)
    {
        ProtocolLMS64C packet;
        int maxDataLength = packet.maxDataLength;
        packet.cmd = pkt.cmd;
        packet.status = pkt.status;
        int byteBlockRatio = 1; //block ratio - how many bytes in one block
        switch( packet.cmd )
        {
        case CMD_PROG_MCU:
        //case CMD_LMS_RST:
        case CMD_GET_INFO:
        case CMD_SI5351_RD:
        case CMD_SI5356_RD:        
            byteBlockRatio = 1;
            break;
        case CMD_SI5351_WR:
        case CMD_SI5356_WR:
            byteBlockRatio = 2;
            break;
        case CMD_LMS7002_RD:
        case CMD_BRDSPI_RD:
        case CMD_BRDSPI8_RD:        
            byteBlockRatio = 2;
            break;
        case CMD_ADF4002_WR:
            byteBlockRatio = 3;
            break;
        case CMD_LMS7002_WR:
        case CMD_BRDSPI_WR:        
            byteBlockRatio = 4;
            break;
        default:
            byteBlockRatio = 1;
        }
        if (packet.cmd == CMD_LMS7002_RD || packet.cmd == CMD_BRDSPI_RD)
            maxDataLength = maxDataLength/2;
        int blockCount = pkt.outLen/byteBlockRatio;
        int bufLen = blockCount/(maxDataLength/byteBlockRatio)
                    +(blockCount%(maxDataLength/byteBlockRatio)!=0);
        bufLen *= packet.pktLength;
        if(bufLen == 0)
            bufLen = packet.pktLength;
        buffer = new unsigned char[bufLen];
        memset(buffer, 0, bufLen);
        int srcPos = 0;
        for(int j=0; j*packet.pktLength<bufLen; ++j)
        {
            int pktPos = j*packet.pktLength;
            buffer[pktPos] = packet.cmd;
            buffer[pktPos+1] = packet.status;
            if(blockCount > (maxDataLength/byteBlockRatio))
            {
                buffer[pktPos+2] = maxDataLength/byteBlockRatio;
                blockCount -= buffer[pktPos+2];
            }
            else
                buffer[pktPos+2] = blockCount;
            memcpy(&buffer[pktPos+3], packet.reserved, sizeof(packet.reserved));
            int bytesToPack = (maxDataLength/byteBlockRatio)*byteBlockRatio;
            memcpy(&buffer[pktPos+8], &pkt.outBuffer[srcPos], bytesToPack);            
            srcPos += bytesToPack;
        }
        length = bufLen;
    }
    return buffer;
}

/** @brief Parses given data buffer into generic packet
    @param pkt destination packet
    @param buffer received data from board
    @param length received buffer length
    @param protocol which protocol to use for data parsing
    @return 1:success, 0:failure
*/
int LMScomms::ParsePacket(GenericPacket& pkt, const unsigned char* buffer, const int length, const eLMS_PROTOCOL protocol)
{
    if(protocol == LMS_PROTOCOL_UNDEFINED)
        return -1;

    if(protocol == LMS_PROTOCOL_LMS64C)
    {
        ProtocolLMS64C packet;
        int inBufPos = 0;
        for(int i=0; i<length; i+=packet.pktLength)
        {
            pkt.cmd = (eCMD_LMS)buffer[i];
            pkt.status = (eCMD_STATUS)buffer[i+1];
            memcpy(&pkt.inBuffer[inBufPos], &buffer[i+8], packet.maxDataLength);
            inBufPos += packet.maxDataLength;
        }
        pkt.inLen = inBufPos;
    }
    return 1;
}
