/* 
 * File:   cli_parser.h
 * Author: erupter
 *
 * Created on August 22, 2012, 12:53 PM
 */

#ifndef CLI_PARSER_H
#define	CLI_PARSER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <argp.h>
#include <stdio.h>
#include "options.h"
    
const char *argp_program_version =
MAIN_PROGRAM_VERSION;

const char *argp_program_bug_address =
"noaddr@email.com";

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  int *P_DEBUG, *Z_DEBUG, *D_DEBUG, *M_DEBUG, robot, *lsamples;
  double *lasfreq, *gpsfreq, *odofreq;
  char *outfile;            /* Argument for -o */
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"PLMDebug", 'P', "n", 0, "Set debug level of the Player Manager"},
  {"ZMQDebug", 'Z', "n", 0, "Set debug level of the 0MQ Manager"},
  {"MAINDebug", 'M', "n", 0, "Set debug level of the main program"},
  {"DECDebug", 'D', "n", 0, "Set debug level of the Decimator"},
  {"output",  'o', "OUTFILE", 0, "Output to OUTFILE instead of to standard output"},
  {"robot", 'r' , "n", 0, "Set robot index for multi robot environment"},
  {"Laser Frequency", 'l', "n", 0, "Set laser sampling frequency"},
  {"GPS Frequency", 'g', "n", 0, "Set GPS reporting frequency"},
  {"Odometry Frequency", 'm', "n", 0, "Set Odometry reporting frequency"},
  {"Laser angular samples", 'h', "n", 0, "Set the angular samples number for the Laser scanners"},
  {0}
};


/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt (char key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
      case 'P':
        *(arguments->P_DEBUG) = atoi(arg);
        break;
    case 'Z':
        *(arguments->Z_DEBUG) = atoi(arg);
        break;
    case 'M':
        *(arguments->M_DEBUG) = atoi(arg);
        break;        
    case 'D':
        *(arguments->D_DEBUG) = atoi(arg);
        break;          
    case 'o':
        arguments->outfile = arg;
        break;
    case 'r':
        arguments->robot = atoi(arg);
        break;

    case 'l':
        *(arguments->lasfreq) = atof(arg);
        break;
        
    case 'g':
        *(arguments->gpsfreq) = atof(arg);
        break;
        
    case 'm':
        *(arguments->odofreq) = atof(arg);
        break;
        
    case 'h':
        *(arguments->lsamples) = atoi(arg);
        break;
        
    case ARGP_KEY_ARG:
    case ARGP_KEY_END:
        argp_usage(state);
        break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
//static char args_doc[] = "ARG1 ARG2";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
"Distributed Map Builder -- A program to demonstrate how to code command-line options and arguments.\vFrom the GNU C Tutorial.";

/*
   The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, 0, doc};


#ifdef	__cplusplus
}
#endif

#endif	/* CLI_PARSER_H */

