import time
import msvcrt
import state_machine

ESC_KEY = '\x1b'
class UserClass:
    EVENT_SPACE = ' '
    EVENT_INIT  = 'i'
    EVENT_ENTER = '\r'

    def __init__(self):
        self.space_ts = time.time()
        self.timeout_ts = 0

    def double_space(self, threshold):
        ts = time.time()
        if (ts - self.space_ts) < threshold:
            return True
        
        self.space_ts = ts
        return False

    def print(self, s):
        print(s)

    def reset_timer(self):
        self.timeout_ts = time.time()

    def timeout(self, t):
        if (time.time() - self.timeout_ts) > t:
            return True
        
        return False

if __name__ == '__main__':

    usr = UserClass()
    sm = state_machine.MyStateMachine(usr)
    last_key = None

    while True:
        if msvcrt.kbhit():
            last_key = msvcrt.getch().decode('utf-8')
            #print(last_key.encode('utf-8'))
            
            if last_key == ESC_KEY:
                print("Esc pressed, exiting...")
                break

        sm.state_func(last_key)
        last_key = None
        time.sleep(0.1)
 