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
#define ESTADO_E 2 // EXECUÇÃO(ULA)
#define ESTADO_EE 3 // ACESSO A MEMORIA
#define ESTADO_LW 4  //ESCREVE NA MEMORIA
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
    int dados;
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
struct elemento{
    unsigned int PC;
    struct mem memory[MEM_SIZE];
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

void carregar_memoria(struct mem memoriaDados[MEM_SIZE]) {
    char nomeArquivo[256], linha[20];
    int i = 0;
    FILE *arquivo = NULL;
    printf("Nome do arquivo:\t");
    setbuf(stdin, NULL);
    scanf("%s", nomeArquivo);
    if ((arquivo = fopen(nomeArquivo, "r")) == NULL)
		printf("\nFalha ao abrir o arquivo!\n");
	else {
		while (fgets(linha, sizeof(linha), arquivo) != NULL){
			linha[16] = '\0';
            if(strlen(linha)==16 && i<260){
                strcpy(memoriaDados[i].inst, linha);
            }else{
				if(strcmp(linha,"\n")!=0){
					memoriaDados[i].dados = atoi(linha);	
				}
            }
            linha[0] = '\0';
			i++;
		}
	}
    fclose(arquivo); //Fecha o arquivo após a leitura
}
void carregaMem(char palavra[] , struct instrucao* memoria){
	char binarioVI[6], binarioIV[4], binarioIII[3], binarioII[12];
	int indiceS; // Indice Switch
	int decimal; // Variavel para preencher o vetor memoria
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
    printf("instrução: \t");
    printf("opcode: %d\t", memoria->opcode); //esta passando um valor de 1 strcut so
    printf("RS: %d\t", memoria->rs);
    printf("RT: %d\t", memoria->rt);
    printf("RD: %d\t", memoria->rd);
    printf("Imm: %d\t", memoria->imm);
    printf("Addr: %d\t", memoria->addr);
    printf("Funct: %d\n\n", memoria->funct);
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
		val.zero = 0;
	}else{
		val.zero = 1;
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

void controle(struct instrucao* instruct, RegistradoresMIPS *mips, struct mem Mem[], unsigned int* PC, int estado){
	ResulULA val;
	int BEQ, RDM;
	while(estado != ESTADO_FINAL) {
		switch(estado){
			case ESTADO_B:
				carregaMem(Mem[*PC].inst, instruct);
				val = ula(*PC, 1, 0);
				*PC = val.resul;
				estado = ESTADO_D;
				break;
				
			case ESTADO_D:
				int val_rs = ler_registrador(mips, instruct->rs);
				int val_rt = ler_registrador(mips, instruct->rt);
				val = ula(*PC, instruct->imm, 0);//Calculo do desvio BEQ
				BEQ = val.resul;
				estado = ESTADO_E;
				break;
				
			case ESTADO_E:
				if(instruct->opcode ==0){//tipo R
					val = ula(val_rs, val_rt, instruct->funct);
					estado = ESTADO_EE;
				}
				else if(instruct->opcode == 11 || instruct->opcode == 15 || instruct->opcode==4){ //lw ou sw ou addi
					val = ula(val_rs, instruct->imm, 0);
					estado = ESTADO_EE;
				} 
				else if (instruct->opcode == 8) { //beq
					if(val.resul == 0)
						*PC = BEQ;
					estado = ESTADO_FINAL;
				}
				else if (instruct->opcode == 2) { //Jump
					*PC = instruct->addr;
					estado = ESTADO_FINAL;
				} 
				else{
					printf("erro no opcode %d\n", instruct->opcode);
					estado= ESTADO_FINAL;
				}
				break;
		
			case ESTADO_EE:
				if(instruct->opcode == 0){//tipo R
					escrever_registrador(mips, instruct->rd, val.resul);
					estado = ESTADO_FINAL;
				}
				if(instruct->opcode == 4){//addi
					escrever_registrador(mips, instruct->rt, val.resul);
					estado = ESTADO_FINAL;
				}
				if(instruct->opcode == 11){// lw 
					RDM = Mem[val.resul].dados;
					estado = ESTADO_LW;
				} 
				else if (instruct->opcode == 15){// sw
					Mem[val.resul].dados = ler_registrador(mips, instruct->rt);
					estado = ESTADO_FINAL;
				}
				break;
			
			case ESTADO_LW:
				if (instruct->opcode == 11){//lw
					mips->registradores[instruct->rt] = RDM;
				} 
				estado= ESTADO_FINAL;
				break;
		}
	}
}
//step back
desc* cria_desc(){
    desc* pilha = (desc*)malloc(sizeof(desc));
    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}
void PUSH(desc* pilha ,back elementos){
    nodo* aux = (nodo*)malloc(sizeof(nodo));
    aux->prox = pilha->topo;
    pilha->topo = aux;
    pilha->tamanho++; 
    pilha->info->PC = elementos->PC;
    for(int i=0; i<256; i++){
		pilha->info->memory[i] = elementos->memory[i];
	}
}
back elementos(int PC, struct mem* Mem){
	back aux;
	aux.PC = PC;
	for(int i=0; i<256; i++){
		aux.memory[i] = Mem[i]->dados;
	}
	return aux;
}
nodo* POP(desc* pilha){
    nodo* aux = pilha->topo;
    pilha->topo = pilha->topo->prox;
    pilha->tamanho--;
    return aux;
}
nodo* TOP(desc* pilha){
    return pilha->topo;
}
int EMPTY(desc* pilha){
    return pilha->tamanho;
}
int main(){
	RegistradoresMIPS mips;
	desc* pilha = cria_desc();
	back backup;
    unsigned int PC = 0;
    struct mem Mem[MEM_SIZE];
    struct instrucao instruct;
    carregar_memoria(Mem);
    while(PC != 10){
		controle(&instruct, &mips, Mem, &PC, ESTADO_B);
		backup = elementos(PC, Mem);
		PUSH(pilha, backup);
	}
	return 0;
}
