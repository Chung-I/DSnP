#include<iostream>
#include<cstring>
#include<sstream>
#include<fstream>
using namespace std;

int main() {
   ifstream ifs("test01.aag");
   if(!ifs.is_open())   return 0;
   string s;
   getline(ifs,s);
   stringstream ss(s);
   string a;
while(ss>>a)   cout<<a;
   int i = 7;
   cout<<endl<<i/2<<endl;
   return 0;
}
