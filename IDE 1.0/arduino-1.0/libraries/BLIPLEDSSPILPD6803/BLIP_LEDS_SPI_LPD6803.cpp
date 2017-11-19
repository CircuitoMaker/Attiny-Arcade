/*
  BLIP_LEDS_SPI.cpp - A Library for use with LED Pixels using the LPD6803 Controller chip
  Copyright (c) 2010 Ben Moyes http:www.bliptronics.com  All right reserved.
  
  
  Modified Jul 2011 by Ben Moyes & Andrew Tobolov:- 
  Portions of this library are based on the FastSPI_LED Library by Daniel Garcia as 
  it related to the LPD6803 Controller chip.
  
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <WProgram.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "BLIP_LEDS_SPI_LPD6803.h"      
#include "wiring.h"


/******************************************************************************
 * SPI Defines
 ******************************************************************************/

// Duemilanove and mini w/328
#if defined(__AVR_ATmega328P__)
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_PIN  PINB
#define SPI_MOSI 3       // Arduino pin 11.
#define SPI_MISO 4       // Arduino pin 12.
#define SPI_SCK  5       // Arduino pin 13.
#define SPI_SSN  2       // Arduino pin 10.
#define DATA_PIN 11
#define SLAVE_PIN 12
#define CLOCK_PIN 13
#define LATCH_PIN 10

// Diecimila, Duemilanove, Nano w/ ATmega168
#elif defined(__AVR_ATmega168__)
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_PIN  PINB
#define SPI_MOSI 3       // Arduino pin 11.
#define SPI_MISO 4       // Arduino pin 12.
#define SPI_SCK  5       // Arduino pin 13.
#define SPI_SSN  2       // Arduino pin 10.
#define DATA_PIN 11
#define SLAVE_PIN 12
#define CLOCK_PIN 13
#define LATCH_PIN 10

// Mega.
#elif defined(__AVR_ATmega1280__)          
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_PIN  PINB
#define SPI_MOSI 2       // Arduino pin 51.
#define SPI_MISO 3       // Arduino pin 50.
#define SPI_SCK  1       // Arduino pin 52.
#define SPI_SSN  0       // Arduino pin 53.
#define DATA_PIN 51
#define SLAVE_PIN 50
#define CLOCK_PIN 52
#define LATCH_PIN 53
#endif

/******************************************************************************
 * Constructors
 ******************************************************************************/

CBLIP_LEDS_SPI BL;


static unsigned char *pData;
static unsigned int *Display;   

         
void CBLIP_LEDS_SPI::init()                     // Initialisation Function
{ 
     
  pData = m_pDataEnd; 
  setup_hardware_spi();               
  
  unsigned int Counter;
  for(Counter = 1; Counter<NUM_LEDS; Counter++)
  {
    setPixel(Counter, 0x80);
  }

  Display=(unsigned int *) m_pData;  
  sei();      
  SPDR = 0x00; 
}

void CBLIP_LEDS_SPI::setDirty() { m_nDirty = 1; } 


#define SPI_DATA(data) SPDR=data;

 //About to send out data stream from start? We need to send 
      
      
ISR(SPI_STC_vect) 
{
      static unsigned char nState=0;             //nState:
                                                 //0-3 set out a 0
      if(nState<=3)                              //4 - init data pointer and send first byte
                                                 //5 - sending data until we hit end
      {                                          //6 - all data sent, just send a zero until dirty is set, then set nstate to 0
        nState++;
        SPI_DATA(0);
        return;
      }

      if(nState==4)
      {
        pData = BL.m_pData;
        nState++;  
      }

      if(nState==5)    
      {
            
        if(pData >= BL.m_pDataEnd)
        {
            nState = 6;
        }
        SPI_DATA (*(pData++)); 
        return;
      }
      if(nState==6)    
      {
            
        if(BL.m_nDirty == 1)
        {
            nState=0;
            BL.m_nDirty = 0; 
        }
        SPI_DATA (0);        
        return;
      }
}        





unsigned int CBLIP_LEDS_SPI::color(byte r, byte g, byte b)           //Take the lowest 5 bits of each value and append them end to end
  {
   return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
  }
  
  
void CBLIP_LEDS_SPI::setup_hardware_spi(void)         
{       

  pinMode(DATA_PIN,OUTPUT);                       // Pin Setup of specified hardware                         
  pinMode(LATCH_PIN,OUTPUT);
  pinMode(CLOCK_PIN,OUTPUT);
  pinMode(SLAVE_PIN,OUTPUT);
  digitalWrite(DATA_PIN,LOW);
  digitalWrite(LATCH_PIN,LOW);
  digitalWrite(CLOCK_PIN,LOW);
  digitalWrite(SLAVE_PIN,LOW);

  // SPI prescaler:
  // SPI2X SPR1 SPR0
  //   0     0     0    fosc/4
  //   0     0     1    fosc/16
  //   0     1     0    fosc/64
  //   0     1     1    fosc/128                  
  //   1     0     0    fosc/2                    << Set in this instance                      
  //   1     0     1    fosc/8
  //   1     1     0    fosc/32
  //   1     1     1    fosc/64
  
  SPCR |= ( (1<<SPE) | (1<<MSTR) | (1<<SPIE) ); // enable SPI as master
  SPCR &= ~( (1<<SPR1) | (1<<SPR0) );           // clear prescaler bits
  SPSR |= (1<<SPI2X);
 
}

void CBLIP_LEDS_SPI::setPixel(unsigned int LED, unsigned int color)
{
    Display[LED]= 0x80 | (((color >> 8)& 0xFF)|((color << 8)&0xFF00));  
}

void CBLIP_LEDS_SPI::setPixel(unsigned int LED, byte rr, byte gg, byte bb)
{
    unsigned int color;
    color=((unsigned int)gg & 0x1F )<<10 | ((unsigned int)bb & 0x1F)<<5 | (unsigned int)rr & 0x1F;
    Display[LED]= 0x80 | (((color >> 8)& 0xFF)|((color << 8)&0xFF00));  
}

/******************************************************************************
 * LED EFFECTS FUNCTIONS
 ******************************************************************************/
 
 
 unsigned int CBLIP_LEDS_SPI::Wheel(byte WheelPos)
{
  byte r,g,b;
  switch(WheelPos >> 5)
  {
  case 0:
    r=31- WheelPos % 32;   //Red down
    g=WheelPos % 32;      // Green up
    b=0;                  //blue off
    break; 
  case 1:
    g=31- WheelPos % 32;  //green down
    b=WheelPos % 32;      //blue up
    r=0;                  //red off
    break; 
  case 2:
    b=31- WheelPos % 32;  //blue down 
    r=WheelPos % 32;      //red up
    g=0;                  //green off
    break; 
  }
  return(BL.color(r,g,b));
}



void CBLIP_LEDS_SPI::setRange(unsigned int startLED, unsigned int endLED, unsigned int color )
{
  // set a linear range of LEDs. The color value must be created with the Color function (15 bit rgb)
  while(startLED <= endLED)
    //Display[startLED++]=color;
    setPixel(startLED, color);
}



//Translate x and y to a LED index number in an array.
//Assume LEDS are layed out in a zig zag manner eg for a 3x3:
//0 5 6
//1 4 7
//2 3 8

unsigned int CBLIP_LEDS_SPI::Translate(byte x, byte y)
{
	if(x%2)
	{
		return(((x+1) * gridHeight)- 1 - y);
	}
	else
	{
		return((x * gridHeight) + y);
	}
}


// Draw a line in defined color between two points
// Using Bresenham's line algorithm, optimized for no floating point.

void CBLIP_LEDS_SPI::line(int x0,  int y0, int x1, int y1, unsigned int color)
{
     boolean steep;
     steep= abs(y1 - y0) > abs(x1 - x0);
     if (steep)
    {
         swap(&x0, &y0);
         swap(&x1, &y1);
    }
     if (x0 > x1)
    {
         swap(&x0, &x1);
         swap(&y0, &y1);
    }
     int deltax = x1 - x0;
     int deltay = abs(y1 - y0);
     int error = 0;
     int ystep;
     int y = y0;
     int x;
     if (y0 < y1) 
       ystep = 1; 
     else 
       ystep = -1;
     for (x=x0; x<=x1; x++) // from x0 to x1
       {
         if (steep)
          setPixel((Translate(y,x)),color);
         else 
           setPixel((Translate(x,y)),color);
         error = error + deltay;
         if (2 * error >= deltax)
           {
           y = y + ystep;
           error = error - deltax;
           }
       }
}


////Swap the values of two variables, for use when drawing lines.
void CBLIP_LEDS_SPI::swap(int * a, int * b)
{
  int temp;
  temp=*b;
  *b=*a;
  *a=temp;
}


void CBLIP_LEDS_SPI::box(byte x0, byte y0, byte x1, byte y1, unsigned int color)
{
  line(x0,y0,x1,y0,color);
  line(x1,y0,x1,y1,color);
  line(x1,y1,x0,y1,color);
  line(x0,y1,x0,y0,color);  

}


/******************************************************************************
 * 
 ******************************************************************************/


