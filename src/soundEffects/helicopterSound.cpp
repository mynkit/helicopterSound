//
//  helicopterSound.cpp
//  helicopterSound
//
//  Created by keita miyano on 2021/01/15.
//

#include "soundEffects/helicopterSound.hpp"


helicopterSound::helicopterSound(int _sampleRate, int _bpm) {
    //! サンプルレート
    sampleRate = _sampleRate;
    // BPM
    bpm = _bpm;
    //! 保持されている音のサンプルサイズ
    loopSize = sampleRate * 60. / (float)bpm;
    // ループさせる音の長さはbpm1回分の長さとする
    loopBuffer.resize(loopSize);
    loopBufferIndex = 0;
    //! 元のsampleが保持用bufferの今どの位置か
    originalSamplePoint = 0;

    helicopterSpeed = 1.;
    accelerationDirection = true;
    accelerationRange = 0.05;
}

void helicopterSound::recording(float sample) {
    if(loopBufferIndex >= loopSize) {
        loopBufferIndex -= loopSize;
    }
    loopBuffer[loopBufferIndex] = sample;
    loopBufferIndex++;
}

void helicopterSound::resetRecording() {
    for (int i = 0; i < loopSize; i++) {
        loopBuffer[i] = 0;
    }
    loopBufferIndex = 0;
    originalSamplePoint = 0;
}

float helicopterSound::effect(float sample, int nxSpeed, bool mix) {
    if (nxSpeed > 0) {
        int th = loopSize / (float)nxSpeed;
        if(originalSamplePoint >= th){originalSamplePoint -= th;}
        if (mix) {
            sample += loopBuffer[originalSamplePoint];
        } else {
            sample = loopBuffer[originalSamplePoint];
        }
        originalSamplePoint++;
    }
    return sample;
}

void helicopterSound::resetHelicopterSpeed(float _helicopterSpeed, bool _accelerationDirection, float _accelerationRange) {
    helicopterSpeed = _helicopterSpeed;
    accelerationDirection = _accelerationDirection;
    accelerationRange = _accelerationRange;
}

float helicopterSound::helicopter(float sample) {
    int helicopterSpeedLimit = 2;
    int nxSpeed = 4;
    int th = loopSize / (float)nxSpeed;
    if (originalSamplePoint >= th / helicopterSpeed) {
        // ループが一周回るごとに速度が変化する
        if (accelerationDirection) {
            if(helicopterSpeed <= (float)helicopterSpeedLimit) {
                helicopterSpeed += accelerationRange;
            } else {
                accelerationDirection = false;
//                helicopterSpeed -= accelerationRange;
            }
        } else {
            if(helicopterSpeed > 0.5) {
                helicopterSpeed -= accelerationRange;
            } else {
                accelerationDirection = true;
//                helicopterSpeed += accelerationRange;
            }
        }
        originalSamplePoint = 0;
    }
    int point = originalSamplePoint * helicopterSpeed;
    if (point >= th) { point %= th; }
    sample = loopBuffer[point];
    originalSamplePoint++;

    return sample;
}
