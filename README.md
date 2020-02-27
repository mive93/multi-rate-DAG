# Latency-Aware Generation of Single-Rate DAGs from Multi-Rate Task Sets

This is the official repository for the paper *"Latency-Aware Generation of Single-Rate DAGs from Multi-Rate Task Sets"*, by Micaela Verucchi, Mirco Theile, Marco Caccamo and Marko Bertogna (in proceeding RTAS 2020). \
Contact: Micaela Verucchi, micaela.verucchi@unimore.it.


## Abstract
This is an implementation of a conversion from a multi-rate DAG to a single rate DAG that meet given requirements on scheduling and end-to-end latency.

#### Input 
The input is a task set ![equation](https://latex.codecogs.com/gif.latex?%5CGamma), modeling an application composed of *N* periodic tasks ![equation](https://latex.codecogs.com/gif.latex?%5Ctau_x) arriving at time *t=0*. Each task ![equation](https://latex.codecogs.com/gif.latex?%5Ctau_x) is described by the tuple ![equation](https://latex.codecogs.com/gif.latex?%28WC_x%2C%20BC_x%2C%20T_x%2C%20D_x%29), where: 

* ![equation](https://latex.codecogs.com/gif.latex?WC_x%20%5Cin%20%5Cmathbb%7BR%7D) is the Worst Case Execution Time (WCET) of the task;
* ![equation](https://latex.codecogs.com/gif.latex?BC_x%20%5Cin%20%5Cmathbb%7BR%7D) is the Best Case Execution Time (BCET);
* ![equation](https://latex.codecogs.com/gif.latex?T_x%20%5Cin%20%5Cmathbb%7BN%7D) is the period;
* ![equation](https://latex.codecogs.com/gif.latex?D_x%20%5Cin%20%5Cmathbb%7BR%7D) represents the relative deadline.

The exchange of data between two tasks is modeled with as data edge, a directed edge between the producer and the consumer of the data. 

To constrain the latency of data propagation in task-chains, upper bounds on data age and on reaction time can be given, as well as maximum number of cores available. 

#### Output

The output is a *single-rate Directed Acyclic Graph (DAG)*. In this model, the full application is represented as directed acyclic graphs, with a unique source vertex and a unique sink vertex. Each vertex represents a task instance (job), while edges represent precedence constraints between jobs. In detail, the DAG is specified by a 3-tuple *(V,E,HP)* where:

* *V* represents the set of nodes, namely the jobs of the tasks of ![equation](https://latex.codecogs.com/gif.latex?%5CGamma), and *n = |V|*;
* *E* is the set of edges describing job-level precedence constraints;
* *HP* is the period of the DAG, namely the hyper-period of the tasks involved: ![equation](https://latex.codecogs.com/gif.latex?HP%20%3D%20lcm_%7B%5Cforall%20%5Ctau_x%20%5Cin%20%5CGamma%7D%20%5C%7BT_x%5C%7D).


In this model, the communication between jobs utilizes buffers in shared memory, which can be accessed by all the cores. The time to write/read a shared buffer is included in the execution time of each task. We adopt the implicit communication model defined in AUTOSAR, solving mutual exclusion via double-buffering. Each task complies with a read-execute-write semantic, i.e., it reads a private copy before the execution, and it writes a private copy at the end of the execution

## How to build the project
```
git clone https://github.com/mive93/multi-rate-DAG
cd multi-rate-DAG
mkdir build
cd build
cmake ../src
./main
```


