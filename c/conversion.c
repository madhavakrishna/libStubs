
typedef union IntOrFloat{
  int i;
  float f;
} intRfloat;

float int2float(int x){ 
  intRfloat tmp;
  tmp.i = x;
  return tmp.f;
}

int float2int(float x){ 
  intRfloat tmp;
  tmp.f = x;
  return tmp.i; 
}


int hex2dec(char x){ 
  if(x >= '0' && x <= '9'){
    return (x - '0');
  } else if(x >= 'A' && x <= 'F'){
    return (x - 'A') + 10; 
  } else if(x >= 'a' && x <= 'f'){
    return (x - 'a') + 10; 
  }
}


int hex2int(char *x){
  int i,r=0;
  for(i=0;i<8;i++){
    r = r + (hex2dec(x[i]) << (4*(7-i)));
  }
  return r;  
}

