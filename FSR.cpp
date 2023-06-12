#include "FSR.h"

FSR::FSR(int newPin, int r) {
  pin = newPin;
  rm = r;

}

void FSR::setPin(int newPin) {
  pin = newPin;
}

void FSR::read() {
  signal = analogRead(pin);
}

double FSR::force() {
  double F30[] = {0, 21.577976542651307, 51.61312776250305, 103.07544171046068, 201.56266313994362, 316.74381373333796, 449.5295769092449, 565.4589960440377, 711.6845903084723, 711.6845903084723, 893.3688325850649, 1000};
  double V30[] = {0, 2.227668455575432, 2.6380043728880938, 3.1307493540051676, 3.528324388789505, 3.771019677996422, 3.9535281256211485, 4.101411250248459, 4.227072152653548, 4.227072152653548, 4.314410653945537, 4.313098787517392};

  double F10[] = {0, 21.513888969147825, 48.685069719331295, 102.2646503054013, 201.6110857492878, 314.1963929708583, 446.7122409000815, 565.3246730732559, 707.7508942450492, 892.8466174711502, 1000};
  double V10[] = {0, 0.8855218587490656, 1.3588365324978928, 1.760547701213403, 2.2027321448450246, 2.496898904279512, 2.7973473704298595, 2.9843672969577457, 3.1415292377665747, 3.3649274025540303, 3.4117618994608865};
  
  double F47[] = {0, 20.716436195110113, 46.97459130545331, 100.68708068719627, 199.60011788432314, 317.9814043258924, 447.318998445545, 565.8043004663366, 710.6864450364344, 889.957295826086, 1000};
  double V47[] = {0, 2.4837788839236015, 3.181604936308265, 3.5917049665240692, 3.8939425263593135, 4.109389163658339, 4.273389418107218, 4.368131868131868, 4.438383613492152, 4.507283599179402, 4.53495491483914};
  
  double F3[] = {0, 21.87794349642591, 49.86391295051748, 97.92488919451492, 196.38719221501177, 314.75114272671067, 443.4704220143195, 562.3602290654201, 706.1906605567145, 891.800683035637, 1000};
  double V3[] = {0, 0.29158251300074745, 0.4980041666004039, 0.7049426694868084, 0.9557338464719072, 1.1860498401742974, 1.3682193349342409, 1.5252620028307438, 1.7304114120322514, 1.8797410982649767, 1.9503506623622398};
  
  const double VOLTAGE = 5.0;
  const double RESOLUTION = 1023.0;
  //Serial.println(sizeof(F30)/sizeof(F30[0]));
  switch ( rm  ) {
    case 3000:
      return V2F(signal * VOLTAGE/RESOLUTION, V3, F3, 11);
      break;
    case 10000:
      return V2F(signal * VOLTAGE/RESOLUTION, V10, F10, 11); 
      break;
    case 30000:
      return V2F(signal * VOLTAGE/RESOLUTION, V30, F30, 12);
      break;
    case 47000:
      return V2F(signal * VOLTAGE/RESOLUTION, V47, F47, 11);
      break;
    default:
      Serial.print('\n');
      Serial.print("error: ");
      Serial.println(rm);
      return -1.0;
  }
  
}

double FSR::V2F(double v, const double V[], const double F[], int n) {  
  for (int i = 0; i < n-1; i++) {
    if (v >= V[i] && v <= V[i+1]) {
      return ( (F[i]-F[i+1]) * v - F[i]*V[i+1]+F[i+1]*V[i] ) / (V[i]-V[i+1]);
    }
  }
}

void FSR::print() {
  Serial.print(signal);
}

void FSR::printSignal() {
  Serial.print(" @");
  Serial.print(pin);
  Serial.print(": ");
  Serial.print(signal);
}

bool FSR::operator<(const FSR& right) const {
  return signal < right.signal;
}

bool FSR::operator>(const FSR& right) const {
  return signal > right.signal;
}

bool FSR::operator==(const FSR& right) const {
  return signal == right.signal;
}

bool FSR::operator!=(const FSR& right) const {
  return signal != right.signal;
}

bool FSR::operator<=(const FSR& right) const {
  return signal < right.signal || signal == right.signal;
}

bool FSR::operator>=(const FSR& right) const {
  return signal > right.signal || signal == right.signal;
}

