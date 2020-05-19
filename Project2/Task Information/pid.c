#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/proc_fs.h>
#include<asm/uaccess.h>
#include<linux/slab.h>
#include<linux/sched.h>
#include<linux/vmalloc.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

long pid;

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count,loff_t *pos);

static struct file_operations proc_ops={
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};
int proc_init(void)
{
	proc_create(PROC_NAME, 0666, NULL, &proc_ops);
	return 0;
}

void proc_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos){
	int rv=0;
	char buffer[BUFFER_SIZE];
	static int completed = 0;
	
	if(completed){
		completed = 0;
		return 0;	
	}
	completed = 1;
	
	struct task_struct *PCB=NULL;
	PCB = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(PCB==NULL)
	{	
		printk(KERN_INFO "Pid doesn't exist!\n");
		rv = sprintf(buffer, "Pid doesn't exist!\n");
	}
	else
		rv = sprintf(buffer, "Commond=[%s], Pid=[%ld], State=[%ld]\n", PCB->comm, pid, PCB->state);

	raw_copy_to_user(usr_buf, buffer, rv);

	return rv;
}

ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count,loff_t *pos){
	char *k_mem;
	/*allocate kernel memory*/
	k_mem = kmalloc(count, GFP_KERNEL);
	
	/*copies user space usr_buf to kernel memory*/
	raw_copy_from_user(k_mem, usr_buf, count);
	k_mem[count] = 0;
	kstrtol(k_mem, 0, &pid);
	
	/*return kernel memory*/
	kfree(k_mem);
	
	return count;
}
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pid Module");
MODULE_AUTHOR("YINHAO");
