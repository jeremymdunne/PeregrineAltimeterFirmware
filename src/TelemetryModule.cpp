#include <TelemetryModule.h> 


TelemetryModuleStatus_t TelemetryModule::begin(RocketState *state){
    // store the state pointer for future reference 
    _rocket_state = state; 
    // start the SerialHandler 
    _serial_handler.begin(); 
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}


TelemetryModuleStatus_t TelemetryModule::send_verbose_string(char * buffer, int length, TelemetryMessageMedium_t medium){
    // send the string through the medium 
    switch(medium){
        case(TELEMETRY_SERIAL):
            // package the message 
            byte out_buffer[length + 1]; 
            out_buffer[0] = COMMUNICATION_VERBOSE_MESSAGE_FLAG; 
            memcpy(&out_buffer[1], buffer, length); 
            // send the message 
            _serial_handler.send_message(out_buffer,length+1); 
    }
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}
