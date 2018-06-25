#include<linux/init.h>
#include<linux/module.h>
//#include <utmp.h>
//#include <fcntl.h> 
//#include <time.h>


static int log_init(void)
{
	 int fd = open("/var/run/utmp", O_RDONLY);	
   if (fd == -1 ){
	   perror("Error in opening utmp file");
	   exit(1);
   }
   struct utmp rec;
   int reclen = sizeof(rec);
   while (read(fd, &rec, reclen) == reclen)
	   show_info(&rec);
   close(fd);

	printk(KERN_ALERT "Hello World\n");	
	return 0;
}

static void log_exit(void)
{
	printk(KERN_ALERT "Good Bye\n");
}

static void show_info(struct utmp *rec){
   if(rec->ut_type != 7)
      return;

 int fd = open("/var/mylog.txt", O_RDWR | O_APPEND,0666);	
   if (fd == -1 ){
	 printk(KERN_ALERT "Error in opening utmp file");
	   return;
   }
	fprintf(fd,"%-10.10s   ", rec->ut_user);
	fprintf(fd,"%-10.10s   ", rec->ut_line);
   long time = rec->ut_time;
   char* dtg = ctime(&time);
	fprintf(fd,"%-15.12s", dtg+4);
   fprintf(fd,"   (%s)", rec->ut_host);
	fprintf(fd,"\n");		
}
module_init(log_init);
module_exit(log_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Azam");
MODULE_DESCRIPTION("A simple skeleton for a loadable Linux kernel module for  logging.");

void show_info(struct utmp *rec){
   if(rec->ut_type != 7)
      return;

 int fd = open("/var/mylog.txt", O_RDWR | O_APPEND,0666);	
   if (fd == -1 ){
	   perror("Error in opening utmp file");
	   exit(1);
   }
	fprintf(fd,"%-10.10s   ", rec->ut_user);
	fprintf(fd,"%-10.10s   ", rec->ut_line);
   long time = rec->ut_time;
   char* dtg = ctime(&time);
	fprintf(fd,"%-15.12s", dtg+4);
   fprintf(fd,"   (%s)", rec->ut_host);
	fprintf(fd,"\n");		
}
