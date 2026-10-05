#include<grid.h>
#include<stdlib.h>

void allocate_grid(Grid grid) {
    size_t grid_dims = grid.X * grid.Y;
    grid.data = calloc(grid_dims, sizeof(double));
    if (grid.data == NULL) 
        GRID_ERROR_CODE = ALLOCATION_ERROR;
}

void initialize_grid(Grid grid) {
    for(size_t i = 0; i < grid.X; i++) {
        for(size_t j = 0; j < grid.Y; j++) {
            double heat = i * j * 2.5;
            grid_set_point(grid, i, j, heat);
        }
    }
}

double grid_at_point(Grid grid, size_t i, size_t j) {
    if (i >= grid.X || j >= grid.Y) {
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return 0.0;    
    }

    size_t index = i * grid.X + j;
    return grid.data[index];
}

void grid_set_point(Grid grid, size_t i, size_t j, double value) {
    if (i >= grid.X || j >= grid.Y) {
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return 0.0;    
    }
        
    size_t index = i * grid.X + j;
    grid.data[index] = value;
}