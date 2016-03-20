#ifndef MOM_BOOSTERFIX_H
#define MOM_BOOSTEFIX_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "triggers.h"
#include "doors.h"
#include "mom_player.h"


class CMOMBoosterFixSystem : CAutoGameSystemPerFrame
{

public:
    CMOMBoosterFixSystem(const char* pName) : CAutoGameSystemPerFrame(pName) {};

    virtual void LevelInitPostEntity() 
    { 
        HookSingleTriggers("trigger_multiple", "OnTrigger");
        HookSingleTriggers("trigger_multiple", "OnStartTouch");
        HookSingleTriggers("trigger_multiple", "OnTouching");
        HookSingleTriggers("trigger_multiple", "OnEndTouch");
    }

    virtual void LevelShutdownPostEntity() 
    { 
        // Remove the ents (if stored)
    }

    //Per-frame methods are below, determine which one may be best to use:
    //virtual void PreClientUpdate() // Not sure
    //virtual void FrameUpdatePostEntityThink() // Each frame after entities think
    virtual void FrameUpdatePreEntityThink()// Each frame before ents think
    {

    }

    void HookPushes();
    void HookTriggers(const char* classname, const char* output);
    void HookSingleTriggers(const char* classname, const char* output);
    void HookGravityBoosters(const char* output);

private:
    void RotatePoint(Vector &out, const Vector p, const QAngle angles);
    struct trigger_booster_t
    {
        CBaseEntity* pTriggerEnt;
        float m_fTriggerDelay;
        float m_flTriggerStartDelay;
        float m_flPushScale;
        bool isTouch;
        Vector pushDir;
        QAngle angles;
        string_t filterName;
        CHandle<CBaseEntity> filterEnt;

        //booster type
        char* type;
    };
    CUtlVector<trigger_booster_t> m_vecTriggerBoosterList;
};

class CMOMOutputInfo
{
public:
    static CBaseEntityOutput* GetOutput(int num, const char *sOutput);
    static int GetOutputCount(CBaseEntity *pOther, const char *sOutput);
    static bool GetOutputParamter(CBaseEntity *pOther, const char *sOutput, int iterator, string_t &param);
    static bool GetOutputTargetInput(CBaseEntity *pOther, const char *sOutput, int iterator, string_t &param);
    static float GetOutputDelay(CBaseEntity *pOther, const char *sOutput, int iterator);
};
extern CMOMBoosterFixSystem *g_MOMBoosterFixer;

#endif //MOM_BOOSTERFIX_H