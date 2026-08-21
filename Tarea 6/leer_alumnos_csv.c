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

/* Elimina el salto de linea final ('\n' o '\r\n') de una cadena */
void quitarSaltoLinea(char *cadena) {
    cadena[strcspn(cadena, "\r\n")] = '\0';
}

/* Primera pasada: cuenta cuantas lineas de datos hay (sin contar el encabezado) */
int contarAlumnos(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;

    /* Descartar la linea de encabezado (Nombre,Apellido,Promedio,Materia) */
    fgets(linea, MAX_LINEA, archivo);

    while (fgets(linea, MAX_LINEA, archivo) != NULL) {
        /* Ignorar posibles lineas vacias al final del archivo */
        quitarSaltoLinea(linea);
        if (strlen(linea) > 0) {
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
    int indice = 0;

    /* Descartar el encabezado */
    fgets(linea, MAX_LINEA, archivo);

    while (fgets(linea, MAX_LINEA, archivo) != NULL && indice < n) {
        quitarSaltoLinea(linea);
        if (strlen(linea) == 0) {
            continue;
        }

        char *token;

        token = strtok(linea, ",");
        if (token != NULL) strncpy(lista[indice].nombre, token, MAX_CAMPO - 1);

        token = strtok(NULL, ",");
        if (token != NULL) strncpy(lista[indice].apellido, token, MAX_CAMPO - 1);

        token = strtok(NULL, ",");
        if (token != NULL) lista[indice].promedio = atof(token);

        token = strtok(NULL, ",");
        if (token != NULL) strncpy(lista[indice].materia, token, MAX_CAMPO - 1);

        indice++;
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
    const char *nombreArchivo = "alumnos.csv";
    if (argc > 1) {
        nombreArchivo = argv[1];
    }

    /* Paso 1: contar cuantas filas de datos hay */
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

    /* Paso 3: cargar los datos del CSV en el array */
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
