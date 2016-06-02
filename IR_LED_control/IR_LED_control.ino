#include <idDHT11.h>
#include <IRLib.h>
  
// Assign pins
#define idDHT11pin 2
#define idDHT11intNumber 0 //interrupt number used by the DHT11 pin
#define GREEN_LED 4
#define RED_LED 5
#define IR_RECEIVER 6
#define RELAY_1 7

// Assign NEC button values
#define NEC_CHANNEL_DOWN 0xFFA25D
#define NEC_CHANNEL 0xFF629D
#define NEC_CHANNEL_UP 0xFFE21D
#define NEC_PREV 0xFF22DD
#define NEC_NEXT 0xFF02FD
#define NEC_PLAY 0xFFC23D
#define NEC_VOL_DOWN 0xFFE01F
#define NEC_VOL_UP 0xFFA857
#define NEC_EQ 0xFF906F
#define NEC_0 0xFF6897
#define NEC_100_PLUS 0xFF9867
#define NEC_200_PLUS 0xFFB04F
#define NEC_1 0xFF30CF
#define NEC_2 0xFF18E7
#define NEC_3 0xFF7A85
#define NEC_4 0xFF10EF
#define NEC_5 0xFF38C7
#define NEC_6 0xFF5AA5
#define NEC_7 0xFF42BD
#define NEC_8 0xFF4AB5
#define NEC_9 0xFF52AD
#define NEC_REPEAT 0xFFFFFFFF

// Assign SONY button values
#define SONY_MUTE 0x290
#define SONY_POWER 0xA90
#define SONY_SCREEN 0x1D0
#define SONY_TXT 0xFD0
#define SONY_AV_INPUT 0xA50
#define SONY_1 0x10
#define SONY_2 0x810
#define SONY_3 0x410
#define SONY_4 0xC10
#define SONY_5 0x210
#define SONY_6 0xA10
#define SONY_7 0x610
#define SONY_8 0xE10
#define SONY_9 0x110
#define SONY_DOUBLE_DIGIT 0xB90
#define SONY_0 0x910
#define SONY_INFO 0x5D0
#define SONY_ZOOM 0x5E25
#define SONY_MENU 0x70
#define SONY_GREEN_UP 0xB38
#define SONY_RED_MINUS 0x338
#define SONY_YELLOW_PLUS 0x738
#define SONY_BLUE_DOWN 0xF38
#define SONY_VOL_UP 0x490
#define SONY_VOL_DOWN 0xC90
#define SONY_PROG_UP 0x90
#define SONY_PROG_DOWN 0x890

IRrecv My_Receiver(IR_RECEIVER);//receiver on pin 11
IRdecode My_Decoder;//Decoder object

//declaration
void dht11_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);

// LED variables
byte redBrightness = 0;
byte fadeAmount = 15;

// IR Variables
byte sonyCount = 0;

// DHT11 Variables
unsigned long previousMillis = millis();

void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  pinMode(RELAY_1, OUTPUT);
  
  My_Receiver.enableIRIn(); // Start the receiver
}

// This wrapper is in charge of calling 
// must be defined like this for the lib to work
void dht11_wrapper() {
  DHT11.isrCallback();
}

void DHT11Update(){
  // Update DHT11 values.
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis > 2000)
  {
    // Sensor information retrieving
    Serial.print("\nRetrieving information from sensor: ");
    Serial.print("Read sensor: ");
    DHT11.acquire();
    while (DHT11.acquiring())
      ;
    int result = DHT11.getStatus();
    switch (result)
    {
    case IDDHTLIB_OK: 
      Serial.println("OK"); 
      break;
    case IDDHTLIB_ERROR_CHECKSUM: 
      Serial.println("Error\n\r\tChecksum error"); 
      break;
    case IDDHTLIB_ERROR_ISR_TIMEOUT: 
      Serial.println("Error\n\r\tISR Time out error"); 
      break;
    case IDDHTLIB_ERROR_RESPONSE_TIMEOUT: 
      Serial.println("Error\n\r\tResponse time out error"); 
      break;
    case IDDHTLIB_ERROR_DATA_TIMEOUT: 
      Serial.println("Error\n\r\tData time out error"); 
      break;
    case IDDHTLIB_ERROR_ACQUIRING: 
      Serial.println("Error\n\r\tAcquiring"); 
      break;
    case IDDHTLIB_ERROR_DELTA: 
      Serial.println("Error\n\r\tDelta time to small"); 
      break;
    case IDDHTLIB_ERROR_NOTSTARTED: 
      Serial.println("Error\n\r\tNot started"); 
      break;
    default: 
      Serial.println("Unknown error"); 
      break;
    }
    Serial.print("Humidity (%): ");
    Serial.println(DHT11.getHumidity(), 2);
  
    Serial.print("Temperature (oC): ");
    Serial.println(DHT11.getCelsius(), 2);
  
    Serial.print("Temperature (oF): ");
    Serial.println(DHT11.getFahrenheit(), 2);
  
    Serial.print("Temperature (K): ");
    Serial.println(DHT11.getKelvin(), 2);
  
    Serial.print("Dew Point (oC): ");
    Serial.println(DHT11.getDewPoint());
  
    Serial.print("Dew Point Slow (oC): ");
    Serial.println(DHT11.getDewPointSlow());
  
    previousMillis = currentMillis;    
  }
  else
  {
  }
}

void irRemoteUpdate(){
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
//    My_Decoder.DumpResults();
    if (My_Decoder.decode_type == NEC) {
      switch(My_Decoder.value) {
        case NEC_CHANNEL_DOWN:  //Channel -
          if (digitalRead(RED_LED) == HIGH)
          {
            digitalWrite(RED_LED, LOW);// Turn off Red LED
            redBrightness = 0;
            Serial.println("RED LED off");
          }
          else
          {
            digitalWrite(RED_LED, HIGH);// Turn on Red LED
            redBrightness = 255;
            Serial.println("RED LED on");
          }
          break;
        case NEC_CHANNEL_UP:  //Channel +
          if (digitalRead(GREEN_LED) == HIGH)
          {
            digitalWrite(GREEN_LED, LOW);// Turn off Green LED
            Serial.println("GREEN LED off");
          }
          else
          {
            digitalWrite(GREEN_LED, HIGH);// Turn on Green LED
            Serial.println("GREEN LED on");
          }
          break;
        case NEC_CHANNEL:  //Channel
          if ((digitalRead(GREEN_LED) == HIGH) || (redBrightness > 0))
          {
            digitalWrite(GREEN_LED, LOW);// Turn off Green LED
            digitalWrite(RED_LED, LOW);// Turn off Red LED
            redBrightness = 0;
            Serial.println("ALL LEDs off");
          }
          else
          {
            digitalWrite(GREEN_LED, HIGH);// Turn on Green LED
            digitalWrite(RED_LED, HIGH);// Turn on Red LED
            redBrightness = 255;
            Serial.println("ALL LEDs on");
          }
          break;
        case NEC_VOL_DOWN:  // Vol -
          if (redBrightness > 0)
          {
            redBrightness -= fadeAmount;
            analogWrite(RED_LED, redBrightness);
          }
          Serial.print("RED LED Brightness: ");
          Serial.println(redBrightness);
          break;
        case NEC_VOL_UP:  // Vol +
          if (redBrightness < 255)
          {
            redBrightness += fadeAmount;
            analogWrite(RED_LED, redBrightness); 
          }
          Serial.print("RED LED Brightness: ");
          Serial.println(redBrightness);
          break;
        case NEC_1:  //Number 1 (Relay 1)
          if (digitalRead(RELAY_1) == HIGH)
          {
            digitalWrite(RELAY_1, LOW);
            Serial.println("Relay 1 on");
          }
          else
          {
            digitalWrite(RELAY_1, HIGH);
            Serial.println("Relay 1 off");
          }
          break;
        case NEC_REPEAT: //Repeat signal of previous press
          break;
        default:
          Serial.println("NEC - Unknown command");
        break;
      }
    }
    else if (My_Decoder.decode_type == SONY)
    {
      sonyCount += 1;
      if (sonyCount == 3)
      {
        switch(My_Decoder.value) {
          case SONY_RED_MINUS:  //Red/-
            if (digitalRead(RED_LED) == HIGH)
            {
              digitalWrite(RED_LED, LOW);// Turn off Red LED
              redBrightness = 0;
              Serial.println("RED LED off");
            }
            else
            {
              digitalWrite(RED_LED, HIGH);// Turn on Red LED
              redBrightness = 255;
              Serial.println("RED LED on");
            }
            break;
          case SONY_GREEN_UP:  //Green/up
            if (digitalRead(GREEN_LED) == HIGH)
            {
              digitalWrite(GREEN_LED, LOW);// Turn off Green LED
              Serial.println("GREEN LED off");
            }
            else
            {
              digitalWrite(GREEN_LED, HIGH);// Turn on Green LED
              Serial.println("GREEN LED on");
            }
            break;
          case SONY_POWER:  //Power
            if ((digitalRead(GREEN_LED) == HIGH) || (redBrightness > 0))
            {
              digitalWrite(GREEN_LED, LOW);// Turn off Green LED
              digitalWrite(RED_LED, LOW);// Turn off Red LED
              redBrightness = 0;
              Serial.println("ALL LEDs off");
            }
            else
            {
              digitalWrite(GREEN_LED, HIGH);// Turn on Green LED
              digitalWrite(RED_LED, HIGH);// Turn on Red LED
              redBrightness = 255;
              Serial.println("ALL LEDs on");
            }
            break;
          case SONY_VOL_DOWN:  // Vol -
            if (redBrightness > 0)
            {
              redBrightness -= fadeAmount;
              analogWrite(RED_LED, redBrightness);
            }
            Serial.print("RED LED Brightness: ");
            Serial.println(redBrightness);
            break;
          case SONY_VOL_UP:  // Vol +
            if (redBrightness < 255)
            {
              redBrightness += fadeAmount;
              analogWrite(RED_LED, redBrightness); 
            }
            Serial.print("RED LED Brightness: ");
            Serial.println(redBrightness);
            break;
          case SONY_1:  //Number 1 (Relay 1)
            if (digitalRead(RELAY_1) == HIGH)
            {
              digitalWrite(RELAY_1, LOW);
              Serial.println("Relay 1 on");
            }
            else
            {
              digitalWrite(RELAY_1, HIGH);
              Serial.println("Relay 1 off");
            }
            break;
          default:
            Serial.println("SONY - Unknown command");
          break;
        }
        sonyCount= 0;
      }
      else
      {
        
      }
    }
    else{
      Serial.println("Invalid Input!");
    }
    My_Receiver.resume(); //Restart the receiver
  }
}

void loop() {
  // DHT11 information retrieving
  DHT11Update();
  
  // IR receiver information retrieving
  irRemoteUpdate();
}
