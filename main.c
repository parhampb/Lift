#include "lift_manager.h"
#include "lift1lib.h"

extern ui_16 BusAddress, BusData;


const uc_8 FLOOR_1 = 0x00;
const uc_8 FLOOR_2 = 0x01;
const uc_8 FLOOR_3 = 0x02;
const uc_8 FLOOR_4 = 0x03;

const uc_8 FLOOR_1_UP = 0x08;
const uc_8 FLOOR_2_DOWN = 0x09;
const uc_8 FLOOR_2_UP = 0x0A;
const uc_8 FLOOR_3_DOWN = 0x0B;
const uc_8 FLOOR_3_UP = 0x0C;
const uc_8 FLOOR_4_DOWN = 0x0D;

const uc_8 DOOR_OPEN_BUTTON = 0x07;
const uc_8 DOOR_CLOSE_BUTTON = 0x06;
const uc_8 DOOR_OPEN_LED = 0x05;
const uc_8 DOOR_CLOSE_LED = 0x04;

const uc_8 DIRECTION_DOWN = 0x00;
const uc_8 DIRECTION_UP = 0x01;
const uc_8 DIRECTION_IDLE = 0x02;

void systemInit();
bool liftMove(uc_8 currentFloor, uc_8 destinationFloor);
void liftDoorOpen();
void liftDoorClose();
void updateState(lift_manager* liftStack, uc_8 currentFloor, uc_8 buttonId);

int main() {
    systemInit();
    
    uc_8 buttonId;
    uc_8 floorId = FLOOR_2;
    ui_32 counterResetFloor = 0;
    ui_32 counterDoorOpenClose = 0;
    
    bool liftMoving = false;
    bool liftOpen = false;
    
    lift_manager* destinationFloor = lift_manager_init(4);
    
    while (true) {
        if (buttonGet(&buttonId)) {
            if (buttonId != DOOR_OPEN_BUTTON || buttonId != DOOR_CLOSE_BUTTON) {
                updateState(destinationFloor, floorId, buttonId);
                WriteLed1(buttonId, LEDOn);
                
                counterResetFloor = 0;
            } else if (buttonId == DOOR_OPEN_BUTTON && !liftMoving) {
                liftOpen = true;
                counterResetFloor = 0;
                liftDoorOpen();
            } else if (buttonId == DOOR_CLOSE_BUTTON && !liftMoving && liftOpen && counterDoorOpenClose < 600000) {
                counterResetFloor = 0;
                counterDoorOpenClose = 599999;
            }
        }
        
        if (!liftMoving && !liftOpen && counterResetFloor++ > 3000000) {
            updateState(destinationFloor, floorId, FLOOR_2);
            counterResetFloor = 0;
        }
        
        if (!liftOpen) {
            floorGet(&floorId);
            if (destinationFloor->size > 0 && floorId != lift_manager_peek(destinationFloor).level) {
                if (!liftMoving) {
                    liftMove(floorId, lift_manager_peek(destinationFloor).level);
                    liftMoving = true;
                }
            } else {
                if (liftMoving) {
                    lift1Stop();
                    liftMoving = false;
                    
                    counterResetFloor = 0;
                    
                    liftDoorOpen();
                    liftOpen = true;
                } else if (floorId == lift_manager_peek(destinationFloor).level) {
                  counterResetFloor = 0;
                    
                    liftDoorOpen();
                    liftOpen = true;
                }
            }
        } else {
          counterDoorOpenClose++;
            if (counterDoorOpenClose == 600000) {
                liftDoorClose();
            } else if (counterDoorOpenClose >= 1200000) {
                liftOpen = false;
                counterDoorOpenClose = 0;
                
                state currentState = lift_manager_pop(destinationFloor);
                state nextState = lift_manager_peek(destinationFloor);
                
                WriteLed1(currentState.buttonId, LEDOff);
                
                if (nextState.level == currentState.level && nextState.direction == DIRECTION_IDLE) {
                    state poppedIdleState = lift_manager_pop(destinationFloor);
                    WriteLed1(poppedIdleState.buttonId, LEDOff);
                }
            }
        }
    }
    
    return 0;
}

void systemInit() {
    Initialise();
    liftInit();
    
    uc_8 i;
    for (i = 0; i < 0x0E; i++) {
        WriteLed1(i, LEDOff);
    }
    for (i = 0; i < 0x0E; i++) {
        WriteLed1(i, LEDOn);
        Delay1(1000000);
    }
    for (i = 0x0E; i > 0; ) {
        WriteLed1(--i, LEDOff);
        Delay1(1000000);
    }
    
    uc_8 floorId = 0xFF;
    lift1Down();
    while (!floorGet(&floorId)) {
        BusAddress = Lift1Address;
        BusRead();
        if ((BusData >> 1) & floorMask & 0x06) {
            lift1Stop();
            lift1Up();
        }
    }
    lift1Stop();
    
    liftMove(floorId, FLOOR_2);
    while(floorId != FLOOR_2) {
        floorGet(&floorId);
    }
    lift1Stop();
    
    // This section is purely to clear any interrupts that may have been set on buttons and clear all leds
    buttonGet(&floorId);
    for(i = 0; i < 0x0E; i++) {
        WriteLed1(i, LEDOff);
    }
}

bool liftMove(uc_8 currentFloor, uc_8 destinationFloor) {
    enum bool flag = false;
    if (currentFloor < destinationFloor) {
        lift1Up();
        flag = true;
    } else if (currentFloor > destinationFloor) {
        lift1Down();
        flag = true;
    }
    return flag;
}

void liftDoorOpen() {
    WriteLed1(DOOR_OPEN_LED, LEDOn);
    WriteLed1(DOOR_OPEN_BUTTON, LEDOn);
    WriteLed1(DOOR_CLOSE_LED, LEDOff);
    WriteLed1(DOOR_CLOSE_BUTTON, LEDOff);
}

void liftDoorClose() {
    WriteLed1(DOOR_OPEN_LED, LEDOff);
    WriteLed1(DOOR_OPEN_BUTTON, LEDOff);
    WriteLed1(DOOR_CLOSE_LED, LEDOn);
    WriteLed1(DOOR_CLOSE_BUTTON, LEDOn);
}

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
    
    lift_manager_push_end(liftStack, requestedFloor, direction, buttonId);
}
