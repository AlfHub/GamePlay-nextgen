#ifndef SCRIPTCONTROLLER_H_
#define SCRIPTCONTROLLER_H_

#include "Script.h"
#include "Game.h"

namespace gameplay
{

/**
 * Controls and manages all scripts.
 */
class ScriptController
{
    friend class Game;
    friend class Platform;
    friend class Script;
    friend class ScriptUtil;
    friend class ScriptTimeListener;

public:

    /**
     * Loads the given script file and executes its code (if it is not
     * alreay loaded).
     *
     * The script is loaded into an environment that is defined by the scope parameter.
     * If the script scope is GLOBAL and the forceReload parameter is false, a
     * previously-loaded script object may be returned. PROTECTED scope always results
     * in a new script being loaded and executed.
     * 
     * @param path The path to the script.
     * @param scope The scope for the script to be executed in.
     * @param forceReload Whether the script should be reloaded if it has already been loaded
     *      (applicable for GLOBAL scripts only).
     *
     * @return The loaded script, or NULL if the script could not be loaded.
     */
    Script* loadScript(const char* path, Script::Scope scope = Script::GLOBAL, bool forceReload = false);

    /**
     * Calls a zero-parameter global function.
     * 
     * @param func The name of the function to call.
     * 
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(const char* func);

    /**
     * Calls a zero-parameter function.
     * 
     * @param script The script to execute the function on, or NULL for the global script environment.
     * @param func The name of the function to call.
     * 
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(Script* script, const char* func);

    /**
     * Calls a global function using the given parameters.
     * 
     * @param func The name of the function to call.
     * @param args The argument signature of the function. Of the form 'xxx', where each 'x' is a parameter type and must be one of:
     *      - 'b' - bool
     *      - 'c' - char
     *      - 'h' - short
     *      - 'i' - int
     *      - 'l' - long
     *      - 'f' - float
     *      - 'd' - double
     *      - 'ui' - unsigned int
     *      - 'ul' - unsigned long
     *      - 'uc' - unsigned char
     *      - 'uh' - unsigned short
     *      - 's' - string
     *      - 'p' - pointer
     *      - '<object-type>' - a <b>pointer</b> to an object of the given type (where the qualified type name is enclosed by angle brackets).
     *      - '[enum-type]' - an enumerated value of the given type (where the qualified type name is enclosed by square brackets).
     * @param ... Parameters to pass to the script fucntion, as specified by the args parameter.
     *
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(const char* func, const char* args, ...);

    /**
     * Calls the a function from the specified script, using the given parameters.
     * 
     * @param script The script to execute the function on, or NULL for the global script environment.
     * @param func The name of the function to call.
     * @param args The argument signature of the function. Of the form 'xxx', where each 'x' is a parameter type and must be one of:
     *      - 'b' - bool
     *      - 'c' - char
     *      - 'h' - short
     *      - 'i' - int
     *      - 'l' - long
     *      - 'f' - float
     *      - 'd' - double
     *      - 'ui' - unsigned int
     *      - 'ul' - unsigned long
     *      - 'uc' - unsigned char
     *      - 'uh' - unsigned short
     *      - 's' - string
     *      - 'p' - pointer
     *      - '<object-type>' - a <b>pointer</b> to an object of the given type (where the qualified type name is enclosed by angle brackets).
     *      - '[enum-type]' - an enumerated value of the given type (where the qualified type name is enclosed by square brackets).
     * @param ... Parameters to pass to the script fucntion, as specified by the args parameter.
     * 
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(Script* script, const char* func, const char* args, ...);

    /**
     * Calls the specified global Lua function using the given parameters.
     * 
     * @param func The name of the function to call.
     * @param args The optional argument signature of the function. Of the form 'xxx', where each 'x' is a parameter type and must be one of:
     *      - 'b' - bool
     *      - 'c' - char
     *      - 'h' - short
     *      - 'i' - int
     *      - 'l' - long
     *      - 'f' - float
     *      - 'd' - double
     *      - 'ui' - unsigned int
     *      - 'ul' - unsigned long
     *      - 'uc' - unsigned char
     *      - 'uh' - unsigned short
     *      - 's' - string
     *      - 'p' - pointer
     *      - '<object-type>' - a <b>pointer</b> to an object of the given type (where the qualified type name is enclosed by angle brackets).
     *      - '[enum-type]' - an enumerated value of the given type (where the qualified type name is enclosed by square brackets).
     * @param list The variable argument list containing the function's parameters, or NULL for an empty parameter list.
     *
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(const char* func, const char* args, va_list* list);

    /**
     * Calls the specified global Lua function using the given parameters.
     * 
     * @param script Optional script to use, or NULL for the global script environment.
     * @param func The name of the function to call.
     * @param args The optional argument signature of the function. Of the form 'xxx', where each 'x' is a parameter type and must be one of:
     *      - 'b' - bool
     *      - 'c' - char
     *      - 'h' - short
     *      - 'i' - int
     *      - 'l' - long
     *      - 'f' - float
     *      - 'd' - double
     *      - 'ui' - unsigned int
     *      - 'ul' - unsigned long
     *      - 'uc' - unsigned char
     *      - 'uh' - unsigned short
     *      - 's' - string
     *      - 'p' - pointer
     *      - '<object-type>' - a <b>pointer</b> to an object of the given type (where the qualified type name is enclosed by angle brackets).
     *      - '[enum-type]' - an enumerated value of the given type (where the qualified type name is enclosed by square brackets).
     * @param list The variable argument list containing the function's parameters, or NULL for an empty parameter list.
     *
     * @return The return value of the executed Lua function.
     *
     * @script{ignore}
     */
    template<typename T> T executeFunction(Script* script, const char* func, const char* args, va_list* list);

    /**
     * Gets the global boolean script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a bool.
     * @param script Optional script to use, or NULL for the global script environment.
     *
     * @return The global boolean script variable.
     * 
     * @script{ignore}
     */
    bool getBool(const char* name, bool defaultValue = false, Script* script = NULL);

    /**
     * Gets the global char script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global char script variable.
     * 
     * @script{ignore}
     */
    char getChar(const char* name, char defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global short script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global short script variable.
     * 
     * @script{ignore}
     */
    short getShort(const char* name, short defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global int script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global int script variable.
     * 
     * @script{ignore}
     */
    int getInt(const char* name, int defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global long script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global long script variable.
     * 
     * @script{ignore}
     */
    long getLong(const char* name, long defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global unsigned char script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global unsigned char script variable.
     * 
     * @script{ignore}
     */
    unsigned char getUnsignedChar(const char* name, unsigned char defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global unsigned short script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global unsigned short script variable.
     * 
     * @script{ignore}
     */
    unsigned short getUnsignedShort(const char* name, unsigned short defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global unsigned int script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global unsigned int script variable.
     * 
     * @script{ignore}
     */
    unsigned int getUnsignedInt(const char* name, unsigned int defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global unsigned long script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global unsigned long script variable.
     * 
     * @script{ignore}
     */
    unsigned long getUnsignedLong(const char* name, unsigned long defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global float script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global float script variable.
     * 
     * @script{ignore}
     */
    float getFloat(const char* name, float defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global double script variable with the given name.
     * 
     * @param name The name of the variable.
     * @param defaultValue The default value to return if the variable is not a number.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global double script variable.
     * 
     * @script{ignore}
     */
    double getDouble(const char* name, double defaultValue = 0, Script* script = NULL);

    /**
     * Gets the global string variable with the given name.
     *
     * The returned character string is temporary and is not gauranteed to live forever, therefore you
     * should not store a pointer to the returned value. If long-term access to the returned string is
     * required, it should be copied into another string for storage.
     *
     * @param name The name of the variable.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The string variable or NULL if the variable is not a string.
     * 
     * @script{ignore}
     */
    const char* getString(const char* name, Script* script = NULL);

    /**
     * Gets the global pointer script variable of the given type with the given name.
     * 
     * @param type The type of the variable in Lua.
     * @param name The name of the variable.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @return The global pointer script variable.
     * 
     * @script{ignore}
     */
    void* getObjectPointer(const char* type, const char* name, Script* script = NULL);

    /**
     * Sets the global boolean script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The boolean value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setBool(const char* name, bool v, Script* script = NULL);

    /**
     * Sets the global char script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The char value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setChar(const char* name, char v, Script* script = NULL);

    /**
     * Sets the global short script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The short value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setShort(const char* name, short v, Script* script = NULL);

    /**
     * Sets the global int script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The int value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setInt(const char* name, int v, Script* script = NULL);

    /**
     * Sets the global long script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The long value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setLong(const char* name, long v, Script* script = NULL);

    /**
     * Gets the global unsigned char script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The unsigned char value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setUnsignedChar(const char* name, unsigned char v, Script* script = NULL);

    /**
     * Sets the global unsigned short script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The unsigned short value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setUnsignedShort(const char* name, unsigned short v, Script* script = NULL);

    /**
     * Sets the global unsigned int script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The unsigned int value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setUnsignedInt(const char* name, unsigned int v, Script* script = NULL);

    /**
     * Sets the global unsigned long script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The unsigned long value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setUnsignedLong(const char* name, unsigned long v, Script* script = NULL);

    /**
     * Sets the global float script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The float value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setFloat(const char* name, float v, Script* script = NULL);

    /**
     * Sets the global double script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The double value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setDouble(const char* name, double v, Script* script = NULL);

    /**
     * Sets the global string script variable with the given name to the given value.
     * 
     * @param name The name of the script variable.
     * @param v The string value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setString(const char* name, const char* v, Script* script = NULL);

    /**
     * Sets the global pointer script variable of the given type with the given name to the given value.
     *
     * When passing an argument for the 'v' parameter, it is recommended that the object passed in
     * be converted to void* using 'dynamic_cast<void*>(o)', to ensure that the address of the 
     * most derived class possible be stored. In this case, the 'type' parameter should also be
     * set to the most derived type to ensure that the full set of methods for that object are
     * available to the script engine.
     *
     * @param type The type of the object point value being set.
     * @param name The name of the variable.
     * @param v The pointer value.
     * @param script Optional script to use, or NULL for the global script environment.
     * 
     * @script{ignore}
     */
    void setObjectPointer(const char* type, const char* name, void* v, Script* script = NULL);

    /**
     * Determines if there exists a function with the specified name in the given script environment.
     *
     * @param name The name of the function to check.
     * @param script Optional script to inspect, or NULL to inspect the global script environment.
     *
     * @return True if the specified function exists, false otherwise.
     */
    bool functionExists(const char* name, const Script* script = NULL) const;

    /**
     * Returns the currently executing script.
     *
     * Note that this function returns the currently executing Script object,
     * which is not guaranteed to be valid unless the returned script's
     * reference count is increased while the caller holds onto it.
     *
     * @return The currently executing script, or NULL if either there is no currently
     *      executing script or the global script environment is current.
     */
    Script* getCurrentScript() const;

    /**
     * Prints the string to the platform's output stream or log file.
     * Used for overriding Lua's print function.
     * 
     * @param str The string to print.
     */
    static void print(const char* str);

    /**
     * Prints the strings to the platform's output stream or log file.
     * Used for overriding Lua's print function.
     * 
     * @param str1 The first string to print.
     * @param str2 The second string to print on the same line as str1.
     */
    static void print(const char* str1, const char* str2);

private:

    /**
     * Allows time listener interaction from Lua scripts.
     */
    struct ScriptTimeListener : public TimeListener
    {
        /**
         * Constructor.
         */
        ScriptTimeListener(Script* script, const char* function);

        /**
         * Destructor.
         */
        ~ScriptTimeListener();

        /**
         * @see TimeListener#timeEvent(long, void*)
         */
        void timeEvent(long timeDiff, void* cookie);

        /** Holds the script to execute the function within. */
        Script* script;
        /** Holds the name of the Lua script function to call back. */
        std::string function;
    };

    /**
     * Constructor.
     */
    ScriptController();

    /**
     * Copy constructor.
     */
    ScriptController(const ScriptController& copy);

    /**
     * Destructor.
     */
    ~ScriptController();

    /**
     * Called to initialize the script controller.
     */
    void initialize();

    /*
     * Called to finalize the script controller.
     */
    void finalize();

    /**
     * Internal loadScript variant that supports loading into an existing Script object
     * for reloading purposes.
     *
     * @param script A Script object containing a valid path and scope to load.
     *
     * @return True if the script is successfully loaded, false otherwise.
     *
     * @see loadScript(const char*, Script::scope, bool)
     */
    bool loadScript(Script* script);

    /**
     * Attempts to unload the specified script.
     *
     * Unloading a script causes the non-global data within the script to be
     * released such that a reference is no longer held to it. It will then
     * be garbage collected at some point in the future. If the script stored
     * any data in the global environment or if any other code still references
     * data from the script, that data will not be freed.
     *
     * Global scripts cannot be explicitly unloaded and this function will do
     * nothing for such scripts.
     *
     * @param script The script to be unloaded.
     */
    void unloadScript(Script* script);

    /**
     * Calls the specified Lua function using the given parameters.
     * 
     * @param resultCount The expected number of returned values.
     * @param func The name of the function to call.
     * @param args The optional argument signature of the function, as a string of the form
     *      'xxx', where each 'x' is a parameter type and must be one of:
     *      - 'b' - bool
     *      - 'c' - char
     *      - 'h' - short
     *      - 'i' - int
     *      - 'l' - long
     *      - 'f' - float
     *      - 'd' - double
     *      - 'ui' - unsigned int
     *      - 'ul' - unsigned long
     *      - 'uc' - unsigned char
     *      - 'uh' - unsigned short
     *      - 's' - string
     *      - 'p' - pointer
     *      - '<object-type>' - a <b>pointer</b> to an object of the given type (where the qualified type name is enclosed by angle brackets).
     *      - '[enum-type]' - an enumerated value of the given type (where the qualified type name is enclosed by square brackets).
     * @param list The variable argument list.
     * @param script Optional script to execute the function in, or NULL for to execute it in the global environment.
     */
    void executeFunctionHelper(int resultCount, const char* func, const char* args, va_list* list, Script* script = NULL);

    /**
     * Schedules a script function to execute after the given time interval.
     *
     * This function is executed in the environment of the script that calls this function.
     *
     * @param timeOffset The number of game milliseconds in the future to schedule the event to be fired.
     * @param function The Lua script function that will receive the event.
     */
    void schedule(float timeOffset, const char* function);

    void pushScript(Script* script);

    void popScript();

    unsigned int _returnCount;
    std::map<std::string, std::vector<std::string> > _hierarchy;
    std::map<std::string, std::vector<Script*> > _scripts;
    std::vector<Script*> _envStack;
    std::list<ScriptTimeListener*> _timeListeners;
};

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(const char* func);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(const char* func);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(const char* func);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(const char* func);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(const char* func);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(const char* func);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(const char* func);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(const char* func);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(const char* func);

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(Script* script, const char* func);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(Script* script, const char* func);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(Script* script, const char* func);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(Script* script, const char* func);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(Script* script, const char* func);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(Script* script, const char* func);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(Script* script, const char* func);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func);

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(const char* func, const char* args, ...);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(const char* func, const char* args, ...);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(const char* func, const char* args, ...);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(const char* func, const char* args, ...);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(const char* func, const char* args, ...);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func, const char* args, ...);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(const char* func, const char* args, ...);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(const char* func, const char* args, ...);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(const char* func, const char* args, ...);

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(Script* script, const char* func, const char* args, ...);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func, const char* args, ...);

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(const char* func, const char* args, va_list* list);

/** Template specialization. */
template<> void ScriptController::executeFunction<void>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> bool ScriptController::executeFunction<bool>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> char ScriptController::executeFunction<char>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> short ScriptController::executeFunction<short>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> int ScriptController::executeFunction<int>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> long ScriptController::executeFunction<long>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned char ScriptController::executeFunction<unsigned char>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned short ScriptController::executeFunction<unsigned short>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned int ScriptController::executeFunction<unsigned int>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> unsigned long ScriptController::executeFunction<unsigned long>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> float ScriptController::executeFunction<float>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> double ScriptController::executeFunction<double>(Script* script, const char* func, const char* args, va_list* list);
/** Template specialization. */
template<> std::string ScriptController::executeFunction<std::string>(Script* script, const char* func, const char* args, va_list* list);

}

#include "ScriptController.inl"

#endif
