#include <math.h>
#include <stdio.h>

/*
	autores:
		Dara Julia Southier
		Filipe Casaletti Legnani
		Julia Kelly Prestes de Oliveira Da Silva
		Paulo Ricardo Savaris dos Santos
	data: 16/09/2021
*/

unsigned long long int tamanhoVetorInt(unsigned long long int *i) {
	unsigned long long int tamanho = 0;
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

double randomDouble() {	 // gera numero aleatório entre -1 e 1
	// pode ser 0 mas não pode ser 1
	return (double)((rand() % 2000) - 1000) / 1000;
}
/*
	Classe criada com a finalidade de facilitar a criação de uma rede neural fazendo todos
	os calculos de alocamento, desalocamento, propagação e retropropagação automaticamento
*/
class RedeNeural {
 private:
	int quantidadeCamadas;			 // salvar
	CAMADA *camadas;						 // salvar
	NEURONIOS *neuroniosSaidas;	 // salvar
	int qtdEntradas;						 // salvar
	int qtdSaidas;							 // salvar
	double bias;								 // salvar

 public:
	int funcao;	 // salvar
	double *valoresEntradas;
	double *valoresSaidas;
	double *saidaDesejada;
	unsigned int seed;
	int backpropagationType;		// salvar
	double taxaDeAprendizagem;	// sugerido 0.001	//salvar

	/*
		construtor da classe RedeNeural.
		params:
		entradas - usado para definir a quantidade de entradas na rede.
		internas - vetor terminado em 0 que deve conter a quantidade
		de neuronios que cada camada deve conter.
		saida - quantidade de neuronios que a camada de saida deve ter.
	*/

	RedeNeural(unsigned long entradas, unsigned long *internas, unsigned long saida) {
		// entradas é a quantidade de valores entrados
		// internas é um vetor de inteiros com a quantidade de neuronios de cada
		// camada interna saida é a quantidade de neuronios de saida
		unsigned long int aux = 0;
		int aux2 = 0, aux3 = 0;
		taxaDeAprendizagem = 0.1f;
		quantidadeCamadas = tamanhoVetorInt((unsigned long long *)internas) + 2;	// entrada + internas + saida
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
		while (aux < saida) {	 // zera os valores de saida
			valoresSaidas[aux] = 0;
			aux++;
		}
		setbias(1.0f);

		// aloca as camadas
		camadas = (CAMADA *)malloc((quantidadeCamadas - 2) * sizeof(CAMADA));

		// aloca os pesos
		for (int camadaAt = 0; camadaAt < quantidadeCamadas - 2; camadaAt++) {
			// definir quantidades de neuronios
			camadas[camadaAt].tamanho = internas[camadaAt];
			// alocar neuronios
			camadas[camadaAt].neuronio = (NEURONIOS *)malloc(internas[camadaAt] * sizeof(NEURONIOS));

			for (int neuronioAt = 0; neuronioAt < internas[camadaAt]; neuronioAt++) {
				// aloca os pesos e peso bias

				if (camadaAt == 0) {
					camadas[camadaAt].neuronio[neuronioAt].quantidade = entradas + 1;
				} else {
					camadas[camadaAt].neuronio[neuronioAt].quantidade = camadas[camadaAt - 1].tamanho + 1;
				}

				// zera os pesos
				camadas[camadaAt].neuronio[neuronioAt].pesosEntrada =
						(double *)malloc(sizeof(double) * camadas[camadaAt].neuronio[neuronioAt].quantidade);
				// R: X é a quantidade de neuronios da camada anterior
				for (int pesoAt = 0; pesoAt < camadas[camadaAt].neuronio[neuronioAt].quantidade; pesoAt++) {
					camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[pesoAt] = 0.0f;
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
				if (camadas[aux].neuronio[aux2].pesosEntrada) {
					free(camadas[aux].neuronio[aux2].pesosEntrada);
				}
			}
			if (camadas[aux].neuronio) {
				free(camadas[aux].neuronio);
			}
		}
		if (camadas) {
			free(camadas);
		}
		if (valoresEntradas) {
			free(valoresEntradas);
		}
		if (valoresSaidas) {
			free(valoresSaidas);
		}
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
				printf("\tN: %i Q: %i S: %.3f/P: ", neuronio, camadas[camadaAtual].neuronio[neuronio].quantidade,
							 camadas[camadaAtual].neuronio[neuronio].saida);
				for (int peso = 0; peso < camadas[camadaAtual].neuronio[neuronio].quantidade; peso++) {
					printf("%.4f ", camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso]);
				}
				printf("B\n");
			}
		}
		printf("saidas\n");
		printf("C: %i T:%i\n", quantidadeCamadas - 2, qtdSaidas);
		for (int aux = 0; aux < qtdSaidas; aux++) {
			printf("\n\tN: %i Q: %i S: %.3f/P: ", aux, neuroniosSaidas[aux].quantidade, neuroniosSaidas[aux].saida);
			for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++) {
				printf("%.4f ", neuroniosSaidas[aux].pesosEntrada[aux2]);
			}
			printf("B\n");
		}
		printf("Saidas: \n");
		for (int i = 0; i < qtdSaidas; i++) {
			printf("%.4f ", valoresSaidas[i]);
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
					camadas[camadaAtual].neuronio[neuronio].pesosEntrada[peso] = randomDouble();
				}
			}
		}
		for (int aux = 0; aux < qtdSaidas; aux++) {
			for (int aux2 = 0; aux2 < neuroniosSaidas[aux].quantidade; aux2++) {
				neuroniosSaidas[aux].pesosEntrada[aux2] = randomDouble();
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
			bufferB = (double *)malloc(((camadas[camadaAtual].tamanho) + 1) * sizeof(double));	// não aloca o suficiente
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
			free(bufferB);
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
			double somaErro = 0.0f;
			double erroSomado = 0.0f;
			for (int neuronioA = 0; neuronioA < this->qtdSaidas; neuronioA++) {
				// percorre os neuronios de saida
				// erro = (desejado - obtido) * dervida(net)
				double erro = (saidaDesejada[neuronioA] - neuroniosSaidas[neuronioA].saida) * derivada(neuroniosSaidas[neuronioA].saida2, 1.0f);
				somaErro += erro;
				for (int peso = 0; peso < neuroniosSaidas[neuronioA].quantidade; peso++) {
					// percore os pesos de cada neuronio
					// Dwjk = ɳ . yj . ek
					// wjk (novo) = wjk + Dwjk
					double deltaPeso = 0.0f;
					double pesoNovo = 0.0f;
					if (peso < neuroniosSaidas[neuronioA].quantidade - 1) {
						deltaPeso = taxaDeAprendizagem * erro * camadas[0].neuronio[peso].saida;	// ok
					} else {
						deltaPeso = taxaDeAprendizagem * erro * bias;	 // ok
					}
					pesoNovo = deltaPeso + neuroniosSaidas[neuronioA].pesosEntrada[peso];
					neuroniosSaidas[neuronioA].pesosEntrada[peso] = pesoNovo;
				}
			}
			erroSomado = somaErro;
			somaErro = 0.0f;
			for (int camadaAt = quantidadeCamadas - 3; camadaAt >= 0; camadaAt--) {
				// percorre cada camada de tras pra frente
				for (int neuronioAt = 0; neuronioAt < camadas[camadaAt].tamanho; neuronioAt++) {
					// percorre os neuronios da camada
					// é a primeira camada antes da saida?
					bool antesSaida = (camadaAt == (quantidadeCamadas - 3));
					for (int neuronioAn = 0; antesSaida ? neuronioAn < qtdSaidas : neuronioAn < camadas[camadaAt + 1].tamanho; neuronioAn++) {
						// percorre a camada anterior a do erro
						// erro =  ( ƩErro . pesoSaidaRecalculado ) . f’(saida(sem função))
						double erro = 0.0f;
						if (antesSaida) {
							erro = (erroSomado * neuroniosSaidas[neuronioAn].pesosEntrada[neuronioAt]) *
										 derivada(camadas[camadaAt].neuronio[neuronioAt].saida2, 1.0f);
						} else {
							erro = (erroSomado * camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[neuronioAt]) *
										 derivada(camadas[camadaAt].neuronio[neuronioAt].saida2, 1.0f);
						}
						somaErro += erro;
						for (int peso = 0; peso < camadas[camadaAt].neuronio[neuronioAt].quantidade; peso++) {
							// percore os pesos de cada neuronio
							// Dwjk = ɳ . yj . ek
							// wjk (novo) = wjk + Dwjk
							double deltaPeso = 0.0f;
							double pesoNovo = 0.0f;
							if (peso < camadas[camadaAt].neuronio[neuronioAt].quantidade - 1) {
								deltaPeso = taxaDeAprendizagem * erro * camadas[camadaAt].neuronio[neuronioAt].saida;	 // ok
							} else {
								deltaPeso = taxaDeAprendizagem * erro * bias;	 // ok
							}
							pesoNovo = deltaPeso + camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[peso];
							camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[peso] = pesoNovo;
						}
					}
				}
				erroSomado = somaErro;
				somaErro = 0.0f;
			}
		}
	}

	/*
		==========================
		|  Cabeçalho de arquivo  |
		==========================
		quantidade de camadas
		qtdEntradas
		qtdSaidas
		bias
		funcao
		backpropagationType
		taxaDeAprendizagem
		camadas[x].tamanho
		camadas[x].neuronio[y].quantidade
		camadas[x].neuronio[y].pesosEntrada[z]
	*/
	void gravaRNA(char *nomeArquivo) {
		FILE *fp = fopen(nomeArquivo, "w+");
		fprintf(fp, "RNAFile\n");
		fprintf(fp, "%i\n", quantidadeCamadas);
		fprintf(fp, "%i\n", qtdEntradas);
		fprintf(fp, "%i\n", qtdSaidas);
		fprintf(fp, "%.2f\n", bias);
		fprintf(fp, "%i\n", funcao);
		fprintf(fp, "%i\n", backpropagationType);
		fprintf(fp, "%.6f\n", taxaDeAprendizagem);
		for (int camadaAt = 0; camadaAt < quantidadeCamadas - 2; camadaAt++) {
			fprintf(fp, "%i\n", camadas[camadaAt].tamanho);
			for (int neuronioAt = 0; neuronioAt < camadas[camadaAt].tamanho; neuronioAt++) {
				fprintf(fp, "%i\n", camadas[camadaAt].neuronio[neuronioAt].quantidade);
				for (int pesos = 0; pesos < camadas[camadaAt].neuronio[neuronioAt].quantidade; pesos++) {
					fprintf(fp, "%.10f\n", camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[pesos]);
				}
			}
		}
		// fprintf(fp, "%i\n", qtdSaidas);
		for (int neuronioAt = 0; neuronioAt < qtdSaidas; neuronioAt++) {
			fprintf(fp, "%i\n", neuroniosSaidas[neuronioAt].quantidade);
			for (int pesoAt = 0; pesoAt < neuroniosSaidas[neuronioAt].quantidade; pesoAt++) {
				fprintf(fp, "%.10f\n", neuroniosSaidas[neuronioAt].pesosEntrada[pesoAt]);
			}
		}
		fprintf(fp, "ERNAFile");
		fflush(fp);
		fclose(fp);
	}

	bool confereheader(char *string) {
		if (string[0] == 'R')
			if (string[1] == 'N')
				if (string[2] == 'A')
					if (string[3] == 'F')
						if (string[4] == 'i')
							if (string[5] == 'l')
								if (string[6] == 'e') return true;
		return false;
	}

	void carregaRNA(char *nomeArquivo) {
		FILE *fp = fopen(nomeArquivo, "r+");
		this->dealloc();
		if (fp == NULL) {
			printf("Erro ao carregar arquivo");
			return;
		}
		char linha[50];
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		if (!confereheader(linha)) {
			printf("Erro arquivo invalido\n");
			return;
		}
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%i", &quantidadeCamadas);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%i", &qtdEntradas);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%i", &qtdSaidas);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%lf", &bias);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%i", &funcao);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%i", &backpropagationType);
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%lf", &taxaDeAprendizagem);

		camadas = (CAMADA *)malloc((quantidadeCamadas - 2) * sizeof(CAMADA));

		for (int camadaAt = 0; camadaAt < quantidadeCamadas - 2; camadaAt++) {
			// circula cada camada
			fscanf(fp, "%50[^\n]s", linha);
			fscanf(fp, "%*c");
			sscanf(linha, "%i", &camadas[camadaAt].tamanho);
			camadas[camadaAt].neuronio = (NEURONIOS *)malloc(camadas[camadaAt].tamanho * sizeof(NEURONIOS));
			for (int neuronioAt = 0; neuronioAt < camadas[camadaAt].tamanho; neuronioAt++) {
				fscanf(fp, "%50[^\n]s", linha);
				fscanf(fp, "%*c");
				sscanf(linha, "%i", &camadas[camadaAt].neuronio[neuronioAt].quantidade);

				// zera os pesos
				camadas[camadaAt].neuronio[neuronioAt].pesosEntrada =
						(double *)malloc(sizeof(double) * camadas[camadaAt].neuronio[neuronioAt].quantidade);
				for (int pesoAt = 0; pesoAt < camadas[camadaAt].neuronio[neuronioAt].quantidade; pesoAt++) {
					fscanf(fp, "%50[^\n]s", linha);
					fscanf(fp, "%*c");
					sscanf(linha, "%lf", &camadas[camadaAt].neuronio[neuronioAt].pesosEntrada[pesoAt]);
				}
			}
		}
		// alocar pra saida
		neuroniosSaidas = (NEURONIOS *)malloc(qtdSaidas * sizeof(NEURONIOS));
		for (int neuronioAt = 0; neuronioAt < qtdSaidas; neuronioAt++) {
			fscanf(fp, "%50[^\n]s", linha);
			fscanf(fp, "%*c");
			neuroniosSaidas[neuronioAt].quantidade = camadas[quantidadeCamadas - 3].tamanho + 1;
			neuroniosSaidas[neuronioAt].pesosEntrada = (double *)malloc(neuroniosSaidas[neuronioAt].quantidade * sizeof(double));
			// zera pesos de saida
			for (int pesoAt = 0; pesoAt < neuroniosSaidas[neuronioAt].quantidade; pesoAt++) {
				fscanf(fp, "%50[^\n]s", linha);
				fscanf(fp, "%*c");
				sscanf(linha, "%lf", &neuroniosSaidas[neuronioAt].pesosEntrada[pesoAt]);
			}
		}
		//
		fclose(fp);
	}
};
