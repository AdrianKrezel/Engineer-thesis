/*
 * main.c			F_CPU = 20MHz
 *
 *  Created on: 2011-11-11
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "diskio.h"
#include "pff.h"

#define USE44KHZ 	0	// 0 = pliki max 22,050kHz,    1 = pliki max 44,1kHz

// obs³uga Timer0 z preskalerem = 8
#define TMR_START TCCR0 |= (1<<CS01)
#define TMR_STOP TCCR0 &= ~(1<<CS01)
// obs³uga Timer0 z preskalerem = 64
#define TMR64_START TCCR0 |= (1<<CS01)|(1<<CS00)
#define TMR64_STOP TCCR0 &= ~((1<<CS01)|(1<<CS00))

// makra i zmienne na potrzeby obs³ugi PetitFAT
#define SCK 	PB7
#define MOSI 	PB5
#define MISO 	PB6
#define CS 		PB4

#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)	/* FourCC */

// definicja struktury z parametrami WAV
typedef struct {
	uint8_t stereo:1;
	uint8_t prescaler:1;
	uint8_t resolution;
	uint16_t khz;
} _FLAGS;

volatile _FLAGS FLAGS;	// definicja struktury

volatile uint8_t can_read;

FATFS Fs;			/* File system object */
DIR Dir;			/* Directory object */
FILINFO Fno;		/* File information */

WORD rb;

static DWORD load_header (void);
static UINT play ( const char *fn );
// ******************************************************

volatile uint16_t tick;			// potrzebne do wyœwietlania czasu utworu
volatile uint8_t slupek;		// do obs³ugi wskaŸnika wysterowania

volatile int8_t sk1=-1, sk2=4;	// zmienne pomocnicze do efektów dŸwiêkowych

#define BUF_SIZE 512			// maksymalny rozmiar pojedynczego bufora

uint8_t  buf[2][BUF_SIZE];		// podwójny bufor do odczytu z karty SD

volatile uint8_t nr_buf;		// indeks aktywnego buforu

BYTE rcv_spi (void)
{
	SPDR = 0xFF;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

// ********** FUNKCJA ODTWARZAJ¥CA DWIÊK *********************************
void komunikat_glosowy(uint8_t komunikat){
	//PLAYER_init();
	while(1) {
		pf_mount(&Fs);	/* Initialize FS */
		_delay_ms(300);			// odczekaj 300ms
		play(dzwiek(komunikat)); // odtwarzaj plik WAV
		break;
	}
}

//*********** INICJALIZACJA ODTWARZACZA ***********************************
void PLAYER_init(void){

	// init SPI
	DDRB |= (1<<CS)|(1<<MOSI)|(1<<SCK);
	PORTB = 0xf0; // podci¹gniêcie SPI do VCC
	SPCR |= (1<<SPE)|(1<<MSTR); //odblokowanie SPI, tryb master
	SPSR |= (1<<SPI2X);	// masymalny zegar SCK (fclk/2)

	// Timer0 (samplowanie)
	TCCR0 = (1<<WGM01);		// tryb CTC
	TIMSK = (1<<OCIE0);		// zezwolenie na przerwanie CompareMatch

	// Timer1 (konfiguracja PWM - noœna)
	TCCR1A = (1<<WGM10)|(1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1);//zezwolenie na przerwanie CompareMatch (ustawia wyjscie na stan niski), tryb PWM 8bit
	TCCR1B = (1<<CS10); // preskaler = 1
}

void dzwiek (int komunikat) {
	char zero[] = "0.wav";					  // "0"
	char jeden[] = "1.wav";					  // "1"
	char dwa[] = "2.wav";					  // "2"
	char trzy[] = "3.wav";					  // "3"
	char cztery[] = "4.wav";				  // "4"
	char piec[] = "5.wav";					  // "5"
	char szesc[] = "6.wav";				  	  // "6"
	char siedem[] = "7.wav";				  // "7"
	char osiem[] = "8.wav";					  // "8"
	char dziewiec[] = "9.wav";				  // "9"

	char start[] = "10.wav";				  // "witaj u¿ytkowniku, trwa konfiguracja urz¹dzenia. Proszê czekaæ"
	char wprowadz_numer[] = "11.wav";	 	  // "proszê wprowadziæ numer linii komunikacji miejskiej"
	char setki[] = "12.wav";				  // "wprowadŸ liczbê setek
	char dziesiatki[] = "13.wav";	 		  // "wprowadŸ liczbê dziesi¹tek
	char jednosci[] = "14.wav";		 		  // "wprowadŸ liczbê jednosci
	char wybrano_linie[] = "15.wav";	 	  // "wybrano liniê numer(...)"
	char podjechala_linia[] = "16.wav";	  	  // "podjecha³a Twoja linia (...)"
	char koniec[] = "17.wav";			 	  // "zakoñczy³em pracê, teraz mo¿esz wy³¹czyæ urz¹dzenie"
	char enter[] = "18.wav";				  // "enter"
	char wprowadz_kierunek[] = "19.wav";	  // "wprowadz kierunek"
	char szukam[] = "20.wav";				  // "szukam wybranej linii"
	char inny_pojazd[] = "21.wav";			  // "Jedzie inny pojazd komunikacji miejskiej"
	char zly_numer[] = "22.wav";			  // "Nie ma takiej linii komunikacji publicznej. Zresetuj urz¹dzenie i wprowadŸ inny numer"

	char Stare_Bemowo[] = "30.wav";
	char PKP_Olszynka_Grochowska[] = "31.wav";
	char Osiedle_Gorczewska[] = "32.wav";
	char Gwiazdzista[] = "33.wav";
	char PR_Al_Krakowska[] = "34.wav";

	char Boernerowo[] = "35.wav";
	char Zeran_FSO[] = "36.wav";
	char Sadyba[] = "37.wav";
	char Szczesliwice[] = "38.wav";
	char Nowodwory[] = "39.wav";

	char Metro_Mlociny[] = "40.wav";
	char CH_Blue_City[] = "41.wav";
	char CH_Reduta[] = "42.wav";
	char Witolin[] = "43.wav";
	char Natolin_Pln[] = "44.wav";

	char Konwiktorska[] = "45.wav";
	char Metro_Marymont[] = "46.wav";
	char Dw_Wsch_Kijowska[] = "47.wav";
	char Annopol[] = "48.wav";
	char Banacha[] = "49.wav";
	char Lipkow_Muzeum[] = "50.wav";


	switch (komunikat) {
		case 0 :    play(zero);				break;
		case 1 :    play(jeden); 			break;
		case 2 :    play(dwa); 	 			break;
		case 3 :    play(trzy); 			break;
		case 4 :    play(cztery); 			break;
		case 5 :    play(piec);  			break;
		case 6 :    play(szesc);    		break;
		case 7 :    play(siedem); 			break;
		case 8 :    play(osiem); 			break;
		case 9 :    play(dziewiec); 		break;

		case 10:	play(start); 			break;
		case 11:	play(wprowadz_numer);	break;
		case 12:	play(setki);			break;
		case 13:	play(dziesiatki);		break;
		case 14:	play(jednosci);			break;
		case 15:	play(wybrano_linie);	break;
		case 16:	play(podjechala_linia); break;
		case 17:	play(koniec);			break;
		case 18: 	play(enter);			break;
		case 19: 	play(wprowadz_kierunek);break;
		case 20: 	play(szukam);			break;
		case 21:    play(inny_pojazd); 		break;
		case 22:    play(zly_numer);		break;

		case 30:   play(Stare_Bemowo);					break;
		case 31:   play(PKP_Olszynka_Grochowska);		break;
		case 32:   play(Osiedle_Gorczewska);			break;
		case 33:   play(Gwiazdzista);					break;
		case 34:   play(PR_Al_Krakowska);				break;
		case 35:   play(Boernerowo);					break;
		case 36:   play(Zeran_FSO);						break;
		case 37:   play(Sadyba);						break;
		case 38:   play(Szczesliwice);					break;
		case 39:   play(Nowodwory);						break;
		case 40:   play(Metro_Mlociny);					break;
		case 41:   play(CH_Blue_City);					break;
		case 42:   play(CH_Reduta);						break;
		case 43:   play(Witolin);						break;
		case 44:   play(Natolin_Pln);					break;
		case 45:   play(Konwiktorska);					break;
		case 46:   play(Metro_Marymont);				break;
		case 47:   play(Dw_Wsch_Kijowska);				break;
		case 48:   play(Annopol);						break;
		case 49:   play(Banacha);						break;
		case 50:   play(Lipkow_Muzeum);					break;
	}
}


//***************** przerwanie TIMER0 - samplowanie ******************************************
ISR(TIMER0_COMP_vect) {

#if USE44KHZ == 1
	static uint16_t buf_idx;		// indeks w pojedynczym buforze
	static uint8_t v1, v2;			// zmienne do przechowywania próbek
	buf_idx++;						// pomijamy m³odszy bajt 16-bitowej próbki kana³ L
	v1 = buf[nr_buf][buf_idx++]-128;// -128 korekcja konwerji próbki 16-bitowej do 8-bitowej
	buf_idx++;						// pomijamy m³odszy bajt 16-bitowej próbki kana³ R
	v2 = buf[nr_buf][buf_idx++]-128;// -128 korekcja konwerji próbki 16-bitowej do 8-bitowej
	OCR1A = v1;						// próbka na wyjœcie PWM1, kana³ L
	OCR1B = v2;						// próbka na wyjœcie PWM2, kana³ R

	buf_idx &= 0x01ff;
	if( !buf_idx ) {
		can_read = 1;
		nr_buf ^= 0x01;
	}
#endif

#if USE44KHZ == 0
	static uint16_t buf_idx;		// indeks w pojedynczym buforze
	static uint8_t v1, v2;			// zmienne do przechowywania próbek
	static uint8_t efekt_cnt;		// zmienna pomocnicza dla uzyskiwania prostych efektów

	if( !efekt_cnt ) {
		if( 16 == FLAGS.resolution ) {				// jeœli rozdzielczoœæ 16-bitów
			if( FLAGS.stereo ) {					// jeœli próbki stereofoniczne
				buf_idx++;							// pomijamy m³odszy bajt 16-bitowej próbki kana³ L
				v1 = buf[nr_buf][buf_idx++]-128;	// -128 korekcja konwerji próbki 16-bitowej do 8-bitowej
				buf_idx++;							// pomijamy m³odszy bajt 16-bitowej próbki kana³ R
				v2 = buf[nr_buf][buf_idx++]-128;	// -128 korekcja konwerji próbki 16-bitowej do 8-bitowej
			} else {								// jeœli próbki monofoniczne
				buf_idx++;							// pomijamy m³odszy bajt 16-bitowej próbki MONO
				v1 = buf[nr_buf][buf_idx++]-128;	// -128 korekcja konwerji próbki 16-bitowej do 8-bitowej
				v2 = v1;							// to samo na dwa kana³y/wyjœcia
			}
		} else {									// jeœli rozdzielczoœæ 8-bitów
			if( !FLAGS.stereo ) {					// jeœli próbki monofoniczne
				v1 = buf[nr_buf][buf_idx++];		// pobieramy próbkê MONO do zmiennej v1
				v2 = v1;							// to samo na dwa kana³y/wyjœcia
			} else {								// jeœli próbki stereofoniczne
				v1 = buf[nr_buf][buf_idx++];		// pobieramy próbkê kana³ L
				v2 = buf[nr_buf][buf_idx++];		// pobieramy próbkê kana³ R
			}
		}

	}

	if(sk1>-1) {
		if( efekt_cnt++ > sk1 ) {
			efekt_cnt=0;
			buf_idx+=sk2;
		}
	} else {
		efekt_cnt=0;
		sk2=4;
	}

	OCR1A = v1;									// próbka na wyjœcie PWM1, kana³ L
	OCR1B = v2;									// próbka na wyjœcie PWM2, kana³ R

	if( buf_idx > BUF_SIZE-1 ) {
		buf_idx=0;								// reset indeksu bufora
		can_read = 1;							// flaga = 1
		nr_buf ^= 0x01;							// zmiana bufora na kolejny
	}

	tick++;										// podstawa czasu

	slupek = v1;								// zmienne na potrzeby wskaŸnika wysterowania
#endif
}
// *************************** koniec przerwania ****************************************


/* 0:Invalid format, 1:I/O error, >1:Number of samples */
static DWORD load_header (void) {
	DWORD sz;
	uint8_t *wsk_buf = &buf[0][0];
	uint16_t ocrx;

	if (pf_read(wsk_buf, 12, &rb)) return 1;	/* Load file header (12 bytes) */

	if (rb != 12 || LD_DWORD(wsk_buf+8) != FCC('W','A','V','E')) return 0;

	for (;;) {
		pf_read(wsk_buf, 8, &rb);					/* Get Chunk ID and size */
		if (rb != 8) return 0;
		sz = LD_DWORD(&wsk_buf[4]);					/* Chunk size */

		switch (LD_DWORD(&wsk_buf[0])) {			/* FCC */
		case FCC('f','m','t',' ') :					/* 'fmt ' chunk */
			if (sz > 100 || sz < 16) return 0;		/* Check chunk size */

			pf_read(wsk_buf, sz, &rb);				/* Get content */

			if (rb != sz) return 0;

			if (wsk_buf[0] != 1) return 0;			/* Check coding type (1) */

			if (wsk_buf[2] != 1 && wsk_buf[2] != 2) /* Check channels (1/2) */
				return 0;

			FLAGS.stereo = wsk_buf[2]==2;				/* Get channel flag */
			if (wsk_buf[14] != 8 && wsk_buf[14] != 16)	/* Check resolution (8/16) */
				return 0;

			FLAGS.resolution = wsk_buf[14];			// ustalamy jaka rozdzielczoœæ 16/8 - bitów
			FLAGS.prescaler = 0;

			// obliczmy preskaler Timera0 w zale¿noœci od czêstotliwoœci samplowania
			FLAGS.khz = LD_WORD(&wsk_buf[4]);
			ocrx = (uint16_t)(F_CPU/8/LD_WORD(&wsk_buf[4]))-1;
			if( ocrx > 255 ) {
				ocrx = (uint16_t)(F_CPU/64/LD_WORD(&wsk_buf[4]))-1;
				FLAGS.prescaler = 1;
			}
			OCR0 = (uint8_t)ocrx;					// obliczona wartoœæ OCR0
			break;

		case FCC('d','a','t','a') :				/* 'data' chunk (start to PLAY) */
			return sz;

		case FCC('L','I','S','T') :				/* 'LIST' chunk (skip) */
		case FCC('f','a','c','t') :				/* 'fact' chunk (skip) */
			pf_lseek(Fs.fptr + sz);
			break;

		default :								/* Unknown chunk (error) */
			return 0;
		}
	}

	return 0;
}

// ******************  funkcja  P L A Y  ********************************
static UINT play ( const char *fn ) {

	DWORD sz;
	FRESULT res;

	if ((res = pf_open(fn)) == FR_OK) {

		sz = load_header();						/* Load file header */
		if (sz < 256) return (UINT)sz;

		pf_lseek(0);

		pf_read(&buf[0][0], BUF_SIZE , &rb);	// za³aduj pierwsz¹ czêœæ bufora
		pf_read(&buf[1][0], BUF_SIZE , &rb);	// za³aduj drug¹ czêœæ bufora

		if( !FLAGS.prescaler ) TMR_START;		// start Timera0 (samplowanie) z
		else TMR64_START;						// preskalerem zale¿nym od czêstotliwoœci

		DDRD  |= (1<<PD5)|(1<<PD4);			// ustaw piny PWM1 (OC1A) oraz PWM2 (OC1B) jako wyjœcia WA¯NE !!!

#if USE44KHZ == 0
		uint8_t ss=0,mm=0;
		uint16_t kdelay = 0;					// zmienna pomocnicza do eliminacji drgañs styków
		uint8_t tab[8] = {0,1,3,7,15,31,63,127};// liniowa charakterystyka wsk. wysterowania
#endif


		while(1) {
			if( can_read ) {				// jeœli flaga ustawiona w obs³udze przerwania

				pf_read(&buf[ nr_buf ^ 0x01 ][0], BUF_SIZE , &rb);	// odczytaj kolejny bufor
				if( rb < BUF_SIZE ) break;		// jeœli koniec pliku przerwij pêtlê while(1)

#if USE44KHZ == 0
				PORTC = ~tab[slupek/33];		// obs³uga wsk. wysterowania

				// obliczanie czasu utworu
				if(tick>FLAGS.khz) {
					ss++;
					if(ss>59) {
						ss=0;
						mm++;
						if(mm>59) mm=0;
					}
					tick=0;
				}

#endif
				can_read = 0;
			}
		}
		DDRD  &= ~((1<<PD5)|(1<<PD4));		// ustaw piny PWM1 (OC1A) oraz PWM2 (OC1B) jako wejœcia WA¯NE !!!

		if( !FLAGS.prescaler ) TMR_STOP;	// wy³¹czenie Timera0 (samplowania)
		else TMR64_STOP;

		PORTC = 255;						// wyzerowanie wsk. wysterowania

		_delay_ms(500);						// przerwa 0,5s
	}

	return res;
}

