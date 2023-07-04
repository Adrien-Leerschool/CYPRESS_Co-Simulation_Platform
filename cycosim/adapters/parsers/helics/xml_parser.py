import xmltodict

from cycosim.utils import remove_superfluous

from cycosim.adapters.cosimulation import HelicsCosimulation

from cycosim.domain.ports import SimulationConnection


def xml_parser(xml_dict: dict, helics_cosim: HelicsCosimulation):
    for key, val in xml_dict["connect"].items():
        if key == "publish":
            if isinstance(val, list):
                for pub in val:
                    helics_cosim.add_connection(SimulationConnection(pub["name"], pub["variable"]))

            else:
                helics_cosim.add_connection(SimulationConnection(val["name"], val["variable"]))

    for key, val in xml_dict["cosimulationParameters"].items():
        if key == "parameter":
            if isinstance(val, list):
                for par in val:
                    if par["name"] == "period":
                        helics_cosim.power_system_federate.period = float(par["value"])

                    elif par["name"] == "log_level":
                        helics_cosim.power_system_federate.log_level = par["value"]

            else:
                if val["name"] == "period":
                    helics_cosim.power_system_federate.period = float(val["value"])

                elif val["name"] == "log_level":
                    helics_cosim.power_system_federate.log_level = val["value"]


class HelicsParserXML:

    """
    Summary :
        A class used to parse xml files containing the data related to the Helics cosimulation .

    Attributes :
        xml_file (str) : The path to the xml file to parse
        helics_cosim (helicsCosimulation) : an HelicsCosimulation object that's going to be filled with
                                            the data contained in the file.
    """

    def __init__(self, _xml_file: str):
        self.xml_file = _xml_file
        self.helics_cosim = HelicsCosimulation()

    def parse(self):
        with open(self.xml_file, "rb") as xml_data:
            xml_dict = xmltodict.parse(xml_data)
            clean_dict = remove_superfluous(xml_dict, ["hel:", "@"])
            xml_parser(clean_dict["cosimulation"], self.helics_cosim)

        return self.helics_cosim
