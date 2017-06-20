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
void diretivas(istream&,ostream&,BSS*);
void instrucoes(istream&,ostream&);

int main(int argc, char *argv[]) {
    BSS *var_bss;
    //Abertura do arquivo
    var_bss = (BSS*)malloc(sizeof(BSS));
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
    diretivas(entradapreproc,saida,var_bss); //Imprimindo section .data e section .bss no arquivo de saida
    entradapreproc.clear(); //Retornando para o inicio do arquivo de entrada
    entradapreproc.seekg(0,ios::beg);
    cout << "Passou Diretivas!" << endl;
    saida.clear();
    instrucoes(entradapreproc,saida);

return 0; 
}

void Preprocessamento(istream &file, string nome_arq) {
EQU_List no; //Objeto da lista da diretiva EQU
string str, linha, strtemp;
string token, token1, token2, dir, linhaout, linhain, aux, aux2, aux3;
string rotulo, valorstr; //strings para abrir arquivo, guardar linhas, verificacoes de linhas e diretivas
unsigned int i, j, k, tam2, tam3; //Contadores e variaveis para tamanho de linha
int flagIF1 = 0, flagIF2 = 0;//Flag para verificar erros, se existe rotulo, flags para diretiva IF, flag para verificar se o programa esta na ordem correta
bool eh_rotulo = false; //Variavel para definir se pertence a lista da diretiva EQU
int posit = 0; //Inteiro para pegar posicao das diretivas IF, CONST e SPACE
size_t pos; //Necessario para funcao str.find()

nome_arq.erase(nome_arq.length()-4,4);
nome_arq += "_Preproc.asm";
fstream out (nome_arq.c_str(),ios::out);

if(out.is_open()) {
while(getline(file,linha)) {
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
            if(linha.at(i) == ':') {
                token += linha.at(i);
                k = i+1;
                break;
            }
            token += linha.at(i);
        }
        aux = no.busca_no(token,eh_rotulo);
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

void diretivas(istream &entrada, ostream &saida, BSS *var_bss) { //Procura pela SECTION DATA e Diretivas SPACE e CONST, gerando a section .data e section .bss
    string linha, var, dir, valor;
    bool flag_DATA = false, flag_SPACE = false; //Flag para indicar SECTION DATA e para indicar diretiva SPACE
    unsigned int i, k, j=0;
    cout << "Entrou Diretivas!" << endl;
    while(getline(entrada,linha)) {
        if(linha == "SECTION DATA") { //Se a linha do arquivo for igual a SECTION DATA, colocar flag_DATA em true e pegar proxima linha
            flag_DATA = true;
            saida << "section .data";
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
            while(linha.at(k) != ' ') { //Verificando diretiva (CONST ou SPACE)
                dir += linha.at(k);
                k++;
            }
            k++; //Indo para o proximo token
            cout << dir << endl;
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
                if(flag_SPACE == true)
                    var_bss = (BSS*)realloc(var_bss,(j+1)*sizeof(BSS)); //Alocando espaco
                cout << "j: " << j << endl;
                var_bss[j].variavel = var; //Salvando dados no elemento j do vetor
                var_bss[j].size = valor;
                flag_SPACE = true;
                j++; //Incrementando contador de elementos
                cout << "var: " << var << endl;
            }
        }
    }
    if(flag_SPACE == true) { //Se entrou na diretiva SPACE tera section .bss
        saida << endl;
        saida << "section .bss" << endl;
        for(i=0;i<=j-1;i++) { //Imprimindo no arquivo de saida os elementos definidos pela diretiva SPACE na section .bss
            saida << var_bss[i].variavel;
            cout << var_bss[i].variavel << endl;
            saida << " resd ";
            saida << var_bss[i].size;
            saida << endl;
       }
    }
    free(var_bss);
}

void instrucoes(istream &entrada,ostream &saida) { //Faz a traducao das instrucoes do assembly inventado
    string line, rotulo, inst, op1, op2;
    unsigned int i1,k1;
    bool flag_TEXT=false, flag_rotulo; //Flag para indicar que se esta na SECTION TEXT e flag para indicar que ha rotulo
    cout << "Entrou Intrucoes!" << endl;
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
            rotulo.clear();
            inst.clear();
            op1.clear();
            op2.clear();
            flag_rotulo = false;
            k1=0;
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
                    op1 += line.at(i1);
                }
                saida << "  add ebx," << '[' << op1 << ']' << endl; //Gravando codigo IA32 correspondente na saida
            } else if(inst == "SUB") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
                saida << "  sub ebx," << '[' << op1 << ']' << endl; //Gravando codigo IA32 correspondente na saida

            } else if(inst == "MUL") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
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

            } else if(inst == "DIV") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
                if(flag_rotulo == false) {
                    saida << "  push eax" << endl; //Gravando codigo IA32 correspondente na saida
                    saida << "  mov eax,ebx" << endl;
                    saida << "  idiv dword " <<  '[' << op1 << ']' << endl;
                    saida << "  mov ebx,eax" << endl;
                    saida << "  pop eax" << endl;
                } else {
                    saida << " push eax" << endl; //Gravando codigo IA32 correspondente na saida
                    saida << "      mov eax,ebx" << endl;
                    saida << "      idiv dword " <<  '[' << op1 << ']' << endl;
                    saida << "      mov ebx,eax" << endl;
                    saida << "      pop eax" << endl;
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
                    op1 += line.at(i1);
                }
                saida << "  mov ebx," << '[' << op1 << ']' << endl;

            } else if (inst == "STORE") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
                saida << "  mov " << '[' << op1 << ']' << ",ebx" << endl;

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

            } else if (inst == "INPUT") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
                if(flag_rotulo == false) {
                    saida << "  push " << op1 << endl;
                    saida << "  call LeerInteiro" << endl;
                } else {
                    saida << " push" << op1 << endl;
                    saida << "      call LeerInteiro" << endl;
                }

            } else if (inst == "OUTPUT") {
                for(i1=k1;i1<line.length();i1++) { //Pegando operando
                    op1 += line.at(i1);
                }
                if(flag_rotulo == false) {
                    saida << "  push " << op1 << endl;
                    saida << "  call EscreverInteiro" << endl;
                } else {
                    saida << " push" << op1 << endl;
                    saida << "      call EscreverInteiro" << endl;
                }
            }
            //Fim da traducao da instrucao
        }
    }
    //Impressao das rotinas
}