#pragma once

#include "cbase.h"
#include "triggers.h"


class CMOMBoosterFixSystem : CAutoGameSystemPerFrame
{

public:
    CMOMBoosterFixSystem(const char* pName) : CAutoGameSystemPerFrame(pName) {};

    virtual void LevelInitPostEntity() 
    { 
        // Find the ents to manipulate
    }

    virtual void LevelShutdownPostEntity() 
    { 
        // Remove the ents (if stored)
    }

    //Per-frame methods are below, determine which one may be best to use:
    //virtual void PreClientUpdate() // Not sure
    //virtual void FrameUpdatePostEntityThink() // Each frame after entities think
    //virtual void FrameUpdatePreEntityThink() // Each frame before ents think


private:

};

extern CMOMBoosterFixSystem *g_MOMBoosterFixer;