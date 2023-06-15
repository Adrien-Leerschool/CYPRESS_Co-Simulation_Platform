import io

from cycosim.domain.ports import Serializer, ObjectToSerialize

from cycosim.adapters.parsers.exceptions import AttributeNotFoundError

PREFIX = "dyn:"
INDENT = "  "

var_to_jobs_mapping = {
    "xmlns": "xmlns:dyn",
    "name": "name",
    "solver_library": "lib",
    "solver_parameter_file": "parFile",
    "network_parameter_file": "parFile",
    "iidm_file": "iidmFile",
    "crv_file": "inputFile",
    "dyd_file": "dydFile",
    "modeler_compilation_directory": "compileDir",
    "network_parameter_id": "parId",
    "solver_parameter_id": "parId",
    "precompiled_models_use_standard_models": "useStandardModels",
    "modelica_models_use_standard_models": "useStandardModels",
    "start_time": "startTime",
    "stop_time": "stopTime",
    "output_directory": "directory",
    "crv_file_export_mode": "exportMode",
    "local_dump_init_values": "local",
    "global_dump_init_values": "global",
    "logs_tag": "tag",
    "logs_output_file": "file",
    "logs_filter_level": "lvlFilter",
}

flag_hierarchy = {
    "root": "jobs",
    "jobs": ["job"],
    "job": ["solver", "modeler", "simulation", "outputs"],
    "solver": [],
    "modeler": ["network", "dynModels", "precompiledModels", "modelicaModels"],
    "network": [],
    "dynModels": [],
    "precompiledModels": [],
    "modelicaModels": [],
    "simulation": [],
    "outputs": ["curves", "dumpInitValues", "logs"],
    "curves": [],
    "dumpInitValues": [],
    "logs": ["appender"],
    "appender": [],
}

flag_attributes = {
    "jobs": ["xmlns"],
    "job": ["name"],
    "solver": ["solver_library", "solver_parameter_file", "solver_parameter_id"],
    "modeler": ["modeler_compilation_directory"],
    "network": ["iidm_file", "network_parameter_file", "network_parameter_id"],
    "dynModels": ["dyd_file"],
    "precompiledModels": ["precompiled_models_use_standard_models"],
    "modelicaModels": ["modelica_models_use_standard_models"],
    "simulation": ["start_time", "stop_time"],
    "outputs": ["output_directory"],
    "curves": ["crv_file", "crv_file_export_mode"],
    "dumpInitValues": ["local_dump_init_values", "global_dump_init_values"],
    "logs": [],
    "appender": ["logs_tag", "logs_output_file", "logs_filter_level"],
}


def is_there_non_empty_flag(sim_obj, curr_flag):
    for att in flag_attributes[curr_flag]:
        if getattr(sim_obj, att) is not None:
            return True

    for sub_flag in flag_hierarchy[curr_flag]:
        if is_there_non_empty_flag(sim_obj, sub_flag):
            return True

    return False


def create_flag(sim_obj, flag_name: str, nbr_indents: int):
    flag = f"{INDENT * nbr_indents}<{PREFIX}{flag_name}"
    for att in flag_attributes[flag_name]:
        if att not in var_to_jobs_mapping:
            raise AttributeNotFoundError(
                f"Serializing Error : {att} not found in the variables-to-JOBS-file mapping."
            )
        elif getattr(sim_obj, att) is None:
            continue
        else:
            flag += f' {var_to_jobs_mapping[att]}="{getattr(sim_obj, att)}"'
    return flag


def xml_serializer(
    out_file: io.TextIOWrapper, sim_obj, nbr_indents: int, curr_flag: str
):
    flag = create_flag(sim_obj, curr_flag, nbr_indents)

    if is_there_non_empty_flag(sim_obj, curr_flag):
        if flag_hierarchy[curr_flag]:
            out_file.write(flag + ">\n")
            for sub_flag in flag_hierarchy[curr_flag]:
                xml_serializer(out_file, sim_obj, nbr_indents + 1, sub_flag)
            out_file.write(f"{INDENT * nbr_indents}</{PREFIX}{curr_flag}>\n")
        else:
            out_file.write(flag + "/>\n")

    return


class DynawoSerializerJOBS(Serializer):
    """_summary_
    Serializer for .jobs files.
    """

    xml_version = "1.0"
    encoding = "UTF-8"

    def __init__(self, _object_to_serialize: ObjectToSerialize):
        super().__init__(_object_to_serialize)

    def serialize(self) -> None:
        with open(
            self.object_to_serialize.output_path, "w", encoding="utf-8"
        ) as out_file:
            out_file.write(
                f'<?xml version="{self.xml_version}" encoding="{self.encoding}"?>\n'
            )
            xml_serializer(
                out_file,
                self.object_to_serialize.object_to_serialize,
                0,
                flag_hierarchy["root"],
            )
