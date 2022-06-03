

#include "../../headers/logger.h"
#include "../../headers/req_parser.h"
void req_parser_init(struct req_parser *p)
{
    p->state = req_version;
    p->cmd = 0;
}

enum req_state req_parser_feed(struct req_parser *p, uint8_t b)
{
    switch (p->state)
    {
        case req_version:
            if (b == 0x05)
            {
                p->state = req_cmd;
                LogInfo("request Version 5");
            }
            else
            {
                LogError(false, "request wants other version different to 5");
                p->state = req_error_unsupported_version;
            }
            break;
        case req_cmd:
            p->cmd = b;
            p->state = req_rsv;
            LogInfo("Current state: cmd");
            break;
        case req_rsv:
            p->state = req_atyp;
            LogInfo("Current state: rsv");
            break;
        case req_atyp:
            p->atyp = b;
            p->state = req_dest_addr;
            LogInfo("Current state: atyp");
            break;
        case req_dest_addr:
//            TODO: solve address times
            LogInfo("Current state: dest_addr");
            if(b == ATYP_IPV4){
                LogInfo("Adress type: ipv4");
            }else if(b == ATYP_IPV6){
                LogInfo("Adress type: ipv6");
            }else if(b == ATYP_DOMAINNAME){
                LogInfo("Adress type: domainname");
            }
            p->state = req_dest_port;
            break;
        case req_dest_port:
            p->dest_port = b;
            p->state = req_done;
            break;
        case req_done:
        case req_error_unsupported_version:
            break;
        default:
            LogError(false, "request invalid state");
            p->state = req_invalid_state;
            break;
    }
    return p->state;
}
