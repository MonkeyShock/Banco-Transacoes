#include "Conta.h"
#include "Transacao.h"
#include "GerenciarTransacao.h"
#include "ServicoConta.h"
#include "ServicoExtrato.h"
#include "ServicoArquivos.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip> // Para std::put_time



using namespace std;
using namespace std::chrono;

int main() {
    // Funcao lambda para buscar conta por ID
    map<string, shared_ptr<Conta>> contas;
    auto consultaConta = [&](const string& id) -> shared_ptr<Conta> {
        auto it = contas.find(id);
        if (it != contas.end()) return it->second;
        return nullptr;
        };

    // Instanciar servicos
    auto gerTrans = make_shared<GerenciadorTransacoes>(consultaConta);
    auto servicoConta = make_shared<ServicoConta>(gerTrans);
    auto servicoExtrato = make_shared<ServicoExtrato>(gerTrans, servicoConta);
    auto hoje = system_clock::now();

    cout << "\n=== Criacao de Contas ===" << endl;
    try {
        // Criar multiplas contas
        auto contaJoao = servicoConta->criarConta("001", "Joao Silva", hoje);
        auto contaMaria = servicoConta->criarConta("002", "Maria Santos", hoje);
        auto contaPedro = servicoConta->criarConta("003", "Pedro Oliveira", hoje);

        contas["001"] = contaJoao;
        contas["002"] = contaMaria;
        contas["003"] = contaPedro;

        // Operacoes na conta do Joao
        auto tx1 = make_shared<Transacao>("tx1", "001", decimal("5000.00"), hoje + hours(1));
        gerTrans->adicionar(tx1);

        auto tx2 = make_shared<Transacao>("tx2", "001", decimal("-1500.00"), hoje + hours(2));
        gerTrans->adicionar(tx2);

        // Operacoes na conta da Maria
        auto tx3 = make_shared<Transacao>("tx3", "002", decimal("3000.00"), hoje + hours(1));
        gerTrans->adicionar(tx3);

        auto tx4 = make_shared<Transacao>("tx4", "002", decimal("-500.00"), hoje + hours(2));
        gerTrans->adicionar(tx4);

        // Operacoes na conta do Pedro
        auto tx5 = make_shared<Transacao>("tx5", "003", decimal("7000.00"), hoje + hours(1));
        gerTrans->adicionar(tx5);

        auto tx6 = make_shared<Transacao>("tx6", "003", decimal("-2000.00"), hoje + hours(2));
        gerTrans->adicionar(tx6);

        // Efetivar todas as transacoes
        gerTrans->efetivarAte(hoje + hours(3));

        // Mostrar saldos
        cout << "\n=== Saldos Atuais ===" << endl;
        cout << "Saldo Joao: " << servicoConta->obterSaldo("001", hoje + hours(3)) << endl;
        cout << "Saldo Maria: " << servicoConta->obterSaldo("002", hoje + hours(3)) << endl;
        cout << "Saldo Pedro: " << servicoConta->obterSaldo("003", hoje + hours(3)) << endl;

        // Gerar extrato para cada conta
        cout << "\n=== Extratos ===" << endl;
        for (const auto& par : contas) {
            cout << "\nExtrato de " << par.second->getNomeTitular() << ":" << endl;
            auto extrato = servicoExtrato->gerarExtrato(par.first, hoje, hoje + hours(3));
            cout << "Saldo inicial: " << extrato.getSaldoAbertura() << endl;

            for (const auto& mov : extrato.getMovimentos()) {
                time_t t = system_clock::to_time_t(mov.data);
                struct tm localTime;
                localtime_s(&localTime, &t);
                cout << put_time(&localTime, "%Y-%m-%d %H:%M") << " | "
                    << mov.valor << " | Saldo: " << mov.saldoApos << endl;
            }
            cout << "Saldo final: " << extrato.getSaldoFechamento() << endl;
        }

        // Persistir dados
        cout << "\n=== Salvando dados ===" << endl;
        ServicoArquivos arquivos;
        arquivos.salvarCSVContas("contas.csv", contas);
        vector<shared_ptr<Transacao>> todasTransacoes = { tx1, tx2, tx3, tx4, tx5, tx6 };
        arquivos.salvarCSVTransacoes("transacoes.csv", todasTransacoes);
        cout << "Dados salvos com sucesso!" << endl;

    }
    catch (const invalid_argument& e) {
        cerr << "Erro de validacao: " << e.what() << endl;
        return 1;
    }
    catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "Erro desconhecido!" << endl;
        return 1;
    }

    return 0;
}


