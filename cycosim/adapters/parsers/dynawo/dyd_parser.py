import xmltodict

from cycosim.domain.ports.parser import Parser, ParsedFileObject  # noqa

from cycosim.domain.models.power_system import (
    DynamicComponent,
    Connection,
    Connector,
    StaticReference,
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


class DynawoParserDYD(Parser):
    def __init__(
        self,
        _file_to_parse,
    ):
        from .dynawo_files import DYDObject

        super().__init__(_file_to_parse)
        self.parsed_file_obj = DYDObject()

    def parse(self) -> ParsedFileObject:
        with open(self.file_to_parse, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            xml_dict = xml_dict["dyn:dynamicModelsArchitecture"]
            xml_dict.pop("@xmlns:dyn", None)
            parse_xml(xml_dict, self.parsed_file_obj.dynamic_model.components)

        return self.parsed_file_obj
