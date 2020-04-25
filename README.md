# chsm
Hierarchical State Machine Framework for C

## Dependencies

### Of the GUI and code generation
1. Chrome. Theoretically other browsers can be used, but only Chrome was tested.
2. Python 3.7+
   The following packages are required:
   * eel
   * docopt

   You can install the packages like this:
   `pip3 install eel docopt`

### Of the C unit tests
The C code uses the Unity unit testing framework for running the tests. The
CMake script that builds the tests assumes that the Unity and the CHSM repos
are in the same directory.

Cloning: `git clone https://github.com/ThrowTheSwitch/Unity.git`

## Usage
1. Clone the repo
2. Checkout the **master** branch
3. Navigate into the **chsm** folder, open a command prompt and run this command:
   **python3 cgen/chsm_backend.py**

The result should be a new window with a simple state machine already in it.

