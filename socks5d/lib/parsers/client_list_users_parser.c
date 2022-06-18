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
        LogError(false, "The word has finished and character given isnt a terminating character");
        return ListInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError(false, "%c is not part of the word \" %s \"", c, p->Word);
    return ListInvalidState;
}

void ClientListUsersParserReset(ClientListUsersParser *p) {
    LogInfo("Resetting ClientListUsersParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientListUsersParser");
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

    LogInfo("ListUsersParser reset!");
}

ClientListUsersParserState ClientListUsersParserFeed(ClientListUsersParser *p, byte c) {
    LogInfo("Feeding %d to ClientListUsersParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed ListUsersParser if is NULL");
        return ListInvalidState;
    }

    switch (p->State) {
        case List:
//            LogError(false, "BufferSizeGet");
            p->State = traverseWordListUsers(p, c, ListUsers, p->Users);
            break;
        case ListUsers:
//            LogError(false, "BufferSize");
            p->State = traverseWordListUsers(p, c, ListCRLF, null);
            break;

        case ListCRLF:
//            LogError(false, "BufferSizeCRLF");
            if( c == '\n'){
                p->State = ListFinished;
                break;
            }
            LogError(false, "There is a CR but no LF");
            p->State =  ListInvalidState;
            break;
        case ListFinished:
//            LogError(false, "BufferSizeFinished");
        case ListInvalidState:
//            LogError(false, "BufferSizeInvalidState");
            break;
    }
    return p->State;
}
size_t ClientListUsersParserConsume(ClientListUsersParser *p, byte *c, size_t length) {
    LogInfo("ClientBufferSizeParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientListUsersParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientListUsersParser cannot consume NULL array");
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
