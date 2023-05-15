import xmltodict

from cycosim.domain.ports.parser import Parser, ParsedFileObject

from cycosim.adapters.parsed_files import JOBSObject


def rmv_superfluous(xml_dict):
    light_dict = {}
    for key, val in xml_dict.items():
        if isinstance(val, dict):
            light_dict[key.replace("dyn:", "").replace("@", "")] = rmv_superfluous(val)
        else:
            light_dict[key.replace("dyn:", "").replace("@", "")] = val
    return light_dict


class DynawoParserJOBS(Parser):
    """
    Summary :
        A class used to parse .jobs files.
        Returns a ParsedFileObject if everything went correctly.

    """

    def __init__(self, _file_to_parse):
        super().__init__(_file_to_parse)
        self.parsed_file_obj = JOBSObject()

    def parse(self) -> ParsedFileObject:
        with open(self.file_to_parse, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            self.parsed_file_obj.jobs = rmv_superfluous(xml_dict)

        return self.parsed_file_obj
