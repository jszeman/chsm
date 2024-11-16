import time
import msvcrt
import state_machine

ESC_KEY = '\x1b'
class UserClass:
    EVENT_SPACE = ' '
    EVENT_INIT  = 'i'

    def a_entry(self):
        print('A')

    def b_entry(self):
        print('B')

    def c_entry(self):
        print('C')

if __name__ == '__main__':

    usr = UserClass()
    sm = state_machine.MyStateMachine(usr)
    last_key = None

    while True:
        if msvcrt.kbhit():
            last_key = msvcrt.getch().decode('utf-8')
            
            if last_key == ESC_KEY:
                print("Esc pressed, exiting...")
                break

        sm.state_func(last_key)
        last_key = None
        time.sleep(0.1)
 