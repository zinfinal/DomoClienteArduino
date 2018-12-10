#include <Ethernet.h>
#include <SPI.h>

#include <AnalogEvent.h>

byte ip[] = { 192, 168, 0, 84 };
byte mac[] = { 0xDE, 0xAB, 0xBE, 0xBE, 0xAB, 0xBE };

byte servidor[] = { 192, 168, 0, 26 };

EthernetClient cliente;

int primercambio;

void setup() {
  Ethernet.begin(mac);
  //Serial.begin(9600);

  primercambio = 0;
  
  if (cliente.connect(servidor, 7070)) {
    //Serial.println("Conectado");
    byte opCode = 0xC1;
    byte sizeCode = 0x05;
    byte headCode = 0xF1;
    byte subCode = 0x02;
    byte id = 0x01;
    
    cliente.write(opCode);
    cliente.write(sizeCode);
    cliente.write(headCode);
    cliente.write(subCode);
    cliente.write(id);
    cliente.flush();

    AnalogEvent.addAnalogPort(1, onChange, 3);
  } else {
    //Serial.println("Conexion fallo");
  }

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {
  if (cliente.available()) {
    
    byte opCode = cliente.read();
    byte sizeCode = cliente.read();
    byte headCode = cliente.read();
    byte subCode = cliente.read();
    
    switch(opCode){
      case 0xC2:
        switch(subCode){
          case 0x01:
            focos();
            break;
          case 0x02:
            openTheDoor();
            break;
        }
        break;
      case 0xC3:
      
        //temperatura = analogRead(A0); //Lee los datos del sensor LM35
        //temperatura = (5.0 * temperatura * 100)/1023.0; //Formula para convertir a ºC
        /*
        byte opCode = 0xC3;
        byte sizeCode = 0x04;
        byte headCode = 0xF1;
        byte subCode = 0x05;
        
        //byte id = 0x01;
        
        cliente.write(opCode);
        cliente.write(sizeCode);
        cliente.write(headCode);
        cliente.write(subCode);
    
        //cliente.write(id);
        cliente.write(temperatura);
        
        Serial.println("TEMPERA:" + temperatura);
        */
        break;
    }
    
  }
  
  if (!cliente.connected()) {
    //Serial.println("desconectando.");
    cliente.stop();
    
    setup();
  }

  AnalogEvent.loop();
}

void focos(){
  char pin = cliente.read();
  char estado = cliente.read();
  /*
  opCode = 0xC3;
  sizeCode = 0x06;
  headCode = 0xF1;
  subCode = 0x06;
  */
  switch(estado){
    case 0x00:
      digitalWrite(pin, LOW);
      break;
    case 0x01:
      digitalWrite(pin, HIGH);
      break;
  }
}

void openTheDoor(){
  char pin = cliente.read();
  
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
}

void onChange(AnalogPortInformation* Sender){
  //Serial.print(Sender->pin);
  //Serial.print(Sender->value);
  
  int temperatura = (5.0 * Sender->value * 100)/1023.0;
  
  byte data[] = {0xC3, 0x06, 0xF1, 0x01, temperatura};
  
  if(primercambio == 0){
    delay(1000);
    primercambio = 1;
  } else {
    cliente.write(data, 5);
    cliente.flush();
  }
}
