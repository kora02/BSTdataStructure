#ifndef TREE_H
#define TREE_H

#include<iostream>
#include<utility>
#include<vector>
#include<initializer_list>

using namespace std;



template <typename Type>
class Tree {

  public:

  struct Node {
    Type value;
    Node *leftChild, *rightChild, *parent;
    std::pair<int, int> subtreeSize; //subtreeSize.first predstavlja broj cvorova u lijevom podstablu cvora, analogno subtreeSize.second u desnom (ovo olaksava provjeru uslova za balansiranost)

    Node(Type value = Type(), Node* leftChild = nullptr, Node* rightChild = nullptr, Node* parent = nullptr, std::pair<int, int> subtreeSize = {0, 0}):
         value(value), leftChild(leftChild), rightChild(rightChild), parent(parent), subtreeSize(subtreeSize) {}
  };


  private:

  Node* root;
  int numberOfNodes;

  void deleteTree(Node* node);
  Node* CopyTree(const Node* copy_node, Node* Parent = nullptr);

  void Print(Node* node);

  Node* FindMin(Node* node); // trazi minimalni element u podstablu ciji je korjen node

  Node* FindNextInSubtree(Node* node, Node* subtree_root); //traži naredni veci element u podstablu ciji je korjen subtree_root

  Node* FromSortedArrayMakeTree(vector<Node*> sortedArray, Node* Parent = nullptr); //od sortiranog vektora pravi stablo
  vector<Node*> FromTreeMakeSortedArray(Node* subtree_root); //od podstabla pravi sortiran vektor

  int maxHeight(Node* node); //vraca visinu cvora

  void Erase(Node* node);

  public:

  //konstruktori
  Tree(): root(nullptr), numberOfNodes(0) {}
  Tree(const Tree& tree);
  Tree(Tree&& tree);
  Tree(std::initializer_list<Type> li);

  ~Tree();

  Tree<Type>& operator=(const Tree& tree);
  Tree<Type>& operator=(Tree&& tree);

  int Size() {return numberOfNodes;}

  std::pair<Node*, bool> Insert(Type element);

  bool Erase(const Type& element);

  Node* Find(const Type& element);

  void Print() {Print(root);} //ispisuje elemente stabla od najmanjeg ka najvecem
};

#include "tree.cpp"

#endif // TREE_H
