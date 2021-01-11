/**
 * @file CommunicationProtocol.h 
 * \brief Contains all the standards needed for the communication protocol 
 * 
 * This file contains all the definitions needed for the communication protocol 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _COMMUNICATION_PROTOCOL_H_
#define _COMMUNICATION_PROTOCOL_H_

/// Standard Communication Flags 
#define COMMUNICATION_CONNECTION_REQUEST_FLAG                   11          //< Communication request ping made by the altimeter to connected software 
#define COMMUNICATION_CONNECTION_SUCCESS_FLAG                   12          //< Communication request success response made by connected software 
#define COMMUNICATION_SEND_OK                                   13          //< Communication message acknowledgement 
#define COMMUNICATION_RESEND_REQUEST                            14          //< Communication message request resend 
#define COMMUNICATION_RESEND_UNABLE                             15          //< Communication message resend request unable to be completed 


/// Data Specific Flags 
#define COMMUNICATION_VERBOSE_MESSAGE_FLAG                      30          //< Communication Verbose Message (interpret as string)
#define COMMUNICATION_ERROR_MESSAGE_FLAG                        31          //< Communication Error Message (interpret as string)


// data request flags 
#define COMMUNICATION_REQUEST_FILE_LIST_FLAG                    40          //< Request for the file list 
// request response as follows 
#define COMMUNICATION_FILE_LIST_FLAG                            41          //< Response for the communication file list request 
/*
format as follows: 
    NUM_FILES (1 byte): Number of files available 
    loop for each available file: 
        FILE_PAGE_LENGTH (2 bytes): Number of pages (256 bytes) available  

*/ 


#endif 
