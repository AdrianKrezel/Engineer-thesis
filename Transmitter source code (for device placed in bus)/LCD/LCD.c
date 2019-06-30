/*
 * main.c
 *
 *  Created on: 26-02-2015
 *      Author: Adrian
 */

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "lcd44780.h"

// funkcja odpowiedzialna za wyswietlenie wybranego numeru i kierunku, w którym jedzie pojazd komunikacji publicznej
void wyswietl_numer (cyfra){//zamiast wybor wstawic autobusik_uint
	char zero[] = {"0"};	//0
	char jeden[] = {"1"};	//1
	char dwa[] = {"2"};		//2
	char trzy[] = {"3"};	//3
	char cztery[] = {"4"};	//4
	char piec[] = {"5"};	//5
	char szesc[] = {"6"};	//6
	char siedem[] = {"7"};	//7
	char osiem[] = {"8"};	//8
	char dziewiec[] = {"9"};//9

	char PKP_Olszynka_Grochowska[] = {"PKP Olsz. Groch."};
	char Stare_Bemowo[] = {"Stare Bemowo"};
	char Osiedle_Gorczewska[] = {"Os. Gorczewska"};
	char Gwiazdzista[] = {"Gwiazdzista"};
	char PR_Al_Krakowska[] = {"P+R Al. Krakow."};
	char Boernerowo[] = {"Boernerowo"};
	char Zeran_FSO[] = {"Zeran FSO"};
	char Sadyba[] = {"Sadyba"};
	char Szczesliwice[] = {"Szczesliwice"};
	char Nowodwory[] = {"Nowodwory"};
	char Metro_Mlociny[] = {"Metro Mlociny"};
	char CH_Blue_City[] = {"CH Blue City"};
	char CH_Reduta[] ={"CH Reduta"};
	char Witolin[] = {"Witolin"};
	char Natolin_Pln[] = {"Natolin Polnocny"};
	char Konwiktorska[] = {"Konwiktorska"};
	char Metro_Marymont[] = {"Metro Marymont"};
	char Dw_Wsch_Kijowska[] = {"Dw.Wsch.Kijowska"};
	char Annopol[] = {"Annopol"};
	char Banacha[] = {"Banacha"};
	char Lipkow_Muzeum[] ={"Lipkow-Muzeum"};


	for (uint8_t k=0; k<16; k++){
		lcd_locate(1,k);
		lcd_str(" ");
	}
	lcd_locate(0,15);
	switch (cyfra){
		case 0 :   lcd_str(zero);		  		break;
		case 1 :   lcd_str(jeden); 		 		break;
		case 2 :   lcd_str(dwa);				break;
		case 3 :   lcd_str(trzy); 		  		break;
		case 4 :   lcd_str(cztery); 	  		break;
		case 5 :   lcd_str(piec); 		  		break;
		case 6 :   lcd_str(szesc); 		 	 	break;
		case 7 :   lcd_str(siedem); 	  		break;
		case 8 :   lcd_str(osiem); 		  		break;
		case 9 :   lcd_str(dziewiec); 	  		break;

		case 30:   lcd_locate(1,0);lcd_str(Stare_Bemowo);			break;
		case 31:   lcd_locate(1,0);lcd_str(PKP_Olszynka_Grochowska);break;
		case 32:   lcd_locate(1,0);lcd_str(Osiedle_Gorczewska);		break;
		case 33:   lcd_locate(1,0);lcd_str(Gwiazdzista);			break;
		case 34:   lcd_locate(1,0);lcd_str(PR_Al_Krakowska);		break;
		case 35:   lcd_locate(1,0);lcd_str(Boernerowo);				break;
		case 36:   lcd_locate(1,0);lcd_str(Zeran_FSO);				break;
		case 37:   lcd_locate(1,0);lcd_str(Sadyba);					break;
		case 38:   lcd_locate(1,0);lcd_str(Szczesliwice);			break;
		case 39:   lcd_locate(1,0);lcd_str(Nowodwory);				break;
		case 40:   lcd_locate(1,0);lcd_str(Metro_Mlociny);			break;
		case 41:   lcd_locate(1,0);lcd_str(CH_Blue_City);			break;
		case 42:   lcd_locate(1,0);lcd_str(CH_Reduta);				break;
		case 43:   lcd_locate(1,0);lcd_str(Witolin);				break;
		case 44:   lcd_locate(1,0);lcd_str(Natolin_Pln);			break;
		case 45:   lcd_locate(1,0);lcd_str(Konwiktorska);			break;
		case 46:   lcd_locate(1,0);lcd_str(Metro_Marymont);			break;
		case 47:   lcd_locate(1,0);lcd_str(Dw_Wsch_Kijowska);		break;
		case 48:   lcd_locate(1,0);lcd_str(Annopol);				break;
		case 49:   lcd_locate(1,0);lcd_str(Banacha);				break;
		case 50:   lcd_locate(1,0);lcd_str(Lipkow_Muzeum);			break;

	}
}
//-------------------------------------------------------------------------------------------------

// animacja startowa-------------------------------------------------------------------------------
void start(void){
	lcd_cls();
	lcd_locate(0,5);
	lcd_str("START");
	_delay_ms(1000);
	for (uint8_t i=0; i<16; i++){
		lcd_locate(1,i);
		_delay_ms(50);
		lcd_str(".");
	}
	_delay_ms(1000);
	lcd_cls();
}
//-------------------------------------------------------------------------------------------------
