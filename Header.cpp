#include "Header.hpp"
#include <deque>

string hashFunction(string text)
{
    unsigned long long h = PrimeNumberSeed;
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
        h = (h * PrimeNumber1) ^ (text[i] * PrimeNumber2);
        ss << std::hex << h % PrimeNumber3;
    }
    string hash = "";
    string all_chars = ss.str();
    for (int i = all_chars.length() - 1; i > all_chars.length() - 65; i--)
    {
        hash += all_chars[i];
    }
    return hash;
}
void validateTransactions(const vector<User *> &users, vector<shared_ptr<Transaction>> &transactions)
{
    vector<double> userBalances;
    int detected = 0;
    for (auto &i : users)
    {
        userBalances.push_back(i->getBalance());
    }
    for (auto &i : transactions)
    {
        int index = 0;
        for (auto &j : users)
        {

            if (j->getPublicKey() == i->getSenderAddress())
            {
                if (userBalances[index] - i->getAmount() < 0 || !i->verifyTransaction())
                {
                    i->setTransactionError(true);
                    break;
                }
                userBalances[index] -= i->getAmount();
            }
            index++;
        }
    }
    int size = transactions.size() - 1;
    while (size != -1)
    {
        if (transactions[size]->getTransactionError())
        {
            auto it = std::find(transactions.begin(), transactions.end(), transactions[size]);
            transactions.erase(it);

            size = transactions.size() - 1;
            continue;
        }
        size--;
    }
}
Node::Node(string TxHash)
{
    this->left = NULL;
    this->right = NULL;
    this->TxHash = TxHash;
}
MerkleTree::MerkleTree(const vector<shared_ptr<Transaction>> &transactions)
{
    vector<shared_ptr<Node>> allTransactionsNodes;

    for (auto &i : transactions)
    {
        shared_ptr<Node> pointer = make_shared<Node>(Node(i->getTransactionHash()));
        allTransactionsNodes.push_back(pointer);
    }
    vector<shared_ptr<Node>> temp;
    while (allTransactionsNodes.size() != 1)
    {

        if (allTransactionsNodes.size() % 2 != 0)
        {
            shared_ptr<Node> pointer = make_shared<Node>(Node(allTransactionsNodes.at(allTransactionsNodes.size() - 1)->getTxHash()));
            allTransactionsNodes.push_back(pointer);
        }
        for (int i = 0, n = 0; i < allTransactionsNodes.size(); i += 2, n++)
        {
            shared_ptr<Node> pointer = make_shared<Node>(Node(hashFunction(allTransactionsNodes[i]->getTxHash() + allTransactionsNodes[i + 1]->getTxHash())));
            temp.push_back(pointer);
            temp[n]->setLeft(allTransactionsNodes[i]);
            temp[n]->setRight(allTransactionsNodes[i + 1]);
        }
        allTransactionsNodes = temp;
        temp.clear();
    }
    this->root = allTransactionsNodes[0];
}
void MerkleTree::TraverseMerkleTree(shared_ptr<Node> root)
{
    if (root)
    {
        cout << root->getTxHash() << '\n';
        TraverseMerkleTree(root->getLeft());
        TraverseMerkleTree(root->getRight());
    }
}

// Transaction constructor
Transaction::Transaction(string SenderAddress, string ReceiverAddress, double amount, string signature)
{
    this->SenderAddress = SenderAddress;
    this->ReceiverAddress = ReceiverAddress;
    RSA::PublicKey publicKey;
    publicKey.Load(StringSource(SenderAddress, true, new HexDecoder()).Ref());
    this->verifier = (publicKey);
    this->amount = amount;
    this->signature = signature;
    this->transactionID = hashFunction(SenderAddress + ReceiverAddress + std::to_string(this->amount));
    this->TransactionError = false;
}
string Transaction::getTransactionHash()
{
    this->transactionID = hashFunction(this->SenderAddress + this->ReceiverAddress + std::to_string(this->amount));
    return this->transactionID;
}
bool Transaction::verifyTransaction()
{
    bool result = false;
    string decodedSignature;
    StringSource ss(this->signature, true, new HexDecoder(new StringSink(decodedSignature)));
    try
    {
        StringSource ss2(decodedSignature + this->getTransactionHash(), true, new SignatureVerificationFilter(this->verifier, new ArraySink((byte *)&result, sizeof(result))));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return result;
    }
    return result;
}
void Transaction::addSignature(string signature)
{
    this->signature = signature;
}
void Transaction::setAmount(double amount)
{
    this->amount = amount;
}
void Transaction::setTransactionError(bool TransactionError)
{
    this->TransactionError = TransactionError;
}
string Block::CalculateHash()
{
    return hashFunction(timestamp + version + std::to_string(difficultyTarget) + previous_hash + std::to_string(nonce) + this->CalculateMerkleRootHash());
}
string Block::CalculateMerkleRootHash()
{
    MerkleTree *merkleTree = new MerkleTree(this->getTransactions());
    string rootHash = merkleTree->getRoot()->getTxHash();
    delete merkleTree;
    return rootHash;
}

void Block::mineBlock()
{
    string difficulty = string(this->difficultyTarget, '0');
    while (this->hash.substr(0, this->difficultyTarget) != difficulty)
    {
        this->nonce++;
        this->hash = this->CalculateHash();
    }
}
// Block constructor
Block::Block(const vector<shared_ptr<Transaction>> &transactions, int difficultyTarget, string version, string previous_hash)
{
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    this->timestamp = ctime(&time);
    this->version = version;
    this->difficultyTarget = difficultyTarget;
    this->previous_hash = previous_hash;
    this->nonce = 0;
    this->transactions = transactions;
    this->MerkleRootHash = this->CalculateMerkleRootHash();
    this->hash = CalculateHash();
}
bool Block::allTransactionsValid()
{
    for (auto &i : this->getTransactions())
    {
        if (!i->verifyTransaction()||i->getTransactionError())
        {
            return false;
        }
    }
    return true;
}
Block::~Block()
{
}
// Blockchain constructor
Blockchain::Blockchain()
{
    this->chain.push_back(this->CreateGenesisBlock());
}
Blockchain::~Blockchain()
{
    for (auto &i : this->chain)
    {
        delete i;
    }
}
void Blockchain::setpendingTransactions(vector<shared_ptr<Transaction>> &pendingTransactions)
{
    this->pendingTransactions = pendingTransactions;
}
Block *Blockchain::CreateGenesisBlock()
{
    vector<shared_ptr<Transaction>> t;
    User user1;
    vector<User *> miner;
    miner.push_back(new User("Miner"));
    shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(user1.getPublicKey(), miner[0]->getPublicKey(), 200.00));
    pointer->addSignature(pointer->getTransactionHash());
    t.push_back(pointer);
    Block *genesis_block = new Block(t);
    genesis_block->mineBlock();
    std::stringstream ss;
    ss << "Block 0" << endl;
    ss << "Previous block hash: "<<genesis_block->getPreviousBlockHash()<<endl;
    ss << "Hash: " << genesis_block->getBlockHash() << endl;
    ss << "Timestamp: " << genesis_block->getTimestamp();
    ss << "Height: "
       << "0" << endl;
    ss << "Miner: " << miner[0]->getName() << endl;
    ss << "Number of transactions: " << genesis_block->getTransactions().size() << endl;
    ss << "Difficulty: " << genesis_block->getDifficultyTarget() << endl;
    ss << "Merkle root hash: " << genesis_block->getMerkleRootHash() << endl;
    ss << "Version: " << genesis_block->getVersion() << endl;
    ss << "Size: " << sizeof(genesis_block) << endl;
    ss << "Nonce: " << genesis_block->getNonce() << endl;
    double sum = 0;
    for (auto &i : genesis_block->getTransactions())
    {
        sum += i->getAmount();
    }
    ss << "Transaction volume: " << std::fixed << std::setprecision(8) << sum << endl;
    ss << "Block reward: " << std::fixed << std::setprecision(8) << this->getMiningReward() << endl;
    ss << "---------------------------------------------------------" << endl;
    std::ofstream rz;
    rz.open("Results.txt", std::ios::app);
    rz << ss.rdbuf();
    rz.close();
    validateTransactions(miner, t);
    this->getpendingTransactions().clear();
    delete miner[0];
    return genesis_block;
}
void Blockchain::addBlock(const vector<User *> &users, User *miner, string MinerName)
{
    User user;
    user.setBalance(this->miningReward);
    shared_ptr<Transaction> t = make_shared<Transaction>(Transaction(user.getPublicKey(), miner->getPublicKey(), this->miningReward));
    t->addSignature(user.Sign(t->getTransactionHash()));
    this->getpendingTransactions().push_back(t);
    validateTransactions(users, this->pendingTransactions);
    Block *newBlock = new Block(this->pendingTransactions, 2, "Version 1.0", this->chain[this->chain.size() - 1]->CalculateHash());
    newBlock->mineBlock();
    std::stringstream ss;
    ss << "Block " << this->chain.size() << endl;
    ss << "Previous block hash: "<<newBlock->getPreviousBlockHash()<<endl;
    ss << "Hash: " << newBlock->getBlockHash() << endl;
    ss << "Timestamp: " << newBlock->getTimestamp();
    ss << "Height: " << this->chain.size() << endl;
    ss << "Miner: " << miner->getName() << endl;
    ss << "Number of transactions: " << newBlock->getTransactions().size() << endl;
    ss << "Difficulty: " << newBlock->getDifficultyTarget() << endl;
    ss << "Merkle root hash: " << newBlock->getMerkleRootHash() << endl;
    ss << "Version: " << newBlock->getVersion() << endl;
    ss << "Size: " << sizeof(newBlock) << endl;
    ss << "Nonce: " << newBlock->getNonce() << endl;
    double sum = 0;
    for (auto &i : newBlock->getTransactions())
    {
        sum += i->getAmount();
    }
    ss << "Transaction volume: " << std::fixed << std::setprecision(8) << sum << endl;
    ss << "Block reward: " << std::fixed << std::setprecision(8) << this->getMiningReward() << endl;
    ss << "---------------------------------------------------------" << endl;
    std::ofstream rz;
    rz.open("Results.txt", std::ios::app);
    rz << ss.rdbuf();
    rz.close();
    for (auto &i : newBlock->getTransactions())
    {
        for (auto &j : users)
        {
            if (j->getPublicKey() == i->getSenderAddress())
            {
                j->setBalance(j->getBalance() - i->getAmount());
            }
            if (j->getPublicKey() == i->getReceiverAddress())
            {
                j->setBalance(j->getBalance() + i->getAmount());
            }
        }
    }
    this->getpendingTransactions().clear();
    this->chain.push_back(newBlock);
    this->setMiningReward();
}
void Blockchain::setMiningReward()
{
    if (this->chain.size() % 20 == 0)
    {
        this->miningReward /= 2.0;
    }
    if (this->miningReward < 1)
    {
        this->miningReward = 500.0;
    }
}

bool Blockchain::isBlockChainValid()
{
    string previousHash;
    bool firstBlock = true;
    for (auto &i : this->chain)
    {
        if (firstBlock)
        {
            previousHash = this->chain[0]->CalculateHash();
            firstBlock = false;
            continue;
        }
        if (!i->allTransactionsValid())
        {
            return false;
        }
        if (previousHash != i->getPreviousBlockHash())
        {
            return false;
        }
        if (i->getBlockHash() != i->CalculateHash())
        {
            return false;
        }
        previousHash = i->CalculateHash();
    }
    return true;
}
