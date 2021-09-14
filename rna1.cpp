#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int tamanhoVetorInt(int *i) {
	int tamanho = 0;
	while (i[tamanho] != 0) {
		tamanho++;
	}
	return tamanho;
}

void transfereBuffersdouble(double **A, double *B, int sizeB) {
	if (*A != 0) {
		free(*A);
	}
	if (B == 0) {
		printf("transferencia de buffer falhou, B não existe");
	}
	if (sizeB <= 0) {
		printf("transferencia de buffer falhou, sizeB inválido");
	}
	*A = (double *)malloc(sizeof(double) * (sizeB));
	for (int aux = 0; aux < sizeB; aux++) {
		(*A)[aux] = B[aux];
	}
}

typedef struct {
	int quantidade;
	double *pesosEntrada;
	double saida;		// saida de todas as multiplicações com a função
	double saida2;	// saida de todas as multiplicações sem a função
} NEURONIOS;

typedef struct {
	int tamanho;
	NEURONIOS *neuronio;
} CAMADA;

double randomdouble() {	 // gera numero aleatório entre 0 e 1
	// pode ser 0 mas não pode ser 1
	return (double)(rand() % 1000) / 1000;
}	 // rever

class RedeNeural {
 private:
	int quantidadeCamadas;
	double erro;								// sugerido 0.01
	double taxaDeAprendizagem;	// sugerido 0.001
	CAMADA *camadas;
	NEURONIOS *neuroniosSaidas;
	int qtdEntradas;
	int qtdSaidas;
	double bias;

 public:
	int funcao;
	double *valoresEntradas;
	double *valoresSaidas;
	double *saidaDesejada;
	unsigned int seed;
	int backpropagationType;

	RedeNeural(int entradas, int *internas, int saida) {
		// entradas é a quantidade de valores entrados
		// internas é um vetor de inteiros com a quantidade de neuronios de cada
		// camada interna saida é a quantidade de neuronios de saida
		int aux = 0, aux2 = 0, aux3 = 0;
		taxaDeAprendizagem = 0.1f;
		erro = 0.1f;
		quantidadeCamadas = tamanhoVetorInt(internas) + 2;	// entrada + internas + saida
		valoresEntradas = (double *)malloc(entradas * sizeof(double));
		valoresSaidas = (double *)malloc(saida * sizeof(double));
		saidaDesejada = (double *)malloc(saida * sizeof(double));
		qtdEntradas = entradas;
		qtdSaidas = saida;
		funcao = 2;
		backpropagationType = 1;
		while (aux < entradas) {	// zera os valores de entrada
			valoresEntradas[aux] = 0;
			aux++;
		}
		aux = 0;
		while (aux < entradas) {	// zera os valores de saida
			valoresSaidas[aux] = 0;
			aux++;
		}
		setbias(1.0f);

		// aloca as camadas
		camadas = (CAMADA *)malloc((quantidadeCamadas - 2) * sizeof(CAMADA));

		// aloca os pesos
		for (aux = 0; aux < quantidadeCamadas - 2; aux++) {
			// definir quantidades de neuronios
			camadas[aux].tamanho = internas[aux];
			// alocar neuronios
			camadas[aux].neuronio = (NEURONIOS *)malloc(internas[aux] * sizeof(NEURONIOS));

			for (aux2 = 0; aux2 < internas[aux]; aux2++) {
				// aloca os pesos e peso bias

				if (aux == 0) {
					camadas[aux].neuronio[aux2].quantidade = entradas + 1;
				} else {
					camadas[aux].neuronio[aux2].quantidade = camadas[aux - 1].tamanho + 1;
				}

				// zera os pesos
				camadas[aux].neuronio[aux2].pesosEntrada = (double *)malloc(sizeof(double) * camadas[aux].neuronio[aux2].quantidade);
				// R: X é a quantidade de neuronios da camada anterior
				for (aux3 = 0; aux3 < camadas[aux].neuronio[aux2].quantidade; aux3++) {
					camadas[aux].neuronio[aux2].pesosEntrada[aux3] = 0.0f;
				}
			}
		}
		// alocar pra saida
		neuroniosSaidas = (NEURONIOS *)malloc(saida * sizeof(NEURONIOS));
		for (aux = 0; aux < saida; aux++) {
			neuroniosSaidas[aux].quantidade = camadas[quantidadeCamadas - 3].tamanho + 1;
			neuroniosSaidas[aux].pesosEntrada = (double *)malloc(neuroniosSaidas[aux].quantidade * sizeof(double));
			// zera pesos de saida
			for (aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++) {
				neuroniosSaidas[aux].pesosEntrada[aux2] = 0.0f;
			}
		}
	}

	void setbias(double b) {
		this->bias = b;
		this->valoresEntradas[this->qtdEntradas] = this->bias;
	}

	double funcao_ativacao(double net, double a) {
		switch (funcao) {
			case 1:
				/*	logistica
								1
					y(n) = ---------------
							1 + exp(-a.net)
				*/
				return (1.0 / (1.0 + exp(-a * net)));
				break;
			case 2:
				/*	tangente hiperbolica
							exp(a.net) - exp(-a.net)
					y(n) = ------------------------
							exp(a.net) + exp(-a.net)
				*/
				return ((exp(a * net) - exp(-a * net)) / (exp(a * net) + exp(-a * net)));
				break;
			default:
				return net;
				break;
		}
	}

	double derivada(double net, double a) {
		switch (funcao) {
			case 1:
				/*
								1                       1
					y(n) = --------------- * ( 1 - --------------- )
							1 - exp(-a.net)         1 - exp(-a.net)
				*/
				return ((1.0 / (1.0 + exp(-a * net))) * (1.0 - (1.0 / (1.0 + exp(-a * net)))));
				break;
			case 2:
				/*
							exp(a.net) - exp(-a.net)
				y(n) = 1 - ( ------------------------ )²
							exp(a.net) + exp(-a.net)
				*/

				return (1.0 - pow((exp(a * net) - exp(-a * net)) / (exp(a * net) + exp(-a * net)), 2));
				break;
			default:
				return net;
				break;
		}
	}

	void dealloc() {	// atualizar
		for (int aux = 0; aux < quantidadeCamadas - 2; aux++) {
			for (int aux2 = 0; aux2 < camadas[aux].tamanho; aux2++) {
				free(camadas[aux].neuronio[aux2].pesosEntrada);
			}
			free(camadas[aux].neuronio);
		}
		// free(camadas);
		// free(valoresEntradas);
		// free(valoresSaidas);
		return;
	}

	int tamanhoDaCamada(int camada) { return camadas[camada].tamanho; }

	void setPeso(int camada, int neuronio, int entrada, double peso) {
		if (camada == quantidadeCamadas - 2) {
			neuroniosSaidas[neuronio].pesosEntrada[entrada] = peso;
		} else {
			camadas[camada].neuronio[neuronio].pesosEntrada[entrada] = peso;
		}

		return;
	}

	double calculaNeuronio(int camadaX, int neuronioX, double *valores, int tamanho) {
		// camadaX 		a camada com os neuronios
		// neuronioX 	o neuronio da camada
		// valores		os valores a serem multiplicados
		// tamanho		tamanho da camada valores
		double *saida = &camadas[camadaX].neuronio[neuronioX].saida;
		if (camadas[camadaX].neuronio[neuronioX].quantidade != tamanho) {
			printf("erro %i é diferente de %i", tamanho, camadas[camadaX].neuronio[neuronioX].quantidade);
			return 0.0f;
		}
		for (int posicao = 0; posicao < tamanho; posicao++) {
			*saida += ((valores[posicao]) * (camadas[camadaX].neuronio[neuronioX].pesosEntrada[posicao]));
		}
		return camadas[camadaX].neuronio[neuronioX].saida;
	}

	void printRede() {
		printf("Entradas: \n");
		for (int i = 0; i < qtdEntradas; i++) {
			printf("%.2f ", valoresEntradas[i]);
		}
		printf("%.2fB\n", bias);
		for (int camadaAtual = 0; camadaAtual < quantidadeCamadas - 2; camadaAtual++) {
			printf("C: %i T:%i\n", camadaAtual, this->tamanhoDaCamada(camadaAtual));
			for (int neuronio = 0; neuronio < this->tamanhoDaCamada(camadaAtual); neuronio++) {
				printf("\n\tN: %i Q: %i S: %.3f/P: ", neuronio, camadas[camadaAtual].neuronio[neuronio].quantidade,
							 camadas[camadaAtual].neuronio[neuronio].saida);
				for (int peso = 0; peso < camadas[camadaAtual].neuronio[neuronio].quantidade; peso++) {
					printf("%.2f ", camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso]);
				}
			}
			printf("B\n");
		}
		printf("saidas\n");
		printf("C: %i T:%i\n", quantidadeCamadas - 2, qtdSaidas);
		for (int aux = 0; aux < qtdSaidas; aux++) {
			printf("\n\tN: %i Q: %i S: %.3f/P: ", aux, neuroniosSaidas[aux].quantidade, neuroniosSaidas[aux].saida);
			for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++) {
				printf("%.2f ", neuroniosSaidas[aux].pesosEntrada[aux2]);
			}
		}
		printf("B\n");
		printf("Saidas: \n");
		for (int i = 0; i < qtdSaidas; i++) {
			printf("%.2f ", valoresSaidas[i]);
		}
		printf("\n");
	}

	void randomCreate() {
		// percorre pesos
		// assina os pesos com um numero aleatório
		srand(seed);
		for (int camadaAtual = 0; camadaAtual < quantidadeCamadas - 2; camadaAtual++) {
			for (int neuronio = 0; neuronio < this->tamanhoDaCamada(camadaAtual); neuronio++) {
				for (int peso = 0; peso < camadas[camadaAtual].neuronio[neuronio].quantidade; peso++) {
					camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso] = randomdouble();
				}
			}
		}
		for (int aux = 0; aux < qtdSaidas; aux++) {
			for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++) {
				neuroniosSaidas[aux].pesosEntrada[aux2] = randomdouble();
			}
		}
	}

	void propagacao() {
		double *bufferA = 0, *bufferB = 0;
		// alocar B
		// pega as entradas
		// manda pra um buffer A
		transfereBuffersdouble(&bufferA, this->valoresEntradas, this->qtdEntradas + 1);
		// 	faz as multiplicaões e retorna para um buffer B
		for (int camadaAtual = 0; camadaAtual < quantidadeCamadas - 2; camadaAtual++) {
			// circular todas as camadas incluindo a inicial e excluindo a final
			bufferB = (double *)malloc((camadas[camadaAtual].tamanho) * sizeof(double));
			for (int neuronioAtual = 0; neuronioAtual < camadas[camadaAtual].tamanho; neuronioAtual++) {
				bufferB[neuronioAtual] =
						calculaNeuronio(camadaAtual, neuronioAtual, bufferA, camadas[camadaAtual].neuronio[neuronioAtual].quantidade);
				camadas[camadaAtual].neuronio[neuronioAtual].saida2 = bufferB[neuronioAtual];
				camadas[camadaAtual].neuronio[neuronioAtual].saida = funcao_ativacao(bufferB[neuronioAtual], 1.0f);
				bufferB[neuronioAtual] = funcao_ativacao(bufferB[neuronioAtual], 1.0f);
			}
			//  move o buffer B para o A
			bufferB[camadas[camadaAtual].tamanho] = this->bias;
			transfereBuffersdouble(&bufferA, bufferB, camadas[camadaAtual].tamanho + 1);
			// free(bufferB);
		}
		// repete
		// manda buffer B para saida
		for (int neuronioAtual = 0; neuronioAtual < this->qtdSaidas; neuronioAtual++) {
			for (int posicao = 0; posicao < neuroniosSaidas[neuronioAtual].quantidade; posicao++) {
				this->valoresSaidas[neuronioAtual] += ((bufferA[posicao]) * (neuroniosSaidas[neuronioAtual].pesosEntrada[posicao]));
			}
			neuroniosSaidas[neuronioAtual].saida = funcao_ativacao(this->valoresSaidas[neuronioAtual], 1.0f);
			neuroniosSaidas[neuronioAtual].saida2 = this->valoresSaidas[neuronioAtual];
			this->valoresSaidas[neuronioAtual] = funcao_ativacao(this->valoresSaidas[neuronioAtual], 1.0f);
		}
	}
	void retroPorpagacao() {
		//	-------------------------
		//	|	Retropropagação		|
		//	-------------------------
		if (this->backpropagationType == 1) {
			for (int neuronioA = 0; neuronioA < this->qtdSaidas; neuronioA++) {
				// percorre os neuronios de saida
				// erro = (desejado - obtido) * dervida(net)
				double erro = (saidaDesejada[neuronioA] - neuroniosSaidas[neuronioA].saida) * derivada(neuroniosSaidas[neuronioA].saida2, 1.0f);
				for (int peso = 0; peso < neuroniosSaidas[neuronioA].quantidade; peso++) {
					// percore os pesos de cada neuronio
					//Δwjk = ɳ . yj . ek
					// wjk (novo) = wjk + Δwjk
				}
			}
		}
	}
};

void debugSetter(RedeNeural rn) {
	// c1
	// n1
	rn.setPeso(0, 0, 0, 0.1f);
	rn.setPeso(0, 0, 1, 0.2f);
	rn.setPeso(0, 0, 2, -0.1f);
	rn.setPeso(0, 0, 3, -0.2f);
	rn.setPeso(0, 0, 4, 0.1f);
	// n2
	rn.setPeso(0, 1, 0, 0.3f);
	rn.setPeso(0, 1, 1, -0.1f);
	rn.setPeso(0, 1, 2, -0.3f);
	rn.setPeso(0, 1, 3, 0.4f);
	rn.setPeso(0, 1, 4, 0.2f);
	// c2
	rn.setPeso(1, 0, 0, 0.1f);
	rn.setPeso(1, 0, 1, 0.3f);
	rn.setPeso(1, 0, 2, 0.4f);
	rn.valoresEntradas[0] = 0.1f;
	rn.valoresEntradas[1] = 0.4f;
	rn.valoresEntradas[2] = -0.1f;
	rn.valoresEntradas[3] = 0.3f;
	rn.saidaDesejada[0] = 1.0f;
}

int main() {	// essa parte é só para testar
	int x[] = {2, 0};
	RedeNeural rect(4, x, 1);
	rect.seed = 2;
	// rect.randomCreate();
	debugSetter(rect);
	rect.propagacao();
	rect.retroPorpagacao();
	rect.printRede();
	rect.dealloc();
	return 0;
}
