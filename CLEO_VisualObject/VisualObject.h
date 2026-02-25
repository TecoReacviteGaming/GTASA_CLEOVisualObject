#pragma once

#include "main.h"

enum eRenderType : int
{
    ATOMIC_RENDERER = 0,
    CLUMP_RENDERER = 1
};

class VisualObject 
{
public:
    RpClump* clump;
    RwFrame* frame;
    RpAtomic* atomic;
    CPed* ped;
    CVehicle* veh;
    CObject* obj;
    int bone_id;
    CVector offset;
    CVector rot;
    CVector scale;
    RwV4d dist;
    bool isVisible;
    bool hideIfDead;
    bool hideIfWeapon;
    bool hideIfCar;
    eRenderType renderType;
    uint16_t ScriptID;
    char dff_name[MAX_STR_LEN];
    int model_id;
    
    VisualObject(RpClump* _clump, RwFrame* _frame, RpAtomic* _atomic, CPed* _ped, CVehicle* _veh, CObject* _obj, int _model_id, int _bone_id, float _x, float _y, float _z, float _rx, float _ry, float _rz, float _sx, float _sy, float _sz, float _dx, float _dy, float _dz, float _dw)
    {
        clump = _clump; 
        frame = _frame; 
        atomic = _atomic; 
        ped = _ped;
        veh = _veh;
        obj = _obj;
        bone_id = _bone_id;
        offset.x = _x; 
        offset.y = _y; 
        offset.z = _z; 
        rot.x = _rx; 
        rot.y = _ry; 
        rot.z = _rz; 
        scale.x = _sx; 
        scale.y = _sy; 
        scale.z = _sz; 
        dist.x = _dx;
        dist.y = _dy;
        dist.z = _dz;
        dist.w = _dw;
        isVisible = true; 
        hideIfDead = false; 
        hideIfWeapon = false; 
        hideIfCar = false; 
        renderType = ATOMIC_RENDERER;
        ScriptID = 0;
        model_id = _model_id;
    }
};