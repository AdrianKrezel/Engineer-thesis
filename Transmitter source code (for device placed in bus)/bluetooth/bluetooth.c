
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

void InitUSART( unsigned long int baud ) // baud - czyli bitrate portu COM
{
   //ustalenie stanu UBRR
   UBRRH = (unsigned char)(((F_CPU/(16UL*baud))-1)>>8);
   UBRRL = (unsigned char)((F_CPU/(16UL*baud))-1);
   // aktywny kana³ odbioru i nadawania
   UCSRB = (1<<RXEN)|(1<<TXEN);
   // 8bitów, brak parzystoœci, 2 bity stopu
   //UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
   // 8bitów, brak parzystoœci, 1 bit stopu
   UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

unsigned char ReceiveUSART( void )  // funkcja odbioru bajtu z RS232 (str.147)
{	//dopoki bufor nie bêdzie pusty zwraca wartosc funkcji
   while ( !(UCSRA & (1<<RXC)) );
   return UDR;
}

void TransmitUSART(unsigned char data)  // funkcja wys³ania bajtu przez RS232
{	//czeka na wys³anie poprzedniej danej
   while ( !( UCSRA & (1<<UDRE)) );
   UDR = data;
}

void bluetooth_transmit(char autobusik_char[4])
{
	TransmitUSART(0x0D); _delay_ms(40); //CR
	TransmitUSART(0x0A); _delay_ms(40); //LF
	for (uint8_t i=0; i<4; i++) { _delay_ms(40); //numer i kierunek pojazdu
		TransmitUSART(autobusik_char[i]);
	}
	TransmitUSART(0x0D); _delay_ms(40); //CR
	TransmitUSART(0x0A); //LF
}
