#include "Header.hpp"
#include <fstream>

int main()
{
    std::ofstream rz;
    vector<User*> users;
    std::stringstream ss;
    for (int i = 0; i < 1000; i++)
    {
        string  name = "User"+to_string(i);
        users.push_back(new User(name));
    }
    for (auto &i: users)
    {
        ss << i->getName() << " " <<i->getBalance() << endl;
    }
    rz.open("StartingData.txt");
    rz << ss.rdbuf();
    rz.close();
    ss.clear();
    vector<shared_ptr<Transaction>> transactions;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist (0, 999);
    std::uniform_real_distribution<double> dist2 (0, 100000);
    for (int i = 0; i < 10000; i++)
    {
        int sender = dist(mt);
        int receiver = dist(mt);
        shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(users[sender]->getPublicKey(),users[receiver]->getPublicKey(),dist2(mt),users[sender]->getBalance()));
        pointer->addSignature(users[sender]->Sign(pointer->getTransactionHash()));
        transactions.push_back(pointer);
    }
    User* miner = new User("Miner");
    users.push_back(miner);
    cout << miner->getName() << " "<< miner->getBalance() << endl;
    Blockchain* blockchain = new Blockchain;
    int ts =0;
    do
    {
    std::shuffle(transactions.begin(),transactions.end(),mt);
    vector<shared_ptr<Transaction>> selectedTransactions;
    for (int i = 0; i < 100; i++)
    {
        selectedTransactions.push_back(transactions[i]);
    }
    balanceCheck(users, selectedTransactions);
    blockchain->setpendingTransactions(selectedTransactions);
    blockchain->addBlock(users,miner->getPublicKey());
    transactions.erase(transactions.begin(),transactions.begin()+100);
    selectedTransactions.clear();
    } while (transactions.size()!=0);
    cout << miner->getName() << " "<< miner->getBalance() << endl;
    for (auto &i: users)
    {
        ss << i->getName() << " " <<i->getBalance() << endl;
    }
    rz.open("ResultData.txt");
    rz << ss.rdbuf();
    rz.close();
    ss.clear();
    users.pop_back();
    delete miner;
    delete blockchain;
    for (auto &i: users)
    {
        delete i;
    }
    return 0;
}
