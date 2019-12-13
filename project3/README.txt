Source code modified from https://github.com/mit-pdos/xv6-public. The
modifications add a new system call getreadcount. Modified files are usys.S,
user.h, syscall.h, syscall.c and Makefile, and added a new program "test".

The system call getreadcount tells how many times the system call read has been
called since starting up the system. getreadcount takes an integer as an
argument. 0 or lower means a normal call that returns the number of read calls.
A positive integer zeroes the counter and also returns the number of read calls,
which is always 0 in this case.
