"""
Entry point of CYPRESS Co-Simulation Platform
"""
import sys

from cycosim.services.input_files import GlobalParser

from cycosim.services.output_files import GlobalSerializer

from cycosim.domain.models.power_system import *  # noqa

from cycosim.domain.ports import ObjectToSerialize

from cycosim.domain.ports.files import FileType


OUTPUT_PATH = "/Users/adrienleerschool/Documents/Cypress/CYPRESS_Co-Simulation_Platform/cycosim_dev/"
FILE_PATH = "/Users/adrienleerschool/Documents/Cypress/CYPRESS_Co-Simulation_Platform/data/RBTS/"


def print_structure(node: Component, indent=""):  # noqa
    print(indent + str(node.__class__))
    for cpnt in node.sub_components:
        print_structure(cpnt, indent + "\t")


def main(args):
    # Parse .iidm file
    parser = GlobalParser(FILE_PATH + "RBTS_ACOPF_peak.iidm")
    parsed_obj = parser.parse()

    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "test_file.iidm",
        parsed_obj.parsed_file_object,
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()

    network = parsed_obj.parsed_file_object

    # Parse .jobs file
    parser = GlobalParser(FILE_PATH + "RBTS.jobs")
    parsed_obj = parser.parse()
    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "test_jobs_file.jobs",
        parsed_obj.jobs,
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()

    # Parse .dyd file
    parser = GlobalParser(FILE_PATH + "RBTS.dyd")
    if parser.file_type == FileType.DYD:
        print("hey")
        parser.parse(network)


if __name__ == "__main__":
    main(sys.argv)
