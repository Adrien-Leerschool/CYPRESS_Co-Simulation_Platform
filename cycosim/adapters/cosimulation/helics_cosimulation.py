from typing import List

from cycosim.domain.ports.cosimulation import Cosimulation, SimulationConnection
from cycosim.domain.ports.simulation import Simulation

from cycosim.adapters.serializers.helics import HelicsSerializerJSON


class HelicsVariable:
    def __init__(self, _name: str, _value):
        self.name = _name
        self.value = _value


class Publication:
    key: str = None
    info: str = None

    def __init__(self, _key: str, _info: str = None):
        self.key = _key
        self.info = _info

    def to_dict(self):
        return {"key": self.key, "info": self.info}


class Subscription:
    key: str = None
    info: str = None

    def __init__(self, _key: str, _info: str = None):
        self.key = _key
        self.info = _info

    def to_dict(self):
        return {"key": self.key, "info": self.info}


class HelicsCosimulationFederate:
    publications: List[Publication] = []
    subscriptions: List[Subscription] = []
    variables: List[HelicsVariable] = []
    simulation: Simulation = None

    name: str
    core_type: str = "zmq"
    terminate_on_error: str = True
    source_only: bool = False
    observer: bool = False
    only_update_on_change: bool = True
    log_level: str = "Debug"
    uninterruptible: bool = True
    period: float = 1.0

    def add_publication(self, key: str, info: str = None):
        self.publications.append(Publication(key, info))

    def add_subscription(self, key: str, info: str = None):
        self.subscriptions.append(Subscription(key, info))

    def get_parameter(self, parameter_name: str):
        return self.simulation.get_parameter(parameter_name)

    def has_variable(self, var_name: str):
        for var in self.variables:
            if var.name == var_name:
                return True
        return False

    def initialize(self, arguments: dict = {}):
        self.simulation.initialize(arguments)
        HelicsSerializerJSON(arguments["output_path"] + self.name + ".json", self).serialize()


class HelicsCosimulation(Cosimulation):
    power_system_federate: HelicsCosimulationFederate

    def __init__(self):
        super().__init__("Helics_Cosimulation")
        self.power_system_federate = HelicsCosimulationFederate()

    def add_connection(self, connection: SimulationConnection):
        var_1_fed, var_2_fed = None, None

        var_1_fed = self.power_system_federate
        if connection.variable_2 is not None:
            var_2_fed = self.power_system_federate

        if connection.variable_2 is None:
            var_1_fed.add_publication(connection.name, connection.variable_1)

        else:
            if not connection.is_bilateral:
                if connection.emitter == connection.variable_1:
                    var_1_fed.add_publication(connection.name, connection.variable_1)
                    var_2_fed.add_subscription(connection.name, connection.variable_2)
                elif connection.emitter == connection.variable_2:
                    var_2_fed.add_publication(connection.name, connection.variable_2)
                    var_1_fed.add_subscription(connection.name, connection.variable_1)
                else:
                    raise ValueError(
                        "If connection not bilateral, the emitter must be specified"
                        "and be one of the two given variables"
                    )

    def get_connection(self, pub: Publication = None, sub: Subscription = None):
        if pub is None and sub is None:
            raise ValueError("Cannot create connection without neither a publication and a subscription.")

        if pub is not None and sub is None:
            return SimulationConnection(pub.key, pub.info)

    def initialize(self, arguments: dict = {}):
        if "output_path" not in arguments:
            raise ValueError("No output path has been given for the serialized files.")

        self.power_system_federate.initialize(arguments)

    def start(self):
        pass

    def add_federate(self, new_federate: HelicsCosimulationFederate):
        for fed in self.federates:
            if fed.name == new_federate.name:
                raise ValueError(
                    f"Unable to add new federate : Federate with the name '{new_federate.name}' already exists."
                )
        self.federates.append(new_federate)
