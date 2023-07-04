import io


def xml_serialize(out_file: io.TextIOWrapper, curves):
    for curve in curves.curves:
        out_file.write(f'  <curve model="{curve.model}" variable="{curve.variable}"/>\n')


class DynawoSerializerCRV:
    """_summary_
    Serializer for .crv files.
    """

    xml_version = "1.0"
    encoding = "UTF-8"

    def __init__(self, _output_path: str, _curves):
        self.output_path = _output_path
        self.curves = _curves

    def serialize(self) -> None:
        with open(self.output_path, "w", encoding="utf-8") as out_file:
            out_file.write(f'<?xml version="{self.xml_version}" encoding="{self.encoding}"?>\n')
            out_file.write(f'<curvesInput xmlns="{self.curves.xmlns}">\n')
            xml_serialize(out_file, self.curves)
            out_file.write("</curvesInput>")
