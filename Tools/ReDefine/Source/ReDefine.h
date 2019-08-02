#ifndef __REDEFINE__
#define __REDEFINE__    // :)

#include <functional>
#include <map>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

class Ini;

class ReDefine
{
public:

    struct ScriptCode;

    typedef std::map<std::string, std::map<int, std::string>>                                                    DefinesMap;
    typedef std::map<std::string, std::map<std::string, std::string>>                                            GenericOperatorsMap;
    typedef std::function<bool (ReDefine* self, const std::vector<std::string>& values, const ScriptCode& code)> ScriptEditIf;
    typedef std::function<void (ReDefine* self, const std::string& value, ScriptCode& code)>                     ScriptEditDo;
    typedef std::map<std::string, std::vector<std::string>>                                                      StringVectorMap;
    typedef std::map<std::string, std::map<std::string, unsigned int>>                                           UnknownMap;

    //
    // ReDefine
    //

    Ini* Config;

    struct SStatus
    {
        struct SCurrent
        {
            std::string  File;
            std::string  Line;
            unsigned int LineNumber;

            SCurrent();

            void         Clear();
        }
        Current;

        struct SProcess
        {
            // total number of files/lines processed

            unsigned int Files;
            unsigned int Lines;

            // total number of files/lines changes or (if running in read-only mode) change candidates

            unsigned int FilesChanges;
            unsigned int LinesChanges;

            UnknownMap   Unknown; // <type, <value,count>>

            SProcess();

            void         Clear();
        }
        Process;

        void Clear();
    }
    Status;

    ReDefine();
    virtual ~ReDefine();

    void Init();
    void Finish();

    void DEBUG( const char* func, const char* format, ... );
    void WARNING( const char* func, const char* format, ... );
    void LOG( const char* format, ... );
    void ILOG( const char* format, ... );

    bool ReadFile( const std::string& filename, std::vector<std::string>& lines );
    bool ReadConfig( const std::string& defines, const std::string& variable_prefix, const std::string& function_prefix, const std::string& raw, const std::string& script );

    void ProcessHeaders( const std::string& path );
    void ProcessScripts( const std::string& path, const bool readOnly = false );

    //
    // Defines
    //

    struct Header
    {
        const std::string Filename;
        const std::string Type;
        const std::string Prefix;
        const std::string Group;

        Header( const std::string& filename, const std::string& type, const std::string& prefix, const std::string& group );
    };

    // holds [Defines] between reading configuration step and processing headers
    std::vector<Header> Headers;

    DefinesMap          RegularDefines; // <type, <value, name>>
    DefinesMap          ProgramDefines; // <type, <value, names>>
    StringVectorMap     VirtualDefines; // <virtual_type, <types>>

    void FinishDefines();

    bool ReadConfigDefines( const std::string& sectionPrefix );

    bool IsDefineType( const std::string& type );
    bool IsRegularDefineType( const std::string& type );
    bool GetDefineName( const std::string& type, const int value, std::string& result, const bool skipVirtual = false );

    bool ProcessHeader( const std::string& path, const Header& header );
    bool ProcessValue( const std::string& type, std::string& value, const bool silent = false );
    void ProcessValueGuessing( std::string& value );

    //
    // Functions
    //

    StringVectorMap     FunctionsArguments; // <name, <types>>
    GenericOperatorsMap FunctionsOperators; // <name, <operatorName, type>>

    void FinishFunctions();

    bool ReadConfigFunctions( const std::string& sectionPrefix );

    void ProcessFunctionArguments( ScriptCode& function );

    //
    // Operators
    //

    std::map<std::string, std::string> Operators; // <operatorName, operator>

    void InitOperators();
    void FinishOperators();

    bool        IsOperator( const std::string& op );
    bool        IsOperatorName( const std::string& opName );
    std::string GetOperator( const std::string& opName );
    std::string GetOperatorName( const std::string& op );

    void ProcessOperator( const GenericOperatorsMap& map, ScriptCode& code );

    //
    // Raw
    //

    std::map<std::string, std::string> Raw;

    void FinishRaw();

    bool ReadConfigRaw( const std::string& section );

    void ProcessRaw( std::string& line );

    //
    // Script
    //

    struct ScriptCode
    {
        bool                     Function;

        std::string              Full; // Name + (Arguments) + (Operator + OperatorArguments)
        std::string              Name;
        std::vector<std::string> Arguments;
        std::string              Operator;
        std::string              OperatorArgument;

        ScriptCode();
    };

    struct ScriptEdit
    {
        struct Condition
        {
            std::string              Name;
            std::vector<std::string> Values;
        };

        struct Result
        {
            std::string Name;
            std::string Value;
        };

        std::string            Name;

        bool                   Before;
        bool                   After;

        std::vector<Condition> Conditions;
        std::vector<Result>    Results;

        ScriptEdit();
    };


    std::map<std::string, ScriptEditIf> EditIf;
    std::map<std::string, ScriptEditDo> EditDo;
    std::vector<ScriptEdit>             EditBefore;
    std::vector<ScriptEdit>             EditAfter;

    void InitScript();
    void FinishScript( bool finishCallbacks = true );

    bool ReadConfigScript( const std::string& sectionPrefix );

    std::string GetFullString( const ScriptCode& code );
    void        SetFullString( ScriptCode& code );

    void ProcessScript( const std::string& path, const std::string& filename, const bool readOnly = false );
    void ProcessScriptEdit( const std::vector<ScriptEdit>& edits, ScriptCode& code );

    //
    // Text
    //

    bool                     TextIsComment( const std::string& text );
    bool                     TextIsInt( const std::string& text );
    std::string              TextGetFilename( const std::string& path, const std::string& filename );
    bool                     TextGetInt( const std::string& text, int& result, const unsigned char& base = 10 );
    std::string              TextGetJoined( const std::vector<std::string>& text, const std::string& delimeter );
    std::string              TextGetLower( const std::string& text );
    std::string              TextGetPacked( const std::string& text );
    std::string              TextGetReplaced( const std::string& text, const std::string& from, const std::string& to );
    std::vector<std::string> TextGetSplitted( const std::string& text, const char& separator );

    std::string TextGetTrimmed( const std::string& text );

    bool       TextIsDefine( const std::string& text );
    bool       TextGetDefine( const std::string& text, const std::regex& re, std::string& name, int& value );
    std::regex TextGetDefineRegex( const std::string& prefix, bool paren );

    std::vector<ScriptCode> TextGetVariables( const std::string& text );
    std::vector<ScriptCode> TextGetFunctions( const std::string& text );

    //
    // Variables
    //

    GenericOperatorsMap      VariablesOperators; // <name, <operatorName, type>>
    std::vector<std::string> VariablesGuessing;  // <types>

    void FinishVariables();

    bool ReadConfigVariables( const std::string& sectionPrefix );
};

#endif // __REDEFINE__ //
