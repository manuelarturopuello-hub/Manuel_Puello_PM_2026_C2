#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 200
#define MAX_CAMPO 50

typedef struct {
    char nombre[MAX_CAMPO];
    char apellido[MAX_CAMPO];
    float promedio;
    char materia[MAX_CAMPO];
} Alumno;

/* Extrae el valor entre comillas de una linea tipo:  "Nombre": "Juan", */
void extraerString(char *linea, char *destino) {
    char *inicio = strchr(linea, ':');
    if (inicio == NULL) {
        destino[0] = '\0';
        return;
    }
    inicio = strchr(inicio, '"');
    if (inicio == NULL) {
        destino[0] = '\0';
        return;
    }
    inicio++; /* saltar la comilla de apertura */

    char *fin = strchr(inicio, '"');
    if (fin == NULL) {
        destino[0] = '\0';
        return;
    }

    int longitud = fin - inicio;
    strncpy(destino, inicio, longitud);
    destino[longitud] = '\0';
}

/* Extrae el valor numerico de una linea tipo:  "Promedio": 8.5, */
float extraerNumero(char *linea) {
    char *inicio = strchr(linea, ':');
    if (inicio == NULL) {
        return 0.0f;
    }
    inicio++; /* saltar los ':' */
    return atof(inicio);
}

/* Primera pasada: cuenta cuantos elementos hay contando la clave "Nombre" */
int contarAlumnos(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;

    while (fgets(linea, MAX_LINEA, archivo) != NULL) {
        if (strstr(linea, "\"Nombre\"") != NULL) {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

/* Segunda pasada: carga los datos en el array ya reservado */
int cargarAlumnos(const char *nombreArchivo, Alumno *lista, int n) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }

    char linea[MAX_LINEA];
    int indice = -1;

    while (fgets(linea, MAX_LINEA, archivo) != NULL && indice < n) {
        if (strstr(linea, "\"Nombre\"") != NULL) {
            indice++;
            extraerString(linea, lista[indice].nombre);
        } else if (strstr(linea, "\"Apellido\"") != NULL) {
            extraerString(linea, lista[indice].apellido);
        } else if (strstr(linea, "\"Promedio\"") != NULL) {
            lista[indice].promedio = extraerNumero(linea);
        } else if (strstr(linea, "\"Materia\"") != NULL) {
            extraerString(linea, lista[indice].materia);
        }
    }

    fclose(archivo);
    return 1;
}

void imprimirAlumnos(Alumno *lista, int n) {
    printf("\n=== Listado de Alumnos (%d) ===\n\n", n);
    for (int i = 0; i < n; i++) {
        printf("Alumno %d:\n", i + 1);
        printf("  Nombre:   %s\n", lista[i].nombre);
        printf("  Apellido: %s\n", lista[i].apellido);
        printf("  Promedio: %.1f\n", lista[i].promedio);
        printf("  Materia:  %s\n", lista[i].materia);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    const char *nombreArchivo = "alumnos.json";
    if (argc > 1) {
        nombreArchivo = argv[1];
    }

    /* Paso 1: contar cuantos elementos hay */
    int n = contarAlumnos(nombreArchivo);
    if (n <= 0) {
        printf("No se encontraron alumnos en el archivo, o hubo un error.\n");
        return 1;
    }
    printf("Cantidad de alumnos encontrados: %d\n", n);

    /* Paso 2: reservar memoria dinamica para n elementos */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (alumnos == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return 1;
    }

    /* Paso 3: cargar los datos del JSON en el array */
    if (!cargarAlumnos(nombreArchivo, alumnos, n)) {
        free(alumnos);
        return 1;
    }

    /* Paso 4: imprimir los datos cargados */
    imprimirAlumnos(alumnos, n);

    /* Paso 5: liberar la memoria reservada */
    free(alumnos);

    return 0;
}
