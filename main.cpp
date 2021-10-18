#include "Header.hpp"

int main(int argc, char const *argv[])
{
    vector<User *> users;
    for (int i = 0; i < 1000; i++)
    {
        users.push_back(new User("User" + to_string(i)));
    }
    User* user =  new User();
    users.push_back(user);
    vector<shared_ptr<Transaction>> transactions;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 999);
    std::uniform_real_distribution<double> dist2(1, 100000);
    std::ofstream rz;
    std::stringstream ss;
    for (int i = 0; i < 10000; i++)
    {
        int sender = dist(mt);
        int receiver = dist(mt);
        shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(users[sender]->getPublicKey(), users[receiver]->getPublicKey(), dist2(mt)));
        pointer->addSignature(users[sender]->Sign(pointer->CalculateTransactionHash()));
        transactions.push_back(pointer);
        ss << "Transaction " << i + 1 << endl;
        ss << "TransactionID: " << pointer->getTransactionID() << endl;
        ss << std::left << std::setw(18) << "Sender address: " << pointer->getSenderAddress() << endl;
        ss << std::left << std::setw(18) << "Receiver address: " << pointer->getReceiverAddress() << endl;
        ss << "Amount: " << std::fixed << std::setprecision(8) << pointer->getAmount() << endl;
        ss << "Signature: " << pointer->getSignature() << endl;
        ss << "---------------------------------------------------------" << endl;
    }
    rz.open("GeneratedTransactions.txt");
    rz << ss.rdbuf();
    rz.close();
    ss.clear();
    rz.open("BlockTransactions.txt",std::ios::trunc);
    rz.close();
    vector<User *> miners;
    for (int i = 0; i < 5; i++)
    {
        miners.push_back(new User("Miner"+to_string(i+1)));
        users.push_back(miners[i]);
    }
    rz.open("Blockchain.txt", std::ios::trunc);
    rz.close();
    Blockchain *blockchain = new Blockchain;
    blockchain->setpendingTransactions(transactions);
    for (int i = 0; i < 100; i++)
    {
        blockchain->addBlock(users,miners);
    }
    if (argc != 1)
    {
        cout << "Block " << argv[1]<<endl;
        std::stringstream ss;
        ss << argv[1];
        int index;
        ss >> index;
        cout << blockchain->getBlock(index);
    }
    delete blockchain;
    for (auto &i : users)
    {
        delete i;
    }
    return 0;
}
