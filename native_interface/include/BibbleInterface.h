// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_BIBBLEINTERFACE_H
#define BIBBLEVM_BIBBLEINTERFACE_H 1

#include <stdint.h>

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

typedef struct BibbleVM BibbleVM;

typedef struct BibbleInterface {
    void (*Test)(BibbleVM*);
} BibbleInterface;

#endif //BIBBLEVM_BIBBLEINTERFACE_H