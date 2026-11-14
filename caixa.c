#include <stdio.h>
#include <string.h>
 
typedef enum
{
    CONTA_ENCERRADA,
    CONTA_ATIVA
} SituacaoConta;
 
typedef struct
{
    long numero;
    char titular[100];
    char cpf[15];
    double saldo;
    SituacaoConta situacao;
} Conta;
 

FILE *abrirArquivo(void)
{
    FILE *arquivo = fopen("contas.dat", "rb+");
    if (arquivo == NULL)
    {
        arquivo = fopen("contas.dat", "wb+");
    }
    return arquivo;
}
 

int buscarConta(FILE *arquivo, long numeroConta, Conta *conta)
{
    Conta lida;
 
    rewind(arquivo);
    while (fread(&lida, sizeof(Conta), 1, arquivo) == 1)
    {
        if (lida.numero == numeroConta)
        {
            *conta = lida;
            fseek(arquivo, -(long)sizeof(Conta), SEEK_CUR);
            return 1;
        }
    }
    return 0;
}
 

int atualizarConta(FILE *arquivo, const Conta *conta)
{
    Conta temp;
 
    if (buscarConta(arquivo, conta->numero, &temp) == 0)
    {
        return 0;
    }
   
    if (fwrite(conta, sizeof(Conta), 1, arquivo) != 1)
    {
        return 0;
    }
    fflush(arquivo);
    return 1;
}
 
void cadastrarConta(void)
{
    Conta nova, existente;
    FILE *arquivo;
 
    printf("Nome do titular: ");
    scanf(" %99[^\n]", nova.titular);
    printf("CPF: ");
    scanf(" %14s", nova.cpf);
    printf("Numero da conta: ");
    scanf("%ld", &nova.numero);
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
 
    if (buscarConta(arquivo, nova.numero, &existente) == 1)
    {
        printf("Ja existe uma conta com esse numero.\n");
        fclose(arquivo);
        return;
    }
 
    nova.saldo = 0;
    nova.situacao = CONTA_ATIVA;
 
    fseek(arquivo, 0, SEEK_END);
    fwrite(&nova, sizeof(Conta), 1, arquivo);
    fclose(arquivo);
 
    printf("Conta cadastrada com sucesso!\n");
}
 
void consultarConta(void)
{
    long numero;
    Conta conta;
    FILE *arquivo;
 
    printf("Numero da conta: ");
    scanf("%ld", &numero);
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
 
    if (buscarConta(arquivo, numero, &conta) == 0)
    {
        printf("Conta nao encontrada.\n");
    }
    else
    {
        printf("Conta: %ld\n", conta.numero);
        printf("Titular: %s\n", conta.titular);
        printf("CPF: %s\n", conta.cpf);
        printf("Saldo: R$ %.2f\n", conta.saldo);
        if (conta.situacao == CONTA_ATIVA)
        {
            printf("Situacao: ATIVA\n");
        }
        else
        {
            printf("Situacao: ENCERRADA\n");
        }
    }
 
    fclose(arquivo);
}
 
int depositar(long numeroConta, double valor)
{
    Conta conta;
    FILE *arquivo;
 
    if (valor <= 0)
    {
        printf("O valor deve ser maior que zero.\n");
        return 0;
    }
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }
 
    if (buscarConta(arquivo, numeroConta, &conta) == 0)
    {
        printf("Conta %ld nao encontrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }
    if (conta.situacao == CONTA_ENCERRADA)
    {
        printf("Conta %ld esta encerrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }
 
    conta.saldo = conta.saldo + valor;
    atualizarConta(arquivo, &conta);
    fclose(arquivo);
    return 1;
}
 
int sacar(long numeroConta, double valor)
{
    Conta conta;
    FILE *arquivo;
 
    if (valor <= 0)
    {
        printf("O valor deve ser maior que zero.\n");
        return 0;
    }
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }
 
    if (buscarConta(arquivo, numeroConta, &conta) == 0)
    {
        printf("Conta %ld nao encontrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }
    if (conta.situacao == CONTA_ENCERRADA)
    {
        printf("Conta %ld esta encerrada.\n", numeroConta);
        fclose(arquivo);
        return 0;
    }
    if (conta.saldo < valor)
    {
        printf("Saldo insuficiente.\n");
        fclose(arquivo);
        return 0;
    }
 
    conta.saldo = conta.saldo - valor;
    atualizarConta(arquivo, &conta);
    fclose(arquivo);
    return 1;
}
 

int transferir(long contaOrigem, long contaDestino, double valor)
{
    Conta origem, destino;
    FILE *arquivo;
 
    if (contaOrigem == contaDestino)
    {
        printf("As contas de origem e destino devem ser diferentes.\n");
        return 0;
    }
    if (valor <= 0)
    {
        printf("O valor deve ser maior que zero.\n");
        return 0;
    }
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }
 
   
    if (buscarConta(arquivo, contaOrigem, &origem) == 0)
    {
        printf("Conta de origem nao encontrada.\n");
        fclose(arquivo);
        return 0;
    }
    if (buscarConta(arquivo, contaDestino, &destino) == 0)
    {
        printf("Conta de destino nao encontrada.\n");
        fclose(arquivo);
        return 0;
    }
    fclose(arquivo);
 
    if (origem.situacao == CONTA_ENCERRADA || destino.situacao == CONTA_ENCERRADA)
    {
        printf("Uma das contas esta encerrada.\n");
        return 0;
    }
    if (origem.saldo < valor)
    {
        printf("Saldo insuficiente na conta de origem.\n");
        return 0;
    }
 
   
    if (sacar(contaOrigem, valor) == 0)
    {
        return 0;
    }
    if (depositar(contaDestino, valor) == 0)
    {
        depositar(contaOrigem, valor);
        return 0;
    }
    return 1;
}
 
void listarContas(void)
{
    Conta conta;
    FILE *arquivo;
    int opcao, total = 0;
 
    printf("1 - Todas\n2 - Somente ativas\n3 - Somente encerradas\n");
    printf("Opcao: ");
    scanf("%d", &opcao);
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
 
    rewind(arquivo);
    while (fread(&conta, sizeof(Conta), 1, arquivo) == 1)
    {
        if (opcao == 2 && conta.situacao != CONTA_ATIVA)
        {
            continue;
        }
        if (opcao == 3 && conta.situacao != CONTA_ENCERRADA)
        {
            continue;
        }
 
        printf("Conta %ld | %s | CPF %s | Saldo R$ %.2f | ",
               conta.numero, conta.titular, conta.cpf, conta.saldo);
        if (conta.situacao == CONTA_ATIVA)
        {
            printf("ATIVA\n");
        }
        else
        {
            printf("ENCERRADA\n");
        }
        total++;
    }
 
    if (total == 0)
    {
        printf("Nenhuma conta encontrada.\n");
    }
 
    fclose(arquivo);
}
 

int encerrarConta(long numeroConta)
{
    Conta conta;
    FILE *arquivo;
 
    arquivo = abrirArquivo();
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }
 
    if (buscarConta(arquivo, numeroConta, &conta) == 0)
    {
        printf("Conta nao encontrada.\n");
        fclose(arquivo);
        return 0;
    }
    if (conta.situacao == CONTA_ENCERRADA)
    {
        printf("A conta ja esta encerrada.\n");
        fclose(arquivo);
        return 0;
    }
    if (conta.saldo != 0)
    {
        printf("So e possivel encerrar contas com saldo zero.\n");
        fclose(arquivo);
        return 0;
    }
 
    conta.situacao = CONTA_ENCERRADA;
    atualizarConta(arquivo, &conta);
    fclose(arquivo);
    return 1;
}
 
int main(void)
{
    int opcao;
    long numero, origem, destino;
    double valor;
 
    do
    {
        printf("\n===== CAIXA ELETRONICO =====\n");
        printf("1 - Cadastrar conta\n");
        printf("2 - Consultar conta\n");
        printf("3 - Realizar deposito\n");
        printf("4 - Realizar saque\n");
        printf("5 - Realizar transferencia\n");
        printf("6 - Listar contas\n");
        printf("7 - Encerrar conta\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
 
        if (opcao == 1)
        {
            cadastrarConta();
        }
        else if (opcao == 2)
        {
            consultarConta();
        }
        else if (opcao == 3)
        {
            printf("Numero da conta: ");
            scanf("%ld", &numero);
            printf("Valor do deposito: ");
            scanf("%lf", &valor);
            if (depositar(numero, valor) == 1)
            {
                printf("Deposito realizado com sucesso!\n");
            }
        }
        else if (opcao == 4)
        {
            printf("Numero da conta: ");
            scanf("%ld", &numero);
            printf("Valor do saque: ");
            scanf("%lf", &valor);
            if (sacar(numero, valor) == 1)
            {
                printf("Saque realizado com sucesso!\n");
            }
        }
        else if (opcao == 5)
        {
            printf("Conta de origem: ");
            scanf("%ld", &origem);
            printf("Conta de destino: ");
            scanf("%ld", &destino);
            printf("Valor da transferencia: ");
            scanf("%lf", &valor);
            if (transferir(origem, destino, valor) == 1)
            {
                printf("Transferencia realizada com sucesso!\n");
            }
        }
        else if (opcao == 6)
        {
            listarContas();
        }
        else if (opcao == 7)
        {
            printf("Numero da conta: ");
            scanf("%ld", &numero);
            if (encerrarConta(numero) == 1)
            {
                printf("Conta encerrada com sucesso!\n");
            }
        }
        else if (opcao != 0)
        {
            printf("Opcao invalida.\n");
        }
 
    } while (opcao != 0);
 
    printf("Ate logo!\n");
    return 0;
}
 
