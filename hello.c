#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

MODULE_LICENCE("GPL");
MODULE_AUTHOR("Zahid");
MODULE_DESCRIPTION("It is simple hello world LKM");
MODULE_VERSION("1.0");

static char *name="ZAHID";
module_param(name,charp,S_IRUGO);
MODULE_PARM_DESC(name,"The name is to display in /var/log/kern.log");

static int __init hello_init(void)
{
	printk(KERN_INFO,"HELOO %s From LKM\n",name);
	return 0;
}


static void __exit hello_exit(void)
{
	printk(KERN_INFO,"Bye %s From LKM\n");
}


module_init(hello_init);
module_exit(hello_exit);
