#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include <cmath>
#include <bits/stdc++.h>
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

// I/O pins
InterruptIn sw2(SW2);
DigitalOut led1(LED1);
I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);

// global variable
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
float xdata[1000];
float ydata[1000];
float zdata[1000];
int logdata[1000];


using namespace std;
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
void logger(int x);
void ISR(void);         // the eventqueue ISR
void ledtoggle(int x);

int main() {
    Thread thread1;
    EventQueue queue(1000 * EVENTS_EVENT_SIZE);
    thread1.start(callback(&queue, &EventQueue::dispatch_forever));
    sw2.rise(queue.event(ISR));
    while(1){}
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}

void logger(int x) {
    static int i = 0;   // loop index
    // pc.baud(115200);
    float t[3];
    uint8_t who_am_i, data[2], res[6];
    int16_t acc16;

    // Enable the FXOS8700Q
    led1 = !led1;
    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    // Get the slave address
    FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
    FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

    acc16 = (res[0] << 6) | (res[1] >> 2);
    if (acc16 > UINT14_MAX/2)
        acc16 -= UINT14_MAX;
    t[0] = ((float)acc16) / 4096.0f;

    acc16 = (res[2] << 6) | (res[3] >> 2);
    if (acc16 > UINT14_MAX/2)
        acc16 -= UINT14_MAX;
    t[1] = ((float)acc16) / 4096.0f;

    acc16 = (res[4] << 6) | (res[5] >> 2);
    if (acc16 > UINT14_MAX/2)
        acc16 -= UINT14_MAX;
    t[2] = ((float)acc16) / 4096.0f;
    // log the data
    
    xdata[i] = t[0];
    ydata[i] = t[1];
    zdata[i] = t[2];
    
    // if the angle > 45 then log
    if (acos(t[2] / (sqrt(pow(t[0], 2) + pow(t[1], 2) + pow(t[2], 2)))) * 180 / M_PI > 45)
        logdata[i] = 1;
    else
        logdata[i] = 0;
    i++;
}
void ISR(void) {
    // declare thread and eventqueue
    Thread thread2;
    Thread thread3;
    EventQueue queue2(32 * EVENTS_EVENT_SIZE);
    EventQueue queue3(32 * EVENTS_EVENT_SIZE);
    // init thread and queue
    thread2.start(callback(&queue2, &EventQueue::dispatch_forever));
    thread3.start(callback(&queue3, &EventQueue::dispatch_forever));
    // call events
    queue2.call_every(100, logger, 0);
    queue3.call_every(500, ledtoggle, 0);
    // wait after 10 seconds
    wait(10);
    thread2.terminate();
    thread3.terminate();
    // print out the data
    for (int i = 0; i < 100; i++) {
        pc.printf("%1.3f\r\n", xdata[i]);
    }
    for (int i = 0; i < 100; i++) {
        pc.printf("%1.3f\r\n", ydata[i]);
    }
    for (int i = 0; i < 100; i++) {
        pc.printf("%1.3f\r\n", zdata[i]);
    }
    for (int i = 0; i < 100; i++) {
        pc.printf("%d\r\n", logdata[i]);
    }
}
void ledtoggle(int x) {
    led1 = !led1;
}