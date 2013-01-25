/* 
 * File:   options.h
 * Author: erupter
 *
 * Created on August 20, 2012, 3:57 PM
 */

#ifndef OPTIONS_H
#define	OPTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdarg.h>
#include "output.h"
struct timeval plm_tv;
time_t plm_t;
struct tm plm_tm;
#define PROC_STR(PROC_NAME) {plm_t=time(NULL); gettimeofday(&plm_tv, NULL); localtime_r(&plm_t,&plm_tm); fprintf(outstream,"[%02d:%02d:%02d.%06ld  %s] ", plm_tm.tm_hour,plm_tm.tm_min,plm_tm.tm_sec,plm_tv.tv_usec,PROC_NAME);}    

//#define MAIN_DEBUG 0
//#define ZMQ_DEBUG 0
//#define PLM_DEBUG 0
int MAIN_DEBUG;
int ZMQ_DEBUG;
int PLM_DEBUG;
int DEC_DEBUG;

#define DEF_SECOND 1000000
#define MAIN_PROGRAM_VERSION "Distributed Map Builder 0.1"

//#if MAIN_DEBUG > 0
#define MAIN_PROC_NAME "Main"
//#endif


//#if PLM_DEBUG > 0
#define PLM_PROC_NAME "Player Manager"
//#endif

//#if ZMQ_DEBUG > 0
#define ZMQ_PROC_NAME "0MQ Lib"
//#endif

#define DEC_PROC_NAME "Decimator"


static void dbg_print(char* proc_name, const char *format,...)
{
    PROC_STR(proc_name);
    va_list argptr;
    va_start (argptr, format);
    vfprintf (outstream, format, argptr);
    va_end (argptr);
}
#ifdef	__cplusplus
}
#endif

#endif	/* OPTIONS_H */

