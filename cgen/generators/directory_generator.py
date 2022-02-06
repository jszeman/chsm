import os
import re
import eel

class DirGen:
    def __init__(self):
        pass
    
    chsm_header_template = str("
                                    #ifndef LM73_H
                                    #define LM73_H

                                    #include "i2c_master.h"
                                    #include "i2c_driver_if.h"
                                    #include "crf.h"
                                    #include "signals.h"
                                    #include "sys_if.h"

                                    /*
                                    LM73 driver
                                    ==========

                                    This module implements a high level driver for the LM73 temperature sensor.

                                    Requirements for the LM73 module:
                                        * Read out the ID register to test the communication
                                        * Retry the ID register reading after a timeout if the previous read failed
                                        * Send an online event, when the link is established
                                        * Send an offline event if the temperature read operation fails for a number of times
                                        * Send periodic temperature events
                                        * Allow triggered temperature reads
                                        * Add a user defined ID into the events (so the application can differentiate between
                                        the different LM73 events)
                                    */

                                    /*
                                    * LM73 SIGNALS
                                    */

                                    typedef enum lm73_signals_ten
                                    {
                                        SIG_LM73_TEMP = SIGNAL_FROM_CLASS(SIG_CLASS_LM73),
                                        SIG_LM73_ONLINE,
                                        SIG_LM73_OFFLINE,
                                        SIG_LM73_READ
                                    } lm73_signals_ten;

                                    #define SIG_LM73_TEMP_TYPE      lm73_temp_tst
                                    #define SIG_LM73_ONLINE_TYPE    lm73_status_tst
                                    #define SIG_LM73_OFFLINE_TYPE   lm73_status_tst
                                    #define SIG_LM73_READ_TYPE      cevent_tst


                                    /*
                                    * EVENT DEFINITIONS
                                    */

                                    typedef struct lm73_temp_tst
                                    {
                                        cevent_tst          super;              // Signal and GC stuff
                                        int16_t             temp_C_i16;         // Temperature
                                        uint16_t            id_u16;             // Sensor ID
                                    } lm73_temp_tst;

                                    typedef struct lm73_status_tst
                                    {
                                        cevent_tst          super;              // Signal and GC stuff
                                        uint16_t            id_u16;             // Sensor ID
                                    } lm73_status_tst;

                                    /*
                                    * ACTIVE OBJECT
                                    */

                                    typedef struct lm73_cfg_tst
                                    {
                                        uint16_t            id_u16;             // This ID will be inserted into temperature events
                                        uint16_t            period_ms_u16;      // Temperature read period in ms
                                        uint16_t            max_error_cnt_u16;  // Number of times the module is allowed to fail a read operation before going offline
                                        uint8_t             address_u8;         // I2C slave address of the sensor
                                    } lm73_cfg_tst;

                                    typedef struct lm73_tst lm73_tst;

                                    struct lm73_tst
                                    {
                                        /* PUBLIC */
                                        chsm_tst            super;
                                        lm73_cfg_tst        config_st;
                                        int16_t             temp_C_i16;
                                        bool                valid_b;

                                        /* PRIVATE */
                                        uint32_t            counter_u32;
                                        uint16_t            error_counter_u32;

                                        i2c_transaction_tst t_st;
                                        uint8_t             tx_buff_au8[4];
                                        uint8_t             rx_buff_au8[4];
                                    };

                                    chsm_result_ten lm73_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

                                    #endif
    ")

    
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
        
        
    def makeBaseModul(self, module_path, module_name):
        module_dir = (module_path + "/" + module_name)
        self.createFolder(module_dir)
        self.createFolder((module_dir + "/src"))
        self.createFolder((module_dir + "/doc"))
        self.createFolder((module_dir + "/inc"))
        self.createFolder((module_dir + "/test/tinc"))
        self.createFolder((module_dir + "/test/tsrc"))
        
        self.createFile(module_dir, module_name, )
        
            
        
        

dg = DirGen()
# dg.greeting()
# dg.makeBaseModulDirs(dg.getLocalDir(), "kecske")
dg.createFiles(dg.getLocalDir(), "kecske.c", chsm_header_template)