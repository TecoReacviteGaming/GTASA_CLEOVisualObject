#include <mod/amlmod.h>
#include <mod/logger.h>
#include "main.h"

MYMOD(net.teco.visualobject, CLEO VisualObject, 1.0.1, TecoReacvite)
NEEDGAME(com.rockstargames.gtasa) // only SA
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.cleolib, 2.0.1.10)
END_DEPLIST()

uintptr_t pGTASA;
void* hGTASA;
cleo_ifs_t* cleo = nullptr;
cleo_addon_ifs_t* cleoaddon = nullptr;
ISAUtils* sautils = nullptr;

RwFrame* (*RwFrameCreate)();
void (*RwFrameDestroy)(RwFrame*);
void (*RwFrameTransform)(RwFrame*, RwMatrix*, RwOpCombineType);
void (*RwFrameTranslate)(RwFrame*, CVector*, RwOpCombineType);
void (*RwFrameScale)(RwFrame*, CVector*, RwOpCombineType);
void (*RwFrameRotate)(RwFrame*, CVector*, float, RwOpCombineType);
void (*RwFrameUpdateObjects)(RwFrame*);

RpAtomic* (*GetFirstAtomic)(RpClump*);
void (*RpAtomicSetFrame)(RpAtomic*, RwFrame*);
void (*RpAtomicDestroy)(RpAtomic*);
RpAtomic* (*RpAtomicClone)(RpAtomic*);

void (*RpClumpDestroy)(RpClump*);
void (*RpClumpRender)(RpClump*);
RpClump* (*RpClumpStreamRead)(RwStream*);

void (*RpClumpForAllAtomics)(RpClump*, RpAtomic*(*)(RpAtomic*, void*), void*);
void (*RpGeometryForAllMaterials)(RpGeometry*, RpMaterial*(*)(RpMaterial*, void*), void*);

RpHAnimHierarchy* (*GetAnimHierarchyFromSkinClump)(RpClump*);
int (*RpHAnimIDGetIndex)(RpHAnimHierarchy*, int);
RwMatrix* (*RpHAnimHierarchyGetMatrixArray)(RpHAnimHierarchy*);
void (*RwV3dTransformPoints)(RwV3d*, RwV3d*, int, RwMatrix*);

// inline functions
inline VisualObject* CreateVisualObjectOnChar(CPed* ped, int modelId, const char* fmt, int bone, float x, float y, float z, float rx, float ry, float rz)
{
    VisualObject* visual = nullptr;
    
    if (modelId == -1)
    {
        RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, fmt);
        if (stream)
        {
            if (RwStreamFindChunk(stream, 16, 0, 0))
            {
                RpClump* clump = RpClumpStreamRead(stream);
                if (clump)
                {
                    RpAtomic* atomic = GetFirstAtomic(clump);
                    if (atomic)
                    {
                        RwFrame* frame = RwFrameCreate();
                        RpAtomicSetFrame(atomic, frame);
                        RwFrameUpdateObjects(frame);
                        
                        visual = new VisualObject(clump, frame, atomic, ped, nullptr, nullptr, -1, bone, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                        const char* filename = strrchr(fmt, '/');
                        filename = filename ? filename + 1 : fmt;
                        strcpy(visual->dff_name, filename);
                    }
                }
            }
            RwStreamClose(stream, 0);
        }
    }
    else
    {
        CBaseModelInfo* modelInfo = CModelInfo::ms_modelInfoPtrs[modelId];
        int rwModelType = modelInfo->GetRwModelType();
        if (rwModelType == 1)
        {
            RpAtomic* atomic = (RpAtomic*)modelInfo->m_pRwObject;
            if (atomic)
            {
                modelInfo->AddRef();
                
                RwFrame* rwFrame = RwFrameCreate();
                RpAtomic* rpAtomic = RpAtomicClone(atomic);
                RpAtomicSetFrame(rpAtomic, rwFrame);
                
                visual = new VisualObject(nullptr, rwFrame, rpAtomic, ped, nullptr, nullptr, modelId, bone, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            RpClump* clump = (RpClump*)modelInfo->m_pRwObject;
            if (clump)
            {
                modelInfo->AddRef();
                
                RpClump* rpClump = (RpClump*)reinterpret_cast<CClumpModelInfo*>(modelInfo)->CreateInstance();
                
                visual = new VisualObject(rpClump, nullptr, nullptr, ped, nullptr, nullptr, modelId, bone, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                visual->renderType = CLUMP_RENDERER;
            }
        }
    }
    if (visual)
    {
        PedData* xdata = PedExtended::GetExtData(ped);
        if (xdata != nullptr) xdata->VisualObjects.push_back(visual);
    }
    return visual;
}
inline VisualObject* CreateVisualObjectOnVehicle(CVehicle* veh, int modelId, const char* fmt, float x, float y, float z, float rx, float ry, float rz)
{
    VisualObject* visual = nullptr;
    
    if (modelId == -1)
    {
        RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, fmt);
        if (stream)
        {
            if (RwStreamFindChunk(stream, 16, 0, 0))
            {
                RpClump* clump = RpClumpStreamRead(stream);
                if (clump)
                {
                    RpAtomic* atomic = GetFirstAtomic(clump);
                    if (atomic)
                    {
                        RwFrame* frame = RwFrameCreate();
                        RpAtomicSetFrame(atomic, frame);
                        RwFrameUpdateObjects(frame);
                        
                        visual = new VisualObject(clump, frame, atomic, nullptr, veh, nullptr, -1, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                        const char* filename = strrchr(fmt, '/');
                        filename = filename ? filename + 1 : fmt;
                        strcpy(visual->dff_name, filename);
                    }
                }
            }
            RwStreamClose(stream, 0);
        }
    }
    else
    {
        CBaseModelInfo* modelInfo = CModelInfo::ms_modelInfoPtrs[modelId];
        int rwModelType = modelInfo->GetRwModelType();
        if (rwModelType == 1)
        {
            RpAtomic* atomic = (RpAtomic*)modelInfo->m_pRwObject;
            if (atomic)
            {
                modelInfo->AddRef();
                
                RwFrame* rwFrame = RwFrameCreate();
                RpAtomic* rpAtomic = RpAtomicClone(atomic);
                RpAtomicSetFrame(rpAtomic, rwFrame);
                
                visual = new VisualObject(nullptr, rwFrame, rpAtomic, nullptr, veh, nullptr, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            RpClump* clump = (RpClump*)modelInfo->m_pRwObject;
            if (clump)
            {
                modelInfo->AddRef();
                
                RpClump* rpClump = (RpClump*)reinterpret_cast<CClumpModelInfo*>(modelInfo)->CreateInstance();
                
                visual = new VisualObject(rpClump, nullptr, nullptr, nullptr, veh, nullptr, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                visual->renderType = CLUMP_RENDERER;
            }
        }
    }
    if (visual)
    {
        VehicleData* xdata = VehicleExtended::GetExtData(veh);
        if (xdata != nullptr) xdata->VisualObjects.push_back(visual);
    }
    return visual;
}
inline VisualObject* CreateVisualObjectOnObject(CObject* obj, int modelId, const char* fmt, float x, float y, float z, float rx, float ry, float rz)
{
    VisualObject* visual = nullptr;
    
    if (modelId == -1)
    {
        RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, fmt);
        if (stream)
        {
            if (RwStreamFindChunk(stream, 16, 0, 0))
            {
                RpClump* clump = RpClumpStreamRead(stream);
                if (clump)
                {
                    RpAtomic* atomic = GetFirstAtomic(clump);
                    if (atomic)
                    {
                        RwFrame* frame = RwFrameCreate();
                        RpAtomicSetFrame(atomic, frame);
                        RwFrameUpdateObjects(frame);
                        
                        visual = new VisualObject(clump, frame, atomic, nullptr, nullptr, obj, -1, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                        const char* filename = strrchr(fmt, '/');
                        filename = filename ? filename + 1 : fmt;
                        strcpy(visual->dff_name, filename);
                    }
                }
            }
            RwStreamClose(stream, 0);
        }
    }
    else
    {
        CBaseModelInfo* modelInfo = CModelInfo::ms_modelInfoPtrs[modelId];
        int rwModelType = modelInfo->GetRwModelType();
        if (rwModelType == 1)
        {
            RpAtomic* atomic = (RpAtomic*)modelInfo->m_pRwObject;
            if (atomic)
            {
                modelInfo->AddRef();
                
                RwFrame* rwFrame = RwFrameCreate();
                RpAtomic* rpAtomic = RpAtomicClone(atomic);
                RpAtomicSetFrame(rpAtomic, rwFrame);
                
                visual = new VisualObject(nullptr, rwFrame, rpAtomic, nullptr, nullptr, obj, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            RpClump* clump = (RpClump*)modelInfo->m_pRwObject;
            if (clump)
            {
                modelInfo->AddRef();
                
                RpClump* rpClump = (RpClump*)reinterpret_cast<CClumpModelInfo*>(modelInfo)->CreateInstance();
                
                visual = new VisualObject(rpClump, nullptr, nullptr, nullptr, nullptr, obj, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                visual->renderType = CLUMP_RENDERER;
            }
        }
    }
    if (visual)
    {
        ObjectData* xdata = ObjectExtended::GetExtData(obj);
        if (xdata != nullptr) xdata->VisualObjects.push_back(visual);
    }
    return visual;
}
inline VisualObject* CreateVisualObjectOnWorld(int modelId, const char* fmt, float x, float y, float z, float rx, float ry, float rz)
{
    VisualObject* visual = nullptr;
    
    if (modelId == -1)
    {
        RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, fmt);
        if (stream)
        {
            if (RwStreamFindChunk(stream, 16, 0, 0))
            {
                RpClump* clump = RpClumpStreamRead(stream);
                if (clump)
                {
                    RpAtomic* atomic = GetFirstAtomic(clump);
                    if (atomic)
                    {
                        RwFrame* frame = RwFrameCreate();
                        RpAtomicSetFrame(atomic, frame);
                        RwFrameUpdateObjects(frame);
                        
                        visual = new VisualObject(clump, frame, atomic, nullptr, nullptr, nullptr, -1, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                        const char* filename = strrchr(fmt, '/');
                        filename = filename ? filename + 1 : fmt;
                        strcpy(visual->dff_name, filename);
                    }
                }
            }
            RwStreamClose(stream, 0);
        }
    }
    else
    {
        CBaseModelInfo* modelInfo = CModelInfo::ms_modelInfoPtrs[modelId];
        int rwModelType = modelInfo->GetRwModelType();
        if (rwModelType == 1)
        {
            RpAtomic* atomic = (RpAtomic*)modelInfo->m_pRwObject;
            if (atomic)
            {
                modelInfo->AddRef();
                
                RwFrame* rwFrame = RwFrameCreate();
                RpAtomic* rpAtomic = RpAtomicClone(atomic);
                RpAtomicSetFrame(rpAtomic, rwFrame);
                
                visual = new VisualObject(nullptr, rwFrame, rpAtomic, nullptr, nullptr, nullptr, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            RpClump* clump = (RpClump*)modelInfo->m_pRwObject;
            if (clump)
            {
                modelInfo->AddRef();
                
                RpClump* rpClump = (RpClump*)reinterpret_cast<CClumpModelInfo*>(modelInfo)->CreateInstance();
                
                visual = new VisualObject(rpClump, nullptr, nullptr, nullptr, nullptr, nullptr, modelId, -1, x, y, z, rx, ry, rz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                visual->renderType = CLUMP_RENDERER;
            }
        }
    }
    if (visual) ObjWorld.push_back(visual);
    return visual;
}
inline void ClumpRGBA(RpClump* clump, uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha)
{
    if (!clump) return;
    RwRGBA color = { _red, _green, _blue, _alpha };
    RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data)
    {
        RpGeometry* geo = atomic->geometry;
        if (geo)
        {
            geo->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
            RpGeometryForAllMaterials(geo, [](RpMaterial *material, void *data)
            {
                RwRGBA newColor = *(RwRGBA*)data;
                material->color = newColor;
                return material;
            }, data);
        }
        return atomic;
    }, &color);
}
inline void AtomicRGBA(RpAtomic* atomic, uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha)
{
    if (!atomic) return;
    RwRGBA color = { _red, _green, _blue, _alpha };
    RpGeometry* geo = atomic->geometry;
    if (geo)
    {
        geo->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
        RpGeometryForAllMaterials(geo, [](RpMaterial *material, void *data)
        {
            RwRGBA newColor = *(RwRGBA*)data;
            material->color = newColor;
            return material;
        }, &color);
    }
}
void NoneFunctionLogic(uintptr_t) { return; }

// main code
ON_MOD_LOAD()
{
    sautils = (ISAUtils*)GetInterface("SAUtils");
    if (sautils)
    {
        char bufVer[128] { 0 };
        snprintf(bufVer, sizeof(bufVer), "Visual Object v%s", modinfo->VersionString());
        sautils->AddButton(SetType_Mods, bufVer, NoneFunctionLogic);
    }
    
    // Game event
    Events::restartGameEvent += []()
    {
        // Ped
        for (PedData* xdata : PedExtData)
        {
            if (xdata != nullptr && xdata->VisualObjects.size() > 0)
            {
                for (VisualObject* visual : xdata->VisualObjects) delete visual;
                xdata->VisualObjects.clear();
            }
            xdata->Reset();
        }
        
        // Vehicle
        for (VehicleData* xdata : VehExtData)
        {
            if (xdata != nullptr && xdata->VisualObjects.size() > 0)
            {
                for (VisualObject* visual : xdata->VisualObjects) delete visual;
                xdata->VisualObjects.clear();
            }
            xdata->Reset();
        }
        
        // Object
        for (ObjectData* xdata : ObjExtData)
        {
            if (xdata != nullptr && xdata->VisualObjects.size() > 0)
            {
                for (VisualObject* visual : xdata->VisualObjects) delete visual;
                xdata->VisualObjects.clear();
            }
            xdata->Reset();
        }
        ObjExtData.clear();
        for (VisualObject* visual : ObjWorld) delete visual;
        ObjWorld.clear();
    };
    
    // Ped event
    Events::pedRenderEvent.after += [](CPed* self)
    {
        PedData* xdata = PedExtended::GetExtData(self);
        if (xdata != nullptr && xdata->VisualObjects.size() > 0)
        {
            if (self->m_pRwObject)
            {
                for (VisualObject* visual : xdata->VisualObjects)
                {
                    if (visual->isVisible)
                    {
                        if (visual->hideIfCar && visual->ped->bInVehicle) continue;
                        if (visual->hideIfDead && visual->ped->m_fHealth <= 0.0f) continue;
                        if (visual->hideIfWeapon && visual->ped->m_nSelectedWepSlot > 0) continue;
                        
                        RpClump* clump = visual->clump;
                        RpAtomic* atomic = visual->atomic;
                        RwFrame* frame = visual->frame;
                        if (clump && visual->renderType == CLUMP_RENDERER) frame = (RwFrame*)clump->object.parent;
                        
                        RpHAnimHierarchy* hierarchy = GetAnimHierarchyFromSkinClump(visual->ped->m_pRwClump);
                        RwMatrix* boneMatrix = &hierarchy->pMatrixArray[RpHAnimIDGetIndex(hierarchy, visual->bone_id)];
                        memcpy(&frame->modelling, boneMatrix, sizeof(frame->modelling));
                        RwV3d pointsIn = { visual->offset.x, visual->offset.y, visual->offset.z };
                        RwV3dTransformPoints(&pointsIn, &pointsIn, 1, boneMatrix);
                        frame->modelling.pos = pointsIn;
                        if (visual->rot.x != 0.0f || visual->rot.y != 0.0f || visual->rot.z != 0.0f)
                        {
                            CVector rotX(1.0f, 0.0f, 0.0f);
                            RwFrameRotate(frame, &rotX, visual->rot.x, rwCOMBINEPRECONCAT);
                            CVector rotY(0.0f, 1.0f, 0.0f);
                            RwFrameRotate(frame, &rotY, visual->rot.y, rwCOMBINEPRECONCAT);
                            CVector rotZ(0.0f, 0.0f, 1.0f);
                            RwFrameRotate(frame, &rotZ, visual->rot.z, rwCOMBINEPRECONCAT);
                        }
                        if (visual->scale.x != 1.0f || visual->scale.y != 1.0f || visual->scale.z != 1.0f)
                        {
                            CVector scale(visual->scale.x, visual->scale.y, visual->scale.z);
                            RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
                        }
                        if (visual->dist.x != 0.0f) frame->modelling.at.x = visual->dist.x;
                        if (visual->dist.y != 0.0f) frame->modelling.at.z = visual->dist.y;
                        if (visual->dist.z != 0.0f) frame->modelling.pos.x = visual->dist.z; // bug?
                        if (visual->dist.w != 0.0f) frame->modelling.pos.y = visual->dist.w; // bug?
                        RwFrameUpdateObjects(frame);
                        
                        if (clump && visual->renderType == CLUMP_RENDERER)
                        {
                            RpClumpRender(clump);
                        }
                        else
                        {
                            if (atomic && visual->renderType == ATOMIC_RENDERER) atomic->renderCallBack(atomic);
                        }
                    }
                }
            }
        }
    };
    Events::pedDtorEvent.before += [](CPed* self)
    {
        PedExtended::GetExtData(self)->Reset();
    };
    
    // Vehicle event
    Events::vehicleRenderEvent.after += [](CVehicle* self)
    {
        VehicleData* xdata = VehicleExtended::GetExtData(self);
        if (xdata != nullptr && xdata->VisualObjects.size() > 0)
        {
            if (self->m_pRwObject)
            {
                for (VisualObject* visual : xdata->VisualObjects)
                {
                    if (visual->isVisible)
                    {
                        RpClump* clump = visual->clump;
                        RpAtomic* atomic = visual->atomic;
                        RwFrame* frame = visual->frame;
                        if (clump && visual->renderType == CLUMP_RENDERER) frame = (RwFrame*)clump->object.parent;
                        
                        CMatrix* matrix = self->m_matrix;
                        memcpy(&frame->modelling, matrix, sizeof(frame->modelling));
                        RwV3d pointsIn = { visual->offset.x, visual->offset.y, visual->offset.z };
                        RwV3dTransformPoints(&pointsIn, &pointsIn, 1, (RwMatrix*)matrix);
                        frame->modelling.pos = pointsIn;
                        if (visual->rot.x != 0.0f || visual->rot.y != 0.0f || visual->rot.z != 0.0f)
                        {
                            CVector rotX(1.0f, 0.0f, 0.0f);
                            RwFrameRotate(frame, &rotX, visual->rot.x, rwCOMBINEPRECONCAT);
                            CVector rotY(0.0f, 1.0f, 0.0f);
                            RwFrameRotate(frame, &rotY, visual->rot.y, rwCOMBINEPRECONCAT);
                            CVector rotZ(0.0f, 0.0f, 1.0f);
                            RwFrameRotate(frame, &rotZ, visual->rot.z, rwCOMBINEPRECONCAT);
                        }
                        if (visual->scale.x != 1.0f || visual->scale.y != 1.0f || visual->scale.z != 1.0f)
                        {
                            CVector scale(visual->scale.x, visual->scale.y, visual->scale.z);
                            RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
                        }
                        if (visual->dist.x != 0.0f) frame->modelling.at.x = visual->dist.x;
                        if (visual->dist.y != 0.0f) frame->modelling.at.z = visual->dist.y;
                        if (visual->dist.z != 0.0f) frame->modelling.pos.x = visual->dist.z; // bug?
                        if (visual->dist.w != 0.0f) frame->modelling.pos.y = visual->dist.w; // bug?
                        RwFrameUpdateObjects(frame);
                        
                        if (clump && visual->renderType == CLUMP_RENDERER)
                        {
                            RpClumpRender(clump);
                        }
                        else
                        {
                            if (atomic && visual->renderType == ATOMIC_RENDERER) atomic->renderCallBack(atomic);
                        }
                    }
                }
            }
        }
    };
    Events::vehicleDtorEvent.before += [](CVehicle* self)
    {
        VehicleExtended::GetExtData(self)->Reset();
    };
    
    // Object event
    Events::objectRenderEvent.after += [](CObject* self)
    {
        ObjectData* xdata = ObjectExtended::GetExtData(self);
        if (xdata != nullptr && xdata->VisualObjects.size() > 0)
        {
            if (self->m_pRwObject)
            {
                for (VisualObject* visual : xdata->VisualObjects)
                {
                    if (visual->isVisible)
                    {
                        RpClump* clump = visual->clump;
                        RpAtomic* atomic = visual->atomic;
                        RwFrame* frame = visual->frame;
                        if (clump && visual->renderType == CLUMP_RENDERER) frame = (RwFrame*)clump->object.parent;
                        
                        CMatrix* matrix = self->m_matrix;
                        memcpy(&frame->modelling, matrix, sizeof(frame->modelling));
                        RwV3d pointsIn = { visual->offset.x, visual->offset.y, visual->offset.z };
                        RwV3dTransformPoints(&pointsIn, &pointsIn, 1, (RwMatrix*)matrix);
                        frame->modelling.pos = pointsIn;
                        if (visual->rot.x != 0.0f || visual->rot.y != 0.0f || visual->rot.z != 0.0f)
                        {
                            CVector rotX(1.0f, 0.0f, 0.0f);
                            RwFrameRotate(frame, &rotX, visual->rot.x, rwCOMBINEPRECONCAT);
                            CVector rotY(0.0f, 1.0f, 0.0f);
                            RwFrameRotate(frame, &rotY, visual->rot.y, rwCOMBINEPRECONCAT);
                            CVector rotZ(0.0f, 0.0f, 1.0f);
                            RwFrameRotate(frame, &rotZ, visual->rot.z, rwCOMBINEPRECONCAT);
                        }
                        if (visual->scale.x != 1.0f || visual->scale.y != 1.0f || visual->scale.z != 1.0f)
                        {
                            CVector scale(visual->scale.x, visual->scale.y, visual->scale.z);
                            RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
                        }
                        if (visual->dist.x != 0.0f) frame->modelling.at.x = visual->dist.x;
                        if (visual->dist.y != 0.0f) frame->modelling.at.z = visual->dist.y;
                        if (visual->dist.z != 0.0f) frame->modelling.pos.x = visual->dist.z; // bug?
                        if (visual->dist.w != 0.0f) frame->modelling.pos.y = visual->dist.w; // bug?
                        RwFrameUpdateObjects(frame);
                        
                        if (clump && visual->renderType == CLUMP_RENDERER)
                        {
                            RpClumpRender(clump);
                        }
                        else
                        {
                            if (atomic && visual->renderType == ATOMIC_RENDERER) atomic->renderCallBack(atomic);
                        }
                    }
                }
            }
        }
        
        // Visual Object on World coordinate
        if (ObjWorld.size() > 0)
        {
            for (VisualObject* visual : ObjWorld)
            {
                if (visual->isVisible)
                {
                    RpClump* clump = visual->clump;
                    RpAtomic* atomic = visual->atomic;
                    RwFrame* frame = visual->frame;
                    if (clump && visual->renderType == CLUMP_RENDERER) frame = (RwFrame*)clump->object.parent;
                    
                    CVector posn(visual->offset.x, visual->offset.y, visual->offset.z);
                    RwFrameTranslate(frame, &posn, rwCOMBINEREPLACE);
                    if (visual->rot.x != 0.0f || visual->rot.y != 0.0f || visual->rot.z != 0.0f)
                    {
                        CVector rotX(1.0f, 0.0f, 0.0f);
                        RwFrameRotate(frame, &rotX, visual->rot.x, rwCOMBINEPRECONCAT);
                        CVector rotY(0.0f, 1.0f, 0.0f);
                        RwFrameRotate(frame, &rotY, visual->rot.y, rwCOMBINEPRECONCAT);
                        CVector rotZ(0.0f, 0.0f, 1.0f);
                        RwFrameRotate(frame, &rotZ, visual->rot.z, rwCOMBINEPRECONCAT);
                    }
                    if (visual->scale.x != 1.0f || visual->scale.y != 1.0f || visual->scale.z != 1.0f)
                    {
                        CVector scale(visual->scale.x, visual->scale.y, visual->scale.z);
                        RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
                    }
                    if (visual->dist.x != 0.0f) frame->modelling.at.x = visual->dist.x;
                    if (visual->dist.y != 0.0f) frame->modelling.at.z = visual->dist.y;
                    if (visual->dist.z != 0.0f) frame->modelling.pos.x = visual->dist.z; // bug?
                    if (visual->dist.w != 0.0f) frame->modelling.pos.y = visual->dist.w; // bug?
                    RwFrameUpdateObjects(frame);
                    
                    if (clump && visual->renderType == CLUMP_RENDERER)
                    {
                        RpClumpRender(clump);
                    }
                    else
                    {
                        if (atomic && visual->renderType == ATOMIC_RENDERER) atomic->renderCallBack(atomic);
                    }
                }
            }
        }
    };
    Events::objectDtorEvent.before += [](CObject* self)
    {
        ObjectExtended::GetExtData(self)->Reset();
    };
}

//--------------------[ START OPCODES ]--------------------
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_CHAR_BONE)
{
    CPed* ped = CPools::GetPed(cleo->ReadParam(handle)->i);
    char buf[MAX_STR_LEN];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    for (int i = 0; buf[i] != 0; ++i) if (buf[i] == '\\') buf[i] = '/';
    int boneId = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnChar(ped, -1, buf, boneId, x, y, z, rx, ry, rz);
    PedData* xdata = PedExtended::GetExtData(ped);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(SET_VISUAL_OBJECT_SCALE)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        visual->scale.x = cleo->ReadParam(handle)->f;
        visual->scale.y = cleo->ReadParam(handle)->f;
        visual->scale.z = cleo->ReadParam(handle)->f;
    }
}
CLEO_Fn(DELETE_VISUAL_OBJECT)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        bool isDeleted = false;
        if (visual->ped && isDeleted != true)
        {
            PedData* xdata = PedExtended::GetExtData(visual->ped);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    xdata->VisualObjects.erase(iter);
                    isDeleted = true;
                    break;
                }
            }
        }
        if (visual->veh && isDeleted != true)
        {
            VehicleData* xdata = VehicleExtended::GetExtData(visual->veh);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    xdata->VisualObjects.erase(iter);
                    isDeleted = true;
                    break;
                }
            }
        }
        if (visual->obj && isDeleted != true)
        {
            ObjectData* xdata = ObjectExtended::GetExtData(visual->obj);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    xdata->VisualObjects.erase(iter);
                    isDeleted = true;
                    break;
                }
            }
        }
        if (isDeleted != true)
        {
            for (std::vector<VisualObject*>::iterator iter = ObjWorld.begin(); iter != ObjWorld.end(); ++iter)
            {
                if (*iter == visual)
                {
                    ObjWorld.erase(iter);
                    isDeleted = true;
                    break;
                }
            }
        }
    }
}
CLEO_Fn(SET_VISUAL_OBJECT_AUTO_HIDE)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        visual->hideIfDead = cleo->ReadParam(handle)->i;
        visual->hideIfWeapon = cleo->ReadParam(handle)->i;
        visual->hideIfCar = cleo->ReadParam(handle)->i;
    }
}
CLEO_Fn(SET_VISUAL_OBJECT_VISIBLE)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    int visibility = cleo->ReadParam(handle)->i;
    if (visual) visual->isVisible = visibility;
}
CLEO_Fn(SET_VISUAL_OBJECT_OFFSET)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        visual->offset.x = cleo->ReadParam(handle)->f;
        visual->offset.y = cleo->ReadParam(handle)->f;
        visual->offset.z = cleo->ReadParam(handle)->f;
    }
}
CLEO_Fn(SET_VISUAL_OBJECT_ROTATION)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        visual->rot.x = cleo->ReadParam(handle)->f;
        visual->rot.y = cleo->ReadParam(handle)->f;
        visual->rot.z = cleo->ReadParam(handle)->f;
    }
}
CLEO_Fn(SET_VISUAL_OBJECT_DISTORTION)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        visual->dist.x = cleo->ReadParam(handle)->f;
        visual->dist.y = cleo->ReadParam(handle)->f;
        visual->dist.z = cleo->ReadParam(handle)->f;
        visual->dist.w = cleo->ReadParam(handle)->f;
    }
}
CLEO_Fn(SWITCH_VISUAL_OBJECT_RENDERER)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual) visual->renderType = (eRenderType)cleo->ReadParam(handle)->i;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_CHAR_BONE_FROM_ID)
{
    CPed* ped = CPools::GetPed(cleo->ReadParam(handle)->i);
    int modelId = cleo->ReadParam(handle)->i;
    int boneId = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnChar(ped, modelId, nullptr, boneId, x, y, z, rx, ry, rz);
    PedData* xdata = PedExtended::GetExtData(ped);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_OBJECT_FROM_ID)
{
    CObject* obj = CPools::GetObject(cleo->ReadParam(handle)->i);
    int modelId = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnObject(obj, modelId, "", x, y, z, rx, ry, rz);
    ObjectData* xdata = ObjectExtended::GetExtData(obj);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_VEHICLE_FROM_ID)
{
    CVehicle* veh = CPools::GetVehicle(cleo->ReadParam(handle)->i);
    int modelId = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnVehicle(veh, modelId, "", x, y, z, rx, ry, rz);
    VehicleData* xdata = VehicleExtended::GetExtData(veh);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_FROM_ID)
{
    int modelId = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnWorld(modelId, "", x, y, z, rx, ry, rz);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_OBJECT)
{
    CObject* obj = CPools::GetObject(cleo->ReadParam(handle)->i);
    char buf[MAX_STR_LEN];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    for (int i = 0; buf[i] != 0; ++i) if (buf[i] == '\\') buf[i] = '/';
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnObject(obj, -1, buf, x, y, z, rx, ry, rz);
    ObjectData* xdata = ObjectExtended::GetExtData(obj);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_TO_VEHICLE)
{
    CVehicle* veh = CPools::GetVehicle(cleo->ReadParam(handle)->i);
    char buf[MAX_STR_LEN];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    for (int i = 0; buf[i] != 0; ++i) if (buf[i] == '\\') buf[i] = '/';
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnVehicle(veh, -1, buf, x, y, z, rx, ry, rz);
    VehicleData* xdata = VehicleExtended::GetExtData(veh);
    xdata->ScriptID = cleoaddon->GetScriptID(handle);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(CREATE_VISUAL_OBJECT_FROM_DFF)
{
    char buf[MAX_STR_LEN];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    for (int i = 0; buf[i] != 0; ++i) if (buf[i] == '\\') buf[i] = '/';
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float rx = cleo->ReadParam(handle)->f;
    float ry = cleo->ReadParam(handle)->f;
    float rz = cleo->ReadParam(handle)->f;
    
    VisualObject* visual = CreateVisualObjectOnWorld(-1, buf, x, y, z, rx, ry, rz);
    cleo->GetPointerToScriptVar(handle)->i = (int)visual;
}
CLEO_Fn(SET_VISUAL_OBJECT_RGBA)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        uint8_t red = (uint8_t)cleo->ReadParam(handle)->i;
        uint8_t green = (uint8_t)cleo->ReadParam(handle)->i;
        uint8_t blue = (uint8_t)cleo->ReadParam(handle)->i;
        uint8_t alpha = (uint8_t)cleo->ReadParam(handle)->i;
        
        if (visual->clump)
        {
            ClumpRGBA(visual->clump, red, green, blue, alpha);
        }
        else
        {
            AtomicRGBA(visual->atomic, red, green, blue, alpha);
        }
    }
}
CLEO_Fn(IS_VISUAL_OBJECT_VISIBLE)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        bool isActuallyVisible = false;
        RwRGBA colorData;
        if (visual->clump)
        {
            RpClumpForAllAtomics(visual->clump, [](RpAtomic* atomic, void* data)
            {
                RpGeometry* geo = atomic->geometry;
                if (geo)
                {
                    RpGeometryForAllMaterials(geo, [](RpMaterial *material, void *data)
                    {
                        RwRGBA* trans = (RwRGBA*)data;
                        trans->alpha = material->color.alpha;
                        return material;
                    }, data);
                }
                return atomic;
            }, &colorData);
            if (visual->isVisible && colorData.alpha > 0) isActuallyVisible = true;
        }
        else
        {
            RpGeometry* geo = visual->atomic->geometry;
            if (geo)
            {
                RpGeometryForAllMaterials(geo, [](RpMaterial *material, void *data)
                {
                    RwRGBA* trans = (RwRGBA*)data;
                    trans->alpha = material->color.alpha;
                    return material;
                }, &colorData);
                if (visual->isVisible && colorData.alpha > 0) isActuallyVisible = true;
            }
        }
        cleoaddon->UpdateCompareFlag(handle, isActuallyVisible);
    }
}
CLEO_Fn(IS_VISUAL_OBJECT_CREATED)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        bool isCreated = false;
        if (visual->ped && isCreated != true)
        {
            PedData* xdata =PedExtended::GetExtData(visual->ped);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    isCreated = true;
                    break;
                }
            }
        }
        if (visual->veh && isCreated != true)
        {
            VehicleData* xdata = VehicleExtended::GetExtData(visual->veh);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    isCreated = true;
                    break;
                }
            }
        }
        if (visual->obj && isCreated != true)
        {
            ObjectData* xdata = ObjectExtended::GetExtData(visual->obj);
            if (xdata == nullptr) return;
            for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
            {
                if (*iter == visual)
                {
                    isCreated = true;
                    break;
                }
            }
        }
        if (isCreated != true)
        {
            for (std::vector<VisualObject*>::iterator iter = ObjWorld.begin(); iter != ObjWorld.end(); ++iter)
            {
                if (*iter == visual)
                {
                    isCreated = true;
                    break;
                }
            }
        }
        cleoaddon->UpdateCompareFlag(handle, isCreated);
    }
}
CLEO_Fn(DELETE_ALL_VISUAL_OBJECT_FROM_CHAR)
{
    CPed* ped = CPools::GetPed(cleo->ReadParam(handle)->i);
    if (ped)
    {
        PedData* xdata = PedExtended::GetExtData(ped);
        if (xdata == nullptr) return;
        uint16_t ThisScriptID = cleoaddon->GetScriptID(handle);
        if (xdata->ScriptID == ThisScriptID) xdata->VisualObjects.clear();
    }
}
CLEO_Fn(DELETE_ALL_VISUAL_OBJECT_FROM_VEHICLE)
{
    CVehicle* veh = CPools::GetVehicle(cleo->ReadParam(handle)->i);
    if (veh)
    {
        VehicleData* xdata = VehicleExtended::GetExtData(veh);
        if (xdata == nullptr) return;
        uint16_t ThisScriptID = cleoaddon->GetScriptID(handle);
        if (xdata->ScriptID == ThisScriptID) xdata->VisualObjects.clear();
    }
}
CLEO_Fn(DELETE_ALL_VISUAL_OBJECT_FROM_OBJECT)
{
    CObject* obj = CPools::GetObject(cleo->ReadParam(handle)->i);
    if (obj)
    {
        ObjectData* xdata = ObjectExtended::GetExtData(obj);
        if (xdata == nullptr) return;
        uint16_t ThisScriptID = cleoaddon->GetScriptID(handle);
        if (xdata->ScriptID == ThisScriptID) xdata->VisualObjects.clear();
    }
}
CLEO_Fn(GET_VISUAL_OBJECT_MODEL_ID)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = visual != nullptr ? visual->model_id : 0;
}
CLEO_Fn(GET_VISUAL_OBJECT_DFF_NAME)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    const char* dffName = visual != nullptr ? visual->dff_name : "NULL";
    cleoaddon->WriteString(handle, dffName);
}
CLEO_Fn(IS_VISUAL_OBJECT_MODEL_ID)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    int VisualID = cleo->ReadParam(handle)->i;
    if (!visual) return;
    cleoaddon->UpdateCompareFlag(handle, VisualID == visual->model_id);
}
CLEO_Fn(IS_VISUAL_OBJECT_DFF_NAME)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        char buf[MAX_STR_LEN];
        cleoaddon->ReadString(handle, buf, sizeof(buf));
        for (int i = 0; buf[i] != 0; ++i) if (buf[i] == '\\') buf[i] = '/';
        cleoaddon->UpdateCompareFlag(handle, strcmp(visual->dff_name, buf) == 0);
    }
}
CLEO_Fn(IS_VISUAL_OBJECT_PED_ATTACHED)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        // There's no detachment, so check if it's available instead.
        CPed* ped = CPools::GetPed(cleo->ReadParam(handle)->i);
        bool isAttached = false;
        PedData* xdata = PedExtended::GetExtData(ped);
        if (xdata == nullptr) return;
        for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
        {
            if (*iter == visual)
            {
                isAttached = true;
                break;
            }
        }
        cleoaddon->UpdateCompareFlag(handle, isAttached);
    }
}
CLEO_Fn(IS_VISUAL_OBJECT_VEHICLE_ATTACHED)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        // There's no detachment, so check if it's available instead.
        CVehicle* veh = CPools::GetVehicle(cleo->ReadParam(handle)->i);
        bool isAttached = false;
        VehicleData* xdata = VehicleExtended::GetExtData(veh);
        if (xdata == nullptr) return;
        for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
        {
            if (*iter == visual)
            {
                isAttached = true;
                break;
            }
        }
        cleoaddon->UpdateCompareFlag(handle, isAttached);
    }
}
CLEO_Fn(IS_VISUAL_OBJECT_OBJECT_ATTACHED)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        // There's no detachment, so check if it's available instead.
        CObject* obj = CPools::GetObject(cleo->ReadParam(handle)->i);
        bool isAttached = false;
        ObjectData* xdata = ObjectExtended::GetExtData(obj);
        if (xdata == nullptr) return;
        for (std::vector<VisualObject*>::iterator iter = xdata->VisualObjects.begin(); iter != xdata->VisualObjects.end(); ++iter)
        {
            if (*iter == visual)
            {
                isAttached = true;
                break;
            }
        }
        cleoaddon->UpdateCompareFlag(handle, isAttached);
    }
}
CLEO_Fn(GET_OFFSET_FROM_VISUAL_OBJECT_IN_WORLD_COORDS)
{
    VisualObject* visual = (VisualObject*)cleo->ReadParam(handle)->i;
    if (visual)
    {
        RpClump* clump = visual->clump;
        RwFrame* frame = visual->frame;
        if (clump) frame = (RwFrame*)clump->object.parent;
        
        RwV3d offset, posReturn;
        offset.x = cleo->ReadParam(handle)->f;
        offset.y = cleo->ReadParam(handle)->f;
        offset.z = cleo->ReadParam(handle)->f;
        RwV3dTransformPoints(&posReturn, &offset, 1, &frame->ltm);
        
        cleo->GetPointerToScriptVar(handle)->f = posReturn.x;
        cleo->GetPointerToScriptVar(handle)->f = posReturn.y;
        cleo->GetPointerToScriptVar(handle)->f = posReturn.z;
    }
}
//--------------------[ END OPCODES ]----------------------

ON_MOD_PRELOAD()
{
    logger->SetTag("[CLEO] Visual Object");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");
    
    // RwFrame
    SET_TO(RwFrameCreate, aml->GetSym(hGTASA, "_Z13RwFrameCreatev"));
    SET_TO(RwFrameDestroy, aml->GetSym(hGTASA, "_Z14RwFrameDestroyP7RwFrame"));
    SET_TO(RwFrameTransform, aml->GetSym(hGTASA, "_Z16RwFrameTransformP7RwFramePK11RwMatrixTag15RwOpCombineType"));
    SET_TO(RwFrameTranslate, aml->GetSym(hGTASA, "_Z16RwFrameTranslateP7RwFramePK5RwV3d15RwOpCombineType"));
    SET_TO(RwFrameScale, aml->GetSym(hGTASA, "_Z12RwFrameScaleP7RwFramePK5RwV3d15RwOpCombineType"));
    SET_TO(RwFrameRotate, aml->GetSym(hGTASA, "_Z13RwFrameRotateP7RwFramePK5RwV3df15RwOpCombineType"));
    SET_TO(RwFrameUpdateObjects, aml->GetSym(hGTASA, "_Z20RwFrameUpdateObjectsP7RwFrame"));
    
    // RpAtomic
    SET_TO(GetFirstAtomic, aml->GetSym(hGTASA, "_Z14GetFirstAtomicP7RpClump"));
    SET_TO(RpAtomicSetFrame, aml->GetSym(hGTASA, "_Z16RpAtomicSetFrameP8RpAtomicP7RwFrame"));
    SET_TO(RpAtomicDestroy, aml->GetSym(hGTASA, "_Z15RpAtomicDestroyP8RpAtomic"));
    SET_TO(RpAtomicClone, aml->GetSym(hGTASA, "_Z13RpAtomicCloneP8RpAtomic"));
    
    // RpClump
    SET_TO(RpClumpDestroy, aml->GetSym(hGTASA, "_Z14RpClumpDestroyP7RpClump"));
    SET_TO(RpClumpRender, aml->GetSym(hGTASA, "_Z13RpClumpRenderP7RpClump"));
    SET_TO(RpClumpStreamRead, aml->GetSym(hGTASA, "_Z17RpClumpStreamReadP8RwStream"));
    SET_TO(RpClumpForAllAtomics, aml->GetSym(hGTASA, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    
    // Geometry
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    
    // RpHAnimHierarchy
    SET_TO(GetAnimHierarchyFromSkinClump, aml->GetSym(hGTASA, "_Z29GetAnimHierarchyFromSkinClumpP7RpClump"));
    SET_TO(RpHAnimIDGetIndex, aml->GetSym(hGTASA, "_Z17RpHAnimIDGetIndexP16RpHAnimHierarchyi"));
    SET_TO(RpHAnimHierarchyGetMatrixArray, aml->GetSym(hGTASA, "_Z30RpHAnimHierarchyGetMatrixArrayP16RpHAnimHierarchy"));
    SET_TO(RwV3dTransformPoints, aml->GetSym(hGTASA, "_Z20RwV3dTransformPointsP5RwV3dPKS_iPK11RwMatrixTag"));
}

ON_ALL_MODS_LOAD()
{
    cleo = (cleo_ifs_t*)GetInterface("CLEO");
    cleoaddon = (cleo_addon_ifs_t*)GetInterface("CLEOAddon");
    if (!cleo || !cleoaddon)
    {
        logger->Error("CLEO not found");
        return;
    }
    
    /* Opcode range [6000 - 601D] */
    CLEO_RegisterOpcode(0x6000, CREATE_VISUAL_OBJECT_TO_CHAR_BONE);             // 6000=10,%10d% = create_visual_object_to_char_bone %1d% from_dff %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d%
    CLEO_RegisterOpcode(0x6001, SET_VISUAL_OBJECT_SCALE);                       // 6001=4,set_visual_object_scale %1d% scale %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x6002, DELETE_VISUAL_OBJECT);                          // 6002=1,delete_visual_object %1d%
    CLEO_RegisterOpcode(0x6003, SET_VISUAL_OBJECT_AUTO_HIDE);                   // 6003=4,set_visual_object_auto_hide %1d% dead_state %2d% weapon_state %3d% car_state %4d%
    CLEO_RegisterOpcode(0x6004, SET_VISUAL_OBJECT_VISIBLE);                     // 6004=2,set_visual_object_visible %1d% visible %2d%
    CLEO_RegisterOpcode(0x6005, SET_VISUAL_OBJECT_OFFSET);                      // 6005=4,set_visual_object_offset %1d% offset %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x6006, SET_VISUAL_OBJECT_ROTATION);                    // 6006=4,set_visual_object_rotation %1d% rotation %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x6007, SET_VISUAL_OBJECT_DISTORTION);                  // 6007=5,set_visual_object_distortion %1d% distortion %2d% %3d% %4d% %5d%
    CLEO_RegisterOpcode(0x6008, SWITCH_VISUAL_OBJECT_RENDERER);                 // 6008=2,switch_visual_object_renderer %1d% render_mode %2d%
    CLEO_RegisterOpcode(0x6009, CREATE_VISUAL_OBJECT_TO_CHAR_BONE_FROM_ID);     // 6009=10,%10d% = create_visual_object_to_char_bone %1d% model_id %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d%
    CLEO_RegisterOpcode(0x600A, CREATE_VISUAL_OBJECT_TO_OBJECT_FROM_ID);        // 600A=9,%9d% = create_visual_object_to_object %1d% model_id %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
    CLEO_RegisterOpcode(0x600B, CREATE_VISUAL_OBJECT_TO_VEHICLE_FROM_ID);       // 600B=9,%9d% = create_visual_object_to_vehicle %1d% model_id %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
    CLEO_RegisterOpcode(0x600C, CREATE_VISUAL_OBJECT_FROM_ID);                  // 600C=8,%8d% = create_visual_object_model_id %1d% offset %2d% %3d% %4d% rotation %5d% %6d% %7d%
    CLEO_RegisterOpcode(0x600D, CREATE_VISUAL_OBJECT_TO_OBJECT);                // 600D=9,%9d% = create_visual_object_to_object %1d% from_dff %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
    CLEO_RegisterOpcode(0x600E, CREATE_VISUAL_OBJECT_TO_VEHICLE);               // 600E=9,%9d% = create_visual_object_to_vehicle %1d% from_dff %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d%
    CLEO_RegisterOpcode(0x600F, CREATE_VISUAL_OBJECT_FROM_DFF);                 // 600F=8,%8d% = create_visual_object_from_dff %1d% offset %2d% %3d% %4d% rotation %5d% %6d% %7d%
    CLEO_RegisterOpcode(0x6010, SET_VISUAL_OBJECT_RGBA);                        // 6010=5,set_visual_object %1d% RGBA %2d% %3d% %4d% %5d%
    CLEO_RegisterOpcode(0x6011, IS_VISUAL_OBJECT_VISIBLE);                      // 6011=1,is_visual_object_visible %1d%
    CLEO_RegisterOpcode(0x6012, IS_VISUAL_OBJECT_CREATED);                      // 6012=1,is_visual_object_created %1d%
    CLEO_RegisterOpcode(0x6013, DELETE_ALL_VISUAL_OBJECT_FROM_CHAR);            // 6013=1,delete_all_visual_objects_from_char %1d% //NOT GLOBAL
    CLEO_RegisterOpcode(0x6014, DELETE_ALL_VISUAL_OBJECT_FROM_VEHICLE);         // 6014=1,delete_all_visual_objects_from_vehicle %1d% //NOT GLOBAL
    CLEO_RegisterOpcode(0x6015, DELETE_ALL_VISUAL_OBJECT_FROM_OBJECT);          // 6015=1,delete_all_visual_objects_from_object %1d% //NOT GLOBAL
    CLEO_RegisterOpcode(0x6016, GET_VISUAL_OBJECT_MODEL_ID);                    // 6016=2,%2d% = get_visual_object_model_id %1d%
    CLEO_RegisterOpcode(0x6017, GET_VISUAL_OBJECT_DFF_NAME);                    // 6017=2,%2d% = get_visual_object_dff_name %1d%
    CLEO_RegisterOpcode(0x6018, IS_VISUAL_OBJECT_MODEL_ID);                     // 6018=2,is_visual_object %1d% model_id %2d%
    CLEO_RegisterOpcode(0x6019, IS_VISUAL_OBJECT_DFF_NAME);                     // 6019=2,is_visual_object %1d% dff_name %2d%
    CLEO_RegisterOpcode(0x601A, IS_VISUAL_OBJECT_PED_ATTACHED);                 // 601A=2,is_visual_object %1d% attached_to_char %2d%
    CLEO_RegisterOpcode(0x601B, IS_VISUAL_OBJECT_VEHICLE_ATTACHED);             // 601B=2,is_visual_object %1d% attached_to_vehicle %2d%
    CLEO_RegisterOpcode(0x601C, IS_VISUAL_OBJECT_OBJECT_ATTACHED);              // 601C=2,is_visual_object %1d% attached_to_object %2d%
    CLEO_RegisterOpcode(0x601D, GET_OFFSET_FROM_VISUAL_OBJECT_IN_WORLD_COORDS); // 601D=7,store_coords_to %1d% %2d% %3d% from_visual_object %4d% with_offset %5d% %6d% %7d%
}