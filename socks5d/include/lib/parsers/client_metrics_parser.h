//
// Created by tluci on 17/6/2022.
//

#ifndef SOCKS5D_CLIENT_METRICS_PARSER_H
#define SOCKS5D_CLIENT_METRICS_PARSER_H

#include "utils/utils.h"
#include <string.h>
#include <stdbool.h>

typedef enum {
    MetricsGet,
    Metrics,
    MetricsCRLF,
    MetricsFinished,
    MetricsInvalidState

} ClientMetricsParserState;

typedef struct {
    ClientMetricsParserState State;
    char Metrics[8];
    char Get[4];
    uint8_t Index;
    char* Word;

} ClientMetricsParser;
bool ClientMetricsParserHasFinished(ClientMetricsParserState state);
void ClientMetricsParserReset(ClientMetricsParser *p);
ClientMetricsParserState ClientMetricsParserFeed(ClientMetricsParser *p, byte c);
size_t ClientMetricsParserConsume(ClientMetricsParser *p, byte *c, size_t length);
bool ClientMetricsParserHasFinished(ClientMetricsParserState state);
#endif //SOCKS5D_CLIENT_METRICS_PARSER_H
