#ifndef MOM_SHAREDDEFS_H
#define MOM_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif


#include "const.h"
#include "shareddefs.h"

// Gamemode for momentum
typedef enum MOMGM
{
    MOMGM_UNKNOWN = 0,
    MOMGM_SURF,
    MOMGM_BHOP,
    MOMGM_SCROLL,
    MOMGM_ALLOWED, //not "official gamemode" but must be allowed for other reasons
    
} GAMEMODES;

typedef enum RUNFLAGS
{
    FLAG_66TICK = 0x0001,
    FLAG_100TICK = 0x0002,
    FLAG_SCROLL_ONLY = 0x0004,
    FLAG_W_ONLY = 0x0008,
    FLAG_HALF_SIDEWAYS = 0x0010,
    FLAG_BACKWARDS_ONLY = 0x0020,
    FLAG_SIDEWAYS_ONLY = 0x0040,
    FLAG_BONUS_STAGE = 0x0080
    
} FLAGS;

#define PANEL_TIMES "times"

// Main Version (0 is alpha, 1 is beta, 2 is release)​.Main feature push (increment by one for each)​.​Small commits or hotfixes​
// When editing this, remember to also edit version.txt on the main dir of the repo
// If you have any doubts, please refer to http://semver.org/
#define MOM_CURRENT_VERSION "0.2.5"

#endif // MOM_SHAREDDEFS_H