#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <new>
#include <ctype.h>
#include <stdlib.h>
#include <sstream>
#include <vector>

using namespace std;

//**************************** LISTA EQU *******************************************************

class no {
	string rot; //Rotulo associado pela diretiva EQU
	string val_rotulo; //Valor associado ao rotulo
	no *prox_rot; //Posicao do proximo rotulo
public:
	no() {};
	void Def_Rotulo (string rotulo) {rot = rotulo;} //Define o rotulo de um no
	void Def_Valor (string valor) {val_rotulo = valor;} //Define o valor associado a um rotulo
	void Def_prox_rot (no *prox) {prox_rot = prox;} //Define proximo no
	string Return_rot() {return rot;}
	string Return_val() {return val_rotulo;}
	no* Return_prox_rot () {return prox_rot;}
};

class EQU_List { //Lista para as definicoes das diretivas EQU
	public:
		no *head;
		EQU_List() {
			head = NULL;
		}
		void insere_final (string,string); //Insere no no final da lista
		string busca_no (string,bool&); //Busca no na lista a partir do rotulo
		void limpa_lista();
};

void EQU_List::insere_final (string rotulo, string val) {
	no *novo_no = new no();
	novo_no->Def_Rotulo(rotulo);
	novo_no->Def_Valor(val);
	novo_no->Def_prox_rot(NULL);
	no *tmp = head;

	if(tmp != NULL) { //Se existem nos na lista, ir para o final
			while(tmp->Return_prox_rot() != NULL) {
				tmp = tmp->Return_prox_rot();
			}
		tmp->Def_prox_rot(novo_no);
	} else {
		head = novo_no;
	}
}

string EQU_List::busca_no (string label, bool& exist) { //Busca no na lista
	bool *aux = &exist;
	no *tmp = head;
	string tmpval;
	while(tmp != NULL) {
		if(tmp->Return_rot() == label) { //Se tiver rotulo == label, retornar valor associado e coloca flag exist em true
			tmpval = tmp->Return_val();
			*aux = true;
			break;
		}
		tmp = tmp->Return_prox_rot();
	}
	if(tmp == NULL) {
		*aux = false;
		return "";
	}
		return tmpval;
}

void EQU_List::limpa_lista () {
	no *tmp = head;
	if (tmp == NULL)
		return;
	no *aux;
	do {
		aux = head->Return_prox_rot();
		head = aux;
		delete tmp;
		tmp = head;
	} while(tmp != NULL);
}
//*********************************************** FIM DA LISTA EQU *******************************************************

//********************************************* INICIO DA STRUCT BSS *****************************************************
typedef struct BSS {
	string variavel;
	string size;
}BSS;
//************************************************************************************************************************

void Preprocessamento(istream&,string);
void diretivas(istream&,ostream&);
void instrucoes(istream&,ostream&);
string InttoString(int);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "O programa deve receber apenas o arquivo .asm!" << endl;
		exit(EXIT_FAILURE);
	}
	std::string str1(argv[1],strlen(argv[1])); //Abrindo arquivo para Pre-processamento
	fstream entrada (argv[1],ios::in);
	if(entrada.is_open()) 
		Preprocessamento(entrada,str1); //Realizando Preprocessamento
	entrada.close(); // Fechando arquivo de entrada
	entrada.clear();
	str1.erase(str1.length()-4,4);
	str1 += ".s";
	fstream saida (str1.c_str(),ios::out); //Criando arquivo de saida .s
	str1.erase (str1.length()-2,2); //Abrindo arquivo Preprocessado
	str1 += "_Preproc.asm";
	fstream entradapreproc (str1.c_str(),ios::in);
	//Fim da abertura do arquivo

	//Chamada das funções de traducao
	diretivas(entradapreproc,saida); //Imprimindo section .data e section .bss no arquivo de saida
	entradapreproc.clear(); //Retornando para o inicio do arquivo de entrada
	entradapreproc.seekg(0,ios::beg);
	saida.clear();
	instrucoes(entradapreproc,saida);
	entradapreproc.close();
	saida.close();

return 0; 
}

void Preprocessamento(istream &file, string nome_arq) {
EQU_List no; //Objeto da lista da diretiva EQU
string str, linha, strtemp;
string token, token1, token2, dir, linhaout, linhain, aux, aux2, aux3, aux4;
string rotulo, valorstr; //strings para abrir arquivo, guardar linhas, verificacoes de linhas e diretivas
unsigned int i=0, j=0, k=0, tam2=0, tam3=0, lin_cont=0,lin_cont_aux=0; //Contadores e variaveis para tamanho de linha
int flagIF1 = 0, flagIF2 = 0;//Flag para verificar erros, se existe rotulo, flags para diretiva IF, flag para verificar se o programa esta na ordem correta
bool eh_rotulo = false; //Variavel para definir se pertence a lista da diretiva EQU
int posit = 0; //Inteiro para pegar posicao das diretivas IF, CONST e SPACE
size_t pos; //Necessario para funcao str.find()

nome_arq.erase(nome_arq.length()-4,4);
nome_arq += "_Preproc.asm";
fstream out (nome_arq.c_str(),ios::out);

if(out.is_open()) {
while(getline(file,linha)) {
	lin_cont++;
	pos = linha.find("\t");
	posit = pos;
	if(posit > -1) {
		while(posit > -1) {
			aux = linha.substr(0,pos);
			aux2 = linha.substr(pos+1,string::npos);
			linha.clear();
			linha += aux;
			linha += ' ';
			linha += aux2;
			aux.clear();
			aux2.clear();
			pos = linha.find("\t");
			posit = pos;
		}
	}
	if(flagIF1 == 0 && flagIF2 == 1) { //Se achou a diretiva IF mas seu operando foi zero, ignorar linha
	   flagIF2 = 0;
	   continue;
	}
	token.clear(); //Limpando strings
	valorstr.clear();
	rotulo.clear();
	aux.clear();
	linhain.clear();
	eh_rotulo = false; //Colocando variavel em false para o caso de nova verificacao
	if(linha.length() == 0) { //Se a linha estiver em branco, ir para a proxima linha
		continue;
	}
	if(linha.at(0) == ';') { //Se for uma linha de comentario, ir para a proxima linha
		continue;
	}
	tam2 = linha.length();
	transform(linha.begin(), linha.end(), linha.begin(), ptr_fun<int, int>(toupper)); //Colocando em maiusculo
	for(i=0;i<tam2;i++) { //Verifica espacos no inicio da linha
		if(linha.at(i) != ' ') {
			k = i;
			break;
		}
	}
	if(i >= linha.length()-1 || linha.at(i) == ';')
		continue;
//************************************** LEITURA DA LINHA DO ARQUIVO DE ENTRADA **************************************************
	k = 0;
	i = 0;
	while(i < linha.length()) {
		for(i=k;i<tam2;i++) { //Verifica espacos na linha
			if(linha.at(i) != ' ') {
				k = i;
				break;
			}
		}
		if(i == tam2 || linha.at(i) == ';') { //Se houver apenas comentarios no final da linha, ir para a proxima
			break;
		}
		for(i=k;i<tam2;i++) {//Pega token
			if(linha.at(i) == ' ') {
				k = i;
				break;
			}
			if(linha.at(i) == ':') { //Se tiver rotulo
				token += linha.at(i);
				k = i+1;
				break;
			}
			token += linha.at(i);
		}
		aux = no.busca_no(token,eh_rotulo); //Procura se esta na lista EQU
		if(eh_rotulo == true) {
			linhain += aux;
			linhain += ' ';
		} else {
			linhain += token;
			linhain += ' ';
		}
		token.clear();
	}
	linhain.erase(linhain.length()-1,1); //Apagando espaço restante no final da linha
	if(flagIF1 == 1) { //Se achou a diretiva IF e seu operando foi diferente de 0, imprimir linha no arquivo de saida
			out << linhain;
			out << endl;
			linhain.clear();
			flagIF1 = 0;
			flagIF2 = 0;
			continue;
	}
	tam3 = linhain.length();
	k = 0; //Zerando contadores
	i = 0;
		cout << linhain << endl;
	//************************** Verificacao da Diretiva EQU ******************************************
		pos = linhain.find("EQU");
		posit = pos;
		if(posit > -1) {
			k = 0;
			for(i=k;i<tam3;i++) {
				if(linhain.at(i) == ':') { //Armazenando rotulo, como esperado para a diretiva EQU
					for(j=0;j<i;j++) {
						rotulo += linha.at(j);
					}
						k = i+2;
						break;
				}
			}
			for(i=k;i<tam3;i++) { //Pegando proximo operando, que deve ser a diretiva EQU
				if(linhain.at(i) == ' ') {
					k = i+1;
					break;
				}
			}
			for(i=k;i<tam3;i++) { //Pega numero apos o EQU
				valorstr += linhain.at(i);
			}
			no.insere_final(rotulo, valorstr); //Criando no na lista de rotulos da diretiva EQU
			continue;
		}    
			//*********************** Fim da verificacao da diretiva EQU **********************************

		pos = linhain.find("IF"); //Procura diretiva IF, para verificacao
		posit = pos;
		if(posit > -1) {
			for(j=pos+3;j<linhain.length();j++) {
				aux += linhain.at(j);
			}
			if(atoi(aux.c_str()) != 0) {
				flagIF1 = 1;
			}
			flagIF2 = 1; //Indica que entrou na diretiva IF, servindo para que nao se imprima a proxima linha
			continue;
		}
	out << linhain;
	out << endl;
	linhain.clear();
	}
}
//*******************************************************************************************************************************
no.limpa_lista();
out.close();
out.clear();
}

//****************************************************** FIM DO PRE PROCESSADOR ***********************************************

void diretivas(istream &entrada, ostream &saida) { //Procura pela SECTION DATA e Diretivas SPACE e CONST, gerando a section .data e section .bss
	string linha, var, dir, valor;
	bool flag_DATA = false, flag_SPACE = false; //Flag para indicar SECTION DATA e para indicar diretiva SPACE
	unsigned int i=0, k=0, j=0,num=1;
	vector<BSS> var_bss;
	while(getline(entrada,linha)) {
		if(linha == "SECTION DATA") { //Se a linha do arquivo for igual a SECTION DATA, colocar flag_DATA em true e pegar proxima linha
			flag_DATA = true;
			saida << "section .data" << endl;
			saida << "msg1 db 'Numero Invalido! Digite Novamente.',0ah" << endl;
			saida << "msg0 db 'Numero Invalido!',0ah" << endl;
			saida << "msg2 db 'Digite um caracter.',0ah" << endl;
			saida << endl;
			continue;
		}
		if(linha == "SECTION TEXT") { //Se encontrou a SECTION TEXT, colocar flag_data em false (Caso esta venha depois da SECTION DATA)
			flag_DATA = false;
		}
		if(flag_DATA == false) { //Se nao chegou na SECTION DATA, pegar proxima linha
			continue;
		} else {
			var.clear();
			valor.clear();
			dir.clear();
			k = 0;
			i = 0;
			for(i=0;i<linha.length();i++) { //Procurando rotulo, o qual sera o nome da variavel 
				if(linha.at(i) == ':') {
					k = i+2;
					break;
				}
				var += linha.at(i);
			}
			for(i=k;i<linha.length();i++) { //Verificando diretiva (CONST ou SPACE)
				if(linha.at(i) == ' ') {
					break;
				}
				dir += linha.at(i);		
			}
			k = i;
			if(k >= linha.length()-1) {
				if(dir == "SPACE") {
					var_bss.push_back(BSS());
					var_bss[j].variavel = var;
					valor = InttoString(num);
					var_bss[j].size = valor;
					flag_SPACE = true;
					j++;
				continue;
				}
			}
			for(i=k;i<linha.length();i++) { //Pegando valor apos diretiva
				valor += linha.at(i);
			}
			if(dir == "CONST") { //Se for a diretiva CONST, gravar no arquivo de saida
				saida << var;
				saida << " dd ";
				saida << valor;
				saida << endl;
			} 
			if(dir == "SPACE") { //Se for a diretiva SPACE, salvar dados para posterior gravacao
				var_bss.push_back(BSS());
				var_bss[j].variavel = var; //Salvando dados no elemento j do vetor
				var_bss[j].size = valor;
				flag_SPACE = true;
				j++; //Incrementando contador de elementos
			}
		}
	}
	if(flag_SPACE == true) { //Se entrou na diretiva SPACE tera section .bss
		saida << endl;
		saida << "section .bss" << endl;
		for(i=0;i<=j-1;i++) { //Imprimindo no arquivo de saida os elementos definidos pela diretiva SPACE na section .bss
			saida << var_bss[i].variavel;
			saida << " resd ";
			saida << var_bss[i].size;
			saida << endl;
	   }
	}
}

void instrucoes(istream &entrada,ostream &saida) { //Faz a traducao das instrucoes do assembly inventado
	string line, rotulo, inst, op1, op2, vet, vet1;
	unsigned int i1=0,k1=0,num=0,num1=4,num2=4;
	bool flag_TEXT=false, flag_rotulo, flag_Vet=false; //Flag para indicar que se esta na SECTION TEXT, flag para indicar que ha rotulo e flag para indicar tratamento de vetor
	saida << endl;
	saida << "section .text" << endl; //Iniciando section .text
	saida << "global _start" << endl; //Colocando variavel global _start
	saida << "_start:" << endl;
	saida << "  mov ebx,0" << endl; //Zerando contador escolhido (ebx)

	while(getline(entrada,line)) {
		if(line == "SECTION TEXT") {
			flag_TEXT = true;
		}
		if(line == "SECTION DATA") {
			flag_TEXT = false;
		}
		if(!flag_TEXT) {
			continue;
		} else { //Se ja chegou na SECTION TEXT
			cout << line << endl;
			rotulo.clear(); //Limpando Strings
			inst.clear();
			op1.clear();
			op2.clear();
			vet.clear();
			vet1.clear();
			flag_rotulo = false; //Reinicializando flags e contadores
			k1=0;
			i1=0;
			num1 = 4;
			num2 = 4;
			flag_Vet = false;

			for(i1=0;i1<line.length();i1++) {
				 if(line.at(i1) == ':') { //Se a linha possuir rotulo
					rotulo += line.at(i1);
					saida << "  " << rotulo;
					k1 = i1+2;
					flag_rotulo = true;
					break;
				}
				rotulo += line.at(i1);
			}

			for(i1=k1;i1<line.length();i1++) { //Pegando instrucao
				if(line.at(i1) == ' ') {
					k1 = i1+1;
					break;
				}
				inst += line.at(i1);
			}

			//Inicio da traducao da instrucao

			if(inst == "ADD") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) //Se nao estamos tratando com vetores
					saida << "  add ebx," << '[' << op1 << ']' << endl; //Gravando codigo IA32 correspondente na saida
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					saida << "  add ebx," << '[' << op1 << '+' << vet << ']' << endl; //Gravando codigo IA32 correspondente na saida
				}

			} else if(inst == "SUB") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) //Se nao estamos tratando com vetores
					saida << "  sub ebx," << '[' << op1 << ']' << endl; //Gravando codigo IA32 correspondente na saida
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					saida << "  sub ebx," << '[' << op1 << '+' << vet << ']' << endl; //Gravando codigo IA32 correspondente na saida
				}

			} else if(inst == "MULT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) { //Se nao estamos tratando com vetores
					if(flag_rotulo == false) {
						saida << "  push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "  mov eax,ebx" << endl;
						saida << "  imul dword " <<  '[' << op1 << ']' << endl;
						saida << "  mov ebx,eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "      mov eax,ebx" << endl;
						saida << "      imul dword " <<  '[' << op1 << ']' << endl;
						saida << "      mov ebx,eax" << endl;
						saida << "      pop eax" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "  mov eax,ebx" << endl;
						saida << "  imul dword " << '[' << op1 << '+' << vet << ']' << endl; 
						saida << "  mov ebx,eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "      mov eax,ebx" << endl;
						saida << "      imul dword " <<  '[' << op1 << '+' << vet << ']' << endl;
						saida << "      mov ebx,eax" << endl;
						saida << "      pop eax" << endl;
					}
				}

			} else if(inst == "DIV") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) { //Se nao estamos tratando com vetores
					if(flag_rotulo == false) {
						saida << "  push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "  push edx" << endl;
						saida << "  mov eax,ebx" << endl;
						saida << "  cdq" << endl;
						saida << "  idiv dword " <<  '[' << op1 << ']' << endl;
						saida << "  mov ebx,eax" << endl;
						saida << "  pop edx" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "      push edx" << endl;
						saida << "      mov eax,ebx" << endl;
						saida << "      cdq" << endl;
						saida << "      idiv dword " <<  '[' << op1 << ']' << endl;
						saida << "      mov ebx,eax" << endl;
						saida << "      pop edx" << endl;
						saida << "      pop eax" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "  push edx" << endl;
						saida << "  mov eax,ebx" << endl;
						saida << "  cdq" << endl;
						saida << "  idiv dword " <<  '[' << op1 << '+' << vet << ']' << endl;
						saida << "  mov ebx,eax" << endl;
						saida << "  pop edx" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl; //Gravando codigo IA32 correspondente na saida
						saida << "      push edx" << endl;
						saida << "      mov eax,ebx" << endl;
						saida << "      cdq" << endl;
						saida << "      idiv dword " <<  '[' << op1 << '+' << vet << ']' << endl;
						saida << "      mov ebx,eax" << endl;
						saida << "      pop edx" << endl;
						saida << "      pop eax" << endl;
					}
				}
				
			} else if(inst == "JMP") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					op1 += line.at(i1);
				}
				saida << "  jmp " << op1 << endl;

			} else if(inst == "JMPN") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					op1 += line.at(i1);
				}
				if(flag_rotulo == false) {
				saida << "  cmp ebx,0" << endl;
				saida << "  jl " << op1 << endl;
				} else {
					saida << " cmp ebx,0" << endl;
					saida << "      jl " << op1 << endl;
				}   

			} else if(inst == "JMPP") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					op1 += line.at(i1);
				}
				if(flag_rotulo == false) {
					saida << "  cmp ebx,0" << endl;
					saida << "  jg " << op1 << endl;
				} else {
					saida << " cmp ebx,0" << endl;
					saida << "      jg " << op1 << endl;
				}   

			} else if(inst == "JMPZ") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					op1 += line.at(i1);
				}
				if(flag_rotulo == false) {
					saida << "  cmp ebx,0" << endl;
					saida << "  je " << op1 << endl;
				} else {
					saida << " cmp ebx,0" << endl;
					saida << "      je " << op1 << endl;
				}   

			} else if (inst == "LOAD") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1)  //Se nao estamos tratando com vetores
					saida << "  mov ebx," << '[' << op1 << ']' << endl;
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					saida << "  mov ebx," << '[' << op1 << '+' << vet << ']' << endl; //Gravando codigo IA32 correspondente na saida
				}

			} else if (inst == "STORE") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length()) {
					i1--;
				}
				if(i1 >= line.length()-1)  //Se nao estamos tratando com vetores
					saida << "  mov " << '[' << op1 << ']' << ",ebx" << endl;
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					saida << "  mov " << '[' << op1 << '+' << vet << ']' << ",ebx" << endl; //Gravando codigo IA32 correspondente na saida
				}

			} else if (inst == "STOP") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					op1 += line.at(i1);
				}
				if(flag_rotulo == false) {
					saida << "  mov eax,1" << endl;
					saida << "  mov ebx,0" << endl;
					saida << "  int 80h" << endl;
				} else {
					saida << " mov eax,1" << endl;
					saida << "  mov ebx,0" << endl;
					saida << "  int 80h" << endl;
				}   

			} else if (inst == "COPY") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == ',' || line.at(i1) == '+') { 
						k1 = i1 + 1;
						break;
					}
					op1 += line.at(i1);
				}
				if(line.at(i1) == '+') { //Se estivermos trabalhando com vetores no primeiro operando
					flag_Vet = true;
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) == ',') {
							k1 = i1+1;
							break;
						}
						vet += line.at(i1);
					}
				}
				for(i1=k1;i1<line.length();i1++) {
					if(line.at(i1) == '+') { //Se estivermos trabalhando com vetores no segundo operando
						k1 = i1+1;
						break;
					}
					op2 += line.at(i1);
				}
				if (i1 >= line.length()-1 && flag_Vet == false) {
					if(flag_rotulo == false) {
						saida << "  push eax" << endl;
						saida << "  mov eax,[" << op1 << ']' << endl;
						saida << "  mov [" << op2 << "],eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl;
						saida << "  mov eax,[" << op1 << ']' << endl;
						saida << "  mov [" << op2 << "],eax" << endl;
						saida << "  pop eax" << endl;
					}  
				}	
				if(line.at(i1) == '+' && flag_Vet == false) { //Se estivermos trabalhando com vetores apenas no segundo operando
					for(i1=k1;i1<line.length();i1++) {
						vet1 += line.at(i1);
					}
					num = atoi(vet1.c_str());
					num2 = num2*num;
					vet1 = InttoString(num2);
					if(flag_rotulo == false) {
						saida << "  push eax" << endl;
						saida << "  mov eax,[" << op1 << ']' << endl;
						saida << "  mov [" << op2 << '+' << vet1 << "],eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl;
						saida << "  mov eax,[" << op1 << ']' << endl;
						saida << "  mov [" << op2 << '+' << vet1 << "],eax" << endl;
						saida << "  pop eax" << endl;
					}  
				} else if (line.at(i1) != '+' && flag_Vet == true) { //Se estivermos trabalhando com vetores apenas no primeiro operando
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push eax" << endl;
						saida << "  mov eax,[" << op1 << '+' << vet << ']' << endl;
						saida << "  mov [" << op2 << "],eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl;
						saida << "  mov eax,[" << op1 << '+' << vet << ']' << endl;
						saida << "  mov [" << op2 << "],eax" << endl;
						saida << "  pop eax" << endl;
					}  
				} else if (line.at(i1) == '+' && flag_Vet == true) { //Se estivermos trabalhando com vetores em ambos os operandos
					for(i1=k1;i1<line.length();i1++) {
						vet1 += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					num = atoi(vet1.c_str());
					num2 = num2*num;
					vet1 = InttoString(num2);
					if(flag_rotulo == false) {
						saida << "  push eax" << endl;
						saida << "  mov eax,[" << op1 << '+' << vet << ']' << endl;
						saida << "  mov [" << op2 << '+' << vet1 << "],eax" << endl;
						saida << "  pop eax" << endl;
					} else {
						saida << " push eax" << endl;
						saida << "  mov eax,[" << op1 << '+' << vet << ']' << endl;
						saida << "  mov [" << op2 << '+' << vet1 << "],eax" << endl;
						saida << "  pop eax" << endl;
					}  
				}

			} else if (inst == "INPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == ' ' || line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 >= line.length()) { //Se nao estamos tratando com vetores
					i1--;
				}
				if(line.at(i1) == ' ') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) == '+') {
							k1 = i1+1;
							break;
						}
					}
				}
				if(i1 >= line.length()-1) { //Se nao estamos tratando com vetores
					if(flag_rotulo == false) {
						saida << "  push " << op1 << endl;
						saida << "  call LeerInteiro" << endl;
					} else {
						saida << " push" << op1 << endl;
						saida << "      call LeerInteiro" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call LeerInteiro" << endl;
					} else {
						saida << " mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "		push eax" << endl;
						saida << "      call LeerInteiro" << endl;
					}
				}

			} else if (inst == "OUTPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == ' ' || line.at(i1) == '+'){ //Verificando se estamos tratando com vetores
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 >= line.length()) { //Se nao estamos tratando com vetores
					i1--;
				}
				if(line.at(i1) == ' ') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) == '+') {
							k1 = i1+1;
							break;
						}
					}
				}
				if(i1 >= line.length()-1) { //Se nao estamos tratando com vetores
					if(flag_rotulo == false) {
						saida << "  push " << op1 << endl;
						saida << "  call EscreverInteiro" << endl;
					} else {
						saida << " push " << op1 << endl;
						saida << "      call EscreverInteiro" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) != ' ') {
							k1 = i1;
							for(i1=k1;i1<line.length();i1++) {
								if(line.at(i1) == ' ') {
									break;
								}
								vet += line.at(i1);
							}
						}
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call EscreverInteiro" << endl;
					} else {
						saida << " mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "		push eax" << endl;
						saida << "      call EscreverInteiro" << endl;
					}
				}

			} else if (inst == "S_INPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == ',' || line.at(i1) == '+') {
						k1 = i1 + 1;
						break;
					}
					op1 += line.at(i1);
				}
				if(line.at(i1) == '+') {
					flag_Vet = true;
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) == ',') {
							k1 = i1 + 1;
							break;
						}
						vet += line.at(i1);
					}
				}
				for(i1=k1;i1<line.length();i1++) {
					op2 += line.at(i1);
				}
				num = atoi(op2.c_str());
				num++;
				op2 = InttoString(num);
				if(flag_Vet == false) {
					if(flag_rotulo == false) {
						saida << "  push DWORD " << op2 << endl;
						saida << "  push " << op1 << endl;
						saida << "  call LeerString" << endl;
					} else {
						saida << " push DWORD " << op2 << endl;
						saida << "      push " << op1 << endl;
						saida << "      call LeerString" << endl;
					}
				} else {
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push DWORD " << op2 << endl;
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call LeerString" << endl;
					} else {
						saida << " push DWORD " << op2 << endl;
						saida << "		mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "  	push eax" << endl;
						saida << "      call LeerString" << endl;
					}
				}

			} else if (inst == "S_OUTPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == ',' || line.at(i1) == '+') {
						k1 = i1 + 1;
						break;
					}
					op1 += line.at(i1);
				}
				if(line.at(i1) == '+') {
					flag_Vet = true;
					for(i1=k1;i1<line.length();i1++) {
						if(line.at(i1) == ',') {
							k1 = i1 + 1;
							break;
						}
						vet += line.at(i1);
					}
				}
				for(i1=k1;i1<line.length();i1++) {
					op2 += line.at(i1);
				}
				num = atoi(op2.c_str());
				num++;
				op2 = InttoString(num);
				if(flag_Vet == false) {
					if(flag_rotulo == false) {
						saida << "  push DWORD " << op2 << endl;
						saida << "  push " << op1 << endl;
						saida << "  call EscreverString" << endl;
					} else {
						saida << " push DWORD " << op2 << endl;
						saida << "      push " << op1 << endl;
						saida << "      call EscreverString" << endl;
					}
				} else {
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push DWORD " << op2 << endl;
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call EscreverString" << endl;
					} else {
						saida << " push DWORD " << op2 << endl;
						saida << "		mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "  	push eax" << endl;
						saida << "      call EscreverString" << endl;
					}
				}

			} else if (inst == "H_INPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+') {
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) {
					if(flag_rotulo == false) {
						saida << "  push " << op1 << endl;
						saida << "  call LeerHexa" << endl;
					} else {
						saida << " push " << op1 << endl;
						saida << "      call LeerHexa" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call LeerHexa" << endl;
					} else {
						saida << " mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "  	push eax" << endl;
						saida << "      call LeerHexa" << endl;
					}
				}

			} else if (inst == "H_OUTPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+') {
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) {
					if(flag_rotulo == false) {
						saida << "  push dword [" << op1 << ']' << endl;
						saida << "  call EscreverHexa" << endl;
					} else {
						saida << " push dword [" << op1 << ']'<< endl;
						saida << "      call EscreverHexa" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "  push dword [" << op1 << '+' << vet << ']' << endl;
						saida << "  call EscreverHexa" << endl;
					} else {
						saida << " push dword [" << op1 << '+' << vet << ']' << endl;
						saida << "      call EscreverHexa" << endl;
					}
				}

			} else if (inst == "C_INPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+') {
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) {
					if(flag_rotulo == false) {
						saida << "  push " << op1 << endl;
						saida << "  call LeerChar" << endl;
					} else {
						saida << " push " << op1 << endl;
						saida << "      call LeerChar" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call LeerChar" << endl;
					} else {
						saida << " mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "  	push eax" << endl;
						saida << "      call LeerChar" << endl;
					}
				} 

			} else if (inst == "C_OUTPUT") {
				for(i1=k1;i1<line.length();i1++) { //Pegando operando
					if(line.at(i1) == '+') {
						k1 = i1+1;
						break;
					}
					op1 += line.at(i1);
				}
				if(i1 == line.length())
					i1--;
				if(i1 >= line.length()-1) {
					if(flag_rotulo == false) {
						saida << "  push " << op1 << endl;
						saida << "  call EscreverChar" << endl;
					} else {
						saida << " push " << op1 << endl;
						saida << "      call EscreverChar" << endl;
					}
				}
				if(line.at(i1) == '+') {
					for(i1=k1;i1<line.length();i1++) {
						vet += line.at(i1);
					}
					num = atoi(vet.c_str());
					num1 = num1*num;
					vet = InttoString(num1);
					if(flag_rotulo == false) {
						saida << "	mov eax," << op1 << endl;
						saida << "	add eax," << vet << endl;
						saida << "  push eax" << endl;
						saida << "  call EscreverChar" << endl;
					} else {
						saida << " mov eax," << op1 << endl;
						saida << "		add eax," << vet << endl;
						saida << "  	push eax" << endl;
						saida << "      call EscreverChar" << endl;
					}
				}
			}
			//Fim da traducao da instrucao
		}
	}
	//Impressao das rotinas
	//LeerInteiro
	saida << "  LeerInteiro: enter 24,0" << endl;
	saida << "               push ebx" << endl;
	saida << "               push ecx" << endl;
	saida << "               push edx" << endl;
	saida << "               push esi" << endl;
	saida << "               push edi" << endl;
	saida << "               mov esi,0" << endl;
	saida << "               mov word [EBP-20],0" << endl;
	saida << "               Get_Int: mov eax,3" << endl;
	saida << "                        mov ebx,0" << endl;
	saida << "                        mov ecx,EBP" << endl;
	saida << "                        sub ecx,18" << endl;
	saida << "                        mov edx,12" << endl;
	saida << "                        int 80h" << endl;
	saida << "               mov ecx,eax" << endl;
	saida << "               mov [EBP-24],eax" << endl;
	saida << "               sub ecx,1" << endl;
	saida << "               mov eax,0" << endl;
	saida << "               mov edi,10" << endl;
	saida << "               mov ebx,0" << endl;
	saida << "               cmp byte [EBP-18],0x2D" << endl;
	saida << "               je Negativo" << endl;
	saida << "               Convert_Int: cmp byte [EBP-18+esi],0x30" << endl;
	saida << "                            jl Invalido" << endl;
	saida << "                            cmp byte [EBP-18+esi],0x39" << endl;
	saida << "                            jg Invalido" << endl;
	saida << "                            mov BL,[EBP-18+esi]" << endl;
	saida << "                            cmp esi,0" << endl;
	saida << "                            je First_Digit" << endl;
	saida << "                            cmp word [EBP-20],1" << endl;
	saida << "                            je Verif_Digit" << endl;
	saida << "                            Mult: mul edi" << endl;
	saida << "                            First_Digit: add eax,ebx" << endl;
	saida << "                                         sub eax,0x30" << endl;
	saida << "                                         inc esi" << endl;
	saida << "                            loop Convert_Int" << endl;
	saida << "                            jmp Armazena_Num" << endl;
	saida << "                            Verif_Digit: cmp esi,1" << endl;
	saida << "                                         je First_Digit" << endl;
	saida << "                                         jmp Mult" << endl;
	saida << "               Negativo: inc esi" << endl;
	saida << "                         dec ecx" << endl;
	saida << "                         mov word [EBP-20],1" << endl;
	saida << "                         jmp Convert_Int" << endl;
	saida << "               Invalido: mov eax,4" << endl;
	saida << "                         mov ebx,1" << endl;
	saida << "                         mov ecx,msg1" << endl;
	saida << "                         mov edx,35" << endl;
	saida << "                         int 80h" << endl;
	saida << "                         jmp Get_Int" << endl;
	saida << "               Armazena_Num: cmp word [EBP-20],1" << endl;
	saida << "                             je Nega_Num" << endl;
	saida << "                             mov ebx,[EBP+8]" << endl;
	saida << "                             mov dword [ebx],eax" << endl;
	saida << "                             jmp FIM_LeInt" << endl;
	saida << "               Nega_Num: neg eax" << endl;
	saida << "                         mov word [EBP-20],0" << endl;
	saida << "                         jmp Armazena_Num" << endl;
	saida << "               FIM_LeInt: mov eax,[EBP-24]" << endl;
	saida << "                    pop edi" << endl;
	saida << "                    pop esi" << endl;
	saida << "                    pop edx" << endl;
	saida << "                    pop ecx" << endl;
	saida << "                    pop ebx" << endl;
	saida << "                    leave" << endl;
	saida << "                    ret 4" << endl;

	//EscreverInteiro
	saida << "  EscreverInteiro: enter 16,0" << endl;
	saida << "                   push ebx" << endl;
	saida << "                   push ecx" << endl;
	saida << "                   push edx" << endl;
	saida << "                   push esi" << endl;
	saida << "                   push edi" << endl;
	saida << "                   mov dword [EBP-16],0" << endl;
	saida << "                   mov ebx,0" << endl;
	saida << "                   mov esi,0" << endl;
	saida << "                   mov ecx,10" << endl;
	saida << "                   mov edi,[EBP+8]" << endl;
	saida << "                   mov eax,[edi]" << endl;
	saida << "                   cmp eax,0" << endl;
	saida << "                   je Zero" << endl;
	saida << "                   cmp dword [edi],0" << endl;
	saida << "                   jl Neg" << endl;
	saida << "                   Positivo: cdq" << endl;
	saida << "                             cmp eax,0" << endl;
	saida << "                             je Imprime_Int" << endl;
	saida << "                             div ecx" << endl;
	saida << "                             add edx,0x30" << endl;
	saida << "                             cmp edx,0x30" << endl;
	saida << "                             jl Numero_Inv" << endl;
	saida << "                             cmp edx,0x39" << endl;
	saida << "                             jg Numero_Inv" << endl;
	saida << "                             mov [EBP-12+esi],edx" << endl;
	saida << "                             inc esi" << endl;
	saida << "                             jmp Positivo" << endl;
	saida << "                   Neg: mov ebx,1" << endl;
	saida << "                        neg eax" << endl;
	saida << "                        Neg_Loop: cdq" << endl;
	saida << "                                  cmp eax,0" << endl;
	saida << "                                  je Imprime_Int" << endl;
	saida << "                                  div ecx" << endl;
	saida << "                                  add edx,0x30" << endl;
	saida << "                                  cmp edx,0x30" << endl;
	saida << "                                  jl Numero_Inv" << endl;
	saida << "                                  cmp edx,0x39" << endl;
	saida << "                                  jg Numero_Inv" << endl;
	saida << "                                  mov [EBP-12+esi],DL" << endl;
	saida << "                                  inc esi" << endl;
	saida << "                                  jmp Neg_Loop" << endl;
	saida << "                  Zero: add eax,0x30" << endl;
	saida << "                        mov [EBP-12+esi],eax" << endl;
	saida << "                        inc esi" << endl;
	saida << "                        jmp Imprime_Int" << endl;
	saida << "                  Numero_Inv: mov eax,4" << endl;
	saida << "                              mov ebx,1" << endl;
	saida << "                              mov ecx,msg0" << endl;
	saida << "                              mov edx,17" << endl;
	saida << "                              int 80h" << endl;
	saida << "                              jmp Exit_Func" << endl;
	saida << "                  Imprime_Int: cmp ebx,1" << endl;
	saida << "                           je Insert_sign" << endl;
	saida << "                           mov byte [EBP-12+esi],0ah" << endl;
	saida << "                           mov edi,esi" << endl;
	saida << "                           mov eax,12" << endl;
	saida << "                           sub eax,esi" << endl;
	saida << "                           mov edi,eax" << endl;
	saida << "                           mov esi,eax" << endl;
	saida << "                           inc esi" << endl;
	saida << "                           Sys_Write: mov eax,4" << endl;
	saida << "                                      mov ebx,1" << endl;
	saida << "                                      mov ecx,EBP" << endl;
	saida << "										sub ecx,esi" << endl;
	saida << "                                      mov edx,1" << endl;
	saida << "                                      int 80h" << endl;
	saida << "                                      inc esi" << endl;
	saida << "                                      inc dword [EBP-16]" << endl;
	saida << "                                      cmp esi,13" << endl;
	saida << "                                      jne Sys_Write" << endl;
	saida << "                                      mov eax,4" << endl;
	saida << "                                      mov ebx,1" << endl;
	saida << "                                      mov ecx,EBP" << endl;
	saida << "                                      sub ecx,edi" << endl;
	saida << "                                      mov edx,1" << endl;
	saida << "                                      int 80h" << endl;
	saida << "                                      inc dword [EBP-16]" << endl;
	saida << "                                      jmp Exit_Func" << endl;
	saida << "                           Insert_sign: mov byte [EBP-12+esi],0x2D" << endl;
	saida << "                                        inc esi" << endl;
	saida << "                                        inc dword [EBP-16]" << endl;
	saida << "                                        mov ebx,0" << endl;
	saida << "                                        jmp Imprime_Int" << endl;
	saida << "                  Exit_Func: mov eax,[EBP-16]" << endl;
	saida << "                             pop edi" << endl;
	saida << "                             pop esi" << endl;
	saida << "                             pop edx" << endl;
	saida << "                             pop ecx" << endl;
	saida << "                             pop ebx" << endl;
	saida << "                             pop eax" << endl;
	saida << "                             leave" << endl;
	saida << "                             ret 4" << endl;

	//LeerString
	saida << "  LeerString: enter 4,0" << endl;
	saida << "              push ebx" << endl;
	saida << "              push ecx" << endl;
	saida << "              push edx" << endl;
	saida << "              push esi" << endl;
	saida << "              push edi" << endl;
	saida << "              mov edi,[EBP+8]" << endl;
	saida << "              mov dword [EBP-4],0" << endl;
	saida << "              mov esi,[EBP+12]" << endl;
	saida << "              Leitura: mov eax,3" << endl;
	saida << "                       mov ebx,0" << endl;
	saida << "                       mov ecx,edi" << endl;
	saida << "                       mov edx,1" << endl;
	saida << "                       int 80h" << endl;
	saida << "                       cmp byte [edi],0ah" << endl;
	saida << "                       je Fim_Leitura" << endl;
	saida << "                       add edi,1" << endl;
	saida << "                       inc dword [EBP-4]" << endl;
	saida << "                       cmp [EBP-4],esi" << endl;
	saida << "                       je Fim_Leitura" << endl;
	saida << "                       jmp Leitura" << endl;
	saida << "              Fim_Leitura: mov eax,[EBP-4]" << endl;
	saida << "                           pop edi" << endl;
	saida << "                           pop esi" << endl;
	saida << "                           pop edx" << endl;
	saida << "                           pop ecx" << endl;
	saida << "                           pop ebx" << endl;
	saida << "                           leave" << endl;
	saida << "                           ret 8" << endl;

	//EscreverString
	saida << "  EscreverString: enter 4,0" << endl;
	saida << "                  push ebx" << endl;
	saida << "                  push ecx" << endl;
	saida << "                  push edx" << endl;
	saida << "                  push esi" << endl;
	saida << "                  push edi" << endl;
	saida << "                  mov edi,[EBP+8]" << endl;
	saida << "                  mov dword [EBP-4],0" << endl;
	saida << "                  mov esi,[EBP+12]" << endl;
	saida << "                  Escrita: mov eax,4" << endl;
	saida << "                           mov ebx,0" << endl;
	saida << "                           mov ecx,edi" << endl;
	saida << "                           mov edx,1" << endl;
	saida << "                           int 80h" << endl;
	saida << "                           cmp byte [edi],0ah" << endl;
	saida << "                           je Fim_Escrita" << endl;
	saida << "                           add edi,1" << endl;
	saida << "                           inc dword [EBP-4]" << endl;
	saida << "                           cmp [EBP-4],esi" << endl;
	saida << "                           je Fim_Escrita" << endl;
	saida << "                           jmp Escrita" << endl;
	saida << "                  Fim_Escrita: mov eax,[EBP-4]" << endl;
	saida << "                               pop edi" << endl;
	saida << "                               pop esi" << endl;
	saida << "                               pop edx" << endl;
	saida << "                               pop ecx" << endl;
	saida << "                               pop ebx" << endl;
	saida << "                               leave" << endl;
	saida << "                               ret 8" << endl;

	//LeerHexa
	saida << "  LeerHexa: enter 21,0" << endl;
	saida << "            push ebx" << endl;
	saida << "            push ecx" << endl;
	saida << "            push edx" << endl;
	saida << "            push esi" << endl;
	saida << "            push edi" << endl;
	saida << "            mov dword [EBP-21],0" << endl;
	saida << "            mov dword [EBP-13],0" << endl;
	saida << "            Input_HEX: mov eax,3" << endl;
	saida << "                       mov ebx,0" << endl;
	saida << "                       mov ecx,EBP" << endl;
	saida << "                       sub ecx,9" << endl;
	saida << "                       mov edx,9" << endl;
	saida << "                       int 80h" << endl;
	saida << "            cmp byte [EBP-9],0x2D" << endl;
	saida << "            je Invalido_HEX" << endl;
	saida << "            mov edi,eax" << endl;
	saida << "            mov [EBP-21],eax" << endl;
	saida << "            sub edi,2" << endl;
	saida << "            mov esi,0" << endl;
	saida << "            xor edx,edx" << endl;
	saida << "            Verifica_Char: cmp byte [EBP-9+esi],0x30" << endl;
	saida << "                           jl Invalido_HEX" << endl;
	saida << "                           cmp byte [EBP-9+esi],0x39" << endl;
	saida << "                           jg Verifica_Char_HEX" << endl;
	saida << "                           xor ebx,ebx" << endl;
	saida << "                           mov BL,[EBP-9+esi]" << endl;
	saida << "                           sub ebx,0x30" << endl;
	saida << "                           jmp Convert_to_Num" << endl;
	saida << "            Verifica_Char_HEX: cmp byte [EBP-9+esi],0x41" << endl;
	saida << "            					 jl Invalido_HEX" << endl;
	saida << "            					 cmp byte [EBP-9+esi],0x46" << endl;
	saida << "            					 jg Invalido_HEX" << endl;
	saida << "            					 xor ebx,ebx" << endl;
	saida << "            					 mov BL,[EBP-9+esi]" << endl;
	saida << "            					 sub ebx,0x41" << endl;
	saida << "            					 add ebx,10" << endl;
	saida << "            Convert_to_Num: cmp edi,0" << endl;
	saida << "			  				  je Last_Digit" << endl;
	saida << "			  				  mov ecx,edi" << endl;
	saida << "			  				  mov eax,1" << endl;
	saida << "			  				  mov [EBP-17],ebx" << endl;
	saida << "			  				  mov ebx,16" << endl;
	saida << "			  				  Loop_Convert: mul ebx" << endl;
	saida << "			  				  				loop Loop_Convert" << endl;
	saida << "			  				  mov ebx,[EBP-17]" << endl;
	saida << "			  				  dec edi" << endl;
	saida << "			  				  mul ebx" << endl;
	saida << "			  				  add [EBP-13],eax" << endl;
	saida << "			  				  inc esi" << endl;
	saida << "			  				  jmp Verifica_Char" << endl;
	saida << "			  				  Last_Digit: add [EBP-13],ebx" << endl;
	saida << "			  				  jmp FIM_HEX" << endl;
	saida << "			  Invalido_HEX: mov eax,4" << endl;
	saida << "			  				mov ebx,1" << endl;
	saida << "			  				mov ecx,msg1" << endl;
	saida << "			  				mov edx,35" << endl;
	saida << "			  				int 80h" << endl;
	saida << "			  				jmp Input_HEX" << endl;
	saida << "			  FIM_HEX: mov eax,[EBP-13]" << endl;
	saida << "			  		   mov ebx,[EBP+8]" << endl;
	saida << "			  		   mov [ebx],eax" << endl;
	saida << "			  		   mov eax,[EBP-21]" << endl;
	saida << "			  		   pop edi" << endl;
	saida << "			  		   pop esi" << endl;
	saida << "			  		   pop edx" << endl;
	saida << "			  		   pop ecx" << endl;
	saida << "			  		   pop ebx" << endl;
	saida << "			  		   leave" << endl;
	saida << "			  		   ret 4" << endl;

	//EscreverHexa
	saida << "	EscreverHexa: enter 13,0" << endl;
	saida << "            	  push ebx" << endl;
	saida << "          	  push ecx" << endl;
	saida << "            	  push edx" << endl;
	saida << "                push esi" << endl;
	saida << "                push edi" << endl;
	saida << "                mov dword [EBP-13],0" << endl;
	saida << "                mov eax,[EBP+8]" << endl;
	saida << "                mov esi,-2" << endl;
	saida << "                xor ebx,ebx" << endl;
	saida << "                mov edi,16" << endl;
	saida << "                cmp eax,0" << endl;
	saida << "                jl Inv_HEX" << endl;
	saida << "                Div_Loop: cdq" << endl;
	saida << "               			div edi" << endl;
	saida << "            				cmp DL,10" << endl;
	saida << "            				jge ASCII_HEX" << endl;
	saida << "            				add DL,0x30" << endl;
	saida << "            				Store_HEX: mov byte [EBP+esi],DL" << endl;
	saida << "            						   mov BL,[EBP+esi]" << endl;
	saida << "            						   dec esi" << endl;
	saida << "            						   cmp eax,0" << endl;
	saida << "                           		   je Print_HEX" << endl;
	saida << "                           		   jmp Div_Loop" << endl;
	saida << "                ASCII_HEX: add DL,0x37" << endl;
	saida << "                           jmp Store_HEX" << endl;
	saida << "                Inv_HEX: mov eax,4" << endl;
	saida << "                         mov ebx,1" << endl;
	saida << "                         mov ecx,msg0" << endl;
	saida << "            			   mov edx,17" << endl;
	saida << "			  			   int 80h" << endl;
	saida << "			  			   jmp F_HEX" << endl;
	saida << "			  	  Print_HEX: mov byte [EBP-1],0ah" << endl;
	saida << "			  				 neg esi" << endl;
	saida << "			  				 dec esi" << endl;
	saida << "			  				 mov eax,4" << endl;
	saida << "			  				 mov ebx,1" << endl;
	saida << "			  				 mov ecx,EBP" << endl;
	saida << "			  				 sub ecx,esi" << endl;
	saida << "			  				 mov edx,esi" << endl;
	saida << "			  				 int 80h" << endl;
	saida << "			  				 mov [EBP-13],eax" << endl;
	saida << "			  	  F_HEX: mov eax,[EBP-13]" << endl;
	saida << "			  		     pop edi" << endl;
	saida << "			  		     pop esi" << endl;
	saida << "			  		     pop edx" << endl;
	saida << "			  		     pop ecx" << endl;
	saida << "			  		     pop ebx" << endl;
	saida << "			  		     leave" << endl;
	saida << "			  		     ret 4" << endl; 

	//LeerChar
	saida << "	LeerChar: enter 2,0" << endl;
	saida << "			  	push ebx" << endl;
	saida << "			  	push ecx" << endl;
	saida << "			  	push edx" << endl;
	saida << "			  	InputC: mov eax,3" << endl;
	saida << "			  		  	mov ebx,0" << endl;
	saida << "			  		  	mov ecx,EBP" << endl;
	saida << "			  		  	sub ecx,2" << endl;
	saida << "			  		  	mov edx,2" << endl;
	saida << "			  		  	int 80h" << endl;
	saida << "			  	cmp byte [EBP-2],0ah" << endl;
	saida << "			  	je Espaco" << endl;
	saida << "			  	mov CL,[EBP-2]" << endl;
	saida << "			  	mov edx,[EBP+8]" << endl;
	saida << "			  	mov [edx],CL" << endl;
	saida << "			  	jmp FIM_C" << endl;
	saida << "			  	Espaco: mov eax,4" << endl;
	saida << "					      	mov ebx,1" << endl;
	saida << "					      	mov ecx,msg2" << endl;
	saida << "					      	mov edx,20" << endl;
	saida << "					      	int 80h" << endl;
	saida << "					      	jmp InputC" << endl;
	saida << "			  	FIM_C: pop edx" << endl;
	saida << "					     	pop ecx" << endl;
	saida << "					     	pop ebx" << endl;
	saida << "					     	leave" << endl;
	saida << "					     	ret 4" << endl;

	//EscreverChar
	saida << "	EscreverChar: enter 4,0" << endl;
	saida << "				  	push ebx" << endl;
	saida << "				  	push ecx" << endl;
	saida << "				  	push edx" << endl;
	saida << "				  	mov dword [EBP-4],0" << endl;
	saida << "				  	mov byte [EBP-4],0ah" << endl;
	saida << "				  	mov eax,4" << endl;
	saida << "				  	mov ebx,1" << endl;
	saida << "				  	mov ecx,[EBP+8]" << endl;
	saida << "				  	mov edx,1" << endl;
	saida << "				  	int 80h" << endl;
	saida << "				  	mov eax,4" << endl;
	saida << "				  	mov ebx,1" << endl;
	saida << "				  	mov ecx,EBP" << endl;
	saida << "				  	sub ecx,4" << endl;
	saida << "				  	mov edx,1" << endl;
	saida << "				  	int 80h" << endl;
	saida << "				  	pop edx" << endl;
	saida << "				  	pop ecx" << endl;
	saida << "				  	pop ebx" << endl;
	saida << "				  	leave" << endl;
	saida << "				  	ret 4";

	//Fim da Impressão das rotinas
}


string InttoString (int Num) {
	ostringstream str;
	str << Num;
	return str.str();
}