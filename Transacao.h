#pragma once

#include <string>
#include <chrono>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>

// Use um tipo decimal de alta precis�o para valores monet�rios 
using decimal = boost::multiprecision::cpp_dec_float_50;


namespace boost {
    namespace serialization {

        template<class Archive>
        void serialize(Archive& ar, std::chrono::system_clock::time_point& t, const unsigned int /*version*/)
        {
            using namespace std::chrono;
            system_clock::duration duration = t.time_since_epoch();
            auto ticks = duration.count();

            ar& ticks;

            t = system_clock::time_point(system_clock::duration(ticks));
        }

    } // namespace serialization
} // namespace boost

// Enumera��o de status da transa��o
enum class StatusTransacao {
    FUTURA,
    EFETIVADA
};

class Transacao {
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& id_;
        ar& idConta_;
        ar& valor_;
        ar& data_;
        ar& status_;
    }

    std::string id_;                        // Identificador �nico da transa��o
    std::string idConta_;                   // ID da conta associada
    decimal valor_;                         // Positivo para cr�dito, negativo para d�bito
    std::chrono::system_clock::time_point data_;  // Data da transa��o
    StatusTransacao status_;                // FUTURA ou EFETIVADA

public:

    Transacao() :
        id_(""),
        idConta_(""),
        valor_(0),
        data_(std::chrono::system_clock::now()),
        status_(StatusTransacao::FUTURA)
    {
    }

    // Main constructor
    Transacao(const std::string& id,
        const std::string& idConta,
        const decimal& valor,
        const std::chrono::system_clock::time_point& data,
        StatusTransacao status = StatusTransacao::FUTURA);

    // Marca esta transa��o como efetivada (ou seja, aplica a transa��o)
    void marcarComoEfetivada();

    // Fun��es auxiliares de consulta 
    bool ehFutura() const;
    bool ehEfetivada() const;

    // Getters
    const std::string& getId() const;
    const std::string& getIdConta() const;
    const decimal& getValor() const;
    std::chrono::system_clock::time_point getData() const;
    StatusTransacao getStatus() const;
};
