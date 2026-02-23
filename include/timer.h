#ifndef TIMER_H
#define TIMER_H

#include "config.h"

#ifndef DISABLE_TIMER_FEATURE
// Timer control functions
void startTimer();
void pauseTimer();
void stopTimer();
void resetTimer();
void setTimerDuration(int hours, int minutes, int seconds);
void getTimerDisplay(char* buffer, size_t size);

// Timer update function (called from main loop)
void updateTimer();

// Display formatting
void displayTimer(bool withAnimation);

// Timer completion handling
void handleTimerCompletion();

// Get timer status as JSON
String getTimerStatusJson();

// Timer Modes
#define TIMER_MODE_COUNTDOWN 0
#define TIMER_MODE_STOPWATCH 1

// Timer States for non-blocking operation
enum TimerState {
  TIMER_STOPPED,
  TIMER_RUNNING,
  TIMER_PAUSED,
  TIMER_COMPLETION_ALERT,  // Buzzer pulsing
  TIMER_COMPLETION_DONE    // Flashing "DONE"
};
extern TimerState currentTimerState;
#endif // DISABLE_TIMER_FEATURE

#endif // TIMER_H
