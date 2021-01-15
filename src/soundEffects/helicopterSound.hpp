//
//  helicopterSound.hpp
//  helicopterSound
//
//  Created by keita miyano on 2021/01/15.
//

#ifndef helicopterSound_hpp
#define helicopterSound_hpp

#include "ofMain.h"

class helicopterSound {
    public:
        int loopSize;
        int sampleRate;
        int originalSamplePoint;
        int bpm;
        vector<float> loopBuffer;
        int loopBufferIndex;
        float helicopterSpeed;
        bool accelerationDirection;
        float accelerationRange;
        helicopterSound(int _sampleRate, int _bpm);
        ~helicopterSound();
        void recording(float sample);
        void resetRecording();
        float effect(float sample, int nxSpeed, bool mix);
        void resetHelicopterSpeed(float _helicopterSpeed, bool _accelerationDirection, float _accelerationRange);
        float helicopter(float sample);
};

#endif /* helicopterSound_hpp */
