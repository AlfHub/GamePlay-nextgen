#include "Base.h"
#include "FileSystem.h"
#include "ScriptController.h"

namespace gameplay
{

Script* ScriptController::loadScript(const char* path, Script::Scope scope, bool forceReload)
{
    return NULL;
}

bool ScriptController::loadScript(Script* script)
{
    return false;
}

void ScriptController::unloadScript(Script* script)
{
}

bool ScriptController::getBool(const char* name, bool defaultValue, Script* script)
{
    return false;
}

char ScriptController::getChar(const char* name, char defaultValue, Script* script)
{
    return 0;
}

short ScriptController::getShort(const char* name, short defaultValue, Script* script)
{
    return 0;
}

int ScriptController::getInt(const char* name, int defaultValue, Script* script)
{
    return 0;
}

long ScriptController::getLong(const char* name, long defaultValue, Script* script)
{
    return 0;
}

unsigned char ScriptController::getUnsignedChar(const char* name, unsigned char defaultValue, Script* script)
{
    return 0;
}

unsigned short ScriptController::getUnsignedShort(const char* name, unsigned short defaultValue, Script* script)
{
    return 0;
}

unsigned int ScriptController::getUnsignedInt(const char* name, unsigned int defaultValue, Script* script)
{
    return 0;
}

unsigned long ScriptController::getUnsignedLong(const char* name, unsigned long defaultValue, Script* script)
{
    return 0;
}

float ScriptController::getFloat(const char* name, float defaultValue, Script* script)
{
    return 0;
}

double ScriptController::getDouble(const char* name, double defaultValue, Script* script)
{
    return 0;
}

const char* ScriptController::getString(const char* name, Script* script)
{
    return 0;
}

void* ScriptController::getObjectPointer(const char* type, const char* name, Script* script)
{
    return 0;
}

void ScriptController::setBool(const char* name, bool v, Script* script)
{
}

void ScriptController::setChar(const char* name, char v, Script* script)
{
}

void ScriptController::setShort(const char* name, short v, Script* script)
{
}

void ScriptController::setInt(const char* name, int v, Script* script)
{
}

void ScriptController::setLong(const char* name, long v, Script* script)
{
}

void ScriptController::setUnsignedChar(const char* name, unsigned char v, Script* script)
{
}

void ScriptController::setUnsignedShort(const char* name, unsigned short v, Script* script)
{
}

void ScriptController::setUnsignedInt(const char* name, unsigned int v, Script* script)
{
}

void ScriptController::setUnsignedLong(const char* name, unsigned long v, Script* script)
{
}

void ScriptController::setFloat(const char* name, float v, Script* script)
{
}

void ScriptController::setDouble(const char* name, double v, Script* script)
{
}

void ScriptController::setString(const char* name, const char* v, Script* script)
{
}

void ScriptController::setObjectPointer(const char* type, const char* name, void* v, Script* script)
{
}

bool ScriptController::functionExists(const char* name, const Script* script) const
{
    return false;
}

Script* ScriptController::getCurrentScript() const
{
    return _envStack.empty() ? NULL : _envStack.back();
}

void ScriptController::print(const char* str)
{
    gameplay::print("%s", str);
}

void ScriptController::print(const char* str1, const char* str2)
{
    gameplay::print("%s%s", str1, str2);
}

ScriptController::ScriptController()
{
}

ScriptController::~ScriptController()
{
}

void ScriptController::initialize()
{
    // Pass game command-line arguments to scripting system
    std::ostringstream args;
    int argc;
    char** argv;
    Game::getInstance()->getArguments(&argc, &argv);
    args << "arg = { }\n";
    for (int i = 0; i < argc; ++i)
    {
        args << "arg[" << (i) << "] = [[" << argv[i] << "]]\n";
    }
    std::string argsStr = args.str();
    if (argsStr.length() > 0)
    {
        // TODO
    }
}

void ScriptController::finalize()
{
    // Cleanup any outstanding time listeners
    for (std::list<ScriptTimeListener*>::iterator itr = _timeListeners.begin(); itr != _timeListeners.end(); ++itr)
    {
        SAFE_DELETE(*itr);
    }
    _timeListeners.clear();

}

void ScriptController::schedule(float timeOffset, const char* function)
{
    // Get the currently execute script
    Script* script = _envStack.empty() ? NULL : _envStack.back();
    if (script)
    {
        // Increase the reference count of the script while we hold it so it doesn't
        // get destroyed while waiting for the event to fire.
        script->addRef();
    }

    ScriptTimeListener* listener = new ScriptTimeListener(script, function);
    _timeListeners.push_back(listener);

    Game::getInstance()->schedule(timeOffset, listener, NULL);
}

void ScriptController::pushScript(Script* script)
{
    // Increase the reference count of the script while it's pushed,
    // to prevent it from being destroyed during this time.
    if (script)
        script->addRef();

    _envStack.push_back(script);
}

void ScriptController::popScript()
{
    GP_ASSERT(!_envStack.empty());

    Script* script = _envStack.back();

    _envStack.pop_back();

    SAFE_RELEASE(script);
}

ScriptController::ScriptTimeListener::ScriptTimeListener(Script* script, const char* function) : script(script), function(function)
{
}

ScriptController::ScriptTimeListener::~ScriptTimeListener()
{
    // Release 
    SAFE_RELEASE(script);
}

void ScriptController::ScriptTimeListener::timeEvent(long timeDiff, void* cookie)
{
    // Remove ourself from the script controller's list
    std::list<ScriptTimeListener*>& list = Game::getInstance()->getScriptController()->_timeListeners;
    std::list<ScriptTimeListener*>::iterator itr = std::find(list.begin(), list.end(), this);
    if (itr != list.end())
        list.erase(itr);

    // Call the script function
    Game::getInstance()->getScriptController()->executeFunction<void>(script, function.c_str(), "l", timeDiff);

    // Free ourself.
    // IMPORTANT: Don't do anything else after this line!!
    delete this;
}

template<> void ScriptController::executeFunction<void>(const char* func)
{
}

template<> bool ScriptController::executeFunction<bool>(const char* func)
{
    return false;
}

template<> char ScriptController::executeFunction<char>(const char* func)
{
    return 0;
}

template<> short ScriptController::executeFunction<short>(const char* func)
{
    return 0;
}

template<> int ScriptController::executeFunction<int>(const char* func)
{
    return 0;
}

template<> long ScriptController::executeFunction<long>(const char* func)
{
    return 0;
}

template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func)
{
    return 0;
}

template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func)
{
    return 0;
}

template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func)
{
    return 0;
}

template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func)
{
    return 0;
}

template<> float ScriptController::executeFunction<float>(const char* func)
{
    return 0;
}

template<> double ScriptController::executeFunction<double>(const char* func)
{
    return 0;
}

template<> std::string ScriptController::executeFunction<std::string>(const char* func)
{
    return "";
}

template<> void ScriptController::executeFunction<void>(Script* script, const char* func)
{
}

template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func)
{
    return false;
}

template<> char ScriptController::executeFunction<char>(Script* script, const char* func)
{
    return 0;
}

template<> short ScriptController::executeFunction<short>(Script* script, const char* func)
{
    return 0;
}

template<> int ScriptController::executeFunction<int>(Script* script, const char* func)
{
    return 0;
}

template<> long ScriptController::executeFunction<long>(Script* script, const char* func)
{
    return 0;
}

template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func)
{
    return 0;
}

template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func)
{
    return 0;
}

template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func)
{
    return 0;
}

template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func)
{
    return 0;
}

template<> float ScriptController::executeFunction<float>(Script* script, const char* func)
{
    return 0;
}

template<> double ScriptController::executeFunction<double>(Script* script, const char* func)
{
    return 0;
}

template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func)
{
    return "";
}

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(const char* func, const char* args, ...)
{
}

/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(const char* func, const char* args, ...)
{
    return false;
}

/** Template specialization. */
template<> char ScriptController::executeFunction<char>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> short ScriptController::executeFunction<short>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> int ScriptController::executeFunction<int>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> long ScriptController::executeFunction<long>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> float ScriptController::executeFunction<float>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> double ScriptController::executeFunction<double>(const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(const char* func, const char* args, ...)
{
    return "";
}

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(Script* script, const char* func, const char* args, ...)
{
}

/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func, const char* args, ...)
{
    return false;
}

/** Template specialization. */
template<> char ScriptController::executeFunction<char>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> short ScriptController::executeFunction<short>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> int ScriptController::executeFunction<int>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> long ScriptController::executeFunction<long>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> float ScriptController::executeFunction<float>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> double ScriptController::executeFunction<double>(Script* script, const char* func, const char* args, ...)
{
    return 0;
}

/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func, const char* args, ...)
{
    return "";
}

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(const char* func, const char* args, va_list* list)
{
}

/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(const char* func, const char* args, va_list* list)
{
    return false;
}

/** Template specialization. */
template<> char ScriptController::executeFunction<char>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> short ScriptController::executeFunction<short>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> int ScriptController::executeFunction<int>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> long ScriptController::executeFunction<long>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> float ScriptController::executeFunction<float>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> double ScriptController::executeFunction<double>(const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(const char* func, const char* args, va_list* list)
{
    return "";
}

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(Script* script, const char* func, const char* args, va_list* list)
{
}

/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func, const char* args, va_list* list)
{
    return false;
}

/** Template specialization. */
template<> char ScriptController::executeFunction<char>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> short ScriptController::executeFunction<short>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> int ScriptController::executeFunction<int>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> long ScriptController::executeFunction<long>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> float ScriptController::executeFunction<float>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> double ScriptController::executeFunction<double>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func, const char* args, va_list* list)
{
    return 0;
}

}
