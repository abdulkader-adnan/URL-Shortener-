#include "HashMap.h"

HashMap::HashMap(int capacity) : capacity(capacity), elementCount(0) {
    table = new Node * [capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
}
HashMap::~HashMap() {
    for (int i = 0; i < capacity; i++) {
        Node* current = table[i];
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}
int HashMap::hashFunction(string key) {
    int hashindex = 0;
    for (char ch : key) {
        hashindex = (hashindex * 31 + ch) % capacity;
    }
    return hashindex;
}
void HashMap::insert(string k, string v) {
    int index = hashFunction(k);
    Node* current = table[index];
    while (current != nullptr) {
        if (current->key == k) {
            current->value = v;
            return;
        }
        current = current->next;
    }
    Node* newNode = new Node(k, v, table[index]);
    table[index] = newNode;
    ++elementCount;
}
string* HashMap::find(string key) {
    int index = hashFunction(key);
    Node* current = table[index];
    while (current != nullptr) {
        if (current->key == key) {
            return &current->value;
        }
        current = current->next;
    }
    return nullptr;
}
string& HashMap::operator[](string& key) {
    int index = hashFunction(key);
    Node* current = table[index];

    while (current != nullptr) {
        if (current->key == key)
            return current->value;
        current = current->next;
    }

    insert(key, "");
    return *(find(key));
}
int HashMap::size() {
    return elementCount;
}
void HashMap::writeHashMapToFile(const string& filename) {
    ofstream outFile(filename);
    for (int i = 0; i < capacity; i++) {
        Node* current = table[i];
        while (current != nullptr) {
            outFile << current->key << " " << current->value << "\n";
            current = current->next;
        }
    }
    outFile.close();
    cout << "HashMap written to file: " << filename << endl;
}
void HashMap::readHashMapFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }
    string key, value;
    while (inFile >> key >> value) {
        insert(key, value);
    }
    inFile.close();
}
