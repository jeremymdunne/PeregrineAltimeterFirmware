#include <UpdateTimer.h> 

UpdateTimerStatus_t UpdateTimer::update(){
    // check if running 
    if(!_running){
        return UPDATE_TIMER_NO_UPDATE; 
    }
    // check the elapsed time 
    unsigned long cur_time = millis(); 
    if(cur_time - _last_exec_time > _duration){
        // time elapsed
        _last_exec_time = _last_exec_time + _duration; // done to keep the duration from 'moving around' 
        return UPDATE_TIMER_UPDATE_AVAILABLE; 
    }
    return UPDATE_TIMER_NO_UPDATE; 
}

void UpdateTimer::set_timer_frequency(float freq){
    // convert to a frequency 
    _duration = (unsigned long)(1000.0/freq + 0.5);  
    _duration_set = true; 
}

void UpdateTimer::set_timer_duration(unsigned long duration){
    _duration = duration; 
    _duration_set = true; 
}

UpdateTimerStatus_t UpdateTimer::start_timer(){
    if(!_duration_set){
        return UPDATE_TIMER_NO_TIMER_SET; 
    }
    _last_exec_time = millis(); 
    _running = true; 
    return UPDATE_TIMER_OK; 
}

void UpdateTimer::pause_timer(){
    _running = false; 
}



