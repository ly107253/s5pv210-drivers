#/bin/sh
echo "arm-linux-gcc led_test.c -o led_test"
arm-linux-gcc led_test.c -o led_test
echo "cp adc_test /opt/EmbedSky/TQ210/rootfs/app"
cp led_test /opt/EmbedSky/TQ210/rootfs/app
cp led_drv.ko /opt/EmbedSky/TQ210/rootfs/drivers
