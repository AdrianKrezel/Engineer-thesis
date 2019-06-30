/***********************************************************************************************************************************
 * main.c
 *
 * ***** NIEWIDOMY *****
 * - FLASH:			- RAM:
 * - 13 118 (40,0%)	- 1579 (77,1%)
 *
 *
 * POD£¥CZENIE:
 * 1. WYSWIETLACZ:
 *    - VSS		do GND
 *    - VCC		do VCC
 *    - VEE		do potencjometru
 *    - RS		do A0
 *    - RW		do A1
 *    - E		do A2
 *    - DB4		do A3
 *    - DB5		do A4
 *    - DB6		do A5
 *    - DB7		do A6
 *    - LED+	do VCC
 *    - LED-	do GND
 *
 * 2. KLAWISZE:
 * 	  - klawisz_NUM do PC0
 * 	  - klawisz_ENT do PC1
 * 	  - reset 		do RESET uC
 * 3. BLUETOOTH:
 * 	  - TX			do PD0 RXD
 * 	  - RX			do PD1 TXD
 * 	  - VCC			do VCC 5V
 * 	  - GND			do GND
 * 4. G£OSNIK:
 * 	  - PD4/PD5 do pierwszego pinu g³onika
 * 	  - GND     do drugiego pinu g³onika
 * 5. MODU£ KART SD:
 *
 **********************************************************************************************************************************/

//1. DODANIE BIBLIOTEK-------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>

//2. DODANIE PLIKÓW PROJEKTU-------------------------------------------------------------------------------------------------------
#include "bluetooth/bluetooth.h"
#include "LCD/LCD.h"
#include "LCD/lcd44780.h"
#include "wav_player/dzwiek.h"

//3. DEFINICJE---------------------------------------------------------------------------------------------------------------------
#define klawisz_NUM				(1<<PC0)
#define klawisz_ENT				(1<<PC1)
#define KEY_PIN					PINC
#define klawisz_ENT_pressed		( ( KEY_PIN & ( klawisz_ENT) ) )
#define klawisz_NUM_pressed		( ( KEY_PIN & ( klawisz_NUM ) ) )
#define reset_ON				PORTA &=~(1<<PA7) //wy³¹czenie zasilania
#define reset_OFF				PORTA |= (1<<PA7) //w³¹czenie zasilania


//4. DEKLARACJA ZMIENNYCH GLOBALNYCH-----------------------------------------------------------------------------------------------

uint8_t i = 0; //licznik pêtli
int cyfra_int = 0; //zmienna, której wartosc przyjmuja setki, dziesiatki i jednosci zaleznie od wyboru (enterem)
char cyfra_char = 0;

char niewidomy_char[4] = {0};//przechowuje numer wybranego autobusu w formacie char
int niewidomy_int[4] = {0}; //przechowuje numer wybranego autobusu w formacie uint8_t

uint8_t kierunek_A = 0; //zawiera 1 z 2 mo¿liwych kierunków jazdy linii komunikacji publicznej danego pojazdu
uint8_t kierunek_B = 0; //zawiera 1 z 2 mo¿liwych kierunków jazdy linii komunikacji publicznej danego pojazdu

//TIMER2
uint8_t sekundy = 0;
uint8_t licznik = 0;

//status po³¹czenia bluetooth
char disconnected[] = {"DISCONNECT"}; //wyswietli je¿eli roz³¹czono
char connected[] = {"CONNECT"}; //wyswietli je¿eli po³¹czono
char OK[]={"OK"}; //potwierdzenie przy komendzie AT_mode oraz przy ath

//komunikaty na LCD
char LCDdisconnect[16] = { "DISCONNECT      " };
char LCDconnect[16] = { "CONNECT         " };
char spacja[1] = {" "};
volatile uint8_t flaga_polaczenie = 0; //Stan po³¹czenia (neutralny)
volatile uint8_t flaga_dane = 0; //flaga danych (czy odebrano)

//void reset_BT(void);
void wprowadz_numer();
void LCD_clear(void);
//---------------------------------------------------------------------------------------------------------------------------------

int main() {
	//5.1. ustawienie uC
	// LCD + pin reset bluetooth
	DDRA = 0xFF; // 11111111
	PORTA = 0x80; // 10000000
	// SPI
	DDRB = 0xE0; // 11100000
	PORTB = 0xF0; // 11110000
	// przyciski
	DDRC = 0xFC; // 11111100
	PORTC = 0xFF; // 11111111
	// g³osnik/s³uchawki
	DDRD = 0x30; // 00110000

	PLAYER_init();	  // inicjalizacja odtwarzacza plików wave
	InitUSART(19200); // inicjalizacja USART
	lcd_init(); // inicjalizacja LCD2x16

	//sekundy2 (przerwanie generowane co 1sek) BT
	TCCR2 |=(1<<CS20)|(1<<CS21)|(1<<CS22) |(1<<WGM21);	//preskaler=1024, tryb CTC
	TIMSK |= (1<<TOIE2); //zezwolenie na przerwanie Overflow
	TCNT2 =249;

	sei();	// zezwolenie na przerwania globalne

	//5.2. start
	reset_ON; //wy³¹czenie modu³u bluetooth
	lcd_cls(); //wyczyszczenie wyswietlacza LCD
	start(); //komunikat powitalny na LCD
	komunikat_glosowy(10); //g³osowy komunikat powitalny
	lcd_cls();
	lcd_locate(0,0);
	lcd_str("NIEWIDOMY");

	//5.3. wprowadzenie numeru i kierunku linii
	komunikat_glosowy(11);
	wprowadz_numer();

	reset_OFF; //w³¹czenie modu³u bluetooth
	char NIEWIDOMY[5]={0};
	for (i=0; i<4; i++){
		NIEWIDOMY[i] = niewidomy_char[i];
	}
	lcd_locate(0,10);
	lcd_str(niewidomy_char); //wyswietlenie wybranego numeru
	for (i = 13; i < 16; i++) { //obszarowe czyszczenie ekranu lcd
		lcd_locate(0, i);
		lcd_str(spacja);
	}

	//5.4. pêtla g³ówna programu: sprawdzenie statusu po³¹czenia i ewentualny odbiór danych wysy³anych z przez urz¹dzenie zainstalowane w autobusie
	while (1) {
		//5.4.1. wyczyszczenie wyswietlacza
		LCD_clear();
		//5.4.2. odbiór danych
		//odbiór danych przez USART i zapis do bufora

		if (flaga_polaczenie==2){
			sekundy=0;
			TransmitUSART('X');	//wys³anie znaku ¿¹dania transmisji
		}

		int bufsize = 1;
		char *bufor = calloc(bufsize, sizeof(char)); //alokacja pamiêci
		bufsize = BT_receive(&bufor); //zapisanie danych do bufora


		//5.4.3. analiza odebranych danych
		char *wynik = strstr(bufor, connected);
		if (wynik!=NULL){ //CONNECT: TAK
			lcd_locate(1,0); lcd_str(LCDconnect);
			flaga_polaczenie=2; //"CONNECT"

		}
		else{ //CONNECT: NIE
			if (flaga_polaczenie==2){
				char *wynik = strstr(bufor, NIEWIDOMY);
				if (wynik!=NULL){ //NUMER: TAK

					lcd_locate(1, 0); lcd_str(LCDdisconnect);
					lcd_locate(1, 14); lcd_str(":)"); //podjecha³ wybrany przez Ciebie pojazd komunikacji miejskej
					flaga_dane=1; //"DATA RECEIVED"

					komunikat_glosowy(16);
					for (i = 0; i < 3; i++){
						komunikat_glosowy(niewidomy_int[i]);
					}
					komunikat_glosowy(cyfra_int);
					komunikat_glosowy(17);
					return 0; //KONIEC PROGRAMU
				}
				else{ //NUMER: NIE
					flaga_dane=1;//"DATA RECEIVED"
					lcd_locate(1,0); lcd_str(LCDdisconnect);
					lcd_locate(1, 14); lcd_str(":("); //jedzie inny pojazd komunikakcji miejskiej
					//reset_BT();
					flaga_polaczenie=0; //"DATA RECEIVED"
					reset_ON;
					komunikat_glosowy(21);
					reset_OFF;
					flaga_polaczenie=1;
				}
			}
		}
		_delay_ms(10); free(bufor);
		_delay_ms(500);
	}
}
// funkcja przerwania odpowiedzialna za cykliczne zrywanie po³¹czenia BT----------------------------------
ISR(TIMER2_OVF_vect){	//przerwanie co 1sek
	licznik++;
	if (licznik>125){ 	//minê³a 1sek
		licznik=0;		//wyzerowanie licznika
		sekundy++;		//zwiêkszenie licznika sekund

//		if (sekundy==255) sekundy=0; //reset licznika sekund (aby siê nie przepe³ni³ gdy doliczy do 255

		if (flaga_polaczenie==2 && sekundy==3 && sekundy!=0){ //reset BT co 3sek
			reset_ON;
		}
		if (flaga_polaczenie==2 && sekundy==4 && sekundy!=0){
			reset_OFF;
			lcd_locate(1,0); lcd_str(LCDdisconnect);
			if (flaga_dane==0){ //je¿eli nie odebrano danych, to znaczy, ¿e to nie autobus/tramwaj
				lcd_locate(1,14); lcd_str("XX");
			}
			flaga_polaczenie=1; //"DISCONNECT"
			flaga_dane=0;
		}

		if (sekundy>9) sekundy=0; //wyzerowanie licznika sekund

	}
	TCNT2=131;
}

//funkcja zrywaj¹ca po³¹czenie BT----------------------------------------------------------------------
//void reset_BT(void){
//	reset_ON;
//	_delay_ms(100);
//	reset_OFF;
//}

//funkcja czyszcz¹ca obszarowo wyswietlacz-------------------------------------------------------------
void LCD_clear(void){
	for (uint8_t i = 0; i < 16; i++) {
		lcd_locate(1,i); lcd_str(spacja); //dolny wiersz
		if (i >= 14)
			lcd_locate(0,i); lcd_str(spacja); //górny wiersz
	}
}

// funkcja wprowadzaj¹ca numer i kierunek--------------------------------------------------------------
void wprowadz_numer() {
	//numer linii
	for (i = 0; i < 3; i++) {
		_delay_ms(200);
		cyfra_int = 0; //zerowanie cyfry
		wyswietl_numer(cyfra_int);
		if (i == 0) {
			komunikat_glosowy(12);
		} else if (i == 1) {
			komunikat_glosowy(13);
		} else if (i == 2) {
			komunikat_glosowy(14);
		}

		komunikat_glosowy(cyfra_int);
		while (klawisz_ENT_pressed) { //dopóki klawisz LED_enter nie jest wcisniety
			if (!(KEY_PIN & (klawisz_NUM))) {
				_delay_ms(80);
				if (!(KEY_PIN & (klawisz_NUM))) {
					cyfra_int++;
					_delay_ms(200);
					if (cyfra_int > 9)
						cyfra_int = 0; // je¿eli wybrana cyfra >9 to j¹ wyzeruje
					wyswietl_numer(cyfra_int);
					komunikat_glosowy(cyfra_int);
				}
			}
		}
		//zapis numeru do tablicy w formacie uint8_t
		niewidomy_int[i] = cyfra_int; //zapisanie inta do tablicy niewidomego
		cyfra_char = cyfra_int+48;
		niewidomy_char[i] = cyfra_char; //zapisanie stringa do tablicy niewidomego
		komunikat_glosowy(18);
	}

	//wybor kierunku
	komunikat_glosowy(19);
	if (niewidomy_int[0] == 0 && niewidomy_int[1] == 0 && niewidomy_int[2] == 4) {        //4
		kierunek_A = 50; kierunek_B = 51; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 0 && niewidomy_int[1] == 1 && niewidomy_int[2] == 1) { //11
		kierunek_A = 40; kierunek_B = 46; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 0 && niewidomy_int[1] == 2 && niewidomy_int[2] == 0) { //20
		kierunek_A = 35; kierunek_B = 36; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 0 && niewidomy_int[1] == 2 && niewidomy_int[2] == 5) { //25
		kierunek_A = 48; kierunek_B = 49; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 0 && niewidomy_int[1] == 2 && niewidomy_int[2] == 8) { //28
		kierunek_A = 32;kierunek_B = 47; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 1 && niewidomy_int[2] == 2) { //112
		kierunek_A = 52; kierunek_B = 53; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 2 && niewidomy_int[2] == 2) { //122
		kierunek_A = 32; kierunek_B = 33; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 3 && niewidomy_int[2] == 3) { //133
		kierunek_A = 61; kierunek_B = 62; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 5 && niewidomy_int[2] == 4) { //154
		kierunek_A = 30; kierunek_B = 34; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 5 && niewidomy_int[2] == 8) { //158
		kierunek_A = 42; kierunek_B = 43;cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 7 && niewidomy_int[2] == 2) { //172
		kierunek_A = 41; kierunek_B = 37; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 8 && niewidomy_int[2] == 4) { //184
		kierunek_A = 40; kierunek_B = 38; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 8 && niewidomy_int[2] == 6) { //186
		kierunek_A = 38; kierunek_B = 39; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 1 && niewidomy_int[1] == 8 && niewidomy_int[2] == 9) { //189
		kierunek_A = 32; kierunek_B = 37; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 3 && niewidomy_int[1] == 0 && niewidomy_int[2] == 3) { //303
		kierunek_A = 56; kierunek_B = 57; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 3 && niewidomy_int[1] == 2 && niewidomy_int[2] == 0) { //320
		kierunek_A = 54; kierunek_B = 55; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 5 && niewidomy_int[1] == 0 && niewidomy_int[2] == 3) { //503
		kierunek_A = 44; kierunek_B = 45; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 5 && niewidomy_int[1] == 2 && niewidomy_int[2] == 3) { //523
		kierunek_A = 30; kierunek_B = 31; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 7 && niewidomy_int[1] == 1 && niewidomy_int[2] == 4) { //714
		kierunek_A = 32; kierunek_B = 60; cyfra_int = kierunek_A;
	} else if (niewidomy_int[0] == 7 && niewidomy_int[1] == 2 && niewidomy_int[2] == 0) { //720
		kierunek_A = 58; kierunek_B = 59; cyfra_int = kierunek_A;
	} else {
		lcd_locate(1,0); lcd_str("Zly numer");
		_delay_ms(2000);
		lcd_locate(1,0); lcd_str("Nac. Reset");
		komunikat_glosowy(22);
		return;//KONIEC PRGORAMU
	}

	wyswietl_numer(cyfra_int);
	komunikat_glosowy(cyfra_int);
	while (klawisz_ENT_pressed) {
		if (!(KEY_PIN & (klawisz_NUM))) {
			_delay_ms(80);
			if (!(KEY_PIN & (klawisz_NUM))) {
				cyfra_int = kierunek_B;
				wyswietl_numer(cyfra_int);
				komunikat_glosowy(cyfra_int);
				_delay_ms(200);
			}
		}
		if (!(KEY_PIN & (klawisz_NUM))) {
			_delay_ms(80);
			if (!(KEY_PIN & (klawisz_NUM))) {
				cyfra_int = kierunek_A;
				wyswietl_numer(cyfra_int);
				komunikat_glosowy(cyfra_int);
				_delay_ms(200);
			}
		}
	}
	komunikat_glosowy(18);
	niewidomy_int[3] = cyfra_int;
	cyfra_char = cyfra_int+48;
	niewidomy_char[3] = cyfra_char;
	komunikat_glosowy(15);
	for (i = 0; i < 3; i++) {
		komunikat_glosowy(niewidomy_int[i]);
	}
	komunikat_glosowy(cyfra_int);
	komunikat_glosowy(20);
}
//--------------------------------------------------------------------------------------------------------------------------------

