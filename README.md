# DAGs

## How to compile this branch (demo + simulator)

Clone the repo and intialize the submodules
```
git clone https://github.com/mive93/multi-rate-DAG
cd multi-rate-DAG
git submodule update --init --recursive #initialize
```

Build the uavAP core
```
cd uavAP
mkdir build && cd build
cmake ../src/
make core # build only the core
```

Build the repo (main and sim)
```
cd ../.. # go back to multi-rate-DAG folder
mkdir build && cd build
cmake ../src/
make -j4
```

Run the main to generate the data for the sim and then the simulator
```
./main 
./sim
```
