import io

from cycosim.domain.models.power_system import Component


iidm_var_mapping = {
    "xml_version": "version",
    "xml_encoding": "encoding",
    "id": "id",
    "nominal_V": "nominalV",
    "V": "v",
    "phase": "angle",
    "active_power_min": "minP",
    "active_power_max": "maxP",
    "is_voltage_regulation": "voltageRegulatorOn",
    "active_power_target": "targetP",
    "voltage_magnitude_target": "targetV",
    "reactive_power_target": "targetQ",
    "bus": "bus",
    "reactive_power_min": "minQ",
    "reactive_power_max": "maxQ",
    "active_power_output": "p",
    "reactive_power_output": "q",
    "active_power_setpoint": "p0",
    "reactive_power_setpoint": "q0",
    "resistance": "r",
    "reactance": "x",
    "bus_1": "bus1",
    "bus_2": "bus2",
    "half_shunt_susceptance": "b1",
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


def xml_cpnt_serialize(out_file: io.TextIOWrapper, cpnt: Component, nbr_tabs: int):
    """_summary_
    Recursively writes to the out_file stream the component and its sub-components.
    The components are written under the xml format.

    Args:
        out_file (io.TextIOWrapper): The stream where the to write
        cpnt (Component): The component currently serialized
        nbr_tabs (int): The current tabulation for the xml file.
    """
    out_file.write("\t" * nbr_tabs)
    out_file.write(f"<iidm:{cpnt.flag_name}")
    if cpnt.id is not None:
        out_file.write(f' id="{cpnt.id}"')

    # Add the available variables.
    for key, val in cpnt.__dict__.items():
        if key in iidm_var_mapping.keys() and key != "id":
            out_file.write(f' {iidm_var_mapping[key]}="{val}"')

    # If there is variables in the infos dictionary, add them.
    for key, val in cpnt.info.items():
        out_file.write(f' {key}="{val}"')

    if cpnt.sub_components:
        out_file.write(">\n")
    else:
        out_file.write("/>\n")

    # Recursively write the sub-components to the file.
    if cpnt.sub_components:
        for sub_cpnt in cpnt.sub_components:
            xml_cpnt_serialize(out_file, sub_cpnt, nbr_tabs + 1)
        out_file.write("\t" * nbr_tabs)
        out_file.write(f"</iidm:{cpnt.flag_name}>\n")


class DynawoSerializerIIDM:
    """_summary_
    Serializer for .iidm and .xiidm files.
    """

    xml_version = "1.0"
    encoding = "UTF-8"

    def __init__(self, _output_path: str, _obj_stat_model):
        self.output_path = _output_path
        self.obj_stat_model = _obj_stat_model

    def serialize(self) -> None:
        with open(self.output_path, "w", encoding="utf-8") as out_file:
            out_file.write(f'<?xml version="{self.xml_version}" encoding="{self.encoding}"?>\n')
            xml_cpnt_serialize(out_file, self.obj_stat_model, 0)
