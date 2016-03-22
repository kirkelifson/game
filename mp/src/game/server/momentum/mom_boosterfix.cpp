#include "cbase.h"
#include "mom_boosterfix.h"

#include "tier0/memdbgon.h"

void CMOMBoosterFixSystem::HookPushes()
{
    m_vTrggerPushes.EnsureCapacity(MAX_BOOSTERS);

    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, "trigger_push")) != NULL)
    {
        CTriggerPush *pEntPush = static_cast<CTriggerPush *>(ent);

        booster_push_t booster = booster_push_t();

        booster.m_vPushDir = pEntPush->m_vecPushDir;
        booster.m_qaAngles = pEntPush->m_vecFinalAngle;
        booster.m_flPushScale = pEntPush->m_flSpeed;
        booster.filterName = pEntPush->m_iFilterName;
        booster.pBoosterEnt = ent;

        if (booster.m_qaAngles[0] || booster.m_qaAngles[1] || booster.m_qaAngles[2])
        {
            RotatePoint(booster.m_vPushDir, booster.m_vPushDir, booster.m_qaAngles);
            DevLog("Has angle, rotating pushdir by %f, %f, %f", booster.m_qaAngles[0], booster.m_qaAngles[1], booster.m_qaAngles[2]);
        }
        
        m_vTrggerPushes.AddToTail(booster);
    }
}
void CMOMBoosterFixSystem::HookTriggers(const char* classname, const char* output)
{
    m_vTriggers.EnsureCapacity(MAX_BOOSTERS);

    __format_string("m_%s", output);
    CBaseEntity *ent = NULL;
    //find a trigger based on passed classname  
    while ((ent = gEntList.FindEntityByClassname(ent, classname)) != NULL)
    {
        int count = CMOMEntityOutputInfo::GetOutputCount(ent, output); // # of outputs in the trigger
        bool bDoubleBreak = false;
        //search through outputs
        for (int i = 0; i < count && !bDoubleBreak; i++)
        {
            string_t param;
            CMOMEntityOutputInfo::GetOutputParamter(ent, output, i, param);
            for (int j = 0; j < m_vTrggerPushes.Size() && !bDoubleBreak; j++)
            {
                //compare the filter name with the paramter passed to the trigger
                string_t compare = m_vTrggerPushes[j].filterName;
                if (param == compare)
                {
                    bool bFound = false;
                    for (int k = 0; k < count; k++)
                    {
                        string_t param2;
                        CMOMEntityOutputInfo::GetOutputParamter(ent, output, k, param2);
                        //check if the parameter changes the player's targetname
                        if (Q_strstr(param2.ToCStr(), "targetname ") != NULL && !Q_strcmp(param2.ToCStr(), compare.ToCStr()))
                        {
                            booster_trigger_t booster = booster_trigger_t();
                            booster.filterName = m_vTrggerPushes[j].filterName;
                            booster.m_fTriggerDelay = CMOMEntityOutputInfo::GetOutputDelay(ent, output, k);
                            booster.m_flTriggerStartDelay = CMOMEntityOutputInfo::GetOutputDelay(ent, output, i);
                            booster.pBoosterEnt = ent;

                            if (!Q_strcmp(classname, "func_button") || !Q_strcmp(classname, "func_physbox"))
                                break; //don't want to add shootboosts to our boosterfix (not yet anyway)
                            
                            m_vTriggers.AddToTail(booster);

                            bFound = true;
                            break;
                        }
                    }
                    if (bFound)
                    {
                        if (!Q_strcmp(classname, output))
                        {
                            //HookSingleEntityOutput(ent, sOutput, OnTrigger);
                        }
                    }
                    bDoubleBreak = true;
                }
            }
        }
    }
}
void CMOMBoosterFixSystem::HookSingleTriggers(const char* classname, const char* output)
{
    __format_string("m_%s", output);
    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, classname)) != NULL)
    {
        int count = CMOMEntityOutputInfo::GetOutputCount(ent, output);
        bool bDoubleBreak = false;

        booster_trigger_t booster = booster_trigger_t();

        for (int i = 0; i < count && !bDoubleBreak; i++)
        {
            string_t param;
            CMOMEntityOutputInfo::GetOutputParamter(ent, output, i, param);
            if (Q_strstr(param.ToCStr(), "targetname ") != NULL)
            {
                char cstrParam[64];
                for (size_t j = 0; j < strlen(param.ToCStr()) - strlen("targetname" ) + 1; j++)
                    cstrParam[j] = param.ToCStr()[j + strlen("targetname ")];
                booster.filterName = castable_string_t(cstrParam);
                booster.m_fTriggerDelay = CMOMEntityOutputInfo::GetOutputDelay(ent, output, i);
            }

        }
        booster.pBoosterEnt = ent;
        booster.m_flTriggerStartDelay = 0;
        m_vTriggers.AddToTail(booster);
        //HookSingleEntityOutput(ent, sOutput, OnTriggerSingle);
    }
}
void CMOMBoosterFixSystem::HookGravityBoosters(const char* output)
{
    m_vGravityBoosters.EnsureCapacity(MAX_BOOSTERS);

    __format_string("m_%s", output);
    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, "trigger_multiple")) != NULL)
    {
        int count = CMOMEntityOutputInfo::GetOutputCount(ent, output);
        bool bGravityFound = false;
        bool bDefaultFound = false;
        float fGravity = 0, fGravityStartDelay = 0, fGravityDelay = 0;
        for (int i = 0; i < count; i++)
        {
            string_t param;
            CMOMEntityOutputInfo::GetOutputParamter(ent, output, i, param);

            //our output parameter contains "gravity" and not "gravity 1"
            if (Q_strstr(param.ToCStr(), "gravity ") != NULL && Q_strstr(param.ToCStr(), "gravity 1") == NULL)
            {
                char cstrGravity[16];
                for (size_t j = 0; j < strlen(param.ToCStr()) - strlen("gravity ") + 1; j++)
                    cstrGravity[j] = param.ToCStr()[j + strlen("gravity ")];
                bGravityFound = true;
                fGravity = strtof(cstrGravity, NULL);
                fGravityStartDelay = CMOMEntityOutputInfo::GetOutputDelay(ent, output, i);
            }
            else if (Q_strstr(param.ToCStr(), "gravity 1") != NULL)
            {
                fGravityDelay = CMOMEntityOutputInfo::GetOutputDelay(ent, output, i);
                bDefaultFound = true;
            }
        }
        // we found a gravity booster and successfully stored it's params in local variables
        if (bGravityFound && bDefaultFound)
        {
            CTriggerMultiple *pEntMulti = static_cast<CTriggerMultiple *>(ent);

            booster_gravity_t gravBooster = booster_gravity_t();
            gravBooster.pBoosterEnt = ent;
            gravBooster.filterName = pEntMulti->m_iFilterName;
            gravBooster.m_flGravityValue = fGravity;
            gravBooster.m_flGraivtyStartDelay = fGravityStartDelay;
            gravBooster.m_flGravityDelay = fGravityDelay;

            
            m_vGravityBoosters.AddToTail(gravBooster);

            /*
            if (StrEqual(sOutput, "OnStartTouch"))
			{
				SDKHook(ent, SDKHook_StartTouch, OnGenericTouchGravity);
			}
			else if (StrEqual(sOutput, "OnTouch"))
			{
				SDKHook(ent, SDKHook_Touch, OnGenericTouchGravity);
			}
			else if (StrEqual(sOutput, "OnEndTouch"))
			{
				SDKHook(ent, SDKHook_EndTouch, OnGenericTouchGravity);
			}
			else if (StrEqual(sOutput, "OnTrigger"))
			{
				SDKHook(ent, SDKHook_StartTouch, OnGenericTouchGravity);
			}*/
        }
    }
}
void CMOMBoosterFixSystem::HookFuncDoors()
{
    m_vFuncDoors.EnsureCapacity(MAX_BOOSTERS);
    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, "func_door")) != NULL)
    {
        CBaseDoor *pEntDoor = static_cast<CBaseDoor *>(ent);
        Vector pushDir = pEntDoor->m_vecMoveDir;

        if (!pEntDoor->HasSpawnFlags(SF_DOOR_PTOUCH))
        {
            DevLog("Skipping door that doesn't have SF_DOOR_PTOUCH, entindex %i", pEntDoor->entindex());
            continue;
        }
        if (pushDir.z != 1.0)
            continue;
        if (pEntDoor->m_flWait > 0.1)
        {
            DevLog("Skipping door with m_flWait over 0.1, entindex %i", pEntDoor->entindex());
            continue;
        }
        booster_func_door_t doorBooster = booster_func_door_t();
        doorBooster.pBoosterEnt = ent;
        doorBooster.m_flFuncDoorSpeed = pEntDoor->m_flSpeed;
        m_vFuncDoors.AddToTail(doorBooster);
        //SDKHook(ent, SDKHook_StartTouch, OnStartTouchFuncDoor);
        //SDKHook(ent, SDKHook_Touch, OnTouchFuncDoor);
    }
}
//Miu's sourcemod plugin hooks basevel boosters too. we probably don't want to do this, they work 100% fine as-is.

//rotate a vector around a point according to given angles
void CMOMBoosterFixSystem::RotatePoint(Vector &out, const Vector point, const QAngle angles)
{
    Vector sine, cosine, temp;

    sine[0] = sin(angles[0] * M_PI / 180.0);
    sine[1] = sin(angles[1] * M_PI / 180.0);
    sine[2] = sin(angles[2] * M_PI / 180.0);
    cosine[0] = cos(angles[0] * M_PI / 180.0);
    cosine[1] = cos(angles[1] * M_PI / 180.0);
    cosine[2] = cos(angles[2] * M_PI / 180.0);

    temp[0] = sine[1] * cosine[0] * point[0] + (cosine[1] * sine[0] * sine[2] - sine[1] * cosine[2]) * point[1] + (sine[1] * sine[2] + sine[1] * sine[0] * sine[2]) * point[2];
    temp[1] = sine[1] * cosine[0] * point[0] + (cosine[1] * cosine[2] + sine[1] * sine[0] * sine[2]) * point[1] + (sine[1] * sine[0] * sine[2] - sine[1] * sine[2]) * point[2];
    temp[2] = cosine[0] * sine[2] * point[1] + cosine[0] * cosine[2] * point[2] - sine[0] * point[0];

    out = temp;
}

// CBaseEntity output info stuff
CBaseEntityOutput* CMOMEntityOutputInfo::GetOutput(int entindex, const char *sOutput)
{
    edict_t *pEdict = engine->PEntityOfEntIndex(entindex);
    CBaseEntity *pEntity = pEdict->GetUnknown()->GetBaseEntity();

    datamap_t *pMap = pEntity->GetBaseMap();
    if (!pMap)
        return 0;

    typedescription_t *typedesc = NULL;
    for (int i = 0; i < pMap->packed_size; i++)
    {
        if (pMap[i].dataClassName == sOutput)
            typedesc = pMap->dataDesc;
    }

    if (!typedesc)
        return NULL;

    int dmap = typedesc->fieldOffset[TD_OFFSET_NORMAL];

    return (CBaseEntityOutput *)((int)pEntity + (int)dmap);
}
int CMOMEntityOutputInfo::GetOutputCount(CBaseEntity *pOther, const char *sOutput)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    return pOutput->NumberOfElements();
}
bool CMOMEntityOutputInfo::GetOutputParamter(CBaseEntity *pOther, const char *sOutput, int i, string_t &param)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    if (!pOutput->get_m_ActionList())
        return false;
    CEventAction *pActionList = pOutput->get_m_ActionList();

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    param = pActionList->m_iParameter;
    return true;
}
bool CMOMEntityOutputInfo::GetOutputTargetInput(CBaseEntity *pOther, const char *sOutput, int i, string_t &param)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    if (!pOutput->get_m_ActionList())
        return false;
    CEventAction *pActionList = pOutput->get_m_ActionList();

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    param = pActionList->m_iTargetInput;
    return true;
}
float CMOMEntityOutputInfo::GetOutputDelay(CBaseEntity *pOther, const char *sOutput, int i)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);

    if (!pOutput->get_m_ActionList())
        return false;
    CEventAction *pActionList = pOutput->get_m_ActionList();

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    return pActionList->m_flDelay;
}
static CMOMBoosterFixSystem s_MOMBoosterFixer("CMOMBoosterFixSystem");
CMOMBoosterFixSystem *g_MOMBoosterFixer = &s_MOMBoosterFixer;