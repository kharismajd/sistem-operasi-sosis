#include "math.h"

int mod(int a, int b) {
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b) {
	int result = 0;
	while (a >= b) {
		a = a - b;
		result++;
	}
	return result;
}