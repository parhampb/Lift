//
//  lift_manager.h
//  Lab 1 Week 10
//
//  Created by Parham Bakhtiari on 3/05/2016.
//  Copyright © 2016 Parham Bakhtiari. All rights reserved.
//

#ifndef lift_manager_h
#define lift_manager_h

#include <stdlib.h>
#include "clic3.h"

//const uc_8 DIRECTION_UNKNOWN = 0xFF;
//const uc_8 BUTTON_UNKNOWN = 0xFF;
//
//const uc_8 FLOOR_UNKNOWN = 0xFF;

typedef struct {
    ui_32* data;
    //uc_8* state;
    ui_16 capacity;
    ui_16 size;
} lift_manager;

typedef struct {
    uc_8 level;
    uc_8 direction;
    uc_8 buttonId;
} state;

lift_manager* lift_manager_init(ui_16 init_capacity);
ui_16 lift_manager_resize(lift_manager* v, ui_16 new_capacity);
ui_32 lift_manager_get(lift_manager* v, ui_16 index);
void lift_manager_push_end(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId);
void lift_manager_push_start(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId);
bool lift_manager_insert(lift_manager* v, uc_8 level, uc_8 direction, uc_8 buttonId, ui_16 position);
state lift_manager_pop(lift_manager* v);
state lift_manager_peek(lift_manager* v);
state lift_manager_get_state(lift_manager* v, ui_16 position);
void lift_manager_free(lift_manager* v);

#endif /* lift_manager_h */
