#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"

#include "system_TM4C1294.h"

#include "timerInit.h"
#include "gpio.h"

int estado = 0;
uint64_t valorServo = 0x8B82;

void TimerInit(void){
  // Enable the GPIO Peripheral used by the Timer.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));

  // Enable Timer0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

  // Configure GPIO Pins for Timer mode.
  GPIOPinConfigure(GPIO_PL4_T0CCP0);
  GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_4);

  // Configure the Timer.
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet64(TIMER0_BASE, valorServo);
  TimerIntRegister(TIMER0_BASE, TIMER_A, TIMER0A_Handler);
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER0_BASE, TIMER_A);
} // TimerInit
/*
void TIMER0A_Handler(void) {
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  TimerDisable(TIMER0_BASE, TIMER_A);
  
  if(estado == 0) {
    TimerLoadSet64(TIMER0_BASE, 0x0007441E - valorServo);
    estado = 1;
    //PortL_Output(0);
  }
  
  else {
    TimerLoadSet64(TIMER0_BASE, valorServo);
    estado = 0;
    //PortL_Output(0xFFFF);
  }
  
  TimerEnable(TIMER0_BASE, TIMER_A);
}
*/