class Component:
    """
    Summary :
        Base class for every elements of a power system simulation.

    Attributes :
        id : The unique identifier of the component.
        info :
        flag_name :
        sub_components :
    """

    def __init__(self):
        self.id = None
        self.info = {}
        self.flag_name = None
        self.sub_components = []

    def add_sub_component(self, new_component):
        """
        Summary :
            Append a new component or set of component to the list of components.

        Args :
            cpnt : The component to add.
            parent_cpnt : If the added component is a subpart of an other component, specifies this other component.
        """
        self.sub_components.append(new_component)

    def get_sub_component(self, component_id: str):
        """
        Summary :
            Returns the Component object attached to the given id.

        Args:
            component_id : The id of the component to search.

        Returns:
            The Component object if it exists, False otherwise.
        """
        for elem in self.sub_components:
            if elem.id == component_id:
                return elem
            elif elem.sub_components:
                return elem.get_sub_component(component_id)

        return None


class Bus(Component):
    V: float = None
    nominal_V: float = None
    phase: float = None
    is_slack_bus: bool = None


class Switch(Component):  # or Breaker ?
    is_open: bool = None
    voltage_level: str = None


class Generator(Component):
    bus: Bus = None
    active_power_min: float = None
    active_power_max: float = None
    reactive_power_min: float = None
    reactive_power_max: float = None
    is_voltage_regulation: bool = None
    active_power_target: float = None
    voltage_magnitude_target: float = None
    reactive_power_target: float = None
    active_power_output: float = None
    reactive_power_output: float = None
    rated_nominal_apparent_power: float = None


class Load(Component):
    bus: Bus = None
    active_power_setpoint: float = None
    reactive_power_setpoint: float = None
    active_power_output: float = None
    reactive_power_output: float = None


class ShuntCompensator(Component):
    bus: Bus = None
    susceptance: float = None


class StaticVARCompensator(Component):
    bus: Bus = None
    reactive_power_setpoint: float = None


class Edge(Component):
    bus_1: Bus = None
    bus_2: Bus = None


class ACLine(Edge):
    resistance: float = None
    reactance: float = None
    half_shunt_susceptance: float = None
    rated_nominal_apparent_power: float = None


class Transformer(Edge):
    resistance: float = None
    reactance: float = None
    magnetizing_susceptance: float = None
    rated_nominal_apparent_power: float = None
