#include "cbase.h"
#include "mom_boosterfix.h"

#include "tier0/memdbgon.h"

void CMOMBoosterFixSystem::HookPushes()
{
    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, "trigger_push")) != NULL)
    {
        CTriggerPush *pEntPush = static_cast<CTriggerPush *>(ent);

        trigger_booster_t booster = trigger_booster_t();

        booster.pushDir = pEntPush->m_vecPushDir;
        booster.angles = pEntPush->GetLocalAngles();
        booster.m_flPushScale = pEntPush->m_flSpeed;
        booster.filterName = pEntPush->m_iFilterName;
        booster.filterEnt = pEntPush->m_hFilter;
        booster.pTriggerEnt = pEntPush;
        booster.type = "single_trigger_push";

        if (booster.angles[0] || booster.angles[1] || booster.angles[2])
        {
            RotatePoint(booster.pushDir, booster.pushDir, booster.angles);
            DevLog("Has angle, rotating pushdir by %f, %f, %f", booster.angles[0], booster.angles[1], booster.angles[2]);
        }

        m_vecTriggerBoosterList.AddToTail(booster);
    }
}
void CMOMBoosterFixSystem::HookTriggers(const char* classname, const char* output)
{
    CBaseEntity *ent = NULL;
    //find a trigger based on passed classname  
    while ((ent = gEntList.FindEntityByClassname(ent, classname)) != NULL)
    {
        int count = CMOMOutputInfo::GetOutputCount(ent, output); // # of outputs in the trigger
        bool bDoubleBreak = false;
        //search through outputs
        for (int i = 0; i < count && !bDoubleBreak; i++)
        {
            string_t param;
            CMOMOutputInfo::GetOutputParamter(ent, output, i, param);
            for (int j = 0; j < m_vecTriggerBoosterList.Size() && !bDoubleBreak; j++)
            {
                string_t compare;
                //compare the filter name with the paramter passed to the trigger
                compare = m_vecTriggerBoosterList[j].filterName;
                if (param == compare)
                {
                    bool bFound = false;
                    for (int k = 0; k < count; k++)
                    {
                        string_t param2;
                        CMOMOutputInfo::GetOutputParamter(ent, output, k, param2);
                        //check if the parameter changes the player's targetname
                        if (Q_strstr(param2.ToCStr(), "targetname ") != NULL && !Q_strcmp(param2.ToCStr(), compare.ToCStr()))
                        {
                            trigger_booster_t booster = trigger_booster_t();
                            booster.m_fTriggerDelay = CMOMOutputInfo::GetOutputDelay(ent, output, k);
                            booster.m_flTriggerStartDelay = CMOMOutputInfo::GetOutputDelay(ent, output, i);
                            booster.pTriggerEnt = ent;
                            booster.type = "multiple_";

                            if (!Q_strcmp(classname, "trigger_multiple"))
                                booster.isTouch = true;
                            else if (!Q_strcmp(classname, "func_button") || !Q_strcmp(classname, "func_physbox"))
                                booster.isTouch = false;

                            m_vecTriggerBoosterList.AddToTail(booster);

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
    CBaseEntity *ent = NULL;
    while ((ent = gEntList.FindEntityByClassname(ent, classname)) != NULL)
    {
        int count = CMOMOutputInfo::GetOutputCount(ent, output);
        bool bDoubleBreak = false;

        trigger_booster_t booster = trigger_booster_t();

        for (int i = 0; i < count && !bDoubleBreak; i++)
        {
            string_t param;
            CMOMOutputInfo::GetOutputParamter(ent, output, i, param);
            if (Q_strstr(param.ToCStr(), "targetname ") != NULL)
            {
                booster.filterName = param; //MOM_TODO: FIX THIS. Should be whatever substring is after "targetname ", not the entire param string
                booster.m_fTriggerDelay = CMOMOutputInfo::GetOutputDelay(ent, output, i);
            }

        }
        booster.pTriggerEnt = ent;
        booster.m_flTriggerStartDelay = 0;
        m_vecTriggerBoosterList.AddToTail(booster);
        DevLog("I'm adding triggers and shit niggah\n");
        //HookSingleEntityOutput(ent, sOutput, OnTriggerSingle);
    }
}
void CMOMBoosterFixSystem::HookGravityBoosters(const char* output)
{
   
}
void CMOMBoosterFixSystem::RotatePoint(Vector &out, const Vector p, const QAngle angles)
{
    Vector sine, cosine, temp;

    sine[0] = sin(angles[0] * M_PI / 180.0);
    sine[1] = sin(angles[1] * M_PI / 180.0);
    sine[2] = sin(angles[2] * M_PI / 180.0);
    cosine[0] = cos(angles[0] * M_PI / 180.0);
    cosine[1] = cos(angles[1] * M_PI / 180.0);
    cosine[2] = cos(angles[2] * M_PI / 180.0);

    temp[0] = sine[1] * cosine[0] * p[0] + (cosine[1] * sine[0] * sine[2] - sine[1] * cosine[2]) * p[1] + (sine[1] * sine[2] + sine[1] * sine[0] * sine[2]) * p[2];
    temp[1] = sine[1] * cosine[0] * p[0] + (cosine[1] * cosine[2] + sine[1] * sine[0] * sine[2]) * p[1] + (sine[1] * sine[0] * sine[2] - sine[1] * sine[2]) * p[2];
    temp[2] = cosine[0] * sine[2] * p[1] + cosine[0] * cosine[2] * p[2] - sine[0] * p[0];

    out = temp;
}
// CBaseEntity output stuff
CBaseEntityOutput* CMOMOutputInfo::GetOutput(int entindex, const char *sOutput)
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
        else
            typedesc = NULL;
    }
    if (!typedesc)
        return 0;

    int dmap = typedesc->fieldOffset[TD_OFFSET_NORMAL];

    return (CBaseEntityOutput *)((int)pEntity + (int)dmap);
}
int CMOMOutputInfo::GetOutputCount(CBaseEntity *pOther, const char *sOutput)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    return pOutput->NumberOfElements();
}
bool CMOMOutputInfo::GetOutputParamter(CBaseEntity *pOther, const char *sOutput, int i, string_t &param)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    if (!pOutput->m_ActionList)
        return false;
    CEventAction *pActionList = pOutput->m_ActionList;

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    param = pActionList->m_iParameter;
    return true;
}
bool CMOMOutputInfo::GetOutputTargetInput(CBaseEntity *pOther, const char *sOutput, int i, string_t &param)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);
    if (!pOutput->m_ActionList)
        return false;
    CEventAction *pActionList = pOutput->m_ActionList;

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    param = pActionList->m_iTargetInput;
    return true;
}
float CMOMOutputInfo::GetOutputDelay(CBaseEntity *pOther, const char *sOutput, int i)
{
    CBaseEntityOutput *pOutput = GetOutput(pOther->entindex(), sOutput);

    if (!pOutput->m_ActionList)
        return false;
    CEventAction *pActionList = pOutput->m_ActionList;

    for (int k = 0; k < i; k++)
        pActionList = pActionList->m_pNext;

    return pActionList->m_flDelay;
}
static CMOMBoosterFixSystem s_MOMBoosterFixer("CMOMBoosterFixSystem");
CMOMBoosterFixSystem *g_MOMBoosterFixer = &s_MOMBoosterFixer;