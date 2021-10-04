#ifndef HEADER_HPP
#define HEADER_HPP
#define A 5870873747 //pirminiai skaiciai
#define B 6863349307
#define C 9698977879
#define seed 4033081603

#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>
#include <vector>

using std::string;
using std::vector;

string hashFunction(string text);

class Transaction
{
private:
    string SenderAddress; 
    string ReceiverAddress;
    string transactionID;
    double amount;
public:
    Transaction(string SenderAddress, string ReceiverAddress, double amount);
    inline string getTransactionHash();
    ~Transaction();
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
    vector<Transaction*> transactions;
public:
    Block(int difficultyTarget = 2, string version = "Version 1.0",string previous_hash = "0");
    string CalculateHash();
    string getMerkleRootHash();
    void mineBlock();
    ~Block(){}
};


class Blockchain
{
private:
    vector<Block*> chain;
public:
    Blockchain();
    Block* CreateGenesisBlock();
    void addBlock()
    {

    }
    ~Blockchain()
    {
        for (auto&i:chain)
        {
            delete i;
        }
    }
};
#endif