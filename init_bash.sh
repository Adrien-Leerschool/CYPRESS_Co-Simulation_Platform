CYPRESS_DIR="/home/adrien/Documents/CYPRESS/CYPRESS_Co-Simulation_Platform"

apt clean
apt-get update && apt-get install -y \
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
    bash-completion \
    bison \
    flex


cd "$CYPRESS_DIR/Simulators/HELICS-CYPRESS"
mkdir build
cd "$CYPRESS_DIR/Simulators/HELICS-CYPRESS/build"
cmake ../
make
make install

# Building DYNAWO

# Download the config.guess and config.sub files
wget -O config.guess "http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD"
wget -O config.sub "http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD"

# Root directory where you want to search and replace
root_directory="$CYPRESS_DIR/Simulators/dynawo-CYPRESS"

# Find all directories containing config.guess and config.sub files within the root_directory and its subdirectories
config_directories=$(find "$root_directory" -name "config.guess" -o -name "config.sub" | xargs -n1 dirname | sort -u)

for dir in $config_directories; do
    echo "Processing directory: $dir"
    
    # Copy config.guess and config.sub files to the directory
    cp config.guess "$dir"
    cp config.sub "$dir"
    
    echo "Copied files to $dir"
done

# Clean up downloaded files
rm config.guess config.sub

echo "All copies completed"

cd "$CYPRESS_DIR/Simulators/dynawo-CYPRESS"
chmod +x myEnvDynawo.sh
./myEnvDynawo.sh build-user
