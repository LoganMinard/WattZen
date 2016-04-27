#include "math.h"
int analogvals[2500];
double a[3] = { 0.1675,    0.3350,    0.1675};
double b[2] = {-0.5570,    0.2270};
double prev_avgs[2];
double filtered[2];
int cnt,cnt2;
double n_avg = 10;
double Vmid = 1.613;
double avgsamp, RMScurrent;
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
        avgsamp = 0;
        cnt2 = 0;
        while (cnt2<n_avg && cnt < 2500){
            avgsamp = avgsamp + (double) analogvals[cnt];
            cnt = cnt + 1;
            cnt2 = cnt2 + 1;
        }
        
        //convert to voltage
        avgsamp = 3.3*avgsamp/(n_avg*4095)-Vmid;
        
        
        //implement 2nd order butterworth filter
        filtered[0] = a[0]*avgsamp + a[1]*prev_avgs[0] + a[2]*prev_avgs[1] - b[0]*filtered[0] - b[1]*filtered[1];
        prev_avgs[1] = prev_avgs[0];
        prev_avgs[0] = avgsamp;
        filtered[1] = filtered[0];
        
        //sprintf(publishString, "%lf", filtered[0]);
        //Particle.publish("filtered",publishString);
        //delay(1000);
        
        RMScurrent = RMScurrent + pow(filtered[0]*20,2);//convert to a current, square it, and sum
        
    }
    
    RMScurrent = sqrt(RMScurrent/(2500/n_avg));//root mean
    
    
    sprintf(publishString, "%lf", 1.16*RMScurrent - 0.29);
    Particle.publish("RMScurrent",publishString);
    delay(1000);
 
}
