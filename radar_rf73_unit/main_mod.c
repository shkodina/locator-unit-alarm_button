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

void SetupTIMER1 (void)
{
     TCCR1B = (1<<CS12);
     TCNT1 = 65536-50;        
     TIMSK |= (1<<TOIE1); // �������� ���������� �� �������
}


ISR (TIMER1_OVF_vect)
{
	if (PIND == 0){
		if (state == ON){ // gotosleep
			DOWNBIT(PORTC, 0);
			state = OFF;
		}else{ // wake up
			UPBIT(PORTC, 0);
			state = ON;
		}
		_delay_ms(2000);
	}
	// run timer
	TCNT1 = 65536 - 500; //  31220;
    TCCR1B = (1<<CS12);
    TIMSK |= (1<<TOIE1);


}



int main ()
{
	// start delay
	
	_delay_ms(DEVICE_START_DELAY_MS);

	Init_Spi();

	DDRC |= 0x01;
	PORTC |= 0x01;

	RFM73_Initialize();

	RFM73_SetPower(MODULE_POWER);
	SwitchToRxMode();

	DDRD = 0b00000000;
	PORTD = 0b00000100;

	SetupTIMER1();
	sei();

	sleep_enable();

	
	while (1)
	{
		if (state == OFF){
			DOWNBIT(PORTC, 0);
			sleep_cpu();
		} else {
			SwitchToTxMode();
			for(char i=0;i<POWERCOUNT;i++)
			{
				SwitchToTxMode();
				Send_Packet(W_TX_PAYLOAD_NOACK_CMD,tx_buf,4);

				SwitchToRxMode();  //switch to Rx mode

				_delay_ms(150);
				INVBIT(PORTC, 0);
			}
		}
	}

	return 0;

}

