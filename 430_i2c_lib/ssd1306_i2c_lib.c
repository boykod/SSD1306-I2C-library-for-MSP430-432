//******************************************************************************************************************************************
//                 MSP430FR2433
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |                 |
//            |                 |
//            |         SDA/P1.2|------->
//            |         SCK/P1.3|------->
//******************************************************************************************************************************************
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306_i2c_lib.h"

volatile uint32_t i;
uint8_t SlaveAddress = 0x3C;
uint8_t TXByteCtr;
uint8_t SlaveFlag = 0;
unsigned char data [2];
unsigned char *TI_transmit_field;
unsigned char *dataBuffer;

//******************************************************************************************************************************************
void i2c_init () {
    P1OUT &= ~BIT3;
    P1DIR|=BIT2 | BIT3;

    // Configure Pins for I2C
    P1SEL0 |= BIT2 | BIT3;                            // I2C pins

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                             // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST;                    // I2C master mode, SMCLK
    UCB0BRW = 0x8;                                    // baudrate = SMCLK / 8
    UCB0CTLW0 &=~ UCSWRST;                            // clear reset register
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable

    __enable_interrupt();
}
//******************************************************************************************************************************************
void i2c_transmit (unsigned char *params, unsigned char flag) {

    TI_transmit_field = params;

    __delay_cycles(5000);
    UCB0I2CSA = SlaveAddress;              // configure slave address
    TXByteCtr = flag;                                    // Load TX byte counter
    while (UCB0CTLW0 & UCTXSTP);                      // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;                      // I2C TX, start condition

    __delay_cycles(2500);

}
//******************************************************************************************************************************************
// I2C interrupt service routine
#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void) {
  switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG)) {
  case USCI_I2C_UCNACKIFG:
      UCB0CTL1 |= UCTXSTT;                      //resend start if NACK
      break;                                      // Vector 4: NACKIFG break;
  case USCI_I2C_UCTXIFG0:
      if (TXByteCtr)  {                              // Check TX byte counter
          UCB0TXBUF = *TI_transmit_field;
          TI_transmit_field++;
          TXByteCtr--;                              // Decrement TX byte counter
      } else {
          UCB0CTLW0 |= UCTXSTP;                     // I2C stop condition
          UCB0IFG &= ~UCTXIFG;                      // Clear USCI_B0 TX int flag
      }
      break;                                      // Vector 26: TXIFG0 break;
  }
}
