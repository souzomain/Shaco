#include "../Protocol/Packer.h"
enum {
    COMMAND_OK = 13,
    COMMAND_GET = 14,
    COMMAND_OUTPUT = 15,
    COMMAND_NO_JOB = 16,
    COMMAND_ERR = 17,
    SHACO_MAGIC = 0x6861636f
};

typedef PPACKER (*command_function)(uint8_t *args);
typedef struct command{
    int id;
    command_function fun;
}COMMAND, *PCOMMAND;

PPACKER exec_command(uint8_t *args, int id);
bool checkin();
