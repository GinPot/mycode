#include <stdio.h>



int factorizate(int n, unsigned int *a, unsigned int *b)
{
	unsigned int i, j, tmp;
	int m;
	
	if(n < 0)
		return 0;
	
	for(i=0;;i++){
		m = n - i*i;
		if(m <= 0)
			return 0;
		for(j=i;;j++){
			tmp = j*j;
			if(m == tmp){
				*a = i;
				*b = j;
				return 1;
			}else{
				if(m < tmp)
					break;
			}
		}
		
		if(n < i*i)
			break;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int i, num;
	unsigned int a, b;
	int arry[] = {
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		56,
		78,
		617271605,
		-13432412,
		765874354,

	};
	num = sizeof(arry)/sizeof(arry[0]);

	for(i=0; i<num; i++){
		if(factorizate(arry[i], &a, &b) == 1)
			printf("n=%d a=%d b=%d\n", arry[i], a, b);
		else
			printf("n=%d fail.\n", arry[i]);
	}

	return 0;
}















