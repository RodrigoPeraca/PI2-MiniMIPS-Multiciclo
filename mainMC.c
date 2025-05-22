#include "bibli.h"

int main(){
	int op = 1, aux;
	RegistradoresMIPS bancoReg; 
    unsigned int programCounter = 0; 				// PC
    desc* pilha = cria_desc(); 						// Desc pilha
	nodo* stepback = NULL; 							// Aux para voltar instrucao
    int topoInst = 0; 								// Cont num de instrucoes
    struct mem memoriaComp[MEM_SIZE]; 				// Passar referencia para alterar dados na funcao controle;
    InicializaReg(&bancoReg);						// Inicializa registradores com tudo zero
    InicializaMem(memoriaComp);						// Inicializa memoria com tudo zero
    struct instrucao inst; 							// Reg para armazenar instrucao a ser executada
    carregar_memoria(memoriaComp, &topoInst); 		// Carrega a .mem
    while(op != 0){
		printf("\n\t\t________________________________________________________\n");
		printf("\t\t|                    Menu de opcoes                     |\n");
		if(programCounter <= topoInst){
			printf("\t\t|\tA proxima instrucao e:");
			casm(memoriaComp[programCounter].inst);
			printf("\t\t|\n");
		}
		printf("\t\t|\tEscolha uma opcao:\t\t\t\t|\n");
		printf("\t\t|\t1- Step\t\t\t\t2- Stepback\t|\n\t\t|\t3- Imprime simulador\t\t4- Run\t\t|\n\t\t|\t0- Exit\t\t\t\t\t\t|\n");
		printf("\t\t|_______________________________________________________|\n");
		scanf("%i", &op);
		switch(op){
			case 1://Executa
				if(programCounter <= topoInst){
					PUSH(pilha, programCounter, memoriaComp, &bancoReg);
					controle(&inst, &bancoReg, memoriaComp, &programCounter, &topoInst, ESTADO_B);
				}else{
					printf("Nao existe mais instrucoes!!!\n");
				}
				break;
			case 2://Step back
				if(pilha->topo != NULL){
					stepback = POP(pilha);
					BackInstruction(stepback, memoriaComp, &programCounter, &bancoReg);
					free(stepback);
					imprimeRegistradores(bancoReg, programCounter);
					imprimeDados(memoriaComp, topoInst);
				}else{
					printf("Nao tem instrucao mais para voltar\n");
				}
				break;	
			case 3://Imprime simulador
				imprimeRegistradores(bancoReg, programCounter);
				imprimeDados(memoriaComp, topoInst);
				break;
			case 4:
				printf("Executando...\n");
				while(programCounter <= topoInst){
					printf("Voce quer voltar uma instrucao:\n 1- Voltar 2- Continuar\n");
					scanf("%i", &aux);
					if(aux == 2){
						PUSH(pilha, programCounter, memoriaComp, &bancoReg);
						controle(&inst, &bancoReg, memoriaComp, &programCounter, &topoInst, ESTADO_B);
					}
					if(aux == 1){
						if(pilha->topo != NULL){
							stepback = POP(pilha);
							BackInstruction(stepback, memoriaComp, &programCounter, &bancoReg);
							free(stepback);
							imprimeRegistradores(bancoReg, programCounter);
							imprimeDados(memoriaComp, topoInst);
						}else{
							printf("Nao tem instrucao mais para voltar\n");
						}
					}						
				}	
				printf("Acabou o programa!!\n");
		
			case 0: //Exit
				fre_pilha(pilha);
				return 0;
		}			
	}
	return 0;
}
