#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bit.h"

#define FOSC 16000000 // Frequency Oscillator 16Mhz for Uno R3
#define BAUD 9600 // 9600 Bits per second
#define MYUBRR FOSC / 16 / BAUD - 1 // My USART Baud Rate Register
#define PRESCALER 3 //250 KHz
#define DELAY 200 // 200 ms

#define PORT &PORTB
#define DDR &DDRB
#define PIN PINB0

int setPrescaler_tc0(char option);
int setMax_count_tc0(int num);
void my_delay_ms(unsigned long x);
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
  // repeatedly counts to 255 and overflows at 256 triggering the ISR to increment.
  while (num0V < num0V_max - 1); 
  //bitClear(TCCR0B, CS00); // stops timer0
  setPrescaler_tc0(0);
}

int setMax_count_tc0(int num)
{
  int prescalers[6] = {0, 1, 8, 64, 256, 1024};
  float overflow_count = ((1 / ((float)FOSC / prescalers[PRESCALER])) * 256);
  float time_sec = (float)num / 1000;
  float max = (time_sec / overflow_count);
  int max_count = (int)round(max);
  return max_count;
}

// custom delay with prescalers
void my_delay_ms(unsigned long x)
{
  unsigned long num0V_max = setMax_count_tc0(x);
  num0V = 0; // timer0 overflow counter, sets count to 0
  TCNT0 = 0; // timer0 counter register, sets count to 0
  setPrescaler_tc0(PRESCALER);
  while (num0V < num0V_max - 1); 
  
  setPrescaler_tc0(0);
}


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
  return option;
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
  bitSet(*DDR, PIN); // set pin 8 as output


  while (1)
  {
    bitSet(*PORT, PIN);
    // usart_tx_string("a:>");
    // usart_transmit('1');
    // usart_transmit('\n');
    my_delay_ms(DELAY);
    bitClear(*PORT, PIN);
    // usart_tx_string("a:>");
    // usart_transmit('0');
    // usart_transmit('\n');
    my_delay_ms(DELAY);
  }
}
