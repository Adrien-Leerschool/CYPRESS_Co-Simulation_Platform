from typing import List

from .dynawo_elements import DynawoParameterSet

from cycosim.domain.ports import Simulation
from cycosim.domain.models.power_system import Component, DynamicModel


class DynawoSimulationParameters:
    # Simulation parameters
    name = None
    start_time = None
    stop_time = None

    # Solver parameters
    solver_library = None
    solver_parameter_id = None
    solver_parameter_file = None

    # File names
    iidm_file = None
    dyd_file = None
    crv_file = None

    # Modeler parameters
    modeler_compilation_directory = None
    network_parameter_id = None
    network_parameter_file = None
    precompiled_models_use_standard_models = None
    modelica_models_use_standard_models = None

    # Logging parameters
    logs_tag = None
    logs_output_file = None
    logs_filter_level = None

    # Outputs parameters
    output_directory = None
    crv_file_export_mode = None

    # Others
    local_dump_init_values = None
    global_dump_init_values = None
    xmlns = "http://www.rte-france.com/dynawo"


class DynawoSimulation(Simulation):
    def __init__(self):
        self.parameter_sets: List[DynawoParameterSet] = None
        self.simulation_parameters: DynawoSimulationParameters = None
        self.static_network: Component = None
        self.dynamic_network: DynamicModel = None

    def add_element(self, element):
        if isinstance(element, list):
            for elem in element:
                if not isinstance(elem, DynawoParameterSet):
                    raise TypeError(
                        f"Class DynawoSimulation does not have list containing {type(elem)}"
                    )
            self.parameter_sets = element
        elif isinstance(element, DynawoSimulationParameters):
            self.simulation_parameters = element
        elif isinstance(element, Component):
            self.static_network = element
        elif isinstance(element, DynamicModel):
            self.dynamic_network = element
        else:
            raise TypeError(f"Class DynawoSimulation does not have {type(element)}")
