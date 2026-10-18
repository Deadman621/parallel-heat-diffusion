#include<heat.h>
#include<grid.h>

#include<stdio.h>

int HEAT_ERROR_CODE = -1;
 
state_t create_initial_state(heat_t* config, char* load_from_file, const grid_t *const load_from_grid) {
    grid_t d1 = {config->x, config->y, NULL}; 
    grid_t d2 = d1;

    double  alpha = config->diffusivity * config->dt / config->h * config->h;

    grid_alloc(&d1); grid_alloc(&d2);

    if (load_from_file && load_from_grid) {
        state_t error = {d1, 0, alpha};
        HEAT_ERROR_CODE = INVALID_ARGUMENT;
        return error;
    }

    if (load_from_file) {
        /* File loading logic */
    } 
    
    else if (load_from_grid) {
        state_t error = {*load_from_grid, 0, alpha};

        if (load_from_grid->X != config->x || load_from_grid->Y != config->y) {
            GRID_ERROR_CODE = DIMENSIONAL_MISMATCH;
            return error;
        }

        if (!load_from_grid->data) {
            GRID_ERROR_CODE = NOT_ALLOCATED;
            return error;
        }
     
        grid_copy(*load_from_grid, d1);
    }

    else {
        grid_init(d1, &config->boundary);
        grid_init(d2, &config->boundary);
    } 

    state_t a = {d1, 0, alpha}, b = {d2, 0, alpha};

    config->states[0] = a;
    config->states[1] = b;

    return a;
}

state_t simulate(heat_t config) {
    size_t K = config.K;
    state_t* states = config.states;

    for (size_t i = 0; i < K; i++) {
        size_t k = i & 1;
        states[k ^ 1] = forward(states[k], states[k ^ 1]);
        printf("timestep = %lu\n", i);
        grid_show(states[k ^ 1].domain);
    }

    return states[K & 1];
}

state_t simulate_parallel(heat_t config) {
    size_t K = config.K;
    state_t* states = config.states;

    for (size_t i = 0; i < K; i++) {
        size_t k = i & 1;
        forward_parallel(states[k], &states[k ^ 1]);
        #pragma omp single
        {
            printf("timestep = %lu\n", i);
            grid_show(states[k ^ 1].domain);
        }
    }

    return states[K & 1];
}

void terminate(heat_t* config) {
    grid_t d1 = config->states[0].domain;
    grid_t d2 = config->states[1].domain;
    
    grid_dlloc(&d1); grid_dlloc(&d2);
}