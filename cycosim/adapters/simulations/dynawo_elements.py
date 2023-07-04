from enum import Enum

from typing import List

from dataclasses import dataclass

from cycosim.domain.ports.external_elements import ExternalElement


class DynawoParameterType(str, Enum):
    DOUBLE = "DOUBLE"
    BOOL = "BOOL"
    INT = "INT"
    STRING = "STRING"


@dataclass
class DynawoParameter:
    name: str
    dynawo_type: DynawoParameterType
    value: float | bool | int


@dataclass
class DynawoReference:
    name: str
    dynawo_type: DynawoParameterType
    original_data_source: str
    original_name: str


class DynawoParameterSet(ExternalElement):
    def __init__(self):
        self.id: str = None
        self.parameters: dict(DynawoParameter) = {}
        self.references: dict(DynawoReference) = {}

    def get_parameter(self, param_name: str):
        """_summary_
        Search in the dict of paramater if there is one with the name 'param_name'.
        Returns the parameter object if it exists, return None otherwise.
        Args:
            param_name (str): The name of the parameter

        Returns:
            DynawoParameter : The parameter
            None : If the parameter does not exist
        """
        if param_name in self.parameters:
            return self.parameters[param_name]
        return None

    def add_parameter(self, name: str, dynawo_type: str, value: float | bool | int):
        """_summary_
        Add a new DynawoParameter object to the dictionary of parameters.
        If the used name already exists, it is replaced by the new one.

        Args:
            name (str): The name of the parameter
            dynawo_type (DynawoParameterType): the dynawo type of the parameter
            value (float | bool | int): the value of the parameter
        """
        self.parameters[name] = DynawoParameter(name, DynawoParameterType(dynawo_type), value)

    def get_reference(self, ref_name: str):
        """_summary_
        Search in the dict of paramater if there is one with the name 'param_name'.
        Returns the parameter object if it exists, return None otherwise.
        Args:
            param_name (str): The name of the parameter

        Returns:
            DynawoParameter : The parameter
            None : If the parameter does not exist
        """
        if ref_name in self.references:
            return self.references[ref_name]
        return None

    def add_reference(
        self,
        name: str,
        dynawo_type: str,
        data_source: str,
        original_name: str,
    ):
        """_summary_
        Add a new DynawoReference object to the dictionary of references.
        If the used name already exists, it is replaced by the new one.

        Args:
            name (str): The name of the reference
            dynawo_type (DynawoParameterType): the dynawo type of the reference
            data_source (str): The original data source of the reference
            original_name (str): The original name of the referencee
        """
        self.references[name] = DynawoReference(name, DynawoParameterType(dynawo_type), data_source, original_name)


class Curve:
    """_summary_
    Class representanting a curve to plot at the end of a cosimulation
    """

    def __init__(self, _model: str, _variable: str):
        self.model = _model
        self.variable = _variable


class DynawoCurves(ExternalElement):
    """_summary_
    Class representing the information contained in a .crv file.
    This class stores the information about the curves we want to plot at the end of the cosimulation
    Args:
        curves (list(Curves)) : List of Curve object.
        xmlns : namespace of the attributed curve file.
    """

    curves: List[Curve] = None
    xmlns: str = "http://www.rte-france.com/dynawo"

    def __init__(self):
        self.curves = []

    def add_curve(self, model: str, variable: str):
        self.curves.append(Curve(model, variable))
