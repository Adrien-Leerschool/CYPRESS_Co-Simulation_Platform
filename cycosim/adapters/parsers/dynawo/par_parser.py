import xmltodict
from enum import Enum
from typing import List
from cycosim.utils import remove_superfluous

from cycosim.domain.ports.parser import Parser, ParsedFileObject

from cycosim.adapters.simulations import DynawoParameterSet

jobs_to_var_mapping = {
    "LOAD-4_isControllable": "LOAD_4_isControllable",
    "origData": "original_data_source",
    "origName": "original_name",
}


class PARFlags(str, Enum):
    SET = "set"
    PAR = "par"
    REFERENCE = "reference"


def set_parser(set_dict: dict):
    new_set = DynawoParameterSet()
    new_set.id = set_dict["id"]

    if PARFlags.PAR in set_dict:
        if isinstance(set_dict["par"], list):
            for param in set_dict["par"]:
                new_set.add_parameter(param["name"], param["type"], param["value"])

        else:
            new_set.add_parameter(
                set_dict["par"]["name"],
                set_dict["par"]["type"],
                set_dict["par"]["value"],
            )

    if PARFlags.REFERENCE in set_dict:
        if isinstance(set_dict["reference"], list):
            for ref in set_dict["reference"]:
                new_set.add_reference(
                    ref["name"], ref["type"], ref["origData"], ref["origName"]
                )

        else:
            new_set.add_reference(
                set_dict["reference"]["name"],
                set_dict["reference"]["type"],
                set_dict["reference"]["origData"],
                set_dict["reference"]["origName"],
            )

    return new_set


def xml_parser(xml_dict: dict, parameters: List[DynawoParameterSet]):
    for set in xml_dict["set"]:
        parameters.append(set_parser(set))


class DynawoParserPAR(Parser):
    """_summary_
    A class used to parse Dynawo .par files.
    Returns a ParsedFileObject if everything went correctly.

    """

    def __init__(self, _file_to_parse):
        super().__init__(_file_to_parse)
        from .dynawo_files import PARObject

        self.parsed_file_obj = PARObject()

    def parse(self) -> ParsedFileObject:
        with open(self.file_to_parse, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            clean_dict = remove_superfluous(xml_dict, ["@", "dyn:"])
            clean_dict = clean_dict["parametersSet"]
            clean_dict.pop("xmlns", None)
            xml_parser(clean_dict, self.parsed_file_obj.parameters)

        return self.parsed_file_obj
