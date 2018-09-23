
obj-m := assignment6.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
	
app: 
	gcc -o sample sample.c
	gcc -o test_1 test_1.c
	gcc -o test_2 test_2.c
	gcc -o test_3 test_3.c	-lpthread
	gcc -o test_4 test_4.c	-lpthread
	

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
