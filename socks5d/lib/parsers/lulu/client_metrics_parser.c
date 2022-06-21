//
// Created by tluci on 17/6/2022.
//

#include "parsers/lulu/client_metrics_parser.h"
#include "utils/logger.h"

ClientMetricsParserState traverseWordMetrics(ClientMetricsParser *p, byte c, ClientMetricsParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == MetricsGet) ||(c == '\r' && p->State == Metrics)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        Debug( "The word has finished and character given isnt a terminating character");
        return MetricsInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogDebug( "%c is not part of the word \" %s \"", c, p->Word);
    return MetricsInvalidState;
}

void ClientMetricsParserReset(ClientMetricsParser *p) {
    Debug("Resetting ClientMetricsParser...");
    if (null == p) {
        Debug( "Cannot reset NULL ClientMetricsParser");
        return;
    }

    p->State = MetricsGet;
    p->Index = 0;

    p->Metrics[0] = 'M';
    p->Metrics[1] = 'E';
    p->Metrics[2] = 'T';
    p->Metrics[3] = 'R';
    p->Metrics[4] = 'I';
    p->Metrics[5] = 'C';
    p->Metrics[6] = 'S';
    p->Metrics[7] = 0;

    p->Get[0] = 'G';
    p->Get[1] = 'E';
    p->Get[2] = 'T';
    p->Get[3] = 0;
    p->Word = p->Get;

    Debug("MetricsParser reset!");
}

ClientMetricsParserState ClientMetricsParserFeed(ClientMetricsParser *p, byte c) {
    LogDebug("Feeding %d to ClientMetricsParser", c);

    if (null == p) {
        Debug( "Cannot feed MetricsParser if is NULL");
        return MetricsInvalidState;
    }

    switch (p->State) {
        case MetricsGet:
            p->State = traverseWordMetrics(p, c, Metrics, p->Metrics);
            break;
        case Metrics:
            p->State = traverseWordMetrics(p, c, MetricsCRLF, null);
            break;

        case MetricsCRLF:
            if( c == '\n'){
                p->State = MetricsFinished;
                break;
            }
            Debug( "There is a CR but no LF");
            p->State =  MetricsInvalidState;
            break;
        case MetricsFinished:
        case MetricsInvalidState:
            break;
    }
    return p->State;
}
size_t ClientMetricsParserConsume(ClientMetricsParser *p, byte *c, size_t length) {
    LogDebug("ClientMetricsParser consuming %d bytes", length);
    if (null == p) {
        Debug( "Cannot consume if ClientMetricsParser is NULL");
        return 0;
    }

    if (null == c) {
        Debug( "ClientMetricsParser cannot consume NULL array");
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        ClientMetricsParserState state = ClientMetricsParserFeed(p, c[i]);
        if (ClientMetricsParserHasFinished(state))
            return i + 1;
    }
    return length;
}

bool ClientMetricsParserHasFinished(ClientMetricsParserState state) {
    switch (state) {
        default:
        case MetricsGet:
        case Metrics:
        case MetricsCRLF:
            return false;
        case MetricsInvalidState:
        case MetricsFinished:
            return true;
    }
}

bool ClientMetricsParserHasFailed(ClientMetricsParserState state) {
    return state == MetricsInvalidState ? true : false;
}


