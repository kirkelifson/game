#include "cbase.h"
#include "mom_boosterfix.h"

#include "tier0/memdbgon.h"




static CMOMBoosterFixSystem s_MOMBoosterFixer("CMOMBoosterFixSystem");
CMOMBoosterFixSystem *g_MOMBoosterFixer = &s_MOMBoosterFixer;