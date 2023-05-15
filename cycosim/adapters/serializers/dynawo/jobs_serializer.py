import io

from cycosim.domain.ports import Serializer, ObjectToSerialize


def xml_dict_serialize(out_file: io.TextIOWrapper, data_dict: dict, flag_stack: list):
    first = True
    had_dict = False
    for key, val in data_dict.items():
        if isinstance(val, dict):
            flag_stack.append(key)
            had_dict = True

        if first and isinstance(val, dict):
            first = False
            if len(flag_stack) > 1:
                out_file.write(">\n" + "  " * (len(flag_stack) - 1))
            out_file.write(f"<dyn:{key}")
            xml_dict_serialize(out_file, val, flag_stack)

        elif not isinstance(val, dict):
            out_file.write(f' {key}="{val}"')

        elif not first and isinstance(val, dict):
            out_file.write("  " * len(flag_stack))
            out_file.write(f"<dyn:{key}")
            xml_dict_serialize(out_file, val, flag_stack)

    if had_dict:
        out_file.write("  " * len(flag_stack))
        if flag_stack:
            out_file.write(f"</dyn:{flag_stack.pop()}>\n")
    else:
        if flag_stack:
            flag_stack.pop()
        out_file.write("/>\n")


class DynawoSerializerJOBS(Serializer):
    """_summary_
    Serializer for .jobs files.
    """

    xml_version = "1.0"
    encoding = "UTF-8"

    def __init__(self, _object_to_serialize: ObjectToSerialize):
        super().__init__(_object_to_serialize)

    def serialize(self) -> None:
        with open(
            self.object_to_serialize.output_path, "w", encoding="utf-8"
        ) as out_file:
            out_file.write(
                f'<?xml version="{self.xml_version}" encoding="{self.encoding}"?>\n'
            )
            xml_dict_serialize(
                out_file, self.object_to_serialize.object_to_serialize, []
            )
