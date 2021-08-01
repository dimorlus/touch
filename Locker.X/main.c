/*
 * IIC (no ACK, 100us delay after IIC byte transfer) Touch keyboard with 
 * slider and LEDs
 * 
 * Try move IIC to the high priority interrupt
 * Try to disable uTouch engine during IIC processing
 * Try to increase Fosc during IIC processing
 * 
 * 
 * 
 */
// <editor-fold defaultstate="collapsed" desc="PIC18F24K40 QFN Pins usage">
/* 
 *                         PIC18F24K40 QFN
 *                   +------------------------+
 *              Rx >-|15 RC7/Rx      Tx/RC6 14|-> Tx
 *             GND *-|16 GND        SDO/RC5 13|-> IO4         
 *             3V3 *-|17 Vcc        SDI/RC4 12|-> SDA
 *              B9 >-|18 RB0/AN12   SCK/RC3 11|-> SCI   
 *             B10 >-|19 RB1/AN10  CCP1/RC2 10|-> IO3          
 *             B11 >-|20 RB2/ANB2  CCP2/RC1  9|-> IO2      
 *             B12 >-|21 RB3/ANB3       RC0  8|-> IO1    
 *                   +------------------------+
 *              S1 >-|22 RB4/ANB4  ANA6/RA6  7|-> B7
 *              S2 >-|23 RB5/ANB5  ANA7/RA7  6|-> B8          
 *         IO5/PGC <-|24 RB6/ANB6      GND   5|-* GND
 *         IO6/PGD <-|25 RB7/ANB7  ANA5/RA5  4|-< B6   
 *             Rst *-|26 MCLR      ANA4/RA4  3|-< B5   
 *              B1 >-|27 RA0/ANA0  ANA3/RA3  2|-< B4 
 *              B2 >-|28 RA1/ANA1  ANA2/RA2  1|-< B3 
 *                   +-----------------------^*
 */
// </editor-fold>
#pragma warning disable 1090 //is not used
#pragma warning disable 1510 //non-reentrant function appears in multiple call graphs and has been duplicated by the compiler
#pragma warning disable 1492 //using updated 32-bit floating-point libraries; improved accuracy might increase code size
#include "mcc_generated_files/mcc.h"
#include "common.h"
/*
                         Main application
 */

uint8_t por_flag;
uint16_t DevID;
uint16_t RevID;

void GetDev(void)
{
  por_flag = ((PCON0&0x0b) == 0);
  //Read DEVICE ID and Revision ID
  //+-------+----------------+---------+
  //|       |                | Rev ID  |
  //|Dev ID | Device         | A3 | A4 |
  //+-------+----------------+----+----+
  //| 69c0  | PIC18F24K40    |a043|a044| 
  //| 6aa0  | PIC18LF24K40   |a043|a044| 
  //| 69a0  | PIC18F25K40    |a043|a044| 
  //| 6aa0  | PIC18LF25K40   |a043|a044| 
  //+-------+----------------+----+----+
/*------------------------------------------------*/
  NVMCON1 = 0x40;
  uint32_t tblptr = TBLPTR;
  TBLPTR = 0x3ffffc;
  asm("TBLRD*+");
  RevID = TABLAT;
  asm("TBLRD*+");
  RevID = RevID+(TABLAT*256);
  asm("TBLRD*+");
  DevID = TABLAT;
  asm("TBLRD*");
  DevID = DevID+(TABLAT*256);
  NVMCON1 = 0x00;
  TBLPTR = tblptr;
  NVMCON1 = 0x80; //Silicon Errata 3.1
 }
/*------------------------------------------------*/


//tld leds; 
//u32b keys; 
tinf inf;
volatile uint8_t _1ms_c; //cycled counter
volatile uint16_t _iic_timeout_ms; //resetable down counter;

#define LEDS 16

void keys_to_leds(void)
{
 inf.leds.u &= ~0xfff;
 inf.leds.u |= inf.keys.u&0xfff;
 switch(inf.keys.slider/51)
 {
  case 0:
   inf.leds.l12 = 0;
   inf.leds.l13 = 0;
   inf.leds.l14 = 0;
   inf.leds.l15 = 0;
  break; 
  case 1:
   inf.leds.l12 = 1;
   inf.leds.l13 = 0;
   inf.leds.l14 = 0;
   inf.leds.l15 = 0;
  break; 
  case 2:
   inf.leds.l12 = 1;
   inf.leds.l13 = 1;
   inf.leds.l14 = 0;
   inf.leds.l15 = 0;
  break; 
  case 3:
   inf.leds.l12 = 1;
   inf.leds.l13 = 1;
   inf.leds.l14 = 1;
   inf.leds.l15 = 0;
  break; 
  case 4:
  case 5:
   inf.leds.l12 = 1;
   inf.leds.l13 = 1;
   inf.leds.l14 = 1;
   inf.leds.l15 = 1;
  break; 
 }
}

void fleds(void)
{
#ifdef TEST
 return;
#else 
 if (_1ms_c&1)
 {
  if (inf.leds.l0) //-RB6
  {
   LATBbits.LATB6 = 0;
   TRISBbits.TRISB6 = 0;
  } 
  else TRISBbits.TRISB6 = 1;

  if (inf.leds.l1) //-RB7 
  {
   LATBbits.LATB7 = 0;
   TRISBbits.TRISB7 = 0;
  }
  else TRISBbits.TRISB7 = 1;

  if (inf.leds.l2) //-RC0 
  {
   LATCbits.LATC0 = 0;
   TRISCbits.TRISC0 = 0;
  }
  else TRISCbits.TRISC0 = 1;
  
  if (inf.leds.l6) //-RC1 
  {
   LATCbits.LATC1 = 0;
   TRISCbits.TRISC1 = 0;
  }
  else TRISCbits.TRISC1 = 1;

  if (inf.leds.l7) //-RC2 
  {
   LATCbits.LATC2 = 0;
   TRISCbits.TRISC2 = 0;
  }
  else TRISCbits.TRISC2 = 1;

  if (inf.leds.l8) //-RC5 
  {
   LATCbits.LATC5 = 0;
   TRISCbits.TRISC5 = 0;
  }
  else TRISCbits.TRISC5 = 1;

  if (inf.leds.l13) //-RC6 
  {
   LATCbits.LATC6 = 0;
   TRISCbits.TRISC6 = 0;
  }
  else TRISCbits.TRISC6 = 1;

  if (inf.leds.l15) //-RC7
  {
   LATCbits.LATC7 = 0;
   TRISCbits.TRISC7 = 0;
  }
  else TRISCbits.TRISC7 = 1;
 }
 else
 {
  if (inf.leds.l3) //+RB6 
  {
   LATBbits.LATB6 = 1;
   TRISBbits.TRISB6 = 0;
  }
  else TRISBbits.TRISB6 = 1;

  if (inf.leds.l4) //+RB7 
  {
   LATBbits.LATB7 = 1;
   TRISBbits.TRISB7 = 0;
  }
  else TRISBbits.TRISB7 = 1;

  if (inf.leds.l5) //+RC0 
  {
   LATCbits.LATC0 = 1;
   TRISCbits.TRISC0 = 0;
  }
  else TRISCbits.TRISC0 = 1;

  if (inf.leds.l9) //+RC1 
  {
   LATCbits.LATC1 = 1;
   TRISCbits.TRISC1 = 0;
  }
  else TRISCbits.TRISC1 = 1;

  if (inf.leds.l10) //+RC2 
  {
   LATCbits.LATC2 = 1;
   TRISCbits.TRISC2 = 0;
  }
  else TRISCbits.TRISC2 = 1;

  if (inf.leds.l11) //+RC5 
  {
   LATCbits.LATC5 = 1;
   TRISCbits.TRISC5 = 0;
  }
  else TRISCbits.TRISC5 = 1;

  if (inf.leds.l12) //+RC6 
  {
   LATCbits.LATC6 = 1;
   TRISCbits.TRISC6 = 0;
  }
  else TRISCbits.TRISC6 = 1;

  if (inf.leds.l14) //+RC7 
  {
   LATCbits.LATC7 = 1;
   TRISCbits.TRISC7 = 0;
  }
  else TRISCbits.TRISC7 = 1;
 } 
#endif 
}

void Press_Button_Callback(enum mtouch_button_names button) 
{ 
 inf.keys.u |= 1<<button;
 #ifdef TEST
 if (button <= Button11)
  {
   printf("Press %d %4x\n\r", button, inf.keys.u);
   //LATCbits.LATC1 = 1;
   TEST1(1);
  } 
 #endif
}

void Release_Button_Callback(enum mtouch_button_names button) 
{ 
 inf.keys.u &= ~(1<<button);
 #ifdef TEST
 if (button <= Button11)
  {
   printf("Release %d %4x\n\r", button, inf.keys.u);
   //LATCbits.LATC1 = 0;
   TEST1(0);
  }
 #endif
}

void Slider_Callback(enum mtouch_slider_names slider)
{
 uint16_t pos = MTOUCH_Slider_Position_Get(slider);
 inf.keys.slider = pos&255;
 #ifdef TEST
 printf("Slider %d\n\r", pos);
 #endif
}

void main(void)
{
 // Initialize the device
 GetDev();
 SYSTEM_Initialize();
 inf.leds.u = 0;
 inf.keys.u = 0;
 
 // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
 // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
 // Use the following macros to:

 // Enable the Global Interrupts
 //INTERRUPT_GlobalInterruptEnable();

 
 // Enable high priority global interrupts
 INTERRUPT_GlobalInterruptHighEnable();

 // Enable low priority global interrupts.
 INTERRUPT_GlobalInterruptLowEnable();

 // Enable the Peripheral Interrupts
 INTERRUPT_PeripheralInterruptEnable();

 // Disable the Peripheral Interrupts
 //INTERRUPT_PeripheralInterruptDisable();

 MTOUCH_Button_SetPressedCallback(Press_Button_Callback);
 MTOUCH_Button_SetNotPressedCallback(Release_Button_Callback);
 MTOUCH_Slider_SetPositionChangedCallback(Slider_Callback);
 TEST2(1);
 while (1)
 {
   ClrWdt();
   MTOUCH_Service_Mainloop();
     // Add your application code
   keys_to_leds();
 }
}
/**
 End of File
*/