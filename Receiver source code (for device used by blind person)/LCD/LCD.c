/*
 * main.c
 *
 *  Created on: 26-02-2015
 *      Author: Adrian
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <inttypes.h>

#include "lcd44780.h"

//wyswietlenie numeru linii
void wyswietl_numer(cyfra) {//zamiast wybor wstawic autobusik_uint
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
	}
}

//animacja startowa
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
	_delay_ms(200);
}

