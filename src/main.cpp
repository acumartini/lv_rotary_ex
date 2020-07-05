
#include <Arduino.h>

#include "display.h"
#include "timer.h"

using namespace display;

void slowISR(void);

void setup()
{
  Serial.begin(115200);

  TC.startTimer3(1000, slowISR); // slow ISR rate of 1 msec == 1000 Hz
  
  DC.begin();
  DC.render();
}

void loop()
{
  uint32_t heartBeat = 0; // used to blink LED as running indicator

  while (1)
  {
    lv_task_handler();

    if ((++heartBeat %= 600) == 0)
    {
    }
    
    delay(2);
  }
}

void slowISR()
{
  lv_tick_inc(1);
}
