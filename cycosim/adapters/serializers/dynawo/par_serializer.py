import io

INDENT = "  "


def xml_serializer(out_file: io.TextIOWrapper, set_list: list):
    for st in set_list:
        out_file.write(f'{INDENT * 1}<set id="{st.id}">\n')
        for _, par in st.parameters.items():
            out_file.write(f'{INDENT * 2}<par type="{par.dynawo_type.name}" name="{par.name}" value="{par.value}"/>\n')

        for _, ref in st.references.items():
            out_file.write(
                f'{INDENT * 2}<reference name="{ref.name}" origData="{ref.original_data_source}" '
                f'origName="{ref.original_name}" type="{ref.dynawo_type.name}"/>\n'
            )

        out_file.write(f"{INDENT * 1}</set>\n")


class DynawoSerializerPAR:
    """_summary_
    Serializer for .jobs files.
    """

    xml_version = "1.0"
    encoding = "UTF-8"

    def __init__(self, _output_path: str, _obj_param):
        self.output_path = _output_path
        self.obj_param = _obj_param

    def serialize(self) -> None:
        with open(self.output_path, "w", encoding="utf-8") as out_file:
            out_file.write(
                f'<?xml version="{self.xml_version}" encoding="{self.encoding}"?>\n'
                '<parametersSet xmlns="http://www.rte-france.com/dynawo">\n'
            )
            xml_serializer(out_file, self.obj_param)
            out_file.write("</parametersSet>")
