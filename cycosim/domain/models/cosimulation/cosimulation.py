from typing import List

from cycosim.domain.ports import (
    Cosimulation as GenCosimulation,
    SimulationConnection as GenConnection,
)


class Cosimulation(GenCosimulation):
    name: str
    cosimulator: GenCosimulation

    def __init__(self, _name: str, _cosimulator: GenCosimulation):
        self.name = _name
        self.cosimulator = _cosimulator

    def add_connection(self, connection: GenConnection):
        self.cosimulator.add_connection(connection)

    def add_connections(self, connections: List[GenConnection]):
        for connection in connections:
            self.add_connection(connection)

    def initialize(self, arguments: dict = {}):
        pass

    def start(self):
        pass
