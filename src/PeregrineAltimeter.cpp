#include <PeregrineAltimeter.h> 


int PeregrineAltimeter::run(){
    // main function of the altimeter 
    init_modules(); 


    _telemetry.light_error_led();
    // Testing purposes, send out strings 
    char buffer[] = "Hello, World!"; 
    // grab the length of the buffer 
    int cntr = 0; 
    bool done = false; 
    while(!done){
        if(buffer[cntr] == '\0'){
            done = true; 
        }
        else cntr ++; 
    }
    // send the message in a loop 
    _telemetry.start_recording(); 
    // run for a bit 
    long start = millis(); 
    while(millis() - start < 10000){
        _kinematics.update(); 
        _telemetry.update(); 
    }
    // stop the telemetry to let it store the data 
    _telemetry.stop(); 
    _telemetry.send_file_list(); 
    _telemetry.send_last_file(); 
    return 0; 
    while(true){
        delay(2000); 
        _kinematics.update(); 
        _telemetry.update(); 
        _telemetry.send_file_list(); 
       
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