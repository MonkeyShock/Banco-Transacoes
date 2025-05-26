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

// Para somas de valores de transa��o com alta precis�o
using decimal = boost::multiprecision::cpp_dec_float_50;

class ServicoConta {
private:
    // Armazena todas as contas criadas
    std::map<std::string, std::shared_ptr<Conta>> contas_;
    // Gerenciador de transa��es (injetado)
    std::shared_ptr<GerenciadorTransacoes> gerTrans_;

public:
    /**
     * @param gerTrans  Gerenciador de transa��es j� inicializado,
     *                  com uma fun��o de lookup de Conta.
     */
    ServicoConta(const std::shared_ptr<GerenciadorTransacoes>& gerTrans);

    /**
     * Cria uma nova conta e armazena no servi�o.
     * @throws invalid_argument se id duplicado.
     */
    std::shared_ptr<Conta> criarConta(
        const std::string& id,
        const std::string& nomeTitular,
        const std::chrono::system_clock::time_point& dataAbertura
    );

    /**
     * Retorna ponteiro para Conta j� criada.
     * @throws invalid_argument se n�o existir.
     */
    std::shared_ptr<Conta> obterConta(const std::string& id) const;

    /**
     * Retorna o saldo da conta na data informada.
     * Efetiva todas as transa��es at� essa data antes de ler o saldo.
     */
    decimal obterSaldo(
        const std::string& id,
        const std::chrono::system_clock::time_point& dataReferencia
    );

    /**
     * Soma todos os lan�amentos efetivados no per�odo [inicio, fim].
     * Retorna um decimal de alta precis�o.
     */
    decimal obterTotalizado(
        const std::string& id,
        const std::chrono::system_clock::time_point& inicio,
        const std::chrono::system_clock::time_point& fim
    );
};
