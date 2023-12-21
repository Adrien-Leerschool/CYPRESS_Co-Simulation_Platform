import xmltodict

from cycosim.utils import remove_superfluous

from ..exceptions import AttributeNotFoundError

from cycosim.adapters.simulations import DynawoSimulation, DynawoSimulationParameters


jobs_to_mapping = {
    "xmlns:dyn": "xmlns",
    "name": "name",
    "lib": "solver_library",
    "parFile": {"solver": "solver_parameter_file", "network": "network_parameter_file"},
    "iidmFile": "iidm_file",
    "inputFile": {"curves": "crv_file"},
    "dydFile": "dyd_file",
    "compileDir": "modeler_compilation_directory",
    "parId": {"network": "network_parameter_id", "solver": "solver_parameter_id"},
    "useStandardModels": {
        "precompiledModels": "precompiled_models_use_standard_models",
        "modelicaModels": "modelica_models_use_standard_models",
    },
    "startTime": "start_time",
    "stopTime": "stop_time",
    "directory": "output_directory",
    "exportMode": {
        "curves": "crv_file_export_mode",
        "timeline": "timeline_export_mode",
    },
    "local": {"dumpInitValues": "local_dump_init_values"},
    "global": {"dumpInitValues": "global_dump_init_values"},
    "tag": "logs_tag",
    "file": {"appender": "logs_output_file"},
    "lvlFilter": "logs_filter_level",
    "filter": "timeline_filter",
    "path": "modelica_directory_path",
    "recursive": "modelica_directory_recursive",
}


def set_variable(obj, variable_name, value):
    if not hasattr(obj, variable_name):
        raise AttributeNotFoundError(
            f"Parsing Error : '{variable_name}' is not an attribute of class '{obj.__class__}'."
        )
    else:
        setattr(obj, variable_name, value)


def parse_xml(xml_dict: dict, sim_obj: DynawoSimulation, flag_name: str):
    var_name = ""
    for key, val in xml_dict.items():
        if isinstance(val, dict):
            parse_xml(val, sim_obj, key)

        elif isinstance(val, list):
            pass

        elif isinstance(val, str):
            try:
                if isinstance(jobs_to_mapping[key], dict):
                    var_name = jobs_to_mapping[key][flag_name]
                else:
                    var_name = jobs_to_mapping[key]
            except KeyError:
                raise AttributeNotFoundError(
                    f"JOBS Parsing Error : Unknown flag attribute '{key}' with flag name '{flag_name}' from JOBS file."
                )
            set_variable(sim_obj, var_name, val)

        else:
            print(f"Error : Unmanaged type '{val.type}' during parsing of JOBS file")


class DynawoParserJOBS:
    """
    Summary :
        A class used to parse .jobs files.
        Returns a ParsedFileObject if everything went correctly.

    """

    def __init__(self, _jobs_file: str):
        self.jobs_file = _jobs_file

        self.simulation_parameters = DynawoSimulationParameters()

    def parse(self) -> DynawoSimulationParameters:
        with open(self.jobs_file, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            clean_dict = remove_superfluous(xml_dict, ["@", "dyn:"])
            parse_xml(clean_dict, self.simulation_parameters, "")

        return self.simulation_parameters
