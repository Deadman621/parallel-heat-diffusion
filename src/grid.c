#include<omp.h>
#include<grid.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

size_t GRID_ERROR_CODE = NO_ERROR;

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
        #pragma omp atomic write
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
        }   
    }
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
        #pragma omp atomic write
        GRID_ERROR_CODE = NOT_ALLOCATED;
        return;
    }

    if (a.X != b.X || a.Y != b.Y) {
        #pragma omp atomic write
        GRID_ERROR_CODE = NOT_ALLOCATED;
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
        #pragma omp atomic write
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return 0.0;    
    }

    size_t index = i * grid.X + j;
    return grid.data[index];
}

void grid_store(grid_t grid, size_t i, size_t j, double value) {
    if (i >= grid.X || j >= grid.Y) {
        #pragma omp atomic write
        GRID_ERROR_CODE = OUT_OF_BOUNDS;
        return;    
    }
        
    size_t index = i * grid.X + j;
    grid.data[index] = value;
}

void grid_fetch_file(grid_t *grid, const char *const file) {
    if (grid->data != NULL) {
        GRID_ERROR_CODE = GRID_NOT_EMPTY;
        return;
    }

    FILE *fp = fopen(file, "rb");
    
    if (fp == NULL) {
        GRID_ERROR_CODE = FILE_OPEN_ERROR;
        return;
    }

    // read metadata
    size_t read;

    read = fread(grid, sizeof(size_t), 2, fp);
    if (read != 2) {
        GRID_ERROR_CODE = FILE_READ_ERROR;
        fclose(fp);
        return;
    }

    size_t grid_size = grid->X * grid->Y;
    
    grid_alloc(grid);
    if (GRID_ERROR_CODE == ALLOCATION_ERROR) {
        fclose(fp);
        return;
    }

    read = fread(grid->data, sizeof(double), grid_size, fp);
    
    if (read != grid_size) {
        GRID_ERROR_CODE = FILE_READ_ERROR;
        free(grid->data);
        grid->data = NULL;
        fclose(fp);
        return;
    }

    if (fclose(fp) != 0)
        GRID_ERROR_CODE = FILE_READ_ERROR;
}

void grid_store_file(const grid_t grid, const char *const file) {
    if (grid.data == NULL)
        return;

    FILE *fp = fopen(file, "wb");
    
    if (fp == NULL) {
        GRID_ERROR_CODE = FILE_OPEN_ERROR;
        return;
    }

    // write metadata
    size_t written;

    written = fwrite(&grid, sizeof(size_t), 2, fp);
    if (written != 2) {
        GRID_ERROR_CODE = FILE_WRITE_ERROR;
        fclose(fp);
        return;
    }

    size_t grid_size = grid.X * grid.Y;

    written = fwrite(grid.data, sizeof(double), grid_size, fp);
    
    if (written != grid_size) {
        GRID_ERROR_CODE = FILE_WRITE_ERROR;
        fclose(fp);
        return;
    }

    if (fclose(fp) != 0)
        GRID_ERROR_CODE = FILE_WRITE_ERROR;
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

void grid_error(char *str, size_t size, const size_t error) {
    if (!str) return;
    switch (error) {
    case 1:
        snprintf(str, size, "Grid: Memory Allocation Error");
        break;

    case 2:
        snprintf(str, size, "Grid: Not Allocated");
        break;

    case 3:
        snprintf(str, size, "Grid: Out of Bounds");
        break;

    case 4:
        snprintf(str, size, "Grid: Dimensional Mismatch");
        break;

    case 5:
        snprintf(str, size, "Grid: File Open Error");
        break;

    case 6:
        snprintf(str, size, "Grid: File Write Error");
        break;
    
    case 7:
        snprintf(str, size, "Grid: File Read Error");
        break;

    case 8:
        snprintf(str, size, "Grid: Not Empty");
        break;
    
    default:
        snprintf(str, size, "Grid: No Errors");
        break;
    }
}