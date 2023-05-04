#include <Servo.h>
//importujemo biblioteku za servo...definisemo nazive koje smo povezali na nas protobord i pinove koje smo povezali sa nasim arduinom
#define LEDRED 12
#define Button 2
#define PHR A1
#define LM35 A0
#define PIRPin 3
#define RelejPin 13
#define ServoPin 10
#define trigPin 8
#define echoPin 9

Servo mojservo;
int flagRucno = 0;
int flagRucnoT = 0;
int flagRucnoR = 0;
int flagRucnoP = 0;


int osvetljenje;
float temperatura;
String Poruka;

void IndikacijaTemperature()
{
 temperatura = (analogRead(LM35)*500.0)/1023;      //formula za temp transformise digitalnu vrednost dobijenu sa izlaza AD konvertora sa Arduina na Celzijusovoj skali
  Serial.println(temperatura);
   if(flagRucnoT == 0){
    if(temperatura>30){                                             //ukoliko je temp veca od 30 bice upaljena dioda 
     digitalWrite(RelejPin, HIGH);
    } else {
      digitalWrite(RelejPin, LOW);                                  //ukoliko nije bice ugasena
    }
  }
}

void AutomatskaKontrolaOsvetljenja()
{
  osvetljenje = map(analogRead(PHR),0, 1023, 0, 100); //mapirali samo vrednost koju dobijamo od 0 1023 inicijalno da bude u 0 do 100 opsegu kako bi imali procenat od 0 do 100 umesto vrednosti od 0 do 1023 za osvetljenje -formula za osv
  Serial.println(osvetljenje);
  if(flagRucno == 0){
    if(osvetljenje<30){
      digitalWrite(LEDRED, HIGH);                                 //ukoliko je osv manje od 30 dioda ce biti upaljena
    } else {
      digitalWrite(LEDRED, LOW);                                  //ukoliko je vece bice ugasena
    }
  }
}

void IndikacijaRazdaljine(){
  
long duration, distance;

digitalWrite(trigPin, LOW);

delayMicroseconds(2);

digitalWrite(trigPin, HIGH);

delayMicroseconds(10);

digitalWrite(trigPin, LOW);

duration = pulseIn(echoPin, HIGH);

distance = (duration/2) / 29.1;

if(flagRucnoR == 0){
if (distance < 10)
{
//Serial.println("the distance is less than 10");
mojservo.write(90);                                   //ukoliko je distanca manje od 10cm vrata ce se otvoriti 
}

else {
mojservo.write(0);                                    //ukoliko je distanca veca od 10cm bice zatvorena
}

if (distance > 10 || distance <= 0)
{
Serial.println("The distance doesn't more than 10cm"); //takodje korisnik ce biti obavesten o tome da je distanca veca od 10cm i da zato ne mogu biti otvorena vrata
delay(5000);
}

else {
Serial.print(distance);
Serial.println(" cm");
}
}
delay(2000); 
}
void IndikacijaPokreta()
{
  int OcitavanjePokreta = digitalRead(PIRPin);
  if(flagRucnoP == 0){
  if(OcitavanjePokreta == HIGH){
    digitalWrite(LEDRED, HIGH);                                 //ocitavanje pokreta...ukoliko se pokret detektuje bice upaljena dioda na 10sec
    Serial.println("Pokret detektovan ");
    delay(10000);
  }else {
   digitalWrite(LEDRED, LOW);                                   //ukoliko ne ocita bice ugasena dioda
    
  }
  
 }
 
}

void ButtonISR()
{
  flagRucno = !flagRucno;             //promenice svoje pocetno stanje i mocice da se ugasi pomocu serijskog monitora
  flagRucnoT = !flagRucnoT;           //promenice svoje pocetno stanje i mocice da se ugasi pomocu serijskog monitora
  flagRucnoR = !flagRucnoR;           //promenice svoje pocetno stanje i mocice da se ugasi pomocu serijskog monitora
  flagRucnoP = !flagRucnoP;  
   if(digitalRead(Button) == LOW){
    digitalWrite(LEDRED, HIGH);
  }else{
    digitalWrite(LEDRED,LOW);
  }//promenice svoje pocetno stanje i mocice da se ugasi pomocu serijskog monitora
  //Serial.write("sendreport");
}

void setup() {
  // put your setup code here, to run once:
  // definisemo komponente koje smo povezali sa protobordom
  pinMode(LEDRED, OUTPUT);
  pinMode(PIRPin, INPUT);
  pinMode(Button, INPUT);
  pinMode(PHR, INPUT);
  pinMode(RelejPin, OUTPUT);
  pinMode(LM35, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mojservo.attach(10);                                                    //definisanje serva na pinu br.10

  attachInterrupt(digitalPinToInterrupt(Button), ButtonISR, FALLING);     //definisemo dugme koje ce kao prekid izvrsavati neku akciju...u kodu navedenom gore ce uvek pritiskom na dugme biti upaljena dioda

  Serial.begin(9600);                                                     //navodimo da se radi o serijskoj komunikaciji

  digitalWrite(LEDRED, LOW);                                              //diodao je na pocetku ugasena
}

void loop() {
  // put your main code here, to run repeatedly:
  // pozivamo funkcije koje ce se non-stop izvrsavati u loop-u, a te funkcije su navedene gore u kodu svaka ima svoj zadatak izvrsavanja
  IndikacijaRazdaljine();
  IndikacijaPokreta();
  IndikacijaTemperature();
  AutomatskaKontrolaOsvetljenja();

  if(Serial.available() > 0){             //izvrsice se ukoliko se ispise poruka koja je navedena dole
    Poruka = Serial.readString();         //ocivatanje poruke
    //Serial.println(Poruka);
    if(Poruka == "upali"){
      flagRucno = 1;
      digitalWrite(LEDRED, HIGH);
    } else if(Poruka == "ugasi"){
      flagRucno = 1;
      digitalWrite(LEDRED, LOW);
    } else if(Poruka == "upalimod"){
      flagRucno = 0;                    //generisemo automatsko paljenje mod-a...to bi znacli da ne mozemo preko serijskog monitora da menjamo stanja
    } else if(Poruka == "ugasimod"){
      flagRucno = 1;                    //generisemo automatsko gasenje mod-a...to bi znacilo da mozemo da menjamo stanja preko serisjkog monitora
    } else if(Poruka == "upalitemp"){
      flagRucnoT = 1;                   //kontrola preko s-monitora da bude upaljeno bez obzira na kolika je temp
      digitalWrite(LEDRED, HIGH);
    } else if(Poruka == "ugasitemp"){
      flagRucnoT = 1;                   //gasenje preko s-monitra diode bez obzira kolika je temperatura
      digitalWrite(LEDRED, LOW);
    }
     else if(Poruka == "upalirazmak"){
      flagRucnoR = 1;                   //kontrola preko s-monitora da bude upaljeno bez obzira na kolika je razdaljina
      digitalWrite(LEDRED, HIGH);
    } else if(Poruka == "ugasirazmak"){
      flagRucnoR = 1;                   //gasenje preko s-monitra diode bez obzira kolika je razdaljina
      digitalWrite(LEDRED, LOW);
    }
     else if(Poruka == "upalipokret"){
      flagRucnoP = 1;                   //kontrola preko s-monitora da bude upaljeno bez obzira da li je detektovan pokret
      digitalWrite(LEDRED, HIGH);
    } else if(Poruka == "ugasipokret"){
      flagRucnoP = 1;                   //gasenje preko s-monitra diode bez obzira da li je detektovan pokret 
      digitalWrite(LEDRED, LOW);
    }
  }
  
}
