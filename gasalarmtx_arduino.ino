#define BitIsClear(reg, bit)    ((reg & (1<<bit)) == 0)
#define SetBit(reg, bit)          reg |= (1<<bit)           
#define ClearBit(reg, bit)       reg &= (~(1<<bit))
#define InvBit(reg, bit)          reg ^= (1<<bit)
#define BitIsSet(reg, bit)       ((reg & (1<<bit)) != 0)

#define heatsens()            PORTB |= (1<<PINB2)
#define ledon()               PORTB |=(1<<PINB5)
#define ledoff()              PORTB &= ~(1<<PINB5)
#define txhigh()              PORTB |= (1<<PINB4)
#define txlow()               PORTB &= ~(1<<PINB4)
#define zummeron()            PORTB |= (1<<PINB3)
#define zummeroff()           PORTB &= ~(1<<PINB3)
#define cfg_port_direct()     DDRB |= (1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5)|(1<<PORTB4)
#define adcmux_init()         ADMUX |= (0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)|(0<<REFS1)|(1<<REFS0) // ADC0(ardpin A0), Vref = Uin.
#define adccontr_init()       ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // f = 128кГц 

unsigned int m,k, i, j, a, adc_data;
unsigned char txbit1[32]={1,0,1,1,0,0,1,1,1,0,1,0,0,0,0,0,1,1,1,1,1,0,0,1,1,0,0,0,1,1,0,0};
unsigned char txbit2[32]={0,1,0,0,1,1,0,0,0,1,0,1,1,1,1,1,0,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1};

volatile float d0 = 5;  
volatile float d1 = 0.3;
volatile float d2 = 15;
volatile float d3 = 0.145;
volatile float d4 = 10000;
volatile float d5 = 240000;
volatile float d6 = 65000;
int adc_level = 400;

//Тревога, повторяется
void alarm(void){
//Передача сигнала аварии по 433МГц
        ledon(); //LED On (ardpin13)
      for(i=0;i<100;i++){  
        //Старт-бит? 
        txhigh();//ardpin 11
        delay(d0); //10ms
        txlow();//ardpin 11
    
        //Передача битов
      for (m = 0;  m < 32; m++){
        if(txbit1[m] == 1){
          txhigh();//ardpin 11
      }
        else{
          txlow();//ardpin 11
         }
          delay(d1);//5ms
          txlow();//ardpin 11
      }
                                
      for (m = 0;  m < 32; m++){
        if(txbit2[m] == 1){
          txhigh();//ardpin 11
       }
        else{
          txlow();//ardpin 11
        }
          delay(d1);//5ms
          txlow();//ardpin 11
      } 
          delay(d2);
    }    

//Звуковая и световая сигнализация     
  for(k=0;k<50;k++){  
    for(j=0;j<65000;j++){
        zummeron(); //Пищалка вкл.(ardpin 12) 
        delay(d3);
        zummeroff(); //Пищалка выкл.(ardpin 12)  
        delay(d3);
     }
   } 
  
}

//Отслеживаем наступление срабатывания датчика по каналу AOUT (DOUT c компаратора)
void check(void){
  if(adc_data > adc_level){ // сравниваем результат преобразования 400 ~5в,(584 ~3.5в). На входе АЦП 2 вольта.  ADC = (Vin * 1023)/Vref
     alarm();
    }    
         
}

// ADC interrupt service routine
ISR (ADC_vect){ 
     ledon(); //LED On (ardpin 13)
     delay(d4);
     ledoff();//LED Off (ardpin 13)
     delay(d5);   
 
   for(a=0;a<50;a++) // Make 50 mesure
    {
     ADCSRA |= (1<<ADSC);                      // start new A/D conversion
     while (!(ADCSRA & (1 << ADIF)));      // wait until ADC is ready
     adc_data = adc_data+ADCW;         
    }     
     adc_data = adc_data/50;       
     check();       
}

void setup() {
//Направление работы порта.
  cfg_port_direct(); //Для подачи питания на датчик, выход на зуммер
//Выкл. аналог.компаратора    
  ACSR=0x80;
//Инициализация АЦП. 
  adcmux_init();
  adccontr_init();
           
  sei();             
    
//Греем датчик
   heatsens(); //ardpin 10   
   delay(d6);    
   ADCSRA |= (1<<ADSC);   
          
}

void loop() {

}
