#include "bibli.h"

void inicializaInst(struct instrucao* inst){
	inst->addr = 0;
	inst->funct = 0;
	inst->imm = 0;
	inst->rd = 0;
	inst->rs = 0;
	inst->rt = 0;
	inst->opcode = 0;
}

void casm(char* palavra){// casm = Converte para .asm	
	char aux[17];
	aux[16] = '\0';
	strncpy(aux, palavra, 16);
	struct instrucao inst;
	int indiceSwitch, funct; // Indice Switch
	carregaMem(aux, &inst);
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
			printf("beq $r%d, $r%d, %d", inst.rt, inst.rs, inst.imm);
			break;
			
		case 11:
			printf("lw $r%d, %d($r%d)", inst.rt, inst.imm, inst.rs);
			break;
				
		case 15:
			printf("sw $r%d, %d($r%d)", inst.rt, inst.imm, inst.rs);
			break;
	}	
}

void assemb(char* palavra){			// casm = Converte para .asm
	FILE* arquivo;
	char nomeArquivo[10] = "saida.asm";
	if((arquivo = fopen(nomeArquivo, "a")) == NULL)
		printf("Problema ao abrir o arquivo!\n");
	else{
		char aux[17];
		aux[16] = '\0';
		strncpy(aux, palavra, 16);
		struct instrucao inst;
		int indiceSwitch, funct; // Indice Switch
		carregaMem(aux, &inst);
		indiceSwitch = inst.opcode; // Conversao do opcode para decimal(Inteiro), a fim de acessar o Switch
		funct = inst.funct;
		switch (indiceSwitch){
			case 0: // Tipo R
				switch (funct){ // Funct
					case 0:
						if(inst.rs != 0 || inst.rt != 0 || inst.rd != 0)
							fprintf(arquivo, "add $r%d, $r%d, $r%d\n", inst.rd, inst.rs, inst.rt);
						break;
							
					case 2:
						fprintf(arquivo, "sub $r%d, $r%d, $r%d\n", inst.rd, inst.rs, inst.rt);
						break;
						
					case 4:
						fprintf(arquivo, "and $r%d, $r%d, $r%d\n", inst.rd, inst.rs, inst.rt);
						break;
							
					case 5:
						fprintf(arquivo, "or $r%d, $r%d, $r%d\n", inst.rd, inst.rs, inst.rt);
						break;
						}
				break;	
							
			case 2:
				fprintf(arquivo, "j %d\n", inst.addr);
				break;
			
			case 4:
				fprintf(arquivo, "addi $r%d, $r%d, %d\n", inst.rt, inst.rs, inst.imm);
				break;
				
			case 8:
				fprintf(arquivo, "beq $r%d, $r%d, %d\n", inst.rt, inst.rs, inst.imm);
				break;
				
			case 11:
				fprintf(arquivo, "lw $r%d, %d($r%d)\n", inst.rt, inst.imm, inst.rs);
				break;
					
			case 15:
				fprintf(arquivo, "sw $r%d, %d($r%d)\n", inst.rt, inst.imm, inst.rs);
				break;
		}	
		fclose(arquivo);
	}
}


void imprimeRegistradores(RegistradoresMIPS bancoReg, unsigned int* programCounter) {
    printf("PC: %d\n", *programCounter);
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
			strncpy(sAux, memoriaComp[aux].inst, 16);
			sAux[16] = '\0';
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

void menuExecucao (RegistradoresMIPS* bancoReg, struct mem memoriaComp[], int *limiteInst, unsigned int* programCounter, int* estado, struct instrucao* RegInst, struct desc_pilha* pilha, int *OP, int *ULASaida, int *RDM, int *Reg_A, int *Reg_B, int flag){
	nodo* stepback = NULL; 	// Aux para voltar instrucao
	if(flag != 1){
		while(*OP != 5){
			printf("----------------------------------------Menu de Execucoes----------------------------------------\n");
			printf("Estado: %i\tInstrucao:", *estado);
			if(*estado > 1)
				casm(RegInst->palavra);
			else
				printf("%s", RegInst->palavra);
			printf("\n\n\tReg_A: %i\n\tReg_B: %i\n\tRDM: %i\n\tULASaida: %i\n", *Reg_A, *Reg_B, *RDM, *ULASaida);
			printf("\n1- Imprime simulador\t2- Stepback\t3- Carrega .mem\t\t4- Gerar o assembly\t5- Executar o estado\n\n");
			setbuf(stdin, NULL);
			getchar();
			scanf("%d", OP);
			switch(*OP){
				case 1:
					imprimeRegistradores(*bancoReg, programCounter);
					imprimeDados(memoriaComp, *limiteInst);
					printf("\n");
					break;
				case 2:
					if(pilha->topo != NULL){
						stepback = POP(pilha);
						BackInstruction(stepback, memoriaComp, programCounter, bancoReg, ULASaida, RDM, estado, Reg_A, Reg_B, RegInst);
						free(stepback);
						imprimeRegistradores(*bancoReg, programCounter);
						imprimeDados(memoriaComp, *limiteInst);
						printf("\n\nULASaida: %i\tRDM: %i\tEstado: %i\tReg_A: %i\tReg_B: %i\tInstrucao: %s\n\n", *ULASaida, *RDM, *estado, *Reg_A, *Reg_B, RegInst->palavra);
					}else{
						printf("Nao tem instrucao mais para voltar\n");
					}
					break;
				case 3:
					InicializaMem(memoriaComp);
					InicializaReg(bancoReg);
					carregar_memoria(memoriaComp, limiteInst);
					break;
				case 4:
					for(int i=0; i<*limiteInst; i++)
						assemb(memoriaComp[i].inst);
					break;
				case 5:
					break;
				case 6:
					break;
					
				default:
					printf("Opcao invalida!!\n");
					break;
			}
		}
		*OP=0;
	}else{
		printf("\n\n\tReg_A: %i\n\tReg_B: %i\n\tRDM: %i\n\tULASaida: %i\n", *Reg_A, *Reg_B, *RDM, *ULASaida);
	}
}


void carregar_memoria(struct mem* memoriaDados, int* limiteInst) {
	char nomeArquivo[256], linha[21], aux[4];
    int num;
    *limiteInst = 0;
    FILE *arquivo = NULL;
    printf("Nome do arquivo:\t");
    setbuf(stdin, NULL);
    scanf("%s", nomeArquivo);
    if ((arquivo = fopen(nomeArquivo, "r")) == NULL)
		printf("\nFalha ao abrir o arquivo!\n");
	else {
		while (strncmp(fgets(linha, sizeof(linha), arquivo), ".data", 5) != 0 && (*limiteInst)<127){
            strcpy(memoriaDados[*limiteInst].inst, linha);
            memoriaDados[*limiteInst].inst[16] = '\0';
            (*limiteInst)++;
        }	
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
	int decimal; // Variavel para preencher o vetor memoria
	strncpy(binarioIV, palavra +0, 4);
	binarioIV[4] = '\0';

	strcpy(memoria->palavra, palavra); // Guarda a palavra de instrucao
	
	memoria->opcode = cbd(binarioIV); // Opcode
	
	strncpy(binarioIII, palavra +10, 3);
	binarioIII[3] = '\0';
	decimal = cbd(binarioIII);
	memoria->rd = decimal; // RD
	
	strncpy(binarioIII, palavra +13, 3);
	binarioIII[3] = '\0';
	decimal = cbd(binarioIII);
	memoria->funct = decimal; // FUNCT


	
	strncpy(binarioII, palavra +4, 12);
	binarioII[12] = '\0';
	memoria->addr = cbd(binarioII); // Addr
	
	
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
	memoria->imm = decimal; }// Imm
			
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
		printf("\n\n OVERFLOW!!\n\n");
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

void controle(struct instrucao* RegInst, RegistradoresMIPS* mips, struct mem Mem[], unsigned int* PC, int* topoInst){
	ResulULA val;
	int ULASaida = 0, RDM = 0, flag;
	char* aux;
	int Reg_A = 0, Reg_B = 0;
	int estado = ESTADO_B, op=5;
	desc* pilha = cria_desc(); 	// Desc pilha
	printf("\t\tMenu:\n\t1- Run\n\t2- Step\n");
	scanf("%i", &flag);
	while(*PC <= *topoInst) {
		menuExecucao(mips, Mem, topoInst, PC, &estado, RegInst, pilha, &op, &ULASaida, &RDM, &Reg_A, &Reg_B, flag);
		if(op != 2){
			PUSH(pilha, *PC, Mem, mips, ULASaida, RDM, RegInst, estado, Reg_A, Reg_B);
			switch(estado){
				case ESTADO_B:
					strcpy(RegInst->palavra, Mem[*PC].inst);
					val = ula(*PC, 1, 0);
					*PC = val.resul;
					estado = ESTADO_D;
					printf("\nEstagio IF\n\tInstrucao:\t%s\n\tPC incrementado:\t%d\n\n", RegInst->palavra, *PC);
					break;
					
				case ESTADO_D:
					carregaMem(RegInst->palavra, RegInst);
					Reg_A = ler_registrador(mips, RegInst->rs);
					Reg_B = ler_registrador(mips, RegInst->rt);
					val = ula(*PC, RegInst->imm, 0);//Calculo do desvio BEQ
					ULASaida = val.resul;
					estado = DecodificaInst(estado, *RegInst);
					printf("\n\nEstagio ID\n\tRegistador A:\t%d\n\tRegistrador B:\t%d\n\tULASaida: %i\n\tInstrucao decodificada:\n", Reg_A, Reg_B, ULASaida);
					printf("opcode: %d\t", RegInst->opcode); 
					printf("RS: %d\t", RegInst->rs);
					printf("RT: %d\t", RegInst->rt);
					printf("RD: %d\t", RegInst->rd);
					printf("Imm: %d\t\t", RegInst->imm);
					printf("Addr: %d\t", RegInst->addr);
					printf("Funct: %d\n\n", RegInst->funct);
					break;
					
				case ESTADO_CalEnd:
					val = ula(Reg_A, RegInst->imm, 0);
					ULASaida = val.resul;
					estado = DecodificaInst(estado, *RegInst);
					printf("\n\nEstagio EX\n\tSaida ULA:\t%d\n\n", ULASaida);
					break;
					
				case ESTADO_AcessoMem:
					RDM = cbd(Mem[ULASaida].inst);
					estado = ESTADO_EscRT;
					printf("\n\nEstagio MEM\n\tEndereco da memoria: %d\n\tRDM:\t%d\n\n", ULASaida, RDM);
					break;
				
				case ESTADO_EscRT:
					mips->registradores[RegInst->rt] = RDM;
					estado= ESTADO_B;
					printf("\n\nEstagio WB\n\tRegistrador:\t%d\n\tValor Escrito(RDM):\t%d\n\n", RegInst->rt, RDM);
					break;
										
				case ESTADO_EscMem:
					aux = cdb(ler_registrador(mips, RegInst->rt));
					strncpy(Mem[ULASaida].inst, aux, 16);
					free(aux);
					estado= ESTADO_B;
					printf("\n\nEstagio MEM\n\tEndereco da memoria: %d\n\tValor Escrito(RDM):\t%d\n\n", ULASaida, RDM);
					break;
					
				case ESTADO_EscAddi:
					escrever_registrador(mips, RegInst->rt, ULASaida);
					estado = ESTADO_B;
					printf("\n\nEstagio WB\n\tRegistrador:\t%d\n\tValor Escrito(ULASaida):\t%d\n\n", RegInst->rt, ULASaida);
					break;
					
				case ESTADO_Exe:
					val = ula(Reg_A, Reg_B, RegInst->funct);
					ULASaida = val.resul;
					estado = ESTADO_EscRD;
					printf("\n\nEstagio EX\n\tSaida ULA:\t%d\n\n", ULASaida);
					break;
					
				case ESTADO_EscRD:
					escrever_registrador(mips, RegInst->rd, ULASaida);
					estado = ESTADO_B;
					printf("\n\nEstagio WB\n\tRegistrador:\t%d\n\tValor Escrito(ULASaida):\t%d\n\n", RegInst->rt, ULASaida);
					break;
					
				case ESTADO_DesvioCond:
					val = ula(Reg_A, Reg_B, 2);
					if(val.zero == 1)
						*PC = ULASaida;
					estado = ESTADO_B;
					printf("\n\nEstagio EX\n\tSaida ULA(teste para desvio):\t%d\n\n", ULASaida);
					break;
					
				case ESTADO_DesvioIncond:
					*PC = RegInst->addr;
					estado = ESTADO_B;
					printf("\n\nEstagio EX\n\tPC com o desvio:\t%d\n\n", *PC);
					break;
					
					
				default:
					printf("erro no opcode %d\n", RegInst->opcode);
					estado= ESTADO_B;
					break;
			}
		}
	}
	printf("\nAcabou o programa!!\n");
}
int DecodificaInst(int estado, struct instrucao inst){
	if(estado == 1){
		if(inst.opcode == 0)
			return ESTADO_Exe;
		if(inst.opcode == 2)
			return ESTADO_DesvioIncond;
		if(inst.opcode == 8)
			return ESTADO_DesvioCond;
		if(inst.opcode == 11 || inst.opcode == 15 || inst.opcode == 4)
			return ESTADO_CalEnd;
	}
	if(estado == 2){
		if(inst.opcode == 4)
			return ESTADO_EscAddi;
		if(inst.opcode == 11)
			return ESTADO_AcessoMem;
		if(inst.opcode == 15)
			return ESTADO_EscMem;
	}
}
	
void InicializaMem(struct mem* memoria){//Inicializa a memoria com zero
	for(int i=0; i<256; i++){
		memoria[i].inst[0] = '\0';
		for(int j=1; j<16 ; j++){
			memoria[i].inst[j] = '0';
		}
	}
}

//Pilha step back - Peraca
desc* cria_desc(){
    desc* pilha = (desc*)malloc(sizeof(desc));
    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}
void PUSH(desc* pilha ,unsigned int PC, struct mem* Mem, RegistradoresMIPS* BancoReg, int ULASaida, int RDM, struct instrucao* RegInst, int estado, int Reg_A, int Reg_B){//Inseri nodo
    nodo* aux = (nodo*)malloc(sizeof(nodo));
    aux->info = (struct elemento*)malloc(sizeof(struct elemento));
    aux->prox = pilha->topo;
    pilha->topo = aux;
    pilha->tamanho++; 
    aux->info->PC = PC;
    aux->info->ULASaida = ULASaida;
    aux->info->RDM = RDM;
    aux->info->inst.imm = RegInst->imm;
    strcpy(aux->info->inst.palavra, RegInst->palavra);
    aux->info->inst.funct =RegInst->funct;
    aux->info->inst.addr = RegInst->addr;
    aux->info->inst.opcode = RegInst->opcode;
    aux->info->inst.rs = RegInst->rs;
    aux->info->inst.rd = RegInst->rd;
    aux->info->inst.rt = RegInst->rt;
    aux->info->estado = estado;
    aux->info->Reg_A = Reg_A;
    aux->info->Reg_B = Reg_B;
    
    for(int i=0; i<256; i++){
		strcpy(pilha->topo->info->memory[i].inst, Mem[i].inst);
	}
	for(int i=0; i<8; i++){
		aux->info->Reg[i] = BancoReg->registradores[i];
	}
}
nodo* POP(desc* pilha){// Remove o topo da pilhaq para voltar uma instrucao
    nodo* aux = pilha->topo;
    pilha->topo = pilha->topo->prox;
    pilha->tamanho--;
    return aux;
}
nodo* TOP(desc* pilha){//Retorna o topo da pilha
    return pilha->topo;
}
int EMPTY(desc* pilha){//Retorna tamanho da pilha
    return pilha->tamanho;
}
void BackInstruction(nodo* backup, struct mem* memoria, unsigned int* PC, RegistradoresMIPS* BancoReg, int *ULASaida, int *RDM, int *estado, int *Reg_A, int *Reg_B, struct instrucao* RegInst){//Volta uma instrucao
	*PC = backup->info->PC;
	for(int i=0; i<256; i++){
		strcpy(memoria[i].inst, backup->info->memory[i].inst);
	}
	for(int i=0; i<8; i++){
		BancoReg->registradores[i] = backup->info->Reg[i];
	}
	*RDM = backup->info->RDM;
	*estado = backup->info->estado;
	*Reg_A = backup->info->Reg_A;
	*Reg_B = backup->info->Reg_B;
	*RDM = backup->info->RDM;
	RegInst->imm = backup->info->inst.imm;
    strcpy(RegInst->palavra, backup->info->inst.palavra);
    RegInst->funct = backup->info->inst.funct;
    RegInst->addr = backup->info->inst.addr;
    RegInst->opcode = backup->info->inst.opcode;
    RegInst->rs = backup->info->inst.rs;
    RegInst->rd = backup->info->inst.rd;
    RegInst->rt = backup->info->inst.rt;
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
