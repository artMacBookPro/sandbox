#ifndef SB_JSON_H_INCLUDED
#define SB_JSON_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_vector.h>

#include "sb_lua_context.h"

namespace GHL {
    struct Data;
    struct DataStream;
}

namespace Sandbox {
    
    
    void register_json(lua_State* L);
    
    sb::string convert_to_json(const LuaContextPtr& ctx);
    LuaContextPtr convert_from_json(lua_State* L,const char* json);
    LuaContextPtr convert_from_json(lua_State* L,const GHL::Data* data);
    
    bool json_parse_object(const char* data,sb::map<sb::string,sb::string>& res);
    
    class JsonBuilderBase {
    protected:
        struct Impl;
        struct ImplRef;
        Impl* m_impl;
        virtual ~JsonBuilderBase();
        explicit JsonBuilderBase();
        
        virtual void Write(const char* data,size_t len) = 0;
    private:
        JsonBuilderBase(const JsonBuilderBase&);
        JsonBuilderBase& operator = (const JsonBuilderBase&);
    public:
 
        void reset();
        
        JsonBuilderBase& SetPretty(bool p);
        
        JsonBuilderBase& BeginObject();
        JsonBuilderBase& EndObject();
        
        JsonBuilderBase& BeginArray();
        JsonBuilderBase& EndArray();
        
        JsonBuilderBase& Key(const char* name);
        JsonBuilderBase& PutNull();
        JsonBuilderBase& PutBool(bool v);
        JsonBuilderBase& PutString(const char* value);
        JsonBuilderBase& PutInteger(long long value);
        JsonBuilderBase& PutNumber(double value);
        
        
    };
    
    class JsonBuilderRef : public JsonBuilderBase {
    protected:
        sb::string& m_data;
        virtual void Write(const char* data,size_t len) SB_OVERRIDE {
            m_data.append(data,len);
        }
    public:
        explicit JsonBuilderRef(sb::string& data) : m_data(data) {}
        ~JsonBuilderRef() {}
        const sb::string& End() const { return m_data; }
    };

    class JsonBuilder : public JsonBuilderBase {
    private:
        JsonBuilder(const JsonBuilder&);
        JsonBuilder& operator = (const JsonBuilder&);
        sb::string  m_out_data;
        virtual void Write(const char* data,size_t len) SB_OVERRIDE {
            m_out_data.append(data,len);
        }
    public:
        void reset();
        JsonBuilder();
        ~JsonBuilder();
        
        const sb::string& End() const { return m_out_data; }
    };
    
    class JsonTraverser {
    private:
        sb::vector<char> m_stack;
        sb::string m_error;
        
        void BeginObject();
        void EndObject();
        void BeginArray();
        void EndArray();
        
        struct Ctx;
    protected:
        size_t GetDepth() const;
        bool IsObject() const;
        bool IsArray() const;
    public:
        JsonTraverser();
        
        virtual void OnBeginObject() {}
        virtual void OnEndObject() {}
        virtual void OnBeginArray() {}
        virtual void OnEndArray() {}
        virtual void OnKey(const sb::string& v) {}
        virtual void OnNull() {}
        virtual void OnBool(bool v) {}
        virtual void OnString(const sb::string& v) {}
        virtual void OnInteger(long long v) {}
        virtual void OnNumber(double v) {}
        
        
        bool TraverseStream( GHL::DataStream* ds );
        bool TraverseString( const char* ds );
        
        const sb::string& GetError() const { return m_error; }
    };
    
    
}

#endif
