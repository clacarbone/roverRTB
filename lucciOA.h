/* 
 * File:   lucciOA.h
 * Author: erupter
 *
 * Created on October 29, 2012, 12:01 PM
 */

#ifndef LUCCIOA_H
#define	LUCCIOA_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include <malloc.h>
#include <math.h>
#include "lucciSERVICE.h"
    
#define PI 3.14159265
/*! \brief OA space required laterally to determine clearance
 *
 *      
 *  This number represents the amplitude of the space that has to be free for
 * the algorithm to determine it has not to avoid it.
 * 
 */
#define OA_SENSOR_CLOCKWISE             0
#define OA_SENSOR_ANTI_CLOCKWISE        1
    
#define OA_ACTIVE_DISTANCE              7.0 //in meters
    
#define OA_GRID_RESOLUTION              0.1 //in meters    
    
#define OA_GRID_RADIAL_SAMPLING         5 //in degrees
    
#define OA_SENSOR_RESOLUTION            0.004363323 //in radians   
    
#define OA_SENSOR_CENTER                0.0   //in radians
    
#define OA_SENSOR_COUNT                 1080 // number of laser readings
    
#define OA_WINDOW_SIZE                  5 // minimum number of consecutive under threshold values to consider a window
    
#define OA_WINDOW_THRESHOLD             5 // value at which to consider a space viable
    
#define OA_SENSOR_ORIENTATION           OA_SENSOR_CLOCKWISE // or OA_RIGHT_TO_LEFT
    
#define OA_SPEED_MAX                    2.0 // meters per second
    
#define OA_SPEED_MIN                    0.1 // meters per second
    
#define OA_SPEED_MODERATION_DIST        3.0 // meters
    
#define OA_SAFETY_DISTANCE              1.0 // meters
    
#define OA_AVOID_INTENSITY              2.0 // adimensional, multiplier of the avoiding vector



void    OA_init (void);
vector OA_perform_avoidance (RTB_FLOAT_TYPE readings[], long count, RTB_FLOAT_TYPE yaw, vector desired_heading);

#ifdef	__cplusplus
}
#endif

#endif	/* LUCCIOA_H */

