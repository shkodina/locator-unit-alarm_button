#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "rf73_spi.h"
#include "settings.h"

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

#define UNITNUMBER 0 // change this from 0 to 5 [0..5]
#define FRAMEBORDER '*'
#define POWERPOSITION  2
#define POWERCOUNT  4

#define OFF 0
#define ON 1
static char state = ON;


UINT8 tx_buf[4] = {	FRAMEBORDER, UNITNUMBER, 0xff, FRAMEBORDER};

UINT8 rx_buf[MAX_PACKET_LEN];

#define BUTTONDDRPORT DDRB 
#define BUTTONPORT PORTB 
#define BUTTONPIN 0 
#define BUTTONPINOUT PINB

#define EXITDDRPORT DDRC 
#define EXITPORT PORTC 
#define EXITPIN 1 

int main ()
{
	UPBIT(EXITDDRPORT, EXITPIN);
	DOWNBIT(EXITPORT, EXITPIN);
	
	DOWNBIT(BUTTONPORT, BUTTONPIN);

	Init_Spi();
	
	RFM73_Initialize();
	RFM73_SetPower(MODULE_POWER);
	SwitchToRxMode();
	
	while (1)
	{
		// wait button is pressed
		if ((BUTTONPINOUT & (1 << BUTTONPIN)) > 0){ // let start party
		
			char cigcount = 20; // 20 times for 300 ms = 6 sec
			while (cigcount--){}
				SwitchToTxMode();
				Send_Packet(W_TX_PAYLOAD_NOACK_CMD,tx_buf,4);
				SwitchToRxMode(); 

				// we will send packet every ~300 ms
				// 36kHz = beep every 27.22 us
				// in 300ms / 27.77 us = 10803
				int switcher = 10803;
				while (switcher--){
					_delay_us(27.77);
					INVBIT(EXITPORT, EXITPIN);
				}
		}
	}

	return 0;

}

