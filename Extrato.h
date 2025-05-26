// Extrato.h
#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

using decimal = boost::multiprecision::cpp_dec_float_50;

/**
 * Representa uma linha de extrato: data, valor (positivo = crédito,
 * negativo = débito) e saldo após aplicação.
 */
struct MovimentoExtrato {
    std::chrono::system_clock::time_point data;
    decimal valor;
    decimal saldoApos;
};

/**
 * DTO que agrupa:
 * - saldo de abertura (antes de qualquer lançamento)
 * - lista de movimentos
 * - saldo de fechamento (após todos os movimentos)
 */
class Extrato {
private:
    decimal saldoAbertura_;
    std::vector<MovimentoExtrato> movimentos_;
    decimal saldoFechamento_;

public:
    Extrato(decimal abertura,
        std::vector<MovimentoExtrato> movimentos,
        decimal fechamento)
        : saldoAbertura_(abertura),
        movimentos_(std::move(movimentos)),
        saldoFechamento_(fechamento)
    {
    }

    decimal getSaldoAbertura() const { return saldoAbertura_; }
    const std::vector<MovimentoExtrato>& getMovimentos() const { return movimentos_; }
    decimal getSaldoFechamento() const { return saldoFechamento_; }
};
