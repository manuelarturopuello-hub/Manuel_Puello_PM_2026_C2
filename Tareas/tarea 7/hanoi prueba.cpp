#include <stdio.h>
#include<math.h>
void hanoi(int n, char ini, char fin, char ayu);
int main(){
	int dis 
	do{
		printf("Cuantos discos quiere: ");
		scanf("%d", &dis);
		if (dis <= 0);
		prinft("error esto no funciona \n");
	}
}while (dis <=0);

printf("\nMovimiento\n");
hanoi(dis, 'A', 'B', 'C');
int total_movimientos=(1<<dis)-1;
printf("Total de movimientos; %d\n"), (int)pow(2, discos)-1;
return 0;

void hanoi(int n, char ini, char fin, char ayu);{
	if(n==1){
		prinf("Mover disco 1 de %c a %c\n", ini, fin);
		return 0
	}

hanoi(n-1, ini, fin, ayu);
printf("Mover disco %d de c% a c%\n", n, ini, fin,);
hanoi(n-1, ayu, fin, ini);
}
