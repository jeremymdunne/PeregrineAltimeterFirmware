#include <TelemetryModule.h> 


TelemetryModuleStatus_t TelemetryModule::begin(RocketState *state){
    // store the state pointer for future reference 
    _rocket_state = state; 
    // start the SerialHandler 
    _serial_handler.begin(); 
    
    _status = initialize_flash_storage(); 
    return _status; 
}

TelemetryModuleStatus_t TelemetryModule::update(){
    // for now, just update the serial handler 
    _serial_handler.update(); 
    return TELEMETRY_MODULE_OK; 
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

TelemetryModuleStatus_t TelemetryModule::send_error_string(char * buffer, int length, TelemetryMessageMedium_t medium){
    // send the string through the medium 
    switch(medium){
        case(TELEMETRY_SERIAL):
            // package the message 
            byte out_buffer[length + 1]; 
            out_buffer[0] = COMMUNICATION_ERROR_MESSAGE_FLAG; 
            memcpy(&out_buffer[1], buffer, length); 
            // send the message 
            _serial_handler.send_message(out_buffer,length+1); 
    }
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}

int TelemetryModule::get_file_list_buff(byte * buffer){
    // convert the file list to a string 
    JFAT::FileList file_list; 
    _flash_storage.read_fat(&file_list); 
    // first write the number of files 
    buffer[0] = file_list.num_files >> 8; 
    buffer[1] = (byte)file_list.num_files; 
    int pntr = 2; 
    for(int i = 0; i < file_list.num_files; i ++){
        int length = file_list.page_length[i]; 
        buffer[pntr] = (byte)(length >> 8); 
        buffer[pntr+1] = (byte)(length); 
        pntr += 2; 
    }
    return pntr; 
}

TelemetryModuleStatus_t TelemetryModule::send_file_list(){
    // compose the message and send it through the serial buffer 
    byte buffer[512]; 
    buffer[0] = COMMUNICATION_FILE_LIST_FLAG; // first byte is always the flag 
    int len = get_file_list_buff(&buffer[1]); 
    len ++; // account for the flag 
    // send over serial 
    _serial_handler.send_message(buffer,len); 
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}

void TelemetryModule::delete_all_files(){
    _flash_storage.delete_all_files(); 
}

TelemetryModuleStatus_t TelemetryModule::initialize_flash_storage(){
    // initalize the flash object 
    int flash_status = _flash_storage.begin(FLASH_STORAGE_CS_PIN); 
    if(flash_status != 0){
        // error initalize the flash object 
        _status = TELEMETRY_MODULE_STORAGE_COMMUNICATION_FAIL; 
        return _status; 
    }
    // delete all files 
    delete_all_files();
    // at least on file must be found 
    JFAT::FileList file_list; 
    _flash_storage.read_fat(&file_list); 
    if(file_list.num_files < 2){
        // create a mock file 
        _flash_storage.open_file_write(); 
        _flash_storage.close(); 
    }
    _flash_storage.open_file_write(); 
    _flash_storage.close(); 
    // return success 
    _status = TELEMETRY_MODULE_OK; 
    return _status; 
}


void TelemetryModule::light_error_led(){
    pinMode(HEART_BEAT_LED, OUTPUT); 
    digitalWrite(HEART_BEAT_LED, LOW); 
}