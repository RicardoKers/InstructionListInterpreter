//#ifndef SIMULAIL_H
//#define SIMULAIL_H

//#endif // SIMULAIL_H

// Definição de memória
#define Max_Memorias 10
#define Max_Registradores 10
#define Max_Temporizadores 10
#define Max_Contadores 10
#define Max_Entradas 10
#define Max_Saidas 10

typedef struct stData {
    // Variáveis de memória
    char Memorias[Max_Memorias];
    int Registradores[Max_Registradores];
    unsigned int TempPreset[Max_Temporizadores];
    unsigned int TempValue[Max_Temporizadores];
    unsigned int ContPreset[Max_Contadores];
    unsigned int ContValue[Max_Contadores];
    char Entradas[Max_Entradas];
    char Saidas[Max_Saidas];
    char acumulador;
} Data;

void atualiza_entradas();
void atualiza_saidas();
Data* inicializa();
void rodaCiclo(char *programa);
void rodaPasso(char *programa);
