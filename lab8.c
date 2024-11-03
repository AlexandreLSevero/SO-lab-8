#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAMANHO_PAGINA 4096    
#define NUM_FRAMES 16          
#define NUM_PAGINAS 32         
#define NUM_PROCESSOS 2        

// struct para representar um frame físico
typedef struct {
    int processo_id;           
    int pagina_virtual;        
    bool ocupado;              
} Frame;

// struct para representar a memória física
typedef struct {
    Frame frames[NUM_FRAMES];
} MemoriaFisica;

// struct para representar a tabela de páginas de um processo
typedef struct {
    int mapeamento[NUM_PAGINAS]; 
} TabelaPaginas;

// struct para representar um processo
typedef struct {
    int id;                      
    TabelaPaginas tabela_paginas; 
    int acessos[5];              
} Processo;

// Função para inicializar a memória física, marcando todos os frames como livres
void iniciarMemoriaFisica(MemoriaFisica *memoria) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        memoria->frames[i].ocupado = false;
        memoria->frames[i].processo_id = -1;
        memoria->frames[i].pagina_virtual = -1;
    }
}

// Função para inicializar a tabela de páginas de um processo
void iniciarTabelaPaginas(TabelaPaginas *tabela){
    for (int i = 0; i < NUM_PAGINAS; i++) {
        tabela->mapeamento[i] = -1;
    }
}

// Função para inicializar um processo e definir seus endereços de acesso
void iniciarProcesso(Processo *processo, int id, int acessos[]) {
    processo->id = id;
    iniciarTabelaPaginas(&processo->tabela_paginas);
    for (int i = 0; i < 5; i++) {
        processo->acessos[i] = acessos[i];
    }
}

// Função para alocar uma página virtual a um frame físico para um processo
int alocarPagina(MemoriaFisica *memoria, Processo *processo, int pagina_virtual) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (!memoria->frames[i].ocupado) {  // Encontra um frame livre
            memoria->frames[i].ocupado = true;
            memoria->frames[i].processo_id = processo->id;
            memoria->frames[i].pagina_virtual = pagina_virtual;
            processo->tabela_paginas.mapeamento[pagina_virtual] = i;  // Mapeia a página virtual para o frame físico
            printf("Alocando página virtual %d do processo %d para frame físico %d\n", pagina_virtual, processo->id, i);
            return i;
        }
    }
    printf("Page fault: Não há frames livres para alocação!\n");
    return -1;  // Nenhum frame livre
}

// Função para traduzir um endereço virtual para um endereço físico
int traduzirEndereco(Processo *processo, MemoriaFisica *memoria, int endereco_virtual) {
    int pagina_virtual = endereco_virtual / TAMANHO_PAGINA;
    int deslocamento = endereco_virtual % TAMANHO_PAGINA;

    int frame_fisico = processo->tabela_paginas.mapeamento[pagina_virtual];
    if (frame_fisico == -1) {  // Page fault: página não está mapeada
        frame_fisico = alocarPagina(memoria, processo, pagina_virtual);
        if (frame_fisico == -1) return -1; // Erro de alocação
    }

    // Calcula o endereço físico usando o frame físico e o deslocamento
    int endereco_fisico = (frame_fisico * TAMANHO_PAGINA) + deslocamento;
    return endereco_fisico;
}


int main() {
    MemoriaFisica memoria;
    iniciarMemoriaFisica(&memoria);

    // Inicializando processos e definindo endereços virtuais que tentarão acessar
    int acessos_processo_1[5] = {0, 4096, 8192, 12288, 16384};
    int acessos_processo_2[5] = {20480, 24576, 28672, 32768, 36864};

    Processo processo_1, processo_2;
    iniciarProcesso(&processo_1, 1, acessos_processo_1);
    iniciarProcesso(&processo_2, 2, acessos_processo_2);

    // Simulando tradução de endereços virtuais para físicos para o processo 1
    printf("\nTradução de endereços para o Processo 1:\n");
    for (int i = 0; i < 5; i++) {
        int endereco_virtual = processo_1.acessos[i];
        printf("Processo %d acessando endereço virtual %d\n", processo_1.id, endereco_virtual);
        int endereco_fisico = traduzirEndereco(&processo_1, &memoria, endereco_virtual);
        if (endereco_fisico != -1) {
            printf("Endereço físico: %d\n", endereco_fisico);
        } else {
            printf("Page fault: Endereço virtual %d não pode ser mapeado\n", endereco_virtual);
        }
    }

    // Simulando tradução de endereços virtuais para físicos para o processo 2
    printf("\nTradução de endereços para o Processo 2:\n");
    for (int i = 0; i < 5; i++) {
        int endereco_virtual = processo_2.acessos[i];
        printf("Processo %d acessando endereço virtual %d\n", processo_2.id, endereco_virtual);
        int endereco_fisico = traduzirEndereco(&processo_2, &memoria, endereco_virtual);
        if (endereco_fisico != -1) {
            printf("Endereço físico: %d\n", endereco_fisico);
        } else {
            printf("Page fault: Endereço virtual %d não pode ser mapeado\n", endereco_virtual);
        }
    }

    return 0;
}
