#include <iostream>
#include <fstream>
using namespace std;
class HashMap {
private:
    class Node {
    public:
        string key;
        string value;
        Node* next;
        Node(string k, string v, Node* n = nullptr) : key(k), value(v), next(n) {}
    };
    int capacity;
    int elementCount;
    Node** table;
    int hashFunction(string key);
    
public:
    HashMap(int capacity = 100);
    ~HashMap();
    void insert(string k, string v);
    string* find(string key);
    string& operator[](string& key);
    void writeHashMapToFile(const string& filename);
    void readHashMapFromFile(const string& filename);
    int size();
};

