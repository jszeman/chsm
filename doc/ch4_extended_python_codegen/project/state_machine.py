# Generated code - any edits will be overwritten.

class MyStateMachineInterface:
    EVENT_INIT  = None # Init event
    EVENT_SPACE = None # This event is generated when the user presses the spacebar.
    EVENT_ENTER = None # This event is generated when the user presses the enter button.

    def reset_timer(self):
        """Save the actual time in an internal variable."""
        pass



    def print(self, *args, **kwargs):
        """Just print the string parameter in the console."""
        pass

    def save_history(self, *args, **kwargs):
        """Save the actual state into an internal variable."""
        pass


    def double_space(self, *args, **kwargs):
        """Return ture, if the spacebar was pressed twice within the time window given as parameter."""
        return False

    def timeout(self, *args, **kwargs):
        """Return true, if the elapsed time since the last reset_timer call is greater than the time given in the argument."""
        return False


    def history(self):
        """Return a state method."""
        return None


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
            self.user_obj.save_history(self.state_func)
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_B(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.reset_timer()
            if self.user_obj.double_space(0.5):
                self.user_obj.print("C")
                self.state_func = self.state_C

        if self.user_obj.timeout(3):
            self.user_obj.save_history(self.state_func)
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_C(self, event):
        if event == self.user_obj.EVENT_SPACE:
            self.user_obj.reset_timer()
            self.user_obj.print("A")
            self.state_func = self.state_A

        if self.user_obj.timeout(3):
            self.user_obj.save_history(self.state_func)
            self.user_obj.print("Timeout")
            self.state_func = self.state_E

    def state_E(self, event):
        if event == self.user_obj.EVENT_ENTER:
            self.user_obj.print("D")
            self.user_obj.reset_timer()
            self.state_func = self.user_obj.history(event)


