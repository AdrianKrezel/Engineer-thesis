/*                  e-gadget.header
 * AUTOBUS_main.c
 * - 3244 bytes
 * - 2924 bytes
 *    Modyfied: 2015-03-21 14:36:53
 *      Author: Author
 *
 *          MCU: ATmega32
 *        F_CPU: 16 000 000 Hz
 *
 */

//1. DODANIE BIBLIOTEK-----------------------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>


//2. DODANIE PLIKÓW PROJEKTU-----------------------------------------------------------------------
#include "bluetooth/bluetooth.h"
#include "LCD/LCD.h"
#include "LCD/lcd44780.h"

//3. DEFINICJE-------------------------------------------------------------------------------------
//3.1.klawisze
#define klawisz_NUM				(1<<PC0)
#define klawisz_ENT				(1<<PC1)
#define klawisz_A				(1<<PC2)
#define klawisz_B				(1<<PC3)
#define KEY_PIN					PINC
#define klawisz_ENT_pressed		( ( KEY_PIN & ( klawisz_ENT) ) )
#define klawisz_NUM_pressed		( ( KEY_PIN & ( klawisz_NUM ) ) )
#define klawisz_A_pressed		( ( KEY_PIN & ( klawisz_A ) ) )
#define klawisz_B_pressed		( ( KEY_PIN & ( klawisz_B ) ) )

//4. U¯YWANE FUNKCJE I ZMIENNE---------------------------------------------------------------------
//4.1. Funkcje z pliku main.c

//4.2. Zmienne globalne
uint8_t i = 0;						// licznik pêtli
int cyfra_int = 0;		     		// zmienna, której wartosc przyjmuja setki, dziesiatki i jednosci zaleznie od wyboru (enterem)
int cyfra_char =0;
int  kierunek_A = 0;				// kierunek A
int kierunek_B = 0;					// kierunek B
char autobusik_char[4] = {0};
int autobusik_int[4] = {0};
char start_transmit=0;				// bufor znaku pocz¹tku transmisji
char connection_status=0;
//-------------------------------------------------------------------------------------------------

void wprowadz_numer();

//*****************************************************************************************************************************************
//5. MAIN**********************************************************************************************************************************
int main()
{
	//5.1. ustawienie uC
	DDRA = 0xFF;
	DDRC = 0xF3;
	PORTA= 0x00;
	PORTC= 0x03;

	InitUSART(19200);
	lcd_init();

	//5.2. wprowadzenie numeru
	start();
	lcd_cls();
	lcd_locate(0,0);
	lcd_str("AUTOBUS");
	wprowadz_numer();

	//5.3 przekonwertowanie numeru na char
	lcd_locate(0,13); lcd_str(autobusik_char);//wyswietlenie numeru
	//5.4. wys³anie numeru linii
	while(1){
		start_transmit = ReceiveUSART(); //odbierz i zapisz znak
		if (start_transmit == 'X'){ //JE¯ELI TO ZNAK POCZ¥TKU TRANSMISJI, TO WYSLIJ NUMER POJAZDU
			_delay_ms(100);
			bluetooth_transmit(autobusik_char);//wyslanie numeru i kierunku
		}
	}
}
//*****************************************************************************************************************************************
//*****************************************************************************************************************************************

//5.2. WPROWADZENIE NUMERU-----------------------------------------------------------------------------------------------------------------
void wprowadz_numer(){	//numer linii
	for ( i=0; i<3; i++){
		_delay_ms(200);
		cyfra_int = 0; //zerowanie cyfry
		while ( klawisz_ENT_pressed ){ 	//dopóki klawisz LED_enter nie jest wcisniety
			wyswietl_numer (cyfra_int);
			if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
				_delay_ms(80);
				if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
					cyfra_int++;
					_delay_ms(200);
					if (cyfra_int>9) cyfra_int=0;
				}
			}
		}

		autobusik_int[i] = cyfra_int; //zapisanie inta do tablicy autobusik
		cyfra_char = cyfra_int+48;
		autobusik_char[i] = cyfra_char; //zapisanie stringa do tablicy autobusik
		//		else break;
	}

	//kierunek A/B
	if (autobusik_int[0] == 0 && autobusik_int[1] == 0 && autobusik_int[2] == 4) {        //4
		kierunek_A = 50; kierunek_B = 51; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 0 && autobusik_int[1] == 1 && autobusik_int[2] == 1) { //11
		kierunek_A = 40; kierunek_B = 46; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 0 && autobusik_int[1] == 2 && autobusik_int[2] == 0) { //20
		kierunek_A = 35; kierunek_B = 36; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 0 && autobusik_int[1] == 2 && autobusik_int[2] == 5) { //25
		kierunek_A = 48; kierunek_B = 49; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 0 && autobusik_int[1] == 2 && autobusik_int[2] == 8) { //28
		kierunek_A = 32;kierunek_B = 47; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 1 && autobusik_int[2] == 2) { //112
		kierunek_A = 52; kierunek_B = 53; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 2 && autobusik_int[2] == 2) { //122
		kierunek_A = 32; kierunek_B = 33; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 3 && autobusik_int[2] == 3) { //133
		kierunek_A = 61; kierunek_B = 62; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 5 && autobusik_int[2] == 4) { //154
		kierunek_A = 30; kierunek_B = 34; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 5 && autobusik_int[2] == 8) { //158
		kierunek_A = 42; kierunek_B = 43;cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 7 && autobusik_int[2] == 2) { //172
		kierunek_A = 41; kierunek_B = 37; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 8 && autobusik_int[2] == 4) { //184
		kierunek_A = 40; kierunek_B = 38; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 8 && autobusik_int[2] == 6) { //186
		kierunek_A = 38; kierunek_B = 39; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 1 && autobusik_int[1] == 8 && autobusik_int[2] == 9) { //189
		kierunek_A = 32; kierunek_B = 37; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 3 && autobusik_int[1] == 0 && autobusik_int[2] == 3) { //303
		kierunek_A = 56; kierunek_B = 57; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 3 && autobusik_int[1] == 2 && autobusik_int[2] == 0) { //320
		kierunek_A = 54; kierunek_B = 55; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 5 && autobusik_int[1] == 0 && autobusik_int[2] == 3) { //503
		kierunek_A = 44; kierunek_B = 45; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 5 && autobusik_int[1] == 2 && autobusik_int[2] == 3) { //523
		kierunek_A = 30; kierunek_B = 31; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 7 && autobusik_int[1] == 1 && autobusik_int[2] == 4) { //714
		kierunek_A = 32; kierunek_B = 60; cyfra_int = kierunek_A;
	} else if (autobusik_int[0] == 7 && autobusik_int[1] == 2 && autobusik_int[2] == 0) { //720
		kierunek_A = 58; kierunek_B = 59; cyfra_int = kierunek_A;
	}
	else{
		lcd_locate(1,0); lcd_str("Bledny numer"); _delay_ms(2000);
		lcd_locate(1,0); lcd_str("Nacisnij Reset  "); _delay_ms(2000);
	}
	_delay_ms(1000);
	wyswietl_numer(cyfra_int);
	while ( klawisz_ENT_pressed ){

		if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
			_delay_ms(80);
			if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
				cyfra_int = kierunek_B;
				wyswietl_numer (cyfra_int);
				_delay_ms(200);
			}
		}
		if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
			_delay_ms(80);
			if (! ( KEY_PIN & ( klawisz_NUM ) ) ){
				cyfra_int = kierunek_A;
				wyswietl_numer (cyfra_int);
				_delay_ms(200);
			}
		}
	}
	autobusik_int[3] = cyfra_int;
	cyfra_char = cyfra_int+48;
	autobusik_char[3] = cyfra_char;
}
//-------------------------------------------------------------------------------------------------
