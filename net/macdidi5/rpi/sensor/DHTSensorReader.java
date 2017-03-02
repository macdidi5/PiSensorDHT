package net.macdidi5.rpi.sensor;

public class DHTSensorReader {
    public static native float[] readData(int pin);
}
