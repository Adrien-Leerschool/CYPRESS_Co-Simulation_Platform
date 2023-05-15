import os

from cycosim.domain.ports.parser import Parser, ParsedFileObject
from cycosim.domain.ports.files import FileType

from cycosim.adapters.parsers import UnknownFileFormatError
from cycosim.adapters.parsers.helics import HelicsParserJSON
from cycosim.adapters.parsers.dynawo import (
    DynawoParserDYD,
    DynawoParserIIDM,
    DynawoParserJOBS,
    DynawoParserPAR,
)


class GlobalParser(Parser):
    def __init__(self, _file_to_parse: str):
        super().__init__(_file_to_parse)
        self.check_file_validity()
        extension = self.file_to_parse.split(".")[-1]
        self.file_type = FileType(extension)

    def parse(self, data_supp=None) -> ParsedFileObject:
        """
        Summary :
            Implementation of the abstract method from the parent class Parser.
        """
        parser = None

        if self.file_type == FileType.IIDM or self.file_type == FileType.XIIDM:
            parser = DynawoParserIIDM(self.file_to_parse)
        elif self.file_type == FileType.DYD:
            parser = DynawoParserDYD(self.file_to_parse, data_supp)
        elif self.file_type == FileType.PAR:
            parser = DynawoParserPAR(self.file_to_parse)
        elif self.file_type == FileType.JOBS:
            parser = DynawoParserJOBS(self.file_to_parse)
        elif self.file_type == FileType.JSON:
            parser = HelicsParserJSON(self.file_to_parse)
        else:
            raise UnknownFileFormatError(f"Unknown file extension '{self.file_type}'.")

        return parser.parse()

    def check_file_validity(self) -> None:
        """
        Summary :
            Check the validity of the given file. Raise a UnknownFileFormatError if the file is not compliant.
        """

        # Check if the file exists
        if not os.path.isfile(self.file_to_parse):
            raise FileNotFoundError(
                f"The given file does not exist ({self.file_to_parse})"
            )

        # Check if the file has an extension
        file_name = self.file_to_parse.split("/")[-1]
        splitted_file = file_name.split(".")
        if len(splitted_file) < 2:
            raise UnknownFileFormatError(
                f"The given file does not have any extension ('{self.file_to_parse}')."
            )

        # Check if the file does not contain mulitple dots in its name
        elif len(splitted_file) > 2:
            err_msg = f"The name of the given file contains more than one dot ('{self.file_to_parse}')."
            err_msg += " It must contain only one for the extension."
            raise UnknownFileFormatError(err_msg)

        # Check if the extension of the given file is a known one
        if splitted_file[-1] not in set(item.value for item in FileType):
            err_msg = f"The given file extension is not known ('.{splitted_file[-1]}')."
            err_msg += f" The available extensions are : {['.'+item.value for item in FileType]}"
            raise UnknownFileFormatError(err_msg)
