#include<linux/init.h>
#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

int counter = 0;

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_write)(unsigned int fd, const char __user *buf, size_t count);

//Our new write system call that will be called inspite of origional write system call
asmlinkage long new_sys_write(unsigned int fd, const char __user *buf, size_t count)
{   
    return ref_sys_write(fd,buf ,count);
}

// this function returns the pointer of the system call table
static unsigned long **aquire_sys_call_table(void)
{
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) 
            return sct;

        offset += sizeof(void *);
    }
    return NULL;
}

