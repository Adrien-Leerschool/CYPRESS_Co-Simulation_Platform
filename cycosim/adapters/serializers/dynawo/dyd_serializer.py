import io

from cycosim.domain.ports import Serializer, ObjectToSerialize

from cycosim.domain.models.power_system import (
    DynamicComponent,
    Connection,
    Connector,
)


def xml_serialize(out_file: io.TextIOWrapper, cpnt_list: list):
    for elem in [e for e in cpnt_list if isinstance(e, DynamicComponent)]:
        out_file.write(
            f'  <dyn:blackBoxModel id="{elem.id}" lib="{elem.library}" '
            f'parFile="{elem.parameter_file}" parId="{elem.parameter_id}"'
        )
        if elem.static_id is not None:
            out_file.write(f' staticId="{elem.static_id}"')

        if elem.static_references:
            out_file.write(">\n")
            for sr in elem.static_references:
                out_file.write(
                    f'    <dyn:staticRef var="{sr.variable}" staticVar="{sr.static_variable}"/>\n'
                )
            out_file.write("  </dyn:blackBoxModel>\n")

        else:
            out_file.write("/>\n")

    for elem in [e for e in cpnt_list if isinstance(e, Connector)]:
        out_file.write(f'  <dyn:macroConnector id="{elem.id}"')

        if elem.connections:
            out_file.write(">\n")
            for c in elem.connections:
                out_file.write(
                    f'    <dyn:connect var1="{c.variable_1}" var2="{c.variable_2}"/>\n'
                )
            out_file.write("  </dyn:macroConnector>\n")

        else:
            out_file.write("/>\n")

    for elem in [e for e in cpnt_list if isinstance(e, Connection)]:
        if elem.is_macro:
            out_file.write(
                f'  <dyn:macroConnect connector="{elem.connector}" id1="{elem.id_1}" '
                f'index1="{elem.index_1}" id2="{elem.id_2}"/>\n'
            )
        else:
            out_file.write(
                f'  <dyn:connect id1="{elem.id_1}" var1="{elem.variable_1}" '
                f'id2="{elem.id_2}" var2="{elem.variable_2}"/>\n'
            )


class DynawoSerializerDYD(Serializer):
    """_summary_
    Serializer for .dyd files.
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
            out_file.write(
                '<dyn:dynamicModelsArchitecture xmlns:dyn="http://www.rte-france.com/dynawo">\n'
            )
            xml_serialize(
                out_file, self.object_to_serialize.object_to_serialize.components
            )
            out_file.write("</dyn:dynamicModelsArchitecture>")
