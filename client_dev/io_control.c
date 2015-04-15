#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/gpio.h>
#include <asm/uaccess.h>
#include <mach/irq.h>
#include <mach/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JustinGreat");

struct io_control_driver
{
	unsigned int dev_major;
	struct class *io_control_class;
	struct device *io_control_device;
	struct cdev *io_control_cdev;
	dev_t  devno;
	signed char speed_val;
};

#define IO_MAN 0
#define IO_NET 1
int work_mode = IO_MAN;

static struct io_control_driver *io_driver;

static int io_control_open(struct inode *inode, struct file *file)
{    
    sw_gpio_setval(0x00);
    return 0;
}

static void sw_gpio_setval(signed char val)
{
    for(int i=0;i<8;i++)
    {
        sw_gpio_setdrvlevel(GPIOD(i),(val>>i)&0x01);
    }
    io_driver->speed_val=val;
}

static ssize_t io_control_read(struct file * file, const char __user * buf, size_t size, loff_t * ppos)
{
    char snd[1];
    if(0==sw_gpio_getdrvlevel(GPIOD(7))&&0!=sw_gpio_getdrvlevel(GPIOD(8)))
        snd[0]='l';
    else if(0==sw_gpio_getdrvlevel(GPIOD(8))&&0!=sw_gpio_getdrvlevel(GPIOD(7)))
        snd[0]='r';
    else
        snd[0]='w';
    if(copy_to_user(buf, snd, 1))
    {
	printk(KERN_ERR "fail  copy_to_user!\n");
	return -EINVAL;
    }	
    return 1;
}

static ssize_t io_control_write(struct file * file, const char __user * buf, size_t size, loff_t * ppos)
{
    char get[1];
    if(copy_from_user(get, buf, size))
    {
	printk(KERN_ERR "failed to copy_from_user!\n");
	return -EINVAL;
    }
    sw_gpio_setval(get[0]);
    return size;
}
unsigned int io_control_poll(struct file *filp, poll_table *wait)
{
    if (work_mode == IO_MAN)
        return POLLIN | POLLRDNORM;
    else
        return POLLOUT | POLLWRNORM;
}
struct file_operations open_file = {
	.owner = THIS_MODULE,
	.open  = io_control_open,
	.read  = io_control_read,
	.write = io_control_write,
        .poll  = io_control_poll
};

/*int cdev struct*/
static void setup_cdev_init(void)
{
	io_driver->io_control_cdev = cdev_alloc();
	cdev_init(io_driver->io_control_cdev, &open_file);
	cdev_add(io_driver->io_control_cdev, io_driver->devno, 1);	
}

static void io_set_man(void)
{
    work_mode=IO_MAN;
}
static void io_set_net(void)
{
    work_mode=IO_NET;
}
static int __init io_control_init(void)
{
	int ret;
	printk(KERN_INFO "io_control_init ok!\n");
	
	io_driver = kmalloc(sizeof(struct io_control_driver), GFP_KERNEL);
	if(NULL == io_driver)
	{
		printk(KERN_ERR "faild to malloc memory\n");
		return -ENOMEM;
	}

	io_driver->dev_major = 0;	
	if(io_driver->dev_major)
	{
		io_driver->devno = MKDEV(100,0);
		ret = register_chrdev_region(io_driver->devno, 1, "io_control_module");
		if(ret < 0)
		{
			printk(KERN_INFO "failed to register_chrdev_region\n");
			goto register_err;
		}
	}
	else
	{
		io_driver->devno = MKDEV(io_driver->dev_major,0);
		ret = alloc_chrdev_region(&io_driver->devno, 0,1, "open_module");
		if(ret < 0)
		{
			printk(KERN_INFO "failed to register_chrdev_region\n");
			goto register_err;
		}
	}
	
	setup_cdev_init();
	
	io_driver->io_control_class = class_create(THIS_MODULE, "io_control_class");
	if (IS_ERR(io_driver->io_control_class))
	{
		ret = PTR_ERR(io_driver->io_control_class);
		goto class_err;
	}
	
	io_driver->io_control_device = device_create(io_driver->io_control_class, NULL, io_driver->devno, NULL, "io_control");
	if (IS_ERR(io_driver->io_control_device))
	{
		ret = PTR_ERR(io_driver->io_control_device);
		goto device_err;
	}

        sw_gpio_setcfg(GPIOD(0),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(1),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(2),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(3),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(4),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(5),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(6),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(7),GPIO_CFG_OUTPUT); //Output
        sw_gpio_setcfg(GPIOD(8),GPIO_CFG_INPUT);  //Input
        sw_gpio_setcfg(GPIOD(9),GPIO_CFG_INPUT);  //Input
        sw_gpio_setpull(GPIOD(0),1);              //Pull up
        sw_gpio_setpull(GPIOD(1),1);              //Pull up
        sw_gpio_setpull(GPIOD(2),1);              //Pull up
        sw_gpio_setpull(GPIOD(3),1);              //Pull up
        sw_gpio_setpull(GPIOD(4),1);              //Pull up
        sw_gpio_setpull(GPIOD(5),1);              //Pull up
        sw_gpio_setpull(GPIOD(6),1);              //Pull up
        sw_gpio_setpull(GPIOD(7),1);              //Pull up
        sw_gpio_setpull(GPIOD(8),1);              //Pull up
        sw_gpio_setpull(GPIOD(9),1);              //Pull up
        sw_gpio_setcfg(GPIOH(14),GPIO_CFG_EINT);  //IRQ
        sw_gpio_setcfg(GPIOH(15),GPIO_CFG_EINT);  //IRQ
        if(sw_gpio_req_request(GPIOH(14),TRIG_EDGE_POSITIVE,io_set_man,NULL)<0)
        {
            printk(KERN_INFO"Register IRQ failed!\n");
            goto irq_err_1;

        if(sw_gpio_req_request(GPIOH(15),TRIG_EDGE_POSITIVE,io_set_net,NULL)<0)
        {
            printk(KERN_INFO"Register IRQ failed!\n");
            goto irq_err_2;
        }
	return 0;
irq_err_2:
        sw_gpio_req_free(GPIOH(15));
irq_err_1:   
        sw_gpio_req_free(GPIOH(14));
device_err:
	class_destroy(io_driver->io_control_class);
class_err:
	unregister_chrdev_region(io_driver->devno, 1);
	cdev_del(io_driver->io_control_cdev);	
register_err:
	kfree(io_driver);
	return ret;	
}
static void __exit io_control_exit(void)
{
	printk(KERN_INFO "io_control_exit ok!\n");
	
        sw_gpio_req_free(GPIOH(15));
        sw_gpio_req_free(GPIOH(14));
	unregister_chrdev_region(io_driver->devno, 1);
	cdev_del(io_driver->io_control_cdev);
	device_destroy(io_driver->io_control_class, io_driver->devno);
	class_destroy(io_driver->io_control_class);
	kfree(io_driver);
}

module_init(io_control_init);
module_exit(io_control_exit);


