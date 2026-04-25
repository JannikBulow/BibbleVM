// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_BIBBLEINTERFACE_H
#define BIBBLEVM_BIBBLEINTERFACE_H 1

#include <stdarg.h>
#include <stdint.h>

#ifdef _WIN32
#   ifdef __cplusplus
#       define BIBBLE_EXPORT extern "C" __declspec(dllexport)
#   else
#       define BIBBLE_EXPORT __declspec(dllexport)
#   endif
#   define BIBBLE_CALL __stdcall
#else
#   ifdef __cplusplus
#       define BIBBLE_EXPORT extern "C"
#   else
#       define BIBBLE_EXPORT
#   endif
#   define BIBBLE_CALL
#endif

#define BIBBLE_TRUE 1
#define BIBBLE_FALSE 0

#ifdef __cplusplus
#define BIBBLE_CLITERAL(T) T
#else
#define BIBBLE_CLITERAL(T) (T)
#endif

// Actual data
typedef int8_t VMByte;
typedef uint8_t VMUByte;
typedef int16_t VMShort;
typedef uint16_t VMUShort;
typedef int32_t VMInt;
typedef uint32_t VMUInt;
typedef int64_t VMLong;
typedef uint64_t VMULong;
typedef float VMFloat;
typedef double VMDouble;
typedef void* VMHandle;
typedef struct VMObject_* VMObject;
typedef VMObject VMArray;
typedef VMObject VMString;
typedef VMObject VMFuture;

typedef VMByte VMBool;

typedef struct VMModule_* VMModule;
typedef struct VMClass_* VMClass;
typedef const struct VMField_* VMField;
typedef const struct VMMethod_* VMMethod;
typedef struct VMFunction_* VMFunction;

typedef union VMValue {
    VMByte b;
    VMUByte ub;
    VMShort s;
    VMUShort us;
    VMInt i;
    VMUInt ui;
    VMLong l;
    VMULong ul;
    VMFloat f;
    VMDouble d;
    VMHandle h;
    VMObject obj;
} VMValue;

// Workaround for not having static types at runtime. Trusting the native authors a lot with this one...
typedef struct VMReturnValue {
    bool reference;
    VMValue value;
} VMReturnValue;

typedef struct VMArgument {
    bool reference;
    VMValue value;
} VMArgument;

typedef struct BibbleVM BibbleVM;
typedef struct BibbleInterface BibbleInterface;

struct BibbleInterface {
    void* reserved[4];

    VMModule (*GetModule)(BibbleVM*, const char*);

    VMObject (*NewGlobalReference)(BibbleVM*, VMObject);
    void (*DeleteGlobalReference)(BibbleVM*, VMObject);
    VMObject (*NewLocalReference)(BibbleVM*, VMObject);
    void (*PushLocalReferenceFrame)(BibbleVM*, VMUInt);
    void (*PopLocalReferenceFrame)(BibbleVM*);

    VMClass (*GetClass)(BibbleVM*, VMModule, const char*);
    VMClass (*GetSuperClass)(BibbleVM*, VMClass);
    VMBool (*IsAssignableFrom)(BibbleVM*, VMClass, VMClass);

    VMField (*GetField)(BibbleVM*, VMClass, const char*);
    VMValue (*GetFieldValue)(BibbleVM*, VMObject, VMField);
    void (*SetFieldValue)(BibbleVM*, VMObject, VMField, VMValue);

    VMMethod (*GetMethod)(BibbleVM*, VMClass, const char*);
    VMFunction (*DispatchMethod)(BibbleVM*, VMObject, VMMethod);
    VMFunction (*DispatchMethodNonVirtual)(BibbleVM*, VMClass, VMMethod); // Get the function associated with that exact method without vtable lookup. If the method is abstract, returns NULL

    VMFunction (*GetFunction)(BibbleVM*, VMModule, const char*);

    VMValue (*CallFunction)(BibbleVM*, BibbleInterface*, VMFunction, ...);
    VMValue (*CallFunctionV)(BibbleVM*, BibbleInterface*, VMFunction, va_list);
    VMValue (*CallFunctionA)(BibbleVM*, BibbleInterface*, VMFunction, const VMArgument*);
};



#ifdef __cplusplus
inline VMArgument BIBBLE_ARG_IMPL_(VMObject value) {
    return VMArgument{true, VMValue{.obj = value}};
}

inline VMArgument BIBBLE_ARG_IMPL_(VMHandle value) {
    return VMArgument{false, VMValue{.h = value}};
}

inline VMArgument BIBBLE_ARG_IMPL_(VMDouble value) {
    return VMArgument{false, VMValue{.d = value}};
}

inline VMArgument BIBBLE_ARG_IMPL_(VMFloat value) {
    return VMArgument{false, VMValue{.d = static_cast<VMDouble>(value)}};
}

template <typename T>
inline VMArgument BIBBLE_ARG_IMPL_(T value) {
    return VMArgument{false, VMValue{.l = static_cast<VMLong>(value)}};
}

inline VMReturnValue BIBBLE_RETURN_IMPL_(VMObject value) {
    return VMReturnValue{true, VMValue{.obj = value}};
}

inline VMReturnValue BIBBLE_RETURN_IMPL_(VMHandle value) {
    return VMReturnValue{false, VMValue{.h = value}};
}

inline VMReturnValue BIBBLE_RETURN_IMPL_(VMDouble value) {
    return VMReturnValue{false, VMValue{.d = value}};
}

inline VMReturnValue BIBBLE_RETURN_IMPL_(VMFloat value) {
    return VMReturnValue{false, VMValue{.d = static_cast<VMDouble>(value)}};
}

template <typename T>
inline VMReturnValue BIBBLE_RETURN_IMPL_(T value) {
    return VMReturnValue{false, VMValue{.l = static_cast<VMLong>(value)}};
}

#define BIBBLE_ARG(T, value) BIBBLE_ARG_IMPL_(static_cast<T>(value))
#define BIBBLE_RETURN(T, value) BIBBLE_RETURN_IMPL_(static_cast<T>(value))
#else
#define BIBBLE_ARG_OR_RETURN(T, R, value) _Generic((T){0}, \
VMObject: BIBBLE_CLITERAL(R){true, BIBBLE_CLITERAL(VMValue){.obj=(VMObject)(value)}}, \
VMHandle: BIBBLE_CLITERAL(R){false, BIBBLE_CLITERAL(VMValue){.h=(VMHandle)(value)}}, \
VMDouble: BIBBLE_CLITERAL(R){false, BIBBLE_CLITERAL(VMValue){.d=(VMDouble)(value)}}, \
VMFloat: BIBBLE_CLITERAL(R){false, BIBBLE_CLITERAL(VMValue){.d=(VMDouble)(value)}}, \
default: BIBBLE_CLITERAL(R){false, BIBBLE_CLITERAL(VMValue){.l=(VMLong)(value)}} \
)
#define BIBBLE_ARG(T, value) BIBBLE_ARG_OR_RETURN(T, VMArgument, value)
#define BIBBLE_RETURN(T, value) BIBBLE_ARG_OR_RETURN(T, VMReturnValue, value)
#endif

#endif //BIBBLEVM_BIBBLEINTERFACE_H