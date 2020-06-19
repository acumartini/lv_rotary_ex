
#include <Arduino.h>

#include "display.h"

using namespace display;

void setup()
{
  Serial.begin(115200);

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
      Serial.println();
      Serial.print("Value: ");
    }
    
    delay(2);
  }
}
