# Autonomic Farm Pattern
Provide a farm pattern capable of adjusting its parallelism degree based on whether the system is performing worse or better
than a given service time. Hence, the pattern aims at achieving (consistently) the service time taken as input.
The pattern is tested providing a collection of tasks such that the tasks in the initial, central and
final part all require a different average time to be computed (e.g. 4L in the first part, L in the second part
and 8L in the third part) and the task collection execution time is considerably longer than the time needed
to reconfigure the farm.

## Usage
```
chmod +x run
make && run  
```
