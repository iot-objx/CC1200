#ifndef _CC120X_MISC_H
#define _CC120X_MISC_H

/* =====================================================================================================================
												CUSTOM TYPE DEFINATIONS
  ===================================================================================================================== */
// State/Status Types
//enum StatType { STATUS, MARC_STATE, MARC_STATUS0, MARC_STATUS1, MODEM_STATUS0, MODEM_STATUS1 };

typedef struct RegValuePair
{
	uint16_t REGISTER;
	byte VALUE;
} registerSetting_t;

/* =====================================================================================================================
													STATUS/ERROR CODES
  ===================================================================================================================== */
// Extracted from: CC120X Low-Power High Performance Sub-1 GHz RF
//                                    Transceivers (Rev. B) -- swru346b.pdf



/******************************************************************************
* STATUS BYTE CODES - Page 8, Table 2: Status Byte Summary
*/
/******************************************************************************
* Status byte:
*              ---------------------------------------------
*              |          |             |                  |
*              | CHIP_RDY | STATE [2:0] |  RESERVED [3:0]  |
*              |          |             |                  |
*              ---------------------------------------------
* CHIP_RDYn Stays high until power and crystal have stabilized.
*           Should always be low when using the SPI interface.
*/
//                      Value    MARC State
#define STATE_IDLE        0   // IDLE
#define STATE_RX          1   // RX | RX_END => Receive Mode
#define STATE_TX          2   // TX | TX_END => Transmit Mode
#define STATE_FSTXON      3   // FSTXON => Fast TX ready
#define STATE_CALIBRATE   4   // ... => Frequency synthesizer calibration is running
#define STATE_SETTLING    5   // ... => PLL is settling
#define STATE_RX_FIFO_ERR 6   // RX_FIFO_ERR => RX FIFO over/underflowed. Flush FIFO with SFRX strobe.
#define STATE_TX_FIFO_ERR 7   // TX_FIFO_ERR => TX FIFO over/underflowed. Flush FIFO with SFTX strobe.
/******************************************************************************/





/*******************************************************************************
* MARC STATUS 0 CODES - Page: 111, MARC_STATUS0 - MARC Status Reg. 0
* Read TXONCCA_FAILED after the TXONCCA_DONE signal has been asserted.
*/
/******************************************************************************
* MARC Status 0 byte:
------------------------------------------------------------------------------------------------------------------
|                             |                        |                |                        |               |
| MARC_STATUS0_NOT_USED [7:4] | MARC_STATUS0_RESERVED3 | TXONCCA_FAILED | MARC_STATUS0_RESERVED1 | RCC_CAL_VALID |
|                             |                        |                |                        |               |
------------------------------------------------------------------------------------------------------------------
*/
//                                                Description
#define MARC_STATUS0_TXONCCA_FAILED_CLR 0      // The channel was clear. The radio will enter TX state.
#define MARC_STATUS0_TXONCCA_FAILED_BSY 1      // The channel was busy. The radio will remain in RX state.
#define MARC_STATUS0_RCC_CAL_VALID      0      // RCOSC has been calibrated at least once. 0x00 at Reset.
/******************************************************************************/





/*******************************************************************************
* MARC STATUS 1 CODES - Page: 22, Table 11: MARC_STATUS_OUT.
*                       Page: 111, MARC_STATUS1 - MARC Status Reg. 1
* All the different cases that can initiate a MCU wake-up (assertion of MCU_WAKEUP).
* This register should be read to find what caused the MCU_WAKEUP signal
* to be asserted. >> MARC_STATUS1.MARC_STATUS_OUT
*/
/******************************************************************************
* MARC Status 1 byte:
*              --------------------------------------------
*              |                                          |
*              |           MARC_STATUS_OUT [7:0]          |
*              |                                          |
*              --------------------------------------------
*/
/******************************************************************************
+---------------+------------------------------------------------------------+
|MARC_STATUS_OUT|                        Description                         |
+---------------+------------------------------------------------------------+
|   0000 0000   | 00 - No failure                                            |
|   0000 0001   | 01 - RX Timeout.                                           |
|   0000 0010   | 02 - RX Termination based on CS or PQT.                    |
|   0000 0011   | 03 - eWOR sync lost (16 slots with no reception.)          |
|   0000 0100   | 04 - Packet discarded due to maximum length filtering.     |
|   0000 0101   | 05 - Packet discarded due to address filtering.            |
|   0000 0110   | 06 - Packet discarded due to CRC filtering.                |
|   0000 0111   | 07 - TX FIFO overflow error                                |
|   0000 1000   | 08 - TX FIFO underflow error                               |
|   0000 1001   | 09 - RX FIFO overflow error                                |
|   0000 1010   | 10 - RX FIFO underflow error                | busy channel.|
|   0000 1011   | 11 - TX ON CCA failed. A TX strobe was ignored due to a ^  |
|   0100 0000   | 64 - TX finished successfully (CC120X is ready for next op)|
|   1000 0000   | 128 - RX finished successfully (a packet is in the RX FIFO ready to be read)  |
+---------------+------------------------------------------------------------+
*/
#define MARC_STATUS1_OUT_NONE            0x00
#define MARC_STATUS1_OUT_RX_TIMEOUT      0x01
#define MARC_STATUS1_OUT_RX_TERM         0x02
#define MARC_STATUS1_OUT_EWOR_SYNC_LOST  0x03
#define MARC_STATUS1_OUT_PACKET_DROP_LEN 0x04
#define MARC_STATUS1_OUT_PACKET_DROP_ADR 0x05
#define MARC_STATUS1_OUT_PACKET_DROP_CRC 0x06
#define MARC_STATUS1_OUT_TX_FIFO_OVERR   0x07
#define MARC_STATUS1_OUT_TX_FIFO_UNERR   0x08
#define MARC_STATUS1_OUT_RX_FIFO_OVERR   0x09
#define MARC_STATUS1_OUT_RX_FIFO_UNERR   0x0A
#define MARC_STATUS1_OUT_TXONCCA_FAILED  0x0B
#define MARC_STATUS1_OUT_TX_OK           0x40
#define MARC_STATUS1_OUT_RX_OK           0x80
/******************************************************************************/





/******************************************************************************
* MARC STATE REGISTER - Page 106, MARCSTATE - MARC State | Register Description
*/
/******************************************************************************
* MARC State byte:
*        -----------------------------------------------------------------
*        |                    |                       |                  |
*        | MARCSTATE_NOT_USED | MARC_2PIN_STATE [6:5] | MARC_STATE [4:0] |
*        |                    |                       |                  |
*        -----------------------------------------------------------------
*/

// MARC 2 pin state value
#define MARC_STATE_2PIN_SETTLING  0
#define MARC_STATE_2PIN_TX        1
#define MARC_STATE_2PIN_IDLE      2
#define MARC_STATE_2PIN_RX        3

// MARC state                                 MARC 2 pin state value
#define MARC_STATE_SLEEP          0           // ????                Read Not Possible
#define MARC_STATE_IDLE           1           // IDLE
#define MARC_STATE_XOFF           2           //                     Read Not Possible
#define MARC_STATE_BIAS_SETTLE_MC 3
#define MARC_STATE_REG_SETTLE_MC  4
#define MARC_STATE_MANCAL         5
#define MARC_STATE_BIAS_SETTLE    6
#define MARC_STATE_REG_SETTLE     7
#define MARC_STATE_STARTCAL       8
#define MARC_STATE_BWBOOST        9
#define MARC_STATE_FS_LOCK       10
#define MARC_STATE_IFADCON       11
#define MARC_STATE_ENDCAL        12
#define MARC_STATE_RX            13           // RX
#define MARC_STATE_RX_END        14           // RX
#define MARC_STATE_RXDCM         15           // RX
#define MARC_STATE_TXRX_SWITCH   16
#define MARC_STATE_RX_FIFO_ERR   17
#define MARC_STATE_FSTXON        18
#define MARC_STATE_TX            19           // TX
#define MARC_STATE_TX_END        20           // TX
#define MARC_STATE_RXTX_SWITCH   21
#define MARC_STATE_TX_FIFO_ERR   22
#define MARC_STATE_IFADCON_TXRX  23
#define MARC_STATE_RESERVED      24 // 24-31 RESERVED. Blanks are SETTLING.
/******************************************************************************/

#endif // !_CC120X_MISC_H

