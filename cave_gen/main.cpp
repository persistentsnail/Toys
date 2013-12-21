#include <iostream>
#include <vector>
using namespace std;
#include "cave_gen.h"


int main(int argc, char *argv) {
	CaveMapGenerator cmg(45,180);
	cmg.generation(0.4, 4, 5);
	cmg.dump();
	return 0;
}