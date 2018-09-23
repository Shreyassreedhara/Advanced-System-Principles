# Producer and Consumer with concurrency using Mutex

In computing, the producer-consumer problem (also known as the bounded-buffer problem) is a classic example of a multi-process synchronization problem. The problem describes two processes, the producer and the consumer, who share a common, fixed-size buffer used as a queue. 

## Description

Implement the Map-Reduce programming model for data processing. MapReduce is a framework to write applications to process huge amounts of data, in parallel, on large clusters of commodity hardware in a reliable manner. The algorithm contains two important tasks, namely Map and Reduce. Map takes a set of data and converts it into another set of data, where individual elements are broken down into tuples (key/value pairs). Secondly, reduce task, which takes the output from a map as an input and combines those data tuples into a smaller set of tuples. Combiner program makes use of fork, exec, pipe and dup2 system calls and combines the Map-Reduce programs.

Unlike the previous assignment, the reducer work will be performed by multiple threads rather than multiple processes. The synchronization of these threads is achieved by using mutexes. A mutual exclusion object is a program object that allows multiple program threads to share the same resource, such as file access, but not simultanously. When a program is started, a mutex is created with a unique name. After this stage, any thread that needs the resource must lock the mutex from other threads while it is using the resource.

## Steps to run the project

You must be on a linux system to run the following commands. Follow the steps below to run the program

1. Clone the repository to your local system
2. Head to the folder consisting of combiner and input files
3. Run the following commands:

```
make
./combiner 10 8 <input.txt
```

If for some reason the above commands do not work as expected, run the program using the below commands:

```
gcc -pthread -o combiner combiner.c
./combiner 10 8 <input.txt
```

## Author information

1. Shreyas Gaadikere Sreedhara, Email - shreyasgaadikere@ufl.edu