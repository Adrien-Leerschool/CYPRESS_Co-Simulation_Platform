# --platform=linux/arm64/v8
FROM --platform=linux/amd64 ubuntu:23.04

WORKDIR /CYPRESS_Co-Simulation_Platform

# Installing all needed dependencies
RUN apt clean
RUN apt-get update && apt-get install -y \
    cmake \
    python3 \
    libboost-dev \
    libzmq5-dev \
    git \
    cmake-curses-gui \
    g++ \
    gfortran \
    autoconf \
    pkgconf \
    automake \
    make \
    libtool \
    hwloc \
    openjdk-8-jdk \
    libblas-dev \
    liblpsolve55-dev \
    libarchive-dev \
    doxygen \
    liblapack-dev \
    libexpat1-dev \
    libsqlite3-dev \
    zlib1g-dev \
    gettext \
    patch \
    clang \
    python3-pip \
    libncurses5-dev \
    libreadline-dev \
    libdigest-perl-md5-perl \
    unzip \
    gcovr \
    lcov \
    libboost-all-dev  \
    lsb-release \
    libxml2-utils \
    python3-lxml \
    python3-psutil \
    wget \
    libcurl4-openssl-dev \
    rsync \
    libopenblas-openmp-dev \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    qtbase5-dev-tools \
    nano \
    pkg-config \
    python3-venv \
    bash-completion

COPY . /CYPRESS_Co-Simulation_Platform

# Building HELICS

WORKDIR /CYPRESS_Co-Simulation_Platform/Simulators/HELICS-CYPRESS
RUN mkdir build
WORKDIR /CYPRESS_Co-Simulation_Platform/Simulators/HELICS-CYPRESS/build
RUN cmake ../
RUN make
RUN make install

# Building DYNAWO

WORKDIR /CYPRESS_Co-Simulation_Platform/Simulators/dynawo-CYPRESS
RUN chmod +x myEnvDynawo.sh
RUN ./myEnvDynawo.sh build-user

WORKDIR /CYPRESS_Co-Simulation_Platform