from asyncio import Task
import base64
import json
import random
import string
from enum import Enum

def build_request(head_type, body: dict) -> dict:
    return {
        "Head": {
            "Type": head_type
        },
        "Body": body
    }


class CommandParam:
    Name: str
    IsFilePath: bool
    IsOptional: bool

    def __init__( self, name: str, is_file_path: bool, is_optional: bool ):
        self.Name = name
        self.IsFilePath = is_file_path
        self.IsOptional = is_optional
        return

class MessageType(Enum):
    Good = 'Good'
    Error = 'Error'
    Info = 'Info'

class Command:
    Name: str
    Description: str
    Help: str
    NeedAdmin: bool
    Mitr: list[ str ]
    Params: list[ CommandParam ]
    CommandId: int
    Author: str

    def job_generate( self, arguments: dict ) -> bytes:
        return b''

    def get_dict( self ) -> dict:
        return {
            "Name": self.Name,
            "Author": self.Author,  # todo: remove this
            "Description": self.Description,
            "Help": self.Help,
            "NeedAdmin": self.NeedAdmin,
            "Mitr": self.Mitr,
        }


class AgentType:
    Name: str
    Author: str
    Version: str
    MagicValue: int
    Description: str
    Arch = list[ str ]
    Formats = list[ dict ]
    Commands: list[ Command ]
    BuildingConfig = dict

    _Service_instance = None

    _current_data: dict = {}

    def task_prepare( self, arguments: dict ) -> bytes:
        for cmd in self.Commands:
            if arguments[ "Command" ] == cmd.Name:
                return cmd.job_generate( arguments )
        return b''
                
    def generate( self, config: dict ) -> None:
        pass

    def download_file( self, agent_id: str, file_name: str, size: int, content: bytes ) -> None:
        ContentB64 = base64.b64encode( content ).decode()

        self._Service_instance.Socket.send( 
            json.dumps( 
                {
                    "Head": {
                        "Type": "Agent"
                    },
                    "Body": {
                        "Type"    : "AgentOutput",
                        "AgentID" : agent_id,
                        "Callback" : {
                            "MiscType" : "download",
                            "FileName" : file_name,
                            "Size"     : size,
                            "Content"  : ContentB64
                        }
                    }
                }
            ) 
        )

        return

    def console_message( self, agent_id: str, type: Enum, message: str, output: str ) -> None:
        
        self._Service_instance.Socket.send(
            json.dumps(
                {
                    "Head": {
                        "Type": "Agent"
                    },
                    "Body": {
                        "Type"    : "AgentOutput",
                        "AgentID" : agent_id,
                        "Callback" : {
                            "Type"   : type.value,
                            "Message": message,
                            "Output" : output
                        }
                    }
                }
            )
        )

        return

    def get_task_queue( self, AgentInfo: dict ) -> bytes:
        
        RandID : str   = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(6))
        Tasks  : bytes = b''

        self._Service_instance.Socket.send(
            json.dumps(
                {
                    "Head": {
                        "Type": "Agent"
                    },
                    "Body": {
                        "Type" :  "AgentTask",
                        "Agent":  AgentInfo, 
                        "Task":   "Get",
                        "RandID": RandID
                    }
                }
            )
        )

        while ( True ):
            if RandID in self._current_data:
                Tasks = self._current_data[ RandID ]
                del self._current_data[ RandID ]
                break
            else:
                continue

        return Tasks

    def register( self, agent_header: dict, register_info: dict ):
        self._Service_instance.Socket.send(
            json.dumps(
                {
                    "Head": {
                        "Type": "Agent"
                    },
                    "Body": {
                        "Type": "AgentRegister",
                        "AgentHeader" : agent_header,
                        "RegisterInfo": register_info
                    }
                }
            )
        )

        return

    def response( self, response: dict ) -> bytes:
        return b''

    def builder_send_message(self, client_id: str, msg_type: Enum, message: str):

        self._Service_instance.Socket.send(
            json.dumps(
                {
                    "Head": {
                        "Type": "Agent"
                    },
                    "Body": {
                        "ClientID": client_id,
                        "Type": "AgentBuild",
                        "Message": {
                            "Type": msg_type.value,
                            "Message": message
                        }
                    }
                }
            )
        )

        return

    def builder_send_payload( self, client_id: str, filename: str, payload: bytes ):

        self._Service_instance.Socket.send(
            json.dumps(
                build_request("Agent", {
                    "ClientID": client_id,
                    "Type": "AgentBuild",
                    "Message": {
                        "FileName": filename,
                        "Payload": base64.b64encode(payload).decode('utf-8')
                    }
                })
            )
        )

        return

    def get_dict( self ) -> dict:
        AgentCommands: list[ dict ] = []

        for command in self.Commands:
            command_params: list[dict] = []

            for param in command.Params:
                command_params.append( {
                    "Name": param.Name,
                    "IsFilePath": param.IsFilePath,
                    "IsOptional": param.IsOptional,
                } )

            AgentCommands.append( {
                "Name": command.Name,
                "Description": command.Description,
                "Help": command.Help,
                "NeedAdmin": command.NeedAdmin,
                "Mitr": command.Mitr,
                "Params": command_params
            } )

        return {
            "Name": self.Name,
            "MagicValue": hex( self.MagicValue ),
            "BuildingConfig": self.BuildingConfig,
            "Arch": self.Arch,
            "Formats": self.Formats,
            "Author": self.Author,
            "Description": self.Description,
            "Commands": AgentCommands
        }
