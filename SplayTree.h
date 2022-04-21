#pragma once
#include <iostream>
#include "flightNode.h"
#include <unordered_set>
using namespace std;

class SplayTree {
private:
    struct Node {
        Node* parent;
        Node* left;
        Node* right;
        string dest;
        unordered_set<flightNode*> flights;
        Node(Node* _parent, string _dest);
        Node();
        ~Node();
    };
    int size;
    int numNodes;
    Node* root;
    Node* toSplay;
    Node* insert(Node* head, string dest, Node* parent, flightNode* flight);
    Node* addNode(Node* head, string dest, Node* parent, Node* toAdd);
    void inOrder(Node* node);
    Node* getNode(Node* head, string dest);
    void splay(Node* node);
    Node* zig(Node* node);
    Node* zag(Node* node);
    Node* zigZig(Node* node);
    Node* zagZag(Node* node);
    Node* zigZag(Node* node);
    Node* zagZig(Node* node);
    void nodeInfo(Node* node);
    void deleteNode(Node* node);
    
public:
    SplayTree();
    ~SplayTree();
    void insert(flightNode* flight);
    void insert(unordered_set<flightNode*>& flightSet);
    unordered_set<flightNode*> search(string dest);
    void inOrder();
    int getSize();
    int nodeCount();
};


//------------SPLAYTREE PUBLIC METHODS---------------
SplayTree::SplayTree() {
    root = nullptr;
    toSplay = nullptr;
    size = 0;
    numNodes = 0;
}

SplayTree::~SplayTree() {
    deleteNode(root);
}

void SplayTree::insert(flightNode* flight) {
    string dest = flight->dest;
    root = insert(root, dest, nullptr, flight);
    splay(toSplay);
    toSplay = nullptr;
    size++;
}

void SplayTree::insert(unordered_set<flightNode*>& flightSet) {
    //cout << "Attempting to add set to tree.\n";
    if (flightSet.size() == 0)
        return;
    auto first = flightSet.begin();
    flightNode* f = *first;
    string dest = f->dest;
    Node* toAdd = new Node();
    toAdd->flights = flightSet;
    toAdd->dest = dest;
    root = addNode(root, dest, nullptr, toAdd);
    splay(toSplay);
    toSplay = nullptr;
    size += flightSet.size();
    numNodes++;
    //cout << numNodes << "\n";
}

//returns set of flight nodes with given destination
unordered_set<flightNode*> SplayTree::search(string dest) {
    Node* target = getNode(root, dest);
    if (!target) {
        cout << "Could not locate " << dest << "\n";
        unordered_set<flightNode*> empty;
        return empty;
    }        
    else {
        splay(target);
        return target->flights;
    }
}

void SplayTree::inOrder() {
    cout <<"In order traversal:\n";
    inOrder(root);
}

int SplayTree::getSize() {
    return size;
}

int SplayTree::nodeCount() {
    return numNodes;
}

//-------------SPLAYTREE PRIVATE METHODS---------------
SplayTree::Node* SplayTree::insert(Node* head, string dest, Node* parent, flightNode* flight) {
    if (!head) {
        head = new Node(parent, dest);
        head->flights.insert(flight);
        toSplay = head;
        return head;
    }
    else if (dest.compare(head->dest) < 0) {
        head->left = insert(head->left, dest, head, flight);
    }
    else {
        head->right = insert(head->right, dest, head, flight);
    }
    return head;
}

SplayTree::Node* SplayTree::addNode(Node* head, string dest, Node* parent, Node* toAdd) {
    if (!head) {
        //cout << "adding node as leaf\n";
        head = toAdd;
        head->parent = parent;
        toSplay = head;
    }
    else if (dest.compare(head->dest) == 0) {
        //cout << "found location, replacing old data\n";
        head->flights = toAdd->flights;
        toSplay = head;
        delete toAdd;
    }
    else if (dest.compare(head->dest) < 0) {
        //cout << "going left\n";
        head->left = addNode(head->left, dest, head, toAdd);
    }
    else {
        //cout << "going right\n";
        head->right = addNode(head->right, dest, head, toAdd);
    }
    return head;
}

void SplayTree::inOrder(Node* node) {
    if (!node)
        return;
    inOrder(node->left);
    cout << node->dest;
    if (node->parent)
        cout << ", parent " << node->parent->dest;
    if (node->left)
        cout << ", left child " << node->left->dest;
    if (node->right)
        cout << ", right child " << node->right->dest;
    cout << "\n";
    inOrder(node->right);
}

void SplayTree::splay(Node* node) {
    if (!node || !node->parent) {
        //cout << "Splaying is on root, no action\n";
        return;
    } 
    Node* p = node->parent;
    //right child of parent
    if (p->right == node) {
        //right child of root
        if (!p->parent) {
            //cout << "zag on " << node->dest << "\n";
            node = zag(node);
        }
        //left-right arrangement
        else if (p->parent->left == p) {
            //cout << "zigZag on " << node->dest << "\n";
            node = zigZag(node);
        }
        //right-right arrangement
        else {
            //cout << "zagZag on " << node->dest << "\n";
            node = zagZag(node);
        }
    }
    //left child of parent
    else {
        //left child of root
        if (!p->parent) {
            //cout << "zig on " << node->dest << "\n";
            node = zig(node);
        }
        //left-left arrangement
        else if (p->parent->left == p) {
            //cout << "zigZig on " << node->dest << "\n";
            node = zigZig(node);
        }
        //right-left arrangement
        else {
            //cout << "zagZig on " << node->dest << "\n";
            node = zagZig(node);
        }
    }
}

SplayTree::Node* SplayTree::getNode(Node* head, string dest) {
    if (!head)
        return nullptr;
    else if (head->dest == dest)
        return head;
    else if (dest.compare(head->dest) < 0)
        return getNode(head->left, dest);
    else
        return getNode(head->right, dest);
}

SplayTree::Node* SplayTree::zig(Node* node) {
    Node* p = node->parent;
    p->left = node->right;
    if (p->left)
        p->left->parent = p;
    node->right = p;
    p->parent = node;
    node->parent = nullptr;
    root = node;
    return node;
}

SplayTree::Node* SplayTree::zag(Node* node) {
    Node* p = node->parent;
    p->right = node->left;
    if (p->right)
        p->right->parent = p;
    node->left = p;
    p->parent = node;
    node->parent = nullptr;
    root = node;
    return node;
}

SplayTree::Node* SplayTree::zigZig(Node* node) {
    Node* p = node->parent;
    Node* g = p->parent;
    Node* gg = g->parent;
    bool right;
    if (gg)
        right = (gg->right == g);
    g->left = p->right;
    if (g->left)
        g->left->parent = g;
    p->right = g;
    g->parent = p;
    p->parent = node;
    p->left = node->right;
    if (p->left)
        p->left->parent = p;
    node->right = p;
    node->parent = gg;
    if (gg) {
        if (right)
            gg->right = node;
        else
            gg->left = node;
    }
    else
        root = node;
    return node;
}

SplayTree::Node* SplayTree::zagZag(Node* node) {
    Node* p = node->parent;
    Node* g = p->parent;
    Node* gg = g->parent;
    bool right;
    if (gg)
        right = (gg->right == g);
    g->right = p->left;
    if (g->right)
        g->right->parent = g;
    p->left = g;
    g->parent = p;
    p->parent = node;
    p->right = node->left;
    if (p->right)
        p->right->parent = p;
    node->left = p;
    node->parent = gg;
    if (gg) {
        if (right)
            gg->right = node;
        else
            gg->left = node;
    }
    else
        root = node;
    return node;
}

SplayTree::Node* SplayTree::zigZag(Node* node) {
    Node* p = node->parent;
    Node* g = p->parent;
    Node* gg = g->parent;
    bool right;
    if (gg)
        right = (gg->right == g);
    if (node->left)
        node->left->parent = p;
    p->right = node->left;
    if (node->right)
        node->right->parent = g;
    g->left = node->right;
    p->parent = node;
    g->parent = node;
    node->left = p;
    node->right = g;
    node->parent = gg;
    if (gg) {
        if (right)
            gg->right = node;
        else
            gg->left = node;
    }
    else
        root = node;
    return node;
}

SplayTree::Node* SplayTree::zagZig(Node* node) {
    Node* p = node->parent;
    Node* g = p->parent;
    Node* gg = g->parent;
    bool right;
    if (gg)
        right = (gg->right == g);    
    if (node->right)
        node->right->parent = p;
    p->left = node->right;
    if (node->left)
        node->left->parent = g;
    g->right = node->left;
    p->parent = node;
    g->parent = node;
    node->right = p;
    node->left = g;
    node->parent = gg;
    if (gg) {
        if (right)
            gg->right = node;
        else
            gg->left = node;
    }
    else
        root = node;
    return node;
}

void SplayTree::nodeInfo(Node* node) {
    if (!node) {
        cout << "Node does not exist in the tree.\n";
        return;
    }
    cout << "Info for node " << node->dest <<":\n";
    cout << "Parent:\n";
    if (node->parent) {
        cout << node->parent->dest << "\n";
    }
    else {
        cout << "null parent\n";
    }
    cout << "Left child:\n";
    if (node->left) {
        cout << node->left->dest << "\n";
    }
    else {
        cout << "null left\n";
    }
    cout << "Right child:\n";
    if (node->right) {
        cout << node->right->dest << "\n";
    }
    else {
        cout << "null right\n";
    }
    cout << "\n";
}

void SplayTree::deleteNode(Node* node) {
    if (!node)
        return;
    deleteNode(node->left);
    deleteNode(node->right);
    delete node;
}

//----------------NODE METHODS--------------------
SplayTree::Node::Node(Node* _parent, string _dest) {
    parent = _parent;
    dest = _dest;
    left = nullptr;
    right = nullptr;
}

SplayTree::Node::Node() {
    parent = nullptr;
    dest = "";
    left = nullptr;
    right = nullptr;
}

SplayTree::Node::~Node() {

}