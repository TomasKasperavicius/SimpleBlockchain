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
    string fromAddress; 
    string toAddress; 
    double amount;
public:
    Transaction(string fromAddress, string toAddress, double amount);
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
    string MerkelRootHash;
    unsigned long long int nonce;
    int difficultyTarget;
    // Block body (Transactions)
    vector<Transaction*> transactions;
public:
    Block(int difficultyTarget = 2, string version = "Version 1.0",string previous_hash = "0");
    string CalculateHash();
    string getMerkelRootHash();
    void mineBlock()
    {
        
    }
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