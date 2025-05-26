// ServicoConta.h
#pragma once

#include "Conta.h"
#include "GerenciarTransacao.h"
#include "Transacao.h"

#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <boost/multiprecision/cpp_dec_float.hpp>

// Para somas de valores de transação com alta precisão
using decimal = boost::multiprecision::cpp_dec_float_50;

class ServicoConta {
private:
    // Armazena todas as contas criadas
    std::map<std::string, std::shared_ptr<Conta>> contas_;
    // Gerenciador de transações (injetado)
    std::shared_ptr<GerenciadorTransacoes> gerTrans_;

public:
    /**
     * @param gerTrans  Gerenciador de transações já inicializado,
     *                  com uma função de lookup de Conta.
     */
    ServicoConta(const std::shared_ptr<GerenciadorTransacoes>& gerTrans);

    /**
     * Cria uma nova conta e armazena no serviço.
     * @throws invalid_argument se id duplicado.
     */
    std::shared_ptr<Conta> criarConta(
        const std::string& id,
        const std::string& nomeTitular,
        const std::chrono::system_clock::time_point& dataAbertura
    );

    /**
     * Retorna ponteiro para Conta já criada.
     * @throws invalid_argument se não existir.
     */
    std::shared_ptr<Conta> obterConta(const std::string& id) const;

    /**
     * Retorna o saldo da conta na data informada.
     * Efetiva todas as transações até essa data antes de ler o saldo.
     */
    decimal obterSaldo(
        const std::string& id,
        const std::chrono::system_clock::time_point& dataReferencia
    );

    /**
     * Soma todos os lançamentos efetivados no período [inicio, fim].
     * Retorna um decimal de alta precisão.
     */
    decimal obterTotalizado(
        const std::string& id,
        const std::chrono::system_clock::time_point& inicio,
        const std::chrono::system_clock::time_point& fim
    );
};
