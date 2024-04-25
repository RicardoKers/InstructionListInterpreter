/*

Programa para interpretar um conjunto de instruções na linguagem "Instruction List", utilizada em CPLs.

O armazenamento de informações utiliza a seguinte nomenclatura:
M Memórias booleano;
R Registradores inteiros com sinal;
T Temporisadores inteiros sem sinal;
C Contadores inteirossem sinal;
X Entradas booleano;
Y Saídas booleano;
K Constantes inteiras (números);

As instruções implementadas são as seguintes:
LD (Load): LD fonte;
LDN (Load Negated): LDN fonte;
ST (Store): ST destino;
STN (Store Negated): STN destino;
SET (Set): S destino;
RST (Reset): R destino;
MOV (Move): MOV origem, destino;
AND (AND lógico): AND operando;
OR (OR lógico): OR  operando;
XOR (XOR lógico): XOR  operando;
ANDN (AND lógico negado): ANDN operando;
ORN (OR lógico negado): ORN operando;
XORN (XOR lógico negado): XORN operando;
NOT (NOT lógico): NOT;
ADD (Adição): ADD operando1, operando2, destino;
SUB (Subtração): SUB operando1, operando2, destino;
MUL (Multiplicação): MUL operando1, operando2, destino;
DIV (Divisão): DIV operando1, operando2, destino;
GT (Maior que): GT operando1, operando2;
GE (Maior ou igual): GE operando1, operando2;
EQ (Igual): EQ operando1, operando2;
NE (Diferente): NE operando1, operando2;
LT (Menor que): LE operando1, operando2;
LE (Menor ou igual): LE operando1, operando2;
CTU (Contador Crescente): CTU operando;
CTD (Contador Decrescente): CTD operando;
TON (Temporizador para Ligar): TON operando;
TOF (Temporizador para Desligar): TOF operando;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulaIL.h"
//#include <ctype.h>
/*
// Definição de memória
#define Max_Memorias 10
#define Max_Registradores 10
#define Max_Temporizadores 10
#define Max_Contadores 10
#define Max_Entradas 10
#define Max_Saidas 10

// Estrutura para armazenar os dados
typedef struct stData {
    // Variáveis de memória
    char Memorias[Max_Memorias];
    int Registradores[Max_Registradores];
    unsigned int TempPreset[Max_Temporizadores];
    unsigned int TempValue[Max_Temporizadores];
    unsigned int Contadores[Max_Contadores];
    char Entradas[Max_Entradas];
    char Saidas[Max_Saidas];
    char acumulador;
    int max_Memorias;
    int max_Registradores;
    int max_Temporizadores;
    int max_Contadores;
    int max_Entradas;
    int max_Saidas;
} Data;

*/

// Definição das variáveis globais
Data data;

// Definição de tokens
#define LD_TOKEN 1
#define LDN_TOKEN 2
#define ST_TOKEN 3
#define STN_TOKEN 4
#define SET_TOKEN 5
#define RST_TOKEN 6
#define MOV_TOKEN 7
#define AND_TOKEN 8
#define OR_TOKEN 9
#define XOR_TOKEN 10
#define ANDN_TOKEN 11
#define ORN_TOKEN 12
#define XORN_TOKEN 13
#define NOT_TOKEN 14
#define ADD_TOKEN 15
#define SUB_TOKEN 16
#define MUL_TOKEN 17
#define DIV_TOKEN 18
#define GT_TOKEN 19
#define GE_TOKEN 20
#define EQ_TOKEN 21
#define NE_TOKEN 22
#define LT_TOKEN 23
#define LE_TOKEN 24
#define IDENTIFIER_TOKEN 25
#define NUMBER_TOKEN 26
#define COMMA_TOKEN 27
#define SEMICOLON_TOKEN 28
#define NEW_LINE 29

// Estrutura para armazenar uma instrução
typedef struct stInstruction {
    int opcode;
    char **operands;
    int num_operands;
} Instruction;

void get_token(char *programa, int *pos, char *token)
{
    int i = 0;
    while (programa[*pos] != ' ' && programa[*pos] != '\n' && programa[*pos] != '\0')
    {
        token[i] = programa[*pos];
        i++;
        (*pos)++;
    }
    token[i] = '\0';
}

void get_next_token(char *programa, int *pos, char *token)
{
    while (programa[*pos] == ' ' || programa[*pos] == '\n')
    {
        (*pos)++;
    }
    get_token(programa, pos, token);
}

void get_instruction(char *programa, int *pos, Instruction *instrucao)
{
    char token[100];
    get_next_token(programa, pos, token);
    if (strcmp(token, "LD") == 0)
    {
        instrucao->opcode = LD_TOKEN;
    }
    else if (strcmp(token, "LDN") == 0)
    {
        instrucao->opcode = LDN_TOKEN;
    }
    else if (strcmp(token, "ST") == 0)
    {
        instrucao->opcode = ST_TOKEN;
    }
    else if (strcmp(token, "STN") == 0)
    {
        instrucao->opcode = STN_TOKEN;
    }
    else if (strcmp(token, "SET") == 0)
    {
        instrucao->opcode = SET_TOKEN;
    }
    else if (strcmp(token, "RST") == 0)
    {
        instrucao->opcode = RST_TOKEN;
    }
    else if (strcmp(token, "MOV") == 0)
    {
        instrucao->opcode = MOV_TOKEN;
    }
    else if (strcmp(token, "AND") == 0)
    {
        instrucao->opcode = AND_TOKEN;
    }
    else if (strcmp(token, "OR") == 0)
    {
        instrucao->opcode = OR_TOKEN;
    }
    else if (strcmp(token, "XOR") == 0)
    {
        instrucao->opcode = XOR_TOKEN;
    }
    else if (strcmp(token, "ANDN") == 0)
    {
        instrucao->opcode = ANDN_TOKEN;
    }
    else if (strcmp(token, "ORN") == 0)
    {
        instrucao->opcode = ORN_TOKEN;
    }
    else if (strcmp(token, "XORN") == 0)
    {
        instrucao->opcode = XORN_TOKEN;
    }
    else if (strcmp(token, "NOT") == 0)
    {
        instrucao->opcode = NOT_TOKEN;
    }
    else if (strcmp(token, "ADD") == 0)
    {
        instrucao->opcode = ADD_TOKEN;
    }
    else if (strcmp(token, "SUB") == 0)
    {
        instrucao->opcode = SUB_TOKEN;
    }
    else if (strcmp(token, "MUL") == 0)
    {
        instrucao->opcode = MUL_TOKEN;
    }
    else if (strcmp(token, "DIV") == 0)
    {
        instrucao->opcode = DIV_TOKEN;
    }
    else if (strcmp(token, "GT") == 0)
    {
        instrucao->opcode = GT_TOKEN;
    }
    else if (strcmp(token, "GE") == 0)
    {
        instrucao->opcode = GE_TOKEN;
    }
    else if (strcmp(token, "EQ") == 0)
    {
        instrucao->opcode = EQ_TOKEN;
    }
    else if (strcmp(token, "NE") == 0)
    {
        instrucao->opcode = NE_TOKEN;
    }
    else if (strcmp(token, "LT") == 0)
    {
        instrucao->opcode = LT_TOKEN;
    }
    else if (strcmp(token, "LE") == 0)
    {
        instrucao->opcode = LE_TOKEN;
    }
    else
    {
        instrucao->opcode = IDENTIFIER_TOKEN;
    }
    instrucao->num_operands = 0;
    instrucao->operands = (char **)malloc(3 * sizeof(char *));
    while (programa[*pos] != '\n' && programa[*pos] != '\0')
    {
        get_next_token(programa, pos, token);
        instrucao->operands[instrucao->num_operands] = (char *)malloc(strlen(token) + 1);
        strcpy(instrucao->operands[instrucao->num_operands], token);
        instrucao->num_operands++;
    }
}

int stract_Number(char *token)
{
    int i = 0;
    int number = 0;
    while (token[i] != '\0')
    {
        number = number * 10 + token[i] - '0';
        i++;
    }
    return number;
}

int get_value(char *token)
{
    if (token[0] == 'X' && stract_Number(token + 1) < Max_Entradas)
    {
        return data.Entradas[stract_Number(token + 1)];
    }
    else if (token[0] == 'Y' && stract_Number(token + 1) < Max_Saidas)
    {
        return data.Saidas[stract_Number(token + 1)];
    }
    else if (token[0] == 'M' && stract_Number(token + 1) < Max_Memorias)
    {
        return data.Memorias[stract_Number(token + 1)];
    }
    else if (token[0] == 'R' && stract_Number(token + 1) < Max_Registradores)
    {
        return data.Registradores[stract_Number(token + 1)];
    }
    else if (token[0] == 'T' && stract_Number(token + 2) < Max_Temporizadores)
    {
        if (token[1] == 'P')
            return data.TempPreset[stract_Number(token + 2)];
        else
            return data.TempValue[stract_Number(token + 2)];
    }
    else if (token[0] == 'C' && stract_Number(token + 2) < Max_Contadores)
    {
        if (token[1] == 'P')
            return data.ContPreset[stract_Number(token + 2)];
        else
            return data.ContValue[stract_Number(token + 2)];
    }
    else if (token[0] == 'K')
    {
        return stract_Number(token + 1);
    }
    return(0);
}

void set_value(char *token, int value)
{
    if (token[0] == 'K')
    {
        printf("Erro: Constantes não podem ser alteradas\n");
    }
    else if (token[0] == 'X')
    {
        printf("Erro: Entradas não podem ser alteradas\n");
    }
    else if (token[0] == 'Y' && stract_Number(token + 1) < Max_Saidas)
    {
        data.Saidas[stract_Number(token + 1)] = value;
    }
    else if (token[0] == 'M' && stract_Number(token + 1) < Max_Memorias)
    {
        data.Memorias[stract_Number(token + 1)] = value;
    }
    else if (token[0] == 'R' && stract_Number(token + 1) < Max_Registradores)
    {
        data.Registradores[stract_Number(token + 1)] = value;
    }
    else if (token[0] == 'T' && stract_Number(token + 3) < Max_Temporizadores)
    {
        if (token[1] == 'P')
            data.TempPreset[stract_Number(token + 2)] = value;
        else
            data.TempValue[stract_Number(token + 2)] = value;
    }
    else if (token[0] == 'C' && stract_Number(token + 1) < Max_Contadores)
    {
        if (token[1] == 'P')
            data.ContPreset[stract_Number(token + 2)] = value;
        else
            data.ContValue[stract_Number(token + 2)] = value;
    }
}

int verify_boolean(char *token)
{
    if (token[0] == 'X')
    {
        return 1;
    }
    if (token[0] == 'Y')
    {
        return 1;
    }
    if (token[0] == 'M')
    {
        return 1;
    }
    printf("Erro: Operando não é booleano\n");
    return 0;
}

int verify_integer(char *token)
{
    if (token[0] == 'R')
    {
        return 1;
    }
    if (token[0] == 'T')
    {
        return 1;
    }
    if (token[0] == 'C')
    {
        return 1;
    }
    if (token[0] == 'K')
    {
        return 1;
    }
    printf("Erro: Operando não é inteiro\n");
    return 0;
}

void execute_instruction(Instruction instrucao)
{
    int i;
    switch (instrucao.opcode)
    {
    case LD_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=get_value(instrucao.operands[0]);
        break;
    case LDN_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(get_value(instrucao.operands[0])==0)?1:0;
        break;
    case ST_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            set_value(instrucao.operands[0],data.acumulador);
        break;
    case STN_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            set_value(instrucao.operands[0],(data.acumulador==0)?1:0);
        break;
    case SET_TOKEN:
        if(data.acumulador==1){
            if(verify_boolean(instrucao.operands[0]))
                set_value(instrucao.operands[0],1);
        }
        break;
    case RST_TOKEN:
        if(data.acumulador==1){
            if(verify_boolean(instrucao.operands[0]))
                set_value(instrucao.operands[0],0);
        }
        break;
    case MOV_TOKEN:
        if(data.acumulador==1){
            if (verify_integer(instrucao.operands[0]) && (verify_integer(instrucao.operands[1])))
                set_value(instrucao.operands[1], get_value(instrucao.operands[0]));
        }
        break;
    case AND_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador && get_value(instrucao.operands[0]))?1:0;
        break;
    case OR_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador || get_value(instrucao.operands[0]))?1:0;
        break;
    case XOR_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador ^ get_value(instrucao.operands[0]))?1:0;
        break;
    case ANDN_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador && !get_value(instrucao.operands[0]))?1:0;
        break;
    case ORN_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador || !get_value(instrucao.operands[0]))?1:0;
        break;
    case XORN_TOKEN:
        if(verify_boolean(instrucao.operands[0]))
            data.acumulador=(data.acumulador ^ !get_value(instrucao.operands[0]))?1:0;
        break;
    case NOT_TOKEN:
        data.acumulador=(data.acumulador==0)?1:0;
        break;
    case ADD_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]) && verify_integer(instrucao.operands[2]))
                set_value(instrucao.operands[2],get_value(instrucao.operands[0])+get_value(instrucao.operands[1]));
        }
        break;
    case SUB_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]) && verify_integer(instrucao.operands[2]))
                set_value(instrucao.operands[2],get_value(instrucao.operands[0])-get_value(instrucao.operands[1]));
        }
        break;
    case MUL_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]) && verify_integer(instrucao.operands[2]))
                set_value(instrucao.operands[2],get_value(instrucao.operands[0])*get_value(instrucao.operands[1]));
        }
        break;
    case DIV_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]) && verify_integer(instrucao.operands[2]))
                set_value(instrucao.operands[2],get_value(instrucao.operands[0])/get_value(instrucao.operands[1]));
        }
        break;
    case GT_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])>get_value(instrucao.operands[1]))?1:0;
        }
        break;
    case GE_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])>=get_value(instrucao.operands[1]))?1:0;
        }
        break;
    case EQ_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])==get_value(instrucao.operands[1]))?1:0;
        }
        break;
    case NE_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])!=get_value(instrucao.operands[1]))?1:0;
        }
        break;
    case LT_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])<get_value(instrucao.operands[1]))?1:0;
        }
        break;
    case LE_TOKEN:
        if(data.acumulador==1){
            if(verify_integer(instrucao.operands[0]) && verify_integer(instrucao.operands[1]))
                data.acumulador=(get_value(instrucao.operands[0])<=get_value(instrucao.operands[1]))?1:0;
        }
        break;
    }
}

Data* inicializa()
{
    // inicializa as memórias
    for (int i = 0; i < Max_Memorias; i++)
    {
        data.Memorias[i] = 0;
    }
    // inicializa os registradores
    for (int i = 0; i < Max_Registradores; i++)
    {
        data.Registradores[i] = 0;
    }
    // inicializa os temporizadores
    for (int i = 0; i < Max_Temporizadores; i++)
    {
        data.TempPreset[i] = 0;
        data.TempValue[i] = 0;
    }
    // inicializa os contadores
    for (int i = 0; i < Max_Contadores; i++)
    {
        data.ContPreset[i] = 0;
        data.ContValue[i] = 0;
    }
    // inicializa as saídas
    for (int i = 0; i < Max_Saidas; i++)
    {
        data.Saidas[i] = 0;
    }
    return &data;
}

void atualiza_entradas() // dependente da plataforma
{
    printf("Testando...\n");
}

void atualiza_saidas() // dependente da plataforma
{

}

int pos = 0;
Instruction instrucao;

void rodaPasso(char *programa)
{
    if(programa[pos] != '\0')
    {
        get_instruction(programa, &pos, &instrucao);
        execute_instruction(instrucao);
    }
    else
    {
        pos = 0;
    }
}

void rodaCiclo(char *programa)
{    
    while (programa[pos] != '\0')
    {
        get_instruction(programa, &pos, &instrucao);
        execute_instruction(instrucao);
    }
    pos = 0;
}


    /*int main()
    {
        char programa[] = "LD X1\nAND X2\nST Y0\n";
        int pos = 0;
        Entradas[0] = 1;
        Entradas[1] = 1;
        Entradas[2] = 1;
        Instruction instrucao;
        while (programa[pos] != '\0')
        {
            get_instruction(programa, &pos, &instrucao);
            execute_instruction(instrucao);
            imprime_instrucao(instrucao);
            printf("A=%d\n",acumulador);
            imprime_entradas(3);
            imprime_saidas(3);
            getchar();
        }
        return 0;
    }*/
