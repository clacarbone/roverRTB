/* 
 * File:   lucciPLAN.h
 * Author: erupter
 *
 * Created on November 2, 2012, 2:41 PM
 */

#ifndef LUCCIPLAN_H
#define	LUCCIPLAN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "lucciRTB.h"
#include "lucciSERVICE.h"
#include <math.h>
    
vector lucciPLAN_givedir(RTB_FLOAT_TYPE actualx, RTB_FLOAT_TYPE actualy, RTB_FLOAT_TYPE desiredx, RTB_FLOAT_TYPE desiredy);


#ifdef	__cplusplus
}
#endif

#endif	/* LUCCIPLAN_H */

