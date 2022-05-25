/*
 * File:   main.c
 * Author: Labmems
 *
 * Created on 27 de abril de 2021, 12:16 AM
 */

#include "device_config.h"
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>


#define _XTAL_FREQ 1000000
#define SWEEP_STEP 1
#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

char frequency[3];        

void send2LCD(char);
void LCD_init(void);
void LCD_cmd(char);
void portsInit(void);
void delay_counter(void);
void IntegertoString(char*,int);
    
void main(void) {
    portsInit();
    
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E  = 0;
    
    LCD_init();
    
    send2LCD('F');
    send2LCD('r');
    send2LCD('e');
    send2LCD('q');
    send2LCD('u');
    send2LCD('e');
    send2LCD('n');
    send2LCD('c');
    send2LCD('y');
    send2LCD(':');
    
    delay_counter();    //initializes the timer and counter, and returns the value of the counter

    
    LCD_cmd(0xC4);      //Position the cursor in the fifth space of the second row    
    send2LCD(' ');
    send2LCD('H');
    send2LCD('z');  
    
    while(1){
        delay_counter();
    }   
}    

void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
    __delay_ms(250);
}

void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x16
    __delay_ms(250);  
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    __delay_ms(250);  
    LCD_cmd(0x01);          // Clear display and move cursor home
    __delay_ms(250);  
}


void LCD_cmd(char cx) {
   //LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

void portsInit(void){
    OSCCON = 0x34;// Set the internal oscillator to 8MHz and stable
    ANSELC = 0;   // Set port D as Digital for 7 segment anodes
    TRISC  = 0x00;      // for Port D, set all pins as outputs for 7 segment anodes
    ANSELD = 0;   // Set port D as Digital for 7 segment anodes
    TRISD  = 0x00;      // for Port D, set all pins as outputs for 7 segment anodes
    TRISAbits.TRISA4 = 1;
    LCD_DATA_DIR = 0x00;
    
}

void delay_counter(void){
    TMR1H = 0x0B;   //High part 0x0BDC
    TMR1L = 0xDC;   //Low part 0x0BDC
    T1GCONbits.TMR1GE = 0;  //Timer starts via firmware
    PIR1bits.TMR1IF = 0;    //Clear the TMR1 overflow flag
    T1CON = 0b00100001;     //Configure and start Timer1
    while(PIR1bits.TMR1IF == 0){    //Wait for overflow
       
        T0CON = 0b11111000;     //Configure and start Timer0
    }
    IntegertoString(frequency, TMR0L);  //Converts value of counter to a character array
    LCD_cmd(0xC0);      //Sends cursor to first position in second row
    for(int i=0; i<sizeof(frequency); i++){ 
        if(frequency[i]=='0' | frequency[i]=='1' | frequency[i]=='2' | frequency[i]=='3' | frequency[i]=='4' | frequency[i]=='5' | frequency[i]=='6' | frequency[i]=='7' | frequency[i]=='8' | frequency[i]=='9'){
            send2LCD(frequency[i]); //if there is a number (char) in the array it sends it to the LCD
        }
        else{
            send2LCD(' ');  //if there isn't a number it sends a blank space
        }
    }
    memset(frequency,' ',sizeof(frequency));    //resets the array
    T0CON = 0x00;   //Stop Timer0
    T1CON = 0x00;   //Stop Timer1
    PIR1bits.TMR1IF = 0;    //Clear the TMR1 overflow flag
    TMR0L = 0x00;   //Reset counter
}

void IntegertoString(char * string, int number) {   //Converts integer into string

   if(number == 0) { string[0] = '0'; return; };
   int divide = 0;
   int modResult;
   int  length = 0;
   int isNegative = 0;
   int  copyOfNumber;
   int offset = 0;
   copyOfNumber = number;
   if( number < 0 ) {
     isNegative = 1;
     number = 0 - number;
     length++;
   }
   while(copyOfNumber != 0) 
   { 
     length++;
     copyOfNumber /= 10;
   }

   for(divide = 0; divide < length; divide++) {
     modResult = number % 10;
     number    = number / 10;
     string[length - (divide + 1)] = modResult + '0';
   }
   if(isNegative) { 
   string[0] = '-';
   }
   string[length] = '\0';
}
