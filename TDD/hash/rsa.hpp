#include <iostream>
#include <math.h>
#include <string>

using namespace std;

string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int modulo(int a, int n) {
  int r(a - n * (a / n));
  if (r < 0)
    r = a - n * ((a / n) - 1);
  return r;
}

int euclides(int a, int b) {
  int r;
  r = modulo(a, b);
  while (r > 0)
  {
    if (r > (b / 2))
      r = b - r;
    a = b;
    b = r;
    r = modulo(a, b);
  }
  return b;
}

int euclidesExtendido(int a, int b) {
  int res;
  int r1{a}, r2{b}, s1{1}, s2{0}, t1{0}, t2{1};
  int q, r, s, t;

  while (r2 > 0) {
    q = r1 / r2;

    r = r1 - q * r2;
    r1 = r2;
    r2 = r;

    s = s1 - q * s2;
    s1 = s2;
    s2 = s;

    t = t1 - q * t2;
    t1 = t2;
    t2 = t;
  }
  res = s1;
  return res;
}

int inversa(int x, int y) {
  int num;
  num = euclides(x, y);
  if (num == 1)
  {
    int aux;
    aux = euclidesExtendido(x, y);
    if (aux < 0)
      aux = modulo(aux, y);
    return aux;
  }
  else
  {
    num = 0;
    cout << "NO TIENE INVERSA" << endl;
    return num;
  }
}

int modPow(int a, int n, int m) {
  int exp(1);
  int x = modulo(a, m);

  while (n > 0) {
    if (modulo(n, int(2)) == 1)
      exp = modulo(exp * x, m);

    x = modulo(x * x, m);
    n /= 2;
  }

  return exp;
}

int gcd(int a, int b) {
  int t;
  while (1) {
    t = a % b;
    if (t == 0)
      return b;
    a = b;
    b = t;
  }
}

class RSA {
private:
  int d;

public:
  int n;
  int e;
  int number_of_pcs;
  RSA();
  RSA(int);
  RSA(int, int);
  void generarClaves();
  int cifrado(int);
  int descifrado(int);
  int hashCipher(string);
  int hashDecipher(string);
};

RSA::RSA() {
  generarClaves();
}

RSA::RSA(int n, int e) {
  this->n = n;
  this->e = e;
}

RSA::RSA(int _number_of_pcs) {
  this->number_of_pcs = _number_of_pcs;
  generarClaves();
}

void RSA::generarClaves() {
  int p;
  int q;
  p = 13;
  q = 11;
  n = p * q;
  int fi_n = (p - 1) * (q - 1);
  e = 7;
  d = inversa(e, fi_n);
}

int RSA::cifrado(int mensaje) {
  int tem;
  tem = modPow(mensaje, e, n);
  return tem;
}

int RSA::descifrado(int mensaje) {
  int tem;
  tem = modPow(mensaje, d, n);
  return tem;
}

int RSA::hashCipher(string mensaje) {
  int tem = 0;
  int pos;

  for(size_t i = 0 ; i < mensaje.size() ; i++) {
    pos = alphabet.find(mensaje[i]);
    tem += modPow(pos, e , n);
  }

  return modulo(cifrado(tem), number_of_pcs);
}

int RSA::hashDecipher(string mensaje) {
  int tem = 0;
  int pos;

  for(size_t i = 0 ; i < mensaje.size() ; i++) {
    pos = alphabet.find(mensaje[i]);
    tem += modPow(pos, d , n);
  }

  return modulo(descifrado(tem), number_of_pcs);
}