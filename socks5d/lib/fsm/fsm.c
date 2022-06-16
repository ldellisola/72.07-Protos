//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#include <stdlib.h>
#include "fsm/fsm.h"

#define N(x) (sizeof(x)/sizeof((x)[0]))

void InitFsm(FiniteStateMachine *fsm, StateDefinition *stateDefinition) {
    fsm->States = stateDefinition;
    fsm->CurrentState = &stateDefinition[0];
    // verificamos que los estados son correlativos, y que están bien asignados.
    for (unsigned i = 0; i <= fsm->StatesSize; i++) {
        if (i != fsm->States[i].state) {
            abort();
        }
    }

    if (fsm->InitialState < fsm->StatesSize) {
        fsm->CurrentState = NULL;
    } else {
        abort();
    }
}

inline static void handle_first(FiniteStateMachine *fsm, void *selectorKey) {
    if (fsm->CurrentState == NULL) {
        fsm->CurrentState = fsm->States + fsm->InitialState;
        if (NULL != fsm->CurrentState->on_arrival) {
            fsm->CurrentState->on_arrival(fsm->CurrentState->state, selectorKey);
        }
    }
}

inline static void jump(FiniteStateMachine *fsm, unsigned next, void *selectorKey) {
    if (next > fsm->StatesSize) {
        abort();
    }
    if (fsm->CurrentState != fsm->States + next) {
        if (fsm->CurrentState != NULL && fsm->CurrentState->on_departure != NULL) {
            fsm->CurrentState->on_departure(fsm->CurrentState->state, selectorKey);
        }
        fsm->CurrentState = fsm->States + next;

        if (NULL != fsm->CurrentState->on_arrival) {
            fsm->CurrentState->on_arrival(fsm->CurrentState->state, selectorKey);
        }
    }
    else {
        if (fsm->CurrentState != NULL && fsm->CurrentState->on_reentry != NULL)
            fsm->CurrentState->on_reentry(fsm->CurrentState->state,selectorKey);
    }
}

unsigned HandleReadFsm(FiniteStateMachine *fsm, void *selectorKey) {
    handle_first(fsm, selectorKey);
    if (fsm->CurrentState->on_read_ready == 0) {
        abort();
    }
    const unsigned int ret = fsm->CurrentState->on_read_ready(selectorKey);
    jump(fsm, ret, selectorKey);

    return ret;
}

unsigned HandleWriteFsm(FiniteStateMachine *fsm, void *selectorKey) {
    handle_first(fsm, selectorKey);
    if (fsm->CurrentState->on_write_ready == 0) {
        abort();
    }
    const unsigned int ret = fsm->CurrentState->on_write_ready(selectorKey);
    jump(fsm, ret, selectorKey);

    return ret;
}

unsigned HandleBlockFsm(FiniteStateMachine *fsm, void *selectorKey) {
    handle_first(fsm, selectorKey);
    if (fsm->CurrentState->on_block_ready == 0) {
        abort();
    }
    const unsigned int ret = fsm->CurrentState->on_block_ready(selectorKey);
    jump(fsm, ret, selectorKey);

    return ret;
}

void HandleCloseFsm(FiniteStateMachine *fsm, void *selectorKey) {
    if (fsm->CurrentState != NULL && fsm->CurrentState->on_departure != NULL) {
        fsm->CurrentState->on_departure(fsm->CurrentState->state, selectorKey);
    }
}

unsigned GetStateFromFsm(FiniteStateMachine *fsm) {
    unsigned ret = fsm->InitialState;
    if (fsm->CurrentState != NULL) {
        ret = fsm->CurrentState->state;
    }
    return ret;
}
