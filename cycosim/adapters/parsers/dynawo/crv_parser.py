import xmltodict

from cycosim.adapters.simulations.dynawo_elements import DynawoCurves

from cycosim.utils import remove_superfluous


def parse_xml(xml_dict: dict, curves: DynawoCurves):
    for key, val in xml_dict.items():
        if key == "curve":
            if isinstance(val, list):
                for curve in val:
                    curves.add_curve(curve["model"], curve["variable"])

            elif isinstance(val, dict):
                curves.add_curve(val["model"], val["variable"])


class DynawoParserCRV:
    def __init__(self, _crv_file: str):
        self.crv_file = _crv_file
        self.curves = DynawoCurves()

    def parse(self) -> DynawoCurves:
        with open(self.crv_file, "rb") as xml_data:
            xml_dict = remove_superfluous(xmltodict.parse(xml_data), ["@"])
            self.curves.xmlns = xml_dict["curvesInput"]["xmlns"]
            parse_xml(xml_dict["curvesInput"], self.curves)

        return self.curves
