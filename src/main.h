#pragma once

#include <vector>
#include <cstddef>
#include <cmath>
#include "cleo.h"
#include "cleoaddon.h"
#include "isautils.h"

extern cleo_ifs_t* cleo;
extern cleo_addon_ifs_t* cleoaddon;
extern ISAUtils* sautils;

#include "aml-psdk/game_sa/plugin.h"
#include "aml-psdk/gta_base/Vector.h"
#include "rw/rwcore.h"
#include "rw/rpworld.h"
#include "rw/rphanim.h"
#include "aml-psdk/game_sa/entity/Ped.h"
#include "aml-psdk/game_sa/entity/Vehicle.h"
#include "aml-psdk/game_sa/entity/Object.h"
#include "aml-psdk/game_sa/engine/ModelInfo.h"
#include "aml-psdk/game_sa/other/Pools.h"
#include "aml-psdk/game_sa/Events.h"
#include "VisualObject.h"

#define MAX_STR_LEN 0xFF

class PedData
{
public:
    std::vector<VisualObject*> VisualObjects;
    uint16_t ScriptID;
    
    void Reset()
    {
        VisualObjects.clear();
        ScriptID = 0;
    }
};
class VehicleData
{
public:
    std::vector<VisualObject*> VisualObjects;
    uint16_t ScriptID;
    CVehicle* thisVeh;
    
    void Reset()
    {
        VisualObjects.clear();
        ScriptID = 0;
    }
};
class ObjectData
{
public:
    std::vector<VisualObject*> VisualObjects;
    uint16_t ScriptID;
    
    void Reset()
    {
        VisualObjects.clear();
        ScriptID = 0;
    }
};
extern PedData;
extern VehicleData;
extern ObjectData;
std::vector<PedData*> PedExtData;
std::vector<VehicleData*> VehExtData;
std::vector<ObjectData*> ObjExtData;
std::vector<VisualObject*> ObjWorld;
/*
extern uintptr_t* ms_pPedPool;
extern uintptr_t* ms_pVehiclePool;
extern uintptr_t* ms_pObjectPool;
extern int (*GetPedRef)(CPed*);
extern int (*GetVehicleRef)(CVehicle*);
extern int (*GetObjectRef)(CObject*);
*/
extern uintptr_t pGTASA;
extern void* hGTASA;

class PedExtended
{
public:
    static PedData* GetExtData(CPed* ped)
    {
        if (PedExtData.size() == 0)
        {
            for (int i = 0; i < CPools::ms_pPedPool->m_nSize; ++i)
            {
                PedData* xdata = new PedData();
                PedExtData.push_back(xdata);
            }
        }
        return PedExtData.at(CPools::GetPedRef(ped) >> 8);
    }
};
class VehicleExtended
{
public:
    static VehicleData* GetExtData(CVehicle* veh)
    {
        if (VehExtData.size() == 0)
        {
            
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; ++i)
            {
                VehicleData* xdata = new VehicleData();
                VehExtData.push_back(xdata);
            }
        }
        return VehExtData.at(CPools::GetVehicleRef(veh) >> 8);
    }
};
class ObjectExtended
{
public:
    static ObjectData* GetExtData(CObject* obj)
    {
        if (ObjExtData.size() == 0)
        {
            for (int i = 0; i < CPools::ms_pObjectPool->m_nSize; ++i)
            {
                ObjectData* xdata = new ObjectData();
                ObjExtData.push_back(xdata);
            }
        }
        return ObjExtData.at(CPools::GetObjectRef(obj) >> 8);
    }
};
extern PedExtended;
extern VehicleExtended;
extern ObjectExtended;
/*
class ScriptPrivateVar
{
public:
    int VarID;
    std::variant<int, float> value;
    uint16_t ScriptID;
    
    ScriptPrivateVar(int _VarID, std::variant<int, float>& _value)
    {
        VarID = _VarID;
        value = _value;
        ScriptID = 0;
    }
};

0000: set_script_private_var 0 is_float 0 value 0@ 
0000: 0@ = get_script_private_var 0 is_float 0 

CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE);
CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE_FROM_SPECIAL);
CLEO_Fn(CREATE_RENDER_OBJECT_TO_OBJECT);
CLEO_Fn(CREATE_RENDER_OBJECT_TO_OBJECT_FROM_SPECIAL);
CLEO_Fn(DELETE_RENDER_OBJECT);
CLEO_Fn(SET_RENDER_OBJECT_AUTO_HIDE);
CLEO_Fn(SET_RENDER_OBJECT_VISIBLE);
CLEO_Fn(SET_RENDER_OBJECT_POSITION);
CLEO_Fn(SET_RENDER_OBJECT_ROTATION);
CLEO_Fn(SET_RENDER_OBJECT_SCALE);
CLEO_Fn(SET_RENDER_OBJECT_DISTORTION);
*/

    /*
    CLEO_RegisterOpcode(0x0E2E, CREATE_RENDER_OBJECT_TO_CHAR_BONE); // 0E2E=10,create_render_object_to_char_bone %1d% model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% store_to %10d%
    CLEO_RegisterOpcode(0x0F02, CREATE_RENDER_OBJECT_TO_CHAR_BONE_FROM_SPECIAL); // 0F02=10,create_render_object_to_char_bone_from_special %1d% special_model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% scale %10d% %11d% %12d% store_to %13d%
    CLEO_RegisterOpcode(0x0F03, CREATE_RENDER_OBJECT_TO_OBJECT); // 0F03=9,create_render_object_to_object %1d% model %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0F04, CREATE_RENDER_OBJECT_TO_OBJECT_FROM_SPECIAL); // 0F04=9,create_render_object_to_object_from_special %1d% special_model %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0E2F, DELETE_RENDER_OBJECT); // 0E2F=1,delete_render_object %1d%
    CLEO_RegisterOpcode(0x0E30, SET_RENDER_OBJECT_AUTO_HIDE); // 0E30=4,set_render_object_auto_hide %1d% dead %2d% weapon %3d% car %4d%
    CLEO_RegisterOpcode(0x0E31, SET_RENDER_OBJECT_VISIBLE); // 0E31=2,set_render_object_visible %1d% %2d%
    CLEO_RegisterOpcode(0x0E35, SET_RENDER_OBJECT_POSITION); // 0E35=4,set_render_object_position %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E36, SET_RENDER_OBJECT_ROTATION); // 0E36=4,set_render_object_rotation %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E37, SET_RENDER_OBJECT_SCALE); // 0E37=4,set_render_object_scale %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E3A, SET_RENDER_OBJECT_DISTORTION); // 0E3A=5,set_render_object_distortion %1d% %2d% %3d% %4d% %5d%
    */
