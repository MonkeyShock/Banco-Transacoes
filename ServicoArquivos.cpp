#include "ServicoArquivos.h"
#include <rapidcsv.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include "Conta.h"
#include <ctime>

using namespace std;

// --- UTILITÁRIOS DE FORMATAÇÃO DE DATA ---

/*
 * Converte um time_point para string no formato YYYY-MM-DD
 * Utiliza localtime_s para garantir thread-safety na conversão
 */
std::string formatarData(const std::chrono::system_clock::time_point& tp) {
    std::time_t tempo = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
    localtime_s(&tm, &tempo); 
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}


/*
 * Converte uma string no formato YYYY-MM-DD para time_point
 * Utilizada na leitura de datas dos arquivos CSV
 */
std::chrono::system_clock::time_point parseData(const std::string& str) {
    std::tm tm = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}



/*
 * Persiste as contas em arquivo CSV
 * Formato: id,nomeTitular,dataAbertura,saldo
 * Utilizado principalmente para exportação e integração com outros sistemas
 */
void ServicoArquivos::salvarCSVContas(const std::string& path, const std::map<std::string, std::shared_ptr<Conta>>& contas) {
    ofstream file(path);
    file << "id,nomeTitular,dataAbertura,saldo\n";
    for (const auto& entry : contas) { 
        file << entry.first << ","
            << entry.second->getNomeTitular() << ","
            << formatarData(entry.second->getaberturaDaConta()) << ","
            << entry.second->getSaldo() << "\n";
    }
    file.close();
}


/*
 * Carrega as contas de um arquivo CSV
 * Formato: id,nomeTitular,dataAbertura,saldo
 * Utiliza a biblioteca rapidcsv para leitura eficiente de arquivos CSV
 */
std::map<std::string, std::shared_ptr<Conta>> ServicoArquivos::carregarCSVContas(const std::string& path) {
    std::map<std::string, std::shared_ptr<Conta>> contas;
    rapidcsv::Document doc(path, rapidcsv::LabelParams(0, -1));
    auto ids = doc.GetColumn<std::string>("id");
    auto nomes = doc.GetColumn<std::string>("nomeTitular");
    auto datas = doc.GetColumn<std::string>("dataAbertura");
    auto saldos = doc.GetColumn<std::string>("saldo");

    for (size_t i = 0; i < ids.size(); ++i) {
        auto conta = Conta::Criar(ids[i], nomes[i], parseData(datas[i]));
        conta->setSaldo(decimal(saldos[i]));
        contas[ids[i]] = conta;
    }
    return contas;
}


/*
 * Persiste as transações em arquivo CSV
 * Formato: id,idConta,valor,data,status
 * Utilizado principalmente para exportação e integração com outros sistemas
 */
void ServicoArquivos::salvarCSVTransacoes(const std::string& path, const std::vector<std::shared_ptr<Transacao>>& transacoes) {
    ofstream file(path);
    file << "id,idConta,valor,data,status\n";
    for (const auto& tx : transacoes) {
        file << tx->getId() << ","
            << tx->getIdConta() << ","
            << tx->getValor() << ","
            << formatarData(tx->getData()) << ","
            << (tx->ehEfetivada() ? "EFETIVADA" : "FUTURA") << "\n";
    }
    file.close();
}


/*
 * Carrega as transações de um arquivo CSV
 * Formato: id,idConta,valor,data,status
 * Utiliza a biblioteca rapidcsv para leitura eficiente de arquivos CSV
 */
std::vector<std::shared_ptr<Transacao>> ServicoArquivos::carregarCSVTransacoes(const std::string& path) {
    std::vector<std::shared_ptr<Transacao>> transacoes;
    rapidcsv::Document doc(path, rapidcsv::LabelParams(0, -1));
    auto ids = doc.GetColumn<std::string>("id");
    auto contas = doc.GetColumn<std::string>("idConta");
    auto valores = doc.GetColumn<std::string>("valor");
    auto datas = doc.GetColumn<std::string>("data");
    auto status = doc.GetColumn<std::string>("status");

    for (size_t i = 0; i < ids.size(); ++i) {
        decimal valor(valores[i]);
        auto data = parseData(datas[i]);
        auto stat = (status[i] == "EFETIVADA") ? StatusTransacao::EFETIVADA : StatusTransacao::FUTURA;
        transacoes.push_back(std::make_shared<Transacao>(ids[i], contas[i], valor, data, stat));
    }
    return transacoes;
}


/*
 * Salva o estado completo do sistema em formato binário
 * Utiliza Boost.Serialization para persistência eficiente
 * Preserva relacionamentos entre objetos e ponteiros compartilhados
 */
void ServicoArquivos::salvarBinario(const std::string& path, const std::map<std::string, std::shared_ptr<Conta>>& contas, const std::vector<std::shared_ptr<Transacao>>& transacoes) {
    std::ofstream ofs(path);
    boost::archive::text_oarchive oa(ofs);
    oa << contas;
    oa << transacoes;
    ofs.close();
}
/*
 * Carrega o estado completo do sistema a partir de um arquivo binário
 * Utiliza Boost.Serialization para leitura eficiente
 * Preserva relacionamentos entre objetos e ponteiros compartilhados
 */
void ServicoArquivos::carregarBinario(const std::string& path, std::map<std::string, std::shared_ptr<Conta>>& contas, std::vector<std::shared_ptr<Transacao>>& transacoes) {
    std::ifstream ifs(path);
    boost::archive::text_iarchive ia(ifs);
    ia >> contas;
    ia >> transacoes;
    ifs.close();
}
