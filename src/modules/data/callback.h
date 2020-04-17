#pragma once

#include <modules\data.h>
#include <modules\data\base.h>

namespace MM2
{
    // Forward declarations
    class datCallback;

    // External declarations
    extern class Base;

    // Class definitions

    namespace $
    {
        namespace datCallback
        {
            declhook(0x4C7A40, _MemberFunc<void>, $$ctor$void);

            declhook(0x4C7A50, _MemberFunc<void>, $$ctor$ThisCB$1);
            declhook(0x4C7A80, _MemberFunc<void>, $$ctor$ThisCB$2);
            declhook(0x4C7AA0, _MemberFunc<void>, $$ctor$ThisCB$3);

            declhook(0x4C7AC0, _MemberFunc<void>, $$ctor$CB$1);
            declhook(0x4C7AF0, _MemberFunc<void>, $$ctor$CB$2);
            declhook(0x4C7B20, _MemberFunc<void>, $$ctor$CB$3);

            declhook(0x4C7B50, _MemberFunc<void>, Call);
        }
    }

    class datCallback
    {
    protected:
        Base* _class {nullptr};
        uintptr_t _callback {0};
        void* _parameter {nullptr};

        enum : uintptr_t
        {
            datCallback_ParamCount0 = 0x1,
            datCallback_ParamCount1 = 0x2,
            datCallback_ParamCount2 = 0x3,
            datCallback_ParamCountFlags = 0x3,
        };

        template <typename T>
        static inline uintptr_t TaggedCallback(T value, uintptr_t flag)
        {
            static_assert(sizeof(T) == sizeof(uintptr_t), "Invalid Callback Type");

            uintptr_t address = 0;
            std::memcpy(&address, &value, sizeof(uintptr_t));

            if ((address & datCallback_ParamCountFlags) != 0)
            {
                Quitf("Misaligned callback");
            }

            return address | flag;
        }

    public:
        datCallback() = default;

        datCallback(void (Base::*callback)(void), Base* this_param)
            : _class(this_param)
            , _callback(TaggedCallback(callback, datCallback_ParamCount0))
            , _parameter(nullptr)
        {
            if (!this_param)
                Quitf("Passed member callback with null 'this' parameter");
        }

        datCallback(void (Base::*callback)(void*), Base* this_param, void* void_param)
            : _class(this_param)
            , _callback(TaggedCallback(callback, datCallback_ParamCount1))
            , _parameter(void_param)
        {
            if (!this_param)
                Quitf("Passed member callback with null 'this' parameter");
        }

        datCallback(void (Base::*callback)(void*, void*), Base* this_param, void* void_param)
            : _class(this_param)
            , _callback(TaggedCallback(callback, datCallback_ParamCount2))
            , _parameter(void_param)
        {
            if (!this_param)
                Quitf("Passed member callback with null 'this' parameter");
        }

        datCallback(void (*callback)(void))
            : _class(nullptr)
            , _callback(TaggedCallback(callback, datCallback_ParamCount0))
            , _parameter(nullptr)
        {}

        datCallback(void (*callback)(void*), void* void_param)
            : _class(nullptr)
            , _callback(TaggedCallback(callback, datCallback_ParamCount1))
            , _parameter(void_param)
        {}

        datCallback(void (*callback)(void*, void*), void* void_param)
            : _class(nullptr)
            , _callback(TaggedCallback(callback, datCallback_ParamCount2))
            , _parameter(void_param)
        {}

        uintptr_t GetCallback()
        {
            return _callback & ~datCallback_ParamCountFlags;
        }

        uintptr_t GetType()
        {
            return _callback & datCallback_ParamCountFlags;
        }

        void Call(void* parameter)
        {
            uintptr_t type = GetType();
            uintptr_t callback = GetCallback();

            if (type)
            {
                if (_class)
                {
                    switch (type)
                    {
                        case datCallback_ParamCount0: return reinterpret_cast<void (__thiscall*)(Base*)>(callback)(_class);
                        case datCallback_ParamCount1: return reinterpret_cast<void (__thiscall*)(Base*, void*)>(callback)(_class, _parameter);
                        case datCallback_ParamCount2: return reinterpret_cast<void (__thiscall*)(Base*, void*, void*)>(callback)(_class, _parameter, parameter);
                    }
                }
                else
                {
                    switch (type)
                    {
                        case datCallback_ParamCount0: return reinterpret_cast<void (*)()>(callback)();
                        case datCallback_ParamCount1: return reinterpret_cast<void (*)(void*)>(callback)(_parameter);
                        case datCallback_ParamCount2:
                            return reinterpret_cast<void (*)(void*, void*)>(callback)(_parameter, parameter);
                    }
                }
            }
        }

        static ageHook::TypeProxy<datCallback> NullCallback;

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datCallback>("datCallback")
                .endClass();
        }
    };

    ASSERT_SIZEOF(datCallback, 0xC);

    // Lua initialization
}