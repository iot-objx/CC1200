/*

Copyright (c) 2018 Md Abdullah AL IMRAN | alimran.mdabdullah@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 1. Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
	derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "CC1200.h"

/* Basic Read-Write Access
																	R/~W B A5 A4 A3 A2 A1 A0 */
#define WRITE_SINGLE			0x00  // Single Byte Write Address -  0  0  x  x  x  x  x  x
#define WRITE_BURST				0x40  // Multi Byte Write Address  -  0  1  x  x  x  x  x  x
#define READ_SINGLE				0x80  // Single Byte Read Address  -  1  0  x  x  x  x  x  x
#define READ_BURST				0xC0  // Multi Byte Read Address   -  1  1  x  x  x  x  x  x

/* FIFO Access Modes - Standard OR Direct Memory Access (DMA) */
#define RADIO_FIFO_ACCESS_STD   0x3F
#define RADIO_FIFO_ACCESS_DMA   0x3E

// RSSI Offset (to be deducted)
#define RSSI_OFFSET				0x30  //dec = 48

// Define CC1200 chip as cc1200
CC1200 cc1200;

// Standard initialization - using default pins [SS, MOSI, MISO, SCK, 9 (RadioReset)]
void CC1200::Init(void)
{
	CC1200::Init(SS, MOSI, MISO, SCK, PIN_UNUSED);
}

// Advanced initialization - using user provided pins [SS, MOSI, MISO, SCK, RadioReset]
void CC1200::Init(uint8_t SS_PIN, uint8_t MOSI_PIN, uint8_t MISO_PIN, uint8_t SCK_PIN, int8_t RESET_PIN)
{
	// Record SPI pins
	_SS_PIN = SS_PIN;
	_MOSI_PIN = MOSI_PIN;
	_MISO_PIN = MISO_PIN;
	_SCK_PIN = SCK_PIN;	

	// SPI Setup
	_spi_begin();

	// Radio Setup
	_RESET_PIN = RESET_PIN;
	if (_RESET_PIN > PIN_UNUSED)
	{
		pinMode(_RESET_PIN, OUTPUT); delay(100);
	}
	Reset(true);

	delay(1000);
}

// Configure Radio
void CC1200::Configure(const registerSetting_t settings[], uint8_t len)
{
	// Method 1: Timing = (toggle of CSN/SS pin + 2 if-loops) * N	: SLOWER
	// Method 2: Timing =  toggle of CSN/SS pin + N if-loops		: FASTER

	// METHOD 1: Using _spi_write_register() method
	/*for (uint8_t i = 0; i < len; i++)
	{
		_spi_write_register(settings[i].REGISTER, &(settings[i].VALUE), 1);
	}*/

	// METHOD 2: Alternative of _spi_write_register() method
	digitalWrite(_SS_PIN, LOW); // Pull the SS pin LOW - Active
	wait_pin_low(_MISO_PIN); // Wait until MISO pin goes LOW
	
	for (uint8_t i = 0; i < len; i++)
	{
		bool normSpace = ((settings[i].REGISTER >> 8) == 0x2F) ? false : true;
		if (normSpace)
		{
			_spi_transfer(WRITE_SINGLE | lowByte(settings[i].REGISTER));
			_spi_transfer(settings[i].VALUE);
		}
		else
		{
			_spi_transfer(WRITE_SINGLE | highByte(settings[i].REGISTER));
			_spi_transfer(lowByte(settings[i].REGISTER));
			_spi_transfer(settings[i].VALUE);
		}
	}

	digitalWrite(_SS_PIN, HIGH); // Pull the SS pin HIGH - Inactive

	delay(2000);
}

// Get Status/State info. Returned result is keepBits bitwise AND-ed with Right(+)/Left(-) shifted.
byte CC1200::GetStat(StatType sType, byte keepBits, int8_t shiftLR)
{
	byte stat = 0x00;
	uint16_t address = sType;

	if (sType == STATUS)
	{
		digitalWrite(_SS_PIN, LOW); // Pull the SS pin LOW - Active
		wait_pin_low(_MISO_PIN); // Wait until MISO pin goes LOW
		stat = _spi_transfer(CC120X_SNOP);
		digitalWrite(_SS_PIN, HIGH); // Pull the SS pin HIGH - Inactive
	}
	else
	{
		_spi_read_register(address, &stat, 1); // Read 1 Byte of state/status
	}

	stat = stat & keepBits; // 
	
	// Shift if shiftLR is non-zero
	if (shiftLR > 0 && shiftLR <= 8) // '+' Right Shift
	{
		stat = stat >> shiftLR;
	}
	else if (shiftLR >= -8 && shiftLR < 0) // '-' Left Shift
	{
		stat = stat << abs(shiftLR);
	}

	return stat;
}

// Command Strobe [CC120X_S???]
void CC1200::Strobe(uint8_t command)
{
	// Execute if valid Command Strobe
	if (command >= 0x30 && command <= 0x3D)
	{
		_spi_strobe(command);
	}
}

// Reset the Chip. Performs Hard Reset if HWreset is TRUE else Soft Reset.
void CC1200::Reset(bool HWreset)
{
	if (HWreset && _RESET_PIN > PIN_UNUSED)
	{
		digitalWrite(_RESET_PIN, LOW); // Reset = Low
		delay(1000);
		digitalWrite(_RESET_PIN, HIGH);
		delay(1000);
	}
	else
	{
		_spi_strobe(CC120X_SRES);
	}
}

// Idle the Chip
void CC1200::Idle(void)
{
	_spi_strobe(CC120X_SIDLE);
}

// Power Down the Chip
void CC1200::PowerDown(void)
{
	_spi_strobe(CC120X_SPWD);
}

// Transmit data available in the TX FIFO
void CC1200::Transmit(void)
{
	_spi_strobe(CC120X_STX);
}

// Ready to receive incoming data into the RX FIFO
void CC1200::Receive(void)
{
	_spi_strobe(CC120X_SRX);
}

// Flush TX FIFO explicitly. (Call ONLY when in IDLE or TX FIFO error state)
void CC1200::FlushTxFifo(void)
{
	_spi_strobe(CC120X_SFTX);
}

// Flush RX FIFO explicitly. (Call ONLY when in IDLE or RX FIFO error state)
void CC1200::FlushRxFifo(void)
{
	_spi_strobe(CC120X_SFRX);
}

// Resolve FIFO Error (if any) - Return 0 (No Error), (1 TXFIFO Error) OR (2 RXFIFO Error)
int CC1200::ResolveFifoErr(void)
{
	int rtn = 0;
	byte state = cc1200.GetStat(StatType::MARC_STATE, 0x1F); // MARC_STATE [4:0]

	if (state == MARC_STATE_TX_FIFO_ERR)
	{
		FlushTxFifo();
		rtn++;
	}

	if (state == MARC_STATE_RX_FIFO_ERR)
	{
		FlushRxFifo();
		rtn++;
	}

	return rtn;
}

// Get Device's Address/ID. fast = FALSE explicitly reads from the Chip and is slower as opposed to software read.
uint8_t CC1200::GetAddress(bool fast)
{
	uint8_t devAddr = _DEVICE_ADDRESS;
	if (!fast)
	{
		_spi_read_register(CC120X_DEV_ADDR, &devAddr, 1);
		_DEVICE_ADDRESS = devAddr; // Update (just in case)
	}
	return devAddr;
}

// Set Device's Address/ID
void CC1200::SetAddress(uint8_t address)
{
	_DEVICE_ADDRESS = address;
	_spi_write_register(CC120X_DEV_ADDR, &address, 1);
}

// Read from Register
bool CC1200::ReadRegister(uint16_t address, byte readBuffer[], uint8_t len)
{
	if (len > 0)
	{
		_spi_read_register(address, readBuffer, len);
	}
	return (len > 0);
}

// Write to Register
bool CC1200::WriteRegister(uint16_t address, byte writeBuffer[], uint8_t len)
{
	if (len > 0)
	{
		_spi_write_register(address, writeBuffer, len);
	}
	return (len > 0);
}

// Update Register according to updateBits. (Different from WriteRegister)
void CC1200::UpdateRegister(uint16_t address, byte updateBits)
{
	byte oldValue, newValue;
	_spi_read_register(address, &oldValue, 1);
	newValue = oldValue | updateBits;
	if (newValue != oldValue)
	{
		_spi_write_register(address, &newValue, 1);
	}
}

// Read from RX FIFO and return amount of bytes read.
uint8_t CC1200::ReadRxFifo(byte readBuffer[])
{
	uint8_t readByte = 0;
	_spi_read_register(CC120X_NUM_RXBYTES, &readByte, 1);
	if (readByte > 0)
	{
		_spi_read_register(RADIO_FIFO_ACCESS_STD, readBuffer, readByte);
	}
	return readByte;
}

// Write to TX FIFO. Assumption: [Length Address --Payload-- +1Byte] where Length = AddressLen(1) + PayloadLength. 
void CC1200::WriteTxFifo(byte writeBuffer[], uint8_t len)
{
	if (len > 2) // 0: Length, 1: Target Address, 2: Source Address !!!
	{
		_spi_write_register(RADIO_FIFO_ACCESS_STD, writeBuffer, len + 1); // +1 otherwise original data overwritten !!!
	}
}

/* SPI Core Methods */
// Configure SPI
void CC1200::_spi_begin(void)
{
	// Configure SPI Pins
	pinMode(_SS_PIN, OUTPUT);
	pinMode(_MOSI_PIN, OUTPUT);
	pinMode(_MISO_PIN, INPUT);
	pinMode(_SCK_PIN, OUTPUT);

	digitalWrite(_SS_PIN, HIGH);
	digitalWrite(_SCK_PIN, HIGH);
	digitalWrite(_MOSI_PIN, LOW);

	// SPI Configuration
	/*
	Spi prescaler:
		SPI2X SPR1 SPR0
			0     0     0    fosc/4
			0     0     1    fosc/16
			0     1     0    fosc/64
			0     1     1    fosc/128
			1     0     0    fosc/2
			1     0     1    fosc/8
			1     1     0    fosc/32
			1     1     1    fosc/64
	*/
	//SPCR = ((1 << SPE) |					// SPI Enable
	//		(0 << SPIE) |					// SPI Interupt Enable
	//		(0 << DORD) |					// Data Order (0:MSB first / 1:LSB first)
	//		(1 << MSTR) |					// Master/Slave select
	//		(0 << SPR1) | (0 << SPR0) |		// SPI Clock Rate
	//		(0 << CPOL) |					// Clock Polarity (0:SCK low / 1:SCK hi when idle)
	//		(0 << CPHA));					// Clock Phase (0:leading / 1:trailing edge sampling)
	//SPSR = (1 << SPI2X);					// Double Clock Rate
	
	byte dummy;
	SPCR = 0;								// Reset to defaults
	SPCR = _BV(SPE) | _BV(MSTR);			// SPI Enable as Master with speed = clk/4
	dummy = SPCR;
	dummy = SPDR;
}

// De-configure SPI
void CC1200::_spi_end(void)
{
	// SPI Pin Directions
	pinMode(_SS_PIN, INPUT);
	pinMode(_MOSI_PIN, INPUT);
	pinMode(_MISO_PIN, INPUT);
	pinMode(_SCK_PIN, INPUT);

	SPCR = 0;
	//SPSR = (0 << SPI2X);					// Double Clock Rate
}

// Strobe command via SPI
void CC1200::_spi_strobe(uint8_t command)
{
	digitalWrite(_SS_PIN, LOW); // Pull the SS pin LOW - Active
	//while (!digitalRead(_MOSI_PIN)); // Wait until MOSI pin goes HIGH
	wait_pin_low(_MISO_PIN); // Wait until MISO pin goes LOw
	_spi_transfer(command);
	//Serial.println("  STROBE OK");
	digitalWrite(_SS_PIN, HIGH); // Pull the SS pin HIGH - Inactive
}

// SPI Single Byte Read/Write
uint8_t CC1200::_spi_transfer(uint8_t data)
{
	SPDR = data; //Serial.println("  DATA WAIT");
	//while (!(SPSR & (1 << SPIF)));       // Wait until the byte has been sent
	wait_spi(); //Serial.println("  DATA OK");
	return SPDR;
}

// SPI Read - Single or Multiple Bytes from Normal/Extended Register space
void CC1200::_spi_read_register(uint16_t address, uint8_t *buffer, uint8_t len)
{
	/*
		NORMAL[0x00-0x2E] or EXTENDED[0x2F00-0x2FFF] space
	*/
	bool normSpace = ((address >> 8) == 0x002F) ? false : true;

	digitalWrite(_SS_PIN, LOW); // Pull the SS pin LOW - Active
	wait_pin_low(_MISO_PIN); // Wait until MISO pin goes LOW

	if (normSpace) // Normal Address Space
	{
		if (len == 1) // Single Read
		{
			_spi_transfer(READ_SINGLE | lowByte(address));
			buffer[0] = _spi_transfer(0xFF); // Put dummy byte to get the received byte
		}
		else // Burst Read
		{
			_spi_transfer(READ_BURST | lowByte(address));
			for (uint8_t i = 0; i < len; i++)
			{
				buffer[i] = _spi_transfer(0xFF); // Put dummy byte to get the received byte
			}
		}
	}
	else // Extended Address Space
	{
		if (len == 1) // Single Read
		{
			_spi_transfer(READ_SINGLE | highByte(address)); // 0x2F??
			_spi_transfer(lowByte(address));
			buffer[0] = _spi_transfer(0xFF); // Put dummy byte to get the received byte
		}
		else // Burst Read
		{
			_spi_transfer(READ_BURST | highByte(address)); // 0x2F??
			_spi_transfer(lowByte(address));
			for (uint8_t i = 0; i < len; i++)
			{
				buffer[i] = _spi_transfer(0xFF); // Put dummy byte to get the received byte
			}
		}
	}

	digitalWrite(_SS_PIN, HIGH); // Pull the SS pin HIGH - Inactive
}

// SPI Write - Single or Multiple Bytes to Normal/Extended Register space
void CC1200::_spi_write_register(uint16_t address, uint8_t *buffer, uint8_t len)
{
	/*
		NORMAL[0x00-0x2E] or EXTENDED[0x2F00-0x2FFF] space
	*/
	bool normSpace = ((address >> 8) == 0x2F) ? false : true;

	digitalWrite(_SS_PIN, LOW); // Pull the SS pin LOW - Active
	wait_pin_low(_MISO_PIN); // Wait until MISO pin goes LOW

	if (normSpace) // Normal Address Space
	{
		if (len == 1) // Single Write
		{
			_spi_transfer(WRITE_SINGLE | lowByte(address));
			_spi_transfer(buffer[0]);
		}
		else // Burst Write
		{
			_spi_transfer(WRITE_BURST | lowByte(address));
			for (uint8_t i = 0; i < len; i++)
			{
				_spi_transfer(buffer[i]);
			}
		}
	}
	else // Extended Address Space
	{
		if (len == 1) // Single Write
		{
			_spi_transfer(WRITE_SINGLE | highByte(address)); // 0x2F??
			_spi_transfer(lowByte(address));
			_spi_transfer(buffer[0]);
		}
		else // Burst Write
		{
			_spi_transfer(WRITE_BURST | highByte(address)); // 0x2F??
			_spi_transfer(lowByte(address));
			for (uint8_t i = 0; i < len; i++)
			{
				_spi_transfer(buffer[i]);
			}
		}
	}

	digitalWrite(_SS_PIN, HIGH); // Pull the SS pin HIGH - Inactive
}
