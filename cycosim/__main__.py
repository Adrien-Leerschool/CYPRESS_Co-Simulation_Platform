"""
Entry point of CYPRESS Co-Simulation Platform
"""
import os
import argparse
from cycosim.services.input_files import DynawoGlobalParser, HelicsGlobalParser
from cycosim.domain.ports import FileType, Simulation
from cycosim.adapters.simulations import DynawoSimulation


def make_valid_path(path: str):
    if path.startswith("./"):
        return os.path.join(os.getcwd(), path[2:])
    elif path.startswith("/"):
        return os.path.join(os.getcwd(), path[1:])
    else:
        return os.path.join(os.getcwd(), path)


def configure_dynawo_simulation(args) -> Simulation:
    """_summary_
    Takes as input the arguments given in the command line, checks the validity of
    dynawo related files and parse them to return a DynawoSimulation object.
    """
    simulation = DynawoSimulation()

    # Retrieve the JOBS data
    parser = None
    if args.jobs is not None:
        if os.path.exists(os.path.join(args.dynawo_data, args.jobs)):
            parser = DynawoGlobalParser(os.path.join(args.dynawo_data, args.jobs))
        else:
            raise FileNotFoundError(f"The given jobs file '{args.jobs}' does not exist.")

    else:
        for file in os.listdir(args.dynawo_data):
            if file.endswith(".jobs"):
                parser = DynawoGlobalParser(os.path.join(args.dynawo_data, file))

    if parser is None:
        raise FileNotFoundError(
            f"No JOBS file in the directory '{args.dynawo_data}'."
            "You can specify the folder where the dynawo files are with "
            "the flag '--dynawo_data path/to/dynawo/files/'."
        )
    simulation.simulation_parameters = parser.parse()

    # Retrieve the two other mandatory files (PAR, DYD)
    file_types = [FileType.PAR, FileType.DYD]
    jobs_param = [
        simulation.simulation_parameters.solver_parameter_file,
        simulation.simulation_parameters.dyd_file,
    ]

    for cnt, f_t, j_p in zip(range(len(file_types)), file_types, jobs_param):
        if j_p is None:
            raise AttributeError(f"No {f_t} file has been specified in the JOBS file.")

        if not os.path.exists(os.path.join(args.dynawo_data, j_p)):
            raise FileNotFoundError(f"The specified {f_t} file does not exist in folder '{args.dynawo_data}'.")

        if f_t == FileType.PAR:
            simulation.parameter_sets = DynawoGlobalParser(os.path.join(args.dynawo_data, j_p)).parse()
        elif f_t == FileType.DYD:
            simulation.dynamic_network = DynawoGlobalParser(os.path.join(args.dynawo_data, j_p)).parse()

    # Check for non-mandatory files and parse them
    file_types = [FileType.IIDM, FileType.CRV]
    jobs_param = [
        simulation.simulation_parameters.iidm_file,
        simulation.simulation_parameters.crv_file,
    ]

    for cnt, f_t, j_p in zip(range(len(file_types)), file_types, jobs_param):
        if j_p is None:
            continue

        if not os.path.exists(os.path.join(args.dynawo_data, j_p)):
            raise FileNotFoundError(f"The specified {f_t} file does not exist in folder '{args.dynawo_data}'.")

        if f_t == FileType.IIDM:
            simulation.static_network = DynawoGlobalParser(os.path.join(args.dynawo_data, j_p)).parse()

        elif f_t == FileType.CRV:
            simulation.curves = DynawoGlobalParser(os.path.join(args.dynawo_data, j_p)).parse()

    # Create the folder where we gonna store the serialized files used for the dynawo simulation
    out_path = os.path.join(args.dynawo_data, "serialized/")
    if not os.path.exists(out_path):
        os.makedirs(out_path)

    return simulation


def configure_helics_cosimulation(args, pw_sys_sim: Simulation):
    # Retrieve the Helics data
    parser = None
    for file in os.listdir(args.helics_data):
        if file.endswith(".xml"):
            parser = HelicsGlobalParser(os.path.join(args.helics_data, file))

    if parser is None:
        raise FileNotFoundError(
            f"No XML file in the directory '{args.helics_data}'."
            "You can specify the folder where the helics files are with "
            "the flag '--helics_data path/to/helics/files/'."
        )
    cosimulation = parser.parse()

    # Attach the power system simulation to the cosimulation
    cosimulation.power_system_federate.simulation = pw_sys_sim

    return cosimulation


def main(args):
    if args.pow_sys_sim == "dynawo":
        args.dynawo_data = make_valid_path(args.dynawo_data)
        pw_sys_sim = configure_dynawo_simulation(args)

    if args.cosim == "helics":
        args.helics_data = make_valid_path(args.helics_data)
        cosimulation = configure_helics_cosimulation(args, pw_sys_sim)
        cosimulation.power_system_federate.name = "Dynawo"

    cosimulation.initialize(
        {"output_path": os.path.join(args.dynawo_data, "serialized/"), "cosimulation": cosimulation}
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--cosim",
        help="Name of the cosimulator to use, option are : helics",
        default="helics",
    )
    parser.add_argument(
        "--cyber_sim",
        help="Name of the simulator to use for the cyber simulation part, option are : ",
        default="omnet",
    )
    parser.add_argument(
        "--pow_sys_sim",
        help="Name of the cosimulator to use for the power system simulation part, option are : dynawo",
        default="dynawo",
    )

    parser.add_argument(
        "--dynawo_data",
        help="The path to the folder where all the needed Dynawo files are.",
        default=".",
    )
    parser.add_argument(
        "--jobs",
        help="The name of the jobs file to use in the folder speicfied "
        "by the '--dynawo_data' flag. If not specified, the first .jobs "
        "file is used.",
    )

    parser.add_argument(
        "--helics_data",
        help="The path to the folder where all the needed Helics files are.",
        default=".",
    )

    parser.add_argument(
        "--con",
        help="Path to the connection file for linking variables in the cosimulation.",
    )
    args = parser.parse_args()
    main(args)
