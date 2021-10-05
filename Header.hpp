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
#include <memory>

using std::string;
using std::vector;
using std::cout;
using std::shared_ptr;
using std::make_shared;
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
    inline string getTransactionHash()
    {
        return this->transactionID;
    }
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
    vector<Transaction*> transactions;

public:
    Block();
    Block(const vector<Transaction*>& transactions, int difficultyTarget = 2, string version = "Version 1.0",string previous_hash = "0");
    string CalculateHash();
    string getMerkleRootHash();
    void mineBlock();
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
    MerkleTree(const vector<Transaction*> transactions);
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
public:
    Blockchain();
    Block* CreateGenesisBlock();
    void addBlock(string MinerPublicKey);
    
    ~Blockchain();
};
#endif