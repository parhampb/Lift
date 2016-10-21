//
//  fvector.c
//  Lab 1 Week 10
//
//  Created by Parham Bakhtiari on 3/05/2016.
//  Copyright © 2016 Parham Bakhtiari. All rights reserved.
//

#include "lift_manager.h"


lift_manager* lift_manager_init(ui_16 init_capacity) {
    lift_manager* v = (lift_manager *) malloc(sizeof(lift_manager));
    
    v->data = (ui_32 *) malloc(init_capacity * sizeof(ui_32));
    
    v->size = 0;
    v->capacity = init_capacity;
    
    return v;
}


ui_16 lift_manager_resize(lift_manager* v, ui_16 new_capacity) {
    v->data = (ui_32 *) realloc(v->data, new_capacity * sizeof(ui_32));
    
    v->capacity = new_capacity;
    
    return 0;
}

void lift_manager_push_end(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId) {
    lift_manager_insert(v, level, direction, buttonId, v->size);
}

void lift_manager_push_start(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId) {
    lift_manager_insert(v, level, direction, buttonId, 0);
}

bool lift_manager_insert(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId, ui_16 position) {
    if (position > v->size) {
        return false;
    }
    
    v->size++;
    if (v->capacity == v->size) {
        lift_manager_resize(v, v->capacity << 1);
    }
    ui_32 fullState = ((ui_32)buttonId) << 16 | ((ui_32)direction) << 8 | level;
    
    ui_16 i;
    for (i = v->size; i > position; i--) {
        v->data[i] = v->data[i - 1];
    }
    
    v->data[position] = fullState;
    return true;
}

ui_32 lift_manager_get(lift_manager* v, ui_16 index) {
    if (index < v->size) {
        return v->data[index];
    }
    
    return 0;
}

state lift_manager_pop(lift_manager* v) {
    state currentState = {0xFF, 0xFF, 0xFF};
    if (v->size > 0) {
        ui_32 fullState = lift_manager_get(v, 0);
        currentState.level = fullState & 0x000000FF;
        currentState.direction = (fullState >> 8) & 0x000000FF;
        currentState.buttonId = (fullState >> 16) & 0x000000FF;
        
        ui_16 i;
        for (i = 0; i < v->size - 1; i++) {
            v->data[i] = v->data[i + 1];
        }
        v->size--;
    }
    return currentState;
}

state lift_manager_peek(lift_manager* v) {
    state currentState = {0xFF, 0xFF, 0xFF};
    if (v->size <= 0) {
        return currentState;
    }
    ui_32 fullState = lift_manager_get(v, 0);
    currentState.level = fullState & 0x000000FF;
    currentState.direction = (fullState >> 8) & 0x000000FF;
    currentState.buttonId = (fullState >> 16) & 0x000000FF;
    return currentState;
}

state lift_manager_get_state(lift_manager* v, ui_16 position) {
    ui_32 fullState = v->data[position];
    state currentState;
    currentState.level = fullState & 0x000000FF;
    currentState.direction = (fullState >> 8) & 0x000000FF;
    currentState.buttonId = (fullState >> 16) & 0x000000FF;
    return currentState;
}

void lift_manager_free(lift_manager* v) {
    free(v->data);
}
