import time
import msvcrt  # Only works on Windows

# This variable stores the last keystroke
last_keystroke = None

# The function to be called every second
def my_function(keystroke=None):
    if keystroke:
        print(f"Function called with keystroke: {keystroke}")
    else:
        print("Function called without keystroke")

# Main loop to call `my_function` every second
while True:
    # Check if a key was pressed
    if msvcrt.kbhit():
        # Get the key pressed and decode it
        key = msvcrt.getch().decode('utf-8')
        last_keystroke = key
        
        # Exit if 'Esc' key is pressed (ASCII code 27)
        if key == '\x1b':
            print("Esc pressed, exiting...")
            break

    # Call the function with the latest keystroke (if any)
    my_function(last_keystroke)
    last_keystroke = None  # Reset after each call
    time.sleep(1)
 