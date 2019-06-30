/*
 * bluetooth_send.h
 *
 *  Created on: 17-01-2015
 *      Author: Adrian
 */


#ifndef BLUETOOTH_H
#define BLUETOOTH_H


// inicjalizacja USART
void InitUSART( unsigned long int baud );

// wysy³anie
void TransmitUSART(unsigned char data);

//odbieranie
unsigned char ReceiveUSART( void );


//KAROL***************************
char *BT_receive();
int buforSize(char *bufor);
//END_KAROL***********************



#endif
