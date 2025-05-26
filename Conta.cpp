#include "Conta.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
using boost::multiprecision::cpp_dec_float_50;

using decimal = boost::multiprecision::cpp_dec_float_50;

// Inicialização do conjunto estático de IDs
std::set<std::string> Conta::idsUtilizados;

Conta::Conta(const std::string& id,
    const std::string& nomeTitular,
    const system_clock::time_point& aberturaDaConta)
    : id(id), nomeTitular(nomeTitular), saldo(0), aberturaDaConta(aberturaDaConta)
{
    idsUtilizados.insert(id);
}

std::shared_ptr<Conta> Conta::Criar(const std::string& id,
    const std::string& nomeTitular,
    const system_clock::time_point& aberturaDaConta)
{
    if (idsUtilizados.find(id) != idsUtilizados.end()) {
        std::cerr << "Erro ao criar conta: ID de conta já utilizado: " << id << std::endl;
		throw std::invalid_argument("ID de conta já utilizado.");
    }
    return std::shared_ptr<Conta>(new Conta(id, nomeTitular, aberturaDaConta));
}

void Conta::credito(decimal valor, const system_clock::time_point& /*data*/) {
    try {
        if (valor <= 0) {
            std::cerr << "Erro: Valor de credito invalido: " << valor << std::endl;
            throw std::invalid_argument("Valor de credito deve ser positivo.");
        }
        std::cout << "Credito (" << nomeTitular << "): Saldo anterior: " << saldo << std::endl;
        saldo += valor;
        std::cout << "Credito (" << nomeTitular << "): Saldo atual apos credito: " << saldo << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Erro ao realizar credito: " << e.what() << std::endl;
        throw;
    }
    catch (...) {
        std::cerr << "Erro desconhecido ao realizar credito." << std::endl;
        throw;
    }
}

void Conta::debito(decimal valor, const system_clock::time_point& /*data*/) {
    try {
        if (valor <= 0) {
            std::cerr << "Erro: Valor de debito invalido: " << valor << std::endl;
            throw std::invalid_argument("Valor de debito deve ser positivo.");
        }
        if (valor > saldo) {
            std::cerr << "Erro: Saldo insuficiente para debito. Tentado: " << valor
                << ", Saldo atual: " << saldo << std::endl;
            throw std::invalid_argument("Saldo insuficiente.");
        }
        std::cout << "Debito (" << nomeTitular << "): Saldo anterior: " << saldo << std::endl;
        saldo -= valor;
        std::cout << "Debito (" << nomeTitular << "): Saldo atual apos debito: " << saldo << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Erro ao realizar debito: " << e.what() << std::endl;
        throw;
    }
    catch (...) {
        std::cerr << "Erro desconhecido ao realizar debito." << std::endl;
        throw;
    }
}

system_clock::time_point Conta::dataMinimaSaldoZero(
    const vector<std::shared_ptr<Transacao>>& transacoes) const {
    try {
        vector<std::shared_ptr<Transacao>> txs;
        for (const auto& tx : transacoes) {
            if (tx->getIdConta() == id && tx->ehEfetivada()) {
                txs.push_back(tx);
                std::cout << "Transação efetivada encontrada. Data: "
                    << system_clock::to_time_t(tx->getData())
                    << " Valor: " << tx->getValor() << std::endl;
            }
        }

        if (txs.empty()) {
            std::cerr << "Erro: Nenhuma transação efetivada encontrada para a conta " << id << std::endl;
            throw std::invalid_argument("Não há transações efetivadas para esta conta");
        }

        sort(txs.begin(), txs.end(), [](const auto& a, const auto& b) {
            return a->getData() < b->getData();
            });

        decimal saldoSimulado{ 0 };
        for (const auto& tx : txs) {
            saldoSimulado += tx->getValor();
            std::cout << "Saldo simulado após transação (Data: "
                << system_clock::to_time_t(tx->getData())
                << "): " << saldoSimulado << std::endl;
            if (saldoSimulado == 0)
                return tx->getData();
        }

        std::cerr << "Erro: O saldo simulado nunca voltou a zero." << std::endl;
        throw std::invalid_argument("Não há data em que o saldo retorna a zero");
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Erro ao calcular data mínima: " << e.what() << std::endl;
        throw;
    }
    catch (...) {
        std::cerr << "Erro desconhecido ao calcular data mínima." << std::endl;
        throw;
    }
}

std::string Conta::getId() const {
    return id;
}

std::string Conta::getNomeTitular() const {
    return nomeTitular;
}

decimal Conta::getSaldo() const {
    return saldo;
}

system_clock::time_point Conta::getaberturaDaConta() const {
    return aberturaDaConta;
}
