"""
Entry point of CYPRESS Co-Simulation Platform
"""
import sys

from cycosim.services.input_files import GlobalParser

from cycosim.services.output_files import GlobalSerializer

from cycosim.domain.models.power_system import *  # noqa

from cycosim.domain.ports import ObjectToSerialize, Simulation

from cycosim.adapters.simulations import DynawoSimulation


# OUTPUT_PATH = "/Users/adrienleerschool/Documents/Cypress/Docker/CYPRESS_Co-Simulation_Platform/data/serialized/"
# FILE_PATH = "/Users/adrienleerschool/Documents/Cypress/Docker/CYPRESS_Co-Simulation_Platform/data/RBTS/"

OUTPUT_PATH = "/CYPRESS_Co-Simulation_Platform/data/RBTS/serialized/"
FILE_PATH = "/CYPRESS_Co-Simulation_Platform/data/RBTS/"


def print_structure(node: Component, indent=""):  # noqa
    print(indent + str(node.__class__))
    for cpnt in node.sub_components:
        print_structure(cpnt, indent + "\t")


def main(args):
    simulation: Simulation = DynawoSimulation()

    # Parse .iidm file
    parser = GlobalParser(FILE_PATH + "RBTS_ACOPF_peak_L3_shunt_typical.iidm")
    parsed_obj = parser.parse()
    simulation.add_element(parsed_obj.get_object())
    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "RBTS_ACOPF_peak_L3_shunt_typical.iidm",
        parsed_obj.get_object(),
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()

    # Parse .jobs file
    parser = GlobalParser(FILE_PATH + "RBTS.jobs")
    parsed_obj = parser.parse()
    simulation.add_element(parsed_obj.get_object())
    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "RBTS.jobs",
        parsed_obj.get_object(),
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()

    # Parse .dyd file
    parser = GlobalParser(FILE_PATH + "RBTS.dyd")
    parsed_obj = parser.parse()
    simulation.add_element(parsed_obj.get_object())
    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "RBTS.dyd",
        parsed_obj.get_object(),
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()

    # Parse .par file
    parser = GlobalParser(FILE_PATH + "RBTS.par")
    parsed_obj = parser.parse()
    simulation.add_element(parsed_obj.get_object())
    obj_to_serialize = ObjectToSerialize(
        parsed_obj.parsed_file_type,
        OUTPUT_PATH + "RBTS.par",
        parsed_obj.get_object(),
    )
    serializer = GlobalSerializer(obj_to_serialize)
    serializer.serialize()


if __name__ == "__main__":
    main(sys.argv)
