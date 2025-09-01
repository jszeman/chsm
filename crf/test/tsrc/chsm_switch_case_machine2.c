#include "chsm_switch_case_machine2.h"
#include "chsm_simple_machine_functions.h"

swcm_state_ten switch_case_machine(swcm_state_ten state_en, swcm_event_ten event_en)
{
    switch(state_en)
    {
        case state_top:
            switch(event_en)
            {
                case SWCM_INIT:
                    a_entry(self, e_u32);
                    return state_a;
            }
        case state_a:
            switch(event_en)
            {
                
            case SMPL_SIG_EVENT_A:
                a_func(self, e_u32);
                break;

            case SMPL_SIG_EVENT_H:
                if (guard_h(self, e_u32, 2))
                {
                    func_h(self, e_u32, 4, 5);
                    a_exit(self, e_u32);
                    c_entry(self, e_u32);
                    b_entry(self, e_u32);
                    return SIMPLE_SM_SET_STATE(self, state_b);
                }
                break;
        }

        if (guard_1(self, e_u32))
        {
            a_exit(self, e_u32);
            c_entry(self, e_u32);
            b_entry(self, e_u32);
            return SIMPLE_SM_SET_STATE(self, state_b);
        }
        }
        case state_b:
            switch(event_en)
            {
                
            case SMPL_SIG_EVENT_B:
                b_func(self, e_u32);
                break;

            case SMPL_SIG_EVENT_F:
                b_to_a(self, e_u32);
                b_exit(self, e_u32);
                c_exit(self, e_u32);
                a_entry(self, e_u32);
                return SIMPLE_SM_SET_STATE(self, state_a);

            case SMPL_SIG_EVENT_K:
                func_k(self, e_u32);
                b_exit(self, e_u32);
                c_exit(self, e_u32);
                a_entry(self, e_u32);
                return SIMPLE_SM_SET_STATE(self, state_a);
        }
        }
    }
}