#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

#define TAM 256

typedef struct no{
    unsigned char caracter;
    int frequencia;
    struct no *esq, *dir, *proximo;
}No;

typedef struct{
    No *inicio;
    int tam;
}Lista;

//----------- parte 1: tabela de frequência ----------------------

//Esta função conta a frequência dos caracteres presente no texto a ser convertido
void contaFrequencia(unsigned char texto[], unsigned int tab[]){
    int i = 0;

    for(i = 0; i < TAM; i++)
        tab[i] = 0;

    i = 0;
    while(texto[i] != '\0'){
        tab[texto[i]]++;
        i++;
    }
}

//----------- parte 2: Lista Encadeada Ordenada ----------------------

void criar_lista(Lista *lista){
    lista->inicio = NULL;
    lista->tam = 0;
}

void inserir_ordenado(Lista *lista, No *no){
    No *aux;
    if(lista->inicio == NULL){
        lista->inicio = no;
    }
    else if(no->frequencia < lista->inicio->frequencia){
        no->proximo = lista->inicio;
        lista->inicio = no;
    }
    else{
        aux = lista->inicio;
        while(aux->proximo && aux->proximo->frequencia <= no->frequencia)
            aux = aux->proximo;
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
    lista->tam++;
}

void preencher_lista(unsigned int tab[], Lista *lista){
    int i;
    No *novo;
    for(i = 0; i < TAM; i++){
        if(tab[i] > 0){
            novo = malloc(sizeof(No));
            if(novo){
                novo->caracter = i;
                novo->frequencia = tab[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->proximo = NULL;

                inserir_ordenado(lista, novo);
            }
            else{
                printf("\tERRO ao alocar memoria em preencher_lista!\n");
                break;
            }
        }
    }
}

/*
void imprimir_lista(Lista *lista){
    No *aux = lista->inicio;

    printf("\n\tLista ordenada: Tamanho: %d\n", lista->tam);
    while(aux){
        printf("\tCaracter: %c Frequência: %d\n", aux->caracter, aux->frequencia);
        aux = aux->proximo;
    }
}
*/

//------------- parte 3: Montar a Árvore de Huffman ----------------------

No* remove_no_inicio(Lista *lista){
    No *aux = NULL;

    if(lista->inicio){
        aux = lista->inicio;
        lista->inicio = aux->proximo;
        aux->proximo = NULL;
        lista->tam--;
    }

    return aux;
}

No* montar_arvore(Lista *lista){
    No *primeiro, *segundo, *novo;
    while(lista->tam > 1){
        primeiro = remove_no_inicio(lista);
        segundo = remove_no_inicio(lista);
        novo = malloc(sizeof(No));

        if(novo){
            novo->caracter = '+';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->proximo = NULL;

            inserir_ordenado(lista, novo);
        }
        else{
            printf("\n\tERRO ao alocar memoria em montar_arvore!\n");
            break;
        }
    }
    return lista->inicio;
}

/*
void imprimir_arvore(No *raiz, int tam){
    if(raiz->esq == NULL && raiz->dir == NULL)
        printf("\tFolha: %c\tAltura: %d\n", raiz->caracter, tam);
    else{
        imprimir_arvore(raiz->esq, tam + 1);
        imprimir_arvore(raiz->dir, tam + 1);
    }
}
*/

//-------------- parte 4: Montar o dicionário ---------------------

int altura_arvore(No *raiz){
    int esq, dir;

    if(raiz == NULL)
        return -1;
    else{
        esq = altura_arvore(raiz->esq) + 1;
        dir = altura_arvore(raiz->dir) + 1;

        if(esq > dir)
            return esq;
        else
            return dir;
    }
}

char** aloca_dicionario(int colunas){
    char **dicionario;
    int i;

    dicionario = malloc(sizeof(char*) * TAM);

    for(i = 0; i < TAM; i++)
        dicionario[i] = calloc(colunas, sizeof(char));

    return dicionario;
}

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas){
    char esquerda[colunas], direita[colunas];

    if(raiz->esq == NULL && raiz->dir == NULL)
        strcpy(dicionario[raiz->caracter], caminho);
    else{
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->dir, direita, colunas);
    }
}

/*
void imprime_dicionario(char **dicionario){
    int i;

    printf("\n\tDicionario:\n");
    for(i = 0; i < TAM; i++){
        if(strlen(dicionario[i]) > 0)
            printf("\t%3d: %s\n", i, dicionario[i]);
    }
}
*/

//-------------- parte 5: Codificar ----------------------------

int calcula_tamanho_string(char **dicionario, unsigned char *texto){
    int i = 0, tam = 0;
    while(texto[i] != '\0'){
        tam = tam + strlen(dicionario[texto[i]]);
        i++;
    }
    return tam + 1;
}

char* codificar(char **dicionario, unsigned char *texto){

    int i = 0, tam = calcula_tamanho_string(dicionario, texto);
    char *codigo = calloc(tam, sizeof(char));

    while(texto[i] != '\0'){
        strcat(codigo, dicionario[texto[i]]);
        i++;
    }
    return codigo;
}

//-------------- parte 6: Decodificar --------------------------
char* decodificar(unsigned char texto[], No *raiz){
    int i = 0;
    No *aux = raiz;
    char temp[2];
    char *decodificado = calloc(strlen(texto), sizeof(char));

    while(texto[i] != '\0'){
        if(texto[i] == '0')
            aux = aux->esq;
        else
            aux = aux->dir;

        if(aux->esq == NULL && aux->dir == NULL){
            temp[0] = aux->caracter;
            temp[1] = '\0';
            strcat(decodificado, temp);
            aux = raiz;
        }

        i++;
    }
    return decodificado;
}

unsigned char convert_byte(char *vet, int n, char *comprimido){
    unsigned char rtno = 0, i = 0;
    int j = 0;

    while(i<n){
        rtno += pow(2, i) * (vet[(n-1)-i] - 48);
        i++;
        /*if(i==7){
            n = n-i;
            i = 0;
            comprimido[j] = rtno;
            j++;
        }*/
    }

    printf("\n%c\n", rtno);
    return rtno;
}

void convert(char ch, char *vet){
    int i = 7;

    while(i>= 0){
        if(ch & 1)
            vet[i] = '1';
        else
            vet[i] = '0';

        i--;
        ch >>= 1;
    }
}

void testaDescomprime(char comprimido[]){
    int n = strlen(comprimido);
    int i = 0;
    char aux[9];
    printf("\n");
    while(i<n){
        convert(comprimido[i], aux);
        printf("%s ", aux);
        i++;
    }
}

/*
void descomprime(char comprimido[]){
    FILE *f_compr = fopen(comprimido, "r");
    if(!f_compr){
        printf("\nNão foi possível abrir o arquivo %s", comprimido);
        return;
    }
    char ch, vet[9];
    while(!feof(comprimido)){
        fscanf(f_compr, "%c", &ch);
        convert(ch, vet);

    }
}
*/

int main() {

    unsigned char texto[] = "Bora mané, isso aqui é corinthians";
    unsigned int tabela_frequencia[TAM];
    Lista lista;
    No *arvore;
    int colunas;
    char **dicionario;
    char *codificado, *decodificado;

    setlocale(LC_ALL, "Portuguese");

    //----------- parte 1: tabela de frequência ---------------
    //inicializa_tabela_com_zero(tabela_frequencia);
    contaFrequencia(texto, tabela_frequencia);
    //imprime_tab_frequencia(tabela_frequencia);

    //----------- parte 2: Lista Encadeada Ordenada -----------
    criar_lista(&lista);
    preencher_lista(tabela_frequencia, &lista);
    //imprimir_lista(&lista);

    //----------- parte 3: Montar a Árvore de Huffman ---------
    arvore = montar_arvore(&lista);
    printf("\n\tÁrvore de Huffman\n");
    //imprimir_arvore(arvore, 0);

    //----------- parte 4: Montar o dicionário ----------------
    colunas = altura_arvore(arvore) + 1;
    dicionario = aloca_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);
    //imprime_dicionario(dicionario);

    //----------- parte 5: Codificar ---------------------------
    codificado = codificar(dicionario, texto);
    printf("\n\tTexto codificado: %s\n", codificado);

    //----------- parte 6: Decodificar -------------------------
    decodificado = decodificar(codificado, arvore);
    printf("\n\tTexto decodificado: %s\n", decodificado);

    //unsigned char textoCodificado;
    int n = strlen(codificado);
    char *comprimido;
    unsigned char a = convert_byte("01111111", 8, comprimido);
    unsigned char b = convert_byte("00100000", 8, comprimido);
    unsigned char c = convert_byte("10110010", 8, comprimido);
    char vet[9];
    convert(a, vet);
    printf("\n%8s", vet);
    convert(b, vet);
    printf("%8s", vet);
    convert(c, vet);
    printf("%8s", vet);
    //printf("\ncomprimido: %s", comprimido);
    //testaDescomprime(comprimido);

    return 0;
}

void comprime(char *codificado, int n, char *comprimido){
    char vet[9];
    strcnpy(vet, codificado, 8);
    vet[9] = '\0';

}