from typing import List

from cycosim.domain.ports import ParsedFileObject, FileType

from cycosim.domain.models.power_system import Component, DynamicModel

from cycosim.adapters.simulations import DynawoSimulationParameters, DynawoParameterSet


class IIDMObject(ParsedFileObject):
    """_summary_
    IIDMObject represents any .iidm & .xiidm file that has been parsed.

    Attributes :
        network : The variables that stores the physical representation of the network.
    """

    def __init__(self):
        self.network = Component()
        super().__init__(
            _parsed_file_type=FileType.IIDM, _parsed_file_object=self.network
        )

    def get_object(self):
        return self.network


class JOBSObject(ParsedFileObject):
    """_summary_
    JOBSObject represents any .jobs file that has been parsed.

    Attributes :
        simulation : A DynawoSimulation object gathering all the information
        related to the simulation.
    """

    def __init__(self):
        self.simulation_parameters = DynawoSimulationParameters()
        super().__init__(
            _parsed_file_type=FileType.JOBS,
            _parsed_file_object=self.simulation_parameters,
        )

    def get_object(self):
        return self.simulation_parameters


class DYDObject(ParsedFileObject):
    """_summary_
    DYDObject represents any .dyd file that has been parsed.

    Attributes :
        dynamic_model : A DynamicModel object gathering all the information related to
        the dynamic part of the power system simulated network.
    """

    def __init__(self):
        self.dynamic_model = DynamicModel()
        super().__init__(
            _parsed_file_type=FileType.DYD, _parsed_file_object=self.dynamic_model
        )

    def get_object(self):
        return self.dynamic_model


class PARObject(ParsedFileObject):
    """_summary_
    DYDObject represents any .dyd file that has been parsed.

    Attributes :
        dynamic_model : A DynamicModel object gathering all the information related to
        the dynamic part of the power system simulated network.
    """

    def __init__(self):
        self.parameters: List[DynawoParameterSet] = []
        super().__init__(
            _parsed_file_type=FileType.PAR, _parsed_file_object=self.parameters
        )

    def get_object(self):
        return self.parameters
