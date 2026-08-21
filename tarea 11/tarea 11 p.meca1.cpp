#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE      80
#define MAX_RUTA        256
#define CAPACIDAD_INI   4
#define ARCHIVO_DEFECTO "riesgos.csv"

/* ---------------------------- Estructuras -------------------------------- */

typedef struct {
    int  id;
    char nombre[MAX_NOMBRE];
    int  F, S, P, E, A, V;     /* factores Mosler, valores 1..5            */
    long importancia;          /* C = I + D, con I=F*S y D=P*E             */
    long probabilidad;         /* A*V                                      */
    long gravedad;              /* importancia*probabilidad                 */
    char nivel[20];            /* clasificacion textual de la gravedad     */
    int  calculado;            /* 0 = aun no calculado, 1 = calculado      */
} Riesgo;

typedef struct {
    Riesgo *riesgos;           /* arreglo con memoria dinamica             */
    int     cantidad;          /* riesgos actualmente almacenados          */
    int     capacidad;         /* espacio reservado en memoria             */
    int     siguienteId;       /* autoincremental para el id del riesgo    */
    char    archivoActual[MAX_RUTA];
} GestorRiesgos;

/* ------------------------- Prototipos de funciones ------------------------ */

void inicializarGestor(GestorRiesgos *g);
void liberarGestor(GestorRiesgos *g);
int  asegurarCapacidad(GestorRiesgos *g);

void crearRiesgo(GestorRiesgos *g);
void modificarRiesgo(GestorRiesgos *g);
void borrarRiesgo(GestorRiesgos *g);
Riesgo *buscarPorId(GestorRiesgos *g, int id);

void calcularGravedadRiesgo(Riesgo *r);
void calcularRiesgos(GestorRiesgos *g);
const char *clasificarGravedad(long gravedad);

void imprimirRiesgos(GestorRiesgos *g);
void imprimirEncabezado(void);
void imprimirFila(Riesgo *r);

void seleccionarArchivo(GestorRiesgos *g);
void crearArchivoNuevo(GestorRiesgos *g);

void guardarArchivo(GestorRiesgos *g);
void guardarComoTXT(GestorRiesgos *g, const char *ruta);
void guardarComoCSV(GestorRiesgos *g, const char *ruta);
void guardarComoJSON(GestorRiesgos *g, const char *ruta);
void guardarComoXML(GestorRiesgos *g, const char *ruta);

void cargarDesdeDisco(GestorRiesgos *g);

int  leerEntero(const char *mensaje, int minimo, int maximo);
void leerTexto(const char *mensaje, char *destino, int tam);
void limpiarBufferEntrada(void);
void pausar(void);

/* ================================ MAIN ==================================== */

int main(void) {
    GestorRiesgos gestor;
    int opcion;

    inicializarGestor(&gestor);

    printf("============================================================\n");
    printf("   SISTEMA DE EVALUACION DE RIESGOS - METODO MOSLER\n");
    printf("============================================================\n");
    printf("Archivo de trabajo por defecto: %s\n", gestor.archivoActual);

    do {
        printf("\n------------------- MENU PRINCIPAL -------------------\n");
        printf(" 1. Crear riesgo\n");
        printf(" 2. Modificar parametros del riesgo\n");
        printf(" 3. Calcular riesgos (metodo Mosler)\n");
        printf(" 4. Imprimir los riesgos\n");
        printf(" 5. Borrar riesgo\n");
        printf(" 6. Seleccionar un archivo especifico\n");
        printf(" 7. Crear un archivo especifico\n");
        printf(" 8. Guardar riesgos en archivo (TXT/JSON/XML/CSV)\n");
        printf(" 9. Cargar riesgos desde disco\n");
        printf(" 0. Salir\n");
        printf("--------------------------------------------------------\n");

        opcion = leerEntero("Seleccione una opcion: ", 0, 9);

        switch (opcion) {
            case 1: crearRiesgo(&gestor);        break;
            case 2: modificarRiesgo(&gestor);    break;
            case 3: calcularRiesgos(&gestor);    break;
            case 4: imprimirRiesgos(&gestor);    break;
            case 5: borrarRiesgo(&gestor);       break;
            case 6: seleccionarArchivo(&gestor); break;
            case 7: crearArchivoNuevo(&gestor);  break;
            case 8: guardarArchivo(&gestor);     break;
            case 9: cargarDesdeDisco(&gestor);   break;
            case 0: printf("\nSaliendo... liberando memoria dinamica.\n"); break;
        }

    } while (opcion != 0);

    liberarGestor(&gestor);
    return 0;
}

/* ============================================================================
 *  GESTION DE MEMORIA DINAMICA
 * ==========================================================================*/

void inicializarGestor(GestorRiesgos *g) {
    g->riesgos = (Riesgo *) malloc(CAPACIDAD_INI * sizeof(Riesgo));
    if (g->riesgos == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria inicial.\n");
        exit(EXIT_FAILURE);
    }
    g->cantidad     = 0;
    g->capacidad    = CAPACIDAD_INI;
    g->siguienteId  = 1;
    strncpy(g->archivoActual, ARCHIVO_DEFECTO, MAX_RUTA - 1);
    g->archivoActual[MAX_RUTA - 1] = '\0';
}

void liberarGestor(GestorRiesgos *g) {
    free(g->riesgos);
    g->riesgos  = NULL;
    g->cantidad = 0;
    g->capacidad = 0;
}

/* Duplica la capacidad del arreglo dinamico cuando se llena (realloc) */
int asegurarCapacidad(GestorRiesgos *g) {
    if (g->cantidad < g->capacidad) return 1;

    int nuevaCapacidad = g->capacidad * 2;
    Riesgo *tmp = (Riesgo *) realloc(g->riesgos, nuevaCapacidad * sizeof(Riesgo));
    if (tmp == NULL) {
        fprintf(stderr, "Error: memoria insuficiente para agregar mas riesgos.\n");
        return 0;
    }
    g->riesgos  = tmp;
    g->capacidad = nuevaCapacidad;
    return 1;
}

/* ============================================================================
 *  1. CREAR RIESGO
 * ==========================================================================*/

void crearRiesgo(GestorRiesgos *g) {
    if (!asegurarCapacidad(g)) return;

    Riesgo nuevo;
    nuevo.id = g->siguienteId;

    printf("\n--- Crear nuevo riesgo (ID %d) ---\n", nuevo.id);
    leerTexto("Nombre / descripcion del riesgo: ", nuevo.nombre, MAX_NOMBRE);

    printf("Ingrese los 6 factores del metodo Mosler (escala 1 a 5):\n");
    nuevo.F = leerEntero("  F - Funcion (importancia de la funcion afectada): ", 1, 5);
    nuevo.S = leerEntero("  S - Sustitucion (dificultad de sustituir): ",         1, 5);
    nuevo.P = leerEntero("  P - Profundidad (perturbacion causada): ",           1, 5);
    nuevo.E = leerEntero("  E - Extension (alcance del dano): ",                 1, 5);
    nuevo.A = leerEntero("  A - Agresion (probabilidad de ocurrencia): ",        1, 5);
    nuevo.V = leerEntero("  V - Vulnerabilidad (probabilidad de dano real): ",   1, 5);

    nuevo.importancia  = 0;
    nuevo.probabilidad = 0;
    nuevo.gravedad      = 0;
    strcpy(nuevo.nivel, "Sin calcular");
    nuevo.calculado     = 0;

    g->riesgos[g->cantidad] = nuevo;
    g->cantidad++;
    g->siguienteId++;

    printf("Riesgo creado correctamente. Use la opcion 3 para calcular su gravedad.\n");
}

/* ============================================================================
 *  2. MODIFICAR PARAMETROS DEL RIESGO
 * ==========================================================================*/

void modificarRiesgo(GestorRiesgos *g) {
    if (g->cantidad == 0) {
        printf("\nNo hay riesgos registrados.\n");
        return;
    }

    imprimirRiesgos(g);
    int id = leerEntero("\nIngrese el ID del riesgo a modificar: ", 1, 999999);
    Riesgo *r = buscarPorId(g, id);
    if (r == NULL) {
        printf("No se encontro un riesgo con ese ID.\n");
        return;
    }

    int opcion;
    do {
        printf("\n--- Modificando riesgo ID %d (%s) ---\n", r->id, r->nombre);
        printf(" 1. Nombre                (%s)\n", r->nombre);
        printf(" 2. F - Funcion           (%d)\n", r->F);
        printf(" 3. S - Sustitucion       (%d)\n", r->S);
        printf(" 4. P - Profundidad       (%d)\n", r->P);
        printf(" 5. E - Extension         (%d)\n", r->E);
        printf(" 6. A - Agresion          (%d)\n", r->A);
        printf(" 7. V - Vulnerabilidad    (%d)\n", r->V);
        printf(" 0. Terminar modificacion\n");

        opcion = leerEntero("Seleccione el parametro a modificar: ", 0, 7);

        switch (opcion) {
            case 1: leerTexto("Nuevo nombre: ", r->nombre, MAX_NOMBRE); break;
            case 2: r->F = leerEntero("Nuevo valor de F (1-5): ", 1, 5); break;
            case 3: r->S = leerEntero("Nuevo valor de S (1-5): ", 1, 5); break;
            case 4: r->P = leerEntero("Nuevo valor de P (1-5): ", 1, 5); break;
            case 5: r->E = leerEntero("Nuevo valor de E (1-5): ", 1, 5); break;
            case 6: r->A = leerEntero("Nuevo valor de A (1-5): ", 1, 5); break;
            case 7: r->V = leerEntero("Nuevo valor de V (1-5): ", 1, 5); break;
        }

        if (opcion != 0) {
            r->calculado = 0;
            strcpy(r->nivel, "Sin calcular");
            printf("Parametro actualizado. Recuerde recalcular (opcion 3) para "
                   "actualizar la gravedad.\n");
        }

    } while (opcion != 0);
}

Riesgo *buscarPorId(GestorRiesgos *g, int id) {
    for (int i = 0; i < g->cantidad; i++) {
        if (g->riesgos[i].id == id) return &g->riesgos[i];
    }
    return NULL;
}

/* ============================================================================
 *  3. CALCULAR RIESGOS (Formula de Mosler)
 * ==========================================================================*/

void calcularGravedadRiesgo(Riesgo *r) {
    long I = (long) r->F * r->S;   /* I = Importancia del suceso (F*S) */
    long D = (long) r->P * r->E;   /* D = Danos ocasionados (P*E)      */
    r->importancia  = I + D;       /* C = I + D                        */
    r->probabilidad = (long) r->A * r->V;
    r->gravedad      = r->importancia * r->probabilidad;
    strncpy(r->nivel, clasificarGravedad(r->gravedad), sizeof(r->nivel) - 1);
    r->nivel[sizeof(r->nivel) - 1] = '\0';
    r->calculado = 1;
}

const char *clasificarGravedad(long gravedad) {
    if (gravedad <= 250)  return "Muy Baja";
    if (gravedad <= 500)  return "Baja";
    if (gravedad <= 750)  return "Media";
    if (gravedad <= 1000) return "Alta";
    return "Muy Alta";
}

void calcularRiesgos(GestorRiesgos *g) {
    if (g->cantidad == 0) {
        printf("\nNo hay riesgos registrados para calcular.\n");
        return;
    }

    for (int i = 0; i < g->cantidad; i++) {
        calcularGravedadRiesgo(&g->riesgos[i]);
    }

    printf("\nSe calculo la gravedad de %d riesgo(s) mediante el metodo Mosler.\n",
           g->cantidad);
    imprimirRiesgos(g);
}

/* ============================================================================
 *  4. IMPRIMIR LOS RIESGOS
 * ==========================================================================*/

void imprimirEncabezado(void) {
    printf("\n%-4s %-22s %3s %3s %3s %3s %3s %3s %6s %6s %8s %-10s\n",
           "ID", "Nombre", "F", "S", "P", "E", "A", "V",
           "Import", "Prob", "Gravedad", "Nivel");
    printf("---------------------------------------------------------------"
           "--------------------\n");
}

void imprimirFila(Riesgo *r) {
    if (r->calculado) {
        printf("%-4d %-22.22s %3d %3d %3d %3d %3d %3d %6ld %6ld %8ld %-10s\n",
               r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V,
               r->importancia, r->probabilidad, r->gravedad, r->nivel);
    } else {
        printf("%-4d %-22.22s %3d %3d %3d %3d %3d %3d %6s %6s %8s %-10s\n",
               r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V,
               "-", "-", "-", "Sin calcular");
    }
}

void imprimirRiesgos(GestorRiesgos *g) {
    if (g->cantidad == 0) {
        printf("\nNo hay riesgos registrados.\n");
        return;
    }

    printf("\n================= LISTADO DE RIESGOS (%d) =================\n",
           g->cantidad);
    imprimirEncabezado();
    for (int i = 0; i < g->cantidad; i++) {
        imprimirFila(&g->riesgos[i]);
    }
}

/* ============================================================================
 *  5. BORRAR RIESGO
 * ==========================================================================*/

void borrarRiesgo(GestorRiesgos *g) {
    if (g->cantidad == 0) {
        printf("\nNo hay riesgos registrados.\n");
        return;
    }

    imprimirRiesgos(g);
    int id = leerEntero("\nIngrese el ID del riesgo a borrar: ", 1, 999999);

    int indice = -1;
    for (int i = 0; i < g->cantidad; i++) {
        if (g->riesgos[i].id == id) { indice = i; break; }
    }

    if (indice == -1) {
        printf("No se encontro un riesgo con ese ID.\n");
        return;
    }

    /* Desplazar los elementos posteriores una posicion hacia atras */
    for (int i = indice; i < g->cantidad - 1; i++) {
        g->riesgos[i] = g->riesgos[i + 1];
    }
    g->cantidad--;

    printf("Riesgo ID %d eliminado correctamente.\n", id);
}

/* ============================================================================
 *  6. SELECCIONAR UN ARCHIVO ESPECIFICO
 * ==========================================================================*/

void seleccionarArchivo(GestorRiesgos *g) {
    char ruta[MAX_RUTA];
    printf("\nArchivo actual: %s\n", g->archivoActual);
    leerTexto("Ingrese la ruta/nombre del archivo a utilizar: ", ruta, MAX_RUTA);

    FILE *f = fopen(ruta, "r");
    if (f == NULL) {
        printf("Aviso: el archivo '%s' no existe todavia. Se usara este nombre "
               "para futuras operaciones de guardado/carga.\n", ruta);
    } else {
        printf("El archivo '%s' existe y fue seleccionado correctamente.\n", ruta);
        fclose(f);
    }

    strncpy(g->archivoActual, ruta, MAX_RUTA - 1);
    g->archivoActual[MAX_RUTA - 1] = '\0';
}

/* ============================================================================
 *  7. CREAR UN ARCHIVO ESPECIFICO
 * ==========================================================================*/

void crearArchivoNuevo(GestorRiesgos *g) {
    char ruta[MAX_RUTA];
    leerTexto("Ingrese el nombre del nuevo archivo a crear: ", ruta, MAX_RUTA);

    FILE *f = fopen(ruta, "w");
    if (f == NULL) {
        printf("Error: no se pudo crear el archivo '%s'.\n", ruta);
        return;
    }
    fclose(f);

    printf("Archivo '%s' creado (vacio) correctamente.\n", ruta);

    strncpy(g->archivoActual, ruta, MAX_RUTA - 1);
    g->archivoActual[MAX_RUTA - 1] = '\0';
    printf("Se establecio '%s' como archivo de trabajo actual.\n", ruta);
}

/* ============================================================================
 *  8. GUARDAR EN ARCHIVO (TXT / JSON / XML / CSV)
 * ==========================================================================*/

void guardarArchivo(GestorRiesgos *g) {
    if (g->cantidad == 0) {
        printf("\nNo hay riesgos para guardar.\n");
        return;
    }

    printf("\nArchivo de trabajo actual: %s\n", g->archivoActual);
    printf("Formatos disponibles:\n");
    printf(" 1. Texto plano (.txt)\n");
    printf(" 2. JSON (.json)\n");
    printf(" 3. XML (.xml)\n");
    printf(" 4. CSV / separado por coma (.csv)  [formato usado para recargar]\n");

    int formato = leerEntero("Seleccione el formato de guardado: ", 1, 4);

    char ruta[MAX_RUTA];
    printf("Presione ENTER para usar '%s' o escriba otra ruta:\n", g->archivoActual);
    leerTexto("Ruta del archivo: ", ruta, MAX_RUTA);
    if (strlen(ruta) == 0) {
        strncpy(ruta, g->archivoActual, MAX_RUTA - 1);
        ruta[MAX_RUTA - 1] = '\0';
    }

    switch (formato) {
        case 1: guardarComoTXT(g, ruta);  break;
        case 2: guardarComoJSON(g, ruta); break;
        case 3: guardarComoXML(g, ruta);  break;
        case 4: guardarComoCSV(g, ruta);  break;
    }

    strncpy(g->archivoActual, ruta, MAX_RUTA - 1);
    g->archivoActual[MAX_RUTA - 1] = '\0';
}

void guardarComoTXT(GestorRiesgos *g, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) { printf("Error al abrir '%s' para escritura.\n", ruta); return; }

    fprintf(f, "REPORTE DE RIESGOS - METODO MOSLER\n");
    fprintf(f, "===================================\n\n");

    for (int i = 0; i < g->cantidad; i++) {
        Riesgo *r = &g->riesgos[i];
        fprintf(f, "ID: %d\n", r->id);
        fprintf(f, "Nombre: %s\n", r->nombre);
        fprintf(f, "F=%d  S=%d  P=%d  E=%d  A=%d  V=%d\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        if (r->calculado) {
            fprintf(f, "Importancia del dano: %ld\n", r->importancia);
            fprintf(f, "Probabilidad: %ld\n", r->probabilidad);
            fprintf(f, "Gravedad: %ld\n", r->gravedad);
            fprintf(f, "Nivel: %s\n", r->nivel);
        } else {
            fprintf(f, "Gravedad: Sin calcular\n");
        }
        fprintf(f, "-----------------------------------\n");
    }

    fclose(f);
    printf("Reporte guardado en texto plano: %s\n", ruta);
}

void guardarComoCSV(GestorRiesgos *g, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) { printf("Error al abrir '%s' para escritura.\n", ruta); return; }

    /* Encabezado */
    fprintf(f, "id,nombre,F,S,P,E,A,V,importancia,probabilidad,gravedad,nivel,calculado\n");

    for (int i = 0; i < g->cantidad; i++) {
        Riesgo *r = &g->riesgos[i];
        fprintf(f, "%d,%s,%d,%d,%d,%d,%d,%d,%ld,%ld,%ld,%s,%d\n",
                r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V,
                r->importancia, r->probabilidad, r->gravedad, r->nivel,
                r->calculado);
    }

    fclose(f);
    printf("Riesgos guardados en formato CSV: %s\n", ruta);
}

void guardarComoJSON(GestorRiesgos *g, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) { printf("Error al abrir '%s' para escritura.\n", ruta); return; }

    fprintf(f, "{\n  \"riesgos\": [\n");
    for (int i = 0; i < g->cantidad; i++) {
        Riesgo *r = &g->riesgos[i];
        fprintf(f, "    {\n");
        fprintf(f, "      \"id\": %d,\n", r->id);
        fprintf(f, "      \"nombre\": \"%s\",\n", r->nombre);
        fprintf(f, "      \"F\": %d,\n", r->F);
        fprintf(f, "      \"S\": %d,\n", r->S);
        fprintf(f, "      \"P\": %d,\n", r->P);
        fprintf(f, "      \"E\": %d,\n", r->E);
        fprintf(f, "      \"A\": %d,\n", r->A);
        fprintf(f, "      \"V\": %d,\n", r->V);
        fprintf(f, "      \"importancia\": %ld,\n", r->importancia);
        fprintf(f, "      \"probabilidad\": %ld,\n", r->probabilidad);
        fprintf(f, "      \"gravedad\": %ld,\n", r->gravedad);
        fprintf(f, "      \"nivel\": \"%s\"\n", r->calculado ? r->nivel : "Sin calcular");
        fprintf(f, "    }%s\n", (i < g->cantidad - 1) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");

    fclose(f);
    printf("Riesgos guardados en formato JSON: %s\n", ruta);
}

void guardarComoXML(GestorRiesgos *g, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) { printf("Error al abrir '%s' para escritura.\n", ruta); return; }

    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<riesgos>\n");
    for (int i = 0; i < g->cantidad; i++) {
        Riesgo *r = &g->riesgos[i];
        fprintf(f, "  <riesgo id=\"%d\">\n", r->id);
        fprintf(f, "    <nombre>%s</nombre>\n", r->nombre);
        fprintf(f, "    <factores F=\"%d\" S=\"%d\" P=\"%d\" E=\"%d\" A=\"%d\" V=\"%d\"/>\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "    <importancia>%ld</importancia>\n", r->importancia);
        fprintf(f, "    <probabilidad>%ld</probabilidad>\n", r->probabilidad);
        fprintf(f, "    <gravedad>%ld</gravedad>\n", r->gravedad);
        fprintf(f, "    <nivel>%s</nivel>\n", r->calculado ? r->nivel : "Sin calcular");
        fprintf(f, "  </riesgo>\n");
    }
    fprintf(f, "</riesgos>\n");

    fclose(f);
    printf("Riesgos guardados en formato XML: %s\n", ruta);
}

/* ============================================================================
 *  9. CARGAR RIESGOS DESDE DISCO (memoria dinamica, formato CSV)
 * ==========================================================================*/

void cargarDesdeDisco(GestorRiesgos *g) {
    char ruta[MAX_RUTA];
    printf("\nArchivo actual: %s\n", g->archivoActual);
    printf("Presione ENTER para usarlo o escriba otra ruta CSV a cargar:\n");
    leerTexto("Ruta del archivo CSV: ", ruta, MAX_RUTA);
    if (strlen(ruta) == 0) {
        strncpy(ruta, g->archivoActual, MAX_RUTA - 1);
        ruta[MAX_RUTA - 1] = '\0';
    }

    FILE *f = fopen(ruta, "r");
    if (f == NULL) {
        printf("Error: no se pudo abrir '%s'.\n", ruta);
        return;
    }

    char linea[512];
    /* Descartar encabezado */
    if (fgets(linea, sizeof(linea), f) == NULL) {
        printf("El archivo esta vacio.\n");
        fclose(f);
        return;
    }

    /* Liberamos lo que hubiera y reiniciamos el arreglo dinamico */
    free(g->riesgos);
    g->capacidad = CAPACIDAD_INI;
    g->riesgos = (Riesgo *) malloc(g->capacidad * sizeof(Riesgo));
    if (g->riesgos == NULL) {
        fprintf(stderr, "Error: memoria insuficiente al cargar.\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    g->cantidad = 0;
    int maxId = 0;

    while (fgets(linea, sizeof(linea), f) != NULL) {
        /* Quitar salto de linea */
        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) == 0) continue;

        /* Redimensionar dinamicamente si hace falta (realloc) */
        if (!asegurarCapacidad(g)) break;

        Riesgo r;
        int leidos = sscanf(linea, "%d,%79[^,],%d,%d,%d,%d,%d,%d,%ld,%ld,%ld,%19[^,],%d",
                             &r.id, r.nombre, &r.F, &r.S, &r.P, &r.E, &r.A, &r.V,
                             &r.importancia, &r.probabilidad, &r.gravedad,
                             r.nivel, &r.calculado);

        if (leidos == 13) {
            g->riesgos[g->cantidad] = r;
            g->cantidad++;
            if (r.id > maxId) maxId = r.id;
        }
    }

    fclose(f);

    g->siguienteId = maxId + 1;
    strncpy(g->archivoActual, ruta, MAX_RUTA - 1);
    g->archivoActual[MAX_RUTA - 1] = '\0';

    printf("Se cargaron %d riesgo(s) desde '%s' usando memoria dinamica.\n",
           g->cantidad, ruta);
}

/* ============================================================================
 *  UTILIDADES DE ENTRADA
 * ==========================================================================*/

void limpiarBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
}

int leerEntero(const char *mensaje, int minimo, int maximo) {
    int valor;
    int valido;

    do {
        printf("%s", mensaje);
        valido = scanf("%d", &valor);
        limpiarBufferEntrada();

        if (!valido) {
            printf("Entrada invalida. Ingrese un numero entero.\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("El valor debe estar entre %d y %d.\n", minimo, maximo);
            valido = 0;
        }
    } while (!valido);

    return valor;
}

void leerTexto(const char *mensaje, char *destino, int tam) {
    printf("%s", mensaje);
    if (fgets(destino, tam, stdin) != NULL) {
        destino[strcspn(destino, "\r\n")] = '\0';
    } else {
        destino[0] = '\0';
    }
}
