#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "can_if.h"
#include "canopen.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define CO_OD_MAX_RECURSION_LEVEL 4

od_entry_tst* find_od_entry_recursive(object_dictionary_tst* od_pst, uint32_t mlx_u32, uint8_t level_u8)
{
    od_entry_tst* od_entry_pst;

    if (level_u8 >= CO_OD_MAX_RECURSION_LEVEL) return NULL;

    for (od_entry_pst = od_pst->objects_ast;
         od_entry_pst->size_u32;
         od_entry_pst++)
    {
        if (mlx_u32 == od_entry_pst->mlx_u32)
        {
            return od_entry_pst;
        }
        else if (od_entry_pst->flags_u16 & OD_ATTR_OBJECT_DICT)
        {
            od_entry_tst* o_pst;

            o_pst = find_od_entry_recursive((object_dictionary_tst* )(od_entry_pst->addr_u), mlx_u32, level_u8 + 1);

            if (NULL != o_pst)
            {
                return o_pst;
            }
        }
    }

    return NULL;
}

od_entry_tst* find_od_entry(sdo_tst* self, uint32_t mlx_u32)
{
    return find_od_entry_recursive(self->config_st.od_pst, mlx_u32, 0);
}