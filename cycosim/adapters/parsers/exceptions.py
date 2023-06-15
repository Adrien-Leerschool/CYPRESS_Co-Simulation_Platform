class UnknownFileFormatError(Exception):
    """Exception raised when the given input file is not known"""

    pass


class AttributeNotFoundError(Exception):
    """Exception raised when we try to set the value of unexisting attribute"""

    pass
