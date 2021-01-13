#include <TelemetryModule.h> 


TelemetryModuleStatus_t TelemetryModule::begin(RocketState *state){
    // store the state pointer for future reference 
    _rocket_state = state; 
    // start the SerialHandler 
    _serial_handler.begin(); 
    
    _status = initialize_flash_storage(); 
    return _status; 
}

TelemetryModuleStatus_t TelemetryModule::stop(){
    // TODO handle a failed-flight stop command (i.e. preflight buffer never written, close out the file with no data written)
    _storage_state = STORAGE_DATA_STORAGE_COMPLETE; 
    // close out the file 
    _flash_storage.close(); 
    // stop the timers for completeness 
    _general_flight_data_timer.pause_timer(); 
    // return an ok 
    return TELEMETRY_MODULE_OK; 
}

TelemetryModuleStatus_t TelemetryModule::start_recording(){
    // open a file to record 
    int flash_status = _flash_storage.open_file_write();  // TODO check the status code 
    // start the timers 
    _general_flight_data_timer.set_timer_frequency(DATA_RECORDING_GENERAL_FLIGHT_DATA_FREQUENCY); 
    _general_flight_data_timer.start_timer(); 
    // TODO add other timers here 

    // set the flag for data storage to be run 
    _storage_state = STORAGE_STORE_DATA; 

    return TELEMETRY_MODULE_OK; 
}

TelemetryModuleStatus_t TelemetryModule::update(){
    if(_storage_state == STORAGE_STORE_DATA){
        // first update timers 
        TelemetryModuleStatus_t update_status = update_storage_data(); 
        // TODO handle 
    }
    // check the serial handler 
    SerialHandlerStatus_t ser_status = _serial_handler.update(); 
    // TODO handle serial messages here 


    return TELEMETRY_MODULE_OK; 
}

TelemetryModuleStatus_t TelemetryModule::update_storage_data(){
    // two main states, preflight and normal flight recording 
    // check the flight phase to determine which to use 
    if(_rocket_state->_flight_phase == WAITING_FOR_LAUNCH_PHASE){
        // no data directly stored onto the computer, instead buffer data as necessary 
        // TODO handle this properly 
    }
    else{ 
        // check all timers 
        if(_general_flight_data_timer.update() == UPDATE_TIMER_UPDATE_AVAILABLE){
            // encode the data and store it 
            byte buffer[STORAGE_GENERAL_FLIGHT_STORAGE_LENGTH]; 
            struct StorableData general_data = {STORAGE_GENERAL_FLIGHT_STORAGE_FLAG, _rocket_state->_flight_time, STORAGE_GENERAL_FLIGHT_STORAGE_LENGTH, buffer }; 
            
        }
    }
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


TelemetryModuleStatus_t TelemetryModule::store_data(StorableData *data){
    // TODO check if the flash storage is actually in write mode 
    // write the data to the storage 
    // construct the buffer to be directly written to storage 
    byte buffer[STORAGE_DATA_HEADER_LENGTH + data->length]; 
    // handle storing time loops 
    while(_last_time_update + 65536 < data->time_stamp){
        // store a time loop 
        store_time_loop(); 
        _last_time_update += 65536; 
    }
    // encode the header 
    encode_header(data->data_flag, data->time_stamp - _last_time_update, data->length, buffer); 
    // write the rest of the message 
    memcpy(&buffer[STORAGE_DATA_HEADER_LENGTH], data->data, data->length); 
    // write to storage 
    _flash_storage.write(buffer, STORAGE_DATA_HEADER_LENGTH + data->length); 
    return TELEMETRY_MODULE_OK; 
}

void TelemetryModule::encode_header(int flag, unsigned long time_stamp, int data_length, byte * buff){
    buff[0] = flag; 
    buff[1] = (byte)(time_stamp >> 8);
    buff[2] = (byte)(time_stamp);
    buff[3] = (byte)(data_length);   
}

void TelemetryModule::store_time_loop(){
    // construct the message 
    byte buff[STORAGE_DATA_HEADER_LENGTH]; 
    encode_header(STORAGE_TIME_LOOP_FLAG, 0, 0, buff); 
    // write the message 
    _flash_storage.write(buff, STORAGE_DATA_HEADER_LENGTH); 
}


void TelemetryModule::encode_general_flight_data(StorableData *data){
    // encode based on the data 
    int pntr = 0; 
    // accel 
    for(int i = 0; i < 3; i ++){
        convert_float_to_bytes(_rocket_state->_acceration[i], &data->data[pntr], STORAGE_GENERAL_FLIGHT_ACCEL_RESOLUTION, STORAGE_GENERAL_FLIGHT_ACCEL_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_ACCEL_MAX_VALUE, STORAGE_GENERAL_FLIGHT_ACCEL_MIN_VALUE);  
        pntr += STORAGE_GENERAL_FLIGHT_ACCEL_BYTE_SIZE; 
    }
    // gyro 
    for(int i = 0; i < 3; i ++){
        convert_float_to_bytes(_rocket_state->_rotation_rates[i], &data->data[pntr], STORAGE_GENERAL_FLIGHT_GYRO_RESOLUTION, STORAGE_GENERAL_FLIGHT_GYRO_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_GYRO_MAX_VALUE, STORAGE_GENERAL_FLIGHT_GYRO_MIN_VALUE);  
        pntr += STORAGE_GENERAL_FLIGHT_GYRO_BYTE_SIZE; 
    }
    // mag 
    for(int i = 0; i < 3; i ++){
        convert_float_to_bytes(_rocket_state->_magnetic_strength[i], &data->data[pntr], STORAGE_GENERAL_FLIGHT_MAG_RESOLUTION, STORAGE_GENERAL_FLIGHT_MAG_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_MAG_MAX_VALUE, STORAGE_GENERAL_FLIGHT_MAG_MIN_VALUE);  
        pntr += STORAGE_GENERAL_FLIGHT_MAG_BYTE_SIZE; 
    }
    // pressure 
    convert_float_to_bytes(_rocket_state->_pressure, &data->data[pntr], STORAGE_GENERAL_FLIGHT_PRESSURE_RESOLUTION, STORAGE_GENERAL_FLIGHT_PRESSURE_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_PRESSURE_MAX_VALUE, STORAGE_GENERAL_FLIGHT_PRESSURE_MIN_VALUE);
    pntr += STORAGE_GENERAL_FLIGHT_PRESSURE_BYTE_SIZE; 
    // temperature 
    convert_float_to_bytes(_rocket_state->_temperature, &data->data[pntr], STORAGE_GENERAL_FLIGHT_TEMPERATURE_RESOLUTION, STORAGE_GENERAL_FLIGHT_TEMPERATURE_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_TEMPERATURE_MAX_VALUE, STORAGE_GENERAL_FLIGHT_TEMPERATURE_MIN_VALUE);
    pntr += STORAGE_GENERAL_FLIGHT_TEMPERATURE_BYTE_SIZE; 
    // altitude  
    convert_float_to_bytes(_rocket_state->_fused_position[2], &data->data[pntr], STORAGE_GENERAL_FLIGHT_ALT_RESOLUTION, STORAGE_GENERAL_FLIGHT_ALT_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_ALT_MAX_VALUE, STORAGE_GENERAL_FLIGHT_ALT_MIN_VALUE);
    pntr += STORAGE_GENERAL_FLIGHT_ALT_BYTE_SIZE; 
    // velocity 
    convert_float_to_bytes(_rocket_state->_fused_velocity[2], &data->data[pntr], STORAGE_GENERAL_FLIGHT_VEL_RESOLUTION, STORAGE_GENERAL_FLIGHT_VEL_BYTE_SIZE, STORAGE_GENERAL_FLIGHT_VEL_MAX_VALUE, STORAGE_GENERAL_FLIGHT_VEL_MIN_VALUE);
    pntr += STORAGE_GENERAL_FLIGHT_VEL_BYTE_SIZE; 
}

void TelemetryModule::convert_float_to_bytes(float data, byte * buff, float resolution, int byte_size, float max_value, float min_value){
    // first check the data is in range 
    if(data > max_value) data = max_value; 
    else if(data < min_value) data = min_value; 
    // convert to dist from min 
    data = data - min_value; 
    // scale 
    unsigned long value = 0.5 + data / resolution; 
    // store in the buffer 
    for(int i = 0; i < byte_size; i ++){
        buff[i] = (byte)(value >> (8 * (byte_size - i - 1)));  
    } 
}

float TelemetryModule::convert_to_float(byte * buff, float resolution, int byte_size, float min_value){
    // construct the value 
    unsigned long value_raw = 0; 
    for(int i = 0; i < byte_size; i ++){
        value_raw += buff[i] << 8 * (byte_size - 1 - i); 
    }
    // convert to float 
    float data = value_raw * resolution; 
    // add min value 
    return data + min_value; 
}

TelemetryModuleStatus_t TelemetryModule::recall_file(int fd){
    // open up the file 
    _flash_storage.open_file_read(fd); 
    // read the contents entry by entry and print to serial 
    byte buffer[256]; 
    while(_flash_storage.peek() > 3){
        // reconstruct the 
    } 
}