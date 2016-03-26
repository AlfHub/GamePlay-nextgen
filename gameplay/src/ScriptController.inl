#include "ScriptController.h"

namespace gameplay
{

template<typename T> T ScriptController::executeFunction(const char* func)
{
    return executeFunction<T>((Script*)NULL, func);
}

template<typename T> T ScriptController::executeFunction(Script* script, const char* func)
{
    T value;
    return value;
}

template<typename T> T ScriptController::executeFunction(const char* func, const char* args, ...)
{
    T value;
    return value;
}

template<typename T> T ScriptController::executeFunction(Script* script, const char* func, const char* args, ...)
{
    T value;
    return value;
}

template<typename T> T ScriptController::executeFunction(const char* func, const char* args, va_list* list)
{
    T value;
    return value;
}

template<typename T> T ScriptController::executeFunction(Script* script, const char* func, const char* args, va_list* list)
{
    T value;
    return value;
}

}
