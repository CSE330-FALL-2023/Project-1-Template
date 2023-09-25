#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/tty.h>
#include <linux/uidgid.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm/param.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/time_namespace.h>
#include <linux/time.h>

#include <linux/proc_fs.h>

// Define a custom data structure to hold process information
struct process_info {
    pid_t pid;
    unsigned long start_time;
    unsigned long boot_time;
};

// Define a buffer to store process information
#define MAX_PROCESSES 100
struct process_info buffer[MAX_PROCESSES];

// Define the target UID you want to filter by
#define TARGET_UID 1000  // Replace with the UID you are interested in

static int __init process_info_module_init(void) {
    struct task_struct *task;
    int fill = 0;

    printk(KERN_INFO "Process Info Module: Initializing\n");

    // Loop over all processes on the system
    for_each_process(task)
    {
        // Check if the process's UID matches the target UID
        if (task->cred->uid.val == TARGET_UID) {
            // Populate the buffer with process information
            if (fill < MAX_PROCESSES) {
                buffer[fill].pid = task->pid;
                buffer[fill].start_time = task->start_time;
                buffer[fill].boot_time = task->start_boottime;
                fill++;
            }
        }
    }

    // Print the collected process information
    printk(KERN_INFO "Processes with UID %d:\n", TARGET_UID);
    for (int i = 0; i < fill; i++) {
        printk(KERN_INFO "PID: %d, Start Time: %lu, Boot Time: %lu\n",
               buffer[i].pid, buffer[i].start_time, buffer[i].boot_time);
    }

    return 0;  // Module loaded successfully
}

static void __exit process_info_module_exit(void) {
    printk(KERN_INFO "Process Info Module: Exiting\n");
}

module_init(process_info_module_init);
module_exit(process_info_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module to collect process info by UID");

