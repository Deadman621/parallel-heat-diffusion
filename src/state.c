#include<grid.h>
#include<state.h>

double forward_cell(state_t S, size_t i, size_t j) {
    grid_t data = S.domain;
    
    double T = grid_fetch(data, i, j);
    
    double T_1 = grid_fetch(data, i - 1, j);
    double T_2 = grid_fetch(data, i + 1, j);
    double T_3 = grid_fetch(data, i, j - 1);
    double T_4 = grid_fetch(data, i, j + 1); 

    double cell = T + S.alpha * (T_1 + T_2 + T_3 + T_4 - (4 * T)); 
    return cell;
}

state_t forward(const state_t A, state_t B) {
    grid_t data = B.domain;

    size_t x = A.domain.X;
    size_t y = A.domain.Y;

    for(size_t i = 1; i < x - 1; i++) {
        for(size_t j = 1; j < y - 1; j++) {
            double cell = forward_cell(A, i, j);
            grid_store(data, i, j, cell);
        }
    }

    B.k = A.k + 1;
    return B;
}