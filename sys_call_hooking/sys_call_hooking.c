#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/stat.h>

unsigned long **sys_call_table;

//asmlinkage long (*ref_sys_read)(unsigned int fd, const char __user *buf, size_t count);

asmlinkage int (*ref_sys_open) (const char*, int, int);

// Our fake system call that calls the original system call
asmlinkage long new_sys_open(const char* file, int flags, int mode)
{	
	if(!(file[0]=='/' || file[0]=='.'))
		printk(KERN_INFO "Your System call is hooked!");
        return ref_sys_open(file, flags, mode);
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

// This function disable the memory protection.
static void disable_page_protection(void) 
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));
    if(!(value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

// This function enable the memory protection
static void enable_page_protection(void) 
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if((value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}

// This function is executed when the module is loaded.
static int __init hooking_start(void) 
{
    if(!(sys_call_table = aquire_sys_call_table())) {
             return -1;
    }

    disable_page_protection();

    ref_sys_open = (void *)sys_call_table[__NR_open];
    sys_call_table[__NR_open] = (unsigned long *)new_sys_open;

    enable_page_protection();

    return 0;
}

// This method is executed when the module is removed/unloaded
static void __exit hooking_end(void) 
{
    if(!sys_call_table)
        return;
    disable_page_protection();
	 sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
    enable_page_protection();
}

module_init(hooking_start);
module_exit(hooking_end);
