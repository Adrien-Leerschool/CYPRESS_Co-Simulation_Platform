from cycosim.domain.ports import ParsedFileObject, FileType

from cycosim.domain.models.power_system import Component


class IIDMObject(ParsedFileObject):
    """_summary_
    IIDMObject represents any .iidm & .xiidm file that has been parsed.

    Attributes :
        network : The variables that stores the network.
    """

    def __init__(self):
        self.network = Component()
        super().__init__(
            _parsed_file_type=FileType.IIDM, _parsed_file_object=self.network
        )


class JOBSObject(ParsedFileObject):
    """_summary_
    JOBSObject represents any .jobs file that has been parsed.

    Attributes :
        jobs : The list of job intended for the Dynawo simulator.
    """

    def __init__(self):
        self.jobs = {}
        super().__init__(_parsed_file_type=FileType.JOBS, _parsed_file_object=self.jobs)


class DYDObject(ParsedFileObject):
    """_summary_
    DYDSObject represents any .dyd file that has been parsed.

    Attributes :
        jobs : The list of job intended for the Dynawo simulator.
    """

    def __init__(self, _network: Component):
        self.network = _network
        super().__init__(
            _parsed_file_type=FileType.DYD, _parsed_file_object=self.network
        )
