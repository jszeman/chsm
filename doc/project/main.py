import time
import msvcrt
from state_machine import StateMachine

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
'''
class StateMachine:
    def __init__(self, usr_obj):
        self.usr_obj = usr_obj
        self.state_func = self.state_top

    def state_top(self, event):
        if event == self.usr_obj.EVENT_INIT:
            self.usr_obj.a_entry()
            self.state_func = self.state_A

    def state_A(self, event):
        if event == self.usr_obj.EVENT_SPACE:
            self.usr_obj.b_entry()
            self.state_func = self.state_B

    def state_B(self, event):
        if event == self.usr_obj.EVENT_SPACE:
            self.usr_obj.c_entry()
            self.state_func = self.state_C

    def state_C(self, event):
        if event == self.usr_obj.EVENT_SPACE:
            self.usr_obj.a_entry()
            self.state_func = self.state_A
'''

if __name__ == '__main__':

    usr = UserClass()
    sm = StateMachine(usr)
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
 