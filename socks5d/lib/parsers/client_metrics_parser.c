//
// Created by tluci on 17/6/2022.
//

#include "parsers/client_metrics_parser.h"
#include "utils/logger.h"

ClientMetricsParserState traverseWordMetrics(ClientMetricsParser *p, byte c, ClientMetricsParserState nextState, char *nextWord) {

    if(strlen(p->Word) == p->Index){
        if((c == '|' && p->State == MetricsGet) ||(c == '\r' && p->State == Metrics)){
            p->Word = nextWord;
            p->Index = 0;
            return nextState;
        }
        LogError(false, "The word has finished and character given isnt a terminating character");
        return MetricsInvalidState;

    }

    if(c == p->Word[p->Index]){
        p->Index++;
        return p->State;
    }
    LogError(false, "%c is not part of the word \" %s \"", c, p->Word);
    return MetricsInvalidState;
}

void ClientMetricsParserReset(ClientMetricsParser *p) {
    LogInfo("Resetting ClientMetricsParser...");
    if (null == p) {
        LogError(false, "Cannot reset NULL ClientMetricsParser");
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

    LogInfo("MetricsParser reset!");
}

ClientMetricsParserState ClientMetricsParserFeed(ClientMetricsParser *p, byte c) {
    LogInfo("Feeding %d to ClientMetricsParser", c);
//    LogError(false, "char = %c", c);

    if (null == p) {
        LogError(false, "Cannot feed MetricsParser if is NULL");
        return MetricsInvalidState;
    }

    switch (p->State) {
        case MetricsGet:
//            LogError(false, "MetricsGet");
            p->State = traverseWordMetrics(p, c, Metrics, p->Metrics);
            break;
        case Metrics:
//            LogError(false, "Metrics");
            p->State = traverseWordMetrics(p, c, MetricsCRLF, null);
            break;

        case MetricsCRLF:
//            LogError(false, "MetricsCRLF");
            if( c == '\n'){
                p->State = MetricsFinished;
                break;
            }
            LogError(false, "There is a CR but no LF");
            p->State =  MetricsInvalidState;
            break;
        case MetricsFinished:
//            LogError(false, "MetricsFinished");
        case MetricsInvalidState:
//            LogError(false, "MetricsInvalidState");
            break;
    }
    return p->State;
}
size_t ClientMetricsParserConsume(ClientMetricsParser *p, byte *c, size_t length) {
    LogInfo("ClientMetricsParser consuming %d bytes", length);
    if (null == p) {
        LogError(false, "Cannot consume if ClientMetricsParser is NULL");
        return 0;
    }

    if (null == c) {
        LogError(false, "ClientMetricsParser cannot consume NULL array");
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

