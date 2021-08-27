
#include "funcionarios.h"
#include <stdlib.h>
#include <limits.h>

#include "particoes.h"

void classificacao_interna(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc)
{
    rewind(arq); //posiciona cursor no inicio do arquivo

    int reg = 0;

    while (reg != nFunc)
    {
        //le o arquivo e coloca no vetor
        TFunc *v[M];
        int i = 0;
        while (!feof(arq))
        {
            fseek(arq, (reg) * tamanho_registro(), SEEK_SET);
            v[i] = le_funcionario(arq);
            imprime_funcionario(v[i]);
            //     imprime_funcionario(v[i]);
            i++;
            reg++;
            if(i>=M) break;
        }
        printf("%d - %d \n", i, M);
        //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
        /*if (i != M) {
            M = i;
            printf("entrou - 4 \n");
        }*/
        printf("%d - %d \n", i, M);
        //faz ordenacao
        for (int j = 1; j < M; j++)
        {
            printf("entrou - 5 \n");
            TFunc *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->cod > f->cod))
            {
                printf("entrou while - 5 \n");
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao
        char *nome_particao = nome_arquivos_saida->nome;
        nome_arquivos_saida = nome_arquivos_saida->prox;
        printf("\n%s\n", nome_particao);
        FILE *p;
        printf("entrou - 6 \n");
        if ((p = fopen(nome_particao, "wb+")) == NULL)
        {
            printf("Erro criar arquivo de saida\n");
        }
        else
        {
            for (int i = 0; i < M; i++)
            {
                fseek(p, (i) * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[i], p);
                imprime_funcionario(v[i]);
                printf("entrou - 7 \n");
            }
            fclose(p);
        }
        for(int jj = 0; jj<M; jj++)
            free(v[jj]);
    }
}

int menorChave(TFunc *array[], int size)
{
    int posMenor, i, posNotNULL;
    char posNull = 'N';
    TFunc *menor;

    for(i = 0; i < size; i++)
    {
        if(array[i] == NULL)
        {
            posNull = 'S'; // caso exista uma posição null, diga que tem.
        }
        else
        {
            posNotNULL = i; // caso tenha posição diga, salva a posição que não é null
        }
    }
    if(posNull == 'N')
    {
        for(i = 0; i < size; i++)
        {
            printf("======Novo Array: %d -- %s \n", array[i]->cod, array[i]->nome);
            if(i == 0)
            {
                menor = array[0];
                posMenor = 0;
            }
            else if (menor->cod > array[i]->cod)
            {
                menor = array[i];
                posMenor = i;
            }
        }
        printf("_____________________________________________________________________________\n");
    }
    else
    {
        return posNotNULL;
    }

    return posMenor;
}

void salva_ArqSaida(FILE *saida, int indice, TFunc *item)
{
    fseek(saida, (indice)*tamanho_registro(), SEEK_SET);
    salva_funcionario(item, saida);
}

void selecao_natural(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc, int n)
{

    rewind(arq);
    TFunc *array[M], *reservatorio[M], *menorRegistro, *proxRegistroEntrada;
    int count = 0;
    int posArqEntrada = 0;
    int posMenorArray = 0;
    int posArqSaida = 0;
    int posReservatorio = 0;

    /* LER M REGISTROS DO ARQUIVO PARA A MEMÓRIA */
    if(arq != NULL)
    {
        while(count < M)
        {
            fseek(arq, (posArqEntrada)*tamanho_registro(), SEEK_SET);
            array[count] = le_funcionario(arq);
            printf("====Inicia no Array: %d -- %s \n", array[count]->cod, array[count]->nome);
            count ++;
            posArqEntrada ++;
            /* CERTO */
        }
        printf("_____________________________________________________________________________\n");
        /* GRAVAR O REGISTRO DE MENOR CHAVE NA PARTIÇÃO DE SAÍDA */
        char *nome_particao = nome_arquivos_saida->nome;
        nome_arquivos_saida = nome_arquivos_saida->prox;

        printf("======Abrindo novo arquivo de saida=====\n");
        FILE *arquivoSaida;
        arquivoSaida = fopen(nome_particao, "wb+");

        /* CASO AINDA EXISTA ESPAÇO LIVRE NO RESERVATÓRIO */
        do
        {
            /* SELECIONAR NO ARRAY EM MEMÓRIA, O REGISTRO R COM MENOR CHAVE */
            posMenorArray = menorChave(array, M);
            menorRegistro = array[posMenorArray];

            /* CERTO */
            if (arquivoSaida != NULL)
            {
                printf("======Salva menor registro no Arq Saida: %d -- %s \n", menorRegistro->cod, menorRegistro->nome);
                salva_ArqSaida(arquivoSaida, posArqSaida, menorRegistro);
                posArqSaida++;
            }
            /* CERTO */

            /* SUBSTITUIR NO ARRAY EM MEMÓRIA O MENOR REGISTRO PELO PRÓXIMO DO ARQUIVO DE ENTRADA */
            fseek(arq, (posArqEntrada)*tamanho_registro(), SEEK_SET);
            proxRegistroEntrada = le_funcionario(arq);
            array[posMenorArray] = proxRegistroEntrada;
            posArqEntrada ++;
            /* CERTO */

            /* CASO A CHAVE DO PROXIMO REGISTRO DE ENTRADA SEJA MENOR DO QUE O MENOR REGISTRO
             * GRAVA NO RESERVATÓRIO E SUBSTITUI NO ARRAY DE MEMÓRIA O MENOR REGISTRO PELO PROXIMO REGISTRO DE ENTRADA
             */
            if(feof(arq))
            {
                posMenorArray = menorChave(array, M);
                menorRegistro = array[posMenorArray];
                printf("======Salva menor registro no Arq Saida: %d -- %s \n", menorRegistro->cod, menorRegistro->nome);
                salva_ArqSaida(arquivoSaida, posArqSaida, menorRegistro);
                break;
            }
            if(proxRegistroEntrada->cod < menorRegistro->cod)
            {
                /* GRAVA NO RESERVATÓRIO */
                if(posArqEntrada < nFunc)
                {

                    reservatorio[posReservatorio] = proxRegistroEntrada;
                    posReservatorio++;

                    /* SUBSTITUIR NO ARRAY EM MEMÓRIA O MENOR REGISTRO PELO PROXIMO REGISTRO DO ARQUIVO DE ENTRADA */
                    posArqEntrada++;
                    fseek(arq, (posArqEntrada)*tamanho_registro(), SEEK_SET);
                    proxRegistroEntrada = le_funcionario(arq);
                    array[posMenorArray] = proxRegistroEntrada;
                    // array[posMenorArray] = proxRegistroEntrada;
                }
                else
                {
                    reservatorio[posReservatorio] = proxRegistroEntrada;
                    array[posMenorArray] = NULL;

                    posMenorArray = menorChave(array, M);
                    menorRegistro = array[posMenorArray];
                    if (arquivoSaida != NULL)
                    {
                        salva_ArqSaida(arquivoSaida, posArqSaida, menorRegistro);
                        posArqSaida++;
                    }

                    posReservatorio++;
                }
            }

            if(posReservatorio >= M)
            {
                /* FECHAR A PARTIÇÃO DE SAÍDA */
                fclose(arquivoSaida);

                /* COPIAR OS REGISTROS DO RESERVATÓRIO PARA O ARRAY EM MEMÓRIA */
                for(int i = 0; i < posReservatorio; i ++)
                {
                    array[i] = reservatorio[i];
                }

                /* ESVAZIAR O RESERVATÓRIO */
                for(int r = 0; r < M; r++)
                {
                    reservatorio[r] = NULL;
                }
                posReservatorio = 0;

                printf("======Abrindo novo arquivo de saida=====\n");
                /* ABRIR NOVO ARQUIVO DE SAÍDA */
                char *nome_particao = nome_arquivos_saida->nome;
                nome_arquivos_saida = nome_arquivos_saida->prox;

                FILE *arquivoSaida;
                arquivoSaida = fopen(nome_particao, "wb+");
                posArqSaida = 0;
            }
        }
        while(posReservatorio < M);
    }
}
