/**
 * @file RocketState.h 
 * 
 * \brief struct containing all information about the state of the rocket 
 * \author Jeremy Dunne 
 */ 

#ifndef _ROCKET_STATE_H_
#define _ROCKET_STATE_H_

/// enum that represents what part of the flight phase the rocket is in  
    enum FLIGHT_PHASE{
        WAITING_FOR_LAUNCH_PHASE,       ///< waiting for launch detection 
        BOOST_PHASE,                    ///< in boost phase (thrust detected)
        COAST_PHASE,                    ///< in coast phase (thrust stopped, velocity still > 0)
        APOGEE_PHASE,                   ///< apogee detected 
        RECOVERY_PHASE,                 ///< descent detected 
        LANDED_PHASE                    ///< landing detected 
    };

struct RocketState{
    
    // time definitions 
    unsigned long _sys_time;            ///< system time according to start up in microsecond  
    unsigned long _flight_time;         ///< time into flight in microseconds 
    unsigned long _launch_time;         ///< time the launch ocurred (according to _sys_time)


    // sensor data 
    float _pressure;                    ///< measured pressure in pascals 
    float _temperature;                 ///< measured temperature in celsius 
    float _acceleration[3];               ///< measured accelleration in m/s/s, XYZ 
    float _rotation_rates[3];           ///< measured rotation rates in deg/s, Roll, Pitch, Yaw 
    float _magnetic_strength[3];        ///< measured in milliteslas, XYZ 

    float _altitude_asl;                ///< measured altitude asl, used for launch determination 

    // compiled data 
    float _fused_position[3];           ///< fused position in m, XYZ 
    float _fused_velocity[3];           ///< estimated velocity in m/s, XYZ 
    float _fused_acceleration[3];       ///< fused acceleration in m/s/s, XYZ 
    float _fused_attitude[3];           ///< fused attitude in deg, XYZ 

    

    // enum members 
    FLIGHT_PHASE _flight_phase;         ///< current flight phase of the rocket 

    // state members 
    bool _in_simulation_mode;           ///< represents the simulation mode 
    

}; 



#endif 