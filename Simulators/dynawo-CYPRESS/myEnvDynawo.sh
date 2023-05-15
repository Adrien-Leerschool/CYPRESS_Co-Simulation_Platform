#!/bin/bash
export DYNAWO_PYTHON_COMMAND=python3
export DYNAWO_HOME=/CYPRESS_Co-Simulation_Platform/Simulators/dynawo-CYPRESS
export DYNAWO_SRC_OPENMODELICA=$DYNAWO_HOME/OpenModelica/Source
export DYNAWO_INSTALL_OPENMODELICA=$DYNAWO_HOME/OpenModelica/Install
export DYNAWO_LOCALE=en_GB
export DYNAWO_RESULTS_SHOW=true
export DYNAWO_BROWSER=firefox
export DYNAWO_NB_PROCESSORS_USED=1
export DYNAWO_BUILD_TYPE=Release
$DYNAWO_HOME/util/envDynawo.sh $@