//
// Created by tluci on 16/6/2022.
//

#include <unistd.h>
#include "parsers/lulu/client_goodbye_parser.h"
#include "utils/logger.h"

void ClientGoodbyeParserReset(ClientGoodbyeParser *p) {
    Debug("Resetting HelloParser...");
    if (null == p) {
        Debug( "Cannot reset NULL HelloParser");
        return;
    }

    p->State = Goodbye;
    p->Index = 0;

    p->Goodbye[0] = 'G';
    p->Goodbye[1] = 'O';
    p->Goodbye[2] = 'O';
    p->Goodbye[3] = 'D';
    p->Goodbye[4] = 'B';
    p->Goodbye[5] = 'Y';
    p->Goodbye[6] = 'E';

    Debug("HelloParser reset!");
}

ClientGoodbyeParserState traverseWordGoodbye(ClientGoodbyeParser *p, byte c, ClientGoodbyeParserState nextState) {


    if (p->Index == strlen(p->Goodbye)-1) {
        if (c == '\r') {
            return nextState;
        }
        Debug( "Im in the last letter of the word and there is no pipe");
        return GoodbyeInvalidState;
    }

    if (c == p->Goodbye[p->Index]) {
        p->Index++;
        return p->State;
    }
    LogDebug( "wrong character for GOODBYE, i was waiting for %c and got %c", p->Goodbye[p->Index], c);
    return GoodbyeInvalidState;

}

ClientGoodbyeParserState ClientGoodbyeParserFeed(ClientGoodbyeParser *p, byte c) {
    LogDebug("Feeding %d to ClientGoodbyeParser", c);
    if (null == p) {
        Debug( "Cannot feed ClientGoodbyeParser if is NULL");
        return GoodbyeInvalidState;
    }

    switch (p->State) {
        case Goodbye:
            p->State = traverseWordGoodbye(p, c, GoodbyeCRLF);
            break;

        case GoodbyeCRLF:
            p->State = c == '\n'? GoodbyeFinished:GoodbyeInvalidState;
            break;
        case GoodbyeFinished:
        case GoodbyeInvalidState:
            break;
    }
    return p->State;
}

size_t ClientGoodbyeParserConsume(ClientGoodbyeParser *p, byte *c, size_t length) {
    LogDebug("AuthParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if HelloParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "HelloParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientGoodbyeParserState state = ClientGoodbyeParserFeed(p, c[i]);
        if (ClientGoodbyeParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientGoodbyeParserHasFinished(ClientGoodbyeParserState state) {
    switch (state) {
        default:
        case Goodbye:
        case GoodbyeCRLF:
            return false;
        case GoodbyeInvalidState:
        case GoodbyeFinished:
            return true;
    }
}
bool ClientGoodbyeParserHasFailed(ClientGoodbyeParserState state) {
    return state == GoodbyeInvalidState ? true : false;
}


