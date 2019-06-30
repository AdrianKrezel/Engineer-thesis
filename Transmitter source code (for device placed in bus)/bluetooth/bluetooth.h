/*
 * bluetooth_send.h
 *
 *  Created on: 17-01-2015
 *      Author: Adrian
 */
#ifndef LCD_H
#define LCD_H

extern char autobusik_char[4];

void InitUSART( unsigned long int baud );

//wysy³anie
void TransmitUSART(unsigned char data);
void bluetooth_transmit(char autobusik_char[4]);

//odbieranie
unsigned char ReceiveUSART( void );

#endif
