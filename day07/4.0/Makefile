obj-m += btn_drv.o
KDIR=/opt/kernel
all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -fr .*.cmd  *.o *.mod.c  *.ko  .tmp_versions module*  Module*
