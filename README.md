# assignment1
first kernel code for true random number

for execution:
1) to execute the program with ioctl read then you can execute like as:
 sudo ./user   (or)
 sudo ./user -g  (it will print the random number stored in the range )
2) To clear the variables that has been created i.e, using ioctl IO operation execution will be like:
 sudo ./user -c 
3) TO set the range of the lower bound and upper bound i.e, ioctl write  we can use the command as
  sudo ./user -s
4) To do both read write operation on an single exectution i.e, ioctl read write . we use the command as:
  sudo ./user -r

description:
	this is the kernel module to generate true random number. It make use of jiffies, to get the number of timer ticks from the time of system booting and msleep will makes to sleep the cpu for pre-mentioned amount of time and capture the number of clock cycles to perform msleep opertion from this we can generate true random number  
	I use IOCTL system call for communication between kernel and the system.
