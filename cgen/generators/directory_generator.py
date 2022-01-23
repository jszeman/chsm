import os
import re
import eel

class DirGen:
    def __init__(self):
        pass
    
    def greeting(self):
        print(" Hello!")
    
    def getLocalDir(self):
        print(os.getcwd())
        dir_name = os.path.dirname(os.path.realpath(__file__))
        print(dir_name)
        return dir_name
    
    def join(path, *paths):
        return os.path.join(path, *paths)
    
    def makeBaseModulDirs(self, module_path):
        dir_test = self.join(module_path, "test")
        os.makedirs(dir_test)

# dg = DirGen()
# dg.greeting()
# dg.makeBaseModulDirs(dg.getLocalDir())