#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bit.h"

# define CS00 0

#define FOSC 16000000 // Frequency Oscillator 16Mhz for Uno R3
#define BAUD 9600 // 9600 Bits per second
#define MYUBRR FOSC / 16 / BAUD - 1 // My USART Baud Rate Register

int setPrescaler_tc0(char option);
//void my_delay_ms(unsigned long x);
void my_delay_1e6us();
volatile unsigned long num0V = 0;

// delay of 1 second with default prescaler
void my_delay_1e6us()
{
  unsigned long num0V_max = 62500;
  num0V = 0; // timer0 overflow counter, sets count to 0
  TCNT0 = 0; // timer0 counter register, sets count to 0
  //bitSet(TCCR0B, CS00); // starts timer0
  setPrescaler_tc0(2);
  // repeatedly counts to 255 and overflows triggering the ISR to increment.
  while (num0V < num0V_max - 1); 
  //bitClear(TCCR0B, CS00); // stops timer0
  setPrescaler_tc0(0);
}


// void my_delay_ms(unsigned long x)
// {
//   for (unsigned long i = 0; i < x; i++)
//   {
//     my_delay_1e6us();
//   }
// }


int setPrescaler_tc0(char option)
{
  switch (option)
  {
  case 0:
    // no clock source 
    bitClear(TCCR0B, CS00);
    bitClear(TCCR0B, CS01);
    bitClear(TCCR0B, CS02);
    break;
  case 1:
    // full clock: 16 Mhz = 62.5 ns
    bitSet(TCCR0B, CS00);
    bitClear(TCCR0B, CS01);
    bitClear(TCCR0B, CS02);
    break;
  case 2:
    // 1/8 clock: 2 Mhz = 500 ns
    bitClear(TCCR0B, CS00);
    bitSet(TCCR0B, CS01);
    bitClear(TCCR0B, CS02);
    break;
  case 3:
    // 1/64 clock: 250 kHz = 4 us
    bitSet(TCCR0B, CS00);
    bitSet(TCCR0B, CS01);
    bitClear(TCCR0B, CS02);
    break;
  case 4:
    // 1/256 clock: 62.5 kHz = 16 us
    bitClear(TCCR0B, CS00);
    bitClear(TCCR0B, CS01);
    bitSet(TCCR0B, CS02);
    break;
  case 5:
    // 1/1024 clock: 15.625 kHz = 64 us
    bitSet(TCCR0B, CS00);
    bitClear(TCCR0B, CS01);
    bitSet(TCCR0B, CS02);
    break;
  default:
    return -1;
  }
  return 0;
}

ISR(TIMER0_OVF_vect)
{
  num0V++;
}

int main()
{
  usart_init(MYUBRR); // 103-9600 bps; 8-115200
  bitSet(TIMSK0, TOIE0); // enable timer0 overflow interrupt
  sei(); // enable global interrupts
  while (1)
  {
    usart_tx_string(">a:");
    usart_transmit('1');
    usart_transmit('\n');

    my_delay_1e6us();

    usart_tx_string(">a:");
    usart_transmit('0');
    usart_transmit('\n');

    my_delay_1e6us();
  }
}
