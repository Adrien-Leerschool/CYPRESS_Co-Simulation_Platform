import xmltodict

from cycosim.domain.ports import Parser, ParsedFileObject

from cycosim.adapters.parsed_files import IIDMObject

from cycosim.domain.models.power_system import (
    Component,
    Bus,
    Switch,
    Generator,
    Load,
    ACLine,
)

var_iidm_mapping = {
    "version": "xml_version",
    "encoding": "xml_encoding",
    "id": "id",
    "nominalV": "nominal_V",
    "v": "V",
    "angle": "phase",
    "minP": "active_power_min",
    "maxP": "active_power_max",
    "voltageRegulatorOn": "is_voltage_regulation",
    "targetP": "active_power_target",
    "targetV": "voltage_magnitude_target",
    "targetQ": "reactive_power_target",
    "bus": "bus",
    "minQ": "reactive_power_min",
    "maxQ": "reactive_power_max",
    "p": "active_power_output",
    "q": "reactive_power_output",
    "p0": "active_power_setpoint",
    "q0": "reactive_power_setpoint",
    "r": "resistance",
    "x": "reactance",
    "bus1": "bus_1",
    "bus2": "bus_2",
    "b1": "half_shunt_susceptance",
}


flag_iidm_mapping = {
    "network": Component,
    "substation": Component,
    "voltageLevel": Component,
    "busBreakerTopology": Component,
    "bus": Bus,
    "generator": Generator,
    "reactiveCapabilityCurve": Component,
    "point": Component,
    "load": Load,
    "switch": Switch,
    "twoWindingsTransformer": Component,
    "phaseTapChanger": Component,
    "ratioTapChanger": Component,
    "terminalRef": Component,
    "step": Component,
    "vscConverterStation": Component,
    "line": ACLine,
    "currentLimit1": Component,
    "currentLimit2": Component,
    "temporaryLimit": Component,
    "minMaxReactiveLimits": Component,
}

attributes_to_ignore = [
    "xmlns:iidm",
    "caseDate",
    "forecastDistance",
    "sourceFormat",
    "topologyKind",
    "energySource",
    "connectableBus",
    "loadType",
    "g1",
    "g2",
    "b2",
    "p1",
    "q1",
    "p2",
    "q2",
    "connectableBus1",
    "voltageLevelId1",
    "connectableBus2",
    "voltageLevelId2",
]


def xml_parser(xml_dict: dict, parent_cpnt: Component):
    """_summary_
    Recursively goes through the dictionary and instantiates the corresponding class of
    the encountered xml flags. The attributes are stored in the corresponding variables
    if there is one, they are store in the 'info' variable otherwise.

    Args:
        xml_dict (dict): The dictionary to parse.
        parent_cpnt (Component): The flag in which the current dictionary has been defined.
    """
    for key, val in xml_dict.items():
        if key.replace("@", "") in attributes_to_ignore:
            parent_cpnt.info[key.replace("@", "")] = val
            continue

        elif isinstance(val, dict):
            curr_cpnt = flag_iidm_mapping[key.replace("iidm:", "")]()
            xml_parser(val, curr_cpnt)
            curr_cpnt.flag_name = key.replace("iidm:", "")
            parent_cpnt.add_sub_component(curr_cpnt)

        elif isinstance(val, list):
            for elem in val:
                curr_cpnt = flag_iidm_mapping[key.replace("iidm:", "")]()
                curr_cpnt.flag_name = key.replace("iidm:", "")
                xml_parser(elem, curr_cpnt)
                parent_cpnt.add_sub_component(curr_cpnt)

        elif isinstance(val, str):
            setattr(parent_cpnt, var_iidm_mapping[key.replace("@", "")], val)
        else:
            print("Error : Unknow type of " + str(val))


class DynawoParserIIDM(Parser):
    """
    Summary :
        A class used to parse iidm or xiidm files.

    Attributes :
        See parent classes.
    """

    def __init__(self, _file_to_parse):
        super().__init__(_file_to_parse)
        self.parsed_file_obj = IIDMObject()

    def parse(self) -> ParsedFileObject:
        with open(self.file_to_parse, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            self.parsed_file_obj.network.flag_name = "network"
            xml_parser(xml_dict["iidm:network"], self.parsed_file_obj.network)

        return self.parsed_file_obj
