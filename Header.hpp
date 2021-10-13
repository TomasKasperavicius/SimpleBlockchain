#ifndef HEADER_HPP
#define HEADER_HPP
#define PrimeNumber1 5870873747 //pirminiai skaiciai
#define PrimeNumber2 6863349307
#define PrimeNumber3 9698977879
#define PrimeNumberSeed 4033081603
#include "User.hpp"
#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <iomanip>

using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

class Transaction
{
private:
    string SenderAddress;
    string ReceiverAddress;
    RSASS<PSS, SHA256>::Verifier verifier;
    string signature;
    string transactionID;
    double amount;
    bool TransactionError;

public:
    Transaction(string SenderAddress, string ReceiverAddress, double amount, string signature = "none");
    inline bool const getTransactionError() const
    {
        return this->TransactionError;
    }
    inline const string getSenderAddress() const
    {
        return this->SenderAddress;
    }
    inline const string getReceiverAddress() const
    {
        return this->ReceiverAddress;
    }
    inline const string getSignature() const
    {
        return this->signature;
    }
    inline const double getAmount() const
    {
        return this->amount;
    }
    inline const string getTransactionID() const
    {
        return this->transactionID;
    }
    string getTransactionHash();
    bool verifyTransaction();
    void addSignature(string signature);
    void setAmount(double amount);
    void setTransactionError(bool TransactionError);
    ~Transaction() {}
};
class Block
{
private:
    // Block header
    string hash;
    string previous_hash;
    string timestamp;
    string version;
    string MerkleRootHash;
    unsigned long long int nonce;
    int difficultyTarget;
    // Block body (Transactions)
    vector<shared_ptr<Transaction>> transactions;

public:
    Block();
    Block(const vector<shared_ptr<Transaction>> &transactions, int difficultyTarget = 2, string version = "Version 1.0", string previous_hash = "0");
    inline const string getBlockHash() const
    {
        return this->hash;
    }
    inline const string getPreviousBlockHash() const
    {
        return this->previous_hash;
    }
    inline const string getTimestamp() const
    {
        return this->timestamp;
    }
    inline const string getVersion() const
    {
        return this->version;
    }
    inline const string getMerkleRootHash() const
    {
        return this->MerkleRootHash;
    }
    inline const int getDifficultyTarget() const
    {
        return this->difficultyTarget;
    }
    inline const unsigned long long int getNonce() const
    {
        return this->nonce;
    }
    inline const vector<shared_ptr<Transaction>> &getTransactions() const
    {
        return this->transactions;
    }
    string CalculateHash();
    string CalculateMerkleRootHash();
    void mineBlock();
    bool allTransactionsValid();
    friend std::ostream &operator<<(std::ostream &output, const Block* block);
    ~Block();
};
class Node
{
private:
    string TxHash;
    shared_ptr<Node> left;
    shared_ptr<Node> right;

public:
    Node(string TxHash);
    inline string getTxHash()
    {
        return this->TxHash;
    }
    inline shared_ptr<Node> getLeft()
    {
        return this->left;
    }
    inline shared_ptr<Node> getRight()
    {
        return this->right;
    }
    void setLeft(shared_ptr<Node> node)
    {
        this->left = node;
    }
    void setRight(shared_ptr<Node> node)
    {
        this->right = node;
    }
    ~Node()
    {
    }
};
class MerkleTree
{
private:
    shared_ptr<Node> root;

public:
    MerkleTree(const vector<shared_ptr<Transaction>> &transactions);
    void TraverseMerkleTree(shared_ptr<Node> root);
    inline shared_ptr<Node> getRoot()
    {
        return this->root;
    }
    ~MerkleTree() {}
    friend Block;
};
class Blockchain
{
private:
    vector<Block *> chain;
    double miningReward = 5000.00;
    vector<shared_ptr<Transaction>> pendingTransactions;

public:
    Blockchain();
    inline vector<shared_ptr<Transaction>> &getpendingTransactions()
    {
        return this->pendingTransactions;
    }
    inline const double getMiningReward() const
    {
        return this->miningReward;
    }
    void setpendingTransactions(vector<shared_ptr<Transaction>> &pendingTransactions);
    Block *CreateGenesisBlock();
    void addBlock(const vector<User *> &users, User *miner, string MinerName = "Miner");
    void setMiningReward();
    bool isBlockChainValid();
    Block* getBlock(int n);
    ~Blockchain();
};
string hashFunction(string text);
void validateTransactions(const vector<User *> &users, vector<shared_ptr<Transaction>> &transactions);
#endif