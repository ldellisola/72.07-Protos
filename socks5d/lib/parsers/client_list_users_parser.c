//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_list_users_parser.h"
#include "utils/logger.h"

ClientListUsersParserState traverseWordListUsers(ClientListUsersParser *p, byte c, ClientListUsersParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == List) ||(c == '\r' && p->State == ListUsers)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Error( "The word has finished and character given isnt a terminating character");
        return ListInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError( "%c is not part of the word \" %s \"", c, p->Word);
    return ListInvalidState;
}

void ClientListUsersParserReset(ClientListUsersParser *p) {
    Debug("Resetting ClientListUsersParser...");
    if (null == p) {
        Error( "Cannot reset NULL ClientListUsersParser");
        return;
    }

    p->State = List;
    p->Index = 0;

    p->List[0] = 'L';
    p->List[1] = 'I';
    p->List[2] = 'S';
    p->List[3] = 'T';
    p->List[4] = 0;

    p->Users[0] = 'U';
    p->Users[1] = 'S';
    p->Users[2] = 'E';
    p->Users[3] = 'R';
    p->Users[4] = 'S';
    p->Users[5] = 0;
    p->Word = p->List;

    Debug("ListUsersParser reset!");
}

ClientListUsersParserState ClientListUsersParserFeed(ClientListUsersParser *p, byte c) {
    LogDebug("Feeding %d to ClientListUsersParser", c);
//    Error( "char = %c", c);

    if (null == p) {
        Error( "Cannot feed ListUsersParser if is NULL");
        return ListInvalidState;
    }

    switch (p->State) {
        case List:
//            Error( "BufferSizeGet");
            p->State = traverseWordListUsers(p, c, ListUsers, p->Users);
            break;
        case ListUsers:
//            Error( "BufferSize");
            p->State = traverseWordListUsers(p, c, ListCRLF, null);
            break;

        case ListCRLF:
//            Error( "BufferSizeCRLF");
            if( c == '\n'){
                p->State = ListFinished;
                break;
            }
            Error( "There is a CR but no LF");
            p->State =  ListInvalidState;
            break;
        case ListFinished:
//            Error( "BufferSizeFinished");
        case ListInvalidState:
//            Error( "BufferSizeInvalidState");
            break;
    }
    return p->State;
}
size_t ClientListUsersParserConsume(ClientListUsersParser *p, byte *c, size_t length) {
    LogDebug("ClientBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        Error( "Cannot consume if ClientListUsersParser is NULL");
        return 0;
    }

    if (null == c) {
        Error( "ClientListUsersParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientListUsersParserState state = ClientListUsersParserFeed(p, c[i]);
        if (ClientListUsersParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientListUsersParserHasFinished(ClientListUsersParserState state) {
    switch (state) {
        default:
        case List:
        case ListUsers:
        case ListCRLF:
            return false;
        case ListInvalidState:
        case ListFinished:
            return true;
    }
}
