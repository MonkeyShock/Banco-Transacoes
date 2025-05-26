# Banco-Transacoes (Bank Transactions) Project 👋

This project, "Banco-Transacoes," is a C++ application designed to simulate and manage basic banking operations. It focuses on creating accounts, processing financial transactions (deposits and withdrawals), managing transaction data, and generating account statements. The system also includes functionality for data persistence by saving and loading account and transaction information to/from CSV and binary files.

---
## Features ✨

* **Account Management:**
    * Create bank accounts with unique IDs, account holder names, and opening dates.
    * Prevents the creation of accounts with duplicate IDs.
    * Stores and retrieves account information, including current balance.
* **Transaction Processing:**
    * Handles credit (deposit) and debit (withdrawal) operations for accounts.
    * Validates transactions:
        * Credit/debit values must be positive.
        * Debits cannot exceed the current account balance.
        * Transaction dates cannot be prior to the account opening date.
    * Transactions can be marked as "FUTURE" or "EFFECTIVE" (processed).
    * Allows for adding and removing transactions.
    * If an "EFFECTIVE" transaction is removed, its effect on the account balance is reverted.
* **Data Management & Persistence:**
    * Manages a list of transactions and can search/filter them by account ID and date range.
    * Includes a `GerenciadorTransacoes` (Transaction Manager) to handle all transaction logic.
    * `ServicoArquivos` (File Service) handles saving and loading:
        * Account data to/from `contas.csv`.
        * Transaction data to/from `transacoes.csv`.
        * The entire system state (accounts and transactions) to/from a binary file for more robust persistence.
    * Uses `rapidcsv` for CSV parsing and `Boost.Serialization` for binary serialization.
* **Statement Generation:**
    * `ServicoExtrato` (Statement Service) generates account statements for a given period.
    * Statements include:
        * Opening balance (balance at the start of the period).
        * A list of movements (transactions) with date, value, and balance after each.
        * Closing balance (balance at the end of the period).
* **Decimal Precision:**
    * Uses `boost::multiprecision::cpp_dec_float_50` for financial calculations to ensure high precision.

---
## Project Structure 📁

The project is organized into several C++ header (`.h`) and source (`.cpp`) files, defining classes that represent different entities and services within the banking system:

* **Core Entities:**
    * `Conta.h` / `Conta.cpp`: Defines the `Conta` (Account) class, representing a bank account with properties like ID, owner name, balance, and opening date. It handles basic operations like credit and debit.
    * `Transacao.h` / `Transacao.cpp`: Defines the `Transacao` (Transaction) class, representing a financial transaction with an ID, account ID, value, date, and status (future or effective).
    * `Extrato.h`: Defines the `Extrato` (Statement) class and `MovimentoExtrato` (Statement Movement) struct, used to hold data for generating account statements.
* **Service/Manager Classes:**
    * `GerenciarTransacao.h` / `GerenciarTransacao.cpp`: `GerenciadorTransacoes` (Transaction Manager) class responsible for adding, removing, searching, and processing transactions, including updating account balances when transactions are made effective.
    * `ServicoConta.h` / `ServicoConta.cpp`: `ServicoConta` (Account Service) class that manages the collection of accounts, providing functionalities to create accounts, retrieve account information, and get balances, interacting with the `GerenciadorTransacoes`.
    * `ServicoExtrato.h` / `ServicoExtrato.cpp`: `ServicoExtrato` (Statement Service) class used to generate detailed account statements based on transactions and account balances over a specified period.
    * `ServicoArquivos.h` / `ServicoArquivos.cpp`: `ServicoArquivos` (File Service) class responsible for data persistence, handling the saving and loading of account and transaction data to/from CSV and binary files.
* **Main Program:**
    * `Main.cpp`: Contains the `main` function, which demonstrates the system's functionalities by creating accounts, adding transactions, processing them, displaying balances, generating statements, and saving data to files.
* **Data Files:**
    * `contas.csv`: CSV file for storing account data (ID, holder name, opening date, balance).
    * `transacoes.csv`: CSV file for storing transaction data (ID, account ID, value, date, status).
* **Documentation:**
    * `README.md`: Basic readme file for the project.

---
## How It Works (Simplified Flow) ⚙️

1.  **Initialization (`Main.cpp`):**
    * A map (`contas`) is used to store `Conta` objects.
    * A lambda function `consultaConta` is defined to allow services to look up accounts by ID.
    * Instances of `GerenciadorTransacoes`, `ServicoConta`, and `ServicoExtrato` are created, injecting dependencies (e.g., `consultaConta` into `GerenciadorTransacoes`).
2.  **Account Creation (`ServicoConta`):**
    * New accounts are created using `servicoConta->criarConta()`, which internally calls `Conta::Criar()` to ensure unique IDs. These are stored in the `contas` map.
3.  **Transaction Handling (`GerenciadorTransacoes`):**
    * New transactions (`Transacao` objects) are created and added to the system via `gerTrans->adicionar()`. This function checks for duplicate transaction IDs and ensures the transaction date is valid relative to the account opening date.
    * Transactions are initially often in a "FUTURE" state.
    * `gerTrans->efetivarAte()` processes transactions up to a specified date, changing their status to "EFFECTIVE" and applying their values to the respective account balances by calling `conta->credito()` or `conta->debito()`.
4.  **Balance Inquiry (`ServicoConta`):**
    * `servicoConta->obterSaldo()` retrieves the balance for an account. It first ensures all relevant transactions up to the reference date are processed by calling `gerTrans->efetivarAte()`.
5.  **Statement Generation (`ServicoExtrato`):**
    * `servicoExtrato->gerarExtrato()` creates an `Extrato` object.
    * It calculates the opening balance by getting the account balance from the day before the statement period starts.
    * It fetches effective transactions within the period using `gerTrans->buscarPorPeriodo()`.
    * It calculates the running balance after each transaction and the final closing balance.
6.  **Data Persistence (`ServicoArquivos`):**
    * The `ServicoArquivos` class provides static methods to:
        * `salvarCSVContas()` and `salvarCSVTransacoes()`: Write account and transaction data to CSV files.
        * `carregarCSVContas()` and `carregarCSVTransacoes()`: Read data from CSV files (though loading is demonstrated more as a capability than used in `Main.cpp`'s primary flow).
        * `salvarBinario()` and `carregarBinario()`: Save and load the state of accounts and transactions using Boost.Serialization.
    * The `Main.cpp` example demonstrates saving the created accounts and transactions to `contas.csv` and `transacoes.csv`.

---
## Getting Started (Conceptual) 🚀

1.  **Environment Setup:**
    * A C++ compiler supporting C++11 or later (due to `std::chrono`, `std::shared_ptr`, etc.).
    * Boost libraries installed and correctly linked (specifically `boost::multiprecision` and `boost::serialization`).
    * The `rapidcsv.h` header file included in the project for CSV parsing.
2.  **Compilation:**
    * Compile all `.cpp` files (`Conta.cpp`, `Transacao.cpp`, `GerenciarTransacao.cpp`, `ServicoConta.cpp`, `ServicoExtrato.cpp`, `ServicoArquivos.cpp`, `Main.cpp`) and link against the Boost libraries.
3.  **Running the System:**
    * Execute the compiled program.
    * The `Main.cpp` will run a demo scenario: creating accounts, adding transactions, effecting them, printing balances and extracts to the console, and finally saving the data to `contas.csv` and `transacoes.csv`.
