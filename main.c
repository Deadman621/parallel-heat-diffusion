#include<stdio.h>
#include<heat.h>

int main(void) {

    heat_t experiment = {
        .x = 10,
        .y = 10,
        .K = 3,
        .h = 1,
        .dt = 0.1,
        .diffusivity = 1.0,
        .boundary = 0.0
    };

    create_initial_state(&experiment, NULL, NULL);
    simulate(experiment);

    terminate(&experiment);
    
    return 0;
}