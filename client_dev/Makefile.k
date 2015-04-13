ifeq ($(KERNELRELEASE),)
	KERNELDIR = /home/jia/MyStudio/RasPi/superlinux/linux-sunxi
	PWD =$(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gnueabihf-gcc -o io_control io_control.c

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
                          
clean:                    
	rm -rf *.o *.ko .tmp_versions *.mod.c modules.order  Module.symvers	
else                      
	obj-m :=io_control.o  
endif                     
    
