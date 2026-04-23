// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_BIBBLEINTERFACE_H
#define BIBBLEVM_BIBBLEINTERFACE_H 1

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

typedef struct VMModule_* VMModule;
typedef struct VMClass_* VMClass;
typedef struct VMField_* VMField;
typedef struct VMMethod_* VMMethod;
typedef struct VMFunction_* VMFunction;

typedef enum VMTypeTag {
    VM_TYPE_PRIMITIVE,
    VM_TYPE_REFERENCE
} VMTypeTag;

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
    VMArray arr;
    VMString str;
    VMFuture fut;
} VMValue;

// Workaround for not having static types at runtime. Trusting the native authors a lot with this one...
typedef struct VMReturnValue {
    VMTypeTag tag;
    VMValue value;
} VMReturnValue;

typedef struct BibbleVM BibbleVM;

typedef struct BibbleInterface {
    void (*Test)(BibbleVM*);
} BibbleInterface;

#endif //BIBBLEVM_BIBBLEINTERFACE_H