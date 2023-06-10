#include<iostream>
#include "tree.h"

using namespace std;

int main() {
  Tree<int> t;
  t.Insert(1);
  t.Insert(5);
  t.Insert(10);
  t.Insert(20); //ovdje ce se vrsit ce se balansiranje na cvoru 1

  cout<<"t: ";

  t.Print();

  cout<<endl<<"-----------------------------"<<endl;

  t.Insert(0);

  /*
  Nakon sto smo ubacili cvor 0 opet se balansira citavo stablo
  jer korjen 10 ima 3 elementa u desnom podstablu i 1 element u lijevom (te se 10 nalazi na visini 3)
  */

  cout<<"t: ";

  t.Print();

  //konstruktor kopije
  Tree<int> d(t);
  cout<<endl<<"d: ";
  d.Print();


  cout<<endl<<"-----------------------------"<<endl;
  Tree<double> stablo = Tree<double>{7.6,15.87,3,22, 24.01, 30.123, 56, 2, 18, 6, 22};
  cout<<"stablo: ";
  stablo.Print();
  cout<<endl;

  stablo.Erase(3);
  stablo.Erase(22);
  cout<<"stablo: ";
  stablo.Print();
  cout<<endl;

  cout<<endl<<"-----------------------------"<<endl;
  Tree<int> s;
  s = t;

  s.Insert(15);
  s.Insert(25);
  s.Insert(-1);
  s.Insert(-3);

  cout<<"t: ";
  t.Print();
  cout<<endl;
  cout<<"s: ";
  s.Print();

  cout<<endl;
  s.Erase(5);
  s.Erase(20);
  s.Erase(-1);
  cout<<"s nakon brisanja: ";
  s.Print();

  cout<<endl<<"-----------------------------"<<endl;
  Tree<int> randStablo;

  for(int i = 0; i < 100; i++)
    randStablo.Insert(rand()%500);

  cout<<"randStablo: ";
  randStablo.Print();
  cout<<endl<<"-----------------------------"<<endl;

}
