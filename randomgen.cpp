#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main(){
	int i = 0;
	ofstream file("randomdata.txt");
	while (i < 1000){
		file << "1 2 3 4 5" << endl;
		sleep(10);
		i++;
	} 
	return 0;
}
