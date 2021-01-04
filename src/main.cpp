/**
 * @file main.cpp
 * \brief first file run by system 
 * 
 * Nothing really happens in this class. Main members are initialized and execution is handed off. 
 * 
 * \author Jeremy Dunne 
 * 
 * \version 0.1 
 * 
 * \date 2021/01/04
 */  


#include <Arduino.h>
#include <PeregrineAltimeter.h> 


PeregrineAltimeter altimeter;         //< Altimeter Object

/**
 * called at the startup 
 */ 
void setup() {
  altimeter = PeregrineAltimeter(); 
  altimeter.run(); 
}
/**
 * loops indefinately
 */ 
void loop() {
  // put your main code here, to run repeatedly:
}