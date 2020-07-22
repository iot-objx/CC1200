#include"CC1200.h"				// TI CC1200 RF Radio

#define MODE // Define this for TX, otherwise code is RX

// Node Addresses
#ifdef MODE
#define THIS_NODE 0x01
#define TARG_NODE 0x02
#else
#define THIS_NODE 0x02
#define TARG_NODE 0x02
#endif

// CC1200 Radio Interrupt Pin
#define RadioTXRXpin	0x02	// CC1200 Packet Semaphore 

// BUFFER
#define FIFO_SIZE		64		// TX and RX Buffer Size
byte readBytes; // Number of bytes read from RX FIFO
byte rxBuffer[FIFO_SIZE]; // Fill with RX FIFO
byte writeBytes; // Number of bytes written to TX FIFO
byte txBuffer[FIFO_SIZE]; // Fill with TX FIFO

#define idxLength		0		// Length index
#define idxTargetNode	1		// TargetNode index
#define idxFrameCount	3		// FrameCount index
#define TIMEOUT	        10000		// TIMEOUT

union DoubleBytes
{
	double DoubleVal;
	byte AsBytes[sizeof(double)]; // 4 Bytes
} DoubleBytes;

String str = "Hello Eylül!";
char arr[15];


// GLOBAL VARIABLES
byte counter = 0x00;
volatile bool packetSemaphore; // RX/TX success flag. Volatile prevents undesired optimizations by the compiler

// Set Packet Semaphore 
void setSemaphore() {
	packetSemaphore = true;
}

// Clear Packet Semaphore 
void clearSemaphore() {
	packetSemaphore = false;
}

// Try receiving incoming, if any, within the TOUT duration.
bool TryReceive(unsigned long qTimeout) {
	bool receiveStatus = false;
	byte marcstate = 0x00;
	Serial.println("Wait Reception...");

	do
	{
		marcstate = cc1200.GetStat(StatType::MARC_STATE, 0x1F); // MARC_STATE[4:0]
		if (marcstate == MARC_STATE_RX_FIFO_ERR)
		{
			cc1200.FlushRxFifo(); delay(1);
			cc1200.Receive(); delay(3);
			//Serial.println("\tTry RX.");
			Serial.println("\tRXFIFO Flushed!");
		}
		else if (marcstate == MARC_STATE_TX_FIFO_ERR)
		{
			cc1200.FlushTxFifo(); delay(1);
			Serial.println("\tTXFIFO Flushed!");
		}
		else if (marcstate >= MARC_STATE_XOFF && marcstate <= MARC_STATE_ENDCAL)
		{
			Serial.print("\tSettling: "); Serial.println(marcstate);
		}
		else if (marcstate != MARC_STATE_RX)
		{
			//Serial.println("\tTry RX.");
			cc1200.Receive(); delay(7);
		}

		// Timeout Implementation
		if (millis() >= qTimeout) //isTimeOut(qTimeout)
		{
			Serial.println("\tTOUT!");
			cc1200.Idle();
			break;
		}

		delay(10);
	} while (!packetSemaphore);

	// Packet received i.e. no timeout occurred
	if (packetSemaphore)
	{
		readBytes = cc1200.ReadRxFifo(rxBuffer);

		if (readBytes >= 0) // Fail-Safe. 2 bytes header (Len-Adrs) + 2 appended bytes (CRC-RSSI) footer
		{
			receiveStatus = true;
		}

		cc1200.Idle(); delay(1);
		cc1200.FlushRxFifo();
		clearSemaphore();
	}

	return receiveStatus;
}

// Try transmitting outgoing, if any, within the TOUT duration.
bool TryTransmit(unsigned long qTimeout) {
	bool transmitStatus = false;
	byte marcstate = 0x00;
	Serial.println("Wait Transmission...");

	do
	{
		marcstate = cc1200.GetStat(StatType::MARC_STATE, 0x1F); // MARC_STATE[4:0]
		if (marcstate == MARC_STATE_IDLE)
		{
			cc1200.FlushTxFifo(); delay(1); // Flush ony in ERR or IDLE
			cc1200.WriteTxFifo(txBuffer, txBuffer[idxLength]); delay(1);
			cc1200.Transmit(); delay(3);
			Serial.println("\tTX");
		}
		else if (marcstate == MARC_STATE_TX_FIFO_ERR)
		{
			cc1200.FlushTxFifo(); delay(1);
			Serial.println("\tTXFIFO Flushed!");
		}
		else if (marcstate == MARC_STATE_RX_FIFO_ERR)
		{
			cc1200.FlushRxFifo(); delay(1);
			Serial.println("\tRXFIFO Flushed!");
		}
		else if (marcstate >= MARC_STATE_XOFF && marcstate <= MARC_STATE_ENDCAL)
		{
			Serial.print("\tSettling: "); Serial.println(marcstate);
		}

		// Timeout Implementation
		if (millis() >= qTimeout) //isTimeOut(qTimeout)
		{
			Serial.println("\tTOUT!");
			cc1200.Idle();
			break;
		}

		delay(10);
	} while (!packetSemaphore);

	if (packetSemaphore)
	{
		cc1200.Idle(); delay(1); // Flush only in IDLE or FIFOERR
		cc1200.FlushTxFifo();
		transmitStatus = true;
		clearSemaphore();
	}

	return transmitStatus;
}

void setup(){
    // SERIAL
	Serial.begin(115200);
	Serial.println("\n>>Start Setup Chain");

	// CC1200 RADIO
	cc1200.Init(SS, MOSI, MISO, SCK, PIN_UNUSED); // SS, MOSI, MISO, SCK, RadioResetpin
	cc1200.Configure(preferredSettings, prefSettLen); // 2sec internal delay
	cc1200.SetAddress(THIS_NODE); delay(10);
	cc1200.Strobe(CC120X_SCAL); delay(1000);
	byte readNode = cc1200.GetAddress(false);
	Serial.print("\tNode: "); Serial.println(readNode);
	if (THIS_NODE == readNode)
	{
        Serial.print("\tNode "); Serial.print(THIS_NODE); Serial.println(" Ok!");
	}
    else{
        Serial.print("\tERROR \n");
        while (true);
    }
	cc1200.FlushRxFifo(); delay(100);
	cc1200.FlushTxFifo(); delay(100);
	Serial.println("\tRadio Config");

	// RADIO INTERRUPT
	packetSemaphore = false;
	pinMode(RadioTXRXpin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(RadioTXRXpin), setSemaphore, FALLING); // LOW, CHANGE, RISING, FALLING
	Serial.println("\tPacket Interrupt Config");

    Serial.flush();

    writeBytes = 4 + 15; // idxFrameCount+1
    txBuffer[idxLength] = writeBytes;
    txBuffer[idxTargetNode] = TARG_NODE;
    txBuffer[idxFrameCount] = counter;
    str.toCharArray(arr, 15);
}

void loop(){
#ifdef MODE
DoubleBytes.DoubleVal = 88.0 + counter;
// txBuffer[4] =DoubleBytes.AsBytes[0];
// txBuffer[5] =DoubleBytes.AsBytes[1];
// txBuffer[6] =DoubleBytes.AsBytes[2];
// txBuffer[7] =DoubleBytes.AsBytes[3];
for (int i = 0; i < 15; i++)
{
    txBuffer[4+i] = arr[i];
}

txBuffer[idxFrameCount] = counter;
if (TryTransmit(TIMEOUT + millis()))
{
    counter++;
    Serial.print(F("TX: "));
    for (int i = 0; i < writeBytes; i++)
    {
        Serial.print(txBuffer[i]); Serial.print(F(", "));
    }
    Serial.println(F(""));
}

delay(5000);
#else
if (TryReceive(TIMEOUT + millis()))
{
    Serial.print(F("RX: "));
    for (int i = 0; i < readBytes; i++)
    {
        Serial.print(rxBuffer[i]); Serial.print(F(", "));
    }
    Serial.println(F(""));
    // DoubleBytes.AsBytes[0] = rxBuffer[4];
    // DoubleBytes.AsBytes[1] = rxBuffer[5];
    // DoubleBytes.AsBytes[2] = rxBuffer[6];
    // DoubleBytes.AsBytes[3] = rxBuffer[7];
    // Serial.print(F("Double: ")); Serial.println(DoubleBytes.DoubleVal);
    Serial.print(F("\tText: "));
    for (int i = 0; i < 15; i++)
    {
        Serial.print(char(rxBuffer[4+i]));
    }
    Serial.print(F("\n"));
}
#endif
}