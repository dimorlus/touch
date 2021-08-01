/**
  Generated Interrupt Manager Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.c

  @Summary:
    This is the Interrupt Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description:
    This header file provides implementations for global interrupt handling.
    For individual peripheral handlers please see the peripheral driver for
    all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC18F24K40
        Driver Version    :  2.03
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45 or later
        MPLAB 	          :  MPLAB X 4.15
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "interrupt_manager.h"
#include "mcc.h"
#include "../common.h"


void  INTERRUPT_Initialize (void)
{
    // Enable Interrupt Priority Vectors
    INTCONbits.IPEN = 1;

    // Assign peripheral interrupt priority vectors

    // SSPI - high priority
    IPR3bits.SSP1IP = 1;


    // TMRI - low priority
    IPR0bits.TMR0IP = 0;    

    // ADTI - low priority
    IPR1bits.ADTIP = 0;    

    // TXI - low priority
    IPR3bits.TXIP = 0;    

    // RCI - low priority
    IPR3bits.RCIP = 0;    

}

void __interrupt() INTERRUPT_InterruptManagerHigh (void)
{
   // interrupt handler
    if(PIE3bits.SSP1IE == 1 && PIR3bits.SSP1IF == 1)
    {
        I2C1_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow (void)
{
    // interrupt handler
    if(PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    else if(PIE1bits.ADTIE == 1 && PIR1bits.ADTIF == 1)
    {
        ADCC_ThresholdISR_mTouch();
    }
    else
#ifdef TEST 
    if(PIE3bits.TXIE == 1 && PIR3bits.TXIF == 1)
    {
        EUSART_TxDefaultInterruptHandler();
    }
    else 
    if(PIE3bits.RCIE == 1 && PIR3bits.RCIF == 1)
    {
        EUSART_RxDefaultInterruptHandler();
    }
    else
#endif     
    {
        //Unhandled Interrupt
    }
}
/**
 End of File
*/
