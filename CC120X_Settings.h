#ifndef CC120X_SETTINGS_H
#define CC120X_SETTINGS_H

/******************************************************************************
* INCLUDES
*/
#include "CC120X_Misc.h"		// import custom types
#include "CC120X_Registers.h"   // import CC120X Specific Registers


/******************************************************************************
* Settings Array Lengths
*/
const uint8_t rxSniffSettLen = 49;
const uint8_t prefSettLen = 48;


/******************************************************************************
* RX SNIFF MODE SETTINGS
*/
// Address Config = No address check
// Bit Rate = 2.4
// Carrier Frequency = 867.999878
// Deviation = 3.986359
// Device Address = 0
// Manchester Enable = false
// Modulation Format = 2-FSK
// Packet Bit Length = 0
// Packet Length = 255
// Packet Length Mode = Variable
// RX Filter BW = 25.252525
// Symbol rate = 2.4
// Whitening = false
const registerSetting_t rxSniffSettings[] = {
	{ CC120X_IOCFG2,            0x06 },
	{ CC120X_DEVIATION_M,       0xD1 },
	{ CC120X_MODCFG_DEV_E,      0x00 },
	{ CC120X_DCFILT_CFG,        0x5D },
	{ CC120X_PREAMBLE_CFG0,     0x8A },
	{ CC120X_IQIC,              0xCB },
	{ CC120X_CHAN_BW,           0x61 },
	{ CC120X_MDMCFG1,           0x40 },
	{ CC120X_MDMCFG0,           0x05 },
	{ CC120X_SYMBOL_RATE2,      0x4F },
	{ CC120X_SYMBOL_RATE1,      0x75 },
	{ CC120X_SYMBOL_RATE0,      0x10 },
	{ CC120X_AGC_REF,           0x33 },
	{ CC120X_AGC_CS_THR,        0x09 },
	{ CC120X_AGC_CFG1,          0x40 },
	{ CC120X_AGC_CFG0,          0x83 },
	{ CC120X_FIFO_CFG,          0x00 },
	{ CC120X_SETTLING_CFG,      0x03 },
	{ CC120X_FS_CFG,            0x12 },
	{ CC120X_WOR_CFG0,          0x20 },
	{ CC120X_WOR_EVENT0_MSB,    0x03 },
	{ CC120X_WOR_EVENT0_LSB,    0xE7 },
	{ CC120X_PKT_CFG2,          0x00 },  // Always give clear channel indication
	{ CC120X_PKT_CFG0,          0x20 },  // Variable Packet Length
	{ CC120X_RFEND_CFG0,        0x09 },  // Term on Bad packet. Term based on CS.
	{ CC120X_PKT_LEN,           0xFF },  // Maximum allowed length packets
	{ CC120X_IF_MIX_CFG,        0x1C },
	{ CC120X_FREQOFF_CFG,       0x22 },
	{ CC120X_MDMCFG2,           0x0C },
	{ CC120X_FREQ2,             0x56 },
	{ CC120X_FREQ1,             0xCC },
	{ CC120X_FREQ0,             0xCC },
	{ CC120X_IF_ADC1,           0xEE },
	{ CC120X_IF_ADC0,           0x10 },
	{ CC120X_FS_DIG1,           0x07 },
	{ CC120X_FS_DIG0,           0xAF },
	{ CC120X_FS_CAL1,           0x40 },
	{ CC120X_FS_CAL0,           0x0E },
	{ CC120X_FS_DIVTWO,         0x03 },
	{ CC120X_FS_DSM1,           0x02 },
	{ CC120X_FS_DSM0,           0x33 },
	{ CC120X_FS_DVC1,           0xF3 },
	{ CC120X_FS_DVC0,           0x13 },
	{ CC120X_FS_PFD,            0x00 },
	{ CC120X_FS_PRE,            0x6E },
	{ CC120X_FS_REG_DIV_CML,    0x1C },
	{ CC120X_FS_SPARE,          0xAC },
	{ CC120X_FS_VCO0,           0xB8 },
	{ CC120X_XOSC5,             0x0E },
};
/*****************************************************************************/


/******************************************************************************
* RF CHIP RX/TX MODE VARIABLES
*/
// Address Config = Address check, 0x00 and 0xFF broadcast 
// Bit Rate = 4.8 
// Carrier Frequency = 867.999878 
// Deviation = 3.986359 
// Device Address = 0 
// Manchester Enable = false 
// Modulation Format = 2-FSK 
// Packet Bit Length = 0 
// Packet Length = 255 
// Packet Length Mode = Variable 
// RX Filter BW = 25.252525 
// Symbol rate = 4.8 
// Whitening = false 
const registerSetting_t preferredSettings[] = {
	{ CC120X_IOCFG2,         0x06 },  // PKT_SYNC_RXTX - RX: Asserted when sync word has been received, and de-asserted at the end.
	{ CC120X_DEVIATION_M,    0xD1 },  //                 TX: Asserted when sync word has been sent, and de-asserted at the end.
	{ CC120X_MODCFG_DEV_E,   0x00 },
	{ CC120X_DCFILT_CFG,     0x5D },
	{ CC120X_PREAMBLE_CFG0,  0x8A },
	{ CC120X_IQIC,           0xCB },
	{ CC120X_CHAN_BW,        0x61 },
	{ CC120X_MDMCFG1,        0x40 }, 
	{ CC120X_MDMCFG0,        0x05 },
	{ CC120X_SYMBOL_RATE2,   0x5F },
	{ CC120X_SYMBOL_RATE1,   0x75 },
	{ CC120X_SYMBOL_RATE0,   0x10 },
	{ CC120X_AGC_REF,        0x33 },
	{ CC120X_AGC_CS_THR,     0xEC },
	{ CC120X_AGC_CFG1,       0x51 },
	{ CC120X_AGC_CFG0,       0x87 },
	{ CC120X_FIFO_CFG,       0x80 },  // CRC_AUTOFLUSH - Flushes RX FIFO if a CRC error occurred.
	{ CC120X_FS_CFG,         0x12 },
	{ CC120X_PKT_CFG2,       0x00 },  // CCA_MODE - Always give a clear channel indication 0x00 / Indicates clear channel unless currently receiving a packet 0x08
	{ CC120X_PKT_CFG1,       0x1F },  // ADDR_CHECK_CFG and APPEND_STATUS - enabled
	{ CC120X_RFEND_CFG1,     0x0F },  // Default:0x0F (IDLE) | RXOFF_MODE: 0x2F(TX), 0x3F(RX)
	{ CC120X_RFEND_CFG0,     0x00 },  // Default:0x00		 | TXOFF_MODE: 0x20(TX), 0x30(RX)
	{ CC120X_PKT_CFG0,       0x20 },  // Variable packet length
	{ CC120X_PKT_LEN,        0xFF },
	{ CC120X_IF_MIX_CFG,     0x1C },
	{ CC120X_FREQOFF_CFG,    0x22 },
	{ CC120X_MDMCFG2,        0x0C },
	{ CC120X_FREQ2,          0x56 },
	{ CC120X_FREQ1,          0xCC },
	{ CC120X_FREQ0,          0xCC },
	{ CC120X_IF_ADC1,        0xEE },
	{ CC120X_IF_ADC0,        0x10 },
	{ CC120X_FS_DIG1,        0x07 },
	{ CC120X_FS_DIG0,        0xAF },
	{ CC120X_FS_CAL1,        0x40 },
	{ CC120X_FS_CAL0,        0x0E },
	{ CC120X_FS_DIVTWO,      0x03 },
	{ CC120X_FS_DSM0,        0x33 },
	{ CC120X_FS_DVC0,        0x17 },
	{ CC120X_FS_PFD,         0x00 },
	{ CC120X_FS_PRE,         0x6E },
	{ CC120X_FS_REG_DIV_CML, 0x1C },
	{ CC120X_FS_SPARE,       0xAC },
	{ CC120X_FS_VCO0,        0xB5 },
	{ CC120X_XOSC5,          0x0E },
	{ CC120X_XOSC1,          0x03 },
	{ CC120X_PARTNUMBER,     0x20 }, // Chip ID: CC1200(0x20) CC1201(0x21) 
	{ CC120X_PARTVERSION,    0x11 }, // Chip Revision
};
/*****************************************************************************/
#endif