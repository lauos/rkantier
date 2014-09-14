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
#define __NR_MKDIR 3
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
char global_buffer_enable[10];
char global_buffer_detect[STRINGLEN];
char proc_buf[1024];

struct proc_dir_entry *anti_rk, *enable, *detection;


#define KERNEL_START_ADDRESS 0xc0008000
#define KERNEL_SIZE 0x2000000
#define SEARCH_START_ADDRESS 0xc0800000
#define KALLSYMS_SIZE 0x200000

unsigned long pattern_kallsyms_addresses[] = {
	0xc0008000,	/* stext */
	0xc0008000,	/* _sinittext */
	0xc0008000,	/* _stext */
	0xc0008000	/* __init_begin */
};
unsigned long pattern_kallsyms_addresses2[] = {
	0xc0008000,	/* stext */
	0xc0008000	/* _text */
};
unsigned long pattern_kallsyms_addresses3[] = {
	0xc00081c0,	/* asm_do_IRQ */
	0xc00081c0,	/* _stext */
	0xc00081c0	/* __exception_text_start */
};
unsigned long pattern_kallsyms_addresses4[] = {
	0xc0008180,	/* asm_do_IRQ */
	0xc0008180,	/* _stext */
	0xc0008180	/* __exception_text_start */
};
unsigned long kallsyms_num_syms;
unsigned long *kallsyms_addresses;
unsigned char *kallsyms_names;
unsigned char *kallsyms_token_table;
unsigned short *kallsyms_token_index;
unsigned long *kallsyms_markers;


unsigned long (*kallsyms_lookup_name_addr)(const char *name);
struct proc_dir_entry* (*proc_mkdir_addr)(const char *name, struct proc_dir_entry *parent);
struct proc_dir_entry* (*create_proc_entry_addr)(const char *name, umode_t mode, struct proc_dir_entry *parent);
void (*remove_proc_entry_addr)(const char *name, struct proc_dir_entry *parent);

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

unsigned long kallsyms_expand_symbol(unsigned long off, char *namebuf) 
{
	int len;
	int skipped_first;
	unsigned char *tptr;
	unsigned char *data;

	/* Get the compressed symbol length from the first symbol byte. */
	data = &kallsyms_names[off];
	len = *data;
	off += len + 1;
	data++;

	skipped_first = 0;
	while (len > 0) {
		tptr = &kallsyms_token_table[kallsyms_token_index[*data]];
		data++;
		len--;

		while (*tptr > 0) {
			if (skipped_first != 0) {
				*namebuf = *tptr;
				namebuf++;
			} else {
				skipped_first = 1;
			}
			tptr++;
		}
	}
	*namebuf = '\0';

	return off;
}

int search_functions() 
{
	char namebuf[1024];
	unsigned long i;
	unsigned long off;
	int cnt;

	cnt = 0;
	for (i = 0, off = 0; i < kallsyms_num_syms; i++) {
		off = kallsyms_expand_symbol(off, namebuf);
		//printk("<%s : %p>\n", namebuf, kallsyms_addresses[i]);
		if (strcmp(namebuf, "kallsyms_lookup_name") == 0) {
			kallsyms_lookup_name_addr = kallsyms_addresses[i];
			printk("(kallsyms_lookup_name_addr : %p)\n", kallsyms_lookup_name_addr);
			cnt++;
		} else if (strcmp(namebuf, "proc_mkdir") == 0) {
			proc_mkdir_addr = kallsyms_addresses[i];
			printk("(proc_mkdir_addr : %p)\n", proc_mkdir_addr);
			cnt++;
		} else if (strcmp(namebuf, "create_proc_entry") == 0) {
			create_proc_entry_addr = kallsyms_addresses[i];
			printk("(create_proc_entry_addr : %p)\n", create_proc_entry_addr);
			cnt++;
		} else if (strcmp(namebuf, "remove_proc_entry") == 0) {
			remove_proc_entry_addr = kallsyms_addresses[i];
			printk("(remove_proc_entry_addr : %p)\n", remove_proc_entry_addr);
			cnt++;
		}
	}
	if (cnt < 4) {
		return -1;
	}

	return 0;
}


static int check_pattern(unsigned long *addr, unsigned long *pattern, int patternnum) 
{
	unsigned long val;
	unsigned long cnt;
	unsigned long i;

	val = *(unsigned long *)addr;	
	if (val == pattern[0]) 
	{
		cnt = 1;
		for (i = 1; i < patternnum; i++) 
		{
			val = *(unsigned long *)(&addr[i]);
			if (val == pattern[i]) 
			{
				cnt++;
			} 
			else 
			{
				break;
			}
		}
		if (cnt == patternnum) 
		{
			return 0;
		}
	}
	return -1;
}

static int check_kallsyms_header(unsigned long *addr) 
{
	if (check_pattern(addr, pattern_kallsyms_addresses, sizeof(pattern_kallsyms_addresses) / 4) == 0) 
	{
		return 0;
	} 
	else if (check_pattern(addr, pattern_kallsyms_addresses2, sizeof(pattern_kallsyms_addresses2) / 4) == 0) 
	{
		return 0;
	} else if (check_pattern(addr, pattern_kallsyms_addresses3, sizeof(pattern_kallsyms_addresses3) / 4) == 0) 
	{
		return 0;
	}else if (check_pattern(addr, pattern_kallsyms_addresses4, sizeof(pattern_kallsyms_addresses4) / 4) == 0) 
	{
		return 0;
	}

	return -1;
}

static int get_kallsyms_addresses() 
{
	unsigned long *endaddr;
	unsigned long i, j;
	unsigned long *addr;
	unsigned long n;
	unsigned long val;
	unsigned long off;
	int cnt, num;

	printk("Search kallsyms...\n");
	endaddr = (unsigned long *)(KERNEL_START_ADDRESS + KERNEL_SIZE);
	cnt = 0;
	num = 0;
	for (i = 0; i < (KERNEL_START_ADDRESS + KERNEL_SIZE - SEARCH_START_ADDRESS); i += 16) 
	{
		for (j = 0; j < 2; j++) 
		{
			cnt += 4;
			if (cnt >= 0x10000) 
			{
				cnt = 0;
				num++;
				printk("%d ", num);
			}

			/* get kallsyms_addresses pointer */
			if (j == 0) 
			{
				kallsyms_addresses = (unsigned long *)(SEARCH_START_ADDRESS + i);
			} 
			else 
			{
				if ((i == 0) || ((SEARCH_START_ADDRESS - i) < KERNEL_START_ADDRESS)) 
				{
					continue;
				}
				kallsyms_addresses = (unsigned long *)(SEARCH_START_ADDRESS - i);
			}
			if (check_kallsyms_header(kallsyms_addresses) != 0) 
			{
				continue;
			}
			addr = kallsyms_addresses;
			off = 0;

			/* search end of kallsyms_addresses */
			n = 0;
			while (1) 
			{
				val = addr[0];
				if (val < KERNEL_START_ADDRESS) 
				{
					break;
				}
				n++;
				addr++;
				off++;
				if (addr >= endaddr) 
				{
					return -1;
				}
			}

			/* skip there is filled by 0x0 */
			while (1) 
			{
				val = addr[0];
				if (val != 0) 
				{
					break;
				}
				addr++;
				off++;
				if (addr >= endaddr) 
				{
					return -1;
				}
			}

			val = addr[0];
			kallsyms_num_syms = val;
			addr++;
			off++;
			if (addr >= endaddr) 
			{
				return -1;
			}

			/* check kallsyms_num_syms */
			if (kallsyms_num_syms != n) 
			{
				continue;
			}

			if (num > 0) {
				printk("\n");
			}
			printk("(kallsyms_addresses=%08x)\n", (unsigned long)kallsyms_addresses);
			printk("(kallsyms_num_syms=%08x)\n", kallsyms_num_syms);

			endaddr = (unsigned long *)((unsigned long)kallsyms_addresses + KALLSYMS_SIZE);
			addr = &kallsyms_addresses[off];

			/* skip there is filled by 0x0 */
			while (addr[0] == 0x00000000) 
			{
				addr++;
				if (addr >= endaddr) 
				{
					return -1;
				}
			}

			kallsyms_names = (unsigned char *)addr;
			printk("(kallsyms_names: %p)\n", kallsyms_names);

			/* search end of kallsyms_names */
			for (i = 0, off = 0; i < kallsyms_num_syms; i++) 
			{
				int len = kallsyms_names[off];
				off += len + 1;
				if (&kallsyms_names[off] >= (unsigned char *)endaddr) 
				{
					return -1;
				}
			}


			/* adjust */
			addr = (unsigned long *)((((unsigned long)&kallsyms_names[off] - 1) | 0x3) + 1);
			if (addr >= endaddr) 
			{
				return -1;
			}

			/* skip there is filled by 0x0 */
			while (addr[0] == 0x00000000) 
			{
				addr++;
				if (addr >= endaddr) {
					return -1;
				}
			}

			/* but kallsyms_markers shoud be start 0x00000000 */
			addr--;

			kallsyms_markers = addr;

			printk("(kallsyms_markers: %p)\n", kallsyms_markers);

			/* end of kallsyms_markers */
			addr = &kallsyms_markers[((kallsyms_num_syms - 1) >> 8) + 1];
			if (addr >= endaddr) 
			{
				return -1;
			}

			/* skip there is filled by 0x0 */
			while (addr[0] == 0x00000000) 
			{
				addr++;
				if (addr >= endaddr) 
				{
					return -1;
				}
			}

			kallsyms_token_table = (unsigned char *)addr;

			printk("(kallsyms_token_table: %p)\n", kallsyms_token_table);

			i = 0;
			while ((kallsyms_token_table[i] != 0x00) || (kallsyms_token_table[i + 1] != 0x00)) 
			{
				i++;
				if (&kallsyms_token_table[i - 1] >= (unsigned char *)endaddr) 
				{
					return -1;
				}
			}

			/* skip there is filled by 0x0 */
			while (kallsyms_token_table[i] == 0x00) 
			{
				i++;
				if (&kallsyms_token_table[i - 1] >= (unsigned char *)endaddr) 
				{
					return -1;
				}
			}

			/* but kallsyms_markers shoud be start 0x0000 */
			kallsyms_token_index = (unsigned short *)&kallsyms_token_table[i - 2];

			printk("(kallsyms_token_index: %p)\n", kallsyms_token_index);

			return 0;
		}
	}

	if (num > 0) {
		printk("\n");
	}
	return -1;
}


static unsigned char syscall_table_flag[400];
static unsigned long exception_table[20];

//Detect Exception Handle Fucntion
static int detect_eht()
{
			return -1;
	void *swi_addr = (long *)0xffff0008;
	unsigned long offset = 0;
	unsigned long *vector_swi_addr = 0;
	int i;
	int eht_len = 20;

	offset = ((*(long *)swi_addr) & 0xfff) + 8;
	vector_swi_addr = *(unsigned long **)(swi_addr + offset);

	memcpy(exception_table, vector_swi_addr, 20 * 8);

	for(i = 0; i < eht_len; vector_swi_addr++, i++)
	{
		if(*(unsigned long *)vector_swi_addr != exception_table[i])
		{
			return -1;
		}
	}

	return 0;
}


//Detect Syscall Table 
static int detect_sct()
{
			return -1;
	int i;
	int flag = 0;

	printk("In detect_sct");
	for(i = 0; i < sys_call_table_size; i++)
	{
		if((unsigned long)sys_call_table[i] == (unsigned long)sys_call_table[i])
		{
			syscall_table_flag[i] = 1;
		}
		else
		{
			syscall_table_flag[i] = 0;
			flag = 1;
		}
	}

	if(flag == 1) 
	{
		return -1;
	}
	else
	{ 
		return 0;
	}
}

//Detect VFS
static int detect_vfs()
{
	return -1;
}

//Detect hidden module
static int detect_mod()
{
	return -1;
}

// static ssize_t check_buf(const char *buf)
// {
// 	return 1;
// }


// asmlinkage ssize_t hacked_read (int fd, char *buf, size_t count)
// {

// 	if(check_buf(buf))
// 	{
// 		printk ("SYS_READ: %02x\n", *buf);
// 	}
// 	return orig_read(fd, buf, count);
// }

// asmlinkage ssize_t hacked_write (int fd, char *buf, size_t count)
// {

//    	if(check_buf(buf))
// 	{
//    		printk ("SYS_WRITE: %02x\n", *buf);		
// 	}
//     return orig_write(fd, buf, count);
// }

// asmlinkage ssize_t hacked_open(const char *pathname, int flags) 
// {
// 	printk("SYS_OPEN: %s\n", pathname);
// 	return orig_open(pathname, flags);
// }

static void do_detect()
{   
	printk("In do_detect\n");

    strcpy(proc_buf, "Result:{");

    strcpy(proc_buf + strlen(proc_buf), "ExceptionTable=");

    if(detect_eht() < 0)
    {
       strcpy(proc_buf + strlen(proc_buf), "1");
    }
    else
    {
       strcpy(proc_buf + strlen(proc_buf), "0");
    }

    strcpy(proc_buf + strlen(proc_buf), ",SyscallTable=");

    if(detect_sct() < 0)
    {
       strcpy(proc_buf + strlen(proc_buf), "1");
    }
    else
    {
       strcpy(proc_buf + strlen(proc_buf), "0");
    }

    strcpy(proc_buf + strlen(proc_buf), ",VFS=");
    if(detect_vfs() < 0)
    {
       strcpy(proc_buf + strlen(proc_buf), "1");
    }
    else
    {
       strcpy(proc_buf + strlen(proc_buf), "0");
    }
    strcpy(proc_buf + strlen(proc_buf), ",HiddenModule=");
    if(detect_mod() < 0)
    {
       strcpy(proc_buf + strlen(proc_buf), "1");
    }
    else
    {
       strcpy(proc_buf + strlen(proc_buf), "0");
    }

    strcpy(proc_buf + strlen(proc_buf), "}");

    memcpy(global_buffer_detect, proc_buf, strlen(proc_buf) + 1);

    printk("%s\n", global_buffer_detect);
}

int proc_read_enable(char *page, char **start, off_t off, int count, int *eof, void *data) 
{
        return sprintf(page, global_buffer_enable); //把global_buffer的内容显示给访问者
}

int proc_read_mod(char *page, char **start, off_t off, int count, int *eof, void *data) 
{
        int len;
        len = sprintf(page, global_buffer_detect); //把global_buffer的内容显示给访问者
        return len;
}

int proc_write_mod(struct file *file, const char *buffer, unsigned long count, void *data)
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

        printk("Count : %lu\n", len);
        copy_from_user(proc_buf, buffer, len);
        proc_buf[strlen(proc_buf)] = '\0';

        printk("proc_buf : %s\n", proc_buf);

		if(strstr(proc_buf, "Detect:") == proc_buf)
		{
			do_detect();
		}
		else
		{
			memset(global_buffer_detect, 0, STRINGLEN);
		}

		printk("reult len : %d\n", strlen(global_buffer_detect));

        return strlen(global_buffer_detect);
}



static int init_rk(void)
{
	printk("Hello, Rootkit\n");
	sys_call_table = find_sys_call_table();
	sys_call_table_size = find_sys_call_table_size();

	printk("Get System Call table : %p\n", sys_call_table);
	printk("Get System Call table size : %lu\n",sys_call_table_size);

	get_kallsyms_addresses();
	search_functions();

	// int ret = (*set_memory_rw_addr)(sys_call_table, 1);
	// printk("Return of set_memory_rw : %d", ret);

	orig_read = sys_call_table[__NR_READ];
	orig_write = sys_call_table[__NR_WRITE];
	orig_open = sys_call_table[__NR_OPEN];

	// sys_call_table[__NR_READ] = hacked_read;
	// sys_call_table[__NR_WRITE] = hacked_write;
	// sys_call_table[__NR_OPEN] = hacked_open;

	anti_rk = (*proc_mkdir_addr)("anti-rk", NULL);
   	enable = (*create_proc_entry_addr)("enable", 0444, anti_rk);
	detection = (*create_proc_entry_addr)("detection", 0666, anti_rk);

    strcpy(global_buffer_enable, "1");
    enable->read_proc = proc_read_enable;

    detection->read_proc = proc_read_mod;
    detection->write_proc = proc_write_mod;


	return 0;
}

static void exit_rk(void)
{
	printk("Bye, Rootkit\n");
	(*remove_proc_entry_addr)("enable", anti_rk);
	(*remove_proc_entry_addr)("detection", anti_rk);
	(*remove_proc_entry_addr)("anti-rk", NULL);
}

module_init(init_rk);
module_exit(exit_rk);
