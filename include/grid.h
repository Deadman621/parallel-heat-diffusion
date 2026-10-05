#pragma once

#include<stddef.h>

int GRID_ERROR_CODE = -1;

enum GridErros {
    ALLOCATION_ERROR,
    OUT_OF_BOUNDS
};

typedef struct Grid {
    size_t X;
    size_t Y;
    double* data;
} Grid;

void allocate_grid(Grid grid);
void initialize_grid(Grid grid);
void destroy_grid(Grid grid);

double grid_at_point(Grid grid, size_t i, size_t j);
void grid_set_point(Grid grid, size_t i, size_t j, double value);