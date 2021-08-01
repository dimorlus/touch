/* 
 * File:   common.h
 * Author: Dmitry Orlov
 *
 * Created on October 23, 2018, 1:52 PM
 */

#ifndef COMMON_H
#define	COMMON_H

//#define TEST

#ifdef TEST
#define TEST1(x) LATCbits.LATC1 = x
#define TEST2(x) LATCbits.LATC2 = x
#define TEST3(x) LATBbits.LATB7 = x
#else
#define TEST1(x)
#define TEST2(x)
#define TEST3(x)
#endif

#include <stdint.h>
#include "types.h"

typedef union 
 {
  struct
  {
   u8 b0:1;
   u8 b1:1;
   u8 b2:1;
   u8 b3:1;
   u8 b4:1;
   u8 b5:1;
   u8 b6:1;
   u8 b7:1;
   u8 b8:1;
   u8 b9:1;
   u8 b10:1;
   u8 b11:1;
   u8 b12:1;
   u8 b13:1;
   u8 b14:1;
   u8 b15:1;
  };
  struct
  {
   uint16_t s0; 
   uint16_t s1; 
  };       
  struct
  {
   uint16_t buttons; 
   uint8_t spare; 
   uint8_t slider; 
  };       
  struct
   {
    uint8_t b[4];
   };
  struct
   {
    uint32_t u;
   };
 } u32b;

 typedef union
 {
  struct
  {
   u8 l0:1;
   u8 l1:1;
   u8 l2:1;
   u8 l3:1;
   u8 l4:1;
   u8 l5:1;
   u8 l6:1;
   u8 l7:1;
   u8 l8:1;
   u8 l9:1;
   u8 l10:1;
   u8 l11:1;
   u8 l12:1;
   u8 l13:1;
   u8 l14:1;
   u8 l15:1;
   u8 l16:1;
   u8 l17:1;
   u8 l18:1;
   u8 l19:1;
   u8 l20:1;
   u8 l21:1;
   u8 l22:1;
   u8 l23:1;
  };
  struct
  {
   u32 u;
  };        
 }tld;
 
 typedef union
 {
   struct
   {
    u32b keys;
    tld leds;
   };
   struct
   {
    uint8_t b[8];
   };
 }tinf;

 
//extern tld leds; 
//extern u32b keys;

extern tinf inf;
extern volatile uint8_t _1ms_c;
extern volatile uint16_t _iic_timeout_ms;
extern void fleds(void);
 
#endif	/* COMMON_H */

