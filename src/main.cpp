#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include <avr/interrupt.h>

//#include "usart.h"
//#include "bit.h"
// bit register manipulation
#define bitSet(reg, n) (reg |= 1 << n)
#define bitRead(reg, n) (reg & (1 << n))
#define bitClear(reg, n) (reg &= ~(1 << n))

# define CS00 0

// delay of 1 second
void my_delay_1e6us()
{
  unsigned long num0V_max = 62500;

  num0V = 0;
  TCNT0 = 0;
  bitSet(TCCR0B, CS00);
  while (num0V < num0V_max - 1);
  bitClear(TCCR0B, CS00);
}

int setPrescaler_tc0(char option);
void my_delay_ms(unsigned long x);

void my_delay_1e6us();

volatile unsigned long num0V = 0;

ISR(TIMER0_OVF_vect)
{
  num0V++;
}

int main()
{
  //usart_init(8); // 103-9600 bps; 8-115200

  bitSet(TIMSK0, TOIE0); // enable timer0 overflow interrupt
  sei();
}
