import os
import re
import eel
from pathlib import Path

class DirGen:
    def __init__(self):
        pass
    
    chsm_header_template = """ 
#ifndef {name_upper}_H_
#define {name_upper}_H_

#include "crf.h"
#include "signals.h"
#include "sys_if.h"

/*
{name_upper} module
==========

{version}

{description}

Requirements for the module:
*/

/*
 * {name_upper} SIGNALS
 */

typedef enum {name}_signals_ten
{{
    SIG_{name_upper} = SIGNAL_FROM_CLASS(SIG_CLASS_{name}),
    SIG_{name_upper}_ONLINE,
    SIG_{name_upper}_OFFLINE,
}} {name}_signals_ten;

#define SIG_{name_upper}_ONLINE_TYPE             {name}_status_tst
#define SIG_{name_upper}_OFFLINE_TYPE            {name}_status_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct {name}_status_tst
{{
}} {name}_status_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct {name}_cfg_tst
{{
}} {name}_cfg_tst;

typedef struct
{{
    /* PUBLIC */
    chsm_tst                             super;

    /* PRIVATE */
}}{name}_tst;

chsm_result_ten {name}_top(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

bool {name}_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
bool {name}_error_count(chsm_tst *self, const cevent_tst *e_pst, uint16_t error_cnt_threshold_u16);

#define {name_upper}_READ_PERIOD_VALUE     100

#define {name_upper}_RETRY_TIMEOUT   500
#define {name_upper}_MAX_ERROR_COUNT (((bodycontour_tst *)self)->config_st.max_error_cnt_u16)

#endif /* {name_upper}_H_ */
"""

    chsm_functions_template = """
#include "{name}.h"
#include "{name}_functions.h"
#include <stdio.h>    
    """
    chsm_test_signal_classes = """
#ifndef SIGNAL_CLASSES_H
#define SIGNAL_CLASSES_H

#include "crf.h"

typedef enum sig_class_ten {{
    SIG_CLASS_SYS = CRF_SIGNAL_CLASS_START,
    SIG_CLASS_{name_upper},
}} sig_class_ten;

#endif
    """
    chsm_test_main = """
#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{{

}}

void enableInterrupts(void)
{{

}}

void run_{name}_tests(void)
{{
	RUN_TEST_GROUP({name});
}}

int main(int argc, const char * argv[])
{{
	return UnityMain(argc, argv, run_{name}_tests);
}}
    """
    chsm_unittests = """
    /*
 * ut_{name}_test.c
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "unity_fixture.h"
#include "crf.h"
#include "cbits.h"
#include "{name}.h"
#include "cevent.h"
#include "sys_if.h"

TEST_GROUP({name});

const cevent_tst*		{name}_events_apst[8];
{name}_tst				{name}_st;


{name}_tst *self = 		&{name}_st; 

chsm_tst*				hsm_apst[] = {{
	(chsm_tst*)(&{name}_st),
	NULL}};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;

const cevent_tst		tick_1ms_st = {{.sig = SIG_SYS_TICK_1ms}};

static void tick_ms(uint32_t tick_cnt_u32)
{{
	while(tick_cnt_u32--)
	{{
	    // drv_mock_st.tick(&drv_mock_st);

		CRF_POST(&tick_1ms_st, &{name}_st);

		while(CRF_STEP())
		{{
			printf("|");
		}}
	}}
}}

void {name}_send(chsm_tst *self, const cevent_tst *e_pst)
{{
	switch(e_pst->sig)
	{{
		case SIG_{name_upper}_ONLINE:
		case SIG_{name_upper}_OFFLINE:
			CRF_POST(e_pst, &q_st);
			break;

		default:
	}}
}}


TEST_SETUP({name})
{{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&crf, 0, sizeof crf);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);
    memset(&{name}_st, 0, sizeof {name}_st);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);
    cqueue_init(&q_st, events_apst, 4);
	chsm_ctor(&{name}_st.super, {name}_top, {name}_events_apst, 8, 0);
	

	CRF_SEND_FUNC(&{name}_st) = {name}_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&{name}_st);
}}

TEST_TEAR_DOWN({name})
{{
}}

/*
 * Just call setup
 */
TEST({name}, init)
{{
}}

TEST_GROUP_RUNNER({name})
{{
	RUN_TEST_CASE({name}, init);
}}
    
    """
    
    
    def getLocalDir(self):
        print(os.getcwd())
        dir_name = os.path.dirname(os.path.realpath(__file__))
        print(dir_name)
        return dir_name
    
    def join(self,path, *paths):
        return os.path.join(path, *paths)
    
    def createFolder(self,directory):
        try:
            if not os.path.exists(directory):
                os.makedirs(directory)
        except OSError:
            print ('Error: Creating directory. ' +  directory)
    
    def createFile(self, file_loc, file_name, content):
        file_dir = os.path.join(file_loc, file_name)
        try:
            if not os.path.exists(file_dir):
                fd_name = open(file_dir, "a+")
                fd_name.write(content)
                fd_name.close()  
        except OSError:
            pass
    
    def generate_modul_dirs(self, module_path, module_name, version, description):
        print(module_path)
        print(type(module_path))
        print(module_name)
        print(type(module_name))
        module_dir = os.path.join(str(module_path), str(module_name))
        print(module_dir)
        print(type(module_dir))
        try:
            if not os.path.exists(module_dir):
                self.createFolder(module_dir)
                self.createFolder(os.path.join(module_dir, "src"))
                self.createFolder(os.path.join(module_dir, "doc"))
                self.createFolder(os.path.join(module_dir, "inc"))
                self.createFolder(os.path.join(module_dir, "test/tinc"))
                self.createFolder(os.path.join(module_dir, "test/tsrc"))
                self.createFile(os.path.join(module_dir, "inc"), 
                                (str(module_name) + ".h"), 
                                self.chsm_header_template.format(name=str(module_name), name_upper = str(module_name).upper(), description=description, version=version))
                self.createFile(os.path.join(module_dir, "src"),
                                (str(module_name) + "_functions" + ".c"),
                                self.chsm_functions_template.format(name=module_name, name_upper=module_name.upper()))
                self.createFile(os.path.join(module_dir, "test/tinc"),
                                ("signal_classes" + ".h"),
                                self.chsm_test_signal_classes.format(name_upper=module_name.upper()))
                self.createFile(os.path.join(module_dir, "test/tsrc"),
                                ("ut_" + str(module_name) + "_test" + ".c"),
                                self.chsm_unittests.format(name=module_name, name_upper=module_name.upper()))
                self.createFile(os.path.join(module_dir, "test/tsrc"),
                                ("main" + ".c"),
                                self.chsm_test_main.format(name=module_name, name_upper=module_name.upper()))
                                
        except OSError:
            pass
