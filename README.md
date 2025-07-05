# A library to test schedulability of the real-time DAG task model

I acknowledge the use of Chat GPT 4o to add the functionality of reading an entire directory as input.

This code represents a fork of the repo from https://github.com/mive93/DAG-scheduling. The original repo is a library for evaluating real-time Directed Acyclic Graph (DAG) tasks. It uses several analysis techniques from literature, which can be found on the original [page](https://github.com/mive93/DAG-scheduling?tab=readme-ov-file#supported-schedulability-tests).
Moreover, several methods from real-time literature have been implemented. This represents the contributions of Micaela Verucchi's Ph.D. thesis "A comprehensive analysis of DAG tasks: solutions for modern real-time embedded systems".

As per the author's wishes, please cite the following paper, if this code is used in research.

```
@article{verucchi2023survey,
  title={A survey on real-time DAG scheduling, revisiting the Global-Partitioned Infinity War},
  author={Verucchi, Micaela and Olmedo, Ignacio Sa{\~n}udo and Bertogna, Marko},
  journal={Real-Time Systems},
  pages={1--52},
  year={2023},
  publisher={Springer}
}
```

## Dependencies

For this repo the following libraries are needed: graphviz (for dot), libyaml-cpp-dev (to read yaml confi files) and python3-matplotlib (to plot the charts).
On Linux, install the dependencies in this way:

```
sudo apt-get install graphviz libyaml-cpp-dev python3-matplotlib libtbb-dev libeigen3-dev python3-dev
```

## Compilation

Once the dependencies have been installed, build the project with cmake.

```
git clone https://github.com/EuEric/DAG-scheduling 
cd DAG-scheduling
git submodule update --init --recursive 
mkdir build && cd build
cmake ..
make 
```

## Usage

After compiling move to the build folder by running the command `cd build`. There will be three compiled executables: **demo**, **eval** and **ray**. The first two, [demo](https://github.com/mive93/DAG-scheduling?tab=readme-ov-file#demo) and [eval](https://github.com/mive93/DAG-scheduling?tab=readme-ov-file#eval) are from the original source code. To see how to work with these files, check the given links. For the third file, **ray**, it provides the necessary executable to perform the RT analysis for a single DAG yaml, or from an entire directory. The Ray executable can be run as:
```
./ray <taskset_file_or_dir> <priority_file_or_dir>
```

where
  * `<taskset_file_or_dir>` is the first argument, representing Ray DAGs in yaml format. The input should be given as a path, to either a yaml file or an entire directory.
  * `<priority_file_or_dir>` is the the second argument, representing priority files for the given Ray DAGs in yaml format. The input should be given as a path, to either a yaml file or an entire directory. This arugment is **optional**, as DAGs can be analyzed **with** and **without** priorities.


## Input and output

This library reads DAGs and outputs RT analysis results. 

### Input

The library accept two input formats for taskse: yaml or DOT. Refer to the original [code](https://github.com/EuEric/DAG-scheduling) for instructions related to DOT format.

#### YAML format

A taskset is specified as an array of **tasks**, which has the following parameters:
 * ```t``` the period
 * ```d``` the deadline
 * ```vertices``` array of nodes of the DAG, defined by: 
    * ```id``` id of the node
    * ```c``` WCET of the node
 * ```edges``` array of **edges** of the DAG, defined by: 
    * ```to``` id of the vertex connected to the **head** of the edge
    * ```from```id of the vertex connected to the **tail** of the edge
 * ```indirect_edges``` array of **indirect edges** of the DAG, defined by: 
    * ```to``` id of the vertex connected to the **head** of the edge
    * ```from```id of the vertex connected to the **tail** of the edge

The `yaml` file comes with a **tasks** array as the top-level node of the yaml format.

An example, specific to Ray DAGs can be found in demo/taskset_ray.yaml.

### Output

Running the `./ray` executable generates an output file named `report.yaml` inside the `build/` directory. This file represents the real-time (RT) analysis performed using the Graham[1] and He[2] schedulability tests. In our code, we modified the He test source code, to enable the usage of a priority file.

### Output structure

The report.yaml file organizes results hierarchically, reflecting the structure of the input folder.

For example, if your input folder is named `DAGs/` and contains several subfolders (e.g., `ETP_0.1_GU_0.2`, `ETP_0.1_GU_0.4`), the yaml file will include a top-level `output:` array node, together with each subfolder, is represented as follows:
* ```output:```
  * ```ETP_0.1_GU_0.2:```
  * ```ETP_0.1_GU_0.4:```


Each subfolder entry is an array containing results for each analyzed yaml file, with the following structure:

* ```taskset: <filename.yaml>```
* ```he_2019:```
  * ```bounds:```
    * ```- <float value>```
  * ```schedulable: <boolean value>```
  * ```graham_1969:```
    * ```bounds:```
      * ```- <float value>```
    * ```schedulable: <boolean value>```

## References

- [1] Graham, R. L. (1969). Bounds on Multiprocessing Timing Anomalies. SIAM Journal on Applied Mathematics, 17(2), 416–429. https://doi.org/10.1137/0117039
- [2] He, Q., Jiang, X., Guan, N., & Guo, Z. (2019). Intra-Task Priority Assignment in Real-Time Scheduling of DAG Tasks on Multi-Cores. IEEE Transactions on Parallel and Distributed Systems, 30(10), 2283–2295. https://doi.org/10.1109/TPDS.2019.2910525


## Related code sources

- To generate Ray DAGs, use the code from https://github.com/EuEric/RD-Gen.
- To generate files containing priorities for the generate Ray DAGs, use the code from https://github.com/EuEric/dag_theory_code.
- To generate runnable Ray applications from Ray DAGs, use the code from https://github.com/EuEric/ray_code_generator.