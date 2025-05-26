// GerenciadorTransacoes.cpp

#include "GerenciarTransacao.h"
#include <algorithm>

using namespace std;
using namespace std::chrono;


// --- GerenciadorTransacoes - Implementação ---

GerenciadorTransacoes::GerenciadorTransacoes(
    function<shared_ptr<Conta>(const string&)> consultaConta
) : consultaConta_(consultaConta)
{
}


/*
 * Adiciona uma nova transação ao gerenciador
 * @throws invalid_argument se o ID da transação já existir ou se a conta não for encontrada
 * @throws invalid_argument se a data da transação for anterior à data de abertura da conta
 */ 
void GerenciadorTransacoes::adicionar(const shared_ptr<Transacao>& tx) {
    // 1) Verifica duplicata de ID
    for (const auto& existente : transacoes_) {
        if (existente->getId() == tx->getId())
            throw invalid_argument("ID de transação duplicado: " + tx->getId());
    }
    // 2) Verifica se a conta existe e se a data é >= à data de abertura
    auto conta = consultaConta_(tx->getIdConta());
    if (!conta)
        throw invalid_argument("Conta não encontrada: " + tx->getIdConta());
    if (tx->getData() < conta->getaberturaDaConta())
        throw invalid_argument("Data de transação anterior à abertura da conta");

    transacoes_.push_back(tx);
}

/// Remove uma transação do gerenciador
void GerenciadorTransacoes::remover(const string& txId) {
    auto it = find_if(transacoes_.begin(), transacoes_.end(),
        [&](const shared_ptr<Transacao>& t) { return t->getId() == txId; });
    if (it == transacoes_.end())
        throw invalid_argument("Transação não encontrada: " + txId);

    // Se já estava efetivada, reverte no saldo
    if ((*it)->ehEfetivada()) {
        auto conta = consultaConta_((*it)->getIdConta());
        decimal valor = (*it)->getValor();
        if (valor > 0)
            conta->debito(valor, (*it)->getData());
        else
            conta->credito(-valor, (*it)->getData());
    }

    transacoes_.erase(it);
}


/*
 * Busca transações por ID da conta e intervalo de datas
 * @param contaId ID da conta a ser buscada
 * @param inicio Data de início do intervalo
 * @param fim Data de fim do intervalo
 * @param somenteEfetivadas Se true, retorna apenas transações efetivadas
 * @return Lista de transações que atendem aos critérios
 */
vector<shared_ptr<Transacao>>
GerenciadorTransacoes::buscarPorPeriodo(
    const string& contaId,
    const system_clock::time_point& inicio,
    const system_clock::time_point& fim,
    bool somenteEfetivadas
) const {
    vector<shared_ptr<Transacao>> resultado;
    for (const auto& tx : transacoes_) {
        if (tx->getIdConta() != contaId)                    continue;
        if (tx->getData() < inicio || tx->getData() > fim)   continue;
        if (somenteEfetivadas && !tx->ehEfetivada())         continue;
        resultado.push_back(tx);
    }
    return resultado;
}

/*
 * Efetiva todas as transações futuras até a data especificada
 * Atualiza o saldo da conta correspondente
 */
void GerenciadorTransacoes::efetivarAte(const system_clock::time_point& ate) {
    for (const auto& tx : transacoes_) {
        if (tx->ehFutura() && tx->getData() <= ate) {
            tx->marcarComoEfetivada();
            auto conta = consultaConta_(tx->getIdConta());
            decimal valor = tx->getValor(); // Use decimal diretamente
            if (valor > 0)
                conta->credito(valor, tx->getData());
            else
                conta->debito(-valor, tx->getData());
        }
    }
}
