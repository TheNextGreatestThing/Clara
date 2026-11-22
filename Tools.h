//
//  Tools.h
//  Digit Recogniion
//
//  Created by Ethan Henry on 12/27/20.
//

#pragma once
#include <iostream>
#include <fstream>
#include <GLUT/glut.h>
#include <string>
#include <cstring>
#include <sstream>
#include <cassert>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <mach-o/dyld.h>
#include <limits.h>
using namespace std;

#define WIDTH 784
#define HEIGHT 784
static float _H = 784;
static float _W = 784;

float resolution = 28;

float scale = 10000.0;

float pairf(float x, float y) {
    return 0.5 * (x+y) * (x+y+1) + y;
}
float pair8(vector<float> nums) {
    float first = pairf(nums[0], nums[1])/scale;
    float second = pairf(nums[2], nums[3])/scale;
    float third = pairf(nums[4], nums[5])/scale;
    float fourth = pairf(nums[6], nums[7])/scale;
    
    float ff = pairf(first, second);
    float ss = pairf(third, fourth);
    
    float final = pairf(ff, ss);
    
    return final;
}

float mapf(float s, float a1, float a2, float b1, float b2) {
    float output;
    output = (b1 + (((s - a1) * (b2 - b1)) / (a2 - a1)));
    return output;
}

vector<double> fliph(vector<double> data, int dim = 28) {
    vector<double> result(data.size());
    int indexx = 0;
    for (float x = dim /2; x <= WIDTH; x+=dim) {
        int indexy = 0;
        for (float y = dim /2; y <= HEIGHT; y+=dim) {
            int newx = (dim-1)-indexx;
            int index = indexy*dim+indexx;
            int nindex = indexy*dim+newx;
            result[index] = data[nindex];
            indexy++;
        }
        indexx++;
    }
    return result;
}

int ReverseInt (int i)
{
    unsigned char ch1, ch2, ch3, ch4;
    ch1=i&255;
    ch2=(i>>8)&255;
    ch3=(i>>16)&255;
    ch4=(i>>24)&255;
    return((int)ch1<<24)+((int)ch2<<16)+((int)ch3<<8)+ch4;
}
void ReadMNIST(int NumberOfImages, int DataOfAnImage,vector<vector<double>> &arr, string filepath)
{
    arr.resize(NumberOfImages,vector<double>(DataOfAnImage));
    ifstream file (filepath, ios::binary);
    if (file.is_open())
    {
        int magic_number=0;
        int number_of_images=0;
        int n_rows=0;
        int n_cols=0;
        file.read((char*)&magic_number,sizeof(magic_number));
        magic_number= ReverseInt(magic_number);
        file.read((char*)&number_of_images,sizeof(number_of_images));
        number_of_images= ReverseInt(number_of_images);
        file.read((char*)&n_rows,sizeof(n_rows));
        n_rows= ReverseInt(n_rows);
        file.read((char*)&n_cols,sizeof(n_cols));
        n_cols= ReverseInt(n_cols);
        for(int i=0;i<number_of_images;++i)
        {
            for(int r=0;r<n_rows;++r)
            {
                for(int c=0;c<n_cols;++c)
                {
                    unsigned char temp=0;
                    file.read((char*)&temp,sizeof(temp));
                    arr[i][(n_rows*r)+c]= (double)temp;
                }
            }
            reverse(arr[i].begin(), arr[i].end());
            arr[i] = fliph(arr[i]);
        }
    } else {
        cout << "Couldn't open file at " << filepath << endl;
        return;
    }
}

void showvector(vector<double> img, int dim = 28) {
    int indexx = 0;
    for (float x = dim /2; x <= WIDTH; x+=dim) {
        int indexy = 0;
        for (float y = dim /2; y <= HEIGHT; y+=dim) {
            int index = indexy*dim+indexx;
            float brightness = mapf(img[index], 0, 255, 0, 1);
            float points[] = { mapf(x, 0, WIDTH, -1, 1), mapf(y, 0, HEIGHT, -1, 1) };
            glPointSize(dim);
            glColor4f(brightness, brightness, brightness, 1);
            glBegin(GL_POINTS);
            glVertex2f(points[0], points[1]);
            glEnd();
            indexy++;
        }
        indexx++;
    }
}

void circle(float Cx, float Cy, int radius) {
    int triangleAmount = 20;
        float brightness = 0; //mapf(d, 0, radius, 1, 0.25);
        float twopi = 2*3.14159265;
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(brightness, brightness, brightness, 1);
        float x = mapf(Cx, 0, WIDTH, -1, 1);
        float y = mapf(Cy, 0, HEIGHT, 1, -1);
        glVertex2f(x, y);
        float mappedr = mapf(radius, 0, WIDTH, 0, 1);
        for (int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                       x + (mappedr*cos(i * twopi/triangleAmount)),
                        y + (mappedr*sin(i * twopi/triangleAmount))
                       );
        }
        glEnd();
}


unsigned char* read_mnist_labels(string full_path, int number_of_labels) {
    auto reverseInt = [](int i) {
        unsigned char c1, c2, c3, c4;
        c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    };

    typedef unsigned char uchar;

    ifstream file(full_path, ios::binary);

    if(file.is_open()) {
        int magic_number = 0;
        file.read((char *)&magic_number, sizeof(magic_number));
        magic_number = reverseInt(magic_number);

        if(magic_number != 2049) throw runtime_error("Invalid MNIST label file!");

        file.read((char *)&number_of_labels, sizeof(number_of_labels)), number_of_labels = reverseInt(number_of_labels);

        uchar* _dataset = new uchar[number_of_labels];
        for(int i = 0; i < number_of_labels; i++) {
            file.read((char*)&_dataset[i], 1);
        }
        return _dataset;
    } else {
        throw runtime_error("Unable to open file `" + full_path + "`!");
    }
}


unsigned int in(ifstream& icin, unsigned int size) {
    unsigned int ans = 0;
    for (int i = 0; i < size; i++) {
        unsigned char x;
        icin.read((char*)&x, 1);
        unsigned int temp = x;
        ans <<= 8;
        ans += temp;
    }
    return ans;
}

template<typename type>
void scalevect(vector<type>& vect) {
    for (auto& val : vect) {
        val /= 255.0f;
    }
}

bool sure (float sensitivity, vector<float> vec) {
    int max = max_element(vec.begin(),vec.end()) - vec.begin();
    auto maxNum = vec[max];
    vec.erase(vec.begin()+max);
    int secondMax = max_element(vec.begin(),vec.end()) - vec.begin();
    if (maxNum*100.0f-vec[secondMax]*100.0f > sensitivity) {
        return true;
    }
    return false;
}

void delay(int time) {
    time *= CLOCKS_PER_SEC;
    clock_t now = clock();
    while(clock() - now <time);
}

void text(float x, float y, float r, float g, float b, const char *string)
{
  glColor3f( r, g, b );
  glRasterPos2f(x, y);
  int len, i;
  len = (int)strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}

string exePath() {
    char buf [PATH_MAX];
      uint32_t bufsize = PATH_MAX;
      if(!_NSGetExecutablePath(buf, &bufsize))
        puts(buf);
    return string(buf);
}
