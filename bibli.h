#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEM_SIZE 256 // Define o tamanho da memória como 256 bytes

//Definir os estados 
#define ESTADO_B 0 //Busca
#define ESTADO_D 1 // Decodificacao
#define ESTADO_CalEnd 2 // Calculo do imm/lw/sw
#define ESTADO_AcessoMem 3 // Acesso a memoria
#define ESTADO_EscRT 4  // Escrita no RT Lw
#define ESTADO_EscMem 5 // Escrita na memoria
#define ESTADO_EscAddi 6 // Termino da instrucao Addi
#define ESTADO_Exe 7 // Execucao do Tipo R
#define ESTADO_EscRD 8 // Termino da escrita tipo R
#define ESTADO_DesvioCond 9 // Termino no desvio condicional
#define ESTADO_DesvioIncond 10 // Termino do desvio incondicional

typedef struct {
    int resul;
    int OF;
    int zero;
}ResulULA;
typedef struct {
    int registradores[8]; // Registradores  ($0 - $7)
} RegistradoresMIPS;
struct mem{
    char inst[17];
};
struct instrucao{
	char palavra[16];
	int opcode;
	int rs;
	int rt;
	int rd;
	int funct;
	int imm;
	int addr;
};
//Step back pilha

struct elemento{//Elementos de estados que sao alterados
    unsigned int PC;
    struct instrucao inst;
    int Reg_A;
    int Reg_B;
    int ULASaida;
    int RDM;
    struct mem memory[MEM_SIZE];
    int estado;
    int Reg[8];
};
struct nodo_pilha{
    struct nodo_pilha *prox;
    struct elemento *info;
};
struct desc_pilha{
    struct nodo_pilha *topo;
    int tamanho;
};
typedef struct elemento back;
typedef struct nodo_pilha nodo;
typedef struct desc_pilha desc;

void assemb(char* palavra); // gera o assembly
int DecodificaInst(int estado, struct instrucao inst); // controla os estado
void inicializaInst(struct instrucao* inst); // Inicializa Inst = "\000000..."
void casm(char* palavra); // Transforma inst em assembly
void imprimeRegistradores(RegistradoresMIPS bancoReg, unsigned int* programCounter); // Imprime banco de registradores
void imprimeDados(struct mem memoriaComp[], int limiteInst); // Imprime Memoria(Dados/Inst)
void menuExecucao (RegistradoresMIPS* bancoReg, struct mem memoriaComp[], int *limiteInst, unsigned int* programCounter, int* estado, struct instrucao* RegInst, struct desc_pilha* pilha, int *OP, int *ULASaida, int *RDM, int *Reg_A, int *Reg_B, int flag); // Menu dos estados
void carregar_memoria(struct mem* memoriaDados, int* limiteInst); // Carrega a .mem
void carregaMem(char palavra[] , struct instrucao* memoria); // Carrega struct instrucao
ResulULA ula(int val1, int val2, int ulaOp); // ULA
int pot(int base, int expoente); // Potencia
int cb2d(char complemento[]); // Converte binario para decimal (complemento de 2)
char* cdb(int inteiro);	//Converte decimal para binario
int cbd(char binario[]); // Converte binario para decimal (sem complemento de dois)
int ler_registrador(RegistradoresMIPS *mips, int reg); // Le o valor do registrador
void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor); // Escreve no banco de registrador
void controle(struct instrucao* RegInst, RegistradoresMIPS* mips, struct mem Mem[], unsigned int* PC, int* topoInst); // Controle
void InicializaMem(struct mem* memoria); // Inicializa memoria = "\000000..."
desc* cria_desc(); // Cria descritor da pilha (StepBack)
void PUSH(desc* pilha ,unsigned int PC, struct mem* Mem, RegistradoresMIPS* BancoReg, int ULASaida, int RDM, struct instrucao* RegInst, int estado, int Reg_A, int Reg_B); // Inseri um elemento na pilha (StepBack)
nodo* POP(desc* pilha); // Remove o topo da pilha (StepBack)
nodo* TOP(desc* pilha); // Retorna o topo da pilha (StepBack)
int EMPTY(desc* pilha); // Retorna o tamanho da pilha (StepBack)
void BackInstruction(nodo* backup, struct mem* memoria, unsigned int* PC, RegistradoresMIPS* BancoReg, int *ULASaida, int *RDM, int *estado, int *Reg_A, int *Reg_B, struct instrucao* RegInst); // Volta uma instrucao (StepBack)
void fre_pilha(desc* pilha); // Realiza o free de toda a pilha (StepBack)
void InicializaReg(RegistradoresMIPS* Reg); // Inicializa banco de registradores com 0
