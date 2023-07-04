from typing import List

from cycosim.adapters.simulations.dynawo_elements import DynawoParameterSet, DynawoCurves

from cycosim.domain.ports import Simulation, Cosimulation
from cycosim.domain.models.power_system import Component, DynamicModel, DynamicComponent, Connection

from cycosim.adapters.serializers import (
    DynawoSerializerJOBS,
    DynawoSerializerIIDM,
    DynawoSerializerDYD,
    DynawoSerializerPAR,
    DynawoSerializerCRV,
)


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
    timeline_export_mode = None

    # Others
    local_dump_init_values = None
    global_dump_init_values = None
    xmlns = "http://www.rte-france.com/dynawo"

    def get_parameter(self, param_name: str):
        if param_name in self.__dict__:
            return self.__dict__[param_name]
        return None


class DynawoSimulation(Simulation):
    def __init__(self):
        self.parameter_sets: List[DynawoParameterSet] = None
        self.simulation_parameters: DynawoSimulationParameters = None
        self.static_network: Component = None
        self.dynamic_network: DynamicModel = None
        self.curves: DynawoCurves = None

    def add_element(self, element):
        if isinstance(element, list):
            for elem in element:
                if not isinstance(elem, DynawoParameterSet):
                    raise TypeError(f"Class DynawoSimulation does not have list containing {type(elem)}")
            self.parameter_sets = element
        elif isinstance(element, DynawoSimulationParameters):
            self.simulation_parameters = element
        elif isinstance(element, Component):
            self.static_network = element
        elif isinstance(element, DynamicModel):
            self.dynamic_network = element
        else:
            raise TypeError(f"Class DynawoSimulation does not have {type(element)}")

    def get_parameter(self, var_name: str):
        split_var = var_name.split("/")
        pure_name = split_var[-1]

        param = None
        # Check in the parameters class
        for sets in self.parameter_sets:
            param = sets.get_parameter(pure_name)
            if param is not None:
                return param

        # Check in the simulation class
        param = self.simulation_parameters.get_parameter(pure_name)
        if param is not None:
            return param

        # Check in the static network
        param = self.static_network.get_sub_component(pure_name)
        if param is not None:
            return param

        # Check in the dynamic network
        param = self.dynamic_network.get_parameter(pure_name)
        if param is not None:
            return param

        return None

    def add_cosimulation_connections(self, cosim_obj: Cosimulation):
        connections = [cosim_obj.get_connection(pub=p) for p in cosim_obj.power_system_federate.publications]
        par_id = "CosimInterface"

        if connections:
            dyn_comp = DynamicComponent()
            dyn_comp.id = par_id
            dyn_comp.library = "CosimulationAutomaton"
            dyn_comp.parameter_file = self.simulation_parameters.network_parameter_file
            dyn_comp.parameter_id = par_id

            self.dynamic_network.components.append(dyn_comp)

            cnt = 1
            for con in connections:
                split_var = con.variable_1.split("/")
                curr_con = Connection()
                curr_con.id_1 = split_var[0]
                curr_con.variable_1 = split_var[1]
                curr_con.id_2 = par_id
                curr_con.variable_2 = f"automaton_inputs_{cnt}_"
                cnt += 1

                self.dynamic_network.components.append(curr_con)

    def add_cosimulation_set(self, cosim_obj: Cosimulation):
        connections = [cosim_obj.get_connection(pub=p) for p in cosim_obj.power_system_federate.publications]
        par_id = "CosimInterface"

        if connections:
            param_set = DynawoParameterSet()
            param_set.id = par_id

            param_set.add_parameter(
                name="automaton_SamplingTime",
                dynawo_type="DOUBLE",
                value=str(cosim_obj.power_system_federate.period * 2),
            )

            cnt = 1
            for con in connections:
                param_set.add_parameter(
                    name=f"automaton_InputsName_{cnt}_",
                    dynawo_type="STRING",
                    value=con.name,
                )
                cnt += 1

            param_set.add_parameter(
                name="automaton_NbInputs",
                dynawo_type="INT",
                value=str(cnt),
            )

            param_set.add_parameter(
                name="automaton_NbOutputs",
                dynawo_type="INT",
                value="0",
            )

            self.parameter_sets.append(param_set)

    def initialize(self, arguments: dict = {}):
        self.add_cosimulation_connections(arguments["cosimulation"])
        self.add_cosimulation_set(arguments["cosimulation"])

        DynawoSerializerJOBS(
            arguments["output_path"] + self.simulation_parameters.name + ".jobs", self.simulation_parameters
        ).serialize()
        DynawoSerializerIIDM(
            arguments["output_path"] + self.simulation_parameters.iidm_file, self.static_network
        ).serialize()
        DynawoSerializerDYD(
            arguments["output_path"] + self.simulation_parameters.dyd_file, self.dynamic_network
        ).serialize()
        DynawoSerializerPAR(
            arguments["output_path"] + self.simulation_parameters.network_parameter_file, self.parameter_sets
        ).serialize()

        if self.curves is not None:
            DynawoSerializerCRV(arguments["output_path"] + self.simulation_parameters.crv_file, self.curves).serialize()
