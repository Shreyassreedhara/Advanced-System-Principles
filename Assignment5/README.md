# Simple Character utility for loading localities (SCULL) device driver

A device driver is a software program that controls a particular type of hardware device that is attached to a computer. Device drivers can generally be categorized as kernel device drivers or user device drivers. Kernel drivers load with the OS as part of the system's memory. User mode's device drivers are triggered by the end-user.  

## Problem statement

The aim of this project is to write the character device driver in the following way:

1. Define a device structure and embed the structure _cdev_ in that structure
    ```
    struct asp_mycdev{
        struct cdev dev;
        char *ramdisk;
        struct semaphore sem;
        int devno;
        // any other needed fields
    };
    ```

2. Support a variable number of devices that can be set at load time (default is 3). The device nodes will be named /dev/mycdev0, /dev/mycdev1,..., /dev/mycdev(N-1), where N is the number of devices.

3. Provide an entry function that would be accessed via lseek() function. This entry function should update the file position pointer based on the offset requested as long as it does not go out of bounds. In case it is going out of bounds then the buffer has to be expanded and the new region has to be filled with zeroes.

4. Provide an entry function that would be accesed via ioctl() function. The user application should be let to clear the data stored in ramdisk. The driver function should also reset the file position pointer to 0.

5. Each device can be opened concurrently and therefore can be accessed for read, write, lseek and ioctl concurrently. This has to happen in race free way.

6. All the resources should be recycled/freed at the time of unloading the device driver module.

## Frequently asked questions

1. What is a major number and minor number?
* The major number identifies the driver associated with the device. For example, /dev/null and /dev/zero are both managed by driver 1, whereas virtual consoles and serial terminals are managed by driver 4. The kernel uses the major number at _open_ time to dispatch execution to the appropriate driver. The minor number is only used by the specified driver. It helps the driver differentiate between several devices.

2. What are the difference between the character and block device driver?
* Character device drivers are used for devices that are slow, which manage a small amount of data, and access to data does not require frequent seek queries. Examples are keyboard, mouse, serial ports, sound card and joystick. Block device drivers are used for devices where data volume is large, data is organized on blocks, and search is common. Examples are hard drives, CD roms, ram disks etc.

3. Can memcpy be used to copy data in user space to kernel space?
* Copy_to_user and Copy_from_user both check if the address in the user space is valid and also that the process requesting this has permission to access this address. Thus memcpy should not be used to copy data from or to user space to or from kernel space. 

4. What is IOCTL?
* In computing, IOCTL is a system call for device-specific input/output operations and other operations which cannot be expressed by regular system calls. It takes a parameter specifying a request code. The effect of a call completely depends on the request code. Device independent request codes are sometimes used to give userspace access to kernel functions.

5. What is ENOTTY?
* Its an error code that the kernel delivers to the application that has made the IOCTL request to a device driver that doesn't recognize it.

6. What is IOCTL magic number?
* IOCTL magic number is a unique number or character that will differentiate our set of IOCTL calls. Sometimes the major number for the device is used here.

Refer [here](https://linux-kernel-labs.github.io/master/labs/device_drivers.html) for more information on how a character device driver is designed.

## Steps to run the project

1. Clone the repository into your local machine
2. Traverse to the Assignment5 folder
3. Open the command line from that directory and become super user by using the following command:
```
sudo su
```
* It is neccessary to become the super user so that we can load the driver that we have written

4. Compile the driver module using the command:
```
make
```
5. Compile the userapp using the following command:
```
make app
```
6. Load the module:
```
insmod char_driver.ko <num_devices>
```
* Here the 'num_devices' is a number of your choice. The number you give here is the number of devices that your driver will support
7. Test the driver
   - Run userapp:
   ```
   ./userapp <device_number>
   ```
   * Here the device number specifies the id number of the device to be tested
   - Choose the appropriate function when prompted and also choose the offset if prompted 
8. Once the testing is done, use this command to unload the module:
```
rmmod char_driver.ko
```
_Note:_
* Userapp has to be executed with root previleges as the device files in /dev/ are created in the driver with root previleges

## Author information

* Shreyas Gaadikere Sreedhara, Email - shreyasgaadikere@ufl.edu