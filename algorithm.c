void updateState(lift_manager* liftStack, uc_8 currentFloor, uc_8 buttonId) {
    uc_8 requestedFloor;
    uc_8 direction = DIRECTION_IDLE;
    if (buttonId < 4) {
        requestedFloor = buttonId;
    } else {
      if (buttonId == FLOOR_1_UP) {
        requestedFloor = FLOOR_1;
                direction = DIRECTION_UP;
      } else if (buttonId == FLOOR_2_DOWN) {
         requestedFloor = FLOOR_2;
                direction = DIRECTION_DOWN;
      } else if (buttonId == FLOOR_2_UP) {
        requestedFloor = FLOOR_2;
                direction = DIRECTION_UP;
      } else if (buttonId == FLOOR_3_DOWN) {
        requestedFloor = FLOOR_3;
                direction = DIRECTION_DOWN;
      } else if (buttonId == FLOOR_3_UP) {
        requestedFloor = FLOOR_3;
                direction = DIRECTION_UP;
      } else if (buttonId == FLOOR_4_DOWN) {
        requestedFloor = FLOOR_4;
                direction = DIRECTION_DOWN;
      } else {
        return;
      }
    }
    
    if (liftStack->size == 0) { // We first check to see if lift is idle
        lift_manager_push_start(liftStack, requestedFloor, direction, buttonId);
    } else if (direction == DIRECTION_IDLE) {
        // Check if the lift is going up or down
        state nextState = lift_manager_peek(liftStack);
        if ((nextState.direction == DIRECTION_UP || nextState.direction == DIRECTION_IDLE) && requestedFloor >= currentFloor) {
            // The requested floor is on the way up and should be stored in the queue here
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (requestedFloor == stateI.level && (stateI.direction == DIRECTION_UP || stateI.direction == DIRECTION_IDLE)) {
                    unsavedI &= 0x0FFF;
                    unsavedI |= (i + 1);
                    break;
                } else if ((requestedFloor < stateI.level && (stateI.direction == DIRECTION_UP || stateI.direction == DIRECTION_IDLE)) || (stateI.direction == DIRECTION_DOWN)) {
                    unsavedI &= 0x0FFF;
                    unsavedI |= i;
                    break;
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        } else if ((nextState.direction == DIRECTION_UP || nextState.direction == DIRECTION_IDLE) && requestedFloor < currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            bool flag = false;
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (stateI.direction == DIRECTION_DOWN) {
                    flag = true;
                }
                
                if (flag) {
                    if (requestedFloor >= stateI.level && (stateI.direction == DIRECTION_DOWN || stateI.direction == DIRECTION_IDLE)) {
                        unsavedI &= 0x0FFF;
                        unsavedI |= (i + 1);
                        break;
                    } else if (stateI.direction == DIRECTION_UP) {
                        unsavedI &= 0x0FFF;
                        unsavedI |= i;
                        break;
                    }
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        } else if ((nextState.direction == DIRECTION_DOWN || nextState.direction == DIRECTION_IDLE) && requestedFloor <= currentFloor) {
            // The requested floor is on the way down and should be stored in the queue here
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is lower than the lowest floor in queue so we save the index of where it should be saved outside of the loop
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (requestedFloor >= stateI.level && (stateI.direction == DIRECTION_DOWN || stateI.direction == DIRECTION_IDLE)) {
                    unsavedI &= 0x0FFF;
                    unsavedI |= (i + 1);
                    break;
                } else if (stateI.direction == DIRECTION_UP) {
                    unsavedI &= 0x0FFF;
                    unsavedI |= i;
                    break;
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        } else if ((nextState.direction == DIRECTION_DOWN || nextState.direction == DIRECTION_IDLE) && requestedFloor > currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            bool flag = false;
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (stateI.direction == DIRECTION_UP) {
                    flag = true;
                }
                
                if (flag) {
                    if (requestedFloor <= stateI.level && (stateI.direction == DIRECTION_UP || stateI.direction == DIRECTION_IDLE)) {
                        unsavedI &= 0x0FFF;
                        unsavedI |= (i + 1);
                        break;
                    } else if (stateI.direction == DIRECTION_DOWN) {
                        unsavedI &= 0x0FFF;
                        unsavedI |= i;
                        break;
                    }
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        }
    } else if (direction == DIRECTION_UP) {
        if (requestedFloor > currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (requestedFloor <= stateI.level && (stateI.direction == DIRECTION_UP || stateI.direction == DIRECTION_IDLE)) {
                    if (i != 0) {
                        state prevState = lift_manager_get_state(liftStack, i - 1);
                        if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                            i--; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                        }
                    }
                    unsavedI &= 0x0FFF;
                    unsavedI |= i;
                    break;
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
                state prevState = lift_manager_get_state(liftStack, liftStack->size - 1);
                if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                    unsavedI = liftStack->size - 1; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                }
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        } else if (requestedFloor <= currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            bool flag = false;
            bool flag1 = false;
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (stateI.direction == DIRECTION_DOWN) {
                    flag = true;
                }
                
                if (flag) {
                    if (stateI.direction == DIRECTION_UP) {
                        flag1 = true;
                    }
                }
                
                if (flag1) {
                    if (requestedFloor <= stateI.level && stateI.direction == DIRECTION_UP) {
                        if (i != 0) {
                            state prevState = lift_manager_get_state(liftStack, i - 1);
                            if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                                i--; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                            }
                        }
                        unsavedI &= 0x0FFF;
                        unsavedI |= i;
                        break;
                    }
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
                state prevState = lift_manager_get_state(liftStack, unsavedI - 1);
                if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                    unsavedI--; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                }
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        }
    } else if (direction == DIRECTION_DOWN) {
        if (requestedFloor < currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (requestedFloor >= stateI.level && stateI.direction == DIRECTION_DOWN) {
                    if (i != 0) {
                        state prevState = lift_manager_get_state(liftStack, i - 1);
                        if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                            i--; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                        }
                    }
                    unsavedI &= 0x0FFF;
                    unsavedI |= i;
                    break;
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                state prevState = lift_manager_get_state(liftStack, liftStack->size - 1);
                if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                    unsavedI = (unsavedI & 0x0FFF) | (liftStack->size - 1); // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                }
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        } else if (requestedFloor >= currentFloor) {
            ui_16 unsavedI = 0xF000; // This is incase the requested floor is higher than the highest floor in queue so we save the index of where it should be saved outside of the loop
            bool flag = false;
            bool flag1 = false;
            for (ui_16 i = 0; i < liftStack->size; i++) {
                state stateI = lift_manager_get_state(liftStack, i);
                if (stateI.direction == DIRECTION_UP) {
                    flag = true;
                }
                
                if (flag) {
                    if (stateI.direction == DIRECTION_DOWN) {
                        flag1 = true;
                    }
                }
                
                if (flag1) {
                    if (requestedFloor >= stateI.level && stateI.direction == DIRECTION_DOWN) {
                        if (i != 0) {
                            state prevState = lift_manager_get_state(liftStack, i - 1);
                            if (prevState.direction == DIRECTION_IDLE && prevState.level == requestedFloor) {
                                i--; // We do this to normalise the array so that idle direction comes after directed direction. This helps in popping the vector later
                            }
                        }
                        unsavedI &= 0x0FFF;
                        unsavedI |= i;
                        break;
                    }
                }
            }
            if (unsavedI >> 12) {
                unsavedI = liftStack->size;
            }
            lift_manager_insert(liftStack, requestedFloor, direction, buttonId, unsavedI);
        }
    }
}
