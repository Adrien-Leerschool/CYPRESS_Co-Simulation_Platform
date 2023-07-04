import os

from cycosim.domain.ports.parser import Parser, ParsedFileObject
from cycosim.domain.ports.files import FileType

from cycosim.adapters.parsers import UnknownFileFormatError
from cycosim.adapters.parsers.dynawo import (
    DynawoParserDYD,
    DynawoParserIIDM,
    DynawoParserJOBS,
    DynawoParserPAR,
    DynawoParserCRV,
)

from cycosim.adapters.parsers.helics import HelicsParserXML


def check_file_validity(file) -> None:
    """_summary_
    Check the validity of the given file.
    Raise a UnknownFileFormatError if the file is not compliant.
    """

    # Check if the file exists
    if not os.path.isfile(file):
        raise FileNotFoundError(f"The given file does not exist ({file})")

    # Check if the file has an extension
    file_name = file.split("/")[-1]
    splitted_file = file_name.split(".")
    if len(splitted_file) < 2:
        raise UnknownFileFormatError(f"The given file does not have any extension ('{file}').")

    # Check if the file does not contain mulitple dots in its name
    elif len(splitted_file) > 2:
        raise UnknownFileFormatError(
            f"The name of the given file contains more than one dot ('{file}')."
            " It must contain only one for the extension."
        )

    # Check if the extension of the given file is a known one
    if splitted_file[-1] not in set(item.value for item in FileType):
        raise UnknownFileFormatError(
            f"The given file extension is not known ('.{splitted_file[-1]}')."
            f" The available extensions are : {['.'+item.value for item in FileType]}"
        )


class DynawoGlobalParser(Parser):
    def __init__(self, _file_to_parse: str):
        super().__init__(_file_to_parse)
        check_file_validity(_file_to_parse)
        extension = self.file_to_parse.split(".")[-1]
        self.file_type = FileType(extension)

    def parse(self):
        """
        Summary :
            Implementation of the abstract method from the parent class Parser.
        """
        parser = None

        if self.file_type == FileType.IIDM or self.file_type == FileType.XIIDM:
            parser = DynawoParserIIDM(self.file_to_parse)
        elif self.file_type == FileType.DYD:
            parser = DynawoParserDYD(self.file_to_parse)
        elif self.file_type == FileType.PAR:
            parser = DynawoParserPAR(self.file_to_parse)
        elif self.file_type == FileType.JOBS:
            parser = DynawoParserJOBS(self.file_to_parse)
        elif self.file_type == FileType.CRV:
            parser = DynawoParserCRV(self.file_to_parse)
        else:
            raise UnknownFileFormatError(f"DynawoParser : Unknown file extension '{self.file_type}'.")

        return parser.parse()


class HelicsGlobalParser(Parser):
    def __init__(self, _file_to_parse: str):
        super().__init__(_file_to_parse)
        check_file_validity(_file_to_parse)
        extension = self.file_to_parse.split(".")[-1]
        self.file_type = FileType(extension)

    def parse(self) -> ParsedFileObject:
        """
        Summary :
            Implementation of the abstract method from the parent class Parser.
        """
        parser = None

        if self.file_type == FileType.XML:
            parser = HelicsParserXML(self.file_to_parse)
        else:
            raise UnknownFileFormatError(f"HelicsParser : Unknown file extension '{self.file_type}'.")

        return parser.parse()
