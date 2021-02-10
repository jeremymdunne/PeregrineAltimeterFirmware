#include <Kinematics.h> 

KinematicsStatus_t Kinematics::begin(RocketState *state){
    _state = state; 
    // init sensors 
    _status = init_sensors(); 
    if(_status != KINEMATICS_OK) return _status; 
    // init timers 
    _bmp_timer.set_timer_frequency(SENSOR_BMP_UPDATE_FREQ); 
    _bmp_timer.start_timer(); 
    _imu_timer.set_timer_frequency(SENSOR_IMU_UPDATE_FREQ); 
    _imu_timer.start_timer(); 
    // return an ok 
    _status = KINEMATICS_OK; 
    return _status; 
}

KinematicsStatus_t Kinematics::update(){
    // check timers, update sensors and fusion algorithms as necessary 
    if(_imu_timer.update() == UPDATE_TIMER_UPDATE_AVAILABLE){
        _status = update_imu(); 
        if(_status != KINEMATICS_OK) return _status; 
    }
    if(_bmp_timer.update() == UPDATE_TIMER_UPDATE_AVAILABLE){
        _status = update_bmp(); 
        if(_status != KINEMATICS_OK) return _status; 
    }
    return KINEMATICS_OK;
}


KinematicsStatus_t Kinematics::check_for_flight_phase_change(){
    // check for a flight phase change 
    // base off of current phase 
    switch(_state->_flight_phase){
        case(WAITING_FOR_LAUNCH_PHASE):
            // flight phase based on sustained acceleration over a time period 
            if(_flight_phase_watch_triggered){
                // already cautious about the flight phase 
                // check if acceleration is maintained 
                if(get_accel_magnitude() > LAUNCH_DETECTION_MINUMUM_SUSTAINED_ACCELERATION){
                    if(_state->_flight_time - _flight_phase_watch_time > LAUNCH_DETECTION_MINUMUM_SUSTAINED_TIME){
                        // acceleration has been maintained, trigger flight phase change 
                        _state->_flight_phase = BOOST_PHASE; 
                        // overwrite the trigger 
                        _flight_phase_watch_triggered = false; 
                        _flight_phase_watch_time = 0; 
                        // return notice of flight change 
                        return KINEMATICS_FLIGHT_PHASE_CHANGE; 
                    }
                    // otherwise, keep on 
                }
                else{
                    // acceleration magnitude fell below threshold 
                    // reset the trigger 
                    _flight_phase_watch_triggered = false; 
                    _flight_phase_watch_time = 0; 
                }
            }
            else{
                // check if the magnitude is above the threshold 
                if(get_accel_magnitude() > LAUNCH_DETECTION_MINUMUM_SUSTAINED_ACCELERATION){
                    // set the trigger and time 
                    _flight_phase_watch_triggered = true; 
                    _flight_phase_watch_time = _state->_flight_time; 
                }
            }
            break; 
        case(BOOST_PHASE):
            // coast phase is determined when the acceleration crosses over the 'zero point' to become negative 
            // this will fail to account for residual thrust in motors, but should be good enough 
            if(_flight_phase_watch_triggered){
                // check if the Z acceleration is less than 0 
                if(_state->_acceration[2] < 0){
                    // check the time 
                    if(_state->_flight_time - _flight_phase_watch_time > COAST_DETECTION_MINUMUM_SUSTAINED_TIME){
                        // change the state 
                        _state->_flight_phase = COAST_PHASE; 
                        //overwrite the states 
                        _flight_phase_watch_triggered = false; 
                        _flight_phase_watch_time = 0; 
                        return KINEMATICS_FLIGHT_PHASE_CHANGE; 
                    }
                }
                // reset the trigger 
                _flight_phase_watch_triggered = false; 
                _flight_phase_watch_time = 0; 
            }
            else{
                if(_state->_acceration[2] < 0){
                    // start the trigger 
                    _flight_phase_watch_triggered = true; 
                    _flight_phase_watch_time = _state->_flight_time; 
                }
            }
            break; 
        case(COAST_PHASE): 
            // wait for baro to be decreasing 
            if(_flight_phase_watch_triggered){
                // 
            }
            else{

            }

    }
}


KinematicsStatus_t Kinematics::init_sensors(){
    // init the bmp 
    KinematicsStatus_t bmp_status = init_bmp(); 
    if(bmp_status == KINEMATICS_BMP_INIT_FAIL) return bmp_status; 
    KinematicsStatus_t imu_status = init_imu(); 
    if(imu_status == KINEMATICS_IMU_INIT_FAIL) return imu_status; 

    return KINEMATICS_OK; 
}


KinematicsStatus_t Kinematics::init_imu(){
    int imu_status = _imu.begin(IMU_CS_PIN); 
    // TODO set imu configurations 
    if(imu_status != 0){
        // imu failure 
        return KINEMATICS_IMU_INIT_FAIL; 
    }
    return KINEMATICS_OK;
}

KinematicsStatus_t Kinematics::update_imu(){
    // get new data from the imu 
    int imu_status = _imu.get_data(&_imu_data); 
    if(imu_status != 0) return KINEMATICS_IMU_UPDATE_FAIL; 
    // copy the data into the state 
    for(int i = 0; i < 3; i ++){
        _state->_acceration[i] = _imu_data.accel[i]; 
        _state->_rotation_rates[i] = _imu_data.gyro[i];
        _state->_magnetic_strength[i] = _imu_data.mag[i];  
    }
    return KINEMATICS_OK; 
}

KinematicsStatus_t Kinematics::init_bmp(){
    int bmp_status = _bmp.begin(BARO_CS_PIN); 
    if(bmp_status != 0){
        // bmp init failure 
        return KINEMATICS_BMP_INIT_FAIL; 
    }
    return KINEMATICS_OK; 
}

KinematicsStatus_t Kinematics::update_bmp(){
    int bmp_status = _bmp.get_new_data(&_bmp_data); 
    if(bmp_status != 0) return KINEMATICS_BMP_UPDATE_FAIL; 
    _state->_pressure = _bmp_data.pressure; 
    _state->_temperature = _bmp_data.temperature; 
    return KINEMATICS_OK; 
}