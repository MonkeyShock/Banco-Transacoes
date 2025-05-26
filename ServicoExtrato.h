// ServicoExtrato.h
#pragma once

#include "Extrato.h"
#include "GerenciarTransacao.h"
#include "ServicoConta.h"

#include <memory>
#include <string>

class ServicoExtrato {
private:
    std::shared_ptr<GerenciadorTransacoes> gerTrans_;
    std::shared_ptr<ServicoConta> servicoConta_;

public:
    /**
     * @param gerTrans      Gerenciador de transações já configurado
     * @param servicoConta  Serviço de conta para obter saldos
     */
    ServicoExtrato(const std::shared_ptr<GerenciadorTransacoes>& gerTrans,
        const std::shared_ptr<ServicoConta>& servicoConta);

    /**
     * Gera um Extrato para a conta no período [inicio, fim]:
     * 1) obtém saldo de abertura em (inicio - 1 dia)
     * 2) percorre lançamentos efetivados no período, calculando saldos após cada um
     * 3) obtém saldo de fechamento em fim
     */
    Extrato gerarExtrato(const std::string& idConta,
        const std::chrono::system_clock::time_point& inicio,
        const std::chrono::system_clock::time_point& fim);
};
