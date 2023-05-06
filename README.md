# Shaco

Shaco is a minimal C linux agent for havoc. 
Shaco communicate with http to the server using hardcoded socket

# Commands

This is the commands that the agent support:

- shell { command }
- upload { local remote }
- sleep { time } 
- jitter { time }
- cd {path} - change directory
- checkin - register again the agent and show informations
- pwd - show the location
- exit - quit

# Features

Features of Shaco agent

- Hardcoded Http client
- Custom Memory Management
- Minimal
- No dependencies
- Random Connect ( randomint(sleep, sleep + jitter) )

# Running and Configuration/Compilation

Clone

```
git clone --recurse-submodules https://github.com/souzomain/Shaco.git
```

After clone this repo, you can execute the python handler

```bash
python handler.py
```

Create a http havoc listenner

https://havocframework.com/docs/listeners

To compile this, you can use havoc payload generator in Attack -> Payload and Chose Shaco option

https://havocframework.com/docs/agent

# Issues

the upload option not work if the size of file is > 7000 because http hardcoded not work with chunk, on havoc 0.6 I work on this

# TODO

TODO of the project

- Implement time to exec, ex: run_time 2020/02/02:10.05 rm -rf /
- Configure compilation to compile for macos and android 
- Implement Crypt to the communication ( after havoc 0.6 I work on this )
- Implement Download option
- Implement python support ( ex: pyload cme.py <args> )
- Implement injection to migrate process
- Automatic agent update
