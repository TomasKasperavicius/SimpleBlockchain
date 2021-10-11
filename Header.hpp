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

using std::to_string;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;

class Transaction
{
private:
    string SenderAddress; 
    string ReceiverAddress;
    RSASS<PSS, SHA256>::Verifier verifier;
    string signature;
    string transactionID;
    double amount;
    bool BalanceError;
public:
    Transaction(string SenderAddress, string ReceiverAddress, double amount,double Balance, string signature = "none");
    inline bool getBalanceError() const{
        return this->BalanceError;
    }
    inline string getSenderAddress()
    {
        return this->SenderAddress;
    }
    inline string getReceiverAddress()
    {
        return this->ReceiverAddress;
    }
    inline double getAmount()
    {
        return this->amount;
    }
    string getTransactionHash();
    bool verifyTransaction();
    void addSignature(string signature);
    void setAmount(double amount);
    void setBalanceError(bool BalanceError);
    ~Transaction(){}
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
    Block(const vector<shared_ptr<Transaction>>& transactions, int difficultyTarget = 2, string version = "Version 1.0",string previous_hash = "0");
    inline const string getBlockHash() const{
        return this->hash;
    }
    inline const string getPreviousBlockHash() const{
        return this->previous_hash;
    }
    inline const string getTimestamp()const{
        return this->timestamp;
    }
    inline const string getMerkleRootHash() const{
        return this->MerkleRootHash;
    }
    inline const int getDifficultyTarget()const{
        return this->difficultyTarget;
    }
    inline const unsigned long long int getNonce()const{
        return this->nonce;
    }
    inline vector<shared_ptr<Transaction>>& getTransactions(){
        return this->transactions;
    }
    string CalculateHash();
    string CalculateMerkleRootHash();
    void mineBlock();
    bool allTransactionsValid();
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
    inline string getTxHash(){
        return this->TxHash;
    }
    inline shared_ptr<Node> getLeft(){
        return this->left;
    }
    inline shared_ptr<Node> getRight(){
        return this->right;
    }
    void setLeft(shared_ptr<Node> node)
    {
        this->left = node;
    }
    void setRight(shared_ptr<Node> node){
        this->right = node;
    }
    ~Node(){
        
    }
    
};
class MerkleTree
{
private:
    shared_ptr<Node> root;
public:
    MerkleTree(const vector<shared_ptr<Transaction>> transactions);
    void TraverseMerkleTree(shared_ptr<Node> root);
    inline shared_ptr<Node> getRoot(){
        return this->root;
    }
    ~MerkleTree(){}
    friend Block;
};
class Blockchain
{
private:
    vector<Block*> chain;
    double miningReward = 5000.00;
    vector<shared_ptr<Transaction>> pendingTransactions;
public:
    Blockchain();
    inline vector<shared_ptr<Transaction>>& getpendingTransactions()
    {
        return this->pendingTransactions;
    }
    void setpendingTransactions(vector<shared_ptr<Transaction>>& pendingTransactions);
    Block* CreateGenesisBlock();
    void addBlock(const vector<User*>& users,string MinerPublicKey,string MinerName="Miner");
    void setMiningReward();
    void validateTransactions();
    bool isBlockChainValid();
    ~Blockchain();
};
string hashFunction(string text);
void balanceCheck(const vector<User*>& users, vector<shared_ptr<Transaction>>& transactions);
#endif