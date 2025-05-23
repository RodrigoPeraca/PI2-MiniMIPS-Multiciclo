#include "bibli.h"

int main(){
	int op = 1, aux;
	RegistradoresMIPS bancoReg; 
    unsigned int programCounter = 0; 				// PC
    int topoInst = 0; 								// Cont num de instrucoes
    struct mem memoriaComp[MEM_SIZE]; 				// Passar referencia para alterar dados na funcao controle;
    InicializaReg(&bancoReg);						// Inicializa registradores com tudo zero
    InicializaMem(memoriaComp);						// Inicializa memoria com tudo zero
    struct instrucao inst; 							// Reg para armazenar instrucao a ser executada
    carregar_memoria(memoriaComp, &topoInst); 		// Carrega a .mem
    controle(&inst, &bancoReg, memoriaComp, &programCounter, &topoInst);
	return 0;
}
