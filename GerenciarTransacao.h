#pragma once

#include "Transacao.h"
#include "Conta.h"

#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <stdexcept>

class GerenciadorTransacoes {
private:
    // Lista de todas as transações
    std::vector<std::shared_ptr<Transacao>> transacoes_;

    // Função para buscar uma Conta dado o seu ID
    std::function<std::shared_ptr<Conta>(const std::string&)> consultaConta_;

public:
    /**
     * @param consultaConta  Função que, dado um accountId, retorna o ponteiro para Conta
     */
    GerenciadorTransacoes(
        std::function<std::shared_ptr<Conta>(const std::string&)> consultaConta
    );

    /**
     * Adiciona uma nova transação:
     * - Nega duplicatas de ID
     * - Não permite data anterior à abertura da conta
     */
    void adicionar(const std::shared_ptr<Transacao>& tx);

    /**
     * Remove transação por ID:
     * - Se já estiver EFETIVADA, reverte o efeito no saldo da conta
     */
    void remover(const std::string& txId);

    /**
     * Busca transações de uma conta num intervalo de datas.
     * @param contaId           ID da conta
     * @param inicio            data inicial (inclusive)
     * @param fim               data final (inclusive)
     * @param somenteEfetivadas se true, retorna apenas as EFETIVADAS
     */
    std::vector<std::shared_ptr<Transacao>>
        buscarPorPeriodo(
            const std::string& contaId,
            const std::chrono::system_clock::time_point& inicio,
            const std::chrono::system_clock::time_point& fim,
            bool somenteEfetivadas
        ) const;

    /**
     * Transforma todas as transações FUTURAS com data ≤ 'ate' em EFETIVADAS e
     * aplica o valor no saldo da conta.
     */
    void efetivarAte(const std::chrono::system_clock::time_point& ate);
};
