#ifndef MOM_BOOSTERFIX_H
#define MOM_BOOSTEFIX_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "triggers.h"
#include "doors.h"
#include "mom_player.h"

#define MAX_BOOSTERS 1024
class CMOMBoosterFixSystem : CAutoGameSystemPerFrame
{

public:
    CMOMBoosterFixSystem(const char* pName) : CAutoGameSystemPerFrame(pName) {};

    virtual void LevelInitPostEntity() 
    {
        HookPushes();

        //HookTriggers("trigger_multiple", "OnTrigger");
        //HookTriggers("trigger_multiple", "OnStartTouch");
        //HookTriggers("trigger_multiple", "OnTouching");
        //HookTriggers("trigger_multiple", "OnEndTouch");
        
        //HookSingleTriggers("trigger_multiple", "OnTrigger");
        //HookSingleTriggers("trigger_multiple", "OnStartTouch");
        //HookSingleTriggers("trigger_multiple", "OnTouching");
        //HookSingleTriggers("trigger_multiple", "OnEndTouch");
        //HookSingleTriggers("func_button", "OnPressed");

        //HookGravityBoosters("OnTrigger");
        //HookGravityBoosters("OnStartTouch");
        //HookGravityBoosters("OnEndTouch");

        //HookFuncDoors();
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
    void HookFuncDoors();


private:
    void RotatePoint(Vector &out, const Vector point, const QAngle angles);

    struct booster_push_t
    {
        CBaseEntity* pBoosterEnt;
        string_t filterName;
        //only static trigger_push entities use these
        float m_flPushScale;
        Vector m_vPushDir;
        QAngle m_qaAngles;
    };
    struct booster_trigger_t
    {
        CBaseEntity* pBoosterEnt;
        string_t filterName;
        float m_fTriggerDelay, m_flTriggerStartDelay;
    };
    struct booster_func_door_t
    {
        CBaseEntity* pBoosterEnt;
        float m_flFuncDoorSpeed, m_flStartTime;
    };
    struct booster_gravity_t
    {
        CBaseEntity* pBoosterEnt;
        string_t filterName;
        float m_flGravityValue, m_flGraivtyStartDelay, m_flGravityDelay;
    };
    CUtlVector<booster_trigger_t> m_vTriggers;
    CUtlVector<booster_push_t> m_vTrggerPushes;
    CUtlVector<booster_func_door_t> m_vFuncDoors;
    CUtlVector<booster_gravity_t> m_vGravityBoosters;

};

class CMOMEntityOutputInfo
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