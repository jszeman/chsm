#ifndef OD_H
#define OD_H

#include "mem_driver_if.h"
#include "limits.h"

typedef enum {
    OD_ATTR_READ =          0x01,   // Readable
    OD_ATTR_WRITE =         0x02,   // Writable
    OD_ATTR_STORE =         0x04,   // On store command, the object need to be stored in non volatile memory
    OD_ATTR_PERSISTENT =    0x08,   // On store command, the object need to be stored in non volatile memory that is not affected by FW update
    OD_ATTR_MAPPABLE =      0x10,   // Can be mapped in PDO
    OD_ATTR_OBJECT_DICT =   0x20,   // The object pointer in this struct points to another object dictionary
    OD_ATTR_32BIT_SIZE =    0x40,   // The object size is larger than 64k so two consecutive entry will be used to store the size info on 32 bits
} od_entry_flags_ten;

typedef struct od_entry_tst
{
    uintptr_t       addr_u;         // Object address
    uint32_t        mlx_u32;        // Index + subindex
    uint32_t        size_u32;       // Object size in bytes
    uint16_t        flags_u16;      // Object attributes
} od_entry_tst;

typedef struct object_dictionary_tst
{
    mem_driver_if_tst*  mem_drv_pst;    // Pointer to the memory driver interface that can access the objects in this dictionary
    uint32_t            mlx_mask_u32;   // For all objects in this list the following must be true: mlx_u32 == mlx_u32 & mlx_mask_u32
    od_entry_tst*       objects_ast;    // Pointer to the object list
} object_dictionary_tst;

#define OD_ENTRY_TERMINATOR {.mlx_u32 = 0, .addr_u = 0, .size_u32 = 0, .flags_u16 = 0}

#define OD_ENTRY_DEF(MLX, OBJ, FLAGS) {         \
    .mlx_u32    = MLX,                          \
    .addr_u     = (uintptr_t)(&OBJ),            \
    .size_u32   = sizeof(OBJ)*(CHAR_BIT/8),     \
    .flags_u16  = FLAGS                         \
    }

#define OD_EXTENSION(OD) {                      \
    .mlx_u32    = 0,                            \
    .addr_u     = (uintptr_t)(&OD),             \
    .size_u32   = 1,                            \
    .flags_u16  = OD_ATTR_OBJECT_DICT           \
    }


#endif
