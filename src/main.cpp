#include <time.h>

#include "redeneural.hpp"

typedef struct {
	bool erro;
	double* entradas;
	double* saidas;
	int qtdEntradas;
	int qtdSaidas;
} FILEDATA;

bool confereheader(char* string) {
	if (string[0] == 'R')
		if (string[1] == 'N')
			if (string[2] == 'T') return true;
	return false;
}

FILEDATA carregaArquivo(char* nome) {
	FILE* fp = fopen(nome, "r+");
	FILEDATA saida;
	char linha[50];
	saida.erro = false;
	if (fp == NULL) {
		printf("error arquivo não encontrado");
		saida.erro = true;
		return saida;
	}
	saida.qtdEntradas = 96000;
	saida.qtdSaidas = 1;
	saida.saidas = (double*)malloc((saida.qtdSaidas + 1) * sizeof(double));
	saida.entradas = (double*)malloc((saida.qtdEntradas + 1) * sizeof(double));

	fscanf(fp, "%50[^\n]s", linha);
	fscanf(fp, "%*c");
	if (!confereheader(linha)) {
		printf("Erro arquivo invalido: %s\n", nome);
		saida.erro = true;
		return saida;
	}

	for (int i = 0; i < saida.qtdSaidas; i++) {
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%lf", &(saida.saidas[i]));
	}
	fscanf(fp, "%*c");
	for (int i = 0; i < saida.qtdEntradas; i++) {
		fscanf(fp, "%50[^\n]s", linha);
		fscanf(fp, "%*c");
		sscanf(linha, "%lf", &(saida.entradas[i]));
	}
	return saida;
}

void assinaEntradas(RedeNeural rn, FILEDATA fd) {
	for (int i = 0; i < fd.qtdEntradas; i++) {
		double a = fd.entradas[i];
		a = rn.valoresEntradas[i];
		rn.valoresEntradas[i] = fd.entradas[i];
	}
	for (int i = 0; i < fd.qtdSaidas; i++) {
		rn.saidaDesejada[i] = fd.saidas[i];
	}
	return;
}

void calculaErros() {}

int main() {
	unsigned long int x[] = {10, 11, 0};
	unsigned long int entrada = 96000;
	unsigned long int saida = 1;
	FILEDATA data[100];
	char nome[50];
	int epocaInicio = 0;
	const int quantidadeDeEpocas = 6000;
	FILE* fconfig = fopen("src/config.cfg", "r");

	if (fconfig != NULL) {
		fscanf(fconfig, "%i", &epocaInicio);
		fclose(fconfig);
	}

	// carrega arquivo
	for (int i = 1; i <= 100; i++) {
		sprintf(nome, "src/amostras/Amostra%i.rnt", i);
		data[i - 1] = carregaArquivo(nome);
		if (data[i - 1].erro) {
			return -1;
		}
	}

	// treina rede
	RedeNeural rna(entrada, x, saida);
	rna.randomCreate();
	char arqEntrada[30];
	sprintf(arqEntrada, "src/epocas/Epoca%04i.rna", epocaInicio);
	rna.carregaRNA(arqEntrada);
	rna.taxaDeAprendizagem = 0.4;
	printf("função %i\n", rna.funcao);

	for (int epoca = epocaInicio; epoca < quantidadeDeEpocas; epoca++) {
		printf("epoca: %i de %i\n", epoca, quantidadeDeEpocas);
		unsigned long int tInicio = time(NULL);
		for (int i = 0; i < 100; i++) {
			// printf("amostra:  %i\n", i);
			assinaEntradas(rna, data[i]);
			rna.propagacao();
			// printf("\tretropropaga a saida: %.4f de %.4f\n", rna.valoresSaidas[0], rna.saidaDesejada[0]);
			if (rna.valoresSaidas[0] != rna.valoresSaidas[0]) {
				printf("!!!erro not a number!!!\n");
				return -1;
			}
			rna.retroPorpagacao();
		}
		char epocaArq[30];
		sprintf(epocaArq, "src/epocas/Epoca%04i.rna", epoca);
		if (epoca % 10 == 0) {
			rna.gravaRNA(epocaArq);
			fconfig = fopen("src/config.cfg", "w+");
			fprintf(fconfig, "%i", epoca);
			fflush(fconfig);
			fclose(fconfig);
		}
		unsigned long int tFinal = time(NULL);
		printf("\ttempo percorrido: %lis\n", (tFinal - tInicio));
		printf("\ttempo estimado: %lis\n", (tFinal - tInicio) * (quantidadeDeEpocas - epoca));
		char arrayS[30];
		time_t estimado = (time_t)((int)((tFinal - tInicio) * (quantidadeDeEpocas - epoca)) + (int)(tFinal));
		strftime(arrayS, 30, "%H:%M:%S", localtime(&estimado));

		printf("\tmomento estimado: %s\n", arrayS);
	}

	printf("finalizada\n");
	// rna.carregaRNA();
	return 0;
}