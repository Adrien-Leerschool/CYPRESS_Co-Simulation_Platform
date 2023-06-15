from abc import ABC, abstractmethod


class Simulation(ABC):
    @abstractmethod
    def add_element(self, element):
        pass
