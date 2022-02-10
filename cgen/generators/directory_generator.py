import os
import re
import eel

class DirGen:
    def __init__(self):
        pass
    
    chsm_header_template = """ 
#ifndef {name}_H_
#define {name}_H_

#include "crf.h"
#include "signals.h"
#include "sys_if.h"

/*
{name} module
==========

This module implements a high level statemachine.

Requirements for the module:
*/

/*
 * {name} SIGNALS
 */

typedef enum {name}_signals_ten
{
    SIG_{name} = SIGNAL_FROM_CLASS(SIG_CLASS_{name}),
    SIG_{name}_ONLINE,
    SIG_{name}_OFFLINE,
} {name}_signals_ten;

#define SIG_{name}_ONLINE_TYPE             {name}_status_tst
#define SIG_{name}_OFFLINE_TYPE            {name}_status_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct {name}_status_tst
{
} {name}_status_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct {name}_cfg_tst
{
} {name}_cfg_tst;

typedef struct
{
    /* PUBLIC */
    chsm_tst                             super;

    /* PRIVATE */
} {name}_tst;

chsm_result_ten {name}_top(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

bool {name}_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
bool {name}_error_count(chsm_tst *self, const cevent_tst *e_pst, uint16_t error_cnt_threshold_u16);

#define {name}_READ_PERIOD_VALUE     100

#define {name}_RETRY_TIMEOUT   500
#define {name}_MAX_ERROR_COUNT (((bodycontour_tst *)self)->config_st.max_error_cnt_u16)

#endif /* {name}_H_ */
"""

    
    chsm_test_main = ""
    chsm_test_signal_classes = ""
    chsm_unittests = ""
    
    
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
        file_dir = (file_loc + "/" + file_name)
        try:
            if not os.path.exists(file_dir):
                fd_name = open(file_dir, "a+")
                fd_name.write(content)
                fd_name.close()  
        except OSError:
            pass
    
    def createCMakeListsFile(self, file_loc, module_name):
        pass
        
    def makeBaseModul(self, module_path, module_name):
        module_dir = (module_path + "/" + module_name)
        self.createFolder(module_dir)
        self.createFolder((module_dir + "/src"))
        self.createFolder((module_dir + "/doc"))
        self.createFolder((module_dir + "/inc"))
        self.createFolder((module_dir + "/test/tinc"))
        self.createFolder((module_dir + "/test/tsrc"))
        
        self.createFile(module_dir, module_name, )
        
            
        
        

# dg = DirGen()
# dg.greeting()
# dg.makeBaseModulDirs(dg.getLocalDir(), "kecske")
# dg.createFiles(dg.getLocalDir(), "kecske.c", chsm_header_template)