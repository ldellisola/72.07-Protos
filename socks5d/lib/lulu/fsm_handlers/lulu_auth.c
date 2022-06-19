//
// Created by tluci on 19/6/2022.
//

#include "lulu/fsm_handlers/lulu_auth.h"
#include "lulu/lulu_connection.h"

#define ATTACHMENT(key) ( (LuluConnection*)((SelectorKey*)(key))->Data)

void AuthReadInit(unsigned int state, void *data) {
//    LuluConnection *connection = ATTACHMENT(data);
//    AuthData *d = &connection->Data.Auth;

//    d->AuthSucceeded = false;
//    d->ReadBuffer = &connection->ReadBuffer;
//    d->WriteBuffer = &connection->WriteBuffer;
//    AuthParserReset(&d->Parser);
}
