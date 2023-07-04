from abc import ABC, abstractmethod


class SimulationConnection:
    """_summary_
    Within a Cosimulation, there is connections between elements coming from different simulators.
    A SimulationConnection object represents a link between the two given variable.

    Attributes :
        name (str) : The name of the connection.
        variable_1 (str) : The first variable to connect.
        variable_2 (str) : The second variable to connect.
        is_bilateral (bool) : True if the two variables exchange messages. False if there is one emitter and one receiver
        emitter (str) : If is_bilateral is set to true, specifies which one is the emitter.
    """

    name: str
    variable_1: str
    variable_2: str = None
    is_bilateral: bool
    emitter: str = None

    def __init__(
        self,
        _name: str,
        var_1: str,
        var_2: str = None,
        _is_bilateral: bool = True,
        _emitter: str = None,
    ):
        self.name = _name
        self.variable_1 = var_1
        self.variable_2 = var_2
        self.is_bilateral = _is_bilateral
        self.emitter = _emitter


class Cosimulation(ABC):
    name: str

    def __init__(self, _name: str):
        self.name = _name

    @abstractmethod
    def add_connection(self, connection: SimulationConnection):
        pass

    @abstractmethod
    def initialize(self, arguments: dict = {}):
        pass

    @abstractmethod
    def start(self):
        pass
