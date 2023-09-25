#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");

static struct semaphore my_semaphore;

static int my_thread_function(void *data) {
    while (!kthread_should_stop()) {
        pr_info("%s: Attempting to acquire semaphore...\n", current->comm);

        down(&my_semaphore); // Blocking acquire

        pr_info("%s: Semaphore acquired. Accessing shared resource...\n", current->comm);

        // Simulate some work
        msleep(2000); // Sleep for 2 seconds

        pr_info("%s: Releasing semaphore and finishing...\n", current->comm);
        up(&my_semaphore); // Release the semaphore
    }

    return 0;
}

static struct task_struct *my_thread;

static int __init my_init(void) {
    sema_init(&my_semaphore, 1); // Initialize the semaphore with an initial count of 1 (available)

    my_thread = kthread_run(my_thread_function, NULL, "my_thread");

    if (IS_ERR(my_thread)) {
        pr_err("Failed to create thread\n");
        return PTR_ERR(my_thread);
    }

    pr_info("Module loaded\n");
    return 0;
}

static void __exit my_exit(void) {
    kthread_stop(my_thread); // Stop the thread

    pr_info("Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);
