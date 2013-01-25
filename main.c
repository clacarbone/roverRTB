/* 
 * File:   main.c
 * Author: erupter
 *
 * Created on October 25, 2012, 12:02 PM
 */
#include <player-3.1/libplayerc/playerc.h>
#include <SDL/SDL.h>

playerc_ranger_t SensorData;
double GlobalX=0,GlobalY=0,GlobalA=0, GlobalV=0, GlobalVA=0;


#include <stdio.h>
#include <unistd.h>

#include <termios.h>
#include <termio.h>
#include <fcntl.h>

#include "service.h"
#include "lucciRTB.h"
#include "lucciOA.h"
#include "lucciPLAN.h"
#include "lucciSERVICE.h"
#include "player_manager.h"
#include "goal.h"
extern RTB_point *RTB_internal_point_start, *RTB_internal_point_last;
extern playerc_position2d_t *PlayerLocalPosition2dRelative;
extern playerc_graphics2d_t *PlayerLocalGraphicsMoving;
extern playerc_graphics2d_t *PlayerLocalGraphics2D;

int NoJoy = 0,
    global_event = 0,
    button[4],
    button_status[4],
    button_counter[4],
    button_debouncingup[4],
    button_debouncingdown[4];
long button_interdiction[4];
    SDL_Joystick *mystick;
    SDL_Event joy_event, event;
    double globalX=0;
    double globalY=0;
    double heading=0;
    double speeda;
    int execute;
    double yaw;
    char mystr[40];
    int len;
    struct termios term;
    int output_pty;
    RTB_status RTBstatus;
    vector dir;
    extern vehicle_control control_values;
    double actual_dir;
    float vel,turn,command_vel,command_turn;
    
#define COEFF 0.98
    
/*
 * 
 */

int main(int argc, char** argv) {

    outstream = stdout;
    PLM_DEBUG=0;
    ZMQ_DEBUG=0;
    MAIN_DEBUG=0;
    struct arguments arguments;
    RTB_status *localRTB;
    outstream = stdout;

    /* Set argument defaults */
    arguments.outfile = NULL;
    arguments.P_DEBUG = &PLM_DEBUG;
    arguments.Z_DEBUG = &ZMQ_DEBUG;
    arguments.M_DEBUG = &MAIN_DEBUG;
    arguments.D_DEBUG = &DEC_DEBUG;
    arguments.robot = 0;
    arguments.gpsfreq = &DecimatorGpsFreq;
    arguments.lasfreq = &DecimatorLaserFreq;
    arguments.odofreq = &DecimatorOdomFreq;
    arguments.lsamples = &DecimatorLaserSamples;

    /* Where the magic happens */
    int arg_parse_res = argp_parse (&argp, argc, argv, 0, 0, &arguments);

    /* Where do we send output? */
    if (arguments.outfile)
    outstream = fopen (arguments.outfile, "w");

    
    s_catch_signals ();
    Player_Manager_Init(arguments.robot);
    DecimatorInitialize();
    Player_Manager_RegisterCallback(PL_CB_ODO, (playerc_callback_fn_t)&DecimatorPosOdoCallback);
    Player_Manager_RegisterCallback(PL_CB_GPS, (playerc_callback_fn_t)&DecimatorPosGpsCallback);
    Player_Manager_RegisterCallback(PL_CB_LF, (playerc_callback_fn_t)&DecimatorLaserFrontCallback);
    Player_Manager_RegisterCallback(PL_CB_LB, (playerc_callback_fn_t)&DecimatorLaserBackCallback);
    RTB_init();
    //RTB_set_mode(RTB_recording);
    OA_init();
    long counter,counter2;
    vector localvec;
    RTBstatus.control_values.heading = 0;
    RTBstatus.control_values.speed = 0;
    player_point_2d_t points[4];
    points[0].px=0;
    points[0].py=0;
    points[1].px=0;
    points[1].py=0;
    points[2].px=0;
    points[2].py=0;
    points[3].px=0;
    points[3].py=0;
    
    
    if (!(SDL_Init(SDL_INIT_EVENTTHREAD | SDL_INIT_JOYSTICK) < 0))
    {
        if (SDL_NumJoysticks() > 1)
            if(mystick = SDL_JoystickOpen(1) != NULL)
                NoJoy = 0;
            else
                NoJoy = 1;
        else
        {
            if (SDL_NumJoysticks() > 0)
                if(mystick = SDL_JoystickOpen(0) != NULL)
                    NoJoy = 0;
                else
                    NoJoy = 1;
            else
                NoJoy = 1;
        }

        
    }
    SDL_JoystickEventState(SDL_QUERY);
    const char *name;
	int i;
    for ( i=0; i<SDL_NumJoysticks(); ++i ) {
		name = SDL_JoystickName(i);
		printf("Joystick %d: %s\n",i,name ? name : "Unknown Joystick");
		mystick = SDL_JoystickOpen(i);
		if (mystick == NULL) {
			fprintf(stderr, "SDL_JoystickOpen(%d) failed: %s\n", i, SDL_GetError());
		} else {
			printf("       axes: %d\n", SDL_JoystickNumAxes(mystick));
			printf("      balls: %d\n", SDL_JoystickNumBalls(mystick));
			printf("       hats: %d\n", SDL_JoystickNumHats(mystick));
			printf("    buttons: %d\n", SDL_JoystickNumButtons(mystick));
			SDL_JoystickClose(mystick);
		}
	}

    
    
    while (1) {

        if (s_interrupted == 1) {
            fprintf(outstream, "\n");
            if (MAIN_DEBUG)
                dbg_print(MAIN_PROC_NAME,"SIGINT interrupt received, killing node\n");
            else
                fprintf(outstream, "\n!!!!!    KILL NODE COMMAND RECEIVED    !!!!!\n\n");
            break;
        }
        
        //Calls for the various Finite State Machines
        Player_Manager();
        if (PLAYER_MANAGER_STATE == PL_IDLE)
            Player_Manager_Connect();
        
        if (PLAYER_MANAGER_STATE == PL_CONNECTED)
        {
            if ((RTBstatus.mode == RTB_idle || RTBstatus.mode == RTB_recording) && (SensorData.info.fresh && PlayerLocalPosition2dRelative->info.fresh))
            {
                
                RTB_update(GlobalX, GlobalY, GlobalV, GlobalVA);
                
                if (counter > 100000)
                {
                    if (2*command_vel != PlayerLocalPosition2dRelative->vx || 2*command_turn != PlayerLocalPosition2dRelative->va)
                        playerc_position2d_set_cmd_vel( PlayerLocalPosition2dRelative, (double)2*command_vel, 0, 2*command_turn, 1);
                    counter=0;
                }
                else
                    counter++;
            }
            else
            {
                if (SensorData.info.fresh && PlayerLocalPosition2dRelative->info.fresh)
                {
                    RTB_update(GlobalX, GlobalY, GlobalV, GlobalVA);
                    //points[1].px = 3*RTBstatus.control_vector.x;
                    //points[1].py = 3*RTBstatus.control_vector.y;
                    localvec.x = GOAL_X-GlobalX;
                    localvec.y = GOAL_Y-GlobalY;
                    localvec = lucciSERVICE_vect_normalize(localvec);

                    //dir = lucciPLAN_givedir(GlobalX,GlobalY,GOAL_X, GOAL_Y);

                    points[2].px = 3*cos(-GlobalA+dir.angle_rad);
                    points[2].py = 3*sin(-GlobalA+dir.angle_rad);
                    //RTBstatus.control_values.speed = 2;
                    RTBstatus.control_vector=OA_perform_avoidance(SensorData.ranges, SensorData.ranges_count,GlobalA,RTBstatus.control_vector);
                    //if(control_values.speed == 0)


                    heading= RTBstatus.control_vector.angle_rad;
                    //heading = lucciSERVICE_degnorth2rad(control_values.heading);
                    actual_dir = PlayerLocalPosition2dRelative->pa;
                    double mydiff = heading - PlayerLocalPosition2dRelative->pa;

                    points[0].px = 3*cos(-GlobalA+RTBstatus.control_vector.angle_rad);
                    points[0].py = 3*sin(-GlobalA+RTBstatus.control_vector.angle_rad);

                    if (mydiff > M_PI)
                        mydiff -= 2*M_PI;
                    if (mydiff < -M_PI)
                        mydiff += 2*M_PI;
                    if (mydiff > 0.05)
                    {
                        speeda=1.2;
                        //playerc_position2d_set_cmd_vel( PlayerLocalPosition2dRelative, 0, 0, 0.8, 1);
                    }
                    if (mydiff < -0.05)
                    {
                        speeda=-1.2;
                        //playerc_position2d_set_cmd_vel( PlayerLocalPosition2dRelative, 0, 0, -0.8, 1);
                    }
                    if (mydiff > -0.05 && mydiff < 0.05)
                        speeda = 0;

                    //if (RTBstatus.control_vector.norm >= 0.1 && RTBstatus.control_vector.norm <= 2.0)
                    RTBstatus.control_values.speed = (double)RTBstatus.control_vector.norm;

                    /*else
                    {
                        //if (RTBstatus.control_vector.norm < 0.1)
                            //RTBstatus.control_values.speed = 0.1;
                        if (RTBstatus.control_vector.norm > 2.0)
                            RTBstatus.control_values.speed = 2.0;
                    }*/

                    playerc_position2d_set_cmd_vel( PlayerLocalPosition2dRelative, (double)RTBstatus.control_values.speed, 0, speeda, 1);



                    if (counter >= 500)
                    {
                        playerc_graphics2d_clear(PlayerLocalGraphicsMoving);
                        playerc_graphics2d_draw_multiline(PlayerLocalGraphicsMoving,points,4);
                        //printf("Actual HDG: %3.2f, Computed instruction %3.2f, Speed %3.2f, Speed %3.2f\n",PlayerLocalPosition2dRelative->pa, heading, RTBstatus.control_vector.norm, RTBstatus.control_values.speed);
                        //printf("Speed command %3.2f, Turn Command %3.2f\n",command_vel, command_turn);
                        counter = 0;
                    }
                    else
                        counter += 1;
                }
            }
        }
        /*if (execute==1)
        {
            localRTB=RTB_update((double)globalX,(double)globalY);
            execute=0;
        }*/
        /*
        ch=getch();
        if (ch=='g')
        {
            printf("\nActual position: %3.2f  %3.2f, heading %3.2f\n\n",PlayerLocalPosition2dRelative->px, PlayerLocalPosition2dRelative->py, PlayerLocalPosition2dRelative->pa);
        }
        if (ch=='s')
        {
            printf ("Enter new position's X value:\n");
            char * chptr;
            getstr(chptr);
            newx=atof(chptr);
            printf ("Enter new position's Y value:\n");
            getstr(chptr);
            newy=atof(chptr);
        }*/

        for (i=0;i<4;i++)
        {
            if (button_interdiction[i]>0)
                button_interdiction[i]--;
        }
        if (button_status[0]==1 && button_interdiction[0]==0)
        {

            printf("Switching to RECORDING mode\n");
            RTB_set_mode(RTB_recording);
            button_status[0]=0;
            button_interdiction[0]=2000000;
            //pause();
        }
        
        if (button_status[1]==1 && button_interdiction[1]==0)
        {
            printf("Switching to TRACKING mode\n");
            RTB_set_mode(RTB_tracking);
            button_status[1]=0;
            button_interdiction[1]=2000000;

            //pause();
        }
        
        if (button_status[2]==1 && button_interdiction[2]==0)
        {
            printf("Switching to IDLE mode\n");
            RTB_set_mode(RTB_idle);
            button_status[2]=0;
            button_interdiction[2]=2000000;
            playerc_graphics2d_clear(PlayerLocalGraphicsMoving);
            playerc_graphics2d_clear(PlayerLocalGraphics2D);
            //pause();
        }
        
        if (!NoJoy)
        {
            SDL_JoystickUpdate();
            vel = SDL_JoystickGetAxis(mystick,1);
            turn = SDL_JoystickGetAxis(mystick,2);
            for (i=0; i<4;i++)
            {
                button[i]=SDL_JoystickGetButton(mystick, i);
                if (button[i] != button_status[i])
                {
                    if (button_debouncingup[i] == 0 && button_debouncingdown[i] == 0)
                        if (button[i] < button_status[i])
                            button_debouncingup[i] = 1;
                        else
                            button_debouncingdown[i]=1;
                        
                    if( button_debouncingup[i]==1 && button[i] < button_status[i] )                    
                        button_counter[i]++;
                    
                    if (button_debouncingdown[i]==1 && button[i] > button_status[i])
                        button_counter[i]++;
                    
                    if ((button_debouncingup[i]==1 && button[i] > button_status[i]) || (button_debouncingdown[i]==1 && button[i] < button_status[i]))
                    {
                        button_counter[i]=0;
                        button_debouncingup[i]=0;
                        button_debouncingdown[i]=0;
                    }
                    
                    if (button_counter[i]>500 && button_debouncingdown[i])
                    {
                        button_counter[i]=0;
                        button_debouncingdown[i]=0;
                        button_status[i]=1;
                    }
                    if (button_counter[i]>200 && button_debouncingup[i])
                    {
                        button_counter[i]=0;
                        button_debouncingup[i]=0;
                        button_status[i]=0;
                    }
                }
                else
                {
                    button_counter[i]=0;
                    button_debouncingup[i]=0;
                    button_debouncingdown[i]=0;
                }
            }
            
            command_vel = COEFF*command_vel + (1-COEFF) * (-vel/32768);
            command_turn = COEFF*command_turn + (1-COEFF) * (-turn/32768);
            
            
        }

    }
    RTB_set_mode(RTB_idle);
    Player_Manager_Disconnect();
    OA_cleanup();
    close(output_pty);
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    
    
    return (EXIT_SUCCESS);
}

