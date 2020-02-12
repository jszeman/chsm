'''
Created on 2019. febr. 19.

@author: jszeman
'''
import unittest
import logging
# from . import plantuml_reader as pr
from cgen import plantuml_reader as pr

class TestObject:
    def event_handler(self, event_id, **cargo):
        print(event_id, cargo)
        
    def __getattr__(self, name):
        print(name, end=' ')
        return self.event_handler

class CgenTest(unittest.TestCase):
    uml = '''
@startuml
state s {
        s: entry/ s_entry()
        s: exit/ s_exit()
        s: init/ s_init()
        s: I/ s_i()
        
        state s1 {
            s1: entry/ s1_entry()
            s1: exit/ s1_exit()
            s1: init/ s1_init()
            s1: I/ s1_i()
            
            state s11 {
                s11: entry/ s11_entry()
                s11: exit/ s11_exit()
                s11: init/ s11_init()
                s11: I/ s11_i()
            }
            
            [*] --> s11
        }
    
        state s2 {
            s2: entry/ s2_entry()
            s2: exit/ s2_exit()
            s2: init/ s2_init()
            s2: I/ s2_i()
            
            state s21 {
                s21: entry/ s21_entry()
                s21: exit/ s21_exit()
                s21: init/ s21_init()
                s21: I/ s21_i()
                
                state s211 {
                    s211: entry/ s211_entry()
                    s211: exit/ s211_exit()
                    s211: init/ s211_init()
                    s211: I/ s211_i()
                }
                
                [*] --> s211
            }
            [*] --> s211
        }
    
      [*] --> s11
    }
    
    [*] --> s2
    s1 --> s: D
    s --> s11: E
    s1 --> s11: B
    s1 --> s1: A
    s11 --> s: H
    s1 --> s2: C
    s2 --> s1: C
    s11 --> s211: G
    s21 --> s11: G
    s2 --> s11: F
    s1 --> s211: F
    s21 --> s21: A
    s21 --> s211: B
    s211 --> s21: D
    s211 -> s: H
@enduml
    '''
    
    functions = {   
        ('__top__', 'entry'): (),
        ('__top__', 'exit'): (),
        ('__top__', 'init'): (   's_entry',
                                 's2_entry',
                                 's2_init',
                                 's21_entry',
                                 's211_entry'),
        ('s', 'E'): ('s1_entry', 's11_entry'),
        ('s', 'I'): ('s_i',),
        ('s', 'entry'): ('s_entry',),
        ('s', 'exit'): ('s_exit',),
        ('s', 'init'): ('s_init', 's1_entry', 's11_entry'),
        ('s1', 'A'): ('s1_exit', 's1_entry', 's1_init', 's11_entry'),
        ('s1', 'B'): ('s11_entry',),
        ('s1', 'C'): (   's1_exit',
                         's2_entry',
                         's2_init',
                         's21_entry',
                         's211_entry'),
        ('s1', 'D'): ('s1_exit', 's_init', 's1_entry', 's11_entry'),
        ('s1', 'F'): ('s1_exit', 's2_entry', 's21_entry', 's211_entry'),
        ('s1', 'I'): ('s1_i',),
        ('s1', 'entry'): ('s1_entry',),
        ('s1', 'exit'): ('s1_exit',),
        ('s1', 'init'): ('s1_init', 's11_entry'),
        ('s11', 'G'): (   's11_exit',
                          's1_exit',
                          's2_entry',
                          's21_entry',
                          's211_entry'),
        ('s11', 'H'): (   's11_exit',
                          's1_exit',
                          's_init',
                          's1_entry',
                          's11_entry'),
        ('s11', 'I'): ('s11_i',),
        ('s11', 'entry'): ('s11_entry',),
        ('s11', 'exit'): ('s11_exit',),
        ('s11', 'init'): ('s11_init',),
        ('s2', 'C'): ('s2_exit', 's1_entry', 's1_init', 's11_entry'),
        ('s2', 'F'): ('s2_exit', 's1_entry', 's11_entry'),
        ('s2', 'I'): ('s2_i',),
        ('s2', 'entry'): ('s2_entry',),
        ('s2', 'exit'): ('s2_exit',),
        ('s2', 'init'): ('s2_init', 's21_entry', 's211_entry'),
        ('s21', 'A'): ('s21_exit', 's21_entry', 's21_init', 's211_entry'),
        ('s21', 'B'): ('s211_entry',),
        ('s21', 'G'): ('s21_exit', 's2_exit', 's1_entry', 's11_entry'),
        ('s21', 'I'): ('s21_i',),
        ('s21', 'entry'): ('s21_entry',),
        ('s21', 'exit'): ('s21_exit',),
        ('s21', 'init'): ('s21_init', 's211_entry'),
        ('s211', 'D'): ('s211_exit', 's21_init', 's211_entry'),
        ('s211', 'H'): (   's211_exit',
                           's21_exit',
                           's2_exit',
                           's_init',
                           's1_entry',
                           's11_entry'),
        ('s211', 'I'): ('s211_i',),
        ('s211', 'entry'): ('s211_entry',),
        ('s211', 'exit'): ('s211_exit',),
        ('s211', 'init'): ('s211_init',)}
    
    transitions = {
        ('__top__', 'init'): 's211',
        ('s', 'E'): 's11',
        ('s', 'init'): 's11',
        ('s1', 'A'): 's11',
        ('s1', 'B'): 's11',
        ('s1', 'C'): 's211',
        ('s1', 'D'): 's11',
        ('s1', 'F'): 's211',
        ('s1', 'init'): 's11',
        ('s11', 'G'): 's211',
        ('s11', 'H'): 's11',
        ('s2', 'C'): 's11',
        ('s2', 'F'): 's11',
        ('s2', 'init'): 's211',
        ('s21', 'A'): 's211',
        ('s21', 'B'): 's211',
        ('s21', 'G'): 's11',
        ('s21', 'init'): 's211',
        ('s211', 'D'): 's211',
        ('s211', 'H'): 's11'}

    def setUp(self):
        to = TestObject()
        self.sm = pr.UmlFlatMachine(to, uml_str=self.uml)


    def tearDown(self):
        pass

    def test_functions(self):
        for k, v in self.functions.items():
            self.assertEqual(v, self.sm.functions[k])


    def test_transitions(self):
        for k, v in self.transitions.items():
            self.assertEqual(v, self.sm.transitions[k])


if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()