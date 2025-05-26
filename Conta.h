// Conta.h
#pragma once
#include <string>
#include <set>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "Transacao.h"

using decimal = boost::multiprecision::cpp_dec_float_50;

class Conta {
private:
    
    Conta() : saldo(0) {}

    
    Conta(const std::string& id,
        const std::string& nomeTitular,
        const std::chrono::system_clock::time_point& aberturaDaConta);

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& id;
        ar& nomeTitular;
        ar& saldo;
        ar& aberturaDaConta;
    }

    
    std::string id;
    std::string nomeTitular;
    decimal saldo;
    std::chrono::system_clock::time_point aberturaDaConta;
    static std::set<std::string> idsUtilizados;

public:

    static void limparIdsUtilizados() {
        idsUtilizados.clear();
    }

    /**
     * M�todo de f�brica:
     * Cria e retorna um objeto Conta compartilhada,
     * garantindo que o id seja �nico e inicializando saldo zero.
     */
    static std::shared_ptr<Conta> Criar(const std::string& id,
        const std::string& nomeTitular,
        const std::chrono::system_clock::time_point& aberturaDaConta);

    /**
     * Deposit (cr�dito) de valor na conta.
     * @param amount  Valor a creditar (deve ser positivo).
     * @param date    Data da opera��o (n�o utilizada nesta vers�o b�sica).
     * @throws std::invalid_argument se amount <= 0.
     */
    void credito(decimal amount, const std::chrono::system_clock::time_point& date);

    /**
     * Retirada (d�bito) de valor da conta.
     * @param amount  Valor a debitar (deve ser positivo e <= saldo).
     * @param date    Data da opera��o (n�o utilizada nesta vers�o b�sica).
     * @throws std::invalid_argument se amount <= 0 ou saldo insuficiente.
     */
    void debito(decimal amount, const std::chrono::system_clock::time_point& date);

    /**
     * Obt�m o identificador da conta.
     */
    std::string getId() const;

    /**
     * Obt�m o nome do titular da conta.
     */
    std::string getNomeTitular() const;

    /**
     * Obt�m o saldo atual da conta.
     */
    decimal getSaldo() const; // Troque double por decimal

    /**
     * Obt�m a data de abertura da conta.
     */
    std::chrono::system_clock::time_point getaberturaDaConta() const;

    void setSaldo(decimal novoSaldo) { saldo = novoSaldo; }

    /**
     * Retorna a primeira data em que o saldo acumulado,
     * simulando-se as transa��es efetivadas, volta a zero.
     * Apenas as transa��es que pertencem a esta conta e que est�o EFETIVADAS s�o consideradas.
     * @param transacoes Lista completa de Transacao.
     * @throws std::invalid_argument se o saldo nunca retornar a zero.
     */
    std::chrono::system_clock::time_point
        dataMinimaSaldoZero(const std::vector<std::shared_ptr<Transacao>>& transacoes) const;
};
