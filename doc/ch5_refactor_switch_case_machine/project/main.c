#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

/* State and event types (generated code - do not edit) */
typedef enum state_ten
{
    STATE_START,
    STATE_A,
    STATE_B,
    STATE_C,
    STATE_D,
} state_ten;

typedef enum event_ten
{
    EVENT_ENTER, 
    EVENT_INIT, 
    EVENT_SPACE, 
    EVENT_TICK, 
    EVENT_EXIT
} event_ten;
/* End of generated code */

typedef struct data_tst
{
    state_ten   state_en;
    state_ten   history_en;
    bool        exit_b;
    uint32_t    counter_u32;
    clock_t     last_space_st;
} data_tst;

bool double_space(data_tst* self, double timeout_d)
{
    clock_t t_st;

    t_st = clock();

    double elapsedSeconds = (double)(t_st - self->last_space_st) / CLOCKS_PER_SEC;
    self->last_space_st = t_st;
        
    if (elapsedSeconds < timeout_d)
    {
        return true;
    }

    return false;
}

void reset_counter(data_tst* self)
{
    self->counter_u32 = 0;
}

void counter_inc(data_tst* self)
{
    self->counter_u32++;
}

void save_last_state(data_tst* self)
{
    self->history_en = self->state_en;
}

void recall_last_state(data_tst* self)
{
    return self->history_en;
}

bool after(data_tst* self, uint32_t t)
{
    return self->counter_u32 > t;
}

/* State machine (generated code - do not edit) */
void state_machine(data_tst* self, event_ten event_en)
{
    switch(self->state_en)
    {
        case STATE_START:
            switch(event_en)
            {
                case EVENT_INIT:
                    printf("A\n");
                    reset_counter(self);
                    self->state_en = STATE_A;
                    break;
            }
            break;


        case STATE_A:
            switch(event_en)
            {
                case EVENT_SPACE:
                    printf("B\n");
                    reset_counter(self);
                    self->state_en = STATE_B;
                    break;

                case EVENT_TICK:
                    counter_inc(self);
                    break;
            }
            break;


        case STATE_B:
            switch(event_en)
            {
                case EVENT_SPACE:
                    reset_counter(self);
                    break;
                    if (double_space(self, 0.5))
                    {
                        printf("C\n");
                        reset_counter(self);
                        self->state_en = STATE_C;
                    }
                    break;

                case EVENT_TICK:
                    counter_inc(self);
                    break;
            }
            break;


        case STATE_C:
            switch(event_en)
            {
                case EVENT_SPACE:
                    printf("A\n");
                    reset_counter(self);
                    self->state_en = STATE_A;
                    break;

                case EVENT_TICK:
                    counter_inc(self);
                    break;
            }
            break;


        case STATE_D:
            switch(event_en)
            {
                case EVENT_ENTER:
                    printf("Resume\n");
                    self->state_en = recall_last_state(self);
                    break;
            }
            break;


    }

    if (EVENT_EXIT == event_en)
    {
        self->exit_b = true;
    }
}
/* End of generated code */

void state_machine(data_tst* self, event_ten event_en)
{
    switch(self->state_en)
    {
        case STATE_START:
            switch (event_en)
            {
                case EVENT_INIT:
                    self->state_en = STATE_A;
                    printf("A\n");
                    break;
            }
            break;

        case STATE_A:
            switch (event_en)
            {
                case EVENT_SPACE:
                    self->state_en = STATE_B;
                    printf("B\n");
                    self->counter_u32 = 0;
                    break;

                case EVENT_TICK:
                    self->counter_u32++;
                    break;
            }

            if (self->counter_u32 > 3)
            {
                self->history_en = self->state_en;
                self->state_en = STATE_D;
                printf("Timeout\n");
            }
            break;

        case STATE_B:
            switch (event_en)
            {
                case EVENT_SPACE:
                    self->counter_u32 = 0;

                    if (double_space(self, 0.5))
                    {
                        self->state_en = STATE_C;
                        printf("C\n");
                    }
                    break;

                case EVENT_TICK:
                    self->counter_u32++;
                    break;
            }

            if (self->counter_u32 > 3)
            {
                self->history_en = self->state_en;
                self->state_en = STATE_D;
                printf("Timeout\n");
            }
            break;

        case STATE_C:
            switch (event_en)
            {
                case EVENT_SPACE:
                    self->state_en = STATE_A;
                    printf("A\n");
                    self->counter_u32 = 0;
                    break;

                case EVENT_TICK:
                    self->counter_u32++;
                    break;
            }

            if (self->counter_u32 > 3)
            {
                self->history_en = self->state_en;
                self->state_en = STATE_D;
                printf("Timeout\n");
            }
            break;

        case STATE_D:
            switch (event_en)
            {
                case EVENT_ENTER:
                    self->state_en = self->history_en;
                    printf("Resume\n");
                    self->counter_u32 = 0;
                    break;
            }
            break;
    }

    if (EVENT_EXIT == event_en)
    {
        self->exit_b = true;
    }
}

#define ESC_KEY 27
#define ENTER_KEY 13
#define SPACE_KEY 32
#define INIT_KEY 'i'

int main() {
    data_tst data_st = {
        .exit_b = false,
        .state_en = STATE_START,
    };

    int key;
    clock_t lastTime = clock();
    
    
    printf("Press i to start the state machine\n");
    printf("Press spacebar to cycle through A, B, C\n");
    printf("Press ESC to quit\n");
    
    while (1) {
        // Check for elapsed time
        clock_t currentTime = clock();
        double elapsedSeconds = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
        
        if (elapsedSeconds >= 1.0) {
            lastTime = currentTime;  // Reset timer
            state_machine(&data_st, EVENT_TICK);
        }
        
        if (_kbhit()) {
            key = _getch();

            switch (key)
            {
                case ESC_KEY:
                    state_machine(&data_st, EVENT_EXIT);
                    break;

                case SPACE_KEY:
                    state_machine(&data_st, EVENT_SPACE);
                    break;

                case INIT_KEY:
                    state_machine(&data_st, EVENT_INIT);
                    break;

                case ENTER_KEY:
                    state_machine(&data_st, EVENT_ENTER);
                    break;
            }
        }

        if (data_st.exit_b) break;
        
        Sleep(10);  // Small delay to prevent CPU hogging
    }
    
    return 0;
}