/* 
 * File:   player_manager.h
 * Author: erupter
 *
 * Created on August 20, 2012, 3:16 PM
 */

#ifndef PLAYER_MANAGER_H
#define	PLAYER_MANAGER_H


#ifdef	__cplusplus
extern "C" {
#endif

#include "libplayerc/playerc.h"
#include <time.h>
//#include "zhelpers.h"
#include "options.h"

typedef enum {
    PL_CB_GPS = 0,
    PL_CB_ODO,
    PL_CB_LF,
    PL_CB_LB
} _player_manager_callbacks_enum;
    
void Player_Manager (void);
void Player_Manager_Init (int index);
void Player_Manager_ConnSetup(char * host, int port);
void Player_Manager_Connect(void);
void Player_Manager_Disconnect(void);
void Player_Manager_FetchSetup(void);
void Player_Manager_RegisterCallback (_player_manager_callbacks_enum type, playerc_callback_fn_t fcn);
playerc_client_t Player_Manager_GetCurrConf (void);

typedef enum {
    PL_UNKNOWN = 0,
    PL_INIT,
    PL_START,
    PL_CONNECT,
    PL_IDLE,
    PL_CONNECTING,
    PL_CONNECTED,
    PL_DISCONNECT,
    PL_DISCONNECTING,
    PL_DISCONNECTED,
    PL_FECTHING_DATA,
    PL_QUIT,
    PL_ERROR
} _player_manager_states_enum;

extern _player_manager_states_enum PLAYER_MANAGER_STATE;
extern playerc_graphics2d_t *PlayerLocalGraphics2D;


#ifdef	__cplusplus
}
#endif

#endif	/* PLAYER_MANAGER_H */

