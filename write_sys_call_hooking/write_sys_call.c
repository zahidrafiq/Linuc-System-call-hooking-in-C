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


// This function disable the memory protection
static void disable_page_protection(void) 
{
	printk(KERN_ALERT "Disabling page protection\n");
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if(!(value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

// This function enable the memory protection
static void enable_page_protection(void) 
{
	printk(KERN_ALERT "Enabling Page protection\n");
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if((value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}


