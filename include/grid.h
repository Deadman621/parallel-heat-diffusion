#pragma once

#include<stddef.h>

extern int GRID_ERROR_CODE;

enum GridErrors {
    ALLOCATION_ERROR,
    NOT_ALLOCATED,
    OUT_OF_BOUNDS,
    DIMENSIONAL_MISMATCH
};

typedef struct Grid {
    size_t X;
    size_t Y;
    double* data;
} grid_t;

void grid_alloc(grid_t* grid);
void grid_dlloc(grid_t* grid);

void grid_init(grid_t grid);
void grid_copy(const grid_t a, grid_t b);

void grid_init_boundary(grid_t grid, double value);

double grid_fetch(const grid_t grid, size_t i, size_t j);
void   grid_store(grid_t grid, size_t i, size_t j, double value);

void grid_show(const grid_t grid);