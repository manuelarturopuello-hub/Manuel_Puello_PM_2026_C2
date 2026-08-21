#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int opcion = 0;
    int cantidad_jugadas = 1; /* Por defecto inicia en 1 jugada */
    int numero_jugado, dinero_jugado;
    int numero_ganador;
    int i;

    while (opcion != 3)
    {
        /* Despliegue del Menú Principal */
        printf("\n========== LOTERIA ==========");
        printf("\n1. Jugar");
        printf("\n2. Elegir cantidad de jugadas (Actual: %d)", cantidad_jugadas);
        printf("\n3. Salir");
        printf("\n=============================");
        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);

        if (opcion == 1)
        {
            /* Bucle para ejecutar la cantidad de jugadas seleccionadas */
            for (i = 0; i < cantidad_jugadas; i++)
            {
                printf("\n--- JUGADA %d DE %d ---", i + 1, cantidad_jugadas);

                /* Entrada y validación del número (Debe ser entre 1 y 40) */
                printf("\nIngresa tu numero de la suerte (1 a 40): ");
                scanf("%d", &numero_jugado);

                while (numero_jugado < 1 || numero_jugado > 40)
                {
                    printf("Numero invalido. Recuerda que debe ser entre 1 y 40: ");
                    scanf("%d", &numero_jugado);
                }

                /* Entrada de la cantidad de dinero a apostar */
                printf("Ingresa la cantidad de dinero a jugar: ");
                scanf("%d", &dinero_jugado);

                while (dinero_jugado <= 0)
                {
                    printf("La cantidad debe ser mayor a 0. Ingresa el dinero: ");
                    scanf("%d", &dinero_jugado);
                }

                /* Generación del número ganador de forma aleatoria entre 1 y 40 */
                /* Usamos el operador módulo (%) que vimos en el programa de Primos */
                numero_ganador = (rand() % 40) + 1;
                printf("\nEl numero ganador de la loteria es: [%d]", numero_ganador);

                /* Evaluación del premio */
                if (numero_jugado == numero_ganador)
                {
                    printf("\n¡Felicidades! Has acertado el numero.");
                    printf("\nTu premio es: $%d\n", dinero_jugado * 1000);
                }
                else
                {
                    printf("\nLo siento, no has acertado esta vez. ¡Sigue intentando!\n");
                }
            }
        }
        else if (opcion == 2)
        {
            /* Opción para configurar la cantidad de jugadas */
            printf("\nIngresa la cantidad de jugadas que deseas realizar: ");
            scanf("%d", &cantidad_jugadas);

            while (cantidad_jugadas <= 0)
            {
                printf("Cantidad invalida. Debe ser por lo menos 1 jugada: ");
                scanf("%d", &cantidad_jugadas);
            }
            printf("¡Configurado con exito! Ahora realizaras %d jugadas por turno.\n", cantidad_jugadas);
        }
        else if (opcion == 3)
        {
            printf("\nGracias por utilizar el programa de loteria. ¡Hasta luego!\n");
        }
        else
        {
            printf("\nOpcion no valida. Por favor, selecciona una opcion del 1 al 3.\n");
        }
    }

    return 0;
}
