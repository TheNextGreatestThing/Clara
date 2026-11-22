
#include "Tools.h"
#include <filesystem>
#define AC_USE_CPU
#define AC_WITH_EIGEN
#include "AlphaCore.h"

using namespace std;
using namespace AlphaCore;
using namespace std::chrono;

int mousex = 0;
int mousey = 0;
int prevmousex = -1;
int prevmousey = -1;
bool pressed = 0;
void display();
void timer(int a);
int cycles = 10;
vector<vector<double>> traininginp;
unsigned const int totalload = 60000;
unsigned const int total = 60000;
unsigned int labels[totalload];
vector<int> savedNumbers;

void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
bool hasNumber = false;
shared_ptr<CPUFeedForwardNeuralNetwork> brain;
float similarity = 0;

bool domouse = false;
auto mousestart = high_resolution_clock::now();

string voice = "Ava";

shared_ptr<CPUFeedForwardNeuralNetwork> brain2;
string trainedLargeNetwork = "trainedLarge";
string sigmoidNet = "trainedSigmoid";
bool useSigmoidNet = true;
bool useLargerNetwork = false;
bool useSmallSigmoid = false;


int main(int argc, char** argv) {
    //Sigmoid Network
    string exeP = exePath();
    string exe = "";
    int index = -1;
    for (int i = exeP.length()-1; i >= 0; i--) {
        if (exeP[i] == 'C') {
            index = i;
            break;
        }
    }
    if (index == -1) {
        cout << "Error locating necessary files." << endl;
        return -1;
    }
    for (int i = 0; i < index; i++) {
        exe+=exeP[i];
    }
    string sN = exe;
    sN += sigmoidNet;
    string rN = exe;
    rN += trainedLargeNetwork;
    if (filesystem::exists("UserData")) {
        brain.reset(new CPUFeedForwardNeuralNetwork({784, 1012, 816, 508, 240, 10}, "UserData"));
        brain->setActivations(0, AlphaCore::sigmoid);
        brain->setDerivatives(0, AlphaCore::sigmoidder);
        brain->setActivations(1, AlphaCore::sigmoid);
        brain->setDerivatives(1, AlphaCore::sigmoidder);
        brain->setActivations(2, AlphaCore::sigmoid);
        brain->setDerivatives(2, AlphaCore::sigmoidder);
        brain->setActivations(3, AlphaCore::sigmoid);
        brain->setDerivatives(3, AlphaCore::sigmoidder);
        brain->setActivations(4, AlphaCore::softmax);
        brain->setDerivatives(4, AlphaCore::softmaxder);
        //Relu Network
        brain2.reset(new CPUFeedForwardNeuralNetwork({784, 1012, 816, 508, 240, 10}, "UserData"));
        brain2->setActivations(0, AlphaCore::relu);
        brain2->setDerivatives(0, AlphaCore::reluder);
        brain2->setActivations(1, AlphaCore::relu);
        brain2->setDerivatives(1, AlphaCore::reluder);
        brain2->setActivations(2, AlphaCore::relu);
        brain2->setDerivatives(2, AlphaCore::reluder);
        brain2->setActivations(3, AlphaCore::relu);
        brain2->setDerivatives(3, AlphaCore::reluder);
        brain2->setActivations(4, AlphaCore::softmax);
        brain2->setDerivatives(4, AlphaCore::softmaxder);
    } else {
        brain.reset(new CPUFeedForwardNeuralNetwork({784, 1012, 816, 508, 240, 10}, sN));
        brain->setActivations(0, AlphaCore::sigmoid);
        brain->setDerivatives(0, AlphaCore::sigmoidder);
        brain->setActivations(1, AlphaCore::sigmoid);
        brain->setDerivatives(1, AlphaCore::sigmoidder);
        brain->setActivations(2, AlphaCore::sigmoid);
        brain->setDerivatives(2, AlphaCore::sigmoidder);
        brain->setActivations(3, AlphaCore::sigmoid);
        brain->setDerivatives(3, AlphaCore::sigmoidder);
        brain->setActivations(4, AlphaCore::softmax);
        brain->setDerivatives(4, AlphaCore::softmaxder);
        //Relu Network
        brain2.reset(new CPUFeedForwardNeuralNetwork({784, 1012, 816, 508, 240, 10}, rN));
        brain2->setActivations(0, AlphaCore::relu);
        brain2->setDerivatives(0, AlphaCore::reluder);
        brain2->setActivations(1, AlphaCore::relu);
        brain2->setDerivatives(1, AlphaCore::reluder);
        brain2->setActivations(2, AlphaCore::relu);
        brain2->setDerivatives(2, AlphaCore::reluder);
        brain2->setActivations(3, AlphaCore::relu);
        brain2->setDerivatives(3, AlphaCore::reluder);
        brain2->setActivations(4, AlphaCore::softmax);
        brain2->setDerivatives(4, AlphaCore::softmaxder);
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(_W, _H);

    glutCreateWindow("Clara");
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    glutMainLoop();

    return 0;
}

vector<double> screen(784, 0);
vector<bool> centers(784*784, 0);
bool inter = false;
int guess;
int titlescreen = 0;
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if (titlescreen > 3) {
        glClearColor(1,1,1,1);
    int lower = 255;
    if (pressed) {
        mousestart = high_resolution_clock::now();
        domouse = true;
        centers[mousey*HEIGHT + mousex] = 1;
        
        int tposx = mousex/28, tposy = 28-mousey/28;
        int index = tposy*28+tposx;
        screen[index] = 255;
        
        int ileft = tposx-1;
        int ilindex = tposy*28+ileft;
        if (ilindex < 784) {
            screen[ilindex] += lower;
        }
        
        int iright = tposx+1;
        int irindex = tposy*28+iright;
        if (iright < 784) {
            screen[irindex] += lower;
        }
        
        int ibot = tposy-1;
        int ibindex = ibot*28+tposx;
        if (ibindex < 784) {
            screen[ibindex] += lower;
        }
        
        int itot = tposy+1;
        int itindex = itot*28+tposx;
        if (itindex < 784) {
            screen[itindex] += lower;
        }
    }
    for (auto& element : screen) {
        if (element > 255) {
            element = 255;
        }
    }

    for (int k = 0; k < HEIGHT; k++) {
        for (int l = 0; l < WIDTH; l++) {
            if (centers[k*HEIGHT + l]==1) {
                circle(l, k, 41);
            }
        }
    }
    
    } else {
        glClearColor(0.0f,0.62f,0.75f, 1.0f);
        const char* t = "Clara";
        text(-0.055, 0.06, 1.0f, 1.0f, 1.0f, t);

        if(titlescreen >1) {
            delay(3);
        }
        titlescreen++;
    }
    
    //Speak
    if (domouse == true && (duration_cast<milliseconds>(high_resolution_clock::now()-mousestart)).count() >= 450) {
        hasNumber = true;
        similarity = 0;
        vector<float> cscreen(screen.begin(), screen.end());
        scalevect(cscreen);
        vector<float> outputs = brain->feedforward(std::move(cscreen));
        guess = std::max_element(outputs.begin(),outputs.end()) - outputs.begin();
        cout << endl;
        
        vector<float> outputs2 = brain2->feedforward(std::move(cscreen));
        int guess2 = std::max_element(outputs2.begin(),outputs2.end()) - outputs2.begin();
        bool firstbrain;
        if (outputs[guess] > outputs2[guess2]) {
            firstbrain = true;
        } else {
            firstbrain = false;
            guess = guess2;
        }
        if (sure(50, outputs) || sure(50, outputs2)) {
            savedNumbers.push_back(guess);
            string speak = "say -v ";
            speak += voice;
            speak += " \"";
            speak += to_string(guess);
            speak +="\"";
            system(speak.c_str());
            domouse=false;
            screen = vector<double>(784, 0);
            centers = vector<bool>(784*784, 0);
            hasNumber = false;
        } else {
            domouse = false;
        }
    }
    glutSwapBuffers();
}


void timer(int) {
    glutPostRedisplay();
    glutTimerFunc(1000/90, timer, 0);
}


void mouse(int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        pressed = 0;
    }
}

void motion(int x, int y) {
    pressed = 1;
    mousex = x;
    mousey = y;
    if (y > HEIGHT) {
        y=HEIGHT;
    } else if (y < 0) {
        y = 0;
    }
}
