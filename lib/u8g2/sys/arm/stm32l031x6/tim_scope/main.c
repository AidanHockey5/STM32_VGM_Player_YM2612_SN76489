/* 

  tim_scope for DC motor

  Example for the STM32L031 Eval Board with 128x64 OLED at PA13/PA14
  
  Single Mosfet Shield
  MOSFET: PA1 / AF2: TIM2_CH2
  VarRes: PA5 / ADC CH5
  Voltage sense: PA6 / ADC CH6
  
  
*/

#include <stdio.h>
#include "stm32l031xx.h"
#include "delay.h"
#include "u8g2.h"

/*=======================================================================*/
/* external functions */
uint8_t u8x8_gpio_and_delay_stm32l0(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/*=======================================================================*/
/* global variables */

u8g2_t u8g2;                    // u8g2 object
uint8_t u8g2_x, u8g2_y;         // current position on the screen

volatile unsigned long SysTickCount = 0;

/*=======================================================================*/

void __attribute__ ((interrupt, used)) SysTick_Handler(void)
{
  SysTickCount++;  
}



void setHSIClock()
{
  
  /* test if the current clock source is something else than HSI */
  if ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) 
  {
    /* enable HSI */
    RCC->CR |= RCC_CR_HSION;    
    /* wait until HSI becomes ready */
    while ( (RCC->CR & RCC_CR_HSIRDY) == 0 )
      ;      
 
    /* enable the HSI "divide by 4" bit */
    RCC->CR |= (uint32_t)(RCC_CR_HSIDIVEN);
    /* wait until the "divide by 4" flag is enabled */
    while((RCC->CR & RCC_CR_HSIDIVF) == 0)
      ;
    
       
    /* then use the HSI clock */
    RCC->CFGR = (RCC->CFGR & (uint32_t) (~RCC_CFGR_SW)) | RCC_CFGR_SW_HSI; 
    
    /* wait until HSI clock is used */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
      ;
  }
  
  /* disable PLL */
  RCC->CR &= (uint32_t)(~RCC_CR_PLLON);
  /* wait until PLL is inactive */
  while((RCC->CR & RCC_CR_PLLRDY) != 0)
    ;

  /* set latency to 1 wait state */
  FLASH->ACR |= FLASH_ACR_LATENCY;
  
  /* At this point the HSI runs with 4 MHz */
  /* Multiply by 16 device by 2 --> 32 MHz */
  RCC->CFGR = (RCC->CFGR & (~(RCC_CFGR_PLLMUL| RCC_CFGR_PLLDIV ))) | (RCC_CFGR_PLLMUL16 | RCC_CFGR_PLLDIV2); 
  
  /* enable PLL */
  RCC->CR |= RCC_CR_PLLON; 
  
  /* wait until the PLL is ready */
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    ;

  /* use the PLL has clock source */
  RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL); 
  /* wait until the PLL source is active */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
    ;

  SystemCoreClockUpdate();				/* Update SystemCoreClock global variable */  
}

/*
  Enable several power regions: PWR, GPIOA

  This must be executed after each reset.
*/
void startUp(void)
{  
  RCC->IOPENR |= RCC_IOPENR_IOPAEN;		/* Enable clock for GPIO Port A */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;	/* enable power interface (PWR) */
  PWR->CR |= PWR_CR_DBP;				/* activate write access to RCC->CSR and RTC */  
  
  SysTick->LOAD = (SystemCoreClock/1000)*50 - 1;   /* 50ms task */
  SysTick->VAL = 0;
  SysTick->CTRL = 7;   /* enable, generate interrupt (SysTick_Handler), do not divide by 2 */      
}

/*=======================================================================*/
/* u8x8 display procedures */

void initDisplay(void)
{
  
  /* setup display */
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_stm32l0);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_SetFont(&u8g2, u8g2_font_6x12_tf);
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawStr(&u8g2, 0,12, "STM32L031");
  u8g2_DrawStr(&u8g2, 0,24, u8x8_u8toa(SystemCoreClock/1000000, 2));
  u8g2_DrawStr(&u8g2, 20,24, "MHz");
  u8g2_SendBuffer(&u8g2);
  
  u8g2_x = 0;
  u8g2_y = 0;  
}


void outChar(uint8_t c)
{
  u8g2_x+=u8g2_DrawGlyph(&u8g2, u8g2_x, u8g2_y, c);
}

void outStr(const char *s)
{
  while( *s )
    outChar(*s++);
}

void outHexHalfByte(uint8_t b)
{
  b &= 0x0f;
  if ( b < 10 )
    outChar(b+'0');
  else
    outChar(b+'a'-10);
}

void outHex8(uint8_t b)
{
  outHexHalfByte(b >> 4);
  outHexHalfByte(b);
}

void outHex16(uint16_t v)
{
  outHex8(v>>8);
  outHex8(v);
}

void outHex32(uint32_t v)
{
  outHex16(v>>16);
  outHex16(v);
}

void setRow(uint8_t r)
{
  u8g2_x = 0;
  u8g2_y = r;
}

/*=======================================================================*/

/* STOP ANY ADC CONVERSION */

void stopADC(void)
{
  ADC1->CR |= ADC_CR_ADSTP;
  while(ADC1->CR & ADC_CR_ADSTP)
    ;
}

/* CONFIGURATION with ADEN=0 */
/* required to change the configuration of the ADC */
void disableADC(void)
{
  /* Check for the ADEN flag. */
  /* Setting ADDIS will fail if the ADC is alread disabled: The while loop will not terminate */
  if ((ADC1->CR & ADC_CR_ADEN) != 0) 
  {
    /* is this correct? i think we must use the disable flag here */
    ADC1->CR |= ADC_CR_ADDIS;
    while(ADC1->CR & ADC_CR_ADDIS)
      ;
  }
}

/* ENABLE ADC (but do not start) */
/* after the ADC is enabled, it must not be reconfigured */
void enableADC(void)
{
  ADC1->ISR |= ADC_ISR_ADRDY; 			/* clear ready flag */
  ADC1->CR |= ADC_CR_ADEN; 			/* enable ADC */
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* wait for ADC */
  {
  }
  
}


void initADC(void)
{
  //__disable_irq();
  
  /* ADC Clock Enable */
  
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;	/* enable ADC clock */
  __NOP();								/* let us wait for some time */
  __NOP();								/* let us wait for some time */  
  
  /* ADC Reset */
  
  RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
  __NOP();								/* let us wait for some time */
  __NOP();								/* let us wait for some time */
  RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;
  __NOP();								/* let us wait for some time */
  __NOP();								/* let us wait for some time */
 
  /* ADC Basic Setup */
  
  ADC1->IER = 0;						/* do not allow any interrupts */
  ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;	/* select HSI16 clock */
  ADC1->CFGR1 = ADC_CFGR1_RES_1;		/* 8 bit resolution */

  
  ADC1->CR |= ADC_CR_ADVREGEN;				/* enable ADC voltage regulator, probably not required, because this is automatically activated */
  ADC->CCR |= ADC_CCR_VREFEN; 			/* Wake-up the VREFINT */  
  ADC->CCR |= ADC_CCR_TSEN; 			/* Wake-up the temperature sensor */  

  __NOP();								/* let us wait for some time */
  __NOP();								/* let us wait for some time */

  /* CALIBRATION */
  
  if ((ADC1->CR & ADC_CR_ADEN) != 0) /* clear ADEN flag if required */
  {
  /* is this correct? i think we must use the disable flag here */
    ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);
  }
  ADC1->CR |= ADC_CR_ADCAL; 				/* start calibration */
  while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) 	/* wait for clibration finished */
  {
  }
  ADC1->ISR |= ADC_ISR_EOCAL; 			/* clear the status flag, by writing 1 to it */
  __NOP();								/* not sure why, but some nop's are required here, at least 4 of them */
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();


  /* CONFIGURATION with ADEN=0 */
  
  disableADC();
  
  //ADC1->CFGR1 &= ~ADC_CFGR1_RES;		/* 12 bit resolution */
  ADC1->CFGR1 = ADC_CFGR1_RES_1;		/* 8 bit resolution */
  
  enableADC();
}

/*
  ch0   PA0     pin 6
  ch1   PA1     pin 7
  ch2   PA2     pin 8
  ch3   PA3     pin 9
  ch4   PA4     pin 10
  ch5   PA5     pin 11
  ch6   PA6     pin 12
  ch7   PA7     pin 13
  ch8   PB0     -
  ch9   PB1     pin 14


  ch 0..15: 	GPIO
  ch 16:		???
  ch 17:		vref (bandgap)
  ch18:		temperature sensor

  returns 12 bit result, right aligned 
*/
uint16_t getADC(uint8_t ch)
{
  //uint32_t i;

  stopADC();
  disableADC();
    
  /* CONFIGURE ADC */
  
  //ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;	/* software enabled conversion start */
  //ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;		/* right alignment */
  ADC1->CFGR1 = ADC_CFGR1_RES_1;		/* 8 bit resolution */
  //ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; /* Select a sampling mode of 111 (very slow)*/

  ADC1->SMPR  = 0;
  enableADC();

  ADC1->CHSELR = 1<<ch; 				/* Select channel (can be done also if ADC is enabled) */

  /* DO CONVERSION */
  
  ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversion */
  while ((ADC1->ISR & ADC_ISR_EOC) == 0) /* wait end of conversion */
  {
  }
  
  return ADC1->DR;
}

void scanADC(uint8_t ch, uint16_t cnt, uint8_t *buf)
{
  
  stopADC();
  disableADC();
  
  
  RCC->AHBENR |= RCC_AHBENR_DMAEN; /* enable DMA clock */
  __NOP(); __NOP();                                           /* extra delay for clock stabilization required? */

  
  /* disable and reset to defaults */
  DMA1_Channel1->CCR = 0;
  
  /* defaults: 
      - 8 Bit access	--> ok
      - read from peripheral	--> ok
      - none-circular mode  --> ok
      - no increment mode   --> will be changed below
  */
  
  
  DMA1_Channel1->CNDTR = cnt;                                        /* buffer size */
  DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);                     /* source value */
  DMA1_Channel1->CMAR = (uint32_t)buf;                   /* destination memory */
  
  DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S;         /* 0000: select ADC for DMA CH 1 (this is reset default) */
  
  DMA1_Channel1->CCR |= DMA_CCR_MINC;		/* increment memory */   
  DMA1_Channel1->CCR |= DMA_CCR_EN;                /* enable */

  
  
  /* 
    detect rising edge on external trigger (ADC_CFGR1_EXTEN_0)
    recive trigger from TIM2 (ADC_CFGR1_EXTSEL_1)  
    8 Bit resolution (ADC_CFGR1_RES_1)
  
    Use DMA one shot mode and enable DMA (ADC_CFGR1_DMAEN)
    Once DMA is finished, it will disable continues mode (ADC_CFGR1_CONT)
  */
  
  
  ADC1->CFGR1 = ADC_CFGR1_EXTEN_0 	/* rising edge */
	| ADC_CFGR1_EXTSEL_1 			/* TIM2 */
	| ADC_CFGR1_RES_1				/* 8 Bit resolution */
	| ADC_CFGR1_CONT				/* continues mode */
	| ADC_CFGR1_DMAEN;			/* enable generation of DMA requests */

  //ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; 
  //ADC1->SMPR = ADC_SMPR_SMP_1 ; 
  ADC1->SMPR = ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 ; 
  
  /*
    12.5 + 8.5 = 21 ADC Cycles pre ADC sampling
    4 MHz / 21 cycle / 256 = 744 Hz
  */

  enableADC();
  
  /* conversion will be started automatically with rising edge of TIM2, yet ADSTART is still required */

  ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversion */

  /* wait until DMA is completed */
  while ( DMA1_Channel1->CNDTR > 0 )
    ;
  
}

/*
  special values:
    -1		Can not find level
    255	no rotation
    0..254	BMEF level, speed is k*(255-getBEMFLevel())
*/
int getBEMFLevel(uint16_t cnt, uint8_t *buf, uint16_t start)
{
  return -1;
}


/*=======================================================================*/

void initTIM(uint16_t tim_cycle)
{
  /* enable clock for TIM2 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  /*enable clock for GPIOA */
  RCC->IOPENR |= RCC_IOPENR_IOPAEN;		/* Enable clock for GPIO Port A */
  
    __NOP();                                                          /* extra delay for clock stabilization required? */
    __NOP();
  
  /* prescalar for AHB and APB1 */
  
  /* reselt defaults for HPRE and PPRE1: no clock division */
  // RCC->CFGR &= ~RCC_CFGR_HPRE;
  // RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  // RCC->CFGR &= ~RCC_CFGR_PPRE1;
  // RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
  
  /* configure GPIOA PA1 for TIM2 */
  GPIOA->MODER &= ~GPIO_MODER_MODE1;	/* clear mode for PA9 */  
  GPIOA->MODER |= GPIO_MODER_MODE1_1;  /* alt fn */
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_1;    /* push-pull */
  GPIOA->AFR[0] &= ~(15<<4);            /* Clear Alternate Function PA1 */
  GPIOA->AFR[0] |= 2<<4;                   /* AF2 Alternate Function PA1 */
  
  /* TIM2 configure */
  /* disable all interrupts */
  //TIM2->DIER = 0;             /* 0 is reset default value */
  
  /* clear everything, including the "Update disable" flag, so that updates */
  /* are generated */
  // TIM2->CR1 = 0;             /* 0 is reset default value */
  //TIM2->CR1 |= TIM_CR1_ARPE;  // ARR is not modified so constant update is ok
  /* Update request by manual UG bit setting or slave controller */
  /* both is not required here */
  /* so, update request by couter over/underflow remains */
  //TIM2->CR1 |= TIM_CR1_URS;             /* only udf/ovf generae events */
  
  TIM2->CR2 |= TIM_CR2_MMS_1;		/* Update event for TRGO */
  
  TIM2->ARR = 5355;                              /* total cycle count */
  TIM2->CCR2 = 1024;                            /* duty cycle */
  //TIM2->CCMR1 &= ~TIM_CCMR1_OC2CE;      /* disable clear output compare 2 **/
  TIM2->CCMR1 |= TIM_CCMR1_OC2M;            /* all 3 bits set: PWM Mode 2 */
  //TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_0;     /* 110: PWM Mode 1 */
  TIM2->CCMR1 |= TIM_CCMR1_OC2PE;            /* preload enable CCR2 is preloaded*/
  // TIM2->CCMR1 &= ~TIM_CCMR1_OC2FE;            /* fast disable (reset default) */
  // TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;               /* configure cc2 as output (this is reset default) */
  
  
  //TIM2->EGR  |=  TIM_EGR_CC2G;              /* capture event cc2 */
  TIM2->CCER |= TIM_CCER_CC2E;                     /* set output enable */
  //TIM2->CCER |= TIM_CCER_CC2P;                     /* polarity 0: normal (reset default) / 1: inverted*/
  
  TIM2->PSC = 7;						/* divide by 8 */
  
  TIM2->CR1 |= TIM_CR1_CEN;            /* counter enable */
  
  /*
    TIM2 cycle:
    32000000Hz / 5355 / 8 = 747 Hz  
  */
}

/*=======================================================================*/

#define BUF_MUL 2
#define TIM_CYCLE_TIME 5355
#define TIM_CYCLE_UPPER_SKIP 100
#define TIM_CYCLE_LOWER_SKIP 400

uint8_t adc_buf[128*BUF_MUL];

void main()
{
  uint16_t adc_value;
  uint16_t tim_duty;
  uint16_t zero_pos;
  uint16_t i;
  u8g2_uint_t y, yy;
  
  setHSIClock();        /* enable 32 MHz Clock */
  startUp();               /* enable systick irq and several power regions  */
  initDisplay();          /* aktivate display */
  initADC();

  RCC->IOPENR |= RCC_IOPENR_IOPAEN;		/* Enable clock for GPIO Port A */
  __NOP();
  __NOP();
  GPIOA->MODER &= ~GPIO_MODER_MODE1;	/* clear mode for PA1 */
  GPIOA->MODER |= GPIO_MODER_MODE1_0;	/* Output mode for PA1 */
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_1;	/* no Push/Pull for PA1 */
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEED1;	/* low speed for PA1 */
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;	/* no pullup/pulldown for PA1 */
  GPIOA->BSRR = GPIO_BSRR_BS_1;		/* atomic set PA1 */
  
  initTIM(TIM_CYCLE_TIME);
  
  

  for(;;)
  {
    
    u8g2_ClearBuffer(&u8g2);
   

    adc_value = getADC(5);
    tim_duty = ((uint32_t)adc_value*((uint32_t)TIM_CYCLE_TIME-TIM_CYCLE_UPPER_SKIP-TIM_CYCLE_LOWER_SKIP))>>8;
    tim_duty += TIM_CYCLE_LOWER_SKIP;
    TIM2->CCR2 = tim_duty;
    
    TIM2->SR &= ~TIM_SR_UIF;
    while( (TIM2->SR & TIM_SR_UIF) == 0 )
      ;
    
    yy = 30;
    for( i = 0; i < 128; i++ )
    {
      y = 30-(getADC(6)>>3);
      u8g2_DrawPixel(&u8g2, i, y);
      if ( y < yy )
	u8g2_DrawVLine(&u8g2, i, y, yy-y+1);
      else
	u8g2_DrawVLine(&u8g2, i, yy, y-yy+1);
      yy = y;
    }
    

    for( i = 0; i < 128*BUF_MUL; i++ )
      adc_buf[i] = i;
    
    
    scanADC(6, 128*BUF_MUL, adc_buf);

    yy = 60;
    
    zero_pos = ((uint32_t)tim_duty * (uint32_t)256) / (uint32_t)TIM_CYCLE_TIME;
    zero_pos +=4;
    zero_pos += (256-zero_pos)>>6;

    setRow(10); outHex16(adc_value); 
    outStr(" "); outHex8(adc_buf[0]); outStr(" "); outHex8(adc_buf[1]); outStr(" "); outHex8(adc_buf[2]);
    outStr("|"); outHex8(adc_buf[zero_pos/2]); outStr("|"); outHex8(adc_buf[zero_pos]); 
    
    u8g2_DrawVLine(&u8g2, zero_pos/2, yy-7, 15);
    u8g2_DrawVLine(&u8g2, zero_pos/4, yy-7, 15);
    for( i = 0; i < 128; i++ )
    {
      y = 60-(adc_buf[i*BUF_MUL]>>3);
      u8g2_DrawPixel(&u8g2, i, y);
      if ( y < yy )
	u8g2_DrawVLine(&u8g2, i, y, yy-y+1);
      else
	u8g2_DrawVLine(&u8g2, i, yy, y-yy+1);
      yy = y;
    }

    u8g2_SendBuffer(&u8g2);
    
  }
  
}
