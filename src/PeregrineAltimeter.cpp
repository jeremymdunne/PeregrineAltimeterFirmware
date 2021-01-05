#include <PeregrineAltimeter.h> 


int PeregrineAltimeter::run(){
    // main function of the altimeter 
    init_modules(); 

    return 0; 
}

int PeregrineAltimeter::init_modules(){
    // initialize all modules 

    // start with telemetry for error reporting 
    _telemetry.begin(&_state); 
    return 0; 
}