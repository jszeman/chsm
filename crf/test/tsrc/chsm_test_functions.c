/*
 * chsm_test_functions.c
 *
 *  Created on: 2019. febr. 4.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "chsm.h"
#include "chsm_test.h"

extern char log_str[1024];

/* CHSM: test
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
 */
