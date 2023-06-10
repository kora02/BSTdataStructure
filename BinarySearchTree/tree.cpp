#ifndef TREE_CPP
#define TREE_CPP

#include "tree.h"

#include<iostream>
#include<utility>
#include<vector>
#include<initializer_list>

using namespace std;


template<typename Type>
Tree<Type>::Tree(const Tree& tree) {
  root = CopyTree(tree.root);
  numberOfNodes = tree.numberOfNodes;
}


template<typename Type>
typename Tree<Type>::Node* Tree<Type>::CopyTree(const Node* copy_node, Node* Parent) {
  if(copy_node == nullptr) {
    return nullptr;
  }

  Node* node = new Node(copy_node->value);
  node->parent = Parent; //posto je funkcija rekurzivna, parametar Parent omogucava da postavljamo roditelje
  Parent = node;
  node->subtreeSize.first = copy_node->subtreeSize.first;
  node->subtreeSize.second = copy_node->subtreeSize.second;
  node->leftChild = CopyTree(copy_node->leftChild, Parent);
  node->rightChild = CopyTree(copy_node->rightChild, Parent);

  return node;

}


template<typename Type>
Tree<Type>::Tree(Tree&& tree) {
  root = nullptr;
  numberOfNodes = tree.numberOfNodes;
  std::swap(root, tree.root);
}


template<typename Type>
Tree<Type>::Tree(std::initializer_list<Type> li) {
  root = nullptr;
  numberOfNodes = 0;
  for(auto el: li) Insert(el);

  //Ovaj konstruktor sam napravio jedino iz razloga da bi testirao ispravnost pomjerajuceg konstruktora kopije i pomjerajuceg operatora dodjele
}


template<typename Type>
Tree<Type>& Tree<Type>::operator=(const Tree& tree) {
  if(this == &tree) return *this;

  deleteTree(root);

  root = nullptr;
  numberOfNodes = tree.numberOfNodes;
  root = CopyTree(tree.root);

  return *this;
}


template<typename Type>
Tree<Type>& Tree<Type>::operator=(Tree&& tree) {

  this->deleteTree();
  root = nullptr;
  numberOfNodes = tree.numberOfNodes;
  std::swap(root, tree.root);

  return *this;
}


template<typename Type>
Tree<Type>::~Tree() {
  deleteTree(root);
}


template<typename Type>
void Tree<Type>::deleteTree(Node* node) {
  if(node == nullptr) return;
  deleteTree(node->leftChild);
  deleteTree(node->rightChild);
  delete node;
}


template<typename Type>
vector<typename Tree<Type>::Node*> Tree<Type>::FromTreeMakeSortedArray(Node* subtree_root) {
  vector<Node*> sortedArray(subtree_root->subtreeSize.first + subtree_root->subtreeSize.second + 1, nullptr);
  int index = 0;
  Node* node = FindMin(subtree_root);

  while(true) {
    sortedArray[index] = node;
    index++;
    node = FindNextInSubtree(node, subtree_root);
    if(node == nullptr) break;
  }

  return sortedArray;
}


template<typename Type>
typename Tree<Type>::Node* Tree<Type>::FindMin(Node* node) {
  if(node == nullptr) return nullptr;
  Node* minEl = node;
  while(minEl->leftChild != nullptr) minEl = minEl->leftChild;

  return minEl;
}


template<typename Type>
typename Tree<Type>::Node* Tree<Type>::FindNextInSubtree(Node* node, Node* subtree_root) {
  if(node->rightChild != nullptr) return FindMin(node->rightChild);

  Node* nextEl = node;
  while(nextEl != subtree_root && nextEl == nextEl->parent->rightChild)
    nextEl = nextEl->parent;

  if(nextEl == subtree_root) return nullptr;
  return nextEl->parent;
}


template<typename Type>
typename Tree<Type>::Node* Tree<Type>::FromSortedArrayMakeTree(vector<Node*> sortedArray, Node* Parent) {
  if(sortedArray.size() == 0) return nullptr;
      //Baza rekurzije - ako je cvor list
      else if(sortedArray.size() == 1) {
        sortedArray[0]->parent = Parent;
        sortedArray[0]->leftChild = nullptr;
        sortedArray[0]->rightChild = nullptr;
        sortedArray[0]->subtreeSize.first = 0;
        sortedArray[0]->subtreeSize.second = 0;

        return sortedArray[0];
      }

  Node* middle = sortedArray[sortedArray.size()/2];

  middle->subtreeSize.first = sortedArray.size()/2;
  middle->subtreeSize.second = sortedArray.size() - sortedArray.size()/2 - 1;

  vector<Node*> left(sortedArray.begin(), sortedArray.begin() + sortedArray.size()/2);
  vector<Node*> right(sortedArray.begin() + sortedArray.size()/2 + 1, sortedArray.end());

  middle->parent = Parent; //posto je funkcija rekurzivna, parametar Parent omogucava da automatski postavljamo roditelje cvorova

  Parent = middle;

  middle->leftChild = FromSortedArrayMakeTree(left, Parent);
  middle->rightChild = FromSortedArrayMakeTree(right, Parent);

  return middle;
}


template<typename Type>
std::pair<typename Tree<Type>::Node*, bool> Tree<Type>::Insert(Type element) {

  if(root == nullptr) {
    root = new Node(element);
    numberOfNodes++;
    return {root, true};
  }

  if(Find(element) != nullptr) return {nullptr, false}; //ako element vec postoji u stablu, ne radimo nista

  numberOfNodes++;

  Node *node = root, *previous = nullptr;

  bool balancing = false; //postat ce true kada dodjemo do cvora koji u jednom podstablu ima duplo vise elemenata nego u drugom (visinu cvora provjeravamo kasnije)

  Node* ParentOfBalancingSubtree = nullptr; //ovdje pamtimo roditelja cvora kojeg balansiramo (bit ce nullptr ako balansiramo citavo stablo)

  Node* BalancingSubtreeRoot = nullptr; //ovdje cemo pamtiti cvor koji ce trebati balansirati

  bool SideOfParent = false; // false for left, true for right (kada balansiramo podstablo, treba postaviti korjen podstabla kao desno ili lijevo dijete od cvora cije se podstablo balansiralo)

  /*
    while petljom cemo traziti gdje treba insertovati cvor
    Pri tom cemo azurirati subtreeSize (u zavisnosti da li se krecemo lijevo ili desno podstablo cvora)
    Takodjer, odmah cemo provjeravati uslov za potrebu balansiranja (u konstantnom vremenu - samo uporedjujemo subtreeSize)
  */

  while(node != nullptr) {
    if(node->value > element) {
      //krecemo se u lijevo podstablo cvora
      node->subtreeSize.first++;

      //prvovjeravamo uslov za balansiranje
      if(!balancing && node->subtreeSize.first >= 2*node->subtreeSize.second) {
        balancing = true;
        BalancingSubtreeRoot = node;
        ParentOfBalancingSubtree = previous;

        if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot->value == ParentOfBalancingSubtree->rightChild->value)
          SideOfParent = true;
      }
      previous = node;
      node = node->leftChild;
    }
    else if(node->value < element) {
      //krecemo se u desno podstablo cvora
      node->subtreeSize.second++;

      //provjeravamo uslov za balansiranje
      if(!balancing && node->subtreeSize.second >= 2*node->subtreeSize.first && node->subtreeSize.first != 0) {
        balancing = true;
        BalancingSubtreeRoot = node;
        ParentOfBalancingSubtree = previous;

        if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot == ParentOfBalancingSubtree->rightChild)
          SideOfParent = true;
      }
      previous = node;
      node = node->rightChild;
    }
  }

  Node* new_element = new Node(element);
  new_element->parent = previous;

  if(element < previous->value) {
    previous->leftChild = new_element;
  }
  else if(element > previous->value) previous->rightChild = new_element;

  //ovdje radimo balansiranje ako je potrebno
  if(balancing && maxHeight(BalancingSubtreeRoot) >= 3) {

    vector<Node*> sortedArray = FromTreeMakeSortedArray(BalancingSubtreeRoot);

    Node* BalancedNode = FromSortedArrayMakeTree(sortedArray, ParentOfBalancingSubtree);

    if(ParentOfBalancingSubtree == nullptr) {
      //balansiralo se cijelo stablo, sto znaci da je dovoljno samo premjestiti atribut root da pokazuje na novi korjen
      root = BalancedNode;
    }
    else {
      if(SideOfParent == false) {
        //korjen balansiranog podstabla se postavlja kao lijevo dijete
        ParentOfBalancingSubtree->leftChild = BalancedNode;
      }


      else {
        //korjen balansiranog podstabla se postavlja kao desno dijete
        ParentOfBalancingSubtree->rightChild = BalancedNode;
      }
    }
  }
  return {new_element, true};
}


template<typename Type>
typename Tree<Type>::Node* Tree<Type>::Find(const Type& element) {
  Node* node = this->root;
  while(true) {
    if(node == nullptr) return nullptr;
    else if(node->value == element) return node;
    else if(node->value < element) node = node->rightChild;
    else node = node->leftChild;
  }
}


template<typename Type>
int Tree<Type>::maxHeight(Node* node) {
  if(node == nullptr) return 0;

  if(node->leftChild == nullptr && node->rightChild == nullptr) return 0;

  else if(node->leftChild == nullptr) return maxHeight(node->rightChild) + 1;

  else if(node->rightChild == nullptr) return maxHeight(node->leftChild)+1;

  else return std::max(maxHeight(node->leftChild), maxHeight(node->rightChild)) + 1;
}


template<typename Type>
bool Tree<Type>::Erase(const Type& element) {
  Node* node = Find(element);

  if(node == nullptr) return false;

  else {
    Erase(node);
    return true;
  }
}


template<typename Type>
void Tree<Type>::Erase(Node* element) {

  Node* node = root, *previous = nullptr;

  /*
    balansiranje cemo implementirati slicno kao u insertu (bilo bi identicno insertu, medjutim postoji mala razlika
    kad brisemo cvor koji ima dva djeteta - mijenja se struktura stabla, tj. subtreeSize za neke cvorove koji se
    nalaze u podstablu cvora koji brisemo)
  */

  bool balancing = false;
  Node* ParentOfBalancingSubtree = nullptr;
  Node* BalancingSubtreeRoot = nullptr;
  bool SideOfParent = false;
   //while petlja je skoro pa ista kao i u insertu
  while(node->value != element->value) {
    if(node->value > element->value) {
      node->subtreeSize.first--;
      if(!balancing && node->subtreeSize.second >= 2*node->subtreeSize.first) {
        balancing = true;
        BalancingSubtreeRoot = node;
        ParentOfBalancingSubtree = previous;

        if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot == ParentOfBalancingSubtree->rightChild)
          SideOfParent = true;
      }
      previous = node;
      node = node->leftChild;
    }
    else {
      node->subtreeSize.second--;
      if(!balancing && node->subtreeSize.first >= 2*node->subtreeSize.second) {
        balancing = true;
        BalancingSubtreeRoot = node;
        ParentOfBalancingSubtree = previous;

        if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot == ParentOfBalancingSubtree->rightChild)
          SideOfParent = true;
      }
      previous = node;
      node = node->rightChild;
    }
  }
  /*
    Sada smo dosli do cvora koji treba da obrisemo, pri tom smo vec azurirali
    subtreeSize za sve cvorove koji u svom podstablu sadrze cvor koji se brise
    (i naravno provjerili da li ce jedan od tih cvorova trebati balansirati nako brisanja)
  */

  //PRVI SLUCAJ: cvor koji brisemo nema djece
  if(node->leftChild == nullptr && node->rightChild == nullptr) {
    //prvo provjeravamo da li je cvor koji brisemo korjen
    if(node == root) {
      root = nullptr;
    }
    else if(previous != nullptr && previous->leftChild == node) previous->leftChild = nullptr;
    else if(previous != nullptr && previous->rightChild == node) previous->rightChild = nullptr;
    node->parent = nullptr;
    delete node;
  }

  //DRUGI SLUCAJ: cvor ima samo lijevo dijete
  else if(node->leftChild != nullptr && node->rightChild == nullptr) {
    if(node == root) {
      root = node->leftChild;
    }
    else if(previous != nullptr && node == previous->leftChild) previous->leftChild = node->leftChild;
    else if(previous != nullptr && node == previous->rightChild) previous->rightChild = node->leftChild;
    node->leftChild->parent = previous;
    node->leftChild = nullptr;
    node->rightChild = nullptr;
    delete node;
  }

  //TRECI SLUCAJ: cvor koji brisemo ima samo desno dijete
  else if(node->leftChild == nullptr && node->rightChild != nullptr) {
    if(root == node) {
      root = node->rightChild;
    }
    else if(previous != nullptr && node == previous->leftChild) previous->leftChild = node->rightChild;
    else if(previous != nullptr && node == previous->rightChild) previous->rightChild = node->rightChild;
    node->rightChild->parent = previous;
    node->rightChild = nullptr;
    node->parent = nullptr;
    delete node;
  }

  //CETVRTI SLUCAJ: cvor koji brisemo ima oba djeteta
  else {
    /*
      Naci cemo minimalni element u desnom podstablu i zamijeniti vrijednosti sa cvorom koji brisemo.
      Nakon toga obrisemo cvor koji je prije zamjene imao minimalnu vrijednost (sto se svodi na prvi ili treci slucaj)
      Medjutim,vidimo da ce se promjeniti subtreeSize za sve elemente koji u svom podstablu
      imaju taj minimalni element.
    */

    Node* minEl = node, *afterMin = node->rightChild;

    minEl->subtreeSize.second--; //smanjujemo subtreeSize u denom podstablu, jer cemo iz njega obrisati cvor
    if(!balancing && minEl->subtreeSize.first >= 2*minEl->subtreeSize.second) {
          balancing = true;
          BalancingSubtreeRoot = minEl;
          ParentOfBalancingSubtree = previous;
          if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot == ParentOfBalancingSubtree->rightChild)
            SideOfParent = true;
    }

    while(true) {
      previous = minEl;
      minEl = afterMin;
      afterMin = afterMin->leftChild;
      if(afterMin != nullptr) {
        minEl->subtreeSize.first--;

        if(!balancing && minEl->subtreeSize.second >= 2*minEl->subtreeSize.first) {
          balancing = true;
          BalancingSubtreeRoot = minEl;
          ParentOfBalancingSubtree = previous;
          if(ParentOfBalancingSubtree != nullptr && BalancingSubtreeRoot == ParentOfBalancingSubtree->rightChild)
            SideOfParent = true;
        }

      }
      else break;
    }

    swap(minEl->value, node->value);

    //ako nema djece
    if(minEl->leftChild == nullptr && minEl->rightChild == nullptr) {
      if(minEl == root) root = nullptr;
      else if(minEl->parent != nullptr && minEl->parent->leftChild == minEl) minEl->parent->leftChild = nullptr;
      else if(minEl->parent != nullptr && minEl->parent->rightChild == minEl) minEl->parent->rightChild = nullptr;
      minEl->parent = nullptr;
      delete minEl;
    }

    //ako ima samo desno dijete
    else if(node->leftChild == nullptr && minEl->rightChild != nullptr) {
      if(minEl == root) {
        root = node->rightChild;
      }
      else if(minEl->parent != nullptr && minEl == minEl->parent->leftChild) minEl->parent->leftChild = minEl->rightChild;
      else if(minEl->parent != nullptr && minEl == minEl->parent->rightChild) previous->rightChild = node->rightChild;

      minEl->rightChild->parent = minEl->parent;
      minEl->rightChild = nullptr;
      minEl->parent = nullptr;
      delete minEl;
    }


  }

  numberOfNodes--;

  //ovaj dio je isti kao u insertu
  if(balancing && maxHeight(BalancingSubtreeRoot) >= 3) {
    vector<Node*> sortedArray = FromTreeMakeSortedArray(BalancingSubtreeRoot);

    Node* BalancedNode = FromSortedArrayMakeTree(sortedArray, ParentOfBalancingSubtree);

    if(ParentOfBalancingSubtree == nullptr) {
      root = BalancedNode;
    }
    else {
      if(SideOfParent == false)
        ParentOfBalancingSubtree->leftChild = BalancedNode;

      else ParentOfBalancingSubtree->rightChild = BalancedNode;
    }
  }

  return;
}


template<typename Type>
void Tree<Type>::Print(Node* node) {
  if(node == nullptr) return;

  Print(node->leftChild);
  std::cout<<node->value<<" ";
  Print(node->rightChild);
}

#endif // TREE_CPP
