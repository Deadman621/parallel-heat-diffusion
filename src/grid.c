#include<omp.h>
#include<grid.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

int GRID_ERROR_CODE = -1;

void grid_alloc(grid_t* grid) {
    size_t grid_dims = grid->X * grid->Y;
    grid->data = calloc(grid_dims, sizeof(double));
    if (grid->data == NULL) 
        GRID_ERROR_CODE = ALLOCATION_ERROR;
}

void grid_dlloc(grid_t* grid) {
    if (grid->data == NULL) 
        return; 
    
    free(grid->data);    
    grid->data = NULL;
}

void grid_init(grid_t grid, const double *const boundary_value) {
    if (grid.data == NULL) {
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return; 
    }   

    bool has_boundary_value = boundary_value? true: false;
    const double value = has_boundary_value? *boundary_value: 0;
    
    for(size_t i = 0; i < grid.X; i++) {
        for(size_t j = 0; j < grid.Y; j++) {
            if (has_boundary_value && (i == 0 || j == 0 || i == grid.X - 1 || j == grid.Y - 1)) 
                grid_store(grid, i, j, value);

            else {
                double heat = (double)((i * j) % 39) * 2.5;
                grid_store(grid, i, j, heat);
            }
        }
    }
}

// must be called in a parallel region
void grid_init_parallel(grid_t grid, const double *const boundary_value) {
    if (grid.data == NULL) {
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return; 
    }   

    bool has_boundary_value = boundary_value? true: false;
    const double value = has_boundary_value? *boundary_value: 0;

    #pragma omp for schedule(static) collapse(2) 
    for(size_t i = 0; i < grid.X; i++) {
        for(size_t j = 0; j < grid.Y; j++) {
            if (has_boundary_value && (i == 0 || j == 0 || i == grid.X - 1 || j == grid.Y - 1)) 
                grid_store(grid, i, j, value);

            else {
                double heat = (double)((i * j) % 39) * 2.5;
                grid_store(grid, i, j, heat);
            }
        }    }
}

void grid_copy(const grid_t a, grid_t b) {
    if (a.data == NULL || b.data == NULL) {
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return;
    }

    if (a.X != b.X || a.Y != b.Y) {
        GRID_ERROR_CODE = DIMENSIONAL_MISMATCH;
        return;
    }

    for(size_t i = 0; i < a.X; i++) {
        for(size_t j = 0; j < a.Y; j++) {
            double p = grid_fetch(a, i, j);
            grid_store(b, i, j, p);
        }
    }
}

void grid_copy_parallel(const grid_t a, grid_t b) {
    if (a.data == NULL || b.data == NULL) {
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return;
    }

    if (a.X != b.X || a.Y != b.Y) {
        GRID_ERROR_CODE = DIMENSIONAL_MISMATCH;
        return;
    }

    #pragma omp for schedule(static) collapse(2) 
    for(size_t i = 0; i < a.X; i++) {
        for(size_t j = 0; j < a.Y; j++) {
            double p = grid_fetch(a, i, j);
            grid_store(b, i, j, p);
        }
    }
}

double grid_fetch(const grid_t grid, size_t i, size_t j) {
    if (i >= grid.X || j >= grid.Y) {
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return 0.0;    
    }

    size_t index = i * grid.X + j;
    return grid.data[index];
}

void grid_store(grid_t grid, size_t i, size_t j, double value) {
    if (i >= grid.X || j >= grid.Y) {
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return;    
    }
        
    size_t index = i * grid.X + j;
    grid.data[index] = value;
}

void grid_show(const grid_t grid) {
    if (grid.data == NULL) {
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return;
    }

    for(size_t i = 0; i < grid.X; i++) {
        printf("[");
        for(size_t j = 0; j < grid.Y; j++) {
            size_t index = i * grid.X + j;
            if (j == grid.Y - 1)
                printf("%5.2f", grid.data[index]);
            else 
                printf("%5.2f, ", grid.data[index]);
        }
        if (i == grid.X - 1) printf("]\n");
        else printf("],\n");
    }
}