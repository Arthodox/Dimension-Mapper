#include <LiquidCrystal_I2C.h>

/*-------------------------------------------------------------------------------------------------------------*/
LiquidCrystal_I2C lcd(0x27,20,4);//For proteus Simulations set it to (0x20,20,4), for Arduino uses (0x27,20,4).
//Sets Pins
const int Start = 3, Set = 2, Read = 4;
const int ML_1 = 11,ML_2 = 10, MR_1 = 9,MR_2 =8; 
const int LDR = A1,LDRL = A0, LDRR = A2;

//Sets Variables
int walls = 0, i = 0, r = 0, temp = 0;
bool ON_OFF= 0, B_Sides,R_Sides;
float timer = 0,elapsed = 0,distance = 0;
float dims[10],e_time[10];
const float M_Sp = 1.1736; //Motor Speed in m/s 
/*-------------------------------------------------------------------------------------------------------------*/

void setup() {
  //pin Setups
  pinMode(Start, INPUT);
  pinMode(Set, INPUT);
  pinMode(Read, INPUT);
  pinMode(ML_1, OUTPUT);
  pinMode(ML_2, OUTPUT);
  pinMode(MR_1, OUTPUT);
  pinMode(MR_2,OUTPUT);
 
  
  //LCD Initialization
  lcd.begin(20,4);
  lcd.init();
  lcd.init();
  lcd_call();
  lcd.clear();
  lcd_call();
  lcd.clear();
  lcd.setCursor(4,1);
  lcd.print("-=Welcome=-");
  lcd.setCursor(2,2);
  lcd.print("Dimension Mapper");
  delay(2000);
  lcd.clear();
}
/*-------------------------------------------------------------------------------------------------------------*/

void lcd_call(){
  lcd.backlight();
  lcd.setCursor(2,1);
  lcd.print("Dimension Mapper");
  lcd.setCursor(1,2);
  lcd.print("Initializing ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
}
/*-------------------------------------------------------------------------------------------------------------*/

//Moves Driver Forward
void motor_forward(){
  digitalWrite(ML_1,LOW);
  digitalWrite(ML_2,HIGH);
  digitalWrite(MR_1,LOW);
  digitalWrite(MR_2,HIGH);
}
/*-------------------------------------------------------------------------------------------------------------*/

//Turns Robot Left
void motor_left(){
  digitalWrite(ML_1,HIGH);
  digitalWrite(ML_2,LOW);
  digitalWrite(MR_1,LOW);
  digitalWrite(MR_2,HIGH);
} 
/*-------------------------------------------------------------------------------------------------------------*/

//Turns Robot Right
void motor_right(){
  digitalWrite(ML_1,LOW);
  digitalWrite(ML_2,HIGH);
  digitalWrite(MR_1,HIGH);
  digitalWrite(MR_2,LOW);
} 
/*-------------------------------------------------------------------------------------------------------------*/

//Stops Robot 
void motor_stop(){
  digitalWrite(ML_1,LOW);
  digitalWrite(ML_2,LOW);
  digitalWrite(MR_1,LOW);
  digitalWrite(MR_2,LOW);
}
/*-------------------------------------------------------------------------------------------------------------*/

//LCD Loop Auto Update when Runned
void lcd_display(){
  // Line 1
    lcd.setCursor(1,1);
    lcd.print("Parameter Settings");
  
  // Line 2 (Number of Wall Input)
    lcd.setCursor(1,2);
    lcd.print("Number of Walls: ");
    lcd.print(walls);
    
    delay(250);
    lcd.clear();
}
/*-------------------------------------------------------------------------------------------------------------*/

//LCD Display While Running
void lcd_run(){
    lcd.print(" Running ");

    //Line 1 (Remaining Walls)
    lcd.setCursor(1, 1);
    lcd.print("Remaining Turns ");
    lcd.print(walls);
  
    //Line 2 (Time Elapsed)
    lcd.setCursor(1, 2);
    lcd.print("Time: ");
    lcd.print((elapsed)/1000);
    lcd.print(" s");
    
    //Line 3 (Length Scanned)
    distance = ((elapsed * M_Sp)/1000);
    lcd.setCursor(1,3);
    lcd.print("Length= ");
    lcd.print(distance);
    lcd.print(" m");

    lcd.setCursor(9,0);
    lcd.print(". ");
    delay(75);
    lcd.print(". ");
    delay(75);
    lcd.print(". ");
    delay(50);

    if(walls==0){
      lcd.setCursor(1,1);
      lcd.print("Run Completed");
      lcd.setCursor(1,2);
      lcd.print("Toggle Read Mode To");
      lcd.setCursor(1,3);
      lcd.print("Check Recorded Data");
    }

    delay(100);
    lcd.clear();
}
/*-------------------------------------------------------------------------------------------------------------*/

//LCD when data reading is initiated
void lcd_read(){
  //Checks If data is available and If not Displays an ERROR
  if(temp==0){ 
    lcd.setCursor(1,0);
    lcd.print("NO Data Available");
    lcd.setCursor(0,2);
    lcd.print("Run the Robot to Get");
    lcd.setCursor(0,3);
    lcd.print("Required Data");
  }
  
  else{
    //Sets show limit so doesn't show empty data 
    if((r+1)>temp){ 
      r=0;
    }
    //Prints Info to LCD Display    
    lcd.print("+=Recorded Data=+");
    lcd.setCursor(0,1);
    lcd.print("Length ");
    lcd.print(r+1);
    lcd.print(": ");
    lcd.print(dims[r]);
    lcd.print(" m");
    
    lcd.setCursor(0,2);
    lcd.print("Time Elapsed: ");
    lcd.print(e_time[r]);
    lcd.print("s");
    delay(300);
    lcd.clear();
  }
}
/*-------------------------------------------------------------------------------------------------------------*/

//Stores data and initialized next time 
void store(){
  dims[i] = distance;
  e_time[i] = elapsed/1000;
  motor_stop();
  i++;
  walls--;  
  delay(500);
  timer=millis();
}
/*-------------------------------------------------------------------------------------------------------------*/

//Checks Left and Right LDRS
void checker(){
    if(analogRead(LDRL)<35 && analogRead(LDRR)<35){
      B_Sides = 1;
    }
    if(analogRead(LDRL)>=75 && analogRead(LDRR)<35){
      R_Sides = 1;
      B_Sides = 0;
    }
    if(analogRead(LDRL)<35 && analogRead(LDRR)>=75){
      R_Sides = 0;
      B_Sides = 0;
    }
}


void loop() {
  //Changes mode from robot parameter do Data record
  if(digitalRead(Read)== HIGH){
    lcd_read();
  //Set for Reading Data
    if(digitalRead(Set)== HIGH){
        r++;
        delay(300);
      }
  }

  else{
  //Set For Run Mode
    lcd_display();
    checker();

    //Wall Counter
      if(digitalRead(Set)== HIGH){
        walls++;       
        delay(300);
        if(walls>9){
          walls=0;          
        }
      }
  
    //Timer Counter
      if(digitalRead(Start)==HIGH){
        timer=millis();
        ON_OFF=1;
        temp = walls;
      }
  
    //Motor Control (LDR Sensitive)
      if(ON_OFF == 1){
        //Checks for number of Walls
          while(walls>=1){
            checker();
            elapsed  = millis()-timer;
            
            //When LDR is no longer recieving lights            
            if(analogRead(LDR)>=75 && B_Sides == 0){
              motor_stop();             
              delay(1000);
              
              //Stop Spinning after adequite distance from next wall
              while(analogRead(LDR)>35){
                  if(R_Sides==0){
                    motor_right();
                    lcd_run();
                  }
                  
                  else{
                    motor_left();
                    lcd_run();
                  }                  
               }
               store();
            }

            else if (B_Sides == 1){
              store();
              delay(100);
              while(elapsed<2000){
                if(R_Sides==0){
                  motor_left();
                  lcd_run();
                }
                
                else{
                  motor_right();
                  lcd_run();
                }
              }
              timer=millis();

              while(B_Sides == 1){
                checker();
                lcd_run();
                motor_forward();
              }
            }
            
            else{
              lcd_run();
              motor_forward();
            }        
          }
          
          //Turns cancels the start input if number of walls is not Set
          if(walls==0){
            ON_OFF=0;
          }
      }
  }
}
