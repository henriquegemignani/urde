#ifndef __URDE_CENTITY_HPP__
#define __URDE_CENTITY_HPP__

#include "RetroTypes.hpp"
#include "ScriptObjectSupport.hpp"

namespace urde
{
class CStateManager;
class IVisitor;

struct SConnection
{
    EScriptObjectState state;
    EScriptObjectMessage msg;
    TEditorId objId;
};

class CEntityInfo
{
    friend class CEntity;
    TAreaId m_aid;
    std::vector<SConnection> m_conns;
public:
    CEntityInfo(TAreaId aid, const std::vector<SConnection>& conns)
    : m_aid(aid), m_conns(conns) {}
    TAreaId GetAreaId() const {return m_aid;}
};

class CEntity
{
protected:
    TUniqueId m_uid;
    CEntityInfo m_info;
    bool m_active = false;
public:
    virtual ~CEntity() {}
    CEntity(TUniqueId uid, const CEntityInfo& info, bool active);
    virtual void Accept(IVisitor&)=0;
    virtual void PreThink(float, CStateManager&) {}
    virtual void Think(float, CStateManager&) {}
    virtual void AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId objId, CStateManager& stateMgr);
    virtual bool GetActive() const {return m_active;}
    virtual void SetActive(bool active) {m_active = active;}

    TUniqueId GetUniqueId() const {return m_uid;}
    void SendScriptMsgs(EScriptObjectState state, CStateManager& stateMgr, EScriptObjectMessage msg);
};

}

#endif // __URDE_CENTITY_HPP__
