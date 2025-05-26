#pragma once

#include "Conta.h"
#include "Transacao.h"

#include <map>
#include <vector>
#include <string>

class ServicoArquivos {
public:
    // --- Persistência em CSV ---

    /**
     * Salva contas no formato CSV.
     * @param path Caminho do arquivo (ex: "contas.csv")
     */
    static void salvarCSVContas(const std::string& path,
        const std::map<std::string, std::shared_ptr<Conta>>& contas);

    /**
     * Lê contas de um arquivo CSV.
     * @return Mapa com as contas carregadas
     */
    static std::map<std::string, std::shared_ptr<Conta>>
        carregarCSVContas(const std::string& path);

    /**
     * Salva transações no formato CSV.
     * @param path Caminho do arquivo (ex: "transacoes.csv")
     */
    static void salvarCSVTransacoes(const std::string& path,
        const std::vector<std::shared_ptr<Transacao>>& transacoes);

    /**
     * Lê transações de um arquivo CSV.
     * @return Vetor com as transações carregadas
     */
    static std::vector<std::shared_ptr<Transacao>>
        carregarCSVTransacoes(const std::string& path);

    // --- Persistência em Binário (futuramente) ---

    static void salvarBinario(const std::string& path,
        const std::map<std::string, std::shared_ptr<Conta>>& contas,
        const std::vector<std::shared_ptr<Transacao>>& transacoes);

    static void carregarBinario(const std::string& path,
        std::map<std::string, std::shared_ptr<Conta>>& contas,
        std::vector<std::shared_ptr<Transacao>>& transacoes);
};
