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
Transaction::Transaction(string fromAddress, string toAddress, double amount)
{

}
string Block::CalculateHash()
{
    return hashFunction(timestamp+version+std::to_string(difficultyTarget)+previous_hash+std::to_string(nonce)+MerkelRootHash);
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
