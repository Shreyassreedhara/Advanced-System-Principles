# Explanation for Deadlock Scenarios:

Testcase 1 (test_1.c):

One process starts executing and at some time it will fork. The child process changes its execution using execl. Both 
the processes are in Mode1 and are trying to access the Driver. The child process executes and it tries to access the 
lock but the parent process has the lock and it doesn't give up the lock. At the same time, the parent process is also
waiting for the child to terminate.Hence, it results in a deadlock as the processes are waiting for each other to 
proceed which will never happen.

Testcase 2 (test_2.c):

This testcase is similar to first testcase except that both the processes will be in Mode 2. Hence, the 'count2' 
variable in Device Driver program will become 2. The parent process waits for the child to terminate. The child process
attempts to change mode using ioctl, but it can't, because the 'count2' is 2 and the ioctl waits until count2 becomes 1.
But the parent ends only after the child ends. Hence, a deadlock is created.

Testcase 3 (test_3.c):

Two threads are created when this testcase is run. Threads may or may not run in parallel. Hence, one of the threads 
will open in Mode1, it acquires the lock for the driver and will wait indefinite time because of while loop whereas the
second thread will be blocked because of the first thread while it tries to open the device in Mode1 as it can't obtain
the lock from the other thread. Thus a deadlock is created.

Testcase 4 (test_4.c):

Two threads are created. The first thread opens in Mode2 and the second thread opens in Mode1. Both the threads will
try to do ioctl to change their respective modes. But the second thread will change to Mode2 before the first thread
change to Mode1. This ordering can be achieved by inserting a sleep statement(sleep) in the test_4.c before executing 
ioctl in thread 1. At some point both threads will be in Mode2, and the first thread which now tries to change to Mode1
by ioctl will get blocked because count2 will be 2, and the second thread will wait for the first thread to end. Hence, 
this will cause a deadlock.

# Explanation of Race Condition Scenarios:

Critical region 1 (e2_open() and e2_release() funtion):

In the open() function, updating the values of the count1 and count2 shared variables is a critical region.Improper 
synchronization without using sem1 and sem2 would have made the devices to read a wrong value of count1
and count2 which can lead to a Race condition. But, the critical region is perfectly enclosed within the sem1
and sem2 which doesn't let the potential Race Conditions to happen. The same explanation suits for the e2_release()
function as well.

Critical region 2 (e2_read() and e2_write() function):

In these functions, the devc->ramdisk variable is the important shared variable and is in the critical region. This
also has been properly synchronised using sem1 and sem2. If it wasn't synchronised the devices would have read and
written values to and from the driver in hapazard manner.

Critical region 3 (e2_ioctl() function):

In this function, the count1 and count2 variables are the important critical shared variables here. They are trivial in
deciding the modes of the devices. They are properly synchronised here using sem1 and sem2. Else, the devices might end
up being in a wrong mode.