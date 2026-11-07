#pragma once

#include<stddef.h>
#include<state.h>

extern size_t HEAT_ERROR_CODE;

enum HeatErrors {
    NO_HEAT_ERROR,
    INVALID_ARGUMENT
};

typedef struct Heat {    
    // grid parameters
    size_t x; // horizontal resolution
    size_t y; // vertical resolution
    size_t K; // number of time steps
    
    double h; // spacing
    double dt; // delta_t = size of time step
    double diffusivity;

    double boundary; // boundary values stay fixed

    state_t states[2]; // use only two states for propagation
} heat_t;

state_t create_initial_state(heat_t* config, char* load_from_file, const grid_t *const load_from_grid); 
state_t simulate(heat_t config);

state_t simulate_parallel(heat_t config);

void save_to_file(heat_t config, state_t state);
void terminate(heat_t* config); // cleanup + release all pointers