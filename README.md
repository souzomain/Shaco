# Shaco

Shaco is a minimal C linux agent for https://github.com/HavocFramework/Havoc.
Shaco communicate with http to the server using hardcoded socket

# Commands

This is the commands that the agent support:

- shell { command }
- upload { localfile remotefile }
- download { remotefile } - download file from remote
- sleep { time } 
- jitter { time }
- cd { path } - change directory
- checkin - register again the agent and show informations
- pwd - show the location
- exit

# Features

Features of Shaco agent

- Random Connect ( randomint(sleep, sleep + jitter) )
- Random hash from http send to avoid rules
- Hardcoded Http client
- Custom Memory Management
- Minimal
- No dependencies
- InLine syscall
- Hide Cmdline changing for a random process in the target

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

- Implement python support ( ex: pyload cme.py <args> )
- Implement in memory file exec ( after havoc 0.6 I work on this )
- Implement shared library injection to migrate process
- Better compilation using havoc
- Update shell command to execute async
- Create "job" command equals demon job
- Implement time to exec, ex: run_time 2020/02/02:10.05 rm -rf /
- Configure compilation to compile for macos and android 
- Implement Crypt to the communication ( after havoc 0.6 I work on this )
- Automatic agent update ( optional )
