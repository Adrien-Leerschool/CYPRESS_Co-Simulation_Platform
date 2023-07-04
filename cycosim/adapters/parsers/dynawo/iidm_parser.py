import xmltodict

from cycosim.utils import remove_superfluous

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
    "currentLimits1": Component,
    "currentLimits2": Component,
}


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
        if isinstance(val, dict):
            curr_cpnt = flag_iidm_mapping[key]()
            xml_parser(val, curr_cpnt)
            curr_cpnt.flag_name = key
            parent_cpnt.add_sub_component(curr_cpnt)

        elif isinstance(val, list):
            for elem in val:
                curr_cpnt = flag_iidm_mapping[key]()
                curr_cpnt.flag_name = key
                xml_parser(elem, curr_cpnt)
                parent_cpnt.add_sub_component(curr_cpnt)

        elif not hasattr(parent_cpnt, key):
            parent_cpnt.info[key] = val

        elif isinstance(val, str):
            setattr(parent_cpnt, var_iidm_mapping[key], val)
        else:
            print("Error : Unknow type of " + str(val))


class DynawoParserIIDM:
    """
    Summary :
        A class used to parse iidm or xiidm files.

    Attributes :
        See parent classes.
    """

    def __init__(self, _iidm_file):
        self.iidm_file = _iidm_file
        self.network = Component()

    def parse(self) -> Component:
        with open(self.iidm_file, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            self.network.flag_name = "network"
            xml_parser(
                remove_superfluous(xml_dict["iidm:network"], ["iidm:", "@"]),
                self.network,
            )

        return self.network
