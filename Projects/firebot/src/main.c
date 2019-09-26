// -------------------------------------------------------------------------------

// Autores:

// Caio Alessandro Resnauer
// Erik Ryuichi Yamamoto
// Victor Jordão

// -------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"

#include "system_TM4C1294.h"

#include "gpio.h"
#include "UARTInit.h"
#include "timerInit.h"

#define BIT0 (0x1)

#define BIT1 (0x2)
#define BITS_1_0 (0x3) // 2 + 1 = 3 = 0x3

#define BIT2 (0x4)
#define BITS_2_0 (0x5) // 4 + 1 = 5 = 0x5
#define BITS_2_1 (0x6) // 4 + 2 = 6 = 0x6
#define BITS_2_1_0 (0x7) // 4 + 2 + 1 = 7 = 0x7

#define BIT3 (0x8)
#define BITS_3_0 (0x9) // 8 + 1 = 9 = 0x9
#define BITS_3_1 (0xA) // 8 + 2 = 10 = 0xA
#define BITS_3_1_0 (0xB) // 8 + 2 + 1 = 11 = 0xB
#define BITS_3_2 (0xC) // 8 + 4 = 12 = 0xC
#define BITS_3_2_0 (0xD) // 8 + 4 + 1 = 13 = 0xD
#define BITS_3_2_1 (0xE) // 8 + 4 + 2 = 14 = 0xE
#define BITS_3_2_1_0 (0xF) // 8 + 4 + 2 + 1 = 15 = 0xF

#define BIT4 (0x10)

#define BIT5 (0x20)
#define BITS_5_4 (0x30) // 2 0 + 1 0 = 3 0 = 0x30

#define BIT6 (0x40)
#define BITS_6_4 (0x50) // 4 0 + 1 0 = 5 0 = 0x50
#define BITS_6_5 (0x60) // 4 0 + 2 0 = 6 0 = 0x60
#define BITS_6_5_4 (0x70) // 4 0 + 2 0 + 1 0 = 7 0 = 0x70

#define BIT7 (0x80)
#define BITS_7_4 (0x90) // 8 0 + 1 0 = 9 0 = 0x90
#define BITS_7_5 (0xA0) // 8 0 + 2 0 = 10 0 = 0xA0
#define BITS_7_5_4 (0xB0) // 8 0 + 2 0 + 1 0 = 11 0 = 0xB0
#define BITS_7_6 (0xC0) // 8 0 + 4 0 = 12 0 = 0xC0
#define BITS_7_6_4 (0xD0) // 8 0 + 4 0 + 1 0 = 13 0 = 0xD0
#define BITS_7_6_5 (0xE0) // 8 0 + 4 0 + 2 0 = 14 0 = 0xE0
#define BITS_7_6_5_4 (0xF0) // 8 0 + 4 0 + 2 0 + 1 0 = 15 0 = 0xF0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void main(void){
  GPIO_Init();
  UARTInit();
  TimerInit();
  PortM_Output(BIT6);
  
  uint64_t valorTimer;
  uint64_t valorA, valorB;
  
  while(1){
    valorTimer = TimerValueGet64(TIMER0_BASE);
    valorA = valorTimer / 0x10000;
    valorB = valorTimer % 0x10000;
    UARTprintf("Valor: 0x%x%x\n", valorA, valorB);
    PortA_Output(valorTimer);
  }
} // main
*/

#define SERVO_PERIOD 20000

uint32_t pulse, period_ns, clock_MHz, delay, servoPulse;
uint64_t pulseTimeWidth;
int32_t angle;

void ServoWrite(uint32_t time) {
    //pulseTimeWidth = (((90 + angle) * 6 + 1000)) * period_ns;
  pulseTimeWidth = time * period_ns;
  TimerMatchSet(TIMER0_BASE, TIMER_A, pulseTimeWidth);
}

void delayMS(uint32_t ms) {
    SysCtlDelay(ms) ;
}

void SysTick_Handler(void){
  delay++;
} // SysTick_Handler

void ServoInit(void) {
  angle = 0;
  servoPulse = 1000;
  ServoWrite(servoPulse);
  delay = 0;

  SysTickPeriodSet(24000);
  SysTickIntEnable();
  SysTickEnable();
  
  while(delay < 1000) {
    
  }
  
  SysTickDisable();
}

int main(void) {
  char command;
  
  GPIO_Init();
  UARTInit();
  PortM_Output(BIT6);
  
  clock_MHz = (SystemCoreClock / 1000000);
  period_ns = 1000 / clock_MHz;
  //pulseTimeWidth = servoPulse * period_ns;
  delay = 0;

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
  GPIOPinConfigure(GPIO_PL4_T0CCP0);
  GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_4);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
  TimerControlLevel(TIMER0_BASE, TIMER_A, true);
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 10);
  TimerLoadSet(TIMER0_BASE, TIMER_A, SERVO_PERIOD * period_ns);
  //TimerMatchSet(TIMER0_BASE, TIMER_A, pulseTimeWidth);
  ServoInit();
  TimerEnable(TIMER0_BASE, TIMER_A);
  
  

  while(1){
    command = UARTgetc();
    
    if(command == 'u') {
      TimerEnable(TIMER0_BASE, TIMER_A);
      ServoWrite(2000);
      delay = 0;
      
      SysTickPeriodSet(24000);
      SysTickIntEnable();
      SysTickEnable();
      
      while(delay < 1000) {
        
      }
      
      SysTickDisable();
    }
    
    else if(command == 'c') {
      TimerEnable(TIMER0_BASE, TIMER_A);
      ServoWrite(1000);
      delay = 0;
      
      SysTickPeriodSet(24000);
      SysTickIntEnable();
      SysTickEnable();
      
      while(delay < 1000) {
        
      }
      
      SysTickDisable();
    }
    
    else if(command == 'd') {
      TimerEnable(TIMER0_BASE, TIMER_A);
      ServoWrite(1500);
      delay = 0;
      
      SysTickPeriodSet(24000);
      SysTickIntEnable();
      SysTickEnable();
      
      while(delay < 1000) {
        
      }
      
      SysTickDisable();
    }
    
    else {
      ServoWrite(0);
      delay = 0;
      TimerDisable(TIMER0_BASE, TIMER_A);
    }
  }
}