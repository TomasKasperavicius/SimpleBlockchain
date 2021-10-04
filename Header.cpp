#include "Header.hpp"


string hashFunction(string text)
{
    unsigned long long h = seed;
    if (text.length() == 0)
    {
        return "ac5171ee1fcf8ad4a986e4d6ab3c0e3c2eb38b79fb557376ea497bee17f8dd7e"; //random hashas
    }
    if (text.length() < 32)
    {
        unsigned long long num = 263239;
        for (int i = 0; i < text.length(); i++)
        {
            num = (num * 5) ^ (text[i]);
        }
        srand(num);
        for (int i = text.length() - 1; i < 32; i++)
        {
            text += std::to_string(rand() % num);
        }
    }
    std::stringstream ss;
    for (int i = 0; i < text.length(); i++)
    {
        h = (h * A) ^ (text[i] * B);
        ss << std::hex << h % C;
    }
    string hash = "";
    string all_chars = ss.str();
    for (int i = all_chars.length() - 1; i > all_chars.length() - 65; i--)
    {
        hash += all_chars[i];
    }
    return hash;
}

// Transaction constructor
Transaction::Transaction(string SenderAddress, string ReceiverAddress, double amount)
{
    this->SenderAddress = SenderAddress;
    this->ReceiverAddress = ReceiverAddress;
    this->amount = amount;
    this->transactionID = hashFunction(this->SenderAddress+this->ReceiverAddress+std::to_string(this->amount));
}
string Transaction::getTransactionHash()
{
    return this->transactionID;
}
string Block::CalculateHash()
{
    return hashFunction(timestamp+version+std::to_string(difficultyTarget)+previous_hash+std::to_string(nonce)+MerkleRootHash);
}
string Block::getMerkleRootHash()
{
    vector<string> TxHashes;
    if (this->transactions.size()%2!=0)
    {
        
        for (int i = 0; i < this->transactions.size(); i++)
        {
            TxHashes.push_back(this->transactions.at(i)->getTransactionHash());
        }
        TxHashes.push_back(this->transactions.at(this->transactions.size()-1)->getTransactionHash());
    }
    else
    {
        for (int i = 0; i < this->transactions.size(); i++)
        {
            TxHashes.push_back(this->transactions.at(i)->getTransactionHash());
        }
    }
    vector<string> temp;
    while (TxHashes.size() != 1)
    {
        if (TxHashes.size() %2!=0)
        {
            TxHashes.push_back(TxHashes.at(TxHashes.size()-1));
        }
        for (int i = 0; i < TxHashes.size(); i+=2)
        {
            temp.push_back(hashFunction(TxHashes[i]+TxHashes[i+1]));
        }
        TxHashes = temp;
        temp.clear();
    }
    return TxHashes[0];
}
void Block::mineBlock()
{
    string difficulty = string(this->difficultyTarget,'0');
    while (this->hash.substr(0,this->difficultyTarget)!=difficulty)
    {
        this->nonce++;
        this->hash = this->CalculateHash();
    }
    
}
// Block constructor
Block::Block(int difficultyTarget, string version,string previous_hash)
{
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    this->timestamp = ctime(&time);
    this->version = version;
    this->difficultyTarget = difficultyTarget;
    this->previous_hash = previous_hash;
    this->nonce = 0;
    this->hash = CalculateHash();
}
// Blockchain constructor
Blockchain::Blockchain()
{
    this->chain.push_back(this->CreateGenesisBlock());
}
Block* Blockchain::CreateGenesisBlock(){
    Block* genesis_block = new Block ();
    return genesis_block;
}
