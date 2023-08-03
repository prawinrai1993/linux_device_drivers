#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_device"
#define BUF_LEN 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveen");

static char message[BUF_LEN];
static int message_len = 0;
static int open_count = 0;

static int device_open(struct inode *inode, struct file *file)
{
    if (open_count)
        return -EBUSY;

    open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;

    if (*offset >= message_len)
        return 0;

    if (*offset + length > message_len)
        length = message_len - *offset;

    bytes_read = copy_to_user(buffer, message + *offset, length);
    if (bytes_read == 0) {
        *offset += length;
        return length;
    }

    return -EFAULT;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    int bytes_written = 0;

    if (length > BUF_LEN)
        return -EFBIG;

    bytes_written = copy_from_user(message, buffer, length);
    if (bytes_written == 0) {
        message_len = length;
        *offset = length;
        return length;
    }

    return -EFAULT;
}

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init char_device_init(void)
{
    if (register_chrdev(0, DEVICE_NAME, &fops) < 0) {
        printk(KERN_ALERT "Failed to register the character device\n");
        return -1;
    }
    printk(KERN_INFO "Character device %s registered\n", DEVICE_NAME);
    return 0;
}

static void __exit char_device_exit(void)
{
    unregister_chrdev(0, DEVICE_NAME);
    printk(KERN_INFO "Character device %s unregistered\n", DEVICE_NAME);
}

module_init(char_device_init);
module_exit(char_device_exit);
