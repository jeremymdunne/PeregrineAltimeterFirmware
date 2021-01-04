/**
 * @file RocketModule.h 
 * 
 * \brief Template for rocket modules requiring access to the vehicle's state 
 * 
 * Contains the required members for modules requiring access to the vehicle's state. 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _ROCKET_MODULE_H_
#define _ROCKET_MODULE_H_

#include <RocketState.h> 


class RocketModule{ 
public: 
    /**
     * initialize the module 
     * 
     * @param state pointer to the state of the rocket 
     * @return error or success code 
     */ 
    int begin(RocketState *state); 

    /**
     * update the module as necessary 
     * 
     * @return error or status code 
     */ 
    int update(); 

    /**
     * stop the module, allow the module to shutdown successfully 
     * 
     * @return error or success code 
     */ 
    int stop(); 

private: 
    RocketState *_rocket_state;         ///< pointer to the state of the rocket
}; 




#endif 