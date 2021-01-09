


#ifndef _UPDATE_TIMER_H_
#define _UPDATE_TIMER_H_

#include <Arduino.h> 

typedef enum{
    UPDATE_TIMER_OK = 0,                    //< No issues 
    UPDATE_TIMER_NO_UPDATE,                 //< No update yet available (timer has not elapsed)
    UPDATE_TIMER_UPDATE_AVAILABLE,          //< Update Available (timer has elapsed)
    UPDATE_TIMER_PAUSED,                    //< Timer paused 
    UPDATE_TIMER_NO_TIMER_SET               //< No timer set 
}   UpdateTimerStatus_t; 

class UpdateTimer{
public: 

    /**
     * update the timer, check if the timer has elapsed 
     * 
     * @return status code 
     */
    UpdateTimerStatus_t update(); 

    /**
     * set the timer frequency 
     * note: frequency will be changed to milliseconds  
     * @param freq frequency (hz) to set the updater to
     */
    void set_timer_frequency(float freq); 

    /**
     * set the timer elapsed time 
     * @param timer_duration time (in milliseconds) to set the updater to 
     */ 
    void set_timer_duration(unsigned long duration); 

    /**
     * start the timer 
     * 
     * @return status code 
     */ 
    UpdateTimerStatus_t start_timer(); 

    /**
     * pause the timer 
     */ 
    void pause_timer(); 

private: 
    UpdateTimerStatus_t _status;                //< most recent status 
    unsigned long _duration;                    //< duration for the timer 
    unsigned long _last_exec_time;              //< last time the update was called 
    bool _running = false;                      //< flag to tell if the timer should be running 
    bool _duration_set = false;                 //< flag to determine if the duration is set 

};



#endif 