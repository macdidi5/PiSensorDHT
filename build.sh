export JAVA_HOME=/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt

echo "Compile net/macdidi5/rpi/sensor/DHTSensorReader.java"
javac net/macdidi5/rpi/sensor/DHTSensorReader.java

echo "Create JNI header file DHTSensorReader.h"
javah -d net/macdidi5/rpi/sensor net.macdidi5.rpi.sensor.DHTSensorReader

echo "Create header net/macdidi5/rpi/sensor/DHTSensorReader.h"
mv net/macdidi5/rpi/sensor/net_macdidi5_rpi_sensor_DHTSensorReader.h net/macdidi5/rpi/sensor/DHTSensorReader.h

echo "Compile net/macdidi5/rpi/sensor/dhtsensor.c with wiringPi library"
gcc -I $JAVA_HOME/include/ -I $JAVA_HOME/include/linux/ -o net/macdidi5/rpi/sensor/dhtsensor.so -shared net/macdidi5/rpi/sensor/dhtsensor.c -l wiringPi

echo "Copy new org/mandfer/dhtsensor.so in /usr/lib/"
sudo cp net/macdidi5/rpi/sensor/dhtsensor.so /usr/lib/

echo "Create symbolic link for the library"
sudo ln -s /usr/lib/dhtsensor.so /usr/lib/libdhtsensor.so

echo "Updated library:"
ls -lart /usr/lib/libdht*
