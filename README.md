# CC1200
The CC1200 chipset works in the ISM (Industrial, Scientific, and Medical) and SRD (Short Range Device) frequency bands i.e. 169, 433, 868, 915, and 920 MHz, making it useful in diverse fields of applications. This is a relatively low-level library to interface the Texas Instrument's low-power high-performance CC1200 RF transceiver with Arduino microcontrollers. The motivation was to depict performance rather than simplicity. Often simplicity in coding means lots of wrappers and code fatigue. 

The library was developed for the CC1200-based Evaluation Module Kit 868-930 MHz showcased on the TI [website](https://www.ti.com/tool/CC1200EMK-868-930#3). Here are some of the pictorial depiction of the sub-1 GHz radio module taken from the manufacturer's site. 

![CC1200EMK Angled](/Documentation/cc1200emk-868-930-angled.jpg)

![CC1200EMK Top](/Documentation/cc1200emk-868-930-top.jpg)

![CC1200EMK Bottom](/Documentation/cc1200emk-868-930-bottom.jpg)

![CC1200EMK Box](/Documentation/cc1200emk-868-930-box.jpg)
***
## Connection Diagram
The CC1200 module utilizes SPI interface to talk to its mother MCU. Although, the connection diagram is pretty straightforward, it is summarized on the table below. Referring to the module sketch below. The prefix ***D*** indicates the pins being digital as opposed to analog ones.

![CC1200EMK Sketch](/Documentation/CC1200-sketch.png)

Note that the illustration above is just for reference and ease-of-use explanation. Furthermore, the CC1200 is a **3.3V** logic level device and hence a supply voltage of such. During development, it was observed that some of the Arduino boards, namely Atmega328P ones, are unable to supply enough current thus leading to unexpected behavior. Therefore, the reset pin was connected to **5V** despite the board being a **3.3V** logic device. My extensive experimentation had not brought up any issues in the duration of two long years. :sweat_smile: 

CC1200  | Arduino Mega
--------|-------------
GND     |	GND		
GPIO0	|	*N/A*	
GPIO2	|	D2 		
CSN	    |	SS	 (D53)
SCLK	|	SCK	 (D52)
MOSI	|	MOSI (D51)
MISO	|	MISO (D50)
VDD	    |	3V3	
RST*	|	5V	

*On the actual module, there is no reset (RST) pin broken out. Here, the reset pin was populated from the female headers underneath. See below. 

![CC1200EMK Reset](/Documentation/cc1200-back-reset.jpg)

***
## Library Functions/Methods
The library functions available to the user are:

* **`Init(SS_PIN, MOSI_PIN, MISO_PIN, SCK_PIN, RESET_PIN)`**: The CC1200 object is initialized using this method. Two form of initialization exists. When no input arguments are provided, the standard SPI pins are used by default *[SS, MOSI, MISO, SCK, PIN_UNUSED]*. The advanced option is when the user supplies the individual SPI pins. 

* **`Configure(settings, len)`**: is used to configure the behavior of the CC1200 module. For convenience, two different settings are provided and advanced users should use that as the starting point for tuning the module as per their needs. The settings along with the array size are stored on the *CC120X_Settings.h* header file: ***rxSniffSettings*** and ***preferredSettings***. One needs extensive understanding of the datasheet/user manual to write their own version of settings. See the header file and the provided documents. 

* **`GetStat(sType, keepBits, shiftLR)`**: is used to get both the ***Stat***e and the ***Stat***us of the radio. The type being investigated is selected using `sType` which can be one of
    * `STATUS`:
	* `MARC_STATE`: Operational states (as in state machine) of the CC1200 module.  
	* `MARC_STATUS0`: MARC status register 0.
	* `MARC_STATUS1`: MARC status register 1.
	* `MODEM_STATUS0`: Modem status register 0. Contains FIFO status.
	* `MODEM_STATUS1`: Modem status register 1. Contains FIFO status.

MARC is the abreviation of Main Radio Control unit. Refer to datasheet for detailed understanding. The `keepBits` and `shiftLR` parameters are used to condition the returned result; `keepBits` bitwise AND-ed with Right(+)/Left(-) shifted `shiftLR` times. By default, the returned "stat" is as-it-is and unshifted. 

* **`Strobe(command)`**: Command Strobes may be viewed as single byte instructions to CC120X. By addressing a command
strobe register, internal sequences will be started. These commands are used to enable receive and transmit mode, enter SLEEP mode, disable the crystal oscillator, etc. The command strobes are listed as
    * `CC120X_SRES`: Reset the chip.
    * `CC120X_SFSTXON`: Enable and calibrate the frequency synthesizer.
    * `CC120X_SXOFF`: Turn off the crystal oscillator.
    * `CC120X_SCAL`: Calibrate the frequency synthesizer and turn it off. Helps fast switching to/from RX/TX modes. 
    * `CC120X_SRX`: Enable RX. Perform calibration if enabled.
    * `CC120X_STX`: Enable TX. If in RX state, only enable TX if CCA passes. 
    * `CC120X_SIDLE`: Exit RX/TX and turn off the frequency synthesizer. 
    * `CC120X_SAFC`: Automatic Frequency Correction. 
    * `CC120X_SWOR`: Start automatic RX polling sequence (Wake-on-Radio).
    * `CC120X_SPWD`: Enter power down mode when CSn goes high.
    * `CC120X_SFRX`: Flush the RX FIFO buffer.
    * `CC120X_SFTX`: Flush the TX FIFO buffer.
    * `CC120X_SWORRST`: Reset real time clock.
    * `CC120X_SNOP`: No operation. Returns status byte. 


* **`Reset(HWreset)`**: Reset the Chip. Performs Hard Reset if HWreset is TRUE else Soft Reset.

* **`Idle()`**: Idle the Chip.

* **`PowerDown()`**: Power Down the Chip.

* **`Transmit()`**: Transmit data available in the TX FIFO.

* **`Receive()`**: Ready to receive incoming data into the RX FIFO.

* **`FlushTxFifo()`**: Flush TX FIFO explicitly. (Call ONLY when in IDLE or TX FIFO error state).

* **`FlushRxFifo()`**: Flush RX FIFO explicitly. (Call ONLY when in IDLE or RX FIFO error state).

* **`ResolveFifoErr()`**: Resolve FIFO Error (if any) - returns 0 (No Error), (1 TXFIFO Error) OR (2 RXFIFO Error).

* **`GetAddress(fast)`**: Get Device's Address/ID. fast = FALSE explicitly reads from the Chip and is slower as opposed to software read.

* **`SetAddress(address)`**: Set Device's Address/ID. Useful to differenciate CC1200 enabled devices e.g. Wireless Sensor Network (WSN). 256 different addressing is possible. The default broadcasting addresses are 
    * `BROADCAST_ADDRESS000`: Broadcast address 0
    * `BROADCAST_ADDRESS255`: Broadcast address 255

    The broadcast addresses can be either of them or both. This depends on the settings used. 

* **`ReadRegister(address, readBuffer, len)`**: Read `len` bytes from the specified `address` register and store it in the `readBuffer` array. Make sure that the buffer can hold the intended amount of data. 

* **`WriteRegister(address, writeBuffer, len)`**: Write `len` bytes to the specified `address` register and from the `writeBuffer` array. Make sure that the target address can hold the buffer data. 

* **`UpdateRegister(address, updateBits)`**: Update Register according to updateBits. (Different from WriteRegister) Instead of overwriting, it performs bitwise operations and updates those bits only. 

* **`ReadRxFifo(readBuffer)`**: Read from RX FIFO and return amount of bytes read. The read data is held on the `readBuffer` array. 

* **`WriteTxFifo(writeBuffer, len)`**: Write to TX FIFO. Assumption: [Length Address --Payload-- +1Byte] where Length = AddressLen(1) + PayloadLength. 

Here, the RX/TX format is assumed to be of the following format

![CC1200EMK Sketch](/Documentation/PacketFormat.PNG)

The length is calculated using the address byte and the data/payload bytes. The two byte Cyclic Redundancy Check (CRC) is calculated using the length, address and the payload bytes. On the figure above, the command byte is incurred into the payload. It can be used to carry out user-specifc tasks on the MCU side. 

***

## Notes

The key motivation of developing this library was to keep the low-level functionalities intact therefore making the library suitable for time sensitive applications. In fact, this library IS the by-product of a project namely Time Synchronization in Wireless Sensor Networks (WSNs). 

Needless to say, proper utilization of the library might require solid understanding of the datasheet. Lastly, a simple yet useful example is provided with the library to showcase its feature amongst other things. 