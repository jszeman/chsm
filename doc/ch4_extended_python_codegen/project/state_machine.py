# Generated code - any edits will be overwritten.

class MyStateMachine:
    def __init__(self, user_obj):
        self.user_obj = user_obj
        self.state_func = self.state_top

    def state_top(self, event):
        if event == self.user_obj.EVENT_INIT:
            self.user_obj.print("D")
            self.user_obj.reset_timer()
            self.user_obj.print("A")
            self.state_func = self.state_A

    def state_A(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.reset_timer()
            self.user_obj.print("B")
            self.state_func = self.state_B

        if self.user_obj.timeout(3):
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_B(self, event):
        if event == self.user_obj.EVENT_SPACE:
            if self.user_obj.double_space(0.5):
                self.user_obj.reset_timer()
                self.user_obj.print("C")
                self.state_func = self.state_C

        if self.user_obj.timeout(3):
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_C(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.reset_timer()
            self.user_obj.print("A")
            self.state_func = self.state_A

        if self.user_obj.timeout(3):
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_E(self, event):
        if event == self.user_obj.EVENT_ENTER:
            self.user_obj.print("D")
            self.user_obj.reset_timer()
            self.user_obj.print("A")
            self.state_func = self.state_A


