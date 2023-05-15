from cycosim.domain.models.power_system import PowerSystemModel


class DynawoModel(PowerSystemModel):
    def __init__(self):
        self.parameters_set = {}
        self.dynamic_architecture = {}
        self.jobs = {}
