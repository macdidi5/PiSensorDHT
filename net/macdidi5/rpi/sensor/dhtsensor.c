#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DHTSensorReader.h"

#define MAXTIMINGS 85

int dht_data[5] = {0, 0, 0, 0, 0};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    if (wiringPiSetup() == -1) {
        exit(1);
    }
    
    return JNI_VERSION_1_6;
}

int readDHT(int dhtPin, float *temp_p, float *hum_p) {
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    dht_data[0] = dht_data[1] = dht_data[2] = dht_data[3] = dht_data[4] = 0;

    // pull pin down for 18 milliseconds
    pinMode(dhtPin, OUTPUT);
    digitalWrite(dhtPin, LOW);
    delay(18);
    
    // then pull it up for 40 microseconds
//    digitalWrite(dhtPin, HIGH);
//    delayMicroseconds(40);
    
    // prepare to read the pin
    pinMode(dhtPin, INPUT);

    // detect change and read data
    for (i = 0; i < MAXTIMINGS; i++) {
        counter = 0;
        
        while (digitalRead(dhtPin) == laststate) {
            counter++;
            delayMicroseconds(1);
            
            if (counter == 255) {
                break;
            }
        }
        
        laststate = digitalRead(dhtPin);

        if (counter == 255) 
            break;

        // ignore first 3 transitions
        if ((i >= 4) && (i % 2 == 0)) {
            // shove each bit into the storage bytes
            dht_data[j / 8] <<= 1;
            
            if (counter > 16)
                dht_data[j / 8] |= 1;
            j++;
        }
    }

    // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
    // print it out if data is good
    if ((j >= 40) && (dht_data[4] == 
            ((dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) & 0xFF))) {
        float h = (float)((dht_data[0] << 8) + dht_data[1]) / 10;
	
        if ( h > 100 ) {
            h = dht_data[0];	// for DHT11
	}
        
	float c = (float)(((dht_data[2] & 0x7F) << 8) + dht_data[3]) / 10;
		
        if ( c > 125 ) {
            c = dht_data[2];	// for DHT11
	}
        
	if ( dht_data[2] & 0x80 ) {
            c = -c;
	}
        
        *temp_p = (float) c;
        *hum_p = (float) h;
        
        return 1;
    } 
    else {
        return 0;
    }
}

JNIEXPORT jfloatArray JNICALL Java_net_macdidi5_rpi_sensor_DHTSensorReader_readData
(JNIEnv *env, jobject obj, jint gpio_pin) {
    jfloatArray j_result = (*env)->NewFloatArray(env, 2);
    jfloat result[2];
    float t, h;
    int pin = (int) gpio_pin;
    int rs = 0;
    int errorCounter = 0;
    
    do {
        rs = readDHT(pin, &t, &h);        
        if (errorCounter > 10) {
            t = -999;
            h = -999;
            break;
        }else{
            errorCounter++;
        }        
    } while (rs == 0);
    
    result[0] = (jfloat) t;
    result[1] = (jfloat) h;
    (*env)->SetFloatArrayRegion(env, j_result, 0, 2, result);
    
    return j_result;
}
