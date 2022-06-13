#include <iostream>
#include "rsa.hpp"

using namespace std;

int main () {
  RSA receptor(9);
  cout << "PC assigned:" << receptor.hashCipher("besro")<< endl;

  // RSA emisor(receptor.n, receptor.e);
  // int aux = emisor.cifrado(9);
  // int lol1 = receptor.descifrado(aux);
  // cout << "mensaje original :" << 9 << endl;
  // cout << "cifrado :" << aux << endl;
  // cout << "decifrado :" << lol1 << endl;

  // RSA test;

  // cout << test.cifrado(9) << endl;
  // cout << test.descifrado(test.cifrado(9)) << endl;


  return 0;
}