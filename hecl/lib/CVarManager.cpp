#include "hecl/CVarManager.hpp"
#include "hecl/Console.hpp"
#include <athena/FileWriter.hpp>
#include <athena/Utility.hpp>
#include <hecl/Runtime.hpp>
#include <memory>

namespace hecl
{

CVar* com_developer = nullptr;
CVar* com_configfile = nullptr;
CVar* com_enableCheats = nullptr;

CVarManager* CVarManager::m_instance = nullptr;

static logvisor::Module CVarLog("CVarManager");
CVarManager::CVarManager(hecl::Runtime::FileStoreManager& store, bool useBinary)
    : m_store(store),
      m_useBinary(useBinary)
{
    m_instance = this;
    com_configfile = newCVar("config", "File to store configuration", std::string("config"), CVar::EFlags::System);
    com_developer = newCVar("developer", "Enables developer mode", false, (CVar::EFlags::System | CVar::EFlags::ReadOnly | CVar::EFlags::InternalArchivable));
    com_enableCheats = newCVar("iamaweiner", "Enable cheats", false, (CVar::EFlags::System | CVar::EFlags::ReadOnly | CVar::EFlags::Hidden));
}

CVarManager::~CVarManager()
{
}

bool CVarManager::registerCVar(CVar* cvar)
{
    std::string tmp(cvar->name());
    athena::utility::tolower(tmp);
    if (m_cvars.find(tmp) != m_cvars.end())
        return false;

    m_cvars[tmp] = cvar;
    return true;
}

CVar* CVarManager::findCVar(std::string_view name)
{
    std::string lower(name);
    athena::utility::tolower(lower);
    auto search = m_cvars.find(lower);
    if (search == m_cvars.end())
        return nullptr;

    return search->second;
}

std::vector<CVar*> CVarManager::archivedCVars() const
{
    std::vector<CVar*> ret;
    for (const std::pair<std::string, CVar*>& pair : m_cvars)
        if (pair.second->isArchive())
            ret.push_back(pair.second);

    return ret;
}

std::vector<CVar*> CVarManager::cvars() const
{
    std::vector<CVar*> ret;
    for (const std::pair<std::string, CVar*>& pair : m_cvars)
        ret.push_back(pair.second);

    return ret;
}

void CVarManager::deserialize(CVar* cvar)
{
    if (!cvar || (!cvar->isArchive() && !cvar->isInternalArchivable()))
        return;

    CVarContainer container;
#if _WIN32
    hecl::SystemString filename = hecl::SystemString(m_store.getStoreRoot()) + _S('/') + com_configfile->toWideLiteral();
#else
    hecl::SystemString filename = hecl::SystemString(m_store.getStoreRoot()) + _S('/') + com_configfile->toLiteral();
#endif
    hecl::Sstat st;

    if (m_useBinary)
    {
        filename += _S(".bin");
        if (hecl::Stat(filename.c_str(), &st) || !S_ISREG(st.st_mode))
            return;
        athena::io::FileReader reader(filename);
        if (reader.isOpen())
            container.read(reader);
    }
    else
    {
        filename += _S(".yaml");
        if (hecl::Stat(filename.c_str(), &st) || !S_ISREG(st.st_mode))
            return;
        athena::io::FileReader reader(filename);
        if (reader.isOpen())
            container.fromYAMLStream(reader);
    }


    if (container.cvars.size() > 0)
    {
        auto serialized = std::find_if(container.cvars.begin(), container.cvars.end(), [&cvar](const DNACVAR::CVar& c) -> bool
        { return c.m_name == cvar->name(); });

        if (serialized != container.cvars.end())
        {
            DNACVAR::CVar& tmp = *serialized;

            if (cvar->m_value != tmp.m_value)
            {
                cvar->unlock();
                cvar->fromLiteralToType(tmp.m_value, true);
                cvar->m_wasDeserialized = true;
                cvar->lock();
            }
        }
    }
}

void CVarManager::serialize()
{
    CVarContainer container;
    for (const std::pair<std::string, CVar*>& pair : m_cvars)
        if (pair.second->isArchive() || (pair.second->isInternalArchivable() && pair.second->wasDeserialized() && !pair.second->hasDefaultValue()))
        {
            CVar tmp = *pair.second;
            container.cvars.push_back(tmp);
        }

    container.cvarCount = atUint32(container.cvars.size());

#if _WIN32
    hecl::SystemString filename = hecl::SystemString(m_store.getStoreRoot()) + _S('/') + com_configfile->toWideLiteral();
#else
    hecl::SystemString filename = hecl::SystemString(m_store.getStoreRoot()) + _S('/') + com_configfile->toLiteral();
#endif

    if (m_useBinary)
    {
        filename += _S(".bin");
        athena::io::FileWriter writer(filename);
        if (writer.isOpen())
            container.write(writer);
    }
    else
    {
        filename += _S(".yaml");
        athena::io::FileWriter writer(filename);
        if (writer.isOpen())
            container.toYAMLStream(writer);
    }
}

CVarManager* CVarManager::instance()
{
    return m_instance;
}

void CVarManager::list(Console* con, const std::vector<std::string>& /*args*/)
{
    for (const auto& cvar : m_cvars)
    {
        if (!cvar.second->isHidden())
            con->report(Console::Level::Info, "%s: %s", cvar.first.c_str(), cvar.second->help().c_str());
    }
}

void CVarManager::setCVar(Console* con, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        con->report(Console::Level::Info, "Usage setCvar <cvar> <value>");
        return;
    }

    std::string cvName = args[0];
    athena::utility::tolower(cvName);
    if (m_cvars.find(cvName) == m_cvars.end())
    {
        con->report(Console::Level::Error, "CVar '%s' does not exist", args[0].c_str());
        return;
    }

    CVar* cv = m_cvars[cvName];
    std::string value = args[1];
    auto it = args.begin() + 2;
    for (; it != args.end(); ++it)
        value += " " + *it;

    if (!cv->fromLiteralToType(value))
        con->report(Console::Level::Warning, "Unable to set cvar '%s' to value '%s'", args[0].c_str(), value.c_str());
}

void CVarManager::getCVar(Console* con, const std::vector<std::string> &args)
{
    if (args.empty())
    {
        con->report(Console::Level::Info, "Usage getCVar <cvar>");
        return;
    }

    std::string cvName = args[0];
    athena::utility::tolower(cvName);
    if (m_cvars.find(cvName) == m_cvars.end())
    {
        con->report(Console::Level::Error, "CVar '%s' does not exist", args[0].c_str());
        return;
    }

    const CVar* cv = m_cvars[cvName];
    con->report(Console::Level::Info, "'%s' = '%s'", cv->name().data(), cv->value().c_str());
}

void CVarManager::setDeveloperMode(bool v, bool setDeserialized)
{
    com_developer->unlock();
    com_developer->fromBoolean(v);
    if (setDeserialized)
        com_developer->m_wasDeserialized = true;
    com_developer->lock();
    com_developer->setModified();

}

bool CVarManager::restartRequired() const
{
    for (const auto& cv : m_cvars)
    {
        if (cv.second->isModified() && cv.second->modificationRequiresRestart())
            return true;
    }

    return false;
}

bool CVarManager::suppressDeveloper()
{
    bool oldDeveloper = com_developer->toBoolean();
    CVarUnlocker unlock(com_developer);
    com_developer->fromBoolean(false);

    return oldDeveloper;
}

void CVarManager::restoreDeveloper(bool oldDeveloper)
{
    CVarUnlocker unlock(com_developer);
    com_developer->fromBoolean(oldDeveloper);
}

}
