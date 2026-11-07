#pragma once

#include<stddef.h>

extern size_t GRID_ERROR_CODE;

enum GridErrors {
    NO_ERROR,
    ALLOCATION_ERROR,
    NOT_ALLOCATED,
    OUT_OF_BOUNDS,
    DIMENSIONAL_MISMATCH,
    FILE_OPEN_ERROR,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    GRID_NOT_EMPTY
};

typedef struct Grid {
    size_t X;
    size_t Y;
    double* data;
} grid_t;

void grid_alloc(grid_t* grid);
void grid_dlloc(grid_t* grid);

void grid_init(grid_t grid, const double *const boundary_value);
void grid_copy(const grid_t a, grid_t b);

void grid_init_parallel(grid_t grid, const double *const boundary_value);
void grid_copy_parallel(const grid_t a, grid_t b);

double grid_fetch(const grid_t grid, size_t i, size_t j);
void   grid_store(grid_t grid, size_t i, size_t j, double value);

void grid_fetch_file(grid_t *grid, const char *const file);
void grid_store_file(const grid_t grid, const char *const file);

void grid_show(const grid_t grid);

void grid_error(char *str, size_t size, const size_t GRID_ERROR_CODE);