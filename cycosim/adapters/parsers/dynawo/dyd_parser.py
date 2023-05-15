import xmltodict

from cycosim.domain.ports.parser import Parser, ParsedFileObject  # noqa

from cycosim.adapters.parsed_files import DYDObject

from cycosim.domain.models.power_system import Component


class DynawoParserDYD(Parser):
    def __init__(self, _file_to_parse, network: Component):
        super().__init__(_file_to_parse)
        self.parsed_file_obj = DYDObject(network)

    def parse(self) -> ParsedFileObject:
        with open(self.file_to_parse, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            print(xml_dict)

        return self.parsed_file_obj
