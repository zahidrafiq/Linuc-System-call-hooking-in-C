#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/stat.h>

unsigned long **sys_call_table;


//From this statment complier came to know that parameters are not available in CPU registers.
//and compiler try to find parameters on stack.  
asmlinkage int (*ref_sys_open) (const char*, int, int);

// Our fake system call. 
// On "open system call" Control will transfer to this segment of code.
// Here we can perform our required tasks. But at the moment we are just printing some message 
// to ensure that system call is hooked After that we are calling origional call so that system 
// continue its working because system uses a lot of "open" calls in its normal working.
asmlinkage long new_sys_open(const char* file, int flags, int mode)
{	
	// Any code according to requirement. 
	if(!(file[0]=='/' || file[0]=='.'))   // this condition is  used  to show  only  those call  that  are from  user  space
 						// to  avoid  rush  in  dmesg
		printk(KERN_INFO "Your System call is hooked!");
	//calling origional system call by using our previoiusly stored reference
        return ref_sys_open(file, flags, mode);
}

// this function returns the pointer of the system call table
static unsigned long **aquire_sys_call_table(void)
{
    unsigned long int offset = PAGE_OFFSET;
				// /usr/src/linux-headers-4.13.0-37/include/asm-generic/page.h:
				//#define PAGE_OFFSET(CONFIG_KERNEL_RAM_BASE_ADDRESS)
				// /usr/src/linux-headers-4.13.0-37/include/asm-generic/page.h: 
				// #define PAGE_OFFSET(0)
				// /usr/src/linux-headers-4.13.0-38/arch/x86/include/asm/page_types.h 
	
				//#define PAGE_OFFSET		((unsigned long)__PAGE_OFFSET) 
				// PAGE_OFFSET is the virtual address of the start of kernel address



    unsigned long **sct;

    while (offset < ULLONG_MAX) {
			//ULLONG_MAX is  defined in linux/kernal.h
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) 
            return sct;
/*
/usr/src/linux-headers-4.13.0-38-generic/arch/x86/include/generated/uapi/asm/unistd_x32.h:#define __NR_close (__X32_SYSCALL_BIT + 3)
/usr/src/linux-headers-4.13.0-38-generic/arch/x86/include/generated/uapi/asm/unistd_32.h:#define __NR_close 6
/usr/src/linux-headers-4.13.0-38-generic/arch/x86/include/generated/uapi/asm/unistd_64.h:#define __NR_close 3
*/
        offset += sizeof(void *);
    }
    return NULL;
}

// This function disable the memory protection.
// So that we can change sytem call table
static void disable_page_protection(void) 
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));
    if(!(value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

// This function enable the memory protection
// After Updating Sytem call table we use this function to 
// enable page protection again
static void enable_page_protection(void) 
{
    unsigned long value;
    asm volatile("mov %%cr0, %0" : "=r" (value));

    if((value & 0x00010000))
        return;

    asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}

// This function is executed when the module is loaded.
// 1. It gets the pointer of system call table
// 2. It disables page protection
// 3. It modifies the entry of the write system call
// 4. It enables page protection

static int __init hooking_start(void) 
{
    if(!(sys_call_table = aquire_sys_call_table())) {
             return -1;
    }

    disable_page_protection();
    // We are storing origional reference of open system call 
    // so that we can restore it at the end of our task
    ref_sys_open = (void *)sys_call_table[__NR_open];

    //We are placing reference of our fake system call in place of origional reference.
    sys_call_table[__NR_open] = (unsigned long *)new_sys_open;

    enable_page_protection();

    return 0;
}

// This method is executed when the module is removed/unloaded
// 1. It disables page protection
// 2. It restores the entry of the write system call
// 3. It enables page protection
static void __exit hooking_end(void) 
{
    if(!sys_call_table)
        return;
    disable_page_protection();
	// We are setting back origional value of "open" sytem call in system call table
	 sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
    enable_page_protection();
}

// We are registering our init method that 
// will be called at start when we will load this module
module_init(hooking_start);

// We are registering our exit method that 
// will be called at start when we will unload this module
module_exit(hooking_end);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZAHID");
MODULE_DESCRIPTION("A loadable Linux kernel module that hooks 'open' system call");
