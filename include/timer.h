
/* 
 * File:   timer.h
 * Author: Neal Martini
 *
 * Created on March 6, 2020, 8:00 AM
 */
#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
  void startTimer3(unsigned long period, void (*f)());
  void setPeriodTimer3(unsigned long period);
  void startTimer4(unsigned long period, void (*f)());
  void setPeriodTimer4(unsigned long period);
};

extern Timer TC;

#endif /* TIMER_H */