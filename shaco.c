
#include "Core/Core.h"
#include "Core/Communication.h"
#include "Core/Commands.h"
#include "Config/Settings.h"

#include <unistd.h>

int main(int argc, char **argv){
    if(!shaco_init(1, argv))  { MSG("Can't init..."); return 0; };
    MSG("Shaco initialized");
    PSETTINGS sett = get_settings();
    do{
        boxboxbox();
        sleep(sett->timeout);
    }while(!sett->quit);
    MSG("Quiting");
    shaco_exit();
    return 0;
}

__attribute__((constructor)) void lib_main(void){
    main(0,NULL);
}
