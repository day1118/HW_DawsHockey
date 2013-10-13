#include "globals.h"
#include "statehelp.h"

void changeState(int newstate){
    state = newstate;
    switch(newstate) {
        case 1:     //drive
            driveStart = 1;
            break;
        case 2:     //goalfind
            goalfindStart = 1;
            break;
        case 3:     //rotate
            reverseStart = 1;
            break;
        case 4:
            kickStart = 1;
            break;
        case 5:
            quickspinStart = 1;
            break;
    }
}