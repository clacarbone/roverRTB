#include "time_decimator.h"
#include "options.h"
#include "lucciOA.h"
#include <math.h>

extern double GlobalX,GlobalY,GlobalA, GlobalV, GlobalVA, globalX,globalY, execute, yaw;
extern playerc_ranger_t SensorData;

long    DecimatorGpsDelay = 0,
        DecimatorLaserDelay = 0,
        DecimatorOdomDelay = 0,
        DecimatorLaserAccumulator = 0,
        DecimatorGpsAccumulator = 0,
        DecimatorOdomAccumulator = 0;

struct timeval  _decim_gps_oldtime,
        _decim_gps_actualtime,
        _decim_laser_front_oldtime,
        _decim_laser_front_actualtime,
        _decim_laser_back_oldtime,
        _decim_laser_back_actualtime,
        _decim_odom_oldtime,
        _decim_odom_actualtime;

double  DecimatorLaserFreq = -1,     
        DecimatorGpsFreq = 5,
        DecimatorOdomFreq = -1,
        DecimatorLaserFreqDetct = 0,
        DecimatorGpsFreqDetct = 0,
        DecimatorOdomFreqDetct = 0;

int     DecimatorLaserSamples = 90,
        DecimatorLaserCounter = 0,
        DecimatorGpsCounter = 0,
        DecimatorOdomCounter = 0;

int     DecimatorInitialize (void)
{
    if(DEC_DEBUG)
        dbg_print(DEC_PROC_NAME,"Initializing dividers\n");
    if (DecimatorLaserFreq != -1)
    {
        DecimatorLaserDelay = DEF_SECOND / DecimatorLaserFreq;
        if (DEC_DEBUG)
            dbg_print(DEC_PROC_NAME,"Laser Freq: %.2f Hz, converted to a %ldus delay.\n",DecimatorLaserFreq, DecimatorLaserDelay);
    }        
    
    if (DecimatorGpsFreq != -1)
    {
        DecimatorGpsDelay = DEF_SECOND / DecimatorGpsFreq;
        if (DEC_DEBUG)
            dbg_print(DEC_PROC_NAME,"GPS Freq: %.2f Hz, converted to a %ldus delay.\n",DecimatorGpsFreq, DecimatorGpsDelay);
    }
    
    if (DecimatorOdomFreq != -1)
    {
        DecimatorOdomDelay = DEF_SECOND / DecimatorOdomFreq;    
        if (DEC_DEBUG)
            dbg_print(DEC_PROC_NAME,"Odom Freq: %.2f Hz, converted to a %ldus delay.\n",DecimatorOdomFreq, DecimatorOdomDelay);
    }
    gettimeofday(&_decim_gps_oldtime,NULL);
    gettimeofday(&_decim_odom_oldtime,NULL);
    gettimeofday(&_decim_laser_front_oldtime,NULL);
    gettimeofday(&_decim_laser_back_oldtime,NULL);
    
    
    
}

int timeval_substract(/*struct timeval *result,*/ struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    //result->tv_sec = diff / 1000000;
    //result->tv_usec = diff % 1000000;

    return (diff);
}

void    DecimatorPosGpsCallback (playerc_position2d_t * data)
{
    //if (DecimatorGpsDelay != 0)
    //{
        long diff;
        
        gettimeofday(&_decim_gps_actualtime, NULL);
        diff = timeval_substract(&_decim_gps_actualtime, &_decim_gps_oldtime);
        if( diff >= DecimatorGpsDelay)
        {
            DecimatorGpsCounter++;
            DecimatorGpsAccumulator += diff;
            if (DecimatorGpsAccumulator > 5 * DEF_SECOND)
            {
                DecimatorGpsFreqDetct =  DEF_SECOND / (DecimatorGpsAccumulator / (float)DecimatorGpsCounter);
                DecimatorGpsAccumulator = 0;
                DecimatorGpsCounter =0;
                if (DEC_DEBUG > 1)
                    dbg_print(DEC_PROC_NAME,"Gps data passed to application, actual freq %.2f Hz\n",DecimatorGpsFreqDetct);
            }
            gettimeofday(&_decim_gps_oldtime, NULL);

        }

    //}
}

void    DecimatorPosOdoCallback (playerc_position2d_t * data)
{
    //if (DecimatorOdomDelay != 0)
    //{
        long diff;
        
        gettimeofday(&_decim_odom_actualtime, NULL);
        diff = timeval_substract(&_decim_odom_actualtime, &_decim_odom_oldtime);
        if( diff >= DecimatorOdomDelay)
        {
            DecimatorOdomCounter++;
            DecimatorOdomAccumulator += diff;
            if (DecimatorOdomAccumulator > 5*DEF_SECOND)
            {
                DecimatorOdomFreqDetct = DEF_SECOND / (DecimatorOdomAccumulator / (float)DecimatorOdomCounter);
                DecimatorOdomAccumulator = 0;
                DecimatorOdomCounter =0;                
                if (DEC_DEBUG > 2)
                    dbg_print(DEC_PROC_NAME,"Odometry data passed to application, actual freq %.2f Hz\n",DecimatorOdomFreqDetct);
            }
            gettimeofday(&_decim_odom_oldtime, NULL);
            GlobalX = data->px;
            GlobalY = data->py;
            GlobalA = data->pa;
            GlobalV = sqrt(pow(data->vx,2)+ pow(data->vy,2));
            GlobalVA = data->va;
            //RTB_update(data->px, data->py, sqrt(pow(data->vx,2)+ pow(data->vy,2)), data->va);
            //yaw = data->pa;
            
            
        }
    //}
}

void    DecimatorLaserFrontCallback (playerc_ranger_t * data)
{
    //if (DecimatorLaserDelay != 0)
    //{
        long diff;
        
        gettimeofday(&_decim_laser_front_actualtime, NULL);
        diff = timeval_substract(&_decim_laser_front_actualtime, &_decim_laser_front_oldtime);
        if( diff >= DecimatorLaserDelay)
        {
            DecimatorLaserCounter++;
            DecimatorLaserAccumulator += diff;
            if (DecimatorLaserAccumulator > 5* DEF_SECOND)
            {
                DecimatorLaserFreqDetct = DEF_SECOND / (DecimatorLaserAccumulator / (float)DecimatorLaserCounter);
                DecimatorLaserAccumulator = 0;
                DecimatorLaserCounter =0;            
                if (DEC_DEBUG > 2)
                    dbg_print(DEC_PROC_NAME,"Front laser data passed to application, %.2f Hz\n",DecimatorLaserFreqDetct);
            }
                
            gettimeofday(&_decim_laser_front_oldtime, NULL);
            SensorData = *data;
            //OA_perform_avoidance(data->ranges,data->ranges_count, yaw,37.67);
        }
    //}
}

void    DecimatorLaserBackCallback (playerc_ranger_t * data)
{
    if (DecimatorLaserDelay != 0)
    {
        long diff;
        gettimeofday(&_decim_laser_back_actualtime, NULL);
        diff = timeval_substract(&_decim_laser_back_actualtime, &_decim_laser_back_oldtime);
        if( diff >= DecimatorLaserDelay)
        {
            gettimeofday(&_decim_laser_back_oldtime, NULL);
            if (DEC_DEBUG > 3)
                dbg_print(DEC_PROC_NAME,"Back laser data passed to application, %ldus since last\n",diff);
        }
    }
}