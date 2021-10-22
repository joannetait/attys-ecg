/* #include "attys-ecg.h"

#define BUFFERLENGTH 3

double bpmBuffer[BUFFERLENGTH] = {0};
//public private etccccc
double hrv;
double currentVal;
double lastVal;
double start;
double end;

//not sure how big the buffer should be. Maybe just current and prev value

void addBPM(double bpm){

    for (int i = (BUFFERLENGTH); i>1; i--){ //count down from buff length to
        int index = i-1;
        bpmBuffer[index]=bpmBuffer[index-1];
    }
    bpmBuffer[0]=bpm;
}
//difference between last two bpm 
void instHRV(double bpm){

    currentVal = bpmBuffer[BUFFERLENGTH-1];
    lastVal = ;
    hrv = currentVal - lastVal;

    if (hrv<0){         //ask what's quickest. Just make 1st bit 0? How many bits is in a double lol
        hrv = hrv* (-1);
    }

}

//add value ()