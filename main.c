#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PRODUTOS 100
#define NOME_ARQUIVO "estoque.bin"
#define CAP_MAX_ALMOXARIFADO 1000
#define CAP_MAX_GONDOLA 300

typedef struct {
    int id;
    char nome[50];
    char lote[20];

    char validade[11];
    char dataEntrada[11];
    char dataGondola[11];
    char dataSaida[11];

    int qtdAlmoxarifado;
    int qtdGondola;
    int qtdVendida;
    int qtdAvariada;
    int qtdRecall;
    int qtdDevolvida;

    float precoCusto;
    float precoVenda;
} Produto;

void cadastrarProduto(Produto estoque[], int *total);
void menuConsultar(const Produto estoque[], int total);
void menuDarBaixa(Produto estoque[], int total);
void transferirParaGondola(Produto estoque[], int total);
void salvarArquivo(const Produto estoque[], int total);
int carregarArquivo(Produto estoque[]);

void exibirStatusAtual(Produto p);
void calcularEspacos(const Produto estoque[], int total);
void consultarPorLote(const Produto estoque[], int total);
void relatorioVencimentos(const Produto estoque[], int total);
int calcularDiasVencimento(const char *dataValidade);

int main() {
    Produto estoque[MAX_PRODUTOS];
    int totalProdutos = 0;
    int opcao;

    totalProdutos = carregarArquivo(estoque);

    do {

        printf("1. Cadastrar (Entrada no Almoxarifado)\n");
        printf("2. Consultar (Status, Espacos, Lote, Vencimentos)\n");
        printf("3. Movimentar (Transferir Almoxarifado -> Gondola)\n");
        printf("4. Dar Baixa (Vendas, Avarias, Recall, Devolucao)\n");
        printf("5. Sair e Salvar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1: cadastrarProduto(estoque, &totalProdutos); break;
            case 2: menuConsultar(estoque, totalProdutos); break;
            case 3: transferirParaGondola(estoque, totalProdutos); break;
            case 4: menuDarBaixa(estoque, totalProdutos); break;
            case 5:
                salvarArquivo(estoque, totalProdutos);
                printf("\nDados salvos. Encerrando...\n");
                break;
            default: printf("\nOpcao invalida!\n");
        }
    } while (opcao != 5);

    return 0;
}
void cadastrarProduto(Produto estoque[], int *total) {
    if (*total >= MAX_PRODUTOS) {
        printf("\nCapacidade maxima de registros atingida!\n");
        return;
    }

    Produto p = {0};
    p.id = *total + 1;

    printf("\n--- CADASTRAR NOVO PRODUTO ---\n");
    printf("Nome: ");
    fgets(p.nome, sizeof(p.nome), stdin); p.nome[strcspn(p.nome, "\n")] = '\0';

    printf("Lote: ");
    fgets(p.lote, sizeof(p.lote), stdin); p.lote[strcspn(p.lote, "\n")] = '\0';

    printf("Data de Validade (DD/MM/AAAA): ");
    fgets(p.validade, sizeof(p.validade), stdin); p.validade[strcspn(p.validade, "\n")] = '\0';

    printf("Data de Entrada (DD/MM/AAAA): ");
    fgets(p.dataEntrada, sizeof(p.dataEntrada), stdin); p.dataEntrada[strcspn(p.dataEntrada, "\n")] = '\0';

    printf("Quantidade que entra no Almoxarifado: ");
    scanf("%d", &p.qtdAlmoxarifado);

    printf("Preco de Custo: R$ ");
    scanf("%f", &p.precoCusto);

    strcpy(p.dataGondola, "Nao enviado");
    strcpy(p.dataSaida, "Sem saida");

    estoque[*total] = p;
    (*total)++;
    printf("\nProduto registrado no Almoxarifado com sucesso!\n");
}


void menuConsultar(const Produto estoque[], int total) {
    int op;
    printf("\n--- MENU DE CONSULTAS ---\n");
    printf("1. Status Atual Geral (Onde esta, Quantos tem, Quantos vendidos)\n");
    printf("2. Espacos Disponiveis (Almoxarifado e Gondola)\n");
    printf("3. Consultar por Lote (Busca em caso de Recall)\n");
    printf("4. Relatorio de Vencimentos e Alertas\n");
    printf("Escolha: ");
    scanf("%d", &op);
    getchar();

    if (op == 1) {
        for (int i = 0; i < total; i++) exibirStatusAtual(estoque[i]);
    } else if (op == 2) {
        calcularEspacos(estoque, total);
    } else if (op == 3) {
        consultarPorLote(estoque, total);
    } else if (op == 4) {
        relatorioVencimentos(estoque, total);
    } else {
        printf("Opcao invalida.\n");
    }
}

void exibirStatusAtual(Produto p) {
    printf("\nID: %d | Nome: %s | Lote: %s\n", p.id, p.nome, p.lote);
    printf("--> LOCALIZACAO E QUANTIDADES:\n");
    printf("  Almoxarifado: %d un.\n", p.qtdAlmoxarifado);
    printf("  Gondola:      %d un.\n", p.qtdGondola);
    printf("--> SAIDAS:\n");
    printf("  Vendidos: %d | Avarias: %d | Devolucao: %d | Recall: %d\n",
           p.qtdVendida, p.qtdAvariada, p.qtdDevolvida, p.qtdRecall);
    printf("--> MOVIMENTACOES (Datas):\n");
    printf("  Chegada: %s | Para Gondola: %s | Ultima Saida: %s\n",
           p.dataEntrada, p.dataGondola, p.dataSaida);

}

void calcularEspacos(const Produto estoque[], int total) {
    int usoAlmox = 0, usoGondola = 0;
    for (int i = 0; i < total; i++) {
        usoAlmox += estoque[i].qtdAlmoxarifado;
        usoGondola += estoque[i].qtdGondola;
    }
    printf("\n--- ESPACOS DISPONIVEIS ---\n");
    printf("ALMOXARIFADO: Ocupado %d / %d (Disponivel: %d)\n", usoAlmox, CAP_MAX_ALMOXARIFADO, CAP_MAX_ALMOXARIFADO - usoAlmox);
    printf("GONDOLAS:     Ocupado %d / %d (Disponivel: %d)\n", usoGondola, CAP_MAX_GONDOLA, CAP_MAX_GONDOLA - usoGondola);
}

void consultarPorLote(const Produto estoque[], int total) {
    char termo[20];
    int encontrou = 0;
    printf("\nDigite o Lote para busca (Recall): ");
    fgets(termo, sizeof(termo), stdin); termo[strcspn(termo, "\n")] = '\0';

    for (int i = 0; i < total; i++) {
        if (strstr(estoque[i].lote, termo) != NULL) {
            exibirStatusAtual(estoque[i]);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Lote '%s' nao encontrado.\n", termo);
}

void relatorioVencimentos(const Produto estoque[], int total) {
    printf("\n--- ALERTAS DE VENCIMENTO ---\n");
    for (int i = 0; i < total; i++) {

        if (estoque[i].qtdAlmoxarifado > 0 || estoque[i].qtdGondola > 0) {
            int dias = calcularDiasVencimento(estoque[i].validade);
            printf("Produto: %s (Lote: %s) - Vence em %d dias\n", estoque[i].nome, estoque[i].lote, dias);
            printf("Status: ");
            if (dias > 30) printf("Em dia - Sem preocupacao\n");
            else if (dias > 15) printf("Atencao - Priorizar a venda\n");
            else if (dias > 5)  printf("Alerta - Aplicar desconto\n");
            else if (dias >= 0) printf("Critico - Valor de custo\n");
            else printf("Vencido - Descartar\n");
            printf("-\n");
        }
    }
}

int calcularDiasVencimento(const char *dataValidade) {
    int dia, mes, ano;
    sscanf(dataValidade, "%d/%d/%d", &dia, &mes, &ano);

    struct tm vencimento = {0};
    vencimento.tm_mday = dia;
    vencimento.tm_mon = mes - 1;
    vencimento.tm_year = ano - 1900;

    time_t tempoVencimento = mktime(&vencimento);
    time_t tempoAtual = time(NULL);

    double diffSegundos = difftime(tempoVencimento, tempoAtual);
    return diffSegundos / (60 * 60 * 24);
}

void transferirParaGondola(Produto estoque[], int total) {
    int idBusca, qtd;
    printf("\nID do produto para transferir: ");
    scanf("%d", &idBusca);

    for (int i = 0; i < total; i++) {
        if (estoque[i].id == idBusca) {
            printf("Quantidade disponivel no Almoxarifado: %d\n", estoque[i].qtdAlmoxarifado);
            printf("Quantas unidades vao para a Gondola? ");
            scanf("%d", &qtd);
            getchar();

            if (qtd <= estoque[i].qtdAlmoxarifado && qtd > 0) {
                estoque[i].qtdAlmoxarifado -= qtd;
                estoque[i].qtdGondola += qtd;

                printf("Data da ida para Gondola (DD/MM/AAAA): ");
                fgets(estoque[i].dataGondola, sizeof(estoque[i].dataGondola), stdin);
                estoque[i].dataGondola[strcspn(estoque[i].dataGondola, "\n")] = '\0';

                if (estoque[i].precoVenda == 0) {
                    printf("Defina o Preco de Venda: R$ ");
                    scanf("%f", &estoque[i].precoVenda);
                }
                printf("Transferencia concluida!\n");
            } else {
                printf("Quantidade invalida!\n");
            }
            return;
        }
    }
    printf("Produto nao encontrado.\n");
}

void menuDarBaixa(Produto estoque[], int total) {
    int idBusca, opBaixa, local, qtd;
    char data[11];

    printf("\nID do produto para dar baixa: ");
    scanf("%d", &idBusca);

    for (int i = 0; i < total; i++) {
        if (estoque[i].id == idBusca) {
            printf("\nMotivo da Baixa:\n");
            printf("1. Venda\n2. Avaria\n3. Recall\n4. Devolucao (Fornecedor)\nEscolha: ");
            scanf("%d", &opBaixa);

            printf("Retirar de onde? (1 - Almoxarifado / 2 - Gondola): ");
            scanf("%d", &local);

            printf("Quantidade: ");
            scanf("%d", &qtd);
            getchar();

            int *origem = (local == 1) ? &estoque[i].qtdAlmoxarifado : &estoque[i].qtdGondola;

            if (qtd > *origem) {
                printf("Erro: Quantidade insuficiente no local escolhido.\n");
                return;
            }

            *origem -= qtd;
            if (opBaixa == 1) estoque[i].qtdVendida += qtd;
            else if (opBaixa == 2) estoque[i].qtdAvariada += qtd;
            else if (opBaixa == 3) estoque[i].qtdRecall += qtd;
            else if (opBaixa == 4) estoque[i].qtdDevolvida += qtd;

            printf("Data da Baixa (DD/MM/AAAA): ");
            fgets(estoque[i].dataSaida, sizeof(estoque[i].dataSaida), stdin);
            estoque[i].dataSaida[strcspn(estoque[i].dataSaida, "\n")] = '\0';

            printf("Baixa registrada com sucesso!\n");
            return;
        }
    }
    printf("Produto nao encontrado.\n");
}

void salvarArquivo(const Produto estoque[], int total) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "wb");
    if (arquivo != NULL) {
        fwrite(&total, sizeof(int), 1, arquivo);
        fwrite(estoque, sizeof(Produto), total, arquivo);
        fclose(arquivo);
    }
}

int carregarArquivo(Produto estoque[]) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "rb");
    int total = 0;
    if (arquivo != NULL) {
        fread(&total, sizeof(int), 1, arquivo);
        fread(estoque, sizeof(Produto), total, arquivo);
        fclose(arquivo);
    }
    return total;
}

