#include "player_manager.h"


_player_manager_states_enum PLAYER_MANAGER_STATE = PL_UNKNOWN;
char _plm_host[30] = "localhost";
int _plm_port = 6665, _plm_index = 0, _plm_once=0;
playerc_client_t *PlayerLocalClient;
playerc_graphics2d_t *PlayerLocalGraphics2D, *PlayerLocalGraphicsMoving;
int PlayerLocalError = 0;      
playerc_position2d_t *PlayerLocalPosition2dAbsolute, *PlayerLocalPosition2dRelative;
playerc_ranger_t *PlayerLocalSensorLaserFront, *PlayerLocalSensorLaserBack;
playerc_callback_fn_t PLM_PosGps_callback = NULL;
playerc_callback_fn_t PLM_PosOdo_callback = NULL;
playerc_callback_fn_t PLM_LaserFront_callback = NULL;
playerc_callback_fn_t PLM_LaserBack_callback = NULL;

void Player_Manager (void)
{
    switch (PLAYER_MANAGER_STATE)
    {
        case PL_UNKNOWN:
            PLAYER_MANAGER_STATE = PL_INIT;
            break;
            
        case PL_INIT:
            PLAYER_MANAGER_STATE = PL_IDLE;
            if (PLM_DEBUG)
                dbg_print(PLM_PROC_NAME,"Initializing\n");
            
            break;
            
        case PL_IDLE:
            if (PLM_DEBUG)
                dbg_print(PLM_PROC_NAME,"Idle\n");
            
            break;            
            
        case PL_CONNECT:
                if (PLM_DEBUG)
                    dbg_print(PLM_PROC_NAME,"Attempting Connection\n");
                
                PlayerLocalClient = playerc_client_create(NULL, _plm_host, _plm_port);
                PlayerLocalError = playerc_client_connect(PlayerLocalClient);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                
                PlayerLocalError = playerc_client_get_devlist(PlayerLocalClient);
                if (PlayerLocalError != 0)
                {
                    if (PLM_DEBUG)
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                }
                PlayerLocalPosition2dAbsolute = playerc_position2d_create (PlayerLocalClient, (2*_plm_index)+1);
                if (playerc_position2d_subscribe(PlayerLocalPosition2dAbsolute, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                
                PlayerLocalPosition2dRelative = playerc_position2d_create (PlayerLocalClient, 2*_plm_index);
                if (playerc_position2d_subscribe(PlayerLocalPosition2dRelative, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                playerc_client_set_replace_rule(PlayerLocalClient,-1, -1, -1, -1, 1);
                PlayerLocalSensorLaserFront = playerc_ranger_create (PlayerLocalClient, (3*_plm_index)+1);
                if (playerc_ranger_subscribe(PlayerLocalSensorLaserFront, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                
                PlayerLocalSensorLaserBack = playerc_ranger_create (PlayerLocalClient, (3*_plm_index)+2);
                if (playerc_ranger_subscribe(PlayerLocalSensorLaserBack, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                
                PlayerLocalGraphics2D = playerc_graphics2d_create(PlayerLocalClient, 0);
                if (playerc_graphics2d_subscribe(PlayerLocalGraphics2D, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                
                PlayerLocalGraphicsMoving = playerc_graphics2d_create(PlayerLocalClient, 1);
                if (playerc_graphics2d_subscribe(PlayerLocalGraphicsMoving, PLAYER_OPEN_MODE))
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }                
                                
                PLAYER_MANAGER_STATE = PL_CONNECTING;
                if (PLM_DEBUG)
                    dbg_print(PLM_PROC_NAME,"Connection successful\n");
                //playerc_position2d_enable(PlayerLocalPosition2dRelative, 1);
                //playerc_position2d_set_cmd_vel (PlayerLocalPosition2dRelative, 0,0,0.5,0);

            break;
            
        case PL_CONNECTING:
                if (PLM_DEBUG)
                    dbg_print(PLM_PROC_NAME,"Setting up data mode\n");
                
                if (PlayerLocalError = playerc_client_datamode(PlayerLocalClient,PLAYERC_DATAMODE_PUSH) != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());

                    PLAYER_MANAGER_STATE = PL_ERROR;
                    break;
                }
                playerc_client_addcallback(PlayerLocalClient,&PlayerLocalPosition2dAbsolute->info,PLM_PosGps_callback,PlayerLocalPosition2dAbsolute);
                playerc_client_addcallback(PlayerLocalClient,&PlayerLocalPosition2dRelative->info,PLM_PosOdo_callback,PlayerLocalPosition2dRelative);
                playerc_client_addcallback(PlayerLocalClient,&PlayerLocalSensorLaserFront->info,PLM_LaserFront_callback,PlayerLocalSensorLaserFront);
                playerc_client_addcallback(PlayerLocalClient,&PlayerLocalSensorLaserBack->info,PLM_LaserBack_callback,PlayerLocalSensorLaserBack);
                
                
                PLAYER_MANAGER_STATE = PL_CONNECTED;
            break;
            
        case PL_CONNECTED:
                PlayerLocalError = playerc_client_read_nonblock(PlayerLocalClient);
                if (PlayerLocalError < 0 && PLM_DEBUG > 0)
                    dbg_print(PLM_PROC_NAME,"\n%s\n",playerc_error_str());
                /*if (!_plm_once)
                {
                    
                    playerc_position2d_set_odom(PlayerLocalPosition2dRelative,PlayerLocalPosition2dAbsolute->px,PlayerLocalPosition2dAbsolute->py,PlayerLocalPosition2dAbsolute->pa);
                    _plm_once=1;
                }*/
                    //dbg_print(PLM_PROC_NAME,"Position: GPS %f %f %f\tODO %f %f %f\n",PlayerLocalPosition2dAbsolute->px, PlayerLocalPosition2dAbsolute->py, PlayerLocalPosition2dAbsolute->pa,PlayerLocalPosition2dRelative->px, PlayerLocalPosition2dRelative->py, PlayerLocalPosition2dRelative->pa);
            break;
            
        case PL_DISCONNECT:
                if (PLM_DEBUG)
                    dbg_print(PLM_PROC_NAME,"Disconnecting\n");
                PlayerLocalError = playerc_position2d_unsubscribe(PlayerLocalPosition2dAbsolute);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_position2d_destroy(PlayerLocalPosition2dAbsolute);
                
                PlayerLocalError = playerc_position2d_unsubscribe(PlayerLocalPosition2dRelative);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_position2d_destroy(PlayerLocalPosition2dRelative);
                
                PlayerLocalError = playerc_ranger_unsubscribe(PlayerLocalSensorLaserFront);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_ranger_destroy(PlayerLocalSensorLaserFront);
                                
                PlayerLocalError = playerc_ranger_unsubscribe(PlayerLocalSensorLaserBack);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_ranger_destroy(PlayerLocalSensorLaserBack);

                
                PlayerLocalError = playerc_graphics2d_unsubscribe(PlayerLocalGraphics2D);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_graphics2d_destroy(PlayerLocalGraphics2D);
                
                PlayerLocalError = playerc_graphics2d_unsubscribe(PlayerLocalGraphicsMoving);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                else
                    playerc_graphics2d_destroy(PlayerLocalGraphicsMoving);
                                
                PlayerLocalError = playerc_client_disconnect(PlayerLocalClient);
                if (PlayerLocalError != 0 )
                {
                    if (PLM_DEBUG)                    
                        dbg_print(PLM_PROC_NAME, "\n%s\n",playerc_error_str());
                    else
                        fprintf(outstream,"Player Client error: %s\n",playerc_error_str());
                }
                
                PLAYER_MANAGER_STATE = PL_DISCONNECTED;
                playerc_client_destroy(PlayerLocalClient);
                if (PLM_DEBUG)
                    dbg_print(PLM_PROC_NAME,"Disconnected\n");
                
                break;
    }

}


void Player_Manager_Connect (void)
{
    PLAYER_MANAGER_STATE = PL_CONNECT;
    if (PLM_DEBUG)
        dbg_print(PLM_PROC_NAME,"Received instruction to connect\n");
    
}

void Player_Manager_Disconnect (void)
{
    if (PLAYER_MANAGER_STATE == PL_CONNECTED)
    {
        PLAYER_MANAGER_STATE = PL_DISCONNECT;
        if (PLM_DEBUG)
            dbg_print(PLM_PROC_NAME,"Received instruction to disconnect\n");
        
        while (PLAYER_MANAGER_STATE != PL_DISCONNECTED)
                Player_Manager();
    }
}

playerc_client_t Player_Manager_GetCurrConf (void)
{
    return *PlayerLocalClient;
}

void Player_Manager_Init(int index)
{
    _plm_index = index;
}

void Player_Manager_RegisterCallback (_player_manager_callbacks_enum type, playerc_callback_fn_t fcn)
{
    switch (type)
    {
        case PL_CB_GPS:
            PLM_PosGps_callback = fcn;
            break;
            
        case PL_CB_ODO:
            PLM_PosOdo_callback = fcn;
            break;
            
        case PL_CB_LF:
            PLM_LaserFront_callback = fcn;
            break;
            
        case PL_CB_LB:
            PLM_LaserBack_callback = fcn;
            break;
            
    }
}

