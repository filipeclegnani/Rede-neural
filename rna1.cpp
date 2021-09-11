#include <stdio.h>
#include <stdlib.h>

int tamanhoVetorInt(int *i)
{
  int tamanho = 0;
  while (i[tamanho] != 0)
  {
    tamanho++;
  }
  return tamanho;
}

void transfereBuffersFloat(float *A, float *B, int sizeB){
  free(A);
  A = (float*)malloc(sizeof(float)*sizeB);
  for(int aux = 0; aux < sizeB; aux++){
    A[aux] = B[aux];
  }
}

typedef struct
{
  int quantidade;
  float *pesosEntrada;
  float saida;  // saida de todas as multiplicações
} NEURONIOS;

typedef struct
{
  int tamanho;
  NEURONIOS *neuronio;
} CAMADA;

float randomFloat()
{ // gera numero aleatório entre 0 e 1
  // pode ser 0 mas não pode ser 1
  return (float)(rand() % 1000) / 1000;
}

class RedeNeural
{
private:
  int quantidadeCamadas;
  float erro;               // sugerido 0.01
  float taxaDeAprendizagem; // sugerido 0.001
  CAMADA *camadas;
  NEURONIOS *neuroniosSaidas;
  int qtdEntradas;
  int qtdSaidas;

public:
  float *valoresEntradas;
  float *valoresSaidas;
  unsigned int seed;

  RedeNeural(int entradas, int *internas, int saida)  {
    // entradas é a quantidade de valores entrados
    // internas é um vetor de inteiros com a quantidade de neuronios de cada camada interna
    // saida é a quantidade de neuronios de saida
    int aux = 0, aux2 = 0, aux3 = 0;
    taxaDeAprendizagem = 0.001;
    erro = 0.01;
    quantidadeCamadas = tamanhoVetorInt(internas) + 2; // entrada + internas + saida
    valoresEntradas = (float *)malloc(entradas * sizeof(float));
    valoresSaidas = (float *)malloc(saida * sizeof(float));
    qtdEntradas = entradas;
    qtdSaidas = saida;

    while (aux < entradas)
    { // zera os valores de entrada
      valoresEntradas[aux] = 0;
      aux++;
    }
    aux = 0;
    while (aux < entradas)
    { // zera os valores de saida
      valoresSaidas[aux] = 0;
      aux++;
    }

    // aloca as camadas
    camadas = (CAMADA *)malloc((quantidadeCamadas - 2) * sizeof(CAMADA));

    // aloca os pesos
    for (aux = 0; aux < quantidadeCamadas - 2; aux++)
    {
      // definir quantidades de neuronios
      camadas[aux].tamanho = internas[aux];
      // alocar neuronios
      camadas[aux].neuronio = (NEURONIOS *)malloc(internas[aux] * sizeof(NEURONIOS));

      for (aux2 = 0; aux2 < internas[aux]; aux2++)
      {
        // aloca os pesos

        if (aux == 0)
        {
          camadas[aux].neuronio[aux2].quantidade = entradas;
        }
        else
        {
          camadas[aux].neuronio[aux2].quantidade = camadas[aux - 1].tamanho;
        }

        // zera os pesos
        camadas[aux].neuronio[aux2].pesosEntrada = (float *)malloc(sizeof(float) * camadas[aux].neuronio[aux2].quantidade);
        // R: X � a quantidade de neuronios da camada anterior
        for (aux3 = 0; aux3 < camadas[aux].neuronio[aux2].quantidade; aux3++)
        {
          camadas[aux].neuronio[aux2].pesosEntrada[aux3] = 0.0f;
        }
      }
    }
    // alocar pra saida
    neuroniosSaidas = (NEURONIOS *)malloc(saida * sizeof(NEURONIOS));
    for (aux = 0; aux < saida; aux++)
    {
      neuroniosSaidas[aux].quantidade = camadas[quantidadeCamadas - 3].tamanho;
      neuroniosSaidas[aux].pesosEntrada = (float *)malloc(neuroniosSaidas[aux].quantidade * sizeof(float));
      // zera pesos de saida
      for (aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++)
      {
        neuroniosSaidas[aux].pesosEntrada[aux2] = 0.0f;
      }
    }
  }

  void dealloc()
  { // atualizar
    for (int aux = 0; aux < quantidadeCamadas - 2; aux++)
    {
      for (int aux2 = 0; aux2 < camadas[aux].tamanho; aux2++)
      {
        free(camadas[aux].neuronio[aux2].pesosEntrada);
      }
      free(camadas[aux].neuronio);
    }
    free(camadas);
    free(valoresEntradas);
    free(valoresSaidas);
    return;
  }

  int tamanhoDaCamada(int camada)
  {
    return camadas[camada].tamanho;
  }

  float calculaNeuronio(int camadaX, int neuronioX, float *valores, int tamanho){
    float *saida = &camadas[camadaX].neuronio[neuronioX].saida;
    for(int i = 0; i < tamanho; i++){
      *saida += valores[i] * camadas[camadaX].neuronio[neuronioX].pesosEntrada[i];
    }
    return camadas[camadaX].neuronio[neuronioX].saida;
  }

  void printRede()
  {
    for (int camadaAtual = 0; camadaAtual < quantidadeCamadas - 2; camadaAtual++)
    {
      printf("C: %i T:%i\n", camadaAtual, this->tamanhoDaCamada(camadaAtual));
      for (int neuronio = 0; neuronio < this->tamanhoDaCamada(camadaAtual); neuronio++)
      {
        printf("\n\tN: %i Q: %i/P: ", neuronio, camadas[camadaAtual].neuronio[neuronio].quantidade);
        for (int peso = 0; peso < camadas[camadaAtual].neuronio[neuronio].quantidade; peso++)
        {
          printf("%.2f ", camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso]);
        }
      }
      printf("\n");
    }
    printf("saidas\n");
    printf("C: %i T:%i\n", quantidadeCamadas - 2, qtdSaidas);
    for (int aux = 0; aux < qtdSaidas; aux++)
    {
      printf("\n\tN: %i Q: %i/P: ", aux, neuroniosSaidas[aux].quantidade);
      for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++)
      {
        printf("%.2f ", neuroniosSaidas[aux].pesosEntrada[aux2]);
      }
    }
    printf("\n");
  }

  void randomCreate()
  {
    // percorre pesos
    // assina os pesos com um numero aleatório
    srand(seed);
    for (int camadaAtual = 0; camadaAtual < quantidadeCamadas - 2; camadaAtual++)
    {
      for (int neuronio = 0; neuronio < this->tamanhoDaCamada(camadaAtual); neuronio++)
      {
        for (int peso = 0; peso < camadas[camadaAtual].neuronio[neuronio].quantidade; peso++)
        {
          camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso] = randomFloat();
        }
      }
    }
    for (int aux = 0; aux < qtdSaidas; aux++)
    {
      for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++)
      {
        neuroniosSaidas[aux].pesosEntrada[aux2] = randomFloat();
      }
    }
  }

  void propagacao()  {
    float *bufferA, *bufferB;
    // pega as entradas
    // manda pra um buffer A
    transfereBuffersFloat(bufferA, this->valoresEntradas, this->qtdEntradas);
    // 	faz as multiplicaões e retorna para um buffer B
    //  move o buffer B para o A
    for(int i = 0; i < this->quantidadeCamadas-2;i++){

    }
    // repete
    // manda buffer B para saida
  }
};

int main(){ //essa parte é só para testar
  int x[] = {7, 2, 0};
  system("cls");
  RedeNeural rect(2, x, 5);
  rect.seed = 2;
  rect.randomCreate();
  rect.printRede();
  rect.dealloc();
  return 0;
}
