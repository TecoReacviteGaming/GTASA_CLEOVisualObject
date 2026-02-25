#pragma once

#include <vector>
#include <cstddef>
#include <cmath>
#include <cstring>
#include <cstdio>

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

#define MAX_STR_LEN 0xFF

#include "VisualObject.h"

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
std::vector<PedData*> PedExtData;
std::vector<VehicleData*> VehExtData;
std::vector<ObjectData*> ObjExtData;
std::vector<VisualObject*> ObjWorld;
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