import abc

from cycosim.domain.ports import FileType


class ParsedFileObject:
    """ "
    Summary :
        Every parser must return a ParsedFileObject after having successfuly parsed a given file.
        This class encapsulates every object that is instantiated from an input file in order to
        manipulate those generically before being used.

    Attributes :
        parsed_file_type : The type of the encapsulated object.
    """

    parsed_file_type: FileType

    def __init__(self, _parsed_file_type: FileType, _parsed_file_object):
        self.parsed_file_type = _parsed_file_type
        self.parsed_file_object = _parsed_file_object


class Parser(abc.ABC):
    """
    Summary :
        Abstract class that every parser must inherit from.

    Attributes :
        file_to_parse : the full path to the file to parse.
    """

    file_to_parse: str
    parsed_file_obj: ParsedFileObject

    def __init__(self, _file_to_parse: str, _parsed_file_obj: ParsedFileObject = None):
        self.file_to_parse = _file_to_parse
        self.parsed_file_obj = _parsed_file_obj

    @abc.abstractmethod
    def parse(self) -> ParsedFileObject:
        pass
