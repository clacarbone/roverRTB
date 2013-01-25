/* 
 * File:   time_decimator.h
 * Author: erupter
 *
 * Created on October 1, 2012, 7:03 PM
 */

#ifndef TIME_DECIMATOR_H
#define	TIME_DECIMATOR_H

#include <time.h>
#include "libplayerc/playerc.h"
#include "options.h"
#ifdef	__cplusplus
extern "C" {
#endif

extern long     DecimatorGpsDelay, DecimatorOdomDelay, DecimatorLaserDelay;
extern double   DecimatorLaserFreq, DecimatorGpsFreq, DecimatorOdomFreq;
extern int      DecimatorLaserSamples;

int             DecimatorInitialize(void);
void            DecimatorPosGpsCallback (playerc_position2d_t * data);
void            DecimatorPosOdoCallback (playerc_position2d_t * data);
void            DecimatorLaserFrontCallback (playerc_ranger_t * data);
void            DecimatorLaserBackCallback (playerc_ranger_t * data);

#ifdef	__cplusplus
}
#endif

#endif	/* TIME_DECIMATOR_H */

