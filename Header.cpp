#include "Header.hpp"

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
    if (transactions.size()==0)
    {
        return;
    }
    vector<double> userBalances;
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
        i->setExecuted(true);
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
MerkleTree::MerkleTree(const vector<shared_ptr<Transaction>>& transactions)
{
    if (transactions.size()==0)
    {
        shared_ptr<Node> pointer = make_shared<Node>(Node("null"));
        this->root = pointer;
        return;
    }
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
    this->Executed = false;
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
void Transaction::setExecuted(bool Executed)
{
    this->Executed = Executed;
}
string Block::CalculateHash()
{
    return hashFunction(this->getTimestamp() + this->getVersion() + std::to_string(this->getDifficultyTarget()) + this->getPreviousBlockHash() + std::to_string(this->getNonce()) + this->getMerkleRootHash());
}
string Block::CalculateMerkleRootHash()
{
    MerkleTree *merkleTree = new MerkleTree(this->getTransactions());
    string rootHash = merkleTree->getRoot()->getTxHash();
    delete merkleTree;
    return rootHash;
}

void Block::mineBlock(unsigned long long int max_hash_attempts)
{
    string difficulty = string(this->difficultyTarget, '0');
    unsigned long long int start = 0;
    while (this->hash.substr(0, this->difficultyTarget) != difficulty)
    {
        this->nonce++;
        this->hash = this->CalculateHash();
        start++;
        if (start==max_hash_attempts)
        {
            break;
        }
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
    this->hash =  this->CalculateHash();
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
std::ostream &operator<<(std::ostream &output, const Block* block)
{
    output << "Previous block hash: "<<block->getPreviousBlockHash()<<endl;
    output << "Hash: " << block->getBlockHash() << endl;
    output << "Timestamp: " << block->getTimestamp();
    output << "Number of transactions: " << block->getTransactions().size() << endl;
    output << "Difficulty: " << block->getDifficultyTarget() << endl;
    output << "Merkle root hash: " << block->getMerkleRootHash() << endl;
    output << "Version: " << block->getVersion() << endl;
    output << "Nonce: " << block->getNonce() << endl;
    double sum = 0;
    int n = 0;
    for (auto &i : block->getTransactions())
    {
        sum += i->getAmount();
        
    }
    output << "Transaction volume: " << std::fixed << std::setprecision(8) << sum << endl;
    output << "---------------------------------------------------------" << endl;
    return output;
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
    std::random_device rd;
    std::mt19937 mt(rd());
    vector<vector<shared_ptr<Transaction>>> t;
    User* user =  new User();
    user->setBalance(this->miningReward*5);
    vector<User *> miner;
    for (int i = 0; i < 5; i++)
    {
        miner.push_back(new User("Miner"+to_string(i+1)));
    }
    for (int i = 0; i < 5; i++)
    {
        shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(user->getPublicKey(), miner[i]->getPublicKey(), this->miningReward));
        pointer->addSignature(user->Sign(pointer->getTransactionHash()));
        vector<shared_ptr<Transaction>> temp{pointer};
        t.push_back(temp);
    }
    miner.push_back(user);
    vector<Block*> candidates;
    candidates.reserve(5);
    for (int i = 0; i < 5; i++)
    {
        validateTransactions(miner, t[i]);
        Block *genesis_block = new Block(t[i]);
        candidates.push_back(genesis_block);
    }
    int k;
    vector<Block*> candidates_after;
    candidates_after.reserve(5);
    omp_set_num_threads(5);
    unsigned long long int max_hash_attempts = 100000;
    while (candidates_after.size()==0)
    {
    #pragma omp parallel for private(k) schedule(static,1)
    for (k = 0; k < 5; k++)
    {
        candidates[k]->mineBlock(max_hash_attempts);
        if (candidates[k]->getBlockHash().substr(0, candidates[k]->getDifficultyTarget())==string(candidates[k]->getDifficultyTarget(), '0'))
        {
            #pragma omp critical
            {
                candidates_after.push_back(candidates[k]);
            }
            
        }
    }
    max_hash_attempts+=100000;
    }
    int index = 0;
    for (int i = 1; i < 5; i++)
    {
        if (candidates[i]==candidates_after[0])
        {
            index = i;
        }
    }
    candidates.clear();
    for (int i = 0; i < 5; i++)
    {
        if (candidates_after[i]!=candidates_after[0])
        {
            delete candidates_after[i];
        }
    }
    miner[index]->setBalance(miner[index]->getBalance()+t[index][0]->getAmount());
    std::stringstream ss,ss2;
    ss << "Block 0" << endl;
    ss << "Previous block hash: "<<candidates_after[0]->getPreviousBlockHash()<<endl;
    ss << "Hash: " << candidates_after[0]->getBlockHash() << endl;
    ss << "Timestamp: " << candidates_after[0]->getTimestamp();
    ss << "Height: "
       << "0" << endl;
    ss << "Miner: " << miner[index]->getName() << endl;
    ss << "Number of transactions: " << candidates_after[0]->getTransactions().size() << endl;
    ss << "Difficulty: " << candidates_after[0]->getDifficultyTarget() << endl;
    ss << "Merkle root hash: " << candidates_after[0]->getMerkleRootHash() << endl;
    ss << "Version: " << candidates_after[0]->getVersion() << endl;
    ss << "Nonce: " << candidates_after[0]->getNonce() << endl;
    double sum = 0;
    int n=0;
    std::ofstream rz;
    ss2 << "Block 0 transactions: "<< endl;
    for (auto &i : candidates_after[0]->getTransactions())
    {
        sum += i->getAmount();
        ss2 << "Transaction " << ++n << endl;
        ss2 << "TransactionID: " << i->getTransactionID() << endl;
        ss2 << std::left << std::setw(18) << "Sender address: " << i->getSenderAddress() << endl;
        ss2 << std::left << std::setw(18) << "Receiver address: " << i->getReceiverAddress() << endl;
        ss2 << "Amount: " << std::fixed << std::setprecision(8) << i->getAmount() << endl;
        ss2 << "Signature: " << i->getSignature() << endl;
        ss2 << "---------------------------------------------------------" << endl;
    }
    rz.open("BlockTransactions.txt", std::ios::app);
    rz << ss2.rdbuf();
    rz.close();
    ss << "Transaction volume: " << std::fixed << std::setprecision(8) << sum << endl;
    ss << "Block reward: " << std::fixed << std::setprecision(8) << this->getMiningReward() << endl;
    ss << "---------------------------------------------------------" << endl;
    rz.open("Blockchain.txt", std::ios::app);
    rz << ss.rdbuf();
    rz.close();
    this->getpendingTransactions().clear();
    for (auto &i:miner)
    {
        delete i;
    }
    return candidates_after[0];
}
void Blockchain::addBlock(vector<User *> &users,const vector<User *>& miners, string MinerName)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    users[users.size()-1]->setBalance(this->miningReward*5);
    vector<Block*> candidates;
    candidates.reserve(5);
    vector<vector<shared_ptr<Transaction>>> candidates_transactions;
    candidates_transactions.reserve(5);
    std::vector<std::shared_ptr<Transaction>> allTransactions = this->getpendingTransactions();
    for (int i = 0; i < 5; i++)
    {
        std::shuffle(allTransactions.begin(), allTransactions.end(), mt);
        vector<shared_ptr<Transaction>> selectedTransactions;
        for (int j = 0; j < 100; j++)
        {
            selectedTransactions.push_back(allTransactions[j]);
        }
        shared_ptr<Transaction> t = make_shared<Transaction>(Transaction(users[users.size()-1]->getPublicKey(), miners[i]->getPublicKey(), this->miningReward));
        t->addSignature(users[users.size()-1]->Sign(t->getTransactionHash()));
        selectedTransactions.push_back(t);
        candidates_transactions.push_back(selectedTransactions);
        validateTransactions(users, selectedTransactions);
        Block *newBlock = new Block(selectedTransactions, 2, "Version 1.0", this->chain[this->chain.size() - 1]->CalculateHash());
        candidates.push_back(newBlock);
        selectedTransactions.clear();
    }
    vector<Block*> candidates_after;
    candidates_after.reserve(5);
    int k;
    omp_set_num_threads(5);
    unsigned long long int max_hash_attempts = 100000;
    while (candidates_after.size()==0)
    {
    #pragma omp parallel for private(k, max_hash_attempts) schedule(static,1)
    for (k = 0; k < 5; k++)
    {
        candidates[k]->mineBlock(max_hash_attempts);
        if (candidates[k]->getBlockHash().substr(0, candidates[k]->getDifficultyTarget())==string(candidates[k]->getDifficultyTarget(), '0'))
        {
            #pragma omp critical
            {
                candidates_after.push_back(candidates[k]);
            }  
        }
    }
    max_hash_attempts+=100000;
    }
    int index = 0;
    for (int i = 1; i < 5; i++)
    {
        if (candidates[i]==candidates_after[0])
        {
            index = i;
        }
    }
    candidates.clear();
    std::stringstream ss, ss2;
    std::ofstream rz;
    ss << "Block " << this->chain.size() << endl;
    ss << "Previous block hash: "<<candidates_after[0]->getPreviousBlockHash()<<endl;
    ss << "Hash: " << candidates_after[0]->getBlockHash() << endl;
    ss << "Timestamp: " << candidates_after[0]->getTimestamp();
    ss << "Height: " << this->chain.size() << endl;
    ss << "Miner: " << miners[index]->getName() << endl;
    ss << "Number of transactions: " << candidates_after[0]->getTransactions().size() << endl;
    ss << "Difficulty: " << candidates_after[0]->getDifficultyTarget() << endl;
    ss << "Merkle root hash: " << candidates_after[0]->getMerkleRootHash() << endl;
    ss << "Version: " << candidates_after[0]->getVersion() << endl;
    ss << "Nonce: " << candidates_after[0]->getNonce() << endl;
    double sum = 0;
    int n = 0;
    ss2 << "Block " << this->chain.size() << " transactions: "<< endl;
    for (auto &i : candidates_after[0]->getTransactions())
    {
        sum += i->getAmount();
        ss2 << "Transaction " << ++n << endl;
        ss2 << "TransactionID: " << i->getTransactionID() << endl;
        ss2 << std::left << std::setw(18) << "Sender address: " << i->getSenderAddress() << endl;
        ss2 << std::left << std::setw(18) << "Receiver address: " << i->getReceiverAddress() << endl;
        ss2 << "Amount: " << std::fixed << std::setprecision(8) << i->getAmount() << endl;
        ss2 << "Signature: " << i->getSignature() << endl;
        ss2 << "---------------------------------------------------------" << endl;
    }
    rz.open("BlockTransactions.txt", std::ios::app);
    rz << ss2.rdbuf();
    rz.close();
    ss << "Transaction volume: " << std::fixed << std::setprecision(8) << sum << endl;
    ss << "Block reward: " << std::fixed << std::setprecision(8) << this->getMiningReward() << endl;
    ss << "---------------------------------------------------------" << endl;
    rz.open("Blockchain.txt", std::ios::app);
    rz << ss.rdbuf();
    rz.close();
    for (auto &i : candidates_after[0]->getTransactions())
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
        i->setExecuted(true);
    }
    this->chain.push_back(candidates_after[0]);
    this->deleteExecutedTransactions(candidates_transactions,candidates_transactions[index]);
    this->setMiningReward();
    for (int i = 0; i < 5; i++)
    {
        if (candidates_after[i]!=candidates_after[0])
        {
            delete candidates_after[i];
        }
    }
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
Block* Blockchain::getBlock(int n)
{
    try
    {
        if (n > this->chain.size() - 1|| n < 0)
        {
            throw "Block doesn't exist";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    return this->chain[n];
}
void Blockchain::deleteExecutedTransactions(vector<vector<shared_ptr<Transaction>>>& selectedTransactions,vector<shared_ptr<Transaction>>& addedToBlockTransactions)
{
    for (auto &i:selectedTransactions)
    {
        i.pop_back();
    }
    int idx =0;
    for (auto &i:selectedTransactions)
    {
        if (i!=addedToBlockTransactions)
        {
        for (auto &j:i)
        {
        j->setExecuted(false);
        }
        }
        idx++;
    }
    for (auto &i:addedToBlockTransactions)
    {
        i->setExecuted(true);
    }
    int size = this->getpendingTransactions().size() - 1;
    while (size != -1)
    {
        if (this->getpendingTransactions()[size]->getExecuted())
        {
            auto it = std::find(this->getpendingTransactions().begin(), this->getpendingTransactions().end(), this->getpendingTransactions()[size]);
            this->getpendingTransactions().erase(it);
            size = this->getpendingTransactions().size() - 1;
            continue;
        }
        size--;
    }
}