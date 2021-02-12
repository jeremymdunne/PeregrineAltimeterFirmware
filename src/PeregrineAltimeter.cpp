#include <PeregrineAltimeter.h> 


int PeregrineAltimeter::run(){
    // main function of the altimeter 
    delay(2000); 
    init_modules(); 


    _telemetry.light_error_led();
    // Testing purposes, send out strings 
    char buffer[] = "Hello, World!"; 
    _telemetry.send_verbose_string(buffer); 
    // send the message in a loop 
    /*
    _telemetry.start_recording(); 
    // run for a bit 
    long start = millis(); 
    _state._flight_phase = BOOST_PHASE; 
    while(millis() - start < 10000){
        // record the flight time 
        _state._flight_time = millis() - start; 
        _kinematics.update(); 
        _telemetry.update(); 
    }
    // stop the telemetry to l=et it store the data 
    _telemetry.stop(); 
    _telemetry.send_file_list(); 
    _telemetry.send_last_file(); 
    */ 
   unsigned long last_time = millis(); 
    while(true){
        // update the time 
        _state._sys_time = millis(); 
        // check if flight time is legit 
        if(_state._flight_phase != WAITING_FOR_LAUNCH_PHASE){
            _state._flight_time = _state._sys_time - _state._launch_time; 
        }

        // update modules 
        // delay(2000); 
        _kinematics.update(); 
        _telemetry.update(); 
        delay(5);
        // print out the loop time 
        long loop_time = millis() - _state._sys_time; 
        char buffer[64] = "Loop Time: ";  
        char time[10]; 
        itoa(loop_time, time, 10); 
        strcat(buffer, time); 
        char end[] = "ms"; 
        strcat(buffer, end); 
        _telemetry.send_verbose_string(buffer); 
       
    }
    return 0; 
}

int PeregrineAltimeter::init_modules(){
    // initialize all modules 

    // start with telemetry for error reporting 
    TelemetryModuleStatus_t stat = _telemetry.begin(&_state); 
    if(stat != TELEMETRY_MODULE_OK){
        _telemetry.light_error_led();
    }
    KinematicsStatus_t kin_status = _kinematics.begin(&_state);
    
    return 0; 
}