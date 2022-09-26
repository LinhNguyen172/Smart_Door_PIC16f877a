/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _MAIN_H_
#define	_MAIN_H_

// PIC16F877A Configuration Bit Settings

// 'C' source line configuration statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON      // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ             4000000

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#define ON 1
#define OFF 0

// use port D for keypad
#define col0 RD0 
#define col1 RD1
#define col2 RD2
#define col3 RD3
#define row0 RD4
#define row1 RD5
#define row2 RD6
#define row3 RD7

// ADC channel
#define ADC_sensor 0
#define threshhold 900

// servo control using timer 1 
#define min_duty 5
#define max_duty 10
#define TMR1_base 65385

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* _MAIN_H */

