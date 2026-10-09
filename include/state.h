#pragma once

#include<stddef.h>
#include<grid.h>

typedef struct State {
    grid_t domain;
    size_t k; // time step

    double alpha;
} state_t;

state_t forward(const state_t A, state_t B);