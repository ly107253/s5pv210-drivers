#/bin/sh
echo "arm-linux-gcc adc_test.c -o adc_test"
arm-linux-gcc adc_test.c -o adc_test
echo "cp adc_test /opt/EmbedSky/TQ210/rootfs/app"
cp adc_test /opt/EmbedSky/TQ210/rootfs/app
cp s5pv210_adc_drv.ko /opt/EmbedSky/TQ210/rootfs/drivers
