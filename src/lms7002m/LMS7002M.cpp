/**
@file LMS7002M.cpp
@author Lime Microsystems (www.limemicro.com)
@brief Implementation of LMS7002M transceiver configuring
*/

#include "LMS7002M.h"
#include <stdio.h>
#include <set>
#include "lmsComms.h"
#include "iniParser.h"
#include <cmath>

#define PI 3.14159265359

///define for parameter enumeration if prefix might be needed
#define LMS7param(id) id

//module addresses needs to be sorted in ascending order
const uint16_t LMS7002M::moduleAddresses[] = { 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0081, 0x0082, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F, 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010C, 0x010D, 0x010E, 0x010F, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011A, 0x011C, 0x011D, 0x011E, 0x011F, 0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0200, 0x0201, 0x0202, 0x0203, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208, 0x0209, 0x020A, 0x020B, 0x020C, 0x0240, 0x0241, 0x0242, 0x0243, 0x0244, 0x0245, 0x0246, 0x0247, 0x0248, 0x0249, 0x024A, 0x024B, 0x024C, 0x024D, 0x024E, 0x024F, 0x0250, 0x0251, 0x0252, 0x0253, 0x0254, 0x0255, 0x0256, 0x0257, 0x0258, 0x0259, 0x025A, 0x025B, 0x025C, 0x025D, 0x025E, 0x025F, 0x0260, 0x0261, 0x0280, 0x0281, 0x0282, 0x0283, 0x0284, 0x0285, 0x0286, 0x0287, 0x0288, 0x0289, 0x028A, 0x028B, 0x028C, 0x028D, 0x028E, 0x028F, 0x0290, 0x0291, 0x0292, 0x0293, 0x0294, 0x0295, 0x0296, 0x0297, 0x0298, 0x0299, 0x029A, 0x029B, 0x029C, 0x029D, 0x029E, 0x029F, 0x02A0, 0x02A1, 0x02A2, 0x02A3, 0x02A4, 0x02A5, 0x02A6, 0x02A7, 0x02C0, 0x02C1, 0x02C2, 0x02C3, 0x02C4, 0x02C5, 0x02C6, 0x02C7, 0x02C8, 0x02C9, 0x02CA, 0x02CB, 0x02CC, 0x02CD, 0x02CE, 0x02CF, 0x02D0, 0x02D1, 0x02D2, 0x02D3, 0x02D4, 0x02D5, 0x02D6, 0x02D7, 0x02D8, 0x02D9, 0x02DA, 0x02DB, 0x02DC, 0x02DD, 0x02DE, 0x02DF, 0x02E0, 0x02E1, 0x02E2, 0x02E3, 0x02E4, 0x02E5, 0x02E6, 0x02E7, 0x0300, 0x0301, 0x0302, 0x0303, 0x0304, 0x0305, 0x0306, 0x0307, 0x0308, 0x0309, 0x030A, 0x030B, 0x030C, 0x030D, 0x030E, 0x030F, 0x0310, 0x0311, 0x0312, 0x0313, 0x0314, 0x0315, 0x0316, 0x0317, 0x0318, 0x0319, 0x031A, 0x031B, 0x031C, 0x031D, 0x031E, 0x031F, 0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0325, 0x0326, 0x0327, 0x0340, 0x0341, 0x0342, 0x0343, 0x0344, 0x0345, 0x0346, 0x0347, 0x0348, 0x0349, 0x034A, 0x034B, 0x034C, 0x034D, 0x034E, 0x034F, 0x0350, 0x0351, 0x0352, 0x0353, 0x0354, 0x0355, 0x0356, 0x0357, 0x0358, 0x0359, 0x035A, 0x035B, 0x035C, 0x035D, 0x035E, 0x035F, 0x0360, 0x0361, 0x0362, 0x0363, 0x0364, 0x0365, 0x0366, 0x0367, 0x0380, 0x0381, 0x0382, 0x0383, 0x0384, 0x0385, 0x0386, 0x0387, 0x0388, 0x0389, 0x038A, 0x038B, 0x038C, 0x038D, 0x038E, 0x038F, 0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A2, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7, 0x0400, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x040D, 0x040E, 0x040F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x045D, 0x045E, 0x045F, 0x0460, 0x0461, 0x0480, 0x0481, 0x0482, 0x0483, 0x0484, 0x0485, 0x0486, 0x0487, 0x0488, 0x0489, 0x048A, 0x048B, 0x048C, 0x048D, 0x048E, 0x048F, 0x0490, 0x0491, 0x0492, 0x0493, 0x0494, 0x0495, 0x0496, 0x0497, 0x0498, 0x0499, 0x049A, 0x049B, 0x049C, 0x049D, 0x049E, 0x049F, 0x04A0, 0x04A1, 0x04A2, 0x04A3, 0x04A4, 0x04A5, 0x04A6, 0x04A7, 0x04C0, 0x04C1, 0x04C2, 0x04C3, 0x04C4, 0x04C5, 0x04C6, 0x04C7, 0x04C8, 0x04C9, 0x04CA, 0x04CB, 0x04CC, 0x04CD, 0x04CE, 0x04CF, 0x04D0, 0x04D1, 0x04D2, 0x04D3, 0x04D4, 0x04D5, 0x04D6, 0x04D7, 0x04D8, 0x04D9, 0x04DA, 0x04DB, 0x04DC, 0x04DD, 0x04DE, 0x04DF, 0x04E0, 0x04E1, 0x04E2, 0x04E3, 0x04E4, 0x04E5, 0x04E6, 0x04E7, 0x0500, 0x0501, 0x0502, 0x0503, 0x0504, 0x0505, 0x0506, 0x0507, 0x0508, 0x0509, 0x050A, 0x050B, 0x050C, 0x050D, 0x050E, 0x050F, 0x0510, 0x0511, 0x0512, 0x0513, 0x0514, 0x0515, 0x0516, 0x0517, 0x0518, 0x0519, 0x051A, 0x051B, 0x051C, 0x051D, 0x051E, 0x051F, 0x0520, 0x0521, 0x0522, 0x0523, 0x0524, 0x0525, 0x0526, 0x0527, 0x0540, 0x0541, 0x0542, 0x0543, 0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B, 0x054C, 0x054D, 0x054E, 0x054F, 0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555, 0x0556, 0x0557, 0x0558, 0x0559, 0x055A, 0x055B, 0x055C, 0x055D, 0x055E, 0x055F, 0x0560, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567, 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0587, 0x0588, 0x0589, 0x058A, 0x058B, 0x058C, 0x058D, 0x058E, 0x058F, 0x0590, 0x0591, 0x0592, 0x0593, 0x0594, 0x0595, 0x0596, 0x0597, 0x0598, 0x0599, 0x059A, 0x059B, 0x059C, 0x059D, 0x059E, 0x059F, 0x05A0, 0x05A1, 0x05A2, 0x05A3, 0x05A4, 0x05A5, 0x05A6, 0x05A7 };
const uint16_t LMS7002M::defaultValues[] =   { 0xFFFD, 0x0E9F, 0x07DF, 0x5559, 0xE4E4, 0x0101, 0x0101, 0xE4E4, 0x0101, 0x0101, 0x0086, 0x0010, 0xFFFF, 0xFFFF, 0x0000, 0x3840, 0x0000, 0x800B, 0x0400, 0x0001, 0x4901, 0x0000, 0x0530, 0x00A0, 0x0514, 0x2100, 0x067B, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6565, 0x658C, 0x6565, 0x658C, 0x6565, 0x658C, 0x658C, 0x6565, 0x6565, 0x6565, 0x6565, 0x6565, 0x6565, 0x0001, 0x6565, 0x0000, 0x8000, 0x0000, 0x0040, 0x2000, 0x03FF, 0x0000, 0x3409, 0x7800, 0x3180, 0x0A12, 0x0088, 0x0007, 0x318C, 0x318C, 0x9426, 0x61C1, 0x104C, 0x88FD, 0x009E, 0x2040, 0x3042, 0x0BF4, 0x0083, 0xC0E6, 0x03C3, 0x008D, 0x0009, 0x8180, 0x280C, 0x018C, 0x18CB, 0x2E02, 0xAD43, 0x0400, 0x0780, 0x3640, 0xB9FF, 0x3404, 0x033F, 0x067B, 0x0000, 0x0081, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x4000, 0x7FFF, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0081, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
const uint16_t LMS7002M::readOnlyRegisters[] =      { 0x002F, 0x008C, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x0123, 0x0209, 0x020A, 0x020B, 0x040E, 0x040F };
const uint16_t LMS7002M::readOnlyRegistersMasks[] = { 0x0000, 0x0FFF, 0x007F, 0x0000, 0x0000, 0x0000, 0x0000, 0x003F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/** @brief Simple logging function to print status messages
    @param text message to print
    @param type message type for filtering specific information
*/
void LMS7002M::Log(const char* text, LogType type)
{
    switch(type)
    {
    case LOG_INFO:
        printf("%s\n", text);
        break;
    case LOG_WARNING:
        printf("Warning: %s\n", text);
        break;
    case LOG_ERROR:
        printf("ERROR: %s\n", text);
        break; 
    case LOG_DATA:
        printf("DATA: %s\n", text);
        break;
    }
}

LMS7002M::LMS7002M() : controlPort(NULL)
{   
    mRefClkSXR_MHz = 30.72;
    mRefClkSXT_MHz = 30.72;
}

/** @brief Creates LMS7002M main control object, it requires LMScomms to communicate with chip
    @param controlPort data connection for controlling LMS7002 chip registers
    @param dataPort data connection for reading RF sampling data (optional)
*/
LMS7002M::LMS7002M(LMScomms* controlPort, LMScomms* dataPort) : controlPort(controlPort)
{
    mRefClkSXR_MHz = 30.72;
    mRefClkSXT_MHz = 30.72;

    //memory intervals for registers tests and calibration algorithms
    MemorySectionAddresses[LimeLight][0] = 0x0020;
    MemorySectionAddresses[LimeLight][1] = 0x002F;
    MemorySectionAddresses[EN_DIR][0] = 0x0081;
    MemorySectionAddresses[EN_DIR][1] = 0x0081;
    MemorySectionAddresses[AFE][0] = 0x0082;
    MemorySectionAddresses[AFE][1] = 0x0082;
    MemorySectionAddresses[BIAS][0] = 0x0084;
    MemorySectionAddresses[BIAS][1] = 0x0084;
    MemorySectionAddresses[XBUF][0] = 0x0085;
    MemorySectionAddresses[XBUF][1] = 0x0085;
    MemorySectionAddresses[CGEN][0] = 0x0086;
    MemorySectionAddresses[CGEN][1] = 0x008C;
    MemorySectionAddresses[LDO][0] = 0x0092;
    MemorySectionAddresses[LDO][1] = 0x00A7;
    MemorySectionAddresses[BIST][0] = 0x00A8;
    MemorySectionAddresses[BIST][1] = 0x00AC;
    MemorySectionAddresses[CDS][0] = 0x00AD;
    MemorySectionAddresses[CDS][1] = 0x00AE;
    MemorySectionAddresses[TRF][0] = 0x0100;
    MemorySectionAddresses[TRF][1] = 0x0104;
    MemorySectionAddresses[TBB][0] = 0x0105;
    MemorySectionAddresses[TBB][1] = 0x010A;
    MemorySectionAddresses[RFE][0] = 0x010C;
    MemorySectionAddresses[RFE][1] = 0x0114;
    MemorySectionAddresses[RBB][0] = 0x0115;
    MemorySectionAddresses[RBB][1] = 0x011A;
    MemorySectionAddresses[SX][0] = 0x011C;
    MemorySectionAddresses[SX][1] = 0x0124;
    MemorySectionAddresses[TxTSP][0] = 0x0200;
    MemorySectionAddresses[TxTSP][1] = 0x020C;
    MemorySectionAddresses[TxNCO][0] = 0x0240;
    MemorySectionAddresses[TxNCO][1] = 0x0261;
    MemorySectionAddresses[TxGFIR1][0] = 0x0280;
    MemorySectionAddresses[TxGFIR1][1] = 0x02A7;
    MemorySectionAddresses[TxGFIR2][0] = 0x02C0;
    MemorySectionAddresses[TxGFIR2][1] = 0x02E7;
    MemorySectionAddresses[TxGFIR3a][0] = 0x0300;
    MemorySectionAddresses[TxGFIR3a][1] = 0x0327;
    MemorySectionAddresses[TxGFIR3b][0] = 0x0340;
    MemorySectionAddresses[TxGFIR3b][1] = 0x0367;
    MemorySectionAddresses[TxGFIR3c][0] = 0x0380;
    MemorySectionAddresses[TxGFIR3c][1] = 0x03A7;
    MemorySectionAddresses[RxTSP][0] = 0x0400;
    MemorySectionAddresses[RxTSP][1] = 0x040F;
    MemorySectionAddresses[RxNCO][0] = 0x0440;
    MemorySectionAddresses[RxNCO][1] = 0x0461;
    MemorySectionAddresses[RxGFIR1][0] = 0x0480;
    MemorySectionAddresses[RxGFIR1][1] = 0x04A7;
    MemorySectionAddresses[RxGFIR2][0] = 0x04C0;
    MemorySectionAddresses[RxGFIR2][1] = 0x04E7;
    MemorySectionAddresses[RxGFIR3a][0] = 0x0500;
    MemorySectionAddresses[RxGFIR3a][1] = 0x0527;
    MemorySectionAddresses[RxGFIR3b][0] = 0x0540;
    MemorySectionAddresses[RxGFIR3b][1] = 0x0567;
    MemorySectionAddresses[RxGFIR3c][0] = 0x0580;
    MemorySectionAddresses[RxGFIR3c][1] = 0x05A7;

    mLocalRegistersCopy = new uint16_t*[2];
    mLocalRegistersCopy[0] = new uint16_t[sizeof(moduleAddresses) / sizeof(uint16_t)];    
    mLocalRegistersCopy[1] = new uint16_t[sizeof(moduleAddresses) / sizeof(uint16_t)];
    //initialize local registers copy to default values
    for (int i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
    {
        mLocalRegistersCopy[0][i] = defaultValues[i];
        mLocalRegistersCopy[1][i] = defaultValues[i];
    }
}

LMS7002M::~LMS7002M()
{

}

/** @brief Sends reset signal to chip, after reset enables B channel controls
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::ResetChip()
{
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;

    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_LMS7002_RST;
    pkt.outBuffer[0] = LMS_RST_PULSE;
    pkt.outLen = 1;
    controlPort->TransferPacket(pkt);
    if (pkt.status == STATUS_COMPLETED_CMD)
    {
        Modify_SPI_Reg_bits(LMS7param(MIMO_SISO), 0); //enable B channel after reset
        return LIBLMS7_SUCCESS;
    }
    else
        return LIBLMS7_FAILURE;
}

/** @brief Reads configuration file and uploads registers to chip
    @param filename Configuration source file
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::LoadConfig(const char* filename)
{
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC)); //remember used channel

    liblms7_status status;
    iniParser parser;
    parser.Open(filename);
    parser.SelectSection("file_info");
    string type = "";
    type = parser.Get("type", "undefined");
    stringstream ss;
    if (type != "lms7002m_minimal_config")
    {
        ss << "File " << filename << " not recognized" << endl;
        return LIBLMS7_FAILURE;
    }

    int fileVersion = 0;
    fileVersion = parser.Get("version", 0);

    vector<uint16_t> addrToWrite;
    vector<uint16_t> dataToWrite;

    if (fileVersion == 1)
    {
        parser.SelectSection("lms7002_registers_a");
        map<string, string>::iterator pairs;
        map<string, string> regValues = parser.GetCurrentSection();

        uint16_t addr = 0;
        uint16_t value = 0;
        uint16_t x0020_value = 0;
        Modify_SPI_Reg_bits(LMS7param(MAC), 1); //select A channel
        for (pairs = regValues.begin(); pairs != regValues.end(); ++pairs)
        {
            sscanf(pairs->first.c_str(), "%hx", &addr);
            sscanf(pairs->second.c_str(), "%hx", &value);
            if (addr == LMS7param(MAC).address) //skip register containing channel selection
            {
                x0020_value = value;
                continue;
            }
            addrToWrite.push_back(addr);
            dataToWrite.push_back(value);
        }        
        status = SPI_write_batch(&addrToWrite[0], &dataToWrite[0], addrToWrite.size());        
        status = LIBLMS7_SUCCESS;
        if (status != LIBLMS7_SUCCESS)
            return status;
        status = SPI_write(0x0020, x0020_value);
        if (status != LIBLMS7_SUCCESS)
            return status;
        Modify_SPI_Reg_bits(LMS7param(MAC), 2);
        if (status != LIBLMS7_SUCCESS)
            return status;

        parser.SelectSection("lms7002_registers_b");
        regValues.clear();
        regValues = parser.GetCurrentSection();

        addrToWrite.clear();
        dataToWrite.clear();
        for (pairs = regValues.begin(); pairs != regValues.end(); ++pairs)
        {
            sscanf(pairs->first.c_str(), "%hx", &addr);
            sscanf(pairs->second.c_str(), "%hx", &value);
            addrToWrite.push_back(addr);
            dataToWrite.push_back(value);
        }
        Modify_SPI_Reg_bits(LMS7param(MAC), 2); //select B channel
        status = SPI_write_batch(&addrToWrite[0], &dataToWrite[0], addrToWrite.size());
        if (status != LIBLMS7_SUCCESS)
            return status;
        Modify_SPI_Reg_bits(LMS7param(MAC), ch);

        parser.SelectSection("reference_clocks");
        mRefClkSXR_MHz = parser.Get("sxr_ref_clk_mhz", 30.72);
        mRefClkSXT_MHz = parser.Get("sxt_ref_clk_mhz", 30.72);
    }

    Modify_SPI_Reg_bits(MAC, 1);
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;
    return LIBLMS7_SUCCESS;
}

/** @brief Reads all registers from chip and saves to file
    @param filename destination filename
    @return 0-success, other failure
*/
liblms7_status LMS7002M::SaveConfig(const char* filename)
{   
    liblms7_status status;
    iniParser parser;
    parser.SelectSection("file_info");
    parser.Set("type", "lms7002m_minimal_config");
    parser.Set("version", 1);

    char addr[80];
    char value[80];

    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));

    vector<uint16_t> addrToRead;
    for (uint8_t i = 0; i < MEMORY_SECTIONS_COUNT; ++i)
        for (uint16_t addr = MemorySectionAddresses[i][0]; addr <= MemorySectionAddresses[i][1]; ++addr)
            addrToRead.push_back(addr);
    vector<uint16_t> dataReceived;
    dataReceived.resize(addrToRead.size(), 0);

    parser.SelectSection("lms7002_registers_a");
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    
    for (uint16_t i = 0; i < addrToRead.size(); ++i)
    {        
        sprintf(addr, "0x%04X", addrToRead[i]);
        sprintf(value, "0x%04X", dataReceived[i]);                
        parser.Set(addr, value);
    }

    parser.SelectSection("lms7002_registers_b");
    addrToRead.clear(); //add only B channel addresses
    for (uint8_t i = 0; i < MEMORY_SECTIONS_COUNT; ++i)
        for (uint16_t addr = MemorySectionAddresses[i][0]; addr <= MemorySectionAddresses[i][1]; ++addr)
            if (addr >= 0x0100)
                addrToRead.push_back(addr);

    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    
    for (uint16_t i = 0; i < addrToRead.size(); ++i)
    {
        sprintf(addr, "0x%04X", addrToRead[i]);
        sprintf(value, "0x%04X", dataReceived[i]);
        parser.Set(addr, value);
    }

    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //retore previously used channel

    parser.SelectSection("reference_clocks");
    parser.Set("sxt_ref_clk_mhz", mRefClkSXT_MHz);
    parser.Set("sxr_ref_clk_mhz", mRefClkSXR_MHz);
    parser.Save(filename);
    return LIBLMS7_SUCCESS;
}

/**	@brief Returns reference clock in MHz used for SXT or SXR
	@param Tx transmitter or receiver selection
*/
float_type LMS7002M::GetReferenceClk_SX(bool tx)
{
	return tx ? mRefClkSXT_MHz : mRefClkSXR_MHz;
}

/**	@return Current CLKGEN frequency in MHz
    Returned frequency depends on reference clock used for Receiver
*/
float_type LMS7002M::GetFrequencyCGEN_MHz()
{
    float_type dMul = (mRefClkSXR_MHz/2.0)/(Get_SPI_Reg_bits(LMS7param(DIV_OUTCH_CGEN))+1); //DIV_OUTCH_CGEN
    uint16_t gINT = Get_SPI_Reg_bits(0x0088, 13, 0); //read whole register to reduce SPI transfers
    uint32_t gFRAC = ((gINT & 0xF) * 65536) | Get_SPI_Reg_bits(0x0087, 15, 0);
    return dMul * (((gINT>>4) + 1 + gFRAC/1048576.0));
}

/** @brief Returns TSP reference frequency
    @param tx TxTSP or RxTSP selection
    @return TSP reference frequency in MHz
*/
float_type LMS7002M::GetReferenceClk_TSP_MHz(bool tx)
{
    float_type cgenFreq = GetFrequencyCGEN_MHz();
	float_type clklfreq = cgenFreq/pow(2.0, Get_SPI_Reg_bits(LMS7param(CLKH_OV_CLKL_CGEN)));
    if(Get_SPI_Reg_bits(LMS7param(EN_ADCCLKH_CLKGN)) == 0)
        return tx ? clklfreq : cgenFreq/4.0;
    else
        return tx ? cgenFreq : clklfreq/4.0;
}

#define CLKGEN_VCO_HIGH 2700
/** @brief Sets CLKGEN frequency, calculations use receiver'r reference clock
    @param freq_MHz desired frequency in MHz
    @return 0-succes, other-cannot deliver desired frequency
*/
liblms7_status LMS7002M::SetFrequencyCGEN(const float_type freq_MHz)
{
    float_type dFvco;
    float_type dFrac;
    int16_t iHdiv;

    //VCO frequency selection according to F_CLKH
	iHdiv = (int16_t)((CLKGEN_VCO_HIGH/2)/freq_MHz)-1;
	dFvco = 2*(iHdiv+1) * freq_MHz;

    //Integer division
    uint16_t gINT = (uint16_t)(dFvco/mRefClkSXR_MHz - 1);

    //Fractional division
    dFrac = dFvco/mRefClkSXR_MHz - (uint32_t)(dFvco/mRefClkSXR_MHz);
    uint32_t gFRAC = (uint32_t)(dFrac * 1048576);

    Modify_SPI_Reg_bits(LMS7param(INT_SDM_CGEN), gINT); //INT_SDM_CGEN
    Modify_SPI_Reg_bits(0x0087, 15, 0, gFRAC&0xFFFF); //INT_SDM_CGEN[15:0]
    Modify_SPI_Reg_bits(0x0088, 3, 0, gFRAC>>16); //INT_SDM_CGEN[19:16]
    Modify_SPI_Reg_bits(LMS7param(DIV_OUTCH_CGEN), iHdiv); //DIV_OUTCH_CGEN

    return TuneVCO(VCO_CGEN);
}

/** @brief Performs VCO tuning operations for CLKGEN, SXR, SXT modules
    @param module module selection for tuning 0-cgen, 1-SXR, 2-SXT
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::TuneVCO(VCO_Module module) // 0-cgen, 1-SXR, 2-SXT
{
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;
	int8_t i;
	uint8_t cmphl; //comparators
	int16_t csw_lowest = -1;
	uint16_t addrVCOpd; // VCO power down address
	uint16_t addrCSW_VCO;
	uint16_t addrCMP; //comparator address
	uint8_t lsb; //SWC lsb index
	uint8_t msb; //SWC msb index

	uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC)); //remember used channel    

	if(module != VCO_CGEN) //set addresses to SX module
	{
        Modify_SPI_Reg_bits(LMS7param(MAC), module);
        addrVCOpd = LMS7param(PD_VCO).address;
        addrCSW_VCO = LMS7param(CSW_VCO).address;
        lsb = LMS7param(CSW_VCO).lsb;
        msb = LMS7param(CSW_VCO).msb;
        addrCMP = LMS7param(VCO_CMPHO).address;
	}
	else //set addresses to CGEN module
    {
        addrVCOpd = LMS7param(PD_VCO_CGEN).address;
        addrCSW_VCO = LMS7param(CSW_VCO_CGEN).address;
        lsb = LMS7param(CSW_VCO_CGEN).lsb;
        msb = LMS7param(CSW_VCO_CGEN).msb;
        addrCMP = LMS7param(VCO_CMPHO_CGEN).address;
    }
	// Initialization
	Modify_SPI_Reg_bits (addrVCOpd, 2, 1, 0); //activate VCO and comparator
    if (Modify_SPI_Reg_bits(addrVCOpd, 2, 1, 0) != 0)
        return LIBLMS7_VCO_IS_POWERED_DOWN;
	if(module == VCO_CGEN)
        Modify_SPI_Reg_bits(LMS7param(SPDUP_VCO_CGEN), 1); //SHORT_NOISEFIL=1 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
	else
        Modify_SPI_Reg_bits(LMS7param(SPDUP_VCO_CGEN), 1); //SHORT_NOISEFIL=1 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
	Modify_SPI_Reg_bits (addrCSW_VCO , msb, lsb , 0); //Set SWC_VCO<7:0>=<00000000>

	i=7;
	while(i>=0)
	{
        Modify_SPI_Reg_bits (addrCSW_VCO, lsb + i, lsb + i, 1); // CSW_VCO<i>=1
        cmphl = (uint8_t)Get_SPI_Reg_bits(addrCMP, 13, 12);
        if ( (cmphl&0x01) == 1) // reduce CSW
            Modify_SPI_Reg_bits (addrCSW_VCO, lsb + i, lsb + i, 0); // CSW_VCO<i>=0
        if( cmphl == 2 && csw_lowest < 0)
            csw_lowest = Get_SPI_Reg_bits(addrCSW_VCO, msb, lsb);
		--i;
	}
	if(csw_lowest >= 0)
    {
        uint16_t csw_highest = Get_SPI_Reg_bits(addrCSW_VCO, msb, lsb);
        if(csw_lowest == csw_highest)
        {
            while(csw_lowest>=0)
            {
                Modify_SPI_Reg_bits(addrCSW_VCO, msb, lsb, csw_lowest);
                if(Get_SPI_Reg_bits(addrCMP, 13, 12) == 0)
                    break;
                else
                    --csw_lowest;
            }
        }
        Modify_SPI_Reg_bits(addrCSW_VCO, msb, lsb, csw_lowest+(csw_highest-csw_lowest)/2);
    }
    if (module == VCO_CGEN)
        Modify_SPI_Reg_bits(LMS7param(SPDUP_VCO_CGEN), 0); //SHORT_NOISEFIL=1 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
    else
        Modify_SPI_Reg_bits(LMS7param(SPDUP_VCO_CGEN), 0); //SHORT_NOISEFIL=1 SPDUP_VCO_ Short the noise filter resistor to speed up the settling time
	cmphl = (uint8_t)Get_SPI_Reg_bits(addrCMP, 13, 12);
    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //restore previously used channel
	if(cmphl == 2)
        return LIBLMS7_SUCCESS;
    else
        return LIBLMS7_FAILURE;
}

/** @brief Returns given parameter value from chip register
    @param param LMS7002M control parameter
    @param fromChip read directly from chip
    @return parameter value
*/
uint16_t LMS7002M::Get_SPI_Reg_bits(const LMS7Parameter &param, bool fromChip)
{
	return Get_SPI_Reg_bits(param.address, param.msb, param.lsb, fromChip);
}

/** @brief Returns given parameter value from chip register
    @param address register address
    @param msb most significant bit index
    @param lsb least significant bit index
    @param fromChip read directly from chip
    @return register bits from selected interval, shifted to right by lsb bits
*/
uint16_t LMS7002M::Get_SPI_Reg_bits(uint16_t address, uint8_t msb, uint8_t lsb, bool fromChip)
{
    return (SPI_read(address, fromChip) & (~(~0<<(msb+1)))) >> lsb; //shift bits to LSB
}

/** @brief Change given parameter value
    @param param LMS7002M control parameter
    @param fromChip read initial value directly from chip
    @param value new parameter value
*/
liblms7_status LMS7002M::Modify_SPI_Reg_bits(const LMS7Parameter &param, const uint16_t value, bool fromChip)
{
    return Modify_SPI_Reg_bits(param.address, param.msb, param.lsb, value, fromChip);
}

/** @brief Change given parameter value
    @param address register address
    @param value new bits value, the value is shifted left by lsb bits
    @param fromChip read initial value directly from chip
*/
liblms7_status LMS7002M::Modify_SPI_Reg_bits(const uint16_t address, const uint8_t msb, const uint8_t lsb, const uint16_t value, bool fromChip)
{   
    uint16_t spiDataReg = SPI_read(address, fromChip); //read current SPI reg data
    uint16_t spiMask = (~(~0 << (msb - lsb + 1))) << (lsb); // creates bit mask
    spiDataReg = (spiDataReg & (~spiMask)) | ((value << lsb) & spiMask);//clear bits
    return SPI_write(address, spiDataReg); //write modified data back to SPI reg
}

/** @brief Modifies given registers with values applied using masks
    @param addr array of register addresses
    @param masks array of applied masks
    @param values array of values to be written
    @param start starting index of given arrays
    @param stop end index of given arrays
*/
liblms7_status LMS7002M::Modify_SPI_Reg_mask(const uint16_t *addr, const uint16_t *masks, const uint16_t *values, uint8_t start, uint8_t stop)
{
    liblms7_status status;
    uint16_t reg_data;
    vector<uint16_t> addresses;
    vector<uint16_t> data;
    while (start <= stop)
    {
        reg_data = SPI_read(addr[start], true, &status); //read current SPI reg data
        reg_data &= ~masks[start];//clear bits
        reg_data |= (values[start] & masks[start]);
        addresses.push_back(addr[start]);
        data.push_back(reg_data);
        ++start;
    }
    if (status != LIBLMS7_SUCCESS)
        return status;
    SPI_write_batch(&addresses[0], &data[0], addresses.size());
    return status;
}

/** @brief Sets SX frequency
    @param Tx Rx/Tx module selection
    @param freq_MHz desired frequency in MHz
	@param refClk_MHz reference clock in MHz
    @return 0-success, other-cannot deliver requested frequency
*/
liblms7_status LMS7002M::SetFrequencySX(bool Tx, float_type freq_MHz, float_type refClk_MHz)
{
	if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;
    const uint8_t sxVCO_N = 2; //number of entries in VCO frequencies
    const float_type m_dThrF = 5500; //threshold to enable additional divider
    const float_type m_pdVCO[3][sxVCO_N] = { { 3800, 5222 }, { 4961, 6754 }, { 6306, 7714 } }; //VCO frequency ranges
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC) ); //remember used channel
    float_type dFvco;
	int8_t i; //iDInd
	int8_t iVCO;
    //VCO selection
    bool canDeliverFrequency = false;
	for(iVCO=2; iVCO>=0; --iVCO)
		for(i=6; i>=0; --i)
		{
			dFvco = (1<<(i+1)) * freq_MHz;
            if ((dFvco >= m_pdVCO[iVCO][0]) && (dFvco <= m_pdVCO[iVCO][sxVCO_N - 1]))
            {
                canDeliverFrequency = true;
                goto vco_found;
            }
		}
    vco_found:

    if (canDeliverFrequency == false)
    {
        Modify_SPI_Reg_bits(LMS7param(MAC), ch); //restore used channel
        return LIBLMS7_CANNOT_DELIVER_FREQUENCY;
    }

    uint16_t gINT = (uint16_t)(dFvco/(refClk_MHz * (1+(dFvco > m_dThrF))) - 4);
    uint32_t gFRAC = (uint32_t)( (dFvco/(refClk_MHz * (1+(dFvco > m_dThrF))) - (uint32_t)(dFvco/(refClk_MHz * (1+(dFvco > m_dThrF))))) * 1048576);

    Modify_SPI_Reg_bits(LMS7param(MAC), Tx + 1);
    Modify_SPI_Reg_bits(LMS7param(SEL_VCO), iVCO); //SEL_VCO
    Modify_SPI_Reg_bits(LMS7param(INT_SDM), gINT); //INT_SDM
    Modify_SPI_Reg_bits(0x011D, 15, 0, gFRAC & 0xFFFF); //FRAC_SDM[15:0]
    Modify_SPI_Reg_bits(0x011E, 3, 0, (gFRAC >> 16)); //FRAC_SDM[19:16]
    Modify_SPI_Reg_bits(LMS7param(DIV_LOCH), i); //DIV_LOCH
    Modify_SPI_Reg_bits(LMS7param(EN_DIV2_DIVPROG), (dFvco > m_dThrF)); //EN_DIV2_DIVPROG
    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //restore used channel
    if (Tx)
        mRefClkSXT_MHz = refClk_MHz;
    else
        mRefClkSXR_MHz = refClk_MHz;
    return TuneVCO( Tx ? VCO_SXT : VCO_SXR); //Rx-1, Tx-2
}

/**	@brief Returns currently set SXR/SXT frequency
	@return SX frequency MHz
*/
float_type LMS7002M::GetFrequencySX_MHz(bool Tx, float_type refClk_MHz)
{
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC)); //remember previously used channel
	float_type dMul;
	if(Tx)
        Modify_SPI_Reg_bits(LMS7param(MAC), 2); // Rx mac = 1, Tx max = 2
	else
        Modify_SPI_Reg_bits(LMS7param(MAC), 1); // Rx mac = 1, Tx max = 2
	uint16_t gINT = Get_SPI_Reg_bits(0x011E, 13, 0);	// read whole register to reduce SPI transfers
    uint32_t gFRAC = ((gINT&0xF) * 65536) | Get_SPI_Reg_bits(0x011D, 15, 0);

    dMul = (float_type)refClk_MHz / (1 << (Get_SPI_Reg_bits(LMS7param(DIV_LOCH)) + 1));
    //Calculate real frequency according to the calculated parameters
    dMul = dMul * ((gINT >> 4) + 4 + (float_type)gFRAC / 1048576.0) * (Get_SPI_Reg_bits(LMS7param(EN_DIV2_DIVPROG)) + 1);
    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //restore used channel
	return dMul;
}

/** @brief Loads given DC_REG values into registers
    @param tx TxTSP or RxTSP selection
    @param I DC_REG I value
    @param Q DC_REG Q value
*/
liblms7_status LMS7002M::LoadDC_REG_IQ(bool tx, int16_t I, int16_t Q)
{
    if(tx)
    {
        Modify_SPI_Reg_bits(LMS7param(DC_REG_TXTSP), I);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_TXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_TXTSP), 1);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_TXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(DC_REG_TXTSP), Q);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_TXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_TXTSP), 1);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_TXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(DC_BYP_TXTSP), 0); //DC_BYP
    }
    else
    {
        Modify_SPI_Reg_bits(LMS7param(DC_REG_RXTSP), I);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_RXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_RXTSP), 1);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDI_RXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(DC_REG_TXTSP), Q);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_RXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_RXTSP), 1);
        Modify_SPI_Reg_bits(LMS7param(TSGDCLDQ_RXTSP), 0);
        Modify_SPI_Reg_bits(LMS7param(DC_BYP_TXTSP), 0); //DC_BYP
    }
    return LIBLMS7_SUCCESS;
}

/** @brief Sets chosen NCO's frequency
    @param tx transmitter or receiver selection
    @param index NCO index from 0 to 15
    @param freq_MHz desired NCO frequency
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::SetNCOFrequency(bool tx, uint8_t index, float_type freq_MHz)
{
    if(index > 15)
        return LIBLMS7_INDEX_OUT_OF_RANGE;
    float_type refClk_MHz = GetReferenceClk_TSP_MHz(tx);
    uint16_t addr = tx ? 0x0240 : 0x0440;
	uint32_t fcw = (uint32_t)((freq_MHz/refClk_MHz)*4294967296);
    SPI_write(addr+2+index*2, (fcw >> 16)); //NCO frequency control word register MSB part.
    SPI_write(addr+3+index*2, fcw); //NCO frequency control word register LSB part.
    return LIBLMS7_SUCCESS;
}

/** @brief Returns chosen NCO's frequency in MHz
    @param tx transmitter or receiver selection
    @param index NCO index from 0 to 15
    @return NCO frequency in MHz
*/
float_type LMS7002M::GetNCOFrequency_MHz(bool tx, uint8_t index)
{
    if(index > 15)
        return LIBLMS7_INDEX_OUT_OF_RANGE;
    float_type refClk_MHz = GetReferenceClk_TSP_MHz(tx);
    uint16_t addr = tx ? 0x0240 : 0x0440;
    uint32_t fcw = 0;
	fcw |= SPI_read(addr+2+index*2) << 16; //NCO frequency control word register MSB part.
    fcw |= SPI_read(addr+3+index*2); //NCO frequency control word register LSB part.
    return refClk_MHz*(fcw/4294967296.0);
}

/** @brief Sets chosen NCO phase offset angle when memory table MODE is 0
@param tx transmitter or receiver selection
@param angle_deg phase offset angle in degrees
@return 0-success, other-failure
*/
liblms7_status LMS7002M::SetNCOPhaseOffsetForMode0(bool tx, float_type angle_deg)
{    
    uint16_t addr = tx ? 0x0241 : 0x0441;
    uint16_t pho = (uint16_t)(65536 * (angle_deg / 360 ));
    SPI_write(addr, pho);
    return LIBLMS7_SUCCESS;
}

/** @brief Sets chosen NCO's phase offset angle
    @param tx transmitter or receiver selection
    @param index PHO index from 0 to 15
    @param angle_deg phase offset angle in degrees
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::SetNCOPhaseOffset(bool tx, uint8_t index, float_type angle_deg)
{
    if(index > 15)
        return LIBLMS7_INDEX_OUT_OF_RANGE;
    uint16_t addr = tx ? 0x0244 : 0x0444;
	uint16_t pho = (uint16_t)(65536*(angle_deg / 360));
    SPI_write(addr+index, pho);
    return LIBLMS7_SUCCESS;
}

/** @brief Returns chosen NCO's phase offset angle in radians
    @param tx transmitter or receiver selection
    @param index PHO index from 0 to 15
    @return phase offset angle in degrees
*/
float_type LMS7002M::GetNCOPhaseOffset_Deg(bool tx, uint8_t index)
{
    uint16_t addr = tx ? 0x0244 : 0x0444;
    uint16_t pho = SPI_read(addr+index);
    float_type angle = 360*pho/65536.0;
    return angle;
}

/** @brief Uploads given FIR coefficients to chip
    @param tx Transmitter or receiver selection
    @param GFIR_index GIR index from 0 to 2
    @param coef array of coefficients
    @param coefCount number of coefficients
    @return 0-success, other-failure

    This function does not change GFIR*_L or GFIR*_N parameters, they have to be set manually
*/
liblms7_status LMS7002M::SetGFIRCoefficients(bool tx, uint8_t GFIR_index, const int16_t *coef, uint8_t coefCount)
{
    uint8_t index;
    uint8_t coefLimit;
    uint16_t startAddr = 0x0280;
    if (tx == false)
        startAddr += 0x0200;
    if (GFIR_index < 2)
        coefLimit = 40;
    else
        coefLimit = 120;
    if (coefCount > coefLimit)
        return LIBLMS7_TOO_MANY_VALUES;
    vector<uint16_t> addresses;
    for (index = 0; index < coefCount; ++index)
    {
        addresses.push_back(startAddr + index + 24 * (index / 40));
        //SPI_write(startAddr + index + 24*(index/40), coef[index]);
    }
    SPI_write_batch(&addresses[0], (uint16_t*)coef, coefCount);
    return LIBLMS7_SUCCESS;
}

/** @brief Returns currently loaded FIR coefficients
    @param tx Transmitter or receiver selection
    @param GFIR_index GIR index from 0 to 2
    @param coef array of returned coefficients
    @param coefCount number of coefficients to read
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::GetGFIRCoefficients(bool tx, uint8_t GFIR_index, int16_t *coef, uint8_t coefCount)
{
    liblms7_status status = LIBLMS7_FAILURE;
    uint8_t index;
    uint8_t coefLimit;
    uint16_t startAddr = 0x0280;
    if (tx == false)
        startAddr += 0x0200;
    if (GFIR_index < 2)
        coefLimit = 40;
    else
        coefLimit = 120;
    if (coefCount > coefLimit)
        return LIBLMS7_TOO_MANY_VALUES;
    for (index = 0; index < coefCount; ++index)
    {
        coef[index] = SPI_read(startAddr + index + 24 * (index / 40), true, &status);
        if (status != LIBLMS7_SUCCESS)
            break;
    }
    return status;
}

/** @brief Write given data value to whole register
    @param address SPI address
    @param data new register value
    @return 0-succes, other-failure
*/
liblms7_status LMS7002M::SPI_write(uint16_t address, uint16_t data)
{
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
#ifndef NDEBUG
    printf("SPI WR: 0x%04X : 0x%04X\n", address, data);
#endif
    
    for (int i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
        if (moduleAddresses[i] == address)
        {                
            if ((mLocalRegistersCopy[0][0] & 0x0003) > 1 && address >= 0x0100)
                mLocalRegistersCopy[1][i] = data;
            else
                mLocalRegistersCopy[0][i] = data;
            break;
        }
    
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;

    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_LMS7002_WR;
    pkt.outBuffer[0] = address >> 8;
    pkt.outBuffer[1] = address & 0xFF;
    pkt.outBuffer[2] = data >> 8;
    pkt.outBuffer[3] = data & 0xFF;
    pkt.outLen = 4;
    controlPort->TransferPacket(pkt);
    if (pkt.status == STATUS_COMPLETED_CMD)
        return LIBLMS7_SUCCESS;
    else
        return LIBLMS7_FAILURE;
}

/** @brief Reads whole register value from given address
    @param address SPI address
    @param status operation status(optional)
    @param fromChip read value directly from chip
    @return register value
*/
uint16_t LMS7002M::SPI_read(uint16_t address, bool fromChip, liblms7_status *status)
{
    if (controlPort == NULL)
    {
        if (status)
            *status = LIBLMS7_NO_CONNECTION_MANAGER;
    }
    if (controlPort->IsOpen() == false || fromChip == false)
    {
        for (int i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
            if (moduleAddresses[i] == address)
            {
                if (status)
                    *status = LIBLMS7_SUCCESS;
                if ((mLocalRegistersCopy[0][0] & 0x0003) > 1 && address >= 0x0100)
                    return mLocalRegistersCopy[1][i];
                else
                    return mLocalRegistersCopy[0][i];
            }
        if (status)
            *status = LIBLMS7_INDEX_OUT_OF_RANGE;
        return 0;
    }
    
    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_LMS7002_RD;
    pkt.outBuffer[0] = address >> 8;
    pkt.outBuffer[1] = address & 0xFF;
    pkt.outLen = 2;
    controlPort->TransferPacket(pkt);
    if (status)
        *status = (pkt.status == STATUS_COMPLETED_CMD ? LIBLMS7_SUCCESS : LIBLMS7_FAILURE);
    return (pkt.inBuffer[2] << 8) | pkt.inBuffer[3];
}

/** @brief Batches multiple register writes into least ammount of transactions
    @param spiAddr spi register addresses to be written
    @param spiData registers data to be written
    @param cnt number of registers to write
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::SPI_write_batch(const uint16_t* spiAddr, const uint16_t* spiData, uint16_t cnt)
{   
    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_LMS7002_WR;
    uint32_t index = 0;
    for (uint32_t i = 0; i < cnt; ++i)
    {
        pkt.outBuffer[index++] = spiAddr[i] >> 8;
        pkt.outBuffer[index++] = spiAddr[i] & 0xFF;
        pkt.outBuffer[index++] = spiData[i] >> 8;
        pkt.outBuffer[index++] = spiData[i] & 0xFF;

        for (int j = 0; j < sizeof(moduleAddresses) / sizeof(uint16_t); ++j)
            if (moduleAddresses[j] == spiAddr[i])
            {
                if ((mLocalRegistersCopy[0][0] & 0x0003) > 1 && spiAddr[i] >= 0x0100)
                    mLocalRegistersCopy[1][j] = spiData[i];
                else
                    mLocalRegistersCopy[0][j] = spiData[i];
                break;
            }

    }
    pkt.outLen = index;

    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;

    controlPort->TransferPacket(pkt);
    if (pkt.status == STATUS_COMPLETED_CMD)
        return LIBLMS7_SUCCESS;
    else
        return LIBLMS7_FAILURE;
}

/** @brief Batches multiple register reads into least amount of transactions
    @param spiAddr SPI addresses to read
    @param spiData array for read data
    @param cnt number of registers to read
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::SPI_read_batch(const uint16_t* spiAddr, uint16_t* spiData, uint16_t cnt)
{
    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_LMS7002_RD;
    uint32_t index = 0;
    for (uint32_t i = 0; i < cnt; ++i)
    {
        pkt.outBuffer[index++] = spiAddr[i] >> 8;
        pkt.outBuffer[index++] = spiAddr[i] & 0xFF;
    }
    pkt.outLen = index;
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;
    controlPort->TransferPacket(pkt);
    for (uint32_t i = 0; i < cnt; ++i)
    {
        spiData[i] = (pkt.inBuffer[2*sizeof(uint16_t)*i + 2] << 8) | pkt.inBuffer[2*sizeof(uint16_t)*i + 3];
        for (int j = 0; j < sizeof(moduleAddresses) / sizeof(uint16_t); ++j)
            if (moduleAddresses[j] == spiAddr[i])
            {   
                if ((mLocalRegistersCopy[0][0] & 0x0003) > 1 && spiAddr[i] >= 0x0100)
                    mLocalRegistersCopy[1][i] = spiData[i];
                else
                    mLocalRegistersCopy[0][i] = spiData[i];
            }
    }
    return pkt.status == STATUS_COMPLETED_CMD ? LIBLMS7_SUCCESS : LIBLMS7_FAILURE;
}

/** @brief Performs registers test by writing known data and confirming readback data
    @return 0-registers test passed, other-failure
*/
liblms7_status LMS7002M::RegistersTest()
{
    char chex[16];
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;

    liblms7_status status;
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));

    //backup both channel data for restoration after test
    vector<uint16_t> ch1Addresses;
    for (uint8_t i = 0; i < MEMORY_SECTIONS_COUNT; ++i)
        for (uint16_t addr = MemorySectionAddresses[i][0]; addr <= MemorySectionAddresses[i][1]; ++addr)
            ch1Addresses.push_back(addr);
    vector<uint16_t> ch1Data;
    ch1Data.resize(ch1Addresses.size(), 0);

    //backup A channel
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    status = SPI_read_batch(&ch1Addresses[0], &ch1Data[0], ch1Addresses.size());
    if (status != LIBLMS7_SUCCESS)
        return status;

    vector<uint16_t> ch2Addresses;
    for (uint8_t i = 0; i < MEMORY_SECTIONS_COUNT; ++i)
        for (uint16_t addr = MemorySectionAddresses[i][0]; addr <= MemorySectionAddresses[i][1]; ++addr)
            if (addr >= 0x0100)
                ch2Addresses.push_back(addr);
    vector<uint16_t> ch2Data;
    ch2Data.resize(ch2Addresses.size(), 0);

    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    status = SPI_read_batch(&ch2Addresses[0], &ch2Data[0], ch2Addresses.size());
    if (status != LIBLMS7_SUCCESS)
        return status;

    //test registers
    ResetChip();
    Modify_SPI_Reg_bits(LMS7param(MIMO_SISO), 0);
    Modify_SPI_Reg_bits(LMS7param(PD_RX_AFE2), 0);
    Modify_SPI_Reg_bits(LMS7param(PD_TX_AFE2), 0);
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);

    stringstream ss;

    //check single channel memory sections
    vector<MemorySection> modulesToCheck = { AFE, BIAS, XBUF, CGEN, LDO, BIST, CDS, TRF, TBB, RFE, RBB, SX,
        TxTSP, TxNCO, TxGFIR1, TxGFIR2, TxGFIR3a, TxGFIR3b, TxGFIR3c,
        RxTSP, RxNCO, RxGFIR1, RxGFIR2, RxGFIR3a, RxGFIR3b, RxGFIR3c, LimeLight };
    const char* moduleNames[] = { "AFE", "BIAS", "XBUF", "CGEN", "LDO", "BIST", "CDS", "TRF", "TBB", "RFE", "RBB", "SX",
        "TxTSP", "TxNCO", "TxGFIR1", "TxGFIR2", "TxGFIR3a", "TxGFIR3b", "TxGFIR3c",
        "RxTSP", "RxNCO", "RxGFIR1", "RxGFIR2", "RxGFIR3a", "RxGFIR3b", "RxGFIR3c", "LimeLight" };

    const uint16_t patterns[] = { 0xAAAA, 0x5555 };
    const uint8_t patternsCount = 2;

    bool allTestSuccess = true;

    for (unsigned i = 0; i < modulesToCheck.size(); ++i)
    {
        bool moduleTestsSuccess = true;
        uint16_t startAddr = MemorySectionAddresses[modulesToCheck[i]][0];
        uint16_t endAddr = MemorySectionAddresses[modulesToCheck[i]][1];
        uint8_t channelCount = startAddr >= 0x0100 ? 2 : 1;
        for (int cc = 1; cc <= channelCount; ++cc)
        {
            Modify_SPI_Reg_bits(LMS7param(MAC), cc);
            sprintf(chex, "0x%04X", startAddr);
            ss << moduleNames[i] << "  [" << chex << ":";
            sprintf(chex, "0x%04X", endAddr);
            ss << chex << "]";
            if (startAddr >= 0x0100)
                ss << " Ch." << (cc == 1 ? "A" : "B");
                ss << endl;
            for (uint8_t p = 0; p < patternsCount; ++p)
                moduleTestsSuccess &= RegistersTestInterval(startAddr, endAddr, patterns[p], ss) == LIBLMS7_SUCCESS;
        }
        allTestSuccess &= moduleTestsSuccess;
    }

    //restore register values
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    SPI_write_batch(&ch1Addresses[0], &ch1Data[0], ch1Addresses.size());
    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    SPI_write_batch(&ch2Addresses[0], &ch2Data[0], ch2Addresses.size());
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);

    fstream fout;
    fout.open("registersTest.txt", ios::out);
    fout << ss.str() << endl;
    fout.close();

    return allTestSuccess ? LIBLMS7_SUCCESS : LIBLMS7_FAILURE;
}

/** @brief Performs registers test for given address interval by writing given pattern data
    @param startAddr first register address
    @param endAddr last reigster address
    @param pattern data to be written into registers
    @return 0-register test passed, other-failure
*/
liblms7_status LMS7002M::RegistersTestInterval(uint16_t startAddr, uint16_t endAddr, uint16_t pattern, stringstream &ss)
{
    vector<uint16_t> addrToWrite;
    vector<uint16_t> dataToWrite;
    vector<uint16_t> dataReceived;
    vector<uint16_t> dataMasks;

    for (uint16_t addr = startAddr; addr <= endAddr; ++addr)
    {
        addrToWrite.push_back(addr);
    }
    dataMasks.resize(addrToWrite.size(), 0xFFFF);
    for (uint16_t j = 0; j < sizeof(readOnlyRegisters)/sizeof(uint16_t); ++j)
        for (uint16_t k = 0; k < addrToWrite.size(); ++k)
            if (readOnlyRegisters[j] == addrToWrite[k])
            {
                dataMasks[k] = readOnlyRegistersMasks[j];
                break;
            }

    dataToWrite.clear();
    dataReceived.clear();
    for (uint16_t j = 0; j < addrToWrite.size(); ++j)
    {
        if (addrToWrite[j] == 0x00A6)
            dataToWrite.push_back(0x1 | pattern & ~0x2);
        else if (addrToWrite[j] == 0x0084)
            dataToWrite.push_back(pattern & ~0x19);
        else
            dataToWrite.push_back(pattern & dataMasks[j]);
    }
    dataReceived.resize(addrToWrite.size(), 0);
    liblms7_status status;
    status = SPI_write_batch(&addrToWrite[0], &dataToWrite[0], addrToWrite.size());
    if (status != LIBLMS7_SUCCESS)
        return status;
    status = SPI_read_batch(&addrToWrite[0], &dataReceived[0], addrToWrite.size());
    if (status != LIBLMS7_SUCCESS)
        return status;
    bool registersMatch = true;
    char ctemp[16];
    for (uint16_t j = 0; j < dataToWrite.size(); ++j)
    {
        if (dataToWrite[j] != (dataReceived[j] & dataMasks[j]))
        {
            registersMatch = false;
            sprintf(ctemp, "0x%04X", addrToWrite[j]);
            ss << "\t" << ctemp << "(wr/rd): ";
            sprintf(ctemp, "0x%04X", dataToWrite[j]);
            ss << ctemp << "/";
            sprintf(ctemp, "0x%04X", dataReceived[j]);
            ss << ctemp << endl;
        }
    }
    if (registersMatch)
    {
        sprintf(ctemp, "0x%04X", pattern);
        ss << "\tRegisters OK (" << ctemp << ")\n";
    }
    return registersMatch ? LIBLMS7_SUCCESS : LIBLMS7_FAILURE;
}

/** @brief Parameters setup instructions for Tx calibration
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::CalibrateTxSetup(float_type bandwidth_MHz)
{
    //Stage 2
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));
    uint8_t sel_band1_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND1_TRF));
    uint8_t sel_band2_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND2_TRF));

    //rfe
    //reset RFE to defaults
    SetDefaults(RFE);
    if (sel_band1_trf == 1)
        Modify_SPI_Reg_bits(LMS7param(SEL_PATH_RFE), 3); //SEL_PATH_RFE 3
    else if (sel_band2_trf == 1)
        Modify_SPI_Reg_bits(LMS7param(SEL_PATH_RFE), 2);
    else
        return LIBLMS7_BAND_NOT_SELECTED;

    if (ch == 2)
        Modify_SPI_Reg_bits(LMS7param(EN_NEXTRX_RFE), 1); // EN_NEXTTX_RFE 1

    Modify_SPI_Reg_bits(LMS7param(G_RXLOOPB_RFE), 8); //G_RXLOOPB_RFE 8
    Modify_SPI_Reg_bits(0x010C, 4, 3, 0); //PD_MXLOBUF_RFE 0, PD_QGEN_RFE 0
    Modify_SPI_Reg_bits(LMS7param(CCOMP_TIA_RFE), 10); //CCOMP_TIA_RFE 10
    Modify_SPI_Reg_bits(LMS7param(CFB_TIA_RFE), 2600); //CFB_TIA_RFE 2600
    Modify_SPI_Reg_bits(LMS7param(ICT_LODC_RFE), 31); //ICT_LODC_RFE 31
    Modify_SPI_Reg_bits(LMS7param(PD_LNA_RFE), 1);

    //RBB
    //reset RBB to defaults
    SetDefaults(RBB);
    Modify_SPI_Reg_bits(LMS7param(PD_LPFL_RBB), 0); //PD_LPFL_RBB 0
    Modify_SPI_Reg_bits(LMS7param(RCC_CTL_LPFL_RBB), 0); //RCC_CTL_LPFL_RBB 0
    Modify_SPI_Reg_bits(LMS7param(C_CTL_LPFL_RBB), 1500); //C_CTL_LPFL_RBB 1500
    Modify_SPI_Reg_bits(LMS7param(G_PGA_RBB), 22); //G_PGA_RBB 22

    //TRF
    //reset TRF to defaults
    //SetDefaults(TRF);
    Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), 0); //L_LOOPB_TXPAD_TRF 0
    Modify_SPI_Reg_bits(LMS7param(EN_LOOPB_TXPAD_TRF), 1); //EN_LOOPB_TXPAD_TRF 1
    Modify_SPI_Reg_bits(LMS7param(EN_G_TRF), 0); //EN_G_TRF 0
    if (ch == 2)
        Modify_SPI_Reg_bits(LMS7param(EN_NEXTTX_TRF), 1); //EN_NEXTTX_TRF 1
    Modify_SPI_Reg_bits(LMS7param(LOSS_LIN_TXPAD_TRF), 0); //LOSS_LIN_TXPAD_TRF 5
    Modify_SPI_Reg_bits(LMS7param(LOSS_MAIN_TXPAD_TRF), 0); //LOSS_MAIN_TXPAD_TRF 5

    //TBB
    //reset TBB to defaults
    /*SetDefaults(TBB);
    Modify_SPI_Reg_bits(LMS7param(CG_IAMP_TBB), 9); //CG_IAMP_TBB 9
    Modify_SPI_Reg_bits(LMS7param(ICT_IAMP_FRP_TBB), 1); //ICT_IAMP_FRP_TBB 1
    Modify_SPI_Reg_bits(LMS7param(ICT_IAMP_GG_FRP_TBB), 6); //ICT_IAMP_GG_FRP_TBB 6
    Modify_SPI_Reg_bits(LMS7param(RCAL_LPFH_TBB), 125); //RCAL_LPFH_TBB 0
    */
    //AFE
    //reset AFE to defaults
    uint8_t isel_dac_afe =(uint8_t) Get_SPI_Reg_bits(LMS7param(ISEL_DAC_AFE));
    SetDefaults(AFE);
    Modify_SPI_Reg_bits(LMS7param(PD_RX_AFE2), 0); //PD_RX_AFE2 0
    Modify_SPI_Reg_bits(LMS7param(ISEL_DAC_AFE), isel_dac_afe);

    if (ch == 2)
        Modify_SPI_Reg_bits(LMS7param(PD_TX_AFE2), 0);
    //BIAS
    uint16_t backup = Get_SPI_Reg_bits(LMS7param(RP_CALIB_BIAS));
    SetDefaults(BIAS);
    Modify_SPI_Reg_bits(LMS7param(RP_CALIB_BIAS), backup); //RP_CALIB_BIAS

    //XBUF
    Modify_SPI_Reg_bits(0x0085, 2, 0, 1); //PD_XBUF_RX 0, PD_XBUF_TX 0, EN_G_XBUF 1

    //CGEN
    //reset CGEN to defaults
    SetDefaults(CGEN);
    //power up VCO
    Modify_SPI_Reg_bits(LMS7param(PD_VCO_CGEN), 0);

    if (SetFrequencyCGEN(122.88) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;
    if (TuneVCO(VCO_CGEN) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;

    //SXR
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    SetDefaults(SX);
    float_type SXTfreqMHz = GetFrequencySX_MHz(Tx, mRefClkSXT_MHz);

    float_type SXRfreqMHz = SXTfreqMHz - bandwidth_MHz / 4 - 1;
    if (SetFrequencySX(Rx, SXRfreqMHz, mRefClkSXR_MHz) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;
    if (TuneVCO(VCO_SXR) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;

    //SXT
    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    Modify_SPI_Reg_bits(LMS7param(PD_LOCH_T2RBUF), 1); //PD_LOCH_T2RBUF 1
    if (SetFrequencySX(Tx, SXTfreqMHz, mRefClkSXT_MHz) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;
    if (TuneVCO(VCO_SXT) != LIBLMS7_SUCCESS)
        return LIBLMS7_FAILURE;
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);

    //TXTSP
    SetDefaults(TxTSP);
    Modify_SPI_Reg_bits(0x0200, 3, 2, 0x3); //TSGMODE 1, INSEL 1
    Modify_SPI_Reg_bits(0x0208, 6, 4, 0x7); //GFIR3_BYP 1, GFIR2_BYP 1, GFIR1_BYP 1
    LoadDC_REG_IQ(Tx, (int16_t)0x7FFF, (int16_t)0x8000);
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
    Modify_SPI_Reg_bits(0x0440, 4, 0, 0); //TX SEL[3:0] = 0 & MODE = 0

    float_type offset = 0.2;
    if (bandwidth_MHz == 8)
    {
        //SXR
        Modify_SPI_Reg_bits(LMS7param(MAC), 1);
        SetDefaults(SX);
        float_type SXTfreqMHz = GetFrequencySX_MHz(Tx, mRefClkSXT_MHz);

        float_type sxrFreq = SXTfreqMHz - bandwidth_MHz / 4 - 1 - offset;
        if (SetFrequencySX(Rx, sxrFreq, mRefClkSXR_MHz) != LIBLMS7_SUCCESS)
            return LIBLMS7_FAILURE;
        SetNCOFrequency(Tx, 0, bandwidth_MHz / 4 + offset);
    }
    else
        SetNCOFrequency(Tx, 0, bandwidth_MHz / 4);

    //RXTSP
    SetDefaults(RxTSP);
    Modify_SPI_Reg_bits(LMS7param(AGC_MODE_RXTSP), 1); //AGC_MODE 1
    Modify_SPI_Reg_bits(0x040C, 7, 0, 0xBF);
    Modify_SPI_Reg_bits(LMS7param(CAPSEL), 0);
    Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), 0); //Decimation HBD ratio
    Modify_SPI_Reg_bits(LMS7param(AGC_AVG_RXTSP), 0x7); //agc_avg iq corr

    return LIBLMS7_SUCCESS;
}

/** @brief Flips the CAPTURE bit and returns digital RSSI value
*/
uint32_t LMS7002M::GetRSSI()
{
    Modify_SPI_Reg_bits(LMS7param(CAPTURE), 0);
    Modify_SPI_Reg_bits(LMS7param(CAPTURE), 1);
    return (Get_SPI_Reg_bits(0x040F, 15, 0) << 2) | Get_SPI_Reg_bits(0x040E, 1, 0);
}

/** @brief Sets Rx Dc offsets by converting two's complementary numbers to sign and magnitude
*/
void LMS7002M::SetRxDCOFF(int8_t offsetI, int8_t offsetQ)
{
    uint16_t valToSend = 0;
    if (offsetI < 0)
        valToSend |= 0x40;
    valToSend |= labs(offsetI);
    valToSend = valToSend << 7;
    if (offsetQ < 0)
        valToSend |= 0x40;
    valToSend |= labs(offsetQ);
    SPI_write(0x010E, valToSend);
}

/** @brief Calibrates Transmitter. DC correction, IQ gains, IQ phase correction
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::CalibrateTx(float_type bandwidth_MHz)
{
    liblms7_status status;
    Log("Tx calibration started", LOG_INFO);
    BackupAllRegisters();

    int16_t iqcorr = 0;
    uint16_t gcorrq = 0;
    uint16_t gcorri = 0;
    uint16_t dccorri;
    uint16_t dccorrq;
    int16_t corrI = 0;
    int16_t corrQ = 0;
    uint32_t minRSSI_i;
    uint32_t minRSSI_q;
    uint32_t minRSSI_iq;
    int16_t i;
    int16_t offsetI = 0;
    int16_t offsetQ = 0;

    const short firCoefs[] =
    {
        -2531,
        -517,
        2708,
        188,
        -3059,
        216,
        3569,
        -770,
        -4199,
        1541,
        4886,
        -2577,
        -5552,
        3909,
        6108,
        -5537,
        -6457,
        7440,
        6507,
        -9566,
        -6174,
        11845,
        5391,
        -14179,
        -4110,
        16457,
        2310,
        -18561,
        0,
        20369,
        -2780,
        -21752,
        5963,
        22610,
        -9456,
        -22859,
        13127,
        22444,
        -16854,
        -21319,
        20489,
        19492,
        -23883,
        -17002,
        26881,
        13902,
        -29372,
        -10313,
        31226,
        6345,
        -32380,
        -2141,
        32767,
        -2141,
        -32380,
        6345,
        31226,
        -10313,
        -29372,
        13902,
        26881,
        -17002,
        -23883,
        19492,
        20489,
        -21319,
        -16854,
        22444,
        13127,
        -22859,
        -9456,
        22610,
        5963,
        -21752,
        -2780,
        20369,
        0,
        -18561,
        2310,
        16457,
        -4110,
        -14179,
        5391,
        11845,
        -6174,
        -9566,
        6507,
        7440,
        -6457,
        -5537,
        6108,
        3909,
        -5552,
        -2577,
        4886,
        1541,
        -4199,
        -770,
        3569,
        216,
        -3059,
        188,
        2708,
        -517,
        -2531
    };

    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));
    //Stage 1
    uint8_t sel_band1_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND1_TRF));
    uint8_t sel_band2_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND2_TRF));
    Log("Setup stage", LOG_INFO);
    status = CalibrateTxSetup(bandwidth_MHz);
    if (status != LIBLMS7_SUCCESS)
        goto TxCalibrationEnd; //go to ending stage to restore registers

    //Stage 3
    //Calibrate Rx DC
    Log("Rx DC calibration", LOG_INFO);
    {
        uint16_t requiredRegs[] = { 0x0400, 0x040A, 0x010D, 0x040C };
        uint16_t requiredMask[] = { 0x6000, 0x3007, 0x0040, 0x00FF }; //CAPSEL, AGC_MODE, AGC_AVG, EN_DCOFF, Bypasses
        uint16_t requiredValue[] = { 0x0000, 0x1007, 0x0040, 0x00BD };

        Modify_SPI_Reg_mask(requiredRegs, requiredMask, requiredValue, 0, 3);
    }
    for (i = 0; i<6; ++i)
    {
        FindMinRSSI(LMS7param(DCOFFI_RFE), offsetI, &offsetI, 3, 2, 32 >> i);
        FindMinRSSI(LMS7param(DCOFFQ_RFE), offsetQ, &offsetQ, 3, 2, 32 >> i);
    }
    SetRxDCOFF((int8_t)offsetI, (int8_t)offsetQ);
    Modify_SPI_Reg_bits(LMS7param(DC_BYP_RXTSP), 0); // DC_BYP 0

    sel_band1_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND1_TRF));
    sel_band2_trf = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_BAND2_TRF));
    //B
    Modify_SPI_Reg_bits(0x0100, 0, 0, 1); //EN_G_TRF 1
    if (sel_band1_trf == 1)
    {
        Modify_SPI_Reg_bits(LMS7param(PD_RLOOPB_1_RFE), 0); //PD_RLOOPB_1_RFE 0
        Modify_SPI_Reg_bits(LMS7param(EN_INSHSW_LB1_RFE), 0); //EN_INSHSW_LB1 0
    }
    if (sel_band2_trf == 1)
    {
        Modify_SPI_Reg_bits(LMS7param(PD_RLOOPB_2_RFE), 0); //PD_RLOOPB_2_RFE 0
        Modify_SPI_Reg_bits(LMS7param(EN_INSHSW_LB2_RFE), 0); // EN_INSHSW_LB2 0
    }
    FixRXSaturation();

    Modify_SPI_Reg_bits(LMS7param(GFIR3_BYP_RXTSP), 0); //GFIR3_BYP 0
    Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), 2);
    Modify_SPI_Reg_bits(LMS7param(GFIR3_L_RXTSP), 7);
    Modify_SPI_Reg_bits(LMS7param(GFIR3_N_RXTSP), 7);

    SetGFIRCoefficients(Rx, 2, firCoefs, sizeof(firCoefs) / sizeof(int16_t));

    Log("IQ correction stage", LOG_INFO);
    Modify_SPI_Reg_bits(LMS7param(GCORRI_TXTSP), 2047);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_TXTSP), 2047);

    Modify_SPI_Reg_bits(LMS7param(IQCORR_TXTSP), 0);

    Log("I gain", LOG_INFO);
    minRSSI_i = FindMinRSSI_Gain(LMS7param(GCORRI_TXTSP), &gcorri);

    Modify_SPI_Reg_bits(LMS7param(GCORRI_TXTSP), 2047);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_TXTSP), 2047);

    Log("Q gain", LOG_INFO);
    minRSSI_q = FindMinRSSI_Gain(LMS7param(GCORRQ_TXTSP), &gcorrq);

    if (minRSSI_i < minRSSI_q)
        gcorrq = 2047;
    else
        gcorri = 2047;

    Modify_SPI_Reg_bits(LMS7param(GCORRI_TXTSP), gcorri);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_TXTSP), gcorrq);

    Log("Phase", LOG_INFO);
    iqcorr = 0;
    for (uint8_t i = 0; i<9; ++i)
        minRSSI_iq = FindMinRSSI(LMS7param(IQCORR_TXTSP), iqcorr, &iqcorr, 3, 1, 256 >> i);

    Modify_SPI_Reg_bits(LMS7param(GCORRI_TXTSP), gcorri);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_TXTSP), gcorrq);
    Modify_SPI_Reg_bits(LMS7param(IQCORR_TXTSP), iqcorr);

    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    status = SetFrequencySX(Rx, GetFrequencySX_MHz(Tx, mRefClkSXT_MHz)-1, mRefClkSXR_MHz);
    if (status != LIBLMS7_SUCCESS)
        goto TxCalibrationEnd; //go to ending stage to restore registers

    //C
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
    Modify_SPI_Reg_bits(LMS7param(AGC_MODE_RXTSP), 1);
    Modify_SPI_Reg_bits(LMS7param(CAPSEL), 0);

    Log("TX LO calibration", LOG_INFO);

    //Calibrate Tx DC
    for (uint8_t i = 0; i<7; ++i)
    {
        FindMinRSSI(LMS7param(DCCORRI_TXTSP), corrI, &corrI, 3, 1, 64 >> i);
        FindMinRSSI(LMS7param(DCCORRQ_TXTSP), corrQ, &corrQ, 3, 1, 64 >> i);
    }

    dccorri = Get_SPI_Reg_bits(LMS7param(DCCORRI_TXTSP));
    dccorrq = Get_SPI_Reg_bits(LMS7param(DCCORRQ_TXTSP));

    // Stage 4
TxCalibrationEnd:
    Log("Restoring registers state", LOG_INFO);
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
    RestoreAllRegisters();
    if (status != LIBLMS7_SUCCESS)
    {
        Log("Tx calibration failed", LOG_WARNING);
        return status;
    }

    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
    Modify_SPI_Reg_bits(LMS7param(DCCORRI_TXTSP), dccorri);
    Modify_SPI_Reg_bits(LMS7param(DCCORRQ_TXTSP), dccorrq);
    Modify_SPI_Reg_bits(LMS7param(GCORRI_TXTSP), gcorri);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_TXTSP), gcorrq);
    Modify_SPI_Reg_bits(LMS7param(IQCORR_TXTSP), iqcorr);

    Modify_SPI_Reg_bits(LMS7param(DC_BYP_TXTSP), 0); //DC_BYP
    Modify_SPI_Reg_bits(0x0208, 1, 0, 0); //GC_BYP PH_BYP
    Log("Tx calibration finished", LOG_INFO);
    return LIBLMS7_SUCCESS;
}

/** @brief Performs Rx DC offsets calibration
*/
void LMS7002M::CalibrateRxDC_RSSI()
{
    int16_t i;
    int16_t offsetI = 0;
    int16_t offsetQ = 0;
    uint16_t requiredRegs[] = { 0x0400, 0x040A, 0x010D, 0x040C };
    uint16_t requiredMask[] = { 0x6000, 0x3007, 0x0040, 0x00FF }; //CAPSEL, AGC_MODE, AGC_AVG, EN_DCOFF, Bypasses
    uint16_t requiredValue[] = { 0x0000, 0x1007, 0x0040, 0x00BD };

    Modify_SPI_Reg_mask(requiredRegs, requiredMask, requiredValue, 0, 3);
    for (i = 0; i<6; ++i)
    {
        FindMinRSSI(LMS7param(DCOFFI_RFE), offsetI, &offsetI, 3, 2, 32 >> i);
        FindMinRSSI(LMS7param(DCOFFQ_RFE), offsetQ, &offsetQ, 3, 2, 32 >> i);
    }
    Modify_SPI_Reg_bits(LMS7param(EN_DCOFF_RXFE_RFE), 1);
    SetRxDCOFF((int8_t)offsetI, (int8_t)offsetQ);
    Modify_SPI_Reg_bits(LMS7param(DC_BYP_RXTSP), 0); // DC_BYP 0
}

/** @brief Tries to detect and fix gains if Rx is saturated
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::FixRXSaturation()
{
    uint8_t g_rxloopb = 0;
    Modify_SPI_Reg_bits(LMS7param(G_RXLOOPB_RFE), g_rxloopb);
    Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), 3);

    int8_t lLoopb = 3;
    const uint32_t rssi_saturation_level = 0xD000;
    while (g_rxloopb < 15)
    {
        g_rxloopb += 1;
        Modify_SPI_Reg_bits(LMS7param(G_RXLOOPB_RFE), g_rxloopb);
        Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), 3);
        if (GetRSSI() < rssi_saturation_level)
        {
            for (lLoopb = 3; lLoopb >= 0; --lLoopb)
            {
                Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), lLoopb);
                if (GetRSSI() > rssi_saturation_level)
                {
                    ++lLoopb;
                    Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), lLoopb);
                    goto finished;
                }
            }
        }
        else
        {
            g_rxloopb -= 1;
            Modify_SPI_Reg_bits(LMS7param(G_RXLOOPB_RFE), g_rxloopb);
            break;
        }
    }
finished:
    return GetRSSI() < rssi_saturation_level ? LIBLMS7_SUCCESS : LIBLMS7_FAILURE;
}

uint32_t LMS7002M::FindMinRSSI(const LMS7Parameter &param, const int16_t startValue, int16_t *result, const uint8_t scanWidth, const uint8_t twoCompl, int8_t stepMult)
{
    return FindMinRSSI(param.address, param.msb, param.lsb, startValue, result, scanWidth, twoCompl, stepMult);
}

/** @brief Searches for minimal RSSI value while changing given address bits
@param addr address of parameter being changed
@param msb most significant bit index
@param lsb least significant bit index
@param startValue initial value where to start search
@param result found minimal parameter value will be set here
@param twoCompl varying parameter value is treated as two's complement
@return found minimal RSSI value
*/
uint32_t LMS7002M::FindMinRSSI(const uint16_t addr, const uint8_t msb, const uint8_t lsb, const int16_t startValue, int16_t *result, const uint8_t scanWidth, const uint8_t twoCompl, int8_t stepMult)
{
    if (scanWidth < 1)
        return ~0;
    int minI;
    int min = startValue;
    int globMin = 0;
    uint32_t minRSSI = ~0;
    unsigned int *rssiField = new unsigned int[scanWidth];
    int minRSSIindex;
    int i;
    int maxVal;
    int minVal = 0;
    if (twoCompl)
    {
        maxVal = (~(~0x0 << (msb - lsb + 1))) / 2;
        minVal = -(~(~0x0 << (msb - lsb + 1))) / 2 - 1;
    }
    else
        maxVal = (~(~0x0 << (msb - lsb + 1)));

    Modify_SPI_Reg_bits(addr, msb, lsb, startValue);

    Modify_SPI_Reg_bits(LMS7param(AGC_MODE_RXTSP), 1);
    Modify_SPI_Reg_bits(LMS7param(CAPSEL), 0);

    minRSSIindex = 0;
    for (i = 0; i<scanWidth; ++i)
    {
        short currentValue = min + (i - scanWidth / 2)*stepMult;
        if (currentValue < minVal)
            currentValue = minVal;
        else if (currentValue > maxVal)
            currentValue = maxVal;
        if (twoCompl == 2)
        {
            uint16_t valToSend = 0;
            if (currentValue < 0)
                valToSend |= 0x40;
            valToSend |= labs(currentValue);
            Modify_SPI_Reg_bits(addr, msb, lsb, valToSend);
        }
        else
            Modify_SPI_Reg_bits(addr, msb, lsb, currentValue);

        rssiField[i] = GetRSSI();
    }
    minI = min;
    minRSSIindex = 0;
    for (i = 0; i<scanWidth; ++i)
        if (rssiField[i] < minRSSI)
        {
            minRSSI = rssiField[i];
            minRSSIindex = i;
            minI = min + (i - scanWidth / 2)*stepMult;
            if (minI > maxVal)
                minI = maxVal;
            else if (minI < minVal)
                minI = minVal;
            globMin = minI;
        }
    min = minI;

    Modify_SPI_Reg_bits(addr, msb, lsb, min);

    *result = min;
    return minRSSI;
}

/** @brief Sets given module registers to default values
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::SetDefaults(MemorySection module)
{
    liblms7_status status = LIBLMS7_SUCCESS;
    uint16_t i;
    vector<uint16_t> addrs;
    vector<uint16_t> values;
    for (i = 0; i<sizeof(moduleAddresses) / 2; ++i)
    {
        if (moduleAddresses[i] > MemorySectionAddresses[module][1])
            break;
        if (moduleAddresses[i] >= MemorySectionAddresses[module][0] && moduleAddresses[i] <= MemorySectionAddresses[module][1])
        {
            addrs.push_back(moduleAddresses[i]);
            values.push_back(defaultValues[i]);            
        }           
    }
    status = SPI_write_batch(&addrs[0], &values[0], addrs.size());
    return status;
}

/** @brief Parameters setup instructions for Rx calibration
    @param bandwidth_MHz filter bandwidth in MHz
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::CalibrateRxSetup(float_type bandwidth_MHz)
{
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));

    //rfe
    if (ch == 2)
        Modify_SPI_Reg_bits(LMS7param(EN_NEXTTX_TRF), 1); // EN_NEXTTX_TRF 0

    Modify_SPI_Reg_bits(LMS7param(G_RXLOOPB_RFE), 15); //G_RXLOOPB_RFE 15
    Modify_SPI_Reg_bits(0x010C, 4, 3, 0); //PD_MXLOBUF_RFE 0, PD_QGEN_RFE 0
    Modify_SPI_Reg_bits(0x010C, 1, 1, 0); //PD_TIA 0
    Modify_SPI_Reg_bits(0x010C, 7, 7, 1); //PD_LNA 1

    Modify_SPI_Reg_bits(0x0110, 4, 0, 31); //ICT_LO_RFE 31
    Modify_SPI_Reg_bits(0x010D, 4, 1, 0xFF); // all short switches are enabled

    //RBB
    Modify_SPI_Reg_bits(0x0115, 15, 14, 0); //Loopback switches disable
    Modify_SPI_Reg_bits(0x0119, 15, 15, 0); //OSW_PGA 0

    //TRF
    //reset TRF to defaults
    SetDefaults(TRF);
    Modify_SPI_Reg_bits(LMS7param(L_LOOPB_TXPAD_TRF), 0); //L_LOOPB_TXPAD_TRF 0
    Modify_SPI_Reg_bits(LMS7param(EN_LOOPB_TXPAD_TRF), 1); //EN_LOOPB_TXPAD_TRF 1
    Modify_SPI_Reg_bits(LMS7param(EN_G_TRF), 0); //EN_G_TRF 0
    if (ch == 2)
        Modify_SPI_Reg_bits(LMS7param(EN_NEXTTX_TRF), 1); //EN_NEXTTX_TRF 1
    Modify_SPI_Reg_bits(LMS7param(LOSS_LIN_TXPAD_TRF), 0); //LOSS_LIN_TXPAD_TRF 5
    Modify_SPI_Reg_bits(LMS7param(LOSS_MAIN_TXPAD_TRF), 0); //LOSS_MAIN_TXPAD_TRF 5

    //TBB
    //reset TBB to defaults
    SetDefaults(TBB);
    Modify_SPI_Reg_bits(LMS7param(CG_IAMP_TBB), 9); //CG_IAMP_TBB 9
    Modify_SPI_Reg_bits(LMS7param(ICT_IAMP_FRP_TBB), 1); //ICT_IAMP_FRP_TBB 1
    Modify_SPI_Reg_bits(LMS7param(ICT_IAMP_GG_FRP_TBB), 6); //ICT_IAMP_GG_FRP_TBB 6

    //AFE
    //reset AFE to defaults
    SetDefaults(AFE);
    Modify_SPI_Reg_bits(LMS7param(PD_RX_AFE2), 0); //PD_RX_AFE2
    if (ch == 2)
    {
        Modify_SPI_Reg_bits(LMS7param(PD_TX_AFE2), 0); //PD_TX_AFE2
    }
    //BIAS
    uint16_t backup = Get_SPI_Reg_bits(0x0084, 10, 6);
    SetDefaults(BIAS);
    Modify_SPI_Reg_bits(0x0084, 10, 6, backup); //RP_CALIB_BIAS

    //XBUF
    Modify_SPI_Reg_bits(0x0085, 2, 0, 1); //PD_XBUF_RX 0, PD_XBUF_TX 0, EN_G_XBUF 1

    //CGEN
    //reset CGEN to defaults
    SetDefaults(CGEN);
    //power up VCO
    Modify_SPI_Reg_bits(0x0086, 2, 2, 0);

    liblms7_status status = SetFrequencyCGEN(122.88);
    if (status != LIBLMS7_SUCCESS)
        return status;

    //    //SXR
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    float_type SXRfreqMHz = GetFrequencySX_MHz(Rx, mRefClkSXR_MHz);

    //SXT
    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    Modify_SPI_Reg_bits(LMS7param(PD_LOCH_T2RBUF), 1); //PD_LOCH_t2RBUF 1
    status = SetFrequencySX(Tx, SXRfreqMHz + bandwidth_MHz / 4, mRefClkSXT_MHz);
    if ( status != LIBLMS7_SUCCESS)
        return status;
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);

    //TXTSP
    SetDefaults(TxTSP);
    Modify_SPI_Reg_bits(0x0200, 3, 2, 0x3); //TSGMODE 1, INSEL 1
    //Modify_SPI_Reg_bits(0x0208, 6, 4, 0xFFFF); //GFIR3_BYP 1, GFIR2_BYP 1, GFIR1_BYP 1
    Modify_SPI_Reg_bits(0x0208, 6, 6, 1); //GFIR3_BYP 1, GFIR2_BYP 1, GFIR1_BYP 1
    Modify_SPI_Reg_bits(0x0208, 5, 5, 1); //GFIR3_BYP 1, GFIR2_BYP 1, GFIR1_BYP 1
    Modify_SPI_Reg_bits(0x0208, 4, 4, 1); //GFIR3_BYP 1, GFIR2_BYP 1, GFIR1_BYP 1
    LoadDC_REG_IQ(Tx, (int16_t)0x7FFF, (int16_t)0x8000);
    SetNCOFrequency(Tx, 0, 0);

    //RXTSP
    SetDefaults(RxTSP);
    Modify_SPI_Reg_bits(LMS7param(AGC_MODE_RXTSP), 1); //AGC_MODE 1
    Modify_SPI_Reg_bits(0x040C, 7, 7, 0x1); //CMIX_BYP 1
    Modify_SPI_Reg_bits(0x040C, 6, 6, 0x0); //AGC_BYP 0
    Modify_SPI_Reg_bits(0x040C, 5, 5, 1); //
    Modify_SPI_Reg_bits(0x040C, 4, 4, 1); //
    Modify_SPI_Reg_bits(0x040C, 3, 3, 1); //
    Modify_SPI_Reg_bits(0x040C, 2, 2, 1); // DC_BYP
    Modify_SPI_Reg_bits(0x040C, 1, 1, 1); //
    Modify_SPI_Reg_bits(0x040C, 0, 0, 1); //

    Modify_SPI_Reg_bits(LMS7param(CAPSEL), 0);
    Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), 2);
    Modify_SPI_Reg_bits(LMS7param(AGC_AVG_RXTSP), 0x7); //agc_avg iq corr
    return LIBLMS7_SUCCESS;
}

/** @brief Calibrates Receiver. DC offset, IQ gains, IQ phase correction
    @return 0-success, other-failure
*/
liblms7_status LMS7002M::CalibrateRx(float_type bandwidth_MHz)
{
    liblms7_status status;
    uint32_t minRSSI_i;
    uint32_t minRSSI_q;
    int16_t iqcorr_rx = 0;
    uint32_t minRSSI_iq;
    int16_t dcoffi;
    int16_t dcoffq;

    const int16_t firCoefs[] =
    {
        -2531,
        -517,
        2708,
        188,
        -3059,
        216,
        3569,
        -770,
        -4199,
        1541,
        4886,
        -2577,
        -5552,
        3909,
        6108,
        -5537,
        -6457,
        7440,
        6507,
        -9566,
        -6174,
        11845,
        5391,
        -14179,
        -4110,
        16457,
        2310,
        -18561,
        0,
        20369,
        -2780,
        -21752,
        5963,
        22610,
        -9456,
        -22859,
        13127,
        22444,
        -16854,
        -21319,
        20489,
        19492,
        -23883,
        -17002,
        26881,
        13902,
        -29372,
        -10313,
        31226,
        6345,
        -32380,
        -2141,
        32767,
        -2141,
        -32380,
        6345,
        31226,
        -10313,
        -29372,
        13902,
        26881,
        -17002,
        -23883,
        19492,
        20489,
        -21319,
        -16854,
        22444,
        13127,
        -22859,
        -9456,
        22610,
        5963,
        -21752,
        -2780,
        20369,
        0,
        -18561,
        2310,
        16457,
        -4110,
        -14179,
        5391,
        11845,
        -6174,
        -9566,
        6507,
        7440,
        -6457,
        -5537,
        6108,
        3909,
        -5552,
        -2577,
        4886,
        1541,
        -4199,
        -770,
        3569,
        216,
        -3059,
        188,
        2708,
        -517,
        -2531
    };

    Log("Rx calibration started", LOG_INFO);
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));
    Log("Saving registers state", LOG_INFO);
    BackupAllRegisters();
    uint8_t sel_path_rfe = (uint8_t)Get_SPI_Reg_bits(LMS7param(SEL_PATH_RFE));
    if (sel_path_rfe == 1 || sel_path_rfe == 0)
        return LIBLMS7_BAD_SEL_PATH;

    Log("Setup stage", LOG_INFO);
    status = CalibrateRxSetup(bandwidth_MHz);
    if (status != LIBLMS7_SUCCESS)
        goto RxCalibrationEndStage;

    Log("Rx DC calibration", LOG_INFO);
    CalibrateRxDC_RSSI();
    dcoffi = Get_SPI_Reg_bits(LMS7param(DCOFFI_RFE));
    dcoffq = Get_SPI_Reg_bits(LMS7param(DCOFFQ_RFE));
    Modify_SPI_Reg_bits(LMS7param(EN_G_TRF), 1);

    if (sel_path_rfe == 2)
    {
        Modify_SPI_Reg_bits(LMS7param(PD_RLOOPB_2_RFE), 0);
        Modify_SPI_Reg_bits(0x0103, 10, 10, 1);
        Modify_SPI_Reg_bits(0x0103, 11, 11, 0);
        Modify_SPI_Reg_bits(LMS7param(EN_INSHSW_LB2_RFE), 0);
    }
    if (sel_path_rfe == 3)
    {
        Modify_SPI_Reg_bits(LMS7param(PD_RLOOPB_1_RFE), 0);
        Modify_SPI_Reg_bits(0x0103, 11, 11, 1);
        Modify_SPI_Reg_bits(0x0103, 10, 10, 0);
        Modify_SPI_Reg_bits(LMS7param(EN_INSHSW_LB1_RFE), 0);
    }

    Modify_SPI_Reg_bits(0x040C, 7, 7, 0); //CMIX_BYP 0
    Modify_SPI_Reg_bits(0x040C, 2, 0, 0); //DC_BYP 0, GC_BYP 0, PH_BYP 0
    Modify_SPI_Reg_bits(LMS7param(CMIX_GAIN_RXTSP), 1); //CMIX_GAIN 1  +6 db
    Modify_SPI_Reg_bits(0x040C, 13, 13, 1); //CMIX_SC 1

    FixRXSaturation();

    Modify_SPI_Reg_bits(0x040C, 5, 5, 0); //GFIR3_BYP 0
    Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), 2);
    Modify_SPI_Reg_bits(LMS7param(GFIR3_L_RXTSP), 7);
    Modify_SPI_Reg_bits(LMS7param(GFIR3_N_RXTSP), 7);

    SetGFIRCoefficients(Rx, 2, firCoefs, sizeof(firCoefs) / sizeof(int16_t));

    SetNCOFrequency(Rx, 0, bandwidth_MHz / 4 + 1);
    Modify_SPI_Reg_bits(LMS7param(GCORRI_RXTSP), 2047);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_RXTSP), 2047);

    Log("IQ correction stage", LOG_INFO);
    iqcorr_rx = 0;
    for (int i = 0; i<9; ++i)
        minRSSI_iq = FindMinRSSI(LMS7param(IQCORR_RXTSP), iqcorr_rx, &iqcorr_rx, 3, 1, 256 >> i);
    Modify_SPI_Reg_bits(LMS7param(IQCORR_RXTSP), iqcorr_rx);

    uint16_t mingcorri;
    Log("I gain", LOG_INFO);
    minRSSI_i = FindMinRSSI_Gain(LMS7param(GCORRI_RXTSP), &mingcorri);

    Modify_SPI_Reg_bits(LMS7param(GCORRI_RXTSP), 2047);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_RXTSP), 2047);

    Log("Q gain", LOG_INFO);
    uint16_t mingcorrq;
    minRSSI_q = FindMinRSSI_Gain(LMS7param(GCORRQ_RXTSP), &mingcorrq);

    if (minRSSI_i < minRSSI_q)
        mingcorrq = 2047;
    else
        mingcorri = 2047;

    Modify_SPI_Reg_bits(LMS7param(GCORRI_RXTSP), mingcorri);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_RXTSP), mingcorrq);

    Log("Phase", LOG_INFO);
    for (int i = 0; i<9; ++i)
        minRSSI_iq = FindMinRSSI(LMS7param(IQCORR_RXTSP), iqcorr_rx, &iqcorr_rx, 3, 1, 256 >> i);

RxCalibrationEndStage:
    Log("Restoring registers state", LOG_INFO);
    RestoreAllRegisters();
    if (status != LIBLMS7_SUCCESS)
    {
        Log("Rx calibration failed", LOG_WARNING);
        return status;
    }

    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
    SetRxDCOFF((int8_t)dcoffi, (int8_t)dcoffq);
    Modify_SPI_Reg_bits(LMS7param(EN_DCOFF_RXFE_RFE), 1);
    Modify_SPI_Reg_bits(LMS7param(GCORRI_RXTSP), mingcorri);
    Modify_SPI_Reg_bits(LMS7param(GCORRQ_RXTSP), mingcorrq);
    Modify_SPI_Reg_bits(LMS7param(IQCORR_RXTSP), iqcorr_rx);
    Modify_SPI_Reg_bits(0x040C, 2, 0, 0); //DC_BYP 0, GC_BYP 0, PH_BYP 0
    Modify_SPI_Reg_bits(0x0110, 4, 0, 31); //ICT_LO_RFE 31
    Log("Rx calibration finished", LOG_INFO);
    return LIBLMS7_SUCCESS;
} 

const uint16_t backupAddrs[] = {
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028,
    0x0029, 0x002A, 0x002B, 0x002C, 0x002E, 0x0081, 0x0082, 0x0084, 0x0085,
    0x0086, 0x0087, 0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x0092, 0x0093, 0x0094,
    0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E,
    0x009F, 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8,
    0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0100, 0x0101, 0x0102, 0x0103,
    0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010C, 0x010D, 0x010E,
    0x010F, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118,
    0x0119, 0x011A, 0x011C, 0x011D, 0x011E, 0x011F, 0x0120, 0x0121, 0x0122, 0x0123,
    0x0124, 0x0200, 0x0201, 0x0202, 0x0203, 0x0204, 0x0205, 0x0206, 0x0207, 0x0208,
    0x0240, 0x0242, 0x0243, 0x0400, 0x0401, 0x0402,
    0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B, 0x040C,
    0x0440, 0x0442, 0x0443 };
uint16_t backupRegs[sizeof(backupAddrs) / 2];
uint16_t backupSXAddr[] = { 0x011C, 0x011D, 0x011E, 0x011F, 0x01200, 0x0121, 0x0122, 0x0123, 0x0124 };
uint16_t backupRegsSXR[sizeof(backupSXAddr) / 2];
uint16_t backupRegsSXT[sizeof(backupSXAddr) / 2];

/** @brief Stores chip current registers state into memory for later restoration
*/
void LMS7002M::BackupAllRegisters()
{
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));
    SPI_read_batch(backupAddrs, backupRegs, sizeof(backupAddrs) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), 1); // channel A
    SPI_read_batch(backupRegsSXR, backupSXAddr, sizeof(backupRegsSXR) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), 2); // channel B
    SPI_read_batch(backupRegsSXT, backupSXAddr, sizeof(backupRegsSXR) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
}

/** @brief Sets chip registers to state that was stored in memory using BackupAllRegisters()
*/
void LMS7002M::RestoreAllRegisters()
{
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));
    SPI_write_batch(backupAddrs, backupRegs, sizeof(backupAddrs) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), 1); // channel A
    SPI_write_batch(backupRegsSXR, backupSXAddr, sizeof(backupRegsSXR) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), 2); // channel B
    SPI_write_batch(backupRegsSXT, backupSXAddr, sizeof(backupRegsSXR) / sizeof(uint16_t));
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);
}

/** @brief Searches for minimal digital RSSI value by changing given gain parameter
    @param param LMS7002M gain correction parameter
    @param foundValue returns value which achieved minimal RSSI
    @return minimal found RSSI value
*/
uint32_t LMS7002M::FindMinRSSI_Gain(const LMS7Parameter &param, uint16_t *foundValue)
{
    uint32_t RSSI = ~0 - 2;
    uint32_t prevRSSI = RSSI + 1;
    uint8_t decrement = 2;
    uint16_t gcorr = 2047;
    while (gcorr > 1024)
    {
        Modify_SPI_Reg_bits(param, gcorr);
        RSSI = GetRSSI();
        if (RSSI < prevRSSI)
        {
            prevRSSI = RSSI;
            *foundValue = gcorr;
            gcorr -= decrement;
            decrement *= 2;
        }
        else
        {
            if (decrement == 2)
                break;
            gcorr -= decrement;
            decrement = 2;
        }
    }
    return prevRSSI;
}

/** @brief Reads all chip configuration and checks if it matches with local registers copy
*/
bool LMS7002M::IsSynced()
{
    if (controlPort->IsOpen() == false)
        return false;
    bool isSynced = true;
    liblms7_status status;    

    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));

    vector<uint16_t> addrToRead;
    for (uint16_t i = 0; i < sizeof(moduleAddresses)/sizeof(uint16_t); ++i)        
            addrToRead.push_back(moduleAddresses[i]);
    vector<uint16_t> dataReceived;
    dataReceived.resize(addrToRead.size(), 0);

    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    if (status != LIBLMS7_SUCCESS)
    {
        isSynced = false;
        goto isSyncedEnding;
    }        

    //mask out readonly bits
    for (uint16_t j = 0; j < sizeof(readOnlyRegisters) / sizeof(uint16_t); ++j)
        for (uint16_t k = 0; k < addrToRead.size(); ++k)
            if (readOnlyRegisters[j] == addrToRead[k])
            {
                dataReceived[k] &= readOnlyRegistersMasks[j];
                break;
            }

    //check if local copy matches chip
    for (uint16_t i = 0; i < addrToRead.size(); ++i)
    {
        if (dataReceived[i] != mLocalRegistersCopy[0][i])
        {
            isSynced = false;
            goto isSyncedEnding;
        }
    }

    addrToRead.clear(); //add only B channel addresses
    for (uint16_t i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
        if (moduleAddresses[i] >= 0x0100)
            addrToRead.push_back(moduleAddresses[i]);

    //mask out readonly bits
    for (uint16_t j = 0; j < sizeof(readOnlyRegisters) / sizeof(uint16_t); ++j)
        for (uint16_t k = 0; k < addrToRead.size(); ++k)
            if (readOnlyRegisters[j] == addrToRead[k])
            {
                dataReceived[k] &= readOnlyRegistersMasks[j];
                break;
            }

    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    if (status != LIBLMS7_SUCCESS)
        return false;
    //check if local copy matches chip
    for (uint16_t i = 0; i < addrToRead.size(); ++i)    
        if (dataReceived[i] != mLocalRegistersCopy[0][i])
        {
            isSynced = false;
            break;
        }

isSyncedEnding:
    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //restore previously used channel
    return isSynced;
}

liblms7_status LMS7002M::UploadAll()
{
    if (controlPort == NULL)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;

    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC)); //remember used channel

    liblms7_status status;
        
    vector<uint16_t> addrToWrite;
    vector<uint16_t> dataToWrite;

    uint16_t x0020_value = 0;
    Modify_SPI_Reg_bits(LMS7param(MAC), 1); //select A channel
    for (int i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
    {        
        if (moduleAddresses[i] == LMS7param(MAC).address) //skip register containing channel selection
        {
            x0020_value = mLocalRegistersCopy[0][i];
            continue;
        }
        addrToWrite.push_back(moduleAddresses[i]);
        dataToWrite.push_back(mLocalRegistersCopy[0][i]);
    }
    status = SPI_write_batch(&addrToWrite[0], &dataToWrite[0], addrToWrite.size());
    status = LIBLMS7_SUCCESS;
    if (status != LIBLMS7_SUCCESS)
        return status;
    status = SPI_write(0x0020, x0020_value);
    if (status != LIBLMS7_SUCCESS)
        return status;
    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    if (status != LIBLMS7_SUCCESS)
        return status;

    addrToWrite.clear();
    dataToWrite.clear();
    for (int i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
    {        
        if (moduleAddresses[i] >= 0x0100)
        {
            addrToWrite.push_back(moduleAddresses[i]);
            dataToWrite.push_back(mLocalRegistersCopy[0][i]);
        }
    }
    Modify_SPI_Reg_bits(LMS7param(MAC), 2); //select B channel
    status = SPI_write_batch(&addrToWrite[0], &dataToWrite[0], addrToWrite.size());
    if (status != LIBLMS7_SUCCESS)
        return status;
    Modify_SPI_Reg_bits(LMS7param(MAC), ch);    
    return LIBLMS7_SUCCESS;
}

liblms7_status LMS7002M::DownloadAll()
{
    if (controlPort == nullptr)
        return LIBLMS7_NO_CONNECTION_MANAGER;
    if (controlPort->IsOpen() == false)
        return LIBLMS7_NOT_CONNECTED;
    liblms7_status status;    
    uint8_t ch = (uint8_t)Get_SPI_Reg_bits(LMS7param(MAC));

    vector<uint16_t> addrToRead;
    for (uint16_t i = 0; i < sizeof(moduleAddresses)/sizeof(uint16_t); ++i)        
        addrToRead.push_back(moduleAddresses[i]);
    vector<uint16_t> dataReceived;
    dataReceived.resize(addrToRead.size(), 0);
    Modify_SPI_Reg_bits(LMS7param(MAC), 1);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    if (status != LIBLMS7_SUCCESS)
        return status;

    for (uint16_t i = 0; i < addrToRead.size(); ++i)
        mLocalRegistersCopy[0][i] = dataReceived[i];

    addrToRead.clear(); //add only B channel addresses
    uint16_t bChannelStartOffset = 0;
    for (uint16_t i = 0; i < sizeof(moduleAddresses) / sizeof(uint16_t); ++i)
    {
        if (moduleAddresses[i] >= 0x0100)
            addrToRead.push_back(moduleAddresses[i]);
        else
            ++bChannelStartOffset;
    }

    Modify_SPI_Reg_bits(LMS7param(MAC), 2);
    status = SPI_read_batch(&addrToRead[0], &dataReceived[0], addrToRead.size());
    if (status != LIBLMS7_SUCCESS)
        return status;
    for (uint16_t i = 0; i < addrToRead.size(); ++i)
    {
        mLocalRegistersCopy[1][i + bChannelStartOffset] = dataReceived[i];
    }

    Modify_SPI_Reg_bits(LMS7param(MAC), ch); //retore previously used channel
    return LIBLMS7_SUCCESS;
}

/** @brief Configures interfaces for desired frequency
*/
liblms7_status LMS7002M::SetInterfaceFrequency(float_type cgen_freq_MHz, const uint8_t interpolation, const uint8_t decimation)
{
    Modify_SPI_Reg_bits(LMS7param(HBD_OVR_RXTSP), decimation);
    Modify_SPI_Reg_bits(LMS7param(HBI_OVR_TXTSP), interpolation);
    liblms7_status status = SetFrequencyCGEN(cgen_freq_MHz);
    if (status != LIBLMS7_SUCCESS)
        return status;
    
    if (decimation == 7 || decimation == 0) //bypass
    {
        Modify_SPI_Reg_bits(LMS7param(RXTSPCLKA_DIV), 0);
        Modify_SPI_Reg_bits(LMS7param(RXDIVEN), false);
        Modify_SPI_Reg_bits(LMS7param(MCLK2SRC), 3);
    }
    else
    {
        uint8_t divider = (uint8_t)pow(2.0, decimation);
        if (divider > 1)
            Modify_SPI_Reg_bits(LMS7param(RXTSPCLKA_DIV), (divider / 2) - 1);
        else
            Modify_SPI_Reg_bits(LMS7param(RXTSPCLKA_DIV), 0);
        Modify_SPI_Reg_bits(LMS7param(RXDIVEN), true);
        Modify_SPI_Reg_bits(LMS7param(MCLK2SRC), 1);
    }
    if (interpolation == 7 || interpolation == 0) //bypass
    {
        Modify_SPI_Reg_bits(LMS7param(TXTSPCLKA_DIV), 0);
        Modify_SPI_Reg_bits(LMS7param(TXDIVEN), false);
        Modify_SPI_Reg_bits(LMS7param(MCLK1SRC), 2);
    }
    else
    {
        uint8_t divider = (uint8_t)pow(2.0, interpolation);
        if (divider > 1)
            Modify_SPI_Reg_bits(LMS7param(TXTSPCLKA_DIV), (divider / 2) - 1);
        else
            Modify_SPI_Reg_bits(LMS7param(TXTSPCLKA_DIV), 0);
        Modify_SPI_Reg_bits(LMS7param(TXDIVEN), true);
        Modify_SPI_Reg_bits(LMS7param(MCLK1SRC), 0);     
    }
    return status;
}
