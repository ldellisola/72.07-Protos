//
// Created by Lucas Dell'Isola on 09/06/2022.
//

#ifndef SOCKS5D_FSM_H
#define SOCKS5D_FSM_H
/**
 * fsm.c - pequeño motor de maquina de estados donde los eventos son los
 *         del selector.c
 *
 * La interfaz es muy simple, y no es un ADT.
 *
 * Los estados se identifican con un número entero (típicamente proveniente de
 * un enum).
 *
 *  - El usuario instancia un `struct FiniteStateMachine'
 *  - Describe la maquina de estados:
 *      - describe el estado inicial en `InitialState'
 *      - todos los posibles estados en `States' (el orden debe coincidir con
 *        el identificador)
 *      - describe la cantidad de estados en `States'.
 *
 * Provee todas las funciones necesitadas en un `struct fd_handler'
 * de selector.c.
 */


/**
 * definición de un estado de la máquina de estados
 */
typedef struct {
    /**
     * identificador del estado: típicamente viene de un enum que arranca
     * desde 0 y no es esparso.
     */
    unsigned state;

    /** ejecutado al arribar al estado */
    void     (*on_arrival)    (const unsigned state, void * selectorKey);
    /** ejecutado al salir del estado */
    void     (*on_departure)  (const unsigned state, void * selectorKey);
    /** ejecutado cuando hay datos disponibles para ser leidos */
    unsigned (*on_read_ready) (void * selectorKey);
    /** ejecutado cuando hay datos disponibles para ser escritos */
    unsigned (*on_write_ready)(void * selectorKey);
    /** ejecutado cuando hay una resolución de nombres lista */
    unsigned (*on_block_ready)(void * selectorKey);
}StateDefinition;

typedef struct {
    /** declaración de cual es el estado inicial */
    unsigned                        InitialState;
    /**
     * declaracion de los estados: deben estar ordenados segun .[].state.
     */
    const StateDefinition *         States;
    /** cantidad de estados */
    unsigned                        StatesSize;
    /** estado actual */
    const StateDefinition *         CurrentState;
} FiniteStateMachine;



/** inicializa el la máquina */
void InitFsm(FiniteStateMachine *fsm);

/** obtiene el identificador del estado actual */
unsigned GetStateFromFsm(FiniteStateMachine *fsm);

/** indica que ocurrió el evento ReadHead. retorna nuevo id de nuevo estado. */
unsigned HandleReadFsm(FiniteStateMachine *fsm, void * selectorKey);

/** indica que ocurrió el evento WriteHead. retorna nuevo id de nuevo estado. */
unsigned HandleWriteFsm(FiniteStateMachine *fsm, void * selectorKey);

/** indica que ocurrió el evento block. retorna nuevo id de nuevo estado. */
unsigned HandleBlockFsm(FiniteStateMachine *fsm, void * selectorKey);

/** indica que ocurrió el evento close. retorna nuevo id de nuevo estado. */
void HandleCloseFsm(FiniteStateMachine *fsm, void * selectorKey);


#endif //SOCKS5D_FSM_H
