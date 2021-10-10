#include "User.hpp"



User::User(std::string Name)
{
    this->Name = Name;
    AutoSeededRandomPool rng(true);
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 1024);
    this->privateKey = (params);
    this->publicKey = (params);
    this->signer = (this->privateKey);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist (100, 1000000);
    this->Balance = dist(mt);
}
void User::setBalance(double Balance)
{
    this->Balance = Balance;
}
std::string User::Sign(std::string transactionID)
{
    AutoSeededRandomPool rng(true);
    std::string signature;
    CryptoPP::StringSource ss1(transactionID, true, new CryptoPP::SignerFilter(rng, this->signer,new CryptoPP::StringSink(signature))); 
    return signature;
}
User::~User()
{
}