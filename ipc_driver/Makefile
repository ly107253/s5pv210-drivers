MODNAME=lyipc
CC=arm-linux-gcc
#APP=s5pv210_adc_test

ifeq ($(PLATFORM),PC)
    KERNELDIR=/lib/modules/$(shell -uname -r)/build
else
    KERNELDIR=/opt/EmbedSky/TQ210/Kernel_3.0.8_TQ210_for_Linux_v2.1
endif

obj-m += $(MODNAME).o
lyipc-objs := ipc_driver.o mempool.o
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
#	$(CC) -o $(APP) $(APP).c
clean:
	rm -fr *.mod.* *.o *.ko .$(MODNAME).* .tmp* Module.* modules.* $(APP)
