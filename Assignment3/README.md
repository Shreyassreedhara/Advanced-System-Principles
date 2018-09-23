# MapReduce with concurrency using Semaphores

In computing, the producer-consumer problem (also known as the bounded-buffer problem) is a classic example of a multi-process synchronization problem. The problem describes two processes, the producer and the consumer, who share a common, fixed-size buffer used as a queue. 

## Description

Implement the Map-Reduce programming model for data processing. MapReduce is a framework to write applications to process huge amounts of data, in parallel, on large clusters of commodity hardware in a reliable manner. The algorithm contains two important tasks, namely Map and Reduce. Map takes a set of data and converts it into another set of data, where individual elements are broken down into tuples (key/value pairs). Secondly, reduce task, which takes the output from a map as an input and combines those data tuples into a smaller set of tuples. Combiner program makes use of fork, exec, pipe and dup2 system calls and combines the Map-Reduce programs.

Unlike the first assignment, the reducer work will be performed by multiple threads rather than multiple processes. The synchronization of these threads is achieved by using semaphores. The semaphore concept was invented by the Dutch computer scientist Edsger Dijkstra. A semaphore is a variable or a abstract data type used to control access to a common resource by a multiple processes in a concurrent system. 

The main difference between mutex and semaphore is that a mutex is a locking mechanism used to synchronize access to a resource while a semaphore is a signalling mechanism. 

## Steps to run the project

You must be on a Linux system to run the following program. Follow the below steps to run the files

1. Clone the repository into your local system.
2. Traverse to Assignment3 folder
3. Run the following commands:

```
make
./combiner 10 8 <input.txt
```
In case this command fails to work, use the below commands to run the program:

```
gcc -pthread -o combiner combiner.c
./combiner 10 8 <input.txt
```

## Note:

* input file provided has 8 unique userIDs
* First and the second argument passed has to be greater than zero and a whole number
* Sample output file has been included to show how the output should look like