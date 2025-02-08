#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Servo.h>

SoftwareSerial myserial(2, 3);
String inputString = "";         
boolean stringComplete = false;  

Servo myservo;

int readval;

boolean firstconn;
boolean changepass;
int Doorstatus;
int Dooraddress=100;

int resultvalue;
int checktime=0;

String readpassword;
String readstr="";
String Password;
String msg;
int pwdlen=0;

int passstatus=0;

void setup() {
  Serial.begin(9600);
  myserial.begin(9600);  
  inputString.reserve(200);

  myservo.attach(5);

  firstconn=false;
  changepass=false;
  readpassword="";
  Password=readpass();  
  Serial.print(Password);
  Doorstatus=EEPROM.read(Dooraddress);
  
  if (Doorstatus==1)
  {
      myservo.write(180);    
      delay(100);    
  }
  else
  {
      myservo.write(0);    
      delay(100);        
  } 
  //writepass("123456789");  
}

void loop() {
  serialEvent(); 
  if (stringComplete) {
    pwdlen=inputString.length();
    pwdlen=pwdlen-1;
    readstr = inputString.substring(1,pwdlen);

    //Serial.println(inputString);
    //Serial.println(readstr);
        
    if (readstr == "NEW")
    {
      passstatus=0;
      myserial.println("WH");
    }
    else if((readstr == "OPEN") && (passstatus>0))
    {
      //Serial.println("Door Open");
      myserial.println("DO");
      Doorstatus=1;
      EEPROM.write(Dooraddress,Doorstatus);
      myservo.write(180);    
      delay(100);
    }    
    else if((readstr == "CLOSE") && (passstatus>0))
    {
      myserial.println("DC");
      //Serial.println("Door Close");
      Doorstatus=0;
      EEPROM.write(Dooraddress,Doorstatus);
      myservo.write(0);    
      delay(100);      
    }
    else if((readstr == Password) && (passstatus==0))
    {
      passstatus=1;
      myserial.println("C1");
      //Serial.println("Correct password");
    }
    else if((readstr != Password) && (passstatus==0))
    {
      passstatus=0;
      myserial.println("E1");
      //Serial.println("Incorrect password");
    }    
    else if(readstr == "CPW")
    {
      passstatus=2;
      //Serial.println("Change Password Request");
    }       
    else if((readstr == Password) && (passstatus==2))
    {
      passstatus=3;
      myserial.println("NP");
      //Serial.println("Correct password");
    }    
    else if((readstr != Password) && (passstatus==2))
    {
      passstatus=1;
      myserial.println("E1");
      //Serial.println("InCorrect password");
    }        
    else if(passstatus==3)
    {
      passstatus=1;
      writepass(readstr);
      myserial.println("PC");
      //Serial.println("Password changed ");
    }
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (myserial.available()) {
    char inChar = (char)myserial.read();
    inputString += inChar;
    if (inChar == '$') {
      stringComplete = true;
    }
  }
}


String readpass()
{
  String mempass;
  int memval;
  int memadd;
  mempass="";
  for(int i=0;i<9;i++)
  {
    memadd=200+i;
    memval=EEPROM.read(memadd);
    if (memval>9)
    {
      memval=0;
    }
    if (memval==0)
    {
      break;
    }
    else
    {
      mempass=mempass + memval;
    }
  }
  return mempass;
}


void writepass(String passw)
{
  Password=passw;
  String mempass;
  int memval;
  int memadd;
  int len;
  len=passw.length();
  for(int i=0;i<9;i++)
  {
    if (i<len)
    {
      mempass=passw.substring(i,i+1);
      memval=mempass.toInt();
    }
    else
    {
       memval=0;
    }
    memadd=200+i;
    EEPROM.write(memadd,memval);
  }
}
