
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>

void InitUSART( unsigned long int baud ){ // baud - czyli bitrate portu COM
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

unsigned char ReceiveUSART( void ){  // funkcja odbioru bajtu z RS232 (str.147)
	//dopoki bufor nie bêdzie pusty zwraca wartosc funkcji
	while ( !(UCSRA & (1<<RXC)) );
	return UDR;
}

void TransmitUSART(unsigned char data){  // funkcja wys³ania bajtu przez RS232
	//czeka na wys³anie poprzedniej danej
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = data;
}


//KAROL*********************************************************************************
int BT_receive(char **bufor, int size){

	uint8_t flag=0;

//	*------FLAG-----*
//	|	0	|	1	|
//	|NEUTRAL|LF_konc|
//	*---------------*

	while(1){
		if (ReceiveUSART()==0x0A){ //LF START
			while(1) {
				char recv = ReceiveUSART(); //zapisz odebrany znak
				if(recv == 0x0A){ //je¿eli znaleziono LF, to ustaw flagê na flag=1
					flag = 1;
				}
				size++; //zwiêksz rozmiar bufora
				*bufor = realloc(*bufor, size); //zmiana rozmiaru alokowanej pamiêci
				(*bufor)[size-1] = recv; //zapis odebranego znaku do bufora
				if(flag == 1){ //je¿eli znaleziono LF, to zwróæ rozmiar bufora
					return size;
				}
			}
		}
	}
	return 0;
}

int buforSize(char *bufor){
	int i=0;
	while(bufor[i]!=0x0D && bufor[i+1]!=0x0A){
		i++;
	}
	return i+2;
}
//END_KAROL***************************************************************************************
