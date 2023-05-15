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
                elem.get_sub_components(component_id)

        return None


class Bus(Component):
    V: float
    nominal_V: float
    phase: float
    is_slack_bus: bool


class Switch(Component):  # or Breaker ?
    is_open: bool
    voltage_level: str


class Generator(Component):
    bus: Bus
    active_power_min: float
    active_power_max: float
    reactive_power_min: float
    reactive_power_max: float
    is_voltage_regulation: bool
    active_power_target: float
    voltage_magnitude_target: float
    reactive_power_target: float
    active_power_output: float
    reactive_power_output: float
    rated_nominal_apparent_power: float


class Load(Component):
    bus: Bus
    active_power_setpoint: float
    reactive_power_setpoint: float
    active_power_output: float
    reactive_power_output: float


class ShuntCompensator(Component):
    bus: Bus
    susceptance: float


class StaticVARCompensator(Component):
    bus: Bus
    reactive_power_setpoint: float


class Edge(Component):
    bus_1: Bus
    bus_2: Bus


class ACLine(Edge):
    resistance: float
    reactance: float
    half_shunt_susceptance: float
    rated_nominal_apparent_power: float


class Transformer(Edge):
    resistance: float
    reactance: float
    magnetizing_susceptance: float
    rated_nominal_apparent_power: float
