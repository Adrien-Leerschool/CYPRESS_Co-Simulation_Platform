from enum import Enum


class FileType(str, Enum):
    IIDM = "iidm"
    XIIDM = "xiidm"
    DYD = "dyd"
    PAR = "par"
    JOBS = "jobs"
    JSON = "json"
    XML = "xml"
    CRV = "crv"


class InputFile(str, Enum):
    SIMULATION_SETTINGS = "simulation_settings"
    GRID_STATIC_DATA = "grid_static_data"
    GRID_DYNAMIC_DATA = "grid_dynamic_data"
    SIMULATION_PARAMETERS = "simulation_parameters"
