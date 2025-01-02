#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "version.h"

#define PROC_NAME "libcamera_proc"
#define PROC_MAX_SIZE 1024

static char *proc_buffer;
static size_t proc_buffer_size;

/**
 * @brief Read callback for the /proc file.
 */
static ssize_t proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *pos) {
    if (*pos > 0 || count < proc_buffer_size)
        return 0; // End of file

    if (copy_to_user(user_buffer, proc_buffer, proc_buffer_size))
        return -EFAULT;

    *pos += proc_buffer_size;
    return proc_buffer_size;
}

/**
 * @brief Write callback for the /proc file.
 */
static ssize_t proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos) {
    if (count > PROC_MAX_SIZE)
        return -EINVAL; // Input too large

    if (copy_from_user(proc_buffer, user_buffer, count))
        return -EFAULT;

    proc_buffer_size = count;
    proc_buffer[count] = '\0'; // Null-terminate the input
    return count;
}

// File operations for the /proc file
static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init proc_file_init(void) {
    // Allocate a buffer for /proc file content
    proc_buffer = kmalloc(PROC_MAX_SIZE, GFP_KERNEL);
    if (!proc_buffer)
        return -ENOMEM;

    // Create the /proc file
    if (!proc_create(PROC_NAME, 0666, NULL, &proc_file_ops)) {
        kfree(proc_buffer);
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "Ver: %s Compiled at: %s\n", GIT_VERSION, VERSION_STRING);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit proc_file_exit(void) {
    // Remove the /proc file and free resources
    remove_proc_entry(PROC_NAME, NULL);
    kfree(proc_buffer);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

module_init(proc_file_init);
module_exit(proc_file_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Li");
MODULE_DESCRIPTION("libcamera_proc");
