/**
 * @file Configurations.h 
 * \brief commonly changed attributes 
 * 
 * This file contains commonly changed atributes about the altimeter 
 * 
 * \author Jeremy Dunne 
 */ 


#ifndef _CONFIGURATIONS_H_
#define _CONFIGURATIONS_H_

// Data recording frequencies 
#define DATA_RECORDING_GENERAL_FLIGHT_DATA_FREQUENCY 10 


// Sensor update frequencies 
#define SENSOR_IMU_UPDATE_FREQ          100 
#define SENSOR_BMP_UPDATE_FREQ          20 

// live computer data frequencies 
#define LIVE_SENSOR_UPDATES_FREQ        3






// Flight Phase Detection Algorithms

#define LAUNCH_DETECTION_MINUMUM_SUSTAINED_ACCELERATION         20 // m/s/s 
#define LAUNCH_DETECTION_MINUMUM_SUSTAINED_TIME                 2000 // ms 

#define COAST_DETECTION_MAXIMUM_SUSTAINED_ACCELERATION          0   // m/s/s 
#define COAST_DETECTION_MINUMUM_SUSTAINED_TIME                   500 // m/s 

#define APOGEE_DETECTION_MAXIMUM_VELOCITY                       30  // m/s 
#define APOGEE_DETECTION_MINUMUM_APOGEE                         20 // m 





#endif 