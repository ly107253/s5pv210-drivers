MODNAME=led_drv
CC=arm-linux-gcc
APP=s5pv210_adc_test

ifeq ($(PLATFORM),PC)
    KERNELDIR=/lib/modules/$(shell -uname -r)/build
else
    KERNELDIR=/opt/EmbedSky/TQ210/Kernel_3.0.8_TQ210_for_Linux_v2.1
endif

obj-m += $(MODNAME).o

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	
clean:
	rm -fr *.mod.* *.o *.ko .$(MODNAME).* .tmp* Module.* modules.* $(APP)
