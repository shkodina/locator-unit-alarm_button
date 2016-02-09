#include <avr/io.h>
#include <avr/delay.h>
#include "rf73_spi.h"

#include "settings.h"

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

UINT8 rx_buf[MAX_PACKET_LEN];

#define COLORDDRPORT 	DDRA
#define COLORPORT 		PORTA
#define COLORPIN 		1

int main ()
{
	UPBIT(COLORDDRPORT, COLORPIN);
	DOWNBIT(COLORPORT, COLORPIN);
	
	Init_Spi();
	RFM73_Initialize();
	SwitchToRxMode();

	_delay_ms(2000);

	while (1)
	{
		_delay_ms(10);
		UINT8 len = Receive_Packet(rx_buf, MAX_PACKET_LEN);

		if (len > 0){
			// if (rx_buf[2] == 0xff)
			UPBIT(COLORPORT, COLORPIN);
		} else {
			DOWNBIT(COLORPORT, COLORPIN);
		}
	}

	return 0;

}


