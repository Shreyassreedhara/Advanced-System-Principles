# Producer Consumer with concurrency using mmap system call

To run the following program you will need to be working on a Linux platform. To run this program in your system follow the below steps:

1. Clone the repository into your local folder.
2. Traverse to Assignment4 folder
3. Run the following commands

```
make
./combiner 10 8 <input.txt
```

In case the above command doesn't work as expected, try the following commands below:

```
gcc -pthread -o combiner combiner.c
./combiner 10 8 <input.txt
```

# Note: 
* input file provided has 8 unique userIDs
* First and second argument passed has to be greater than zero and a whole number
