#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/device.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <linux/poll.h>
#include <asm/gpio.h>

struct io_control_driver
{
	unsigned int dev_major;
	struct class *io_control_class;
	struct device *io_control_device;
	struct cdev *io_control_cdev;
	dev_t  devno;
	unsigned int rval;
};

#define IO_MAN 0
#define IO_NET 1
int work_mode = IO_MAN;

static struct io_control_driver *io_driver;

static int io_control_open(struct inode *inode, struct file *file)
{    
    sw_gpio_setcfg(GPIOD(0),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(1),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(2),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(3),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(4),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(5),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(6),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(7),GPIO_CFG_OUTPUT); //Output
    sw_gpio_setcfg(GPIOD(8),GPIO_CFG_INPUT); //Input
    sw_gpio_setcfg(GPIOD(9),GPIO_CFG_INPUT); //Input
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
    sw_gpio_setcfg(GPIOH(14),GPIO_CFG_EINT); //IRQ
    sw_gpio_setcfg(GPIOH(15),GPIO_CFG_EINT); //IRQ
    sw_gpio_setval(0x00);
    return 0;
}

static void sw_gpio_setval(s8 val)
{
    for(int i=0;i<8;i++)
    {
        sw_gpio_setdrvlevel(GPIOD(i),(val>>i)&0x01);
    }
}
static ssize_t io_control_read(struct file * file, const char __user * buf, size_t size, loff_t * ppos)
{

	if(copy_to_user(buf, &a10_driver->rval, size))
	{
		printk(KERN_ERR "fail  copy_to_user!\n");
		return -EINVAL;
	}
	
	return size;
}

static ssize_t io_control_write(struct file * file, const char __user * buf, size_t size, loff_t * ppos)
{

    if(copy_from_user(&a10_driver->rval, buf, size))
    {
		printk(KERN_ERR "failed to copy_from_user!\n");
		return -EINVAL;
    }

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
	a10_driver->io_control_cdev = cdev_alloc();
	cdev_init(a10_driver->io_control_cdev, &open_file);
	cdev_add(a10_driver->io_control_cdev, a10_driver->devno, 1);	
}

static int __init io_control_test_init(void)
{
	int ret;
	printk(KERN_INFO "io_control_init ok!\n");
	
	a10_driver = kmalloc(sizeof(struct io_control_driver), GFP_KERNEL);
	if(NULL == a10_driver)
	{
		printk(KERN_ERR "faild to malloc memory\n");
		return -ENOMEM;
	}

	a10_driver->dev_major = 0;	
	if(a10_driver->dev_major)
	{
		a10_driver->devno = MKDEV(100,0);
		ret = register_chrdev_region(a10_driver->devno, 1, "io_control_module");
		if(ret < 0)
		{
			printk(KERN_INFO "failed to register_chrdev_region\n");
			goto register_err;
		}
	}
	else
	{
		a10_driver->devno = MKDEV(a10_driver->dev_major,0);
		ret = alloc_chrdev_region(&a10_driver->devno, 0,1, "open_module");
		if(ret < 0)
		{
			printk(KERN_INFO "failed to register_chrdev_region\n");
			goto register_err;
		}
	}
	
	setup_cdev_init();
	
	a10_driver->io_control_class = class_create(THIS_MODULE, "io_control_class");
	if (IS_ERR(a10_driver->io_control_class))
	{
		ret = PTR_ERR(a10_driver->io_control_class);
		goto class_err;
	}
	
	a10_driver->io_control_device = device_create(a10_driver->io_control_class, NULL, a10_driver->devno, NULL, "io_control");
	if (IS_ERR(a10_driver->io_control_device))
	{
		ret = PTR_ERR(a10_driver->io_control_device);
		goto device_err;
	}

	/*map the addr*/
	picfg0 = ioremap(vPI_CFG0, 0x10);
	picfg1 = ioremap(vPI_CFG1, 0x10);
	pidat0 = ioremap(vPI_DAT0, 0x10);
	pipul0 = ioremap(vPI_PUL0, 0X10);
	pgcfg1 = ioremap(vPG_CFG1, 0x10);
	pgdat0 = ioremap(vPG_DAT0, 0x10);
	pgpul0 = ioremap(vPG_PUL0, 0X10);
	pecfg0 = ioremap(vPE_CFG0, 0x10);
	pecfg1 = ioremap(vPE_CFG1, 0x10);
	pedat =  ioremap(vPE_DAT,  0x10);
	phcfg0 = ioremap(vPH_CFG0, 0x10);
	phdat0 = ioremap(vPH_DAT0, 0x10);

        if(sw_gpio_req_request(GPIOH(14),TRIG_EDGE_POSITIVE,io_set_man,Null)<0)
        {
            printk(KERN_INFO"Register IRQ failed!\n");
            goto irq_err;

        if(sw_gpio_req_request(GPIOH(15),TRIG_EDGE_POSITIVE,io_set_net,Null)<0)
        {
            printk(KERN_INFO"Register IRQ failed!\n");
            goto irq_err;
        }
	return 0;
irq_err:
        free_irq(gpio_to_irq(),"Free IRQ");
device_err:
	class_destroy(a10_driver->io_control_class);
class_err:
	unregister_chrdev_region(a10_driver->devno, 1);
	cdev_del(a10_driver->io_control_cdev);	
register_err:
	kfree(a10_driver);
	return ret;	
}
static void io_set_man(void)
{
    work_mode=IO_MAN;
}
static void io_set_net(void)
{
    work_mode=IO_NET;
}
static void __exit io_control_test_exit(void)
{
	printk(KERN_INFO "io_control_exit ok!\n");
	
	unregister_chrdev_region(a10_driver->devno, 1);
	cdev_del(a10_driver->io_control_cdev);
	device_destroy(a10_driver->io_control_class, a10_driver->devno);
	class_destroy(a10_driver->io_control_class);
	iounmap(picfg0);
	iounmap(picfg1);
	iounmap(pidat0);
	iounmap(pipul0);
	iounmap(pecfg0);
	iounmap(pecfg1);
	iounmap(pedat);
	iounmap(phcfg0);
	iounmap(phdat0);
	kfree(a10_driver);
}

module_init(io_control_test_init);
module_exit(io_control_test_exit);

MODULE_LICENSE("GPL");

