// ServicoExtrato.cpp

#include "ServicoExtrato.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

ServicoExtrato::ServicoExtrato(
    const shared_ptr<GerenciadorTransacoes>& gerTrans,
    const shared_ptr<ServicoConta>& servicoConta
) : gerTrans_(gerTrans),
servicoConta_(servicoConta)
{
}

Extrato ServicoExtrato::gerarExtrato(
    const string& idConta,
    const system_clock::time_point& inicio,
    const system_clock::time_point& fim
) {
    // 1) Obter saldo inicial (saldo até o dia anterior ao início do período)
    auto umDia = hours(24);
    decimal saldoAbertura = servicoConta_->obterSaldo(idConta, inicio - umDia);

    // 2) Lista de lançamentos efetivados no período 
    auto listaTx = gerTrans_->buscarPorPeriodo(idConta, inicio, fim, true);

    vector<MovimentoExtrato> movimentos;
    decimal saldoCorrente = saldoAbertura;

    for (const auto& tx : listaTx) {
        auto valor = tx->getValor();
        saldoCorrente += valor;
        movimentos.push_back({ tx->getData(), valor, saldoCorrente });
    }

    // 3) Saldo de fechamento = saldo atual real da conta
    decimal saldoFechamento = servicoConta_->obterSaldo(idConta, fim);

    return Extrato(saldoAbertura, movimentos, saldoFechamento);
}

