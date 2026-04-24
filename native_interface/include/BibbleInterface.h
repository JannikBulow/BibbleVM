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

typedef struct BibbleVM BibbleVM;

typedef struct BibbleInterface {
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

    VMValue (*CallFunction)(BibbleVM*, VMFunction, ...);
    VMValue (*CallFunctionV)(BibbleVM*, VMFunction, va_list);
    VMValue (*CallFunctionA)(BibbleVM*, VMFunction, const VMValue* args);
} BibbleInterface;

#endif //BIBBLEVM_BIBBLEINTERFACE_H