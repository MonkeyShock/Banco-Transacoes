// ServicoConta.cpp

#include "ServicoConta.h"

using namespace std;
using namespace std::chrono;

/*
 * ServicoConta - Serviço responsável pelo gerenciamento de contas bancárias
 * Implementa as principais operações de gestão de contas como criação, consulta
 * e movimentações financeiras.
 */

 /*
  * Construtor que recebe o gerenciador de transações como dependência
  * permitindo o desacoplamento entre a gestão de contas e suas transações
  */
ServicoConta::ServicoConta(const shared_ptr<GerenciadorTransacoes>& gerTrans)
    : gerTrans_(gerTrans)
{
}

/*
 * Cria uma nova conta bancária com os dados fornecidos
 * @throws invalid_argument se já existir uma conta com o ID fornecido
 * Utiliza o padrão Factory Method da classe Conta para garantir a
 * unicidade do ID e inicialização adequada da conta
 */
shared_ptr<Conta> ServicoConta::criarConta(
    const string& id,
    const string& nomeTitular,
    const system_clock::time_point& dataAbertura
) {
    if (contas_.count(id))
        throw invalid_argument("ID de conta já existente: " + id);

    auto conta = Conta::Criar(id, nomeTitular, dataAbertura);
    contas_.emplace(id, conta);
    return conta;
}

/*
 * Recupera uma conta existente pelo seu ID
 * @throws invalid_argument se a conta não for encontrada
 */
shared_ptr<Conta> ServicoConta::obterConta(const string& id) const {
    auto it = contas_.find(id);
    if (it == contas_.end())
        throw invalid_argument("Conta não encontrada: " + id);
    return it->second;
}

/*
 * Obtém o saldo atual da conta na data de referência especificada
 * Primeiro efetiva todas as transações pendentes até a data de referência
 * para garantir que o saldo retornado reflita todas as operações agendadas
 */
decimal ServicoConta::obterSaldo(
    const string& id,
    const system_clock::time_point& dataReferencia
) {
    gerTrans_->efetivarAte(dataReferencia);
    auto conta = obterConta(id);
    return conta->getSaldo();
}

/*
 * Calcula o total de movimentações em um período específico
 * Considera apenas transações já efetivadas no período informado
 * Útil para análises de fluxo de caixa e extratos
 */
decimal ServicoConta::obterTotalizado(
    const string& id,
    const system_clock::time_point& inicio,
    const system_clock::time_point& fim
) {
    auto lista = gerTrans_->buscarPorPeriodo(
        id, inicio, fim, /*somenteEfetivadas=*/true
    );

    decimal soma = 0;
    for (const auto& tx : lista) {
        soma += tx->getValor();
    }
    return soma;
}
