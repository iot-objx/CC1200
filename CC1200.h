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

#ifndef _CC1200_h
#define _CC1200_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// Libraries
#include "CC120X_Settings.h"
#include "CC120X_Misc.h"

// Defines
#define PIN_UNUSED				-1		// Unused Pin defination
#define BROADCAST_ADDRESS000	0x00	// Broadcast addresse 0
#define BROADCAST_ADDRESS255	0xFF	// Broadcast addresse 255

// State/Status Types
enum StatType
{
	STATUS = 0x0000,
	MARC_STATE = CC120X_MARCSTATE,
	MARC_STATUS0 = CC120X_MARC_STATUS0,
	MARC_STATUS1 = CC120X_MARC_STATUS1,
	MODEM_STATUS0 = CC120X_MODEM_STATUS0,
	MODEM_STATUS1 = CC120X_MODEM_STATUS1
};

// Macros
#define higherByte(w) ((uint8_t) ((w) >> 16))		// Higher Byte (2nd highest)
#define highestByte(w) ((uint8_t) ((w) >> 24))		// Highest Byte
#define wait_spi()  while(!(SPSR & _BV(SPIF)))		// Wait until SPI operation is terminated
#define wait_pin_low(pin) while(digitalRead(pin))	// Wait until pin goes LOW
#define wait_pin_high(pin) while(!digitalRead(pin))	// Wait until pin goes HIGH

class CC1200
{
public:
	void Init(void);
	void Init(uint8_t SS_PIN, uint8_t MOSI_PIN, uint8_t MISO_PIN, uint8_t SCK_PIN, int8_t RESET_PIN);
	void Configure(const registerSetting_t settings[], uint8_t len);
	byte GetStat(StatType sType, byte keepBits = 0xFF, int8_t shiftLR = 0);
	void Strobe(uint8_t command);
	void Reset(bool HWreset = true);
	void Idle(void);
	void PowerDown(void);
	void Transmit(void);
	void Receive(void);	
	void FlushTxFifo(void);
	void FlushRxFifo(void);	
	int ResolveFifoErr(void);
	uint8_t GetAddress(bool fast = true);
	void SetAddress(uint8_t address);
	bool ReadRegister(uint16_t address, byte readBuffer[], uint8_t len);
	bool WriteRegister(uint16_t address, byte writeBuffer[], uint8_t len);
	void UpdateRegister(uint16_t address, byte updateBits);
	uint8_t ReadRxFifo(byte readBuffer[]);
	void WriteTxFifo(byte writeBuffer[], uint8_t len);	

private:	
	uint8_t _RESET_PIN;
	uint8_t _SS_PIN, _MOSI_PIN, _MISO_PIN, _SCK_PIN;
	uint8_t _DEVICE_ADDRESS = BROADCAST_ADDRESS000; // Broadcast Address: 0x00 and/or 0xFF

	void _spi_begin(void);
	void _spi_end(void);
	void _spi_strobe(uint8_t command);
	uint8_t _spi_transfer(uint8_t data);
	void _spi_read_register(uint16_t address, uint8_t *buffer, uint8_t len);
	void _spi_write_register(uint16_t address, uint8_t *buffer, uint8_t len);
};

extern CC1200 cc1200; 

#endif

