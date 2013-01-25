#include "lucciPLAN.h"


vector lucciPLAN_givedir(RTB_FLOAT_TYPE actualx, RTB_FLOAT_TYPE actualy, RTB_FLOAT_TYPE desiredx, RTB_FLOAT_TYPE desiredy)
{
    vector localvec;
    RTB_FLOAT_TYPE localx, localy;
    localx = desiredx - actualx;
    localy = desiredy - actualy;
    localvec.angle_rad = atan2(localy, localx);
    localvec.angle_deg_north = lucciSERVICE_rad2degnorth(localvec.angle_rad);
    localvec.x = cos(localvec.angle_rad);
    localvec.y = sin(localvec.angle_rad);
    localvec = lucciSERVICE_vect_normalize(localvec);
    localvec = lucciSERVICE_vect_set_norm(2.0, localvec);
    return localvec;
}
