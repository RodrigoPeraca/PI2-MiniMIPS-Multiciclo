#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


int cbd(char binario[]);
int cb2d(char completo[]);

#define MEM_SIZE 256 // Define o tamanho da memória como 256 bytes

//definir os estados 
#define ESTADO_B 0 //BUSCA
#define ESTADO_D 1 // DECODIFICAÇÃO
#define ESTADO_E 2 // EXECUCAO
#define ESTADO_EE 3 // EXECUCAO 2
#define ESTADO_LW 4  //EXECUCAO DO LW
#define ESTADO_FINAL 5 //DEFINIR O FINAL

typedef struct {
    int resul;
    int OF;
    int zero;
}ResulULA;
typedef struct {
    int registradores[8]; // Registradores  ($0 - $7)
} RegistradoresMIPS;
struct mem{
    char inst[16];
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
//step back pilha

struct elemento{//elementos de estados que sao alterados
    unsigned int PC;
    struct mem memory[MEM_SIZE];
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

void inicializaInst(struct instrucao* inst){
	inst->addr = 0;
	inst->funct = 0;
	inst->imm = 0;
	inst->rd = 0;
	inst->rs = 0;
	inst->rt = 0;
	inst->opcode = 0;
}
	
void carregaInst(char palavra[], struct instrucao* inst){
	char binarioVI[6], binarioIV[4], binarioIII[3];
	int indiceS; // Indice Switch
	int decimal; // Variavel para preencher o vetor memoria
	inicializaInst(inst);
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';
	indiceS = cbd(binarioIV); // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	switch (indiceS){
		case 0://tipo R
			strcpy(inst->palavra, palavra); // Guarda a palavra de instrucao
			
			inst->opcode = indiceS; // indiceS Possui decimal do opcode
			
			strncpy(binarioIII, palavra +4, 3); // Copia para a string[3] as tres primeiras posicoes da palavra[] apartir da quarta posicao
			binarioIII[3] = '\0'; // Sinaliza o fim da String
			decimal = cbd(binarioIII); // Conversao binario(string)/decimal(int)
			inst->rs = decimal; // Membro recebe valor decimal		RS
			
			strncpy(binarioIII, palavra +7, 3); // ||		...	Apartir da setima posicao
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rt = decimal; // RT
			
			strncpy(binarioIII, palavra +10, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rd = decimal; // RD
			
			strncpy(binarioIII, palavra +13, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->funct = decimal; // FUNCT
			break;
		
		case 2:// tipo j
			strcpy(inst->palavra, palavra); // PALAVRA
			inst->opcode = indiceS;
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[16] = '\0';

			inst->addr = cbd(binarioVI);
			break;
		//tipo I
		case 4: //ADDi
			strcpy(inst->palavra, palavra); // PALAVRA
			
			inst->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				decimal = cbd(binarioVI);
			inst->imm = decimal; // IMM
			break;
			
		case 8: //BEQ
			strcpy(inst->palavra, palavra); // PALAVRA
			
			inst->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			decimal = cbd(binarioVI);
			inst->imm = decimal; // ADDR
			break;
			
		case 11: //LW
			strcpy(inst->palavra, palavra); // PALAVRA
			
			inst->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			decimal = cbd(binarioVI); // 											RT É O DESTINO!!
			inst->addr = decimal; // ADDR
			break;
				
		case 15: //SW
			strcpy(inst->palavra, palavra); // PALAVRA
			
			inst->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			inst->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII); // 												ADDR É O DESTINO!!!!
			inst->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			decimal = cbd(binarioVI);
			inst->addr = decimal; // ADDR
			break;
			
			default: 
				printf("Erro na leitura, carregamento de memoria corrompido...\n\n");
				break;
	}
}

void casm(char* palavra){// casm = Converte para .asm	
	char aux[17];
	aux[16] = '\0';
	strncpy(aux, palavra, 16);
	struct instrucao inst;
	int indiceSwitch, funct; // Indice Switch
	carregaInst(aux, &inst);
	indiceSwitch = inst.opcode; // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	funct = inst.funct;
	switch (indiceSwitch){
		case 0: // Tipo R
			switch (funct){ // Funct
				case 0:
					if(inst.rs != 0 || inst.rt != 0 || inst.rd != 0)
						printf("add $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
					break;
						
				case 2:
					printf("sub $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
					break;
					
				case 4:
					printf("and $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
					break;
						
				case 5:
					printf("or $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
					break;
			}
			break;	
					
		case 2:
			printf("j %d", inst.addr);
			break;
		
		case 4:
			printf("addi $r%d, $r%d, %d", inst.rt, inst.rs, inst.imm);
			break;
			
		case 8:
			printf("beq $r%d, $r%d, %d", inst.rt, inst.rs, inst.addr);
			break;
			
		case 11:
			printf("lw $r%d, %d($r%d)", inst.rt, inst.addr, inst.rs);
			break;
				
		case 15:
			printf("sw $r%d, %d($r%d)", inst.rt, inst.addr, inst.rs);
			break;
	}	
}


void imprimeRegistradores(RegistradoresMIPS bancoReg, unsigned int programCounter) {
    printf("PC: %d\n", programCounter);
    for (int i = 0; i < 8; i++) {
        printf("$%d: %d\t", i, bancoReg.registradores[i]);
        if (i % 4 == 3) printf("\n");
    }
}
void imprimeDados(struct mem memoriaComp[], int limiteInst){
	int aux = 0;
	char sAux[17];
	printf("\n---------------------------- Memoria de Dados ---------------------------\n");
	printf("Legenda: Posicao [Conteudo]\n");
	if(limiteInst < 256){
		for(aux=0; aux<limiteInst; aux++){
			strcpy(sAux, memoriaComp[aux].inst);
			printf("%d [", aux);
			casm(sAux);
			printf("]\t");
		}
		while(aux<256){
			strcpy(sAux, memoriaComp[aux].inst);
			printf("%d [%s]\t", aux, memoriaComp[aux].inst);
			aux++;
		}
		printf("\n");
	}
}
// Em andamento...
void menuExecucao (RegistradoresMIPS bancoReg, struct mem memoriaComp[], int limiteInst, unsigned int programCounter, int estado, struct instrucao* RegInst){
	int e;
	printf("----Menu de Execucoes----\n");
	if(estado != 5)
		printf("Clock: %i\tInstrucao:", estado);
	else
		printf("Ultimo Clock \tInstrucao:");
	casm(RegInst->palavra);
	printf("\n1- Imprime simulador\t2- Proximo estado\n");
	scanf("%d", &e);
	switch(e){
		case 1:
			imprimeRegistradores(bancoReg, programCounter);
			imprimeDados(memoriaComp, limiteInst);
			break;
	}
	
}


void carregar_memoria(struct mem* memoriaDados, int* limiteInst) {
	char nomeArquivo[256], linha[21], aux[4];
    int num;
    FILE *arquivo = NULL;
    printf("Nome do arquivo:\t");
    setbuf(stdin, NULL);
    scanf("%s", nomeArquivo);
    if ((arquivo = fopen(nomeArquivo, "r")) == NULL)
		printf("\nFalha ao abrir o arquivo!\n");
	else {
		while (strncmp(fgets(linha, sizeof(linha), arquivo), ".data", 5) != 0 && (*limiteInst)<127){
			linha[16] = '\0';
            strcpy(memoriaDados[*limiteInst].inst, linha);
            memoriaDados[*limiteInst].inst[16] = '\0';
            (*limiteInst)++;
        }	
        strcpy(memoriaDados[127].inst, linha);
        while(fgets(linha, sizeof(linha), arquivo) != NULL){
			strncpy(aux, linha, 3);
			aux[3] = '\0';
			num = atoi(aux);
			if(256>num && num>127){
				strncpy(memoriaDados[num].inst, linha + 4, 16);
				memoriaDados[num].inst[16] = '\0';
			}
		}
	}
    fclose(arquivo); //Fecha o arquivo após a leitura
}

void carregaMem(char palavra[] , struct instrucao* memoria){
	char binarioVI[6], binarioIV[4], binarioIII[3], binarioII[12];
	int indiceS; // Indice Switch
	int decimal; // Variavel para preencher o vetor memoria
	inicializaInst(memoria);
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';
	indiceS = cbd(binarioIV); // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
	switch (indiceS){
		case 0://tipo R
			strcpy(memoria->palavra, palavra); // Guarda a palavra de instrucao
			
			memoria->opcode = indiceS; // indiceS Possui decimal do opcode
			
			strncpy(binarioIII, palavra +4, 3); // Copia para a string[3] as tres primeiras posicoes da palavra[] apartir da quarta posicao
			binarioIII[3] = '\0'; // Sinaliza o fim da String
			decimal = cbd(binarioIII); // Conversao binario(string)/decimal(int)
			memoria->rs = decimal; // Membro recebe valor decimal		RS
			
			strncpy(binarioIII, palavra +7, 3); // ||		...	Apartir da setima posicao
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioIII, palavra +10, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rd = decimal; // RD
			
			strncpy(binarioIII, palavra +13, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->funct = decimal; // FUNCT
			break;
		
		case 2:// tipo j
			strcpy(memoria->palavra, palavra); // PALAVRA
			memoria->opcode = indiceS;
			
			strncpy(binarioII, palavra +4, 12);
			binarioII[12] = '\0';
			memoria->addr = cbd(binarioII);
			break;
		//tipo i
		case 4: //ADDi
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1')
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
			else
				decimal = cbd(binarioVI);
			memoria->imm = decimal; // IMM
			break;
			
		case 8: //BEQ
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE

			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			decimal = cbd(binarioVI);
			memoria->imm = decimal; // ADDR
			break;
			
		case 11: //LW
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1'){
				decimal = cb2d(binarioVI); // cb2d = Conversao binario(complemento de dois)/decimal
				}
			else{
				decimal = cbd(binarioVI); // 											RT É O DESTINO!!
			memoria->addr = decimal;} // ADDR
			break;
				
		case 15: //SW
			strcpy(memoria->palavra, palavra); // PALAVRA
			
			memoria->opcode = indiceS; // OPCODE
			
			strncpy(binarioIII, palavra +4, 3); 
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII);
			memoria->rs = decimal; // RS
			
			strncpy(binarioIII, palavra +7, 3);
			binarioIII[3] = '\0';
			decimal = cbd(binarioIII); // 												ADDR É O DESTINO!!!!
			memoria->rt = decimal; // RT
			
			strncpy(binarioVI, palavra +10, 6);
			binarioVI[6] = '\0';
			if(binarioVI[0] == '1'){
				decimal = cb2d(binarioVI);} // cb2d = Conversao binario(complemento de dois)/decimal
			else{
				decimal = cbd(binarioVI); 										
			memoria->imm = decimal; }// ADDR
			break;
	}
	/*
    printf("instrucao: %s\t",memoria->palavra);
    printf("opcode: %d\t", memoria->opcode); //esta passando um valor de 1 strcut so
    printf("RS: %d\t", memoria->rs);
    printf("RT: %d\t", memoria->rt);
    printf("RD: %d\t", memoria->rd);
    printf("Imm: %d\t", memoria->imm);
    printf("Addr: %d\t", memoria->addr);
    printf("Funct: %d\n\n", memoria->funct);
    * */
}
ResulULA ula(int val1, int val2, int ulaOp) {
	ResulULA val;
    switch(ulaOp) {
		case 0:  // ADD/SW/LW/ADDi
			val.resul = val1 + val2;
			break;
		case 2: 
			val.resul = val1 - val2; // SUB
			break;
		case 4: 
			val.resul = val1 & val2; // AND
			break;
		case 5: 
			val.resul = val1 | val2; // OR
			break;
		}
	if(val.resul > -128 && val.resul<127){
		val.OF = 0;
	}else{
		val.OF = 1;
	}
	if(val.resul == 0){
		val.zero = 1;
	}else{
		val.zero = 0;
	}
    return val;
}
int pot(int base, int expoente){
	int valor = base, i;
	if(expoente > 0){
		for(i=1; i<expoente; i++)  // Caso um, retorna sem alteracoes
			valor = valor * base;
		
		return(valor);
	}
	return(1); // Caso zero, retorna 1
}
int cb2d(char complemento[]){ // CB2D = Conversao binaria(complemento de dois)/decimal
	int potencia = 0, decimal = 0;
	int indice;
	indice = strlen(complemento) -1; // indice recebe tamanho do vetor -1 para percorrer coretamente durante o laço 
	while(complemento[indice] != '1'){ // Busca a primeira posicao preenchida com o caractere '1'
		potencia++; // potencia sobe conforme o vetor é percorrido partindo do fim (bit menos significativo)
		indice--;   // indice que percorre o vetor
	}
	decimal = decimal + pot(2, potencia); // Incrementa a variavel de retorno uma vez para o primeiro caractere '1'
	potencia++;
	indice--;
	while(indice >= 0){ // Continua percorrendo o vetor, desa vez ate o final...
		if(complemento[indice] == '0'){
			decimal = decimal + pot(2, potencia); // Incrementa o vetor agora quando o caractere igual a '0'
		}
		potencia++;
		indice--;
	}
	decimal = decimal *(-1); // Retorna o valor resultante negativo(multipicado por -1)
	return (decimal);
}
char* cdb(int inteiro){
	int i=0;
	char* binario = (char*)malloc(sizeof(char)*16);
	while(inteiro != 0){
		if(inteiro%2 == 0)
			binario[i] = '0';
		else
			binario[i] = '1';
		inteiro = inteiro/2;
		i++;
	}
	return binario;
}
int cbd(char binario[]){ // CBD = conversao binaria/decimal
	int potencia = 0, decimal = 0;
	int indice;
	
	indice = strlen(binario) -1; // indice recebe tamanho do vetor -1 para percorrer coretamente durante o laço 
	while(indice >= 0){
		if(binario[indice] == '1'){ // apenas há o incremento na variavel de retorno caso o caractere seja "1" em tal indice
			decimal = decimal + pot(2, potencia);
		}
		potencia++; // potencia sobe conforme o vetor é percorrido partindo do fim (bit menos significativo)
		indice--;   // indice que percorre o vetor
	}
	return (decimal);
}
int ler_registrador(RegistradoresMIPS *mips, int reg) {
    if (reg >= 0 && reg < 8) {
        return mips->registradores[reg];
    }
    return 0;
}

void escrever_registrador(RegistradoresMIPS *mips, int reg, int valor) {
    if (reg != 0 && reg >= 0 && reg < 32) {
        mips->registradores[reg] = valor;
    }
}

void controle(struct instrucao* RegInst, RegistradoresMIPS* mips, struct mem Mem[], unsigned int* PC, int* topoInst, int estado){
	ResulULA val;
	int ULASaida, RDM;
	char* aux;
	while(estado != ESTADO_FINAL) {
		switch(estado){
			case ESTADO_B:
				carregaMem(Mem[*PC].inst, RegInst);
				val = ula(*PC, 1, 0);
				*PC = val.resul;
				estado = ESTADO_D;
				menuExecucao(*mips, Mem, *topoInst, *PC, estado, RegInst);
				break;
				
			case ESTADO_D:
				int Reg_A = ler_registrador(mips, RegInst->rs);
				int Reg_B = ler_registrador(mips, RegInst->rt);
				val = ula(*PC, RegInst->imm, 0);//Calculo do desvio BEQ
				ULASaida = val.resul;
				estado = ESTADO_E;
				menuExecucao(*mips, Mem, *topoInst, *PC, estado, RegInst);
				break;
				
			case ESTADO_E:
				if(RegInst->opcode ==0){//tipo R
					val = ula(Reg_A, Reg_B, RegInst->funct);
					estado = ESTADO_EE;
				}
				else if(RegInst->opcode == 11 || RegInst->opcode == 15 || RegInst->opcode==4){ //lw ou sw ou addi
					val = ula(Reg_A, RegInst->imm, 0);
					estado = ESTADO_EE;
				} 
				else if (RegInst->opcode == 8) { //beq
					val = ula(Reg_A, Reg_B, 2);
					if(val.zero == 1)
						*PC = ULASaida;
					estado = ESTADO_FINAL;
				}
				else if (RegInst->opcode == 2) { //Jump
					*PC = RegInst->addr;
					estado = ESTADO_FINAL;
				} 
				else{
					printf("erro no opcode %d\n", RegInst->opcode);
					estado= ESTADO_FINAL;
				}
				menuExecucao(*mips, Mem, *topoInst, *PC, estado, RegInst);
				break;
		
			case ESTADO_EE:
				if(RegInst->opcode == 0){//tipo R
					escrever_registrador(mips, RegInst->rd, val.resul);
					estado = ESTADO_FINAL;
				}
				if(RegInst->opcode == 4){//addi
					escrever_registrador(mips, RegInst->rt, val.resul);
					estado = ESTADO_FINAL;
				}
				if(RegInst->opcode == 11){// lw 
					RDM = cbd(Mem[val.resul].inst);
					estado = ESTADO_LW;
				} 
				else if (RegInst->opcode == 15){// sw
					
					aux = cdb(ler_registrador(mips, RegInst->rt));
					strncpy(Mem[val.resul].inst, aux, 16);
					free(aux);
					estado = ESTADO_FINAL;
				}
				menuExecucao(*mips, Mem, *topoInst, *PC, estado, RegInst);
				break;
			
			case ESTADO_LW:
				if (RegInst->opcode == 11){//lw
					mips->registradores[RegInst->rt] = RDM;
				} 
				estado= ESTADO_FINAL;
				menuExecucao(*mips, Mem, *topoInst, *PC, estado, RegInst);
				break;
		}
	}
}
void InicializaMem(struct mem* memoria){//inicializa a memoria com zero
	for(int i=0; i<256; i++){
		memoria[i].inst[0] = '\0';
		for(int j=1; j<16 ; j++){
			memoria[i].inst[j] = '0';
		}
	}
}

//pilha step back - Peraca
desc* cria_desc(){
    desc* pilha = (desc*)malloc(sizeof(desc));
    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}
void PUSH(desc* pilha ,unsigned int PC, struct mem* Mem, RegistradoresMIPS* BancoReg){//inseri nodo
    nodo* aux = (nodo*)malloc(sizeof(nodo));
    aux->info = (struct elemento*)malloc(sizeof(struct elemento));
    aux->prox = pilha->topo;
    pilha->topo = aux;
    pilha->tamanho++; 
    aux->info->PC = PC;
    for(int i=0; i<256; i++){
		strcpy(pilha->topo->info->memory[i].inst, Mem[i].inst);
	}
	for(int i=0; i<8; i++){
		aux->info->Reg[i] = BancoReg->registradores[i];
	}
}
nodo* POP(desc* pilha){// remove o topo da pilhaq para voltar uma instrucao
    nodo* aux = pilha->topo;
    pilha->topo = pilha->topo->prox;
    pilha->tamanho--;
    return aux;
}
nodo* TOP(desc* pilha){//retorna o topo da pilha
    return pilha->topo;
}
int EMPTY(desc* pilha){//retorna tamanho da pilha
    return pilha->tamanho;
}
void BackInstruction(nodo* backup, struct mem* memoria, unsigned int* PC, RegistradoresMIPS* BancoReg){//Volta uma instrucao
	*PC = backup->info->PC;
	for(int i=0; i<256; i++){
		strcpy(memoria[i].inst, backup->info->memory[i].inst);
	}
	for(int i=0; i<8; i++){
		BancoReg->registradores[i] = backup->info->Reg[i];
	}
	printf("Voltou uma instrucao com sucesso!!\n");
}
void fre_pilha(desc* pilha){//limpa a pilha inteira
	nodo* aux = pilha->topo;
	while(pilha->topo != NULL){
		pilha->topo = pilha->topo->prox;
		free(aux);
		aux = pilha->topo;
	}
	free(pilha);
}
//fim da pilha - peraca
void InicializaReg(RegistradoresMIPS* Reg){
	for(int i=0; i<8; i++){
		Reg->registradores[i] = 0;
	}
}
int main(){
	int op = 1, aux;
	RegistradoresMIPS bancoReg;
    unsigned int programCounter = 0;
    desc* pilha = cria_desc(); //desc pilha
	nodo* stepback = NULL;
    int topoInst = 0;
    struct mem memoriaComp[MEM_SIZE]; // Passar referencia para alterar dados na funcao controle;
    InicializaReg(&bancoReg);//inicializa registradores com tudo zero
    InicializaMem(memoriaComp);//inicializa memoria com tudo zero
    //struct instrucao inst;
    carregar_memoria(memoriaComp, &topoInst);
    while(op != 0){
		printf("\n\t\t________________________________________________________\n");
		printf("\t\t|                    Menu de opcoes                     |\n");
		if(programCounter <= topoInst){
			printf("\t\t|\tA proxima instrucao e:");
			casm(memoriaComp[programCounter].inst);
			printf("\t\t|\n");
		}
		printf("\t\t|\tEscolha uma opcao:\t\t\t\t|\n");
		printf("\t\t|\t1- step\t\t\t\t2- stepback\t|\n\t\t|\t3- Imprime simulador\t\t4- run\t\t|\n\t\t|\t0- exit\t\t\t\t\t\t|\n");
		printf("\t\t|_______________________________________________________|\n");
		scanf("%i", &op);
		switch(op){
			case 1://executa
				if(programCounter <= topoInst){
					PUSH(pilha, programCounter, memoriaComp, &bancoReg);
					controle(&inst, &bancoReg, memoriaComp, &programCounter, &topoInst, ESTADO_B);
				}else{
					printf("Nao existe mais instrucoes!!!\n");
				}
				break;
			case 2://step back
				if(pilha->topo != NULL){
					stepback = POP(pilha);
					BackInstruction(stepback, memoriaComp, &programCounter, &bancoReg);
					free(stepback);
					imprimeRegistradores(bancoReg, programCounter);
					//imprimeDados(memoriaComp, topoInst);
				}else{
					printf("Nao tem instrucao mais para voltar\n");
				}
				break;	
			case 3://imprime simulador
				imprimeRegistradores(bancoReg, programCounter);
				//imprimeDados(memoriaComp, topoInst);
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
							//imprimeDados(memoriaComp, topoInst);
						}else{
							printf("Nao tem instrucao mais para voltar\n");
						}
					}						
				}	
				printf("Acabou o programa!!");
		
			case 0: //Exit
				fre_pilha(pilha);
				return 0;
		}			
	}
	return 0;
}
