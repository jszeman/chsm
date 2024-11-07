# Generated code - any edits will be overwritten.

class StateMachine:
    def __init__(self, user_obj):
        self.user_obj = user_obj
        self.state_func = self.state_top

    def state_top(self, event):
        if event == self.user_obj.EVENT_INIT:
            self.user_obj.a_entry()
            self.state_func = self.state_A

    def state_A(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.b_entry()
            self.state_func = self.state_B

    def state_B(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.c_entry()
            self.state_func = self.state_C

    def state_C(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.a_entry()
            self.state_func = self.state_A