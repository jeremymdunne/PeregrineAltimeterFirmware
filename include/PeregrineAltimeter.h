/**
 * @file PeregrineAltimeter.h 
 * \brief Main control program for the altimeter 
 * 
 * This class controls the execution of all other modules running on the altimeter. 
 * 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _PEREGRINE_ALTIMETER_H_
#define _PEREGRINE_ALTIMETER_H_

#include <Arduino.h> 
#include <RocketState.h> 

class PeregrineAltimeter{
public: 
    /**
     * run the altimeter 
     * 
     * @return error or success code 
     */ 
    int run(); 
private: 
    RocketState _state;             ///< Main state of the rocket 

    /**
     * initialize the altimeter and all modules 
     * 
     * @return error or success code 
     */ 
    int init_modules(); 

    /** 
     * update all modules
     * 
     * @return error or status code 
     */ 
    int update_modules(); 

    /**
     * stop all modules 
     * 
     * @return error or success code 
     */ 
    int stop_all_modules(); 

    
}; 


#endif 