// ServicoConta.cpp

#include "ServicoConta.h"

using namespace std;
using namespace std::chrono;

/*
 * ServicoConta - Servi�o respons�vel pelo gerenciamento de contas banc�rias
 * Implementa as principais opera��es de gest�o de contas como cria��o, consulta
 * e movimenta��es financeiras.
 */

 /*
  * Construtor que recebe o gerenciador de transa��es como depend�ncia
  * permitindo o desacoplamento entre a gest�o de contas e suas transa��es
  */
ServicoConta::ServicoConta(const shared_ptr<GerenciadorTransacoes>& gerTrans)
    : gerTrans_(gerTrans)
{
}

/*
 * Cria uma nova conta banc�ria com os dados fornecidos
 * @throws invalid_argument se j� existir uma conta com o ID fornecido
 * Utiliza o padr�o Factory Method da classe Conta para garantir a
 * unicidade do ID e inicializa��o adequada da conta
 */
shared_ptr<Conta> ServicoConta::criarConta(
    const string& id,
    const string& nomeTitular,
    const system_clock::time_point& dataAbertura
) {
    if (contas_.count(id))
        throw invalid_argument("ID de conta j� existente: " + id);

    auto conta = Conta::Criar(id, nomeTitular, dataAbertura);
    contas_.emplace(id, conta);
    return conta;
}

/*
 * Recupera uma conta existente pelo seu ID
 * @throws invalid_argument se a conta n�o for encontrada
 */
shared_ptr<Conta> ServicoConta::obterConta(const string& id) const {
    auto it = contas_.find(id);
    if (it == contas_.end())
        throw invalid_argument("Conta n�o encontrada: " + id);
    return it->second;
}

/*
 * Obt�m o saldo atual da conta na data de refer�ncia especificada
 * Primeiro efetiva todas as transa��es pendentes at� a data de refer�ncia
 * para garantir que o saldo retornado reflita todas as opera��es agendadas
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
 * Calcula o total de movimenta��es em um per�odo espec�fico
 * Considera apenas transa��es j� efetivadas no per�odo informado
 * �til para an�lises de fluxo de caixa e extratos
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
