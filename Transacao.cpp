#include "Transacao.h"
#include <stdexcept>

// Construtor
Transacao::Transacao(
    const std::string& id,
    const std::string& idConta,
    const decimal& valor,
    const std::chrono::system_clock::time_point& data,
    StatusTransacao status
)
    : id_(id)
    , idConta_(idConta)
    , valor_(valor)
    , data_(data)
    , status_(status)
{
    if (id_.empty()) {
        throw std::invalid_argument("O ID da transa��o n�o pode ser vazio");
    }
    if (idConta_.empty()) {
        throw std::invalid_argument("O ID da conta n�o pode ser vazio");
    }
    // O valor pode ser positivo (cr�dito) ou negativo (d�bito), ent�o n�o validamos o sinal aqui
}

// Marca esta transa��o como efetivada
void Transacao::marcarComoEfetivada() {
    status_ = StatusTransacao::EFETIVADA;
}

// Consulta se a transa��o � futura
bool Transacao::ehFutura() const {
    return status_ == StatusTransacao::FUTURA;
}

// Consulta se a transa��o j� foi efetivada
bool Transacao::ehEfetivada() const {
    return status_ == StatusTransacao::EFETIVADA;
}

// Getters
const std::string& Transacao::getId() const {
    return id_;
}

const std::string& Transacao::getIdConta() const {
    return idConta_;
}

const decimal& Transacao::getValor() const {
    return valor_;
}

std::chrono::system_clock::time_point Transacao::getData() const {
    return data_;
}

StatusTransacao Transacao::getStatus() const {
    return status_;
}
