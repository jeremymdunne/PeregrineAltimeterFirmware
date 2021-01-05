#include <TelemetryModule.h> 


TelemetryModuleStatus_t TelemetryModule::begin(RocketState *state){
    // store the state pointer for future reference 
    _rocket_state = state; 
    // start the SerialHandler 
    _serial_handler.begin(); 
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}