import abc

from cycosim.domain.ports.files import FileType


class ObjectToSerialize:
    """
    Summary :
        Generic class to manipulate the objects that need to be serialized.

    Attributes :
        target_file_type: The target file extension.
        output_path : The path where the file must be store.
        object_to_serialize :
    """

    def __init__(
        self, _target_file_type: FileType, _output_path: str, _object_to_serialize
    ):
        self.target_file_type = _target_file_type
        self.output_path = _output_path
        self.object_to_serialize = _object_to_serialize


class Serializer(abc.ABC):
    """
    Summary :
        Abstract class that every serializer must inherit from.

    Attributes :
        file_to_serialize : A pointer to a ObjectToSerialize object that contains the object to serialize.
    """

    object_to_serialize: ObjectToSerialize

    def __init__(self, _object_to_serialize: ObjectToSerialize):
        self.object_to_serialize = _object_to_serialize

    @abc.abstractmethod
    def serialize(self) -> None:
        """
        Summary :
            Take the object stored in the object_to_serialize variable and create the suitable output file at the
            given output_path.
        """
        pass
