# Shaco

<p align="center">
<img src="https://github.com/souzomain/Shaco/assets/92044641/0576991d-0676-4587-86f0-7107c914f76d" alt="Shaco Generation" />
</p>

Shaco is a minimal C linux agent for the Havoc Framework (https://github.com/HavocFramework/Havoc).

Shaco communicates over http to the Havoc Teamserver using a hardcoded websocket

# Built-In Agent-Commands

These are the commands that the agent is supporting:

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

Features of the Shaco Agent:

- Random Connect ( randomint(sleep, sleep + jitter) )
- Random hash from http send to avoid rules
- Hardcoded Http client
- Custom Memory Management
- Minimal
- No dependencies
- InLine syscall
- Hide Cmdline changing for a random process in the target

# Running and Configuration/Compilation

1. Clone Shaco

```
git clone --recurse-submodules https://github.com/souzomain/Shaco.git
```

2. After cloning this repo, you can execute the python handler. (Make Sure you installed the websocket-client for your Python Version "pip3 install websocket-client" as example)

Also before Executing "handler.py" Edit the Line of the Endpoint where It'll connect to Havoc-Server (Service API/Python API), Change it to your IP if not using it locally.

```bash
python3.<Version> handler.py
```

3. Create a http Havoc listener in your Havoc-Framework Instance, to See how check out:

https://havocframework.com/docs/listeners

To compile this, you can use in Havoc the payload generator in the Menu "Attack" -> "Payload" and Chose Shaco as Option

https://havocframework.com/docs/agent

# Issues

The upload option doesn't work if the size of the file is > 7000 because http is hardcoded and don't work with chunk on havoc 0.6 (I work on this)

# Upcoming TODO's

TODO's of the project

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
