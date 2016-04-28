#include "math.h"
int analogvals[2500];
double a[3] = {0.008732,    0.008732*2,    0.008732};
double b[2] = {-1.7188,    0.7537};
double prev[2];
double filtered[2];
int cnt,cnt2;
double n_avg = 10;
double Vmid = 1.613;
double voltage;
double RMSsamp, RMScurrent;
char publishString[40];

void setup() {
    Particle.variable("RMScurrent", &RMScurrent, DOUBLE); 
}

void loop() {
    
    //very quickly sample the signal
    cnt = 0;
    while(cnt<2500){
        
        analogvals[cnt] = analogRead(A0); ///store current read to the array location cnt
        cnt = cnt + 1;
        
    }
    
    //transform the raw readings into averages and then calculate RMS current
    cnt = 0;
    RMScurrent = 0;
    while(cnt<2500){

        //create a averaged point of 10 samples
        RMSsamp = 0;
        cnt2 = 0;
        while (cnt2<n_avg && cnt < 2500){
            //convert the raw readings to a voltage
            voltage = (double)analogvals[cnt]*3.3/4095 - Vmid;
            
            //implement a 2nd order butterworth to filter the raw readings
            filtered[0] = a[0]*voltage + a[1]*prev[0] + a[2]*prev[1] - b[0]*filtered[0] - b[1]*filtered[1];
            prev[1] = prev[0];
            prev[0] = voltage;
            filtered[1] = filtered[0];
            
            //convert the filtered value and square it, add it to the sum of 10 RMS values
            RMSsamp = RMSsamp + pow(filtered[0]*26.67,2);
            cnt = cnt + 1;
            cnt2 = cnt2 + 1;
        }
        
        //average 10 RMS values
        RMSsamp = RMSsamp/n_avg;
        
        //sum the squared and averaged samples
        RMScurrent = RMScurrent + RMSsamp;
        
    }
    
    //find the total RMS current value of the entire data set
    RMScurrent = sqrt(RMScurrent/(2500/n_avg));//root mean
    
    //publish the RMS wattage value with a linear calibration
    sprintf(publishString, "%lf", 120*(1.44*RMScurrent - 0.35));
    Particle.publish("RMS wattage",publishString);
    delay(1000);
 
}
