/**
 * @file kinematics.h 
 * \brief class to handle all sensor fusion and kinematic estimates 
 * 
 * handles combining and running all sensors and sensor fusion algorithms to accurately determine 
 * the state of the rocket
 * 
 * TODO rename this class to a more appropriate name 
 * 
 * 
 * 
 * Z + is up (the direction the rocket is supposed to travel)
 * 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _KINEMATICS_H_
#define _KINEMATICS_H_ 

#include <HardwareDefs.h> 
#include <RocketState.h> 
#include <UpdateTimer.h> 

// sensor libraries 
#include <BMP388.h> 
#include <ICM20948.h>

// configurations 
#include <Configurations.h> 

typedef enum{
    KINEMATICS_OK = 0,                      ///< Kinematic engine ok 
    KINEMATICS_IMU_INIT_FAIL,               ///< Failed to init the IMU 
    KINEMATICS_IMU_UPDATE_FAIL,             ///< Failed to update the IMU
    KINEMATICS_BMP_INIT_FAIL,               ///< Failed to init the BMP 
    KINEMATICS_BMP_UPDATE_FAIL,             ///< Failed to update the BMP 
    KINEMATICS_SENSOR_INIT_FAIL,            ///< Failed to init a sensor 
    KINEMATICS_UNKOWN_ERROR,                ///< Kinematics unkown error 
    KINEMATICS_FLIGHT_PHASE_CHANGE          ///< Flight Phase Change Detected 
}   KinematicsStatus_t; 

class Kinematics{
public: 

    /**
     * initailze the kinematics and all sensors 
     * 
     * @return status code 
     */ 
    KinematicsStatus_t begin(RocketState *state); 

    /**
     * update all fusion algorithms and sensors 
     * 
     * @return status code 
     */ 
    KinematicsStatus_t update(); 

    /**
     * return the status 
     * @return status code 
     */ 
    KinematicsStatus_t get_status(){ return _status; }

private: 
    RocketState *_state; 
    KinematicsStatus_t _status; 

    BMP388 _bmp; 
    BMP388::BMP388_data _bmp_data; 
    UpdateTimer _bmp_timer; 

    ICM20948 _imu; 
    ICM20948::ICM20948_raw_data _imu_data; 
    UpdateTimer _imu_timer; 


    // sensor fusion 
    UpdateTimer _position_fuser_timer; 

    // velocity data 
    unsigned long _last_accel_update = 0; 
    float _baro_running_value; 



    // preflight constants 
    float _ground_level_asl = 0; 
    float _ground_level_pressure = 0;

    float _apogee = 0; // used to watch for apogee phase 

    // matched pair used to determine launch 
    float _prelaunch_altitude_buffer[32]; 
    float _prelaunch_acceleration_buffer[32];
    int _prelaunch_buffer_index = 0; 
    bool _prelaunch_buffer_filled = false; 


    // flight phase detection variables 
    bool _flight_phase_watch_triggered = false;
    unsigned long _flight_phase_watch_time = 0; 

    

    /**
     * update the altitude fusing algorithm 
     * @return status code 
     */ 
    KinematicsStatus_t update_altitude_fusion(); 

    /**
     * get the altitude from barometric and temperature data 
     * @return altitude (asl) in meters 
     */ 
    float get_altitude_from_baro(); 

    /**
     * check for a flight phase update 
     * @return status code 
     */ 
    KinematicsStatus_t check_for_flight_phase_change(); 


    /**
     * get the total acceleration magnitude 
     * @return total acceleration on rocket 
     */ 
    float get_accel_magnitude(){
        return pow(_state->_acceleration[0], 2) + pow(_state->_acceleration[1], 2) + pow(_state->_acceleration[2], 2); 
    }


    /**
     * init all sensors 
     * @return status code 
     */ 
    KinematicsStatus_t init_sensors(); 

    /** 
     * init the imu 
     * apply sensor configurations as appropriate 
     * @return status code 
     */ 
    KinematicsStatus_t init_imu(); 

    /**
     * update the imu 
     * @return status code 
     */ 
    KinematicsStatus_t update_imu(); 


    /**
     * init the bmp 
     * apply sensor configurations as appropriate 
     * @return status code 
     */ 
    KinematicsStatus_t init_bmp(); 

    /**
     * update the bmp 
     * @return status code 
     */ 
    KinematicsStatus_t update_bmp(); 


};


#endif 