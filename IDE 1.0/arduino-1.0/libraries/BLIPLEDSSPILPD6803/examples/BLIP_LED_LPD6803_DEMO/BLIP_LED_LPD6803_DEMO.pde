#include <BLIP_LEDS_SPI_LPD6803.h>

/*Example sketch for driving LPD6803 controlled RGB LEDs

  Bliptronics.com
  Ben Moyes 2009
  Use this as you wish, but please give credit, or at least buy some of my LEDs!

  Modified Jul 2011 by Ben Moyes and Andrew Tobolov to show off updated BLIP_LEDS_SPI_LPD6803 Library

  Note, because this library uses the arduino's hardware SPI support, you MUST use the SPI 
  pins on your hardware - e.g. pins 11 and 13 on a Arduino pro mini or such. 
  Refer to the library for pins in use by your board type.

  Each LED holds a 15 bit RGB value.
  Note, you've only got limited memory on the Arduino, so you can only control 
  Several hundred LEDs on a normal arduino. Double that on a Duemilanove. 
  
  Refer to the accompanied README.txt file for further explanation on how to use our library and the
  functions used in this sketch*/

#define NUM_LEDS 25                        // Set the number of LEDs in use here


void setup() 
{  
  BL.setLeds(NUM_LEDS);                   // Initialisation functions
  BL.init();                              // SPI interrupt will now send out data to LEDs. This happens in the background, pretty fast.
        
  BL.gridHeight=5;                        // Grid dimensions for LINE and BOX effect functions are defined here. Also used for Spectrum Analyzer function.
  BL.gridWidth=5;
  
}

void loop() 
{
 
  unsigned int Counter, Counter2, Counter3;
  blank();                                                                  // Clears any color data remaining in LEDs
  
  for(int Counter=0;Counter<10;Counter++)                                   // Fade out effect
   ON_Fade();
  blank();
  

  for(int i=0;i<20;i++)                                                      // Strobe Effect
   strobe(30);


  for(Counter=0;Counter < 5;Counter++)                                      // Color Wipe Effect
  {
    ColorUp(BL.color(random(0,32),random(0,32),random(0,32)));
  }
  

  for(Counter=0; Counter < 96 ; Counter++)                                   // Scrolling Rainbow Effect
  {
    for(Counter2=0; Counter2 < NUM_LEDS; Counter2++)
    {
      BL.setPixel(Counter2, BL.Wheel((Counter2*2 + Counter)%96));
      Counter3+=(96 / NUM_LEDS);
    }    
    BL.show();
    delay(25);
  }
  
  blank();

  BL.line(0,0,4,4,BL.color(random(0,31),random(0,31),random(0,31)));          // LINE Function Demo
  BL.show();
  delay(500);
  blank();
  BL.line(0,4,4,0,BL.color(random(0,31),random(0,31),random(0,31)));
  BL.show();
  delay(500);
  blank();
  
  
  BL.box(0,0,4,4,BL.color(random(0,31),random(0,31),random(0,31)));          // BOX Function Demo
  BL.show();
  delay(500);
  blank();
  BL.box(1,1,3,3,BL.color(random(0,31),random(0,31),random(0,31)));
  BL.show();
  delay(500);
  blank();
  BL.box(2,2,2,2,BL.color(random(0,31),random(0,31),random(0,31)));
  BL.show();
  delay(500);
}

//***********************************************************************************************************


void blank()                                      //Used to load all LEDs with no color value, clearing LEDs
{
for(byte Counter=0;Counter < NUM_LEDS; Counter++)
    BL.setPixel(Counter,BL.color(0,0,0));
    BL.show();
}   
    

void ColorUp( unsigned int ColorToUse)
{
  byte Counter;
  for(Counter=0;Counter < NUM_LEDS; Counter++)
  {
    BL.setPixel(Counter, ColorToUse);
    BL.show();
    delay(25);
  }  
}


void ON_Fade ()
{
  byte Counter;
  unsigned int a=0xff,b=0xff,c=0xff;
  
  for(Counter=0;Counter < NUM_LEDS; Counter++)
  {  
    BL.setPixel(Counter,BL.color(31,31,31));
  }
  BL.show();
  delay(10);
  
  for(int i = 0; i<31;i++)
  {
    for(Counter=0;Counter < NUM_LEDS; Counter++)
    { 
      BL.setPixel(Counter,BL.color(a,b,c));
    }
BL.show();
    delay(10);
    a--;
    b--;
    c--; 
  }
}


void strobe(int flash_rate)
{
  for(int i=0;i < NUM_LEDS; i++)
  {  
    BL.setPixel(i,BL.color(31,31,31));
  }
  BL.show();
  delay(flash_rate);
  
  blank();
  BL.show();
  delay(flash_rate);
}



