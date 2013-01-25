/* 
 * File:   service.h
 * Author: erupter
 *
 * Created on October 25, 2012, 12:08 PM
 */

#ifndef SERVICE_H
#define	SERVICE_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "output.h"
//#include <zmq.h>
//#include <sys/time.h>
//#include "zhelpers.h"
//#include <czmq.h>
//#include "zmq_manager.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "player_manager.h"
#include "options.h"
#include "cli_parser.h"
#include <omp.h>
#include "time_decimator.h"
//#include <fcntl.h>>

volatile sig_atomic_t s_interrupted = 0;
volatile sig_atomic_t alarm_expired = 0;


static void s_signal_handler (int signal_value)
{
    if (signal_value == SIGTERM || signal_value == SIGSTOP || signal_value == SIGINT)
        s_interrupted = 1;
    if (signal_value == SIGALRM)
        //signal(SIGALRM,alarm_wakeup);
        alarm_expired = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGALRM, &action, NULL);
}

#ifdef	__cplusplus
}
#endif

#endif	/* SERVICE_H */

