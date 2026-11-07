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
    
    if (HEAT_ERROR_CODE) {
        printf("Invalid Arguments Passed");
        return -1;
    }

    if (GRID_ERROR_CODE) {
        char msg[100];
        grid_error(msg, 100, GRID_ERROR_CODE);
        printf("%s", msg);
        return -1;
    }
    
    #pragma omp parallel shared(GRID_ERROR_CODE, HEAT_ERROR_CODE)
    {
        simulate_parallel(experiment);
    }

    if (GRID_ERROR_CODE) {
        char msg[100];
        grid_error(msg, 100, GRID_ERROR_CODE);
        printf("%s", msg);
        terminate(&experiment);
        return -1;
    }

    terminate(&experiment);
    
    return 0;
}