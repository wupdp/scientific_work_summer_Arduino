#include <LiquidCrystal.h>
#include <Keypad.h>

#define INITIAN_BITS 7
#define SUM_BITS 8

int buttonZeroState = 0;
int buttonOneState = 0;

char firstNum[] = "2222222";
char secondNum[] = "2222222";
char resultNum[] = "0000000";
char prev = '0';
const char addOne7[] = "0000001";
const char helloMessage[] = "hello, enter first num in binary code";

bool firstFull = 0;
bool secondFull = 0;
bool end = 0;
bool whichNum = 0;
bool overflow = 0;

//pins for display
const int rs = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(){
  lcd.begin(16, 2);
  for(int i = 0; i + 15 < 37; i++){
    lcd.setCursor(0, 0);
    for(int j = 0; j < 16; j++)
        lcd.print(helloMessage[i + j]);
    if(i == 0)
        delay(50);//вернуть на 200
    delay(20);//вернуть на 80
  }
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  for(int i = 6; i < 11; i++)
  	pinMode(i, OUTPUT);
  pinMode(13, OUTPUT);
}
int findPos(){
	for(int i = 7;i >=0;i--){
    	if(whichNum == 0 && firstNum[i]=='2'){
          if(i == 0)
            firstFull = 1;
          return i;
        }
      	else if(whichNum == 1 && secondNum[i] == '2'){
          if(i == 0)
            secondFull = 1;
        return i;
        } 
    }
  return -1;
}
void printNum(){
	if(whichNum==0){
    	for(int i = 0; i <8; i++){
          if(firstNum[i] != '2')
      		lcd.print(firstNum[i]);
          else 
            lcd.print(' ');
    	}
    }
  else{
  	for(int i = 0; i <8; i++){
      		if(secondNum[i] != '2')
      			lcd.print(secondNum[i]);
      		else lcd.print(' ');
    	}
  }
    
}

char XOR_l(char a1, char a2) {
    if(a1 == a2)
     	return '0';
    else
    	return '1';
}

char AND_l(char a1, char a2) {
    if (a1 == '1' && a2 == '1')
    	return '1';
    else
    	return '0';
}

char OR_l(char a1, char a2){
    if(a1 == '1' || a2 == '1') 
      return '1';
    else 
      return '0';
}

char HSM(char a1, char a2){
  char lastPrev = prev;
  if(OR_l(AND_l(prev, XOR_l(a1, a2)), AND_l(a1, a2))=='1') 
    prev = '1';
  else 
    prev = '0';
  if(XOR_l(XOR_l(a1, a2), lastPrev)=='1')  
    return '1';
  else return '0';
}

void SM_8_BIT(){
  for(int i = 6; i > -1; i--){
  	resultNum[i] = HSM(firstNum[i], secondNum[i]);    
  }
  if(prev != '0')
    overflow = 1;
  prev = '0';
}

void SM_CON(){
  if(firstNum[0] == '1')
  	for(int i = 6; i > -1; i--)
  		firstNum[i] = HSM(addOne7[i], firstNum[i]);	// 7 разрядов
    prev = '0';
  if(secondNum[0] == '1')
  	for(int i = 6; i > -1; i--)
  		secondNum[i] = HSM(addOne7[i], secondNum[i]);	
    prev = '0';
}

void SM_CON_RES(){
  	for(int i = 6; i > -1; i--)
  		resultNum[i] = HSM(addOne7[i], resultNum[i]);	// 7 разрядов	
    prev = '0';
}

char reverse(char c){
  if(c == '1') 
    return '0';
  else 
    return '1';
}

void conversionNum(){
  if(firstNum[0] == '1'){
  	for(int i = 1; i < 7; i++) //реверс всех, кроме знакового
    	firstNum[i] = reverse(firstNum[i]);
  }
  if(secondNum[0] == '1'){
  	for(int i = 1; i < 7; i++) //реверс всех, кроме знакового
    	secondNum[i] = reverse(secondNum[i]);
  }
	SM_CON();  
}

void conversionResult(){
  if(resultNum[0] == '1'){
  	for(int i = 1; i < 7; i++) //реверс всех, кроме знакового
    	resultNum[i] = reverse(resultNum[i]);
	SM_CON_RES();  
  }
}

void loop(){
  if(end)
    return;
  if(secondFull){
    end = 1;
    //conversion(&firstNum, INITIAN_BITS); \\перевод 1-го числа в доп код
    lcd.setCursor(0, 0);
    lcd.print(firstNum);
    lcd.print('+');
    lcd.print(secondNum);
    lcd.print('=');
    if(firstNum[0] != secondNum[0])
      conversionNum();
  	SM_8_BIT();
    if(firstNum[0] != secondNum[0])
    	conversionResult();
    if(firstNum[0] == secondNum[0] && resultNum[0] != firstNum[0])
     	overflow = 1;
    lcd.setCursor(0, 1);
    if(overflow == 1){
    	lcd.setCursor(15,1);
      	lcd.print("1");
    }
    lcd.print(resultNum);
    return;
  }
  lcd.setCursor(0,0);
  if(firstFull==0)
    lcd.print("First num:      ");
  else 
    lcd.print("Second num:");
  
    lcd.setCursor(0, 1);
  	printNum();
  buttonZeroState = digitalRead(5);
  buttonOneState = digitalRead(4);
  if (whichNum == 0 && !firstFull){
        if (buttonZeroState == HIGH){
            int pos = findPos();
            if (pos >= 0){
                firstNum[pos] = '0';
            }
        }
        if (buttonOneState == HIGH){
            int pos = findPos();
            if (pos >= 0){
                firstNum[pos] = '1';
            }
        }
    }
    if (whichNum == 1 && !secondFull){
        if (buttonZeroState == HIGH){
            int pos = findPos();
            if (pos >= 0){
                secondNum[pos] = '0';
            }
        }
        if (buttonOneState == HIGH){
            int pos = findPos();
            if (pos >= 0){
                secondNum[pos] = '1';
            }
        }
    }
  if(firstFull)
    whichNum = 1;
  delay(100);
}