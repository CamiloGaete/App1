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
