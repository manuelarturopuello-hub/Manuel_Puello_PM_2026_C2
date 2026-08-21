

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VEHICULOS      100
#define MAX_COMBUSTIBLES   20
#define MAX_NOMBRE         100
#define MAX_TIPO           30
#define ARCHIVO_VEHICULOS  "vehiculos.dat"
#define ARCHIVO_COMBUSTIBLES "combustibles.dat"

/* ------------------------------------------------------------------ */
/* ESTRUCTURAS DE DATOS                                                */
/* ------------------------------------------------------------------ */

typedef struct {
    int usado;                          /* 1 = ocupado, 0 = borrado/libre */
    char nombre[MAX_NOMBRE];
    double costo_vehiculo;
    double vida_util_anios;
    double vida_util_km;
    double mantenimiento_anual;
    double seguro_anual;
    double costo_neumatico;
    int cantidad_neumaticos;
    double vida_util_neumaticos_km;
    char tipo_combustible[MAX_TIPO];
    double consumo_ciudad_l100km;
    double consumo_autopista_l100km;
} Vehiculo;

typedef struct {
    char nombre[MAX_TIPO];
    double precio;
} Combustible;

typedef struct {
    double amortizacion_km;
    double mantenimiento_km;
    double seguro_km;
    double neumaticos_km;
    double costo_fijo_km;
    double consumo_promedio_l100km;
    double combustible_ciudad_km;
    double combustible_autopista_km;
    double costo_ciudad_km;
    double costo_autopista_km;
} CostosVehiculo;

typedef struct {
    double km_ciudad;
    double km_autopista;
    double km_totales;
    double costo_combustible_viaje;
    double costo_fijo_viaje;
    double costo_total_viaje;
    double costo_por_km_promedio;
} ResultadoViaje;

/* Variables globales en memoria (se cargan/guardan desde/hacia disco) */
static Vehiculo vehiculos[MAX_VEHICULOS];
static int numSlotsVehiculos = 0;      /* tamano actual del arreglo usado */

static Combustible combustibles[MAX_COMBUSTIBLES];
static int numCombustibles = 0;

/* ------------------------------------------------------------------ */
/* PERSISTENCIA (guardar / cargar archivos binarios)                  */
/* ------------------------------------------------------------------ */

void cargarVehiculos(void) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "rb");
    if (f == NULL) {
        numSlotsVehiculos = 0;
        return;
    }
    fread(&numSlotsVehiculos, sizeof(int), 1, f);
    fread(vehiculos, sizeof(Vehiculo), numSlotsVehiculos, f);
    fclose(f);
}

void guardarVehiculos(void) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "wb");
    if (f == NULL) {
        printf("  -> Error: no se pudo guardar el archivo de vehiculos.\n");
        return;
    }
    fwrite(&numSlotsVehiculos, sizeof(int), 1, f);
    fwrite(vehiculos, sizeof(Vehiculo), numSlotsVehiculos, f);
    fclose(f);
}

void cargarCombustibles(void) {
    FILE *f = fopen(ARCHIVO_COMBUSTIBLES, "rb");
    if (f == NULL) {
        /* Valores por defecto la primera vez que se ejecuta el programa */
        numCombustibles = 3;
        strcpy(combustibles[0].nombre, "gasolina"); combustibles[0].precio = 250.0;
        strcpy(combustibles[1].nombre, "diesel");   combustibles[1].precio = 230.0;
        strcpy(combustibles[2].nombre, "gnv");      combustibles[2].precio = 120.0;
        return;
    }
    fread(&numCombustibles, sizeof(int), 1, f);
    fread(combustibles, sizeof(Combustible), numCombustibles, f);
    fclose(f);
}

void guardarCombustibles(void) {
    FILE *f = fopen(ARCHIVO_COMBUSTIBLES, "wb");
    if (f == NULL) {
        printf("  -> Error: no se pudo guardar el archivo de combustibles.\n");
        return;
    }
    fwrite(&numCombustibles, sizeof(int), 1, f);
    fwrite(combustibles, sizeof(Combustible), numCombustibles, f);
    fclose(f);
}

/* ------------------------------------------------------------------ */
/* UTILIDADES DE ENTRADA DE DATOS                                     */
/* ------------------------------------------------------------------ */

/* Lee una linea completa de stdin de forma segura */
void leerLinea(char *buffer, int tam) {
    if (fgets(buffer, tam, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

double pedirFloat(const char *mensaje, double minimo) {
    char linea[64];
    double valor;
    char *fin;
    while (1) {
        printf("%s", mensaje);
        leerLinea(linea, sizeof(linea));
        valor = strtod(linea, &fin);
        if (fin == linea || *fin != '\0') {
            printf("  -> Ingrese un numero valido.\n");
            continue;
        }
        if (valor < minimo) {
            printf("  -> El valor debe ser mayor o igual a %.2f.\n", minimo);
            continue;
        }
        return valor;
    }
}

int pedirInt(const char *mensaje, int minimo) {
    char linea[64];
    long valor;
    char *fin;
    while (1) {
        printf("%s", mensaje);
        leerLinea(linea, sizeof(linea));
        valor = strtol(linea, &fin, 10);
        if (fin == linea || *fin != '\0') {
            printf("  -> Ingrese un numero entero valido.\n");
            continue;
        }
        if (valor < minimo) {
            printf("  -> El valor debe ser mayor o igual a %d.\n", minimo);
            continue;
        }
        return (int)valor;
    }
}

void pedirTexto(const char *mensaje, char *destino, int tam) {
    while (1) {
        printf("%s", mensaje);
        leerLinea(destino, tam);
        if (strlen(destino) > 0) {
            return;
        }
        printf("  -> Este campo no puede estar vacio.\n");
    }
}

/* Convierte una cadena a minusculas, en el propio buffer */
void aMinusculas(char *s) {
    for (; *s; s++) {
        *s = (char) tolower((unsigned char) *s);
    }
}

/* ------------------------------------------------------------------ */
/* MANEJO DE PRECIOS DE COMBUSTIBLE                                   */
/* ------------------------------------------------------------------ */

int buscarCombustible(const char *tipo) {
    for (int i = 0; i < numCombustibles; i++) {
        if (strcmp(combustibles[i].nombre, tipo) == 0) {
            return i;
        }
    }
    return -1;
}

/* Si el tipo no existe, pregunta su precio y lo agrega. Devuelve el precio final. */
double obtenerOAgregarPrecioCombustible(const char *tipo) {
    int idx = buscarCombustible(tipo);
    if (idx != -1) {
        return combustibles[idx].precio;
    }
    if (numCombustibles >= MAX_COMBUSTIBLES) {
        printf("  -> Limite de tipos de combustible alcanzado. Se usara precio 0.\n");
        return 0.0;
    }
    char mensaje[80];
    snprintf(mensaje, sizeof(mensaje), "'%s' es nuevo. Precio por litro/unidad: ", tipo);
    double precio = pedirFloat(mensaje, 0);
    strncpy(combustibles[numCombustibles].nombre, tipo, MAX_TIPO - 1);
    combustibles[numCombustibles].nombre[MAX_TIPO - 1] = '\0';
    combustibles[numCombustibles].precio = precio;
    numCombustibles++;
    guardarCombustibles();
    return precio;
}

/* ------------------------------------------------------------------ */
/* LOGICA DE CALCULO DE COSTOS                                        */
/* ------------------------------------------------------------------ */

CostosVehiculo calcularCostosVehiculo(const Vehiculo *v, double precioCombustible) {
    CostosVehiculo c;

    /* 1) Amortizacion = costo del vehiculo / vida util en km */
    c.amortizacion_km = v->costo_vehiculo / v->vida_util_km;

    /* 2) Mantenimiento total durante la vida util / vida util en km */
    double mantenimientoTotal = v->mantenimiento_anual * v->vida_util_anios;
    c.mantenimiento_km = mantenimientoTotal / v->vida_util_km;

    /* Seguro total durante la vida util / vida util en km */
    double seguroTotal = v->seguro_anual * v->vida_util_anios;
    c.seguro_km = seguroTotal / v->vida_util_km;

    /* Neumaticos: costo de un juego completo / km que dura ese juego */
    double costoJuegoNeumaticos = v->costo_neumatico * v->cantidad_neumaticos;
    c.neumaticos_km = costoJuegoNeumaticos / v->vida_util_neumaticos_km;

    /* Costo fijo por km (no depende del tipo de recorrido) */
    c.costo_fijo_km = c.amortizacion_km + c.mantenimiento_km + c.seguro_km + c.neumaticos_km;

    /* 3) Combustible: consumo (L/100km) convertido a costo por km */
    c.combustible_ciudad_km = (v->consumo_ciudad_l100km / 100.0) * precioCombustible;
    c.combustible_autopista_km = (v->consumo_autopista_l100km / 100.0) * precioCombustible;

    c.consumo_promedio_l100km = (v->consumo_ciudad_l100km + v->consumo_autopista_l100km) / 2.0;

    /* 4) y 5) Costo real por km en cada escenario */
    c.costo_ciudad_km = c.costo_fijo_km + c.combustible_ciudad_km;
    c.costo_autopista_km = c.costo_fijo_km + c.combustible_autopista_km;

    return c;
}

ResultadoViaje calcularViaje(const Vehiculo *v, double precioCombustible,
                              double kmCiudad, double kmAutopista) {
    ResultadoViaje r;
    CostosVehiculo c = calcularCostosVehiculo(v, precioCombustible);

    r.km_ciudad = kmCiudad;
    r.km_autopista = kmAutopista;
    r.km_totales = kmCiudad + kmAutopista;

    r.costo_combustible_viaje = kmCiudad * c.combustible_ciudad_km
                               + kmAutopista * c.combustible_autopista_km;
    r.costo_fijo_viaje = r.km_totales * c.costo_fijo_km;
    r.costo_total_viaje = r.costo_combustible_viaje + r.costo_fijo_viaje;
    r.costo_por_km_promedio = (r.km_totales > 0) ? (r.costo_total_viaje / r.km_totales) : 0;

    return r;
}

/* ------------------------------------------------------------------ */
/* OPERACIONES DEL MENU                                                */
/* ------------------------------------------------------------------ */

/* Muestra la lista de vehiculos activos y devuelve el indice real
   dentro del arreglo global, o -1 si se cancela / no hay vehiculos. */
int seleccionarVehiculo(void) {
    int indicesActivos[MAX_VEHICULOS];
    int cantidad = 0;

    for (int i = 0; i < numSlotsVehiculos; i++) {
        if (vehiculos[i].usado) {
            indicesActivos[cantidad] = i;
            cantidad++;
        }
    }

    if (cantidad == 0) {
        printf("\nNo hay vehiculos registrados todavia.\n\n");
        return -1;
    }

    printf("\nVehiculos registrados:\n");
    for (int j = 0; j < cantidad; j++) {
        Vehiculo *v = &vehiculos[indicesActivos[j]];
        printf("  %d) %s  (combustible: %s)\n", j + 1, v->nombre, v->tipo_combustible);
    }

    char mensaje[64];
    snprintf(mensaje, sizeof(mensaje), "Seleccione un vehiculo (1-%d), 0 para cancelar: ", cantidad);
    int idx = pedirInt(mensaje, 0);
    if (idx == 0 || idx > cantidad) {
        return -1;
    }
    return indicesActivos[idx - 1];
}

void crearVehiculo(void) {
    printf("\n--- CREAR VEHICULO ---\n");

    if (numSlotsVehiculos >= MAX_VEHICULOS) {
        printf("  -> Se alcanzo el limite maximo de %d vehiculos.\n", MAX_VEHICULOS);
        return;
    }

    Vehiculo v;
    memset(&v, 0, sizeof(Vehiculo));
    v.usado = 1;

    pedirTexto("Nombre/alias del vehiculo (ej: Toyota Corolla 2022): ", v.nombre, MAX_NOMBRE);
    v.costo_vehiculo = pedirFloat("Costo de compra del vehiculo: ", 0);
    v.vida_util_anios = pedirFloat("Vida util estimada (anios): ", 0.1);
    v.vida_util_km = pedirFloat("Vida util estimada (km totales): ", 1);
    v.mantenimiento_anual = pedirFloat("Gasto de mantenimiento ANUAL promedio: ", 0);
    v.seguro_anual = pedirFloat("Costo del seguro ANUAL: ", 0);
    v.costo_neumatico = pedirFloat("Costo de UN neumatico: ", 0);
    v.cantidad_neumaticos = pedirInt("Cantidad de neumaticos del vehiculo: ", 1);
    v.vida_util_neumaticos_km = pedirFloat("Duracion de un juego de neumaticos (km): ", 1);

    printf("Tipos de combustible disponibles: ");
    for (int i = 0; i < numCombustibles; i++) {
        printf("%s%s", combustibles[i].nombre, (i < numCombustibles - 1) ? ", " : "\n");
    }
    if (numCombustibles == 0) printf("(ninguno todavia)\n");

    char tipoTemp[MAX_TIPO];
    pedirTexto("Tipo de combustible que usa: ", tipoTemp, MAX_TIPO);
    aMinusculas(tipoTemp);
    strcpy(v.tipo_combustible, tipoTemp);
    obtenerOAgregarPrecioCombustible(v.tipo_combustible);

    v.consumo_ciudad_l100km = pedirFloat("Consumo en CIUDAD (litros por cada 100 km): ", 0);
    v.consumo_autopista_l100km = pedirFloat("Consumo en AUTOPISTA (litros por cada 100 km): ", 0);

    /* Busca un slot libre (previamente borrado); si no hay, agrega uno nuevo */
    int slotLibre = -1;
    for (int i = 0; i < numSlotsVehiculos; i++) {
        if (!vehiculos[i].usado) {
            slotLibre = i;
            break;
        }
    }
    if (slotLibre == -1) {
        slotLibre = numSlotsVehiculos;
        numSlotsVehiculos++;
    }
    vehiculos[slotLibre] = v;

    guardarVehiculos();
    printf("\nVehiculo '%s' creado y guardado correctamente.\n", v.nombre);
}

void borrarVehiculo(void) {
    printf("\n--- BORRAR VEHICULO ---\n");
    int idx = seleccionarVehiculo();
    if (idx == -1) return;

    char respuesta[16];
    printf("Confirma borrar '%s'? (s/n): ", vehiculos[idx].nombre);
    leerLinea(respuesta, sizeof(respuesta));

    if (respuesta[0] == 's' || respuesta[0] == 'S') {
        vehiculos[idx].usado = 0;
        guardarVehiculos();
        printf("Vehiculo eliminado.\n");
    } else {
        printf("Operacion cancelada.\n");
    }
}

void modificarVehiculo(void) {
    printf("\n--- MODIFICAR VEHICULO ---\n");
    int idx = seleccionarVehiculo();
    if (idx == -1) return;
    Vehiculo *v = &vehiculos[idx];

    printf("\nEditando: %s\n", v->nombre);
    printf("  1) nombre = %s\n", v->nombre);
    printf("  2) costo_vehiculo = %.2f\n", v->costo_vehiculo);
    printf("  3) vida_util_anios = %.2f\n", v->vida_util_anios);
    printf("  4) vida_util_km = %.2f\n", v->vida_util_km);
    printf("  5) mantenimiento_anual = %.2f\n", v->mantenimiento_anual);
    printf("  6) seguro_anual = %.2f\n", v->seguro_anual);
    printf("  7) costo_neumatico = %.2f\n", v->costo_neumatico);
    printf("  8) cantidad_neumaticos = %d\n", v->cantidad_neumaticos);
    printf("  9) vida_util_neumaticos_km = %.2f\n", v->vida_util_neumaticos_km);
    printf("  10) tipo_combustible = %s\n", v->tipo_combustible);
    printf("  11) consumo_ciudad_l100km = %.2f\n", v->consumo_ciudad_l100km);
    printf("  12) consumo_autopista_l100km = %.2f\n", v->consumo_autopista_l100km);
    printf("  0) Cancelar\n");

    int opcion = pedirInt("Que campo desea modificar?: ", 0);

    switch (opcion) {
        case 1:
            pedirTexto("Nuevo nombre: ", v->nombre, MAX_NOMBRE);
            break;
        case 2:
            v->costo_vehiculo = pedirFloat("Nuevo costo del vehiculo: ", 0);
            break;
        case 3:
            v->vida_util_anios = pedirFloat("Nueva vida util (anios): ", 0.1);
            break;
        case 4:
            v->vida_util_km = pedirFloat("Nueva vida util (km): ", 1);
            break;
        case 5:
            v->mantenimiento_anual = pedirFloat("Nuevo mantenimiento anual: ", 0);
            break;
        case 6:
            v->seguro_anual = pedirFloat("Nuevo seguro anual: ", 0);
            break;
        case 7:
            v->costo_neumatico = pedirFloat("Nuevo costo por neumatico: ", 0);
            break;
        case 8:
            v->cantidad_neumaticos = pedirInt("Nueva cantidad de neumaticos: ", 1);
            break;
        case 9:
            v->vida_util_neumaticos_km = pedirFloat("Nueva duracion de neumaticos (km): ", 1);
            break;
        case 10: {
            char tipoTemp[MAX_TIPO];
            pedirTexto("Nuevo tipo de combustible: ", tipoTemp, MAX_TIPO);
            aMinusculas(tipoTemp);
            strcpy(v->tipo_combustible, tipoTemp);
            obtenerOAgregarPrecioCombustible(v->tipo_combustible);
            break;
        }
        case 11:
            v->consumo_ciudad_l100km = pedirFloat("Nuevo consumo ciudad (L/100km): ", 0);
            break;
        case 12:
            v->consumo_autopista_l100km = pedirFloat("Nuevo consumo autopista (L/100km): ", 0);
            break;
        default:
            printf("Operacion cancelada.\n");
            return;
    }

    guardarVehiculos();
    printf("Vehiculo actualizado correctamente.\n");
}

void modificarPrecioCombustible(void) {
    printf("\n--- MODIFICAR PRECIO DE COMBUSTIBLE ---\n");

    for (int i = 0; i < numCombustibles; i++) {
        printf("  %d) %s: %.2f\n", i + 1, combustibles[i].nombre, combustibles[i].precio);
    }
    printf("  %d) Agregar nuevo tipo de combustible\n", numCombustibles + 1);

    char mensaje[64];
    snprintf(mensaje, sizeof(mensaje), "Seleccione una opcion (1-%d): ", numCombustibles + 1);
    int idx = pedirInt(mensaje, 1);

    if (idx == numCombustibles + 1) {
        if (numCombustibles >= MAX_COMBUSTIBLES) {
            printf("  -> Limite de tipos de combustible alcanzado.\n");
            return;
        }
        char tipoTemp[MAX_TIPO];
        pedirTexto("Nombre del nuevo combustible: ", tipoTemp, MAX_TIPO);
        aMinusculas(tipoTemp);
        double precio = pedirFloat("Precio del nuevo combustible: ", 0);
        strcpy(combustibles[numCombustibles].nombre, tipoTemp);
        combustibles[numCombustibles].precio = precio;
        numCombustibles++;
    } else if (idx >= 1 && idx <= numCombustibles) {
        char mensajePrecio[80];
        snprintf(mensajePrecio, sizeof(mensajePrecio), "Nuevo precio para '%s' (actual %.2f): ",
                  combustibles[idx - 1].nombre, combustibles[idx - 1].precio);
        combustibles[idx - 1].precio = pedirFloat(mensajePrecio, 0);
    } else {
        printf("Opcion invalida.\n");
        return;
    }

    guardarCombustibles();
    printf("Precio(s) de combustible actualizado(s) correctamente.\n");
}

void mostrarFichaCostos(void) {
    printf("\n--- FICHA DE COSTOS POR KM ---\n");
    int idx = seleccionarVehiculo();
    if (idx == -1) return;
    Vehiculo *v = &vehiculos[idx];

    double precio = obtenerOAgregarPrecioCombustible(v->tipo_combustible);
    CostosVehiculo c = calcularCostosVehiculo(v, precio);

    printf("\nVehiculo: %s  |  Combustible: %s (precio actual: %.2f)\n",
           v->nombre, v->tipo_combustible, precio);
    printf("  Amortizacion.........: %.4f / km\n", c.amortizacion_km);
    printf("  Mantenimiento.........: %.4f / km\n", c.mantenimiento_km);
    printf("  Seguro................: %.4f / km\n", c.seguro_km);
    printf("  Neumaticos............: %.4f / km\n", c.neumaticos_km);
    printf("  --------------------------------------\n");
    printf("  Costo fijo total......: %.4f / km\n", c.costo_fijo_km);
    printf("  Consumo promedio......: %.2f L/100km\n", c.consumo_promedio_l100km);
    printf("  Combustible ciudad....: %.4f / km\n", c.combustible_ciudad_km);
    printf("  Combustible autopista.: %.4f / km\n", c.combustible_autopista_km);
    printf("  ========================================\n");
    printf("  COSTO REAL EN CIUDAD......: %.4f / km\n", c.costo_ciudad_km);
    printf("  COSTO REAL EN AUTOPISTA...: %.4f / km\n", c.costo_autopista_km);
}

void calcularViajeMenu(void) {
    printf("\n--- CALCULAR COSTO DE UN VIAJE ---\n");
    int idx = seleccionarVehiculo();
    if (idx == -1) return;
    Vehiculo *v = &vehiculos[idx];

    double precio = obtenerOAgregarPrecioCombustible(v->tipo_combustible);

    double kmCiudad = pedirFloat("Kilometros a recorrer en CIUDAD: ", 0);
    double kmAutopista = pedirFloat("Kilometros a recorrer en AUTOPISTA: ", 0);

    if (kmCiudad == 0 && kmAutopista == 0) {
        printf("Debe ingresar al menos un km recorrido.\n");
        return;
    }

    ResultadoViaje r = calcularViaje(v, precio, kmCiudad, kmAutopista);

    printf("\nResultado del viaje con '%s':\n", v->nombre);
    printf("  Km en ciudad.............: %.2f\n", r.km_ciudad);
    printf("  Km en autopista...........: %.2f\n", r.km_autopista);
    printf("  Km totales................: %.2f\n", r.km_totales);
    printf("  Costo en combustible......: %.2f\n", r.costo_combustible_viaje);
    printf("  Costo fijo (amort/mant/seguro/neumaticos): %.2f\n", r.costo_fijo_viaje);
    printf("  ------------------------------------------\n");
    printf("  COSTO TOTAL DEL VIAJE.....: %.2f\n", r.costo_total_viaje);
    printf("  Costo promedio por km.....: %.4f\n", r.costo_por_km_promedio);
}

void listarVehiculos(void) {
    printf("\n--- VEHICULOS REGISTRADOS ---\n");
    int hayAlguno = 0;
    for (int i = 0; i < numSlotsVehiculos; i++) {
        if (vehiculos[i].usado) {
            hayAlguno = 1;
            Vehiculo *v = &vehiculos[i];
            printf("  - %s  |  combustible: %s  |  costo: %.2f  |  vida util: %.2f km\n",
                   v->nombre, v->tipo_combustible, v->costo_vehiculo, v->vida_util_km);
        }
    }
    if (!hayAlguno) {
        printf("No hay vehiculos registrados.\n");
    }
}

/* ------------------------------------------------------------------ */
/* MENU PRINCIPAL                                                      */
/* ------------------------------------------------------------------ */

void mostrarMenu(void) {
    printf("\n===== GESTOR DE COSTOS DE VEHICULOS =====\n");
    printf("  1) Crear vehiculo\n");
    printf("  2) Borrar vehiculo\n");
    printf("  3) Modificar parametros de un vehiculo\n");
    printf("  4) Calcular viaje\n");
    printf("  5) Modificar precio de combustible\n");
    printf("  6) Ver ficha de costos por km de un vehiculo\n");
    printf("  7) Listar vehiculos\n");
    printf("  0) Salir\n");
}

int main(void) {
    cargarVehiculos();
    cargarCombustibles();

    while (1) {
        mostrarMenu();
        int opcion = pedirInt("Seleccione una opcion: ", 0);

        switch (opcion) {
            case 1: crearVehiculo(); break;
            case 2: borrarVehiculo(); break;
            case 3: modificarVehiculo(); break;
            case 4: calcularViajeMenu(); break;
            case 5: modificarPrecioCombustible(); break;
            case 6: mostrarFichaCostos(); break;
            case 7: listarVehiculos(); break;
            case 0:
                printf("Hasta luego!\n");
                return 0;
            default:
                printf("Opcion invalida, intente de nuevo.\n");
        }
    }

    return 0;
}
