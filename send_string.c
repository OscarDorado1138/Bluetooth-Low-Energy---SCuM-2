# include <stdio .h>
# include <rt_misc .h>
# include <stdlib .h>
# include " Memory_map .h"

// How to send a packet

//1.) All packet data should be stored in a continuous, sequential, word-aligned
//	section of data memory. This can be done by creating an array of chars with
//	length equal to that of the packet data in bytes.

//2.) Write the starting address of the packet data to the RFCONTROLLER_REG__TX_DATA_
//	ADDR register.

//3.) Write the length of the packet, in bytes, to the RFCONTROLLER_REG__PACK_LEN
//	register.

//4.) Set the TX_LOAD bit of the RFCONTROLLER_REG__CONTROL register, or use the
//	radio timer to send a TX_LOAD signal/trigger. This will begin copying the
//	packet data from memory.

//5.) Wait for the packet data to nish copying. This can be indicated either by the
//	TX_LOAD_DONE interrupt, or by monitoring/polling the TX_STATE bits of the
//	RFCONTROLLER_REG__STATUS register.

//6.) Set the TX_SEND bit of the RFCONTROLLER_REG__CONTROL register, or use the
//	radio timer to send a TX_SEND signal/trigger. This will begin sending the
// 	packet data.

//7.) Wait for the packet to nish sending. This can be indicated by the TX_SEND-
//	_DONE interrupt, or by monitoring/polling the TX_STATE bits of the RFCONTROLLER_
// 	REG__STATUS register.


int flag ;

int main ( void ) 
{
	char send_packet [127];
	char packet_length ;
	char i;
	flag = 0;
	// advertising packet test, no crc
	unsigned char rxPacket1[13] = { 0xAA, 0x8e, 0x89,
		0xbe, 0xd6, 0x00, 0x18, 0xff,0xff,0xff,0xff,0xff,0xff
		};
	// connection request packet test, no crc or address of either device
	unsigned char rxPacket1[40] = { 0xAA, 0x8e, 0x89,
		0xbe, 0xd6, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xab,0xcd,0xab,0xcd,0x00,0x00,0x00,0x14,
		0x14,0x20,0x07,0x06
	};
	// Compose message
	sprintf ( send_packet , " This is a test packet ");
	// Set radio controller registers
	RFCONTROLLER_REG__TX_DATA_ADDR = & send_packet [0];
	RFCONTROLLER_REG__TX_PACK_LEN = 21;
	// Set radio timer registers
	RFTIMER_REG__COMPARE0 = 500000; // Set compare unit 0 value to 1s
	RFTIMER_REG__COMPARE0_CONTROL = 0xB; // Binary value 001011
	// Enable compare unit
	// Enable interrupt
	// Enable the TX_SEND output to radio
	RFTIMER_REG__CAPTURE1_CONTROL = 0x9; // Binary value 000001001
	// Enable interrupt
	// Enable TX_SFD_DONE input
	RFTIMER_REG__CAPTURE2_CONTROL = 0x11; // Binary value 000010001
	// Enable interrupt
	// Enable TX_SEND_DONE input
	// Start sending packet with TX_LOAD
	RFCONTROLLER_REG__CONTROL = 0x01;
	// Start the timer , enable interrupts , and reset counter
	RFTIMER_REG__CONTROL = 0x7;
	// Wait until packet is finished sending and
	// the radio timer prints all values
	while ( flag == 0) {}
		printf (" done \n");
}

168
void RFTIMER_ISR()
{
	// Read the interrupt register
	unsigned int interrupt = RFTIMER_REG__INT ;
	// Respond to different interrupt sources
	if ( interrupt & 0 x00000001 ) printf (" Telling radio to send packet with TX_SEND \n
	");
	if ( interrupt & 0 x00000200 ) printf (" TX SFD DONE at 0x%x\n",
	RFTIMER_REG__CAPTURE1 );
	if ( interrupt & 0 x00000400 ) { printf (" TX SEND DONE at 0x%x\n",
	RFTIMER_REG__CAPTURE2 ); flag = 1; }
	// Clear the interrupt register
	RFTIMER_REG__INT_CLEAR = interrupt ;
}







