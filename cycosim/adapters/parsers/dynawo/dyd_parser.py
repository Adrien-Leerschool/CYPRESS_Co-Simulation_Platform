import xmltodict

from cycosim.domain.models.power_system import (
    DynamicComponent,
    Connection,
    Connector,
    StaticReference,
    DynamicModel,
)

flag_dyd_mapping = {
    "blackBoxModel": DynamicComponent,
    "staticRef": StaticReference,
    "macroConnector": Connector,
    "macroConnect": Connection,
    "connect": Connection,
}


def parse_xml(xml_dict: dict, elements: list):
    for key, val in xml_dict.items():
        if isinstance(val, dict):
            curr_cpnt = flag_dyd_mapping[key.replace("dyn:", "")]()
            curr_cpnt.parse(val)
            elements.append(curr_cpnt)

        elif isinstance(val, list):
            for elem in val:
                curr_cpnt = flag_dyd_mapping[key.replace("dyn:", "")]()
                curr_cpnt.parse(elem)
                elements.append(curr_cpnt)

        elif isinstance(val, str):
            print("Error : Attributes not managed here.")

        else:
            print("Error : Unknown type of " + str(val))


class DynawoParserDYD:
    def __init__(self, _dyd_file: str):
        self.dyd_file = _dyd_file
        self.dynamic_model = DynamicModel()

    def parse(self) -> DynamicModel:
        with open(self.dyd_file, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            xml_dict = xml_dict["dyn:dynamicModelsArchitecture"]
            xml_dict.pop("@xmlns:dyn", None)
            parse_xml(xml_dict, self.dynamic_model.components)

        return self.dynamic_model
