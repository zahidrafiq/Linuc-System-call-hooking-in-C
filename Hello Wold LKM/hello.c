#include<linux/init.h>
#include<linux/module.h>

static int hello_init(void)
{
	printk(KERN_ALERT "Hello World\n");	
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Good Bye\n");
}

// We are registering our init method that 
// will be called at start when we will load this module
module_init(hello_init);

// We are registering our exit method that 
// will be called at start when we will unloaded this module
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZAHID");
MODULE_DESCRIPTION("A simple skeleton for a loadable Linux kernel module.");
