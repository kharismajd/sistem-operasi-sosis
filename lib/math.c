/* Library Matematika */
/* Berisi mod dan div */

/* Deklarasi fungsi */
int mod(int a, int b);
int div(int a, int b);

int mod(int a, int b) {
    while(a >= b){
        a = a - b;
    }
    return a;
}

/* Implementasi fungsi */

int div(int a, int b) {
	int result = 0;
	while (a >= b) {
		a = a - b;
		result++;
	}
	return result;
}