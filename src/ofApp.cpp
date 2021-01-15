#include "ofApp.h"


#define BUFFERSIZE 256; // バッファサイズ(256推奨．大きくすると処理に余裕はでるが遅延が長くなる)
#define SAMPLERATE 44100; // サンプルレート(Hz)
#define BPM 160; // テンポ


//--------------------------------------------------------------
void ofApp::setup(){
    
    // audio setting
    bufferSize = BUFFERSIZE;
    sampleRate = SAMPLERATE;
    bpm = BPM;
    
    inputBuffer.resize(bufferSize);
    ofSoundStreamSettings soundSettings;
    soundSettings.setOutListener(this);
    soundSettings.setInListener(this);
    soundSettings.sampleRate = sampleRate;
    soundSettings.numOutputChannels=2;
    soundSettings.numInputChannels=1;
    soundSettings.bufferSize = bufferSize;
    sound_stream.setup(soundSettings);

	ofSetBackgroundColor(20);
    
    myHelicopterSound = new helicopterSound(sampleRate, bpm);

	// setup the server to listen on 11999
	ofxTCPSettings tcpSettings(11999);

	// set other options
	//settings.blocking = false;
	//settings.reuse = true;
	//settings.messageDelimiter = "\n";

	TCP.setup(tcpSettings);

	// optionally set the delimiter to something else.  The delimiter in the client and the server have to be the same, default being [/TCP]
	TCP.setMessageDelimiter("\n");
	lastSent = 0;
    
    // 加速度
    accelerationx = 0.;
    accelerationy = 0.;
    accelerationz = 0.;
    angle = 0;

}

//--------------------------------------------------------------
void ofApp::update(){

	// for each client lets send them a message letting them know what port they are connected on
	// we throttle the message sending frequency to once every 100ms
	uint64_t now = ofGetElapsedTimeMillis();
	if(now - lastSent >= 100){
		for(int i = 0; i < TCP.getLastID(); i++){
			if( !TCP.isClientConnected(i) ) continue;

			TCP.send(i, "hello client - you are connected on port - "+ofToString(TCP.getClientPort(i)) );
		}
		lastSent = now;
	}
    
    if (angle > 0 && angle < 5) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, true, 0.);
    } else if (angle > 5 && angle < 60) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, true, 0.05);
    } else if (angle > 60 && angle < 90) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, true, 0.1);
    } else if (angle < 0 && angle > -5) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, false, 0.);
    } else if (angle < -5 && angle > -60) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, false, 0.1);
    } else if (angle < -60 && angle > -90) {
        myHelicopterSound->resetHelicopterSpeed(myHelicopterSound->helicopterSpeed, false, 0.2);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(220);
	ofDrawBitmapString("TCP SERVER Example \n\nconnect on port: "+ofToString(TCP.getPort()), 10, 20);

	ofSetColor(0);
	ofDrawRectangle(10, 60, ofGetWidth()-24, ofGetHeight() - 65 - 15);

	ofSetColor(220);

	// for each connected client lets get the data being sent and lets print it to the screen
	for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++){

		if( !TCP.isClientConnected(i) )continue;

		// give each client its own color
		ofSetColor(255 - i*30, 255 - i * 20, 100 + i*40);

		// calculate where to draw the text
		int xPos = 15;
		int yPos = 80 + (12 * i * 4);

		// get the ip and port of the client
		string port = ofToString( TCP.getClientPort(i) );
		string ip   = TCP.getClientIP(i);
		string info = "client "+ofToString(i)+" -connected from "+ip+" on port: "+port;


		// if we don't have a string allocated yet
		// lets create one
		if(i >= storeText.size() ){
			storeText.push_back( string() );
		}

		// receive all the available messages, separated by \n
		// and keep only the last one
		string str;
		string tmp;
		do{
			str = tmp;
			tmp = TCP.receive(i);
		}while(tmp!="");

		// if there was a message set it to the corresponding client
		if(str.length() > 0){
			storeText[i] = str;
		}

		// draw the info text and the received text bellow it
		ofDrawBitmapString(info, xPos, yPos);
		ofDrawBitmapString(storeText[i], 25, yPos + 20);
        vector<string> xyz = splitString(storeText[i], ',');
        if (xyz.size() == 3) {
            accelerationx = stof(xyz[0]);
            accelerationy = stof(xyz[1]);
            accelerationz = stof(xyz[2]);
        }
        if (accelerationx > 0) {
            angle = (180 / M_PI) * atan(accelerationy / accelerationx);
        } else if (accelerationx < 0 && accelerationy > 0) {
            angle = 90;
        } else if (accelerationx < 0 && accelerationy < 0) {
            angle = -90;
        } else {
            angle = 90;
        }
        ofDrawBitmapString( "angle: " + to_string(angle) , 25, yPos + 40);

	}
    if (recordingOn) {
        ofDrawBitmapString("RECORDING", 200, 50);
    } else if (helicopterOn) {
        ofDrawBitmapString("HELICOPTER", 200, 50);
    } else {
        ofDrawBitmapString("", 200, 50);
    }
    

}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer &buffer){
    const int frames = buffer.getNumFrames();
    float curVol = 0.0;
    for(int i = 0; i < frames; i++){
        inputBuffer[i] = buffer[i];
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &buffer){
    const int frames = buffer.getNumFrames();
    float curVol = 0.0;
    for(int i = 0; i < frames; i++){
        const int channels = buffer.getNumChannels();
        float currentSample = inputBuffer[i];
        if (recordingOn) {
            myHelicopterSound->recording(currentSample);
        }
        if (helicopterOn) {
            currentSample = myHelicopterSound->helicopter(currentSample);
        }
        buffer[i*channels+0] = currentSample;
        buffer[i*channels+1] = currentSample;
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
    case 'e':
        helicopterOn = false;
        if (recordingOn) {
            recordingOn = false;
        } else {
            myHelicopterSound->resetRecording();
            recordingOn = true;
        }
        break;
    case 'h':
        if (helicopterOn) {
            helicopterOn = false;
        } else {
            myHelicopterSound->resetHelicopterSpeed(1., false, 0.05);
            helicopterOn = true;
            recordingOn = false;
        }
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}
