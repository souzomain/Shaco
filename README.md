# Shaco

Shaco is a minimal C linux agent for havoc. 
Shaco communicate with http to the server using hardcoded socket

# Features

This is the commands that the agent support:

- shell { command }
- upload { local remote }
- sleep { time }
- cd {path}
- checkin
- pwd
- exit

# Configuration/Compilation

Clone

```
git clone --recurse-submodules https://github.com/souzomain/Shaco.git
```

While I don't integrate the agent compilation in havoc, you can compile using cmake

To configure the agent, see the file Config/Settings.c

```c
#define IP "127.0.0.1"
#define ENDPOINT ""
#define DOMAIN "google.com"
#define USERAGENT "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0"
#define VERSION "0.1"
#define TIMEOUT 10
#define PORT 8080
#define USESSL false
```

Change domain to you host
Change the ip to you ip server
Change useragent to you configured useragent
Change the endpoint to you configured endpoint ex: admin/index.php
Change the port to the port of TS

You can compile using this:

```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

# TODO

TODO of the project

- Implement the jitter
- Configure handler to compile the payload
- Implement AES Crypt to the communication
- Implement Download option
- Implement python support ( ex: pyload cme.py <args> )
- Implement injection to migrate process
- Automatic agent update
- Crypt the settings
