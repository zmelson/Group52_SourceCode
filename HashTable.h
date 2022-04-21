//Maps destination airport to set of flights with that dest
//Utilizing fact that no removal from table to simplify
#pragma once
#include <queue>
#include <unordered_set>
#include <string>
#include <iostream>
#include "flightNode.h"
#define MAX_LOAD_FACTOR 0.75
#define INITIAL_BUCKET_COUNT 7
using namespace std;

class HashTable {
public:
    struct HashNode {
        unordered_set<flightNode*> flights;
        string dest;
        bool hasData;
        HashNode() {
            hasData = false;
            dest = "";
        }
    };
    HashTable();
    ~HashTable();
    void insert(unordered_set<flightNode*>& flightSet);
    int size();
    unordered_set<flightNode*> search(string dest);
private:
    int bucketCount;
    int numElements;
    HashNode* values;
    int hash(string dest);
    void resize();
    bool present(string dest);
    
};

//---------------HASHTABLE PRIVATE FUNCTIONS-------------
int HashTable::hash(string dest) {
    //temp hash function, can replace
    int sum = 0;
    int mult = 1;
    for (int i = 0; i < dest.size(); i++) {
        mult *= 31;
        sum += mult * (dest.at(i) - 'A');
    }
    return sum % bucketCount;
}

void HashTable::resize() {
    //cout << "resizing...\n";
    queue<unordered_set<flightNode*>> elements;
    for (int i = 0; i < bucketCount; i++) {
        if (values[i].hasData)
            elements.push(values[i].flights);         
    }
    delete[] values;
    bucketCount*=2;
    numElements = 0;
    //cout << "New bucket count: " << bucketCount << "\n";
    values = new HashNode[bucketCount];
    while (!elements.empty()) {
        unordered_set<flightNode*> current = elements.front();
        insert(current);
        elements.pop();
    }
}

HashTable::HashTable() {
    bucketCount = INITIAL_BUCKET_COUNT;
    numElements = 0;
    values = new HashNode[bucketCount];
}

bool HashTable::present(string dest) {
    int start = hash(dest);
    for (int i = 0; i < bucketCount; i++) {
        int curr = (start + i) % bucketCount;
        if (values[curr].hasData && values[curr].dest == dest)
            return true;
    }
    return false;
}

//---------------HASHTABLE PUBLIC FUNCTIONS-------------
int HashTable::size() {
    return numElements;
}

void HashTable::insert(unordered_set<flightNode*>& flightSet) {
    //cout << "Attempting to add set to table.\n";
    if (flightSet.size() == 0)
        return;
    auto first = flightSet.begin();
    flightNode* f = *first;
    string dest = f->dest;
    int startIndex = hash(dest);
    bool reinserting = present(dest);
    for (int i = 0; i < bucketCount; i++) {
        int curr = (startIndex + i) % bucketCount;
        if (reinserting && values[curr].hasData && values[curr].dest == dest) {
            //cout << "found location, replacing old data\n";
            values[curr].flights = flightSet;
        }
        else if (!reinserting && !values[curr].hasData) {
            //cout << "Added set to table.\n";
            values[curr].hasData = true;
            values[curr].dest = dest;
            values[curr].flights = flightSet;
            numElements++;
            if ((numElements / bucketCount) >= MAX_LOAD_FACTOR) {
                //cout << "resizing...\n";
                resize();
            }
            return; 
        }
    }
    
}

//returns set of flight nodes with given destination
unordered_set<flightNode*> HashTable::search(string dest) {
    int startIndex = hash(dest);
    for (int i = 0; i < bucketCount; i++) {
        int curr = (startIndex + i) % bucketCount;
        if (values[curr].hasData && values[curr].dest == dest)
            return values[curr].flights;
    }
    unordered_set<flightNode*> empty;
    return empty;
}

HashTable::~HashTable() {
    delete[] values;
}