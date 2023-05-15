from cycosim.domain.models.power_system import Component


class PowerSystemModel:
    """_summary_
    A class gathering all the necessary elements for creating a power system simulation.

    Attributes:
        network : The highest component of the simulated network, containing all the sub components.
    """

    network: Component


class StaticPowerSystemModel(PowerSystemModel):
    pass


class DynamicPowerSystemModel(PowerSystemModel):
    pass
