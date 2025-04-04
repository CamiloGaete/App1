#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definiciones de constantes
#define MAX_LINE 1024       // Máximo largo de una línea de texto
#define MAX_PIZZAS 1000     // Máximo número de ventas registradas
#define MAX_NAME 100        // Máximo largo de nombre de pizza, categoría, etc.

// Estructura para representar una venta de pizza
typedef struct {
    char pizza_id[MAX_NAME];
    char pizza_name[MAX_NAME];
    char order_date[20];
    int quantity;
    float total_price;
    char pizza_category[MAX_NAME];
    char pizza_ingredients[MAX_LINE];
} PizzaSale;

// Estructura para agrupar por tipo de pizza
typedef struct {
    char nombre[MAX_NAME];
    int cantidad_total;
} PizzaResumen;

// Arreglos para guardar ventas y el resumen por pizza
PizzaSale ventas[MAX_PIZZAS];
PizzaResumen resumen[100];
int total_ventas = 0;            // Contador de ventas leídas
int total_pizzas_unicas = 0;     // Contador de pizzas distintas

// Reinicia el resumen de pizzas agrupadas
void reiniciar_resumen() {
    total_pizzas_unicas = 0;
    for (int i = 0; i < 100; i++) {
        resumen[i].nombre[0] = '\0';
        resumen[i].cantidad_total = 0;
    }
}

// Agrupa ventas por nombre de pizza y suma cantidades
void agrupar_por_pizza() {
    reiniciar_resumen();
    for (int i = 0; i < total_ventas; i++) {
        int encontrado = 0;
        for (int j = 0; j < total_pizzas_unicas; j++) {
            if (strcmp(resumen[j].nombre, ventas[i].pizza_name) == 0) {
                resumen[j].cantidad_total += ventas[i].quantity;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            strcpy(resumen[total_pizzas_unicas].nombre, ventas[i].pizza_name);
            resumen[total_pizzas_unicas].cantidad_total = ventas[i].quantity;
            total_pizzas_unicas++;
        }
    }
}

// Imprime el resumen agrupado (para debug)
void debug_resumen() {
    printf("\n--- DEBUG RESUMEN ---\n");
    for (int i = 0; i < total_pizzas_unicas; i++) {
        printf("%s: %d\n", resumen[i].nombre, resumen[i].cantidad_total);
    }
    printf("---------------------\n");
}

// Lee archivo CSV y carga las ventas en el arreglo
void leer_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    char line[MAX_LINE];
    fgets(line, MAX_LINE, file); // Lee encabezado

    while (fgets(line, MAX_LINE, file) && total_ventas < MAX_PIZZAS) {
        char* campos[12];
        int columna = 0;
        char* ptr = line;

        // Separación de campos con manejo de comillas
        while (*ptr && columna < 12) {
            char* start;
            if (*ptr == '"') {
                ptr++;
                start = ptr;
                while (*ptr && !(*ptr == '"' && *(ptr + 1) == ',')) ptr++;
                if (*ptr == '"') *ptr = '\0';
                ptr += 2;
            } else {
                start = ptr;
                while (*ptr && *ptr != ',') ptr++;
                if (*ptr == ',') *ptr = '\0';
                ptr++;
            }
            campos[columna++] = start;
        }

        // Asignación de campos si están completos
        if (columna == 12) {
            strcpy(ventas[total_ventas].pizza_id, campos[2]);
            ventas[total_ventas].quantity = atoi(campos[3]);
            strcpy(ventas[total_ventas].order_date, campos[4]);
            ventas[total_ventas].total_price = atof(campos[7]);
            strcpy(ventas[total_ventas].pizza_category, campos[9]);
            strcpy(ventas[total_ventas].pizza_ingredients, campos[10]);
            strcpy(ventas[total_ventas].pizza_name, campos[11]);

            // Quitar salto de línea final
            ventas[total_ventas].pizza_name[strcspn(ventas[total_ventas].pizza_name, "\r\n")] = 0;

            total_ventas++;
        }
    }

    fclose(file);
}

// Encuentra e imprime la pizza más vendida
void pizza_mas_vendida() {
    int max = 0;
    for (int i = 1; i < total_pizzas_unicas; i++) {
        if (resumen[i].cantidad_total > resumen[max].cantidad_total) {
            max = i;
        }
    }
    printf("Pizza mas vendida: %s (%d ventas)\n", resumen[max].nombre, resumen[max].cantidad_total);
}

// Encuentra e imprime la pizza menos vendida
void pizza_menos_vendida() {
    int min = 0;
    for (int i = 1; i < total_pizzas_unicas; i++) {
        if (resumen[i].cantidad_total < resumen[min].cantidad_total) {
            min = i;
        }
    }
    printf("Pizza menos vendida: %s (%d ventas)\n", resumen[min].nombre, resumen[min].cantidad_total);
}

// Encuentra fechas con más o menos ventas (por dinero o por cantidad)
void fecha_mas_menos(const char* tipo, int por_dinero) {
    float total_por_dia[1000] = {0};
    int cantidad_por_dia[1000] = {0};
    char fechas[1000][20];
    int total_fechas = 0;

    // Agrupa por fecha
    for (int i = 0; i < total_ventas; i++) {
        int index = -1;

        for (int j = 0; j < total_fechas; j++) {
            if (strcmp(fechas[j], ventas[i].order_date) == 0) {
                index = j;
                break;
            }
        }
        if (index == -1) {
            strcpy(fechas[total_fechas], ventas[i].order_date);
            index = total_fechas++;
        }
        total_por_dia[index] += ventas[i].total_price;
        cantidad_por_dia[index] += ventas[i].quantity;
    }

    // Buscar mejor fecha
    int mejor = 0;
    for (int i = 1; i < total_fechas; i++) {
        if ((por_dinero && total_por_dia[i] > total_por_dia[mejor]) ||
            (!por_dinero && cantidad_por_dia[i] > cantidad_por_dia[mejor])) {
            mejor = i;
        }
    }

    // Buscar peor fecha
    int peor = 0;
    for (int i = 1; i < total_fechas; i++) {
        if ((por_dinero && total_por_dia[i] < total_por_dia[peor]) ||
            (!por_dinero && cantidad_por_dia[i] < cantidad_por_dia[peor])) {
            peor = i;
        }
    }

    if (strcmp(tipo, "max") == 0)
        printf("Fecha con mas ventas (%s): %s (%.2f / %d)\n", por_dinero ? "dinero" : "pizzas",
            fechas[mejor], por_dinero ? total_por_dia[mejor] : 0, cantidad_por_dia[mejor]);
    else
        printf("Fecha con menos ventas (%s): %s (%.2f / %d)\n", por_dinero ? "dinero" : "pizzas",
            fechas[peor], por_dinero ? total_por_dia[peor] : 0, cantidad_por_dia[peor]);
}

// Calcula promedio de pizzas por orden
void promedio_por_orden() {
    int ordenes = 0;
    for (int i = 0; i < total_ventas; i++) {
        ordenes++;
    }
    float promedio = (float)(ordenes ? total_ventas : 0) / ordenes;
    printf("Promedio de pizzas por orden: %.2f\n", promedio);
}

// Calcula promedio de pizzas por día
void promedio_por_dia() {
    char fechas[1000][20];
    int total_fechas = 0;
    for (int i = 0; i < total_ventas; i++) {
        int existe = 0;
        for (int j = 0; j < total_fechas; j++) {
            if (strcmp(fechas[j], ventas[i].order_date) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) strcpy(fechas[total_fechas++], ventas[i].order_date);
    }
    float promedio = (float)(total_ventas) / total_fechas;
    printf("Promedio de pizzas por dia: %.2f\n", promedio);
}

// Encuentra el ingrediente más vendido sumando la cantidad de ventas por ingrediente
void ingrediente_mas_vendido() {
    char ingredientes[1000][MAX_NAME];
    int cantidades[1000] = {0};
    int total = 0;

    for (int i = 0; i < total_ventas; i++) {
        char temp[MAX_LINE];
        strcpy(temp, ventas[i].pizza_ingredients);
        char* token = strtok(temp, ",");

        while (token) {
            while (*token == ' ') token++; // Elimina espacios iniciales
            int existe = 0;
            for (int j = 0; j < total; j++) {
                if (strcmp(ingredientes[j], token) == 0) {
                    cantidades[j] += ventas[i].quantity;
                    existe = 1;
                    break;
                }
            }
            if (!existe) {
                strcpy(ingredientes[total], token);
                cantidades[total++] = ventas[i].quantity;
            }
            token = strtok(NULL, ",");
        }
    }

    int max = 0;
    for (int i = 1; i < total; i++) {
        if (cantidades[i] > cantidades[max]) max = i;
    }
    printf("Ingrediente mas vendido: %s (%d unidades)\n", ingredientes[max], cantidades[max]);
}

// Muestra la cantidad total de pizzas por categoría
void cantidad_por_categoria() {
    char categorias[100][MAX_NAME];
    int cantidades[100] = {0};
    int total = 0;
    for (int i = 0; i < total_ventas; i++) {
        int existe = 0;
        for (int j = 0; j < total; j++) {
            if (strcmp(categorias[j], ventas[i].pizza_category) == 0) {
                cantidades[j] += ventas[i].quantity;
                existe = 1;
                break;
            }
        }
        if (!existe) {
            strcpy(categorias[total], ventas[i].pizza_category);
            cantidades[total++] = ventas[i].quantity;
        }
    }
    printf("Cantidad de pizzas por categoria:\n");
    for (int i = 0; i < total; i++) {
        printf("%s: %d\n", categorias[i], cantidades[i]);
    }
}

// Función principal: interpreta argumentos y ejecuta métricas
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s archivo.csv metrica1 metrica2 ...\n", argv[0]);
        return 1;
    }

    leer_csv(argv[1]);
    agrupar_por_pizza();

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "pms") == 0) pizza_mas_vendida();
        else if (strcmp(argv[i], "pls") == 0) pizza_menos_vendida();
        else if (strcmp(argv[i], "dms") == 0) fecha_mas_menos("max", 1);
        else if (strcmp(argv[i], "dls") == 0) fecha_mas_menos("min", 1);
        else if (strcmp(argv[i], "dmsp") == 0) fecha_mas_menos("max", 0);
        else if (strcmp(argv[i], "dlsp") == 0) fecha_mas_menos("min", 0);
        else if (strcmp(argv[i], "apo") == 0) promedio_por_orden();
        else if (strcmp(argv[i], "apd") == 0) promedio_por_dia();
        else if (strcmp(argv[i], "ims") == 0) ingrediente_mas_vendido();
        else if (strcmp(argv[i], "hp") == 0) cantidad_por_categoria();
        else if (strcmp(argv[i], "debug") == 0) debug_resumen();
        else printf("Metrica no reconocida: %s\n", argv[i]);
    }

    return 0;
}


