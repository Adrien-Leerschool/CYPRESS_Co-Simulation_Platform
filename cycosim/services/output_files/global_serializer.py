from cycosim.domain.ports import Serializer, ObjectToSerialize, FileType

from cycosim.adapters.serializers.dynawo import (
    DynawoSerializerIIDM,
    DynawoSerializerJOBS,
    DynawoSerializerDYD,
    DynawoSerializerPAR,
)

from cycosim.adapters.parsers import UnknownFileFormatError


class GlobalSerializer(Serializer):
    def __init__(self, _object_to_serialize: ObjectToSerialize):
        super().__init__(_object_to_serialize)

    def serialize(self):
        serializer = None

        if self.object_to_serialize.target_file_type == FileType.IIDM:
            serializer = DynawoSerializerIIDM(self.object_to_serialize)

        elif self.object_to_serialize.target_file_type == FileType.JOBS:
            serializer = DynawoSerializerJOBS(self.object_to_serialize)

        elif self.object_to_serialize.target_file_type == FileType.DYD:
            serializer = DynawoSerializerDYD(self.object_to_serialize)

        elif self.object_to_serialize.target_file_type == FileType.PAR:
            serializer = DynawoSerializerPAR(self.object_to_serialize)

        else:
            raise UnknownFileFormatError(
                f"Unknown targeted output file type '{self.object_to_serialize.target_file_type}'."
            )

        serializer.serialize()
