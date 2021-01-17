#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "soundEffects/helicopterSound.hpp"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
        void exit();
        // audio setting
        void audioIn(ofSoundBuffer &buffer);
        void audioOut(ofSoundBuffer &buffer);
        ofSoundStream sound_stream;
        vector<float> inputBuffer;
        int bpm;
        int bufferSize;
        int sampleRate;
        bool recordingOn = false;
        bool helicopterOn = false;
        helicopterSound* myHelicopterSound;
        //
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

		ofxTCPServer TCP;

		ofTrueTypeFont  mono;
		ofTrueTypeFont  monosm;

		vector <string> storeText;
		uint64_t lastSent;
    
    
        // 加速度
        float accelerationx;
        float accelerationy;
        float accelerationz;
        // スマホの角度
        int angle;
    
        // 文字列操作関数定義
        vector<string> splitString(string& input, char delimiter) {
            istringstream stream(input);
            string field;
            vector<string> result;
            while (getline(stream, field, delimiter)) {
                result.push_back(field);
            }
            return result;
        }
};

