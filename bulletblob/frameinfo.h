#ifndef FRAMEINFO_D6F54G654DF654HFD654H65
#define FRAMEINFO_D6F54G654DF654HFD654H65

#include <vector>
#include <glfw3.h>
using namespace std;

struct frameinfo {
	int rwidth, rheight;
	float ratio;
	double delta;
	vector<double> *times;
	GLFWwindow *window;
};

#endif