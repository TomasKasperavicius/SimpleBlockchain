#include "Header.hpp"

int main()
{
    vector<User *> users;
    for (int i = 0; i < 1000; i++)
    {
        users.push_back(new User("User" + to_string(i)));
    }
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
        pointer->addSignature(users[sender]->Sign(pointer->getTransactionHash()));
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
    User *miner = new User("Miner");
    users.push_back(miner);
    rz.open("Results.txt", std::ios::trunc);
    rz.close();
    Blockchain *blockchain = new Blockchain;
    do
    {
        std::shuffle(transactions.begin(), transactions.end(), mt);
        vector<shared_ptr<Transaction>> selectedTransactions;
        for (int i = 0; i < 100; i++)
        {
            selectedTransactions.push_back(transactions[i]);
        }
        blockchain->setpendingTransactions(selectedTransactions);
        blockchain->addBlock(users, miner);
        transactions.erase(transactions.begin(), transactions.begin() + 100);
        selectedTransactions.clear();
    } while (transactions.size() != 0);
    users.pop_back();
    delete miner;
    delete blockchain;
    for (auto &i : users)
    {
        delete i;
    }
    return 0;
}
