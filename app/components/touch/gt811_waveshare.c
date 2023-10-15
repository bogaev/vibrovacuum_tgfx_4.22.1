#include "GT811_waveshare.h"
#include "CT_I2C.h"

#include "tests/config_tests.h"
#include <cstdio>

static uint8_t GTP_CFG_DATA[]=
{
  0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0x05,0x55,0x15,0x55,0x25,0x55,
  0x35,0x55,0x45,0x55,0x55,0x55,0x65,0x55,0x75,0x55,0x85,0x55,0x95,0x55,0xA5,0x55,
  0xB5,0x55,0xC5,0x55,0xD5,0x55,0xE5,0x55,0xF5,0x55,0x1B,0x03,0x00,0x00,0x00,0x13,
  0x13,0x13,0x0F,0x0F,0x0A,0x50,0x30,0x05,0x03,0x64,0x05,0xe0,0x01,0x20,0x03,0x00,
  0x00,0x32,0x2C,0x34,0x2E,0x00,0x00,0x04,0x14,0x22,0x04,0x00,0x00,0x00,0x00,0x00,
  0x20,0x14,0xEC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x30,
  0x25,0x28,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

GT811_Device GT811_Dev;

static uint8_t GT811_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
  uint8_t i;
  uint8_t ret=0;
  CT_I2C_Start();
  CT_I2C_Send_Byte(GT811_CMD_WR);
  CT_I2C_Wait_Ack();
  CT_I2C_Send_Byte(reg>>8);
  CT_I2C_Wait_Ack();
  CT_I2C_Send_Byte(reg&0XFF);
  CT_I2C_Wait_Ack();
  for(i=0;i<len;i++)
  {
    CT_I2C_Send_Byte(buf[i]);
    ret=CT_I2C_Wait_Ack();
    if(ret)break;
  }
  CT_I2C_Stop();
  return ret;
}

static void GT811_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
  uint8_t i;
  CT_I2C_Start();
  CT_I2C_Send_Byte(GT811_CMD_WR);
  CT_I2C_Wait_Ack();
  CT_I2C_Send_Byte(reg>>8);
  CT_I2C_Wait_Ack();
  CT_I2C_Send_Byte(reg&0XFF);
  CT_I2C_Wait_Ack();
  CT_I2C_Stop();
  CT_I2C_Start();
  CT_I2C_Send_Byte(GT811_CMD_RD);
  CT_I2C_Wait_Ack();
  for(i=0;i<len;i++)
  {
    buf[i]=CT_I2C_Read_Byte(i==(len-1)?0:1);
  }
  CT_I2C_Stop();
}

static uint8_t GT811_Send_Cfg(uint8_t * buf,uint16_t cfg_len)
{
  uint8_t ret=0;
  uint8_t retry=0;
  for(retry=0;retry<5;retry++)
  {
    ret=GT811_WR_Reg(GT811_CONFIG_REG,buf,cfg_len);
    if(ret==0)break;
    HAL_Delay(100);
  }
  return ret;
}

static void GT811_Read_Cfg()
{
#ifdef TOUCH_DEBUG_ON
  uint8_t CFG_DATA[sizeof(GTP_CFG_DATA)];
  uint16_t tmp;
  uint8_t tmp2;
  const uint16_t CFG_SIZE = sizeof(GTP_CFG_DATA);

  GT811_RD_Reg(GT811_CONFIG_REG,CFG_DATA,CFG_SIZE);

  for (tmp = 0; tmp < CFG_SIZE - 1; tmp++)
  {
    GT811_RD_Reg(GT811_CONFIG_REG+tmp,&tmp2,1);				//Read version of eight
    printf("REG:0x%x:0x%02x -- 0x%02x\r\n",GT811_CONFIG_REG+tmp,tmp2, GTP_CFG_DATA[tmp]);
    if (tmp2 !=  GTP_CFG_DATA[tmp])
    {
      printf("***error***\r\n");
    }
  }
#endif
}

static void GT811_Reset()
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GT811_RST_CLK();

  GPIO_InitStruct.Pin = GT811_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GT811_RST_PORT, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GT811_RST_PORT, GT811_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(GT811_RST_PORT, GT811_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(200);
}

static uint8_t GT811_Init(void)
{
  uint16_t version=0;
  uint8_t temp;

  CT_I2C_Init();
  GT811_Reset();

  GTP_CFG_DATA[62] = GT811_MAX_WIDTH >> 8;
  GTP_CFG_DATA[61] = GT811_MAX_WIDTH & 0xff;
  GTP_CFG_DATA[60] = GT811_MAX_HEIGHT >> 8;
  GTP_CFG_DATA[59] = GT811_MAX_HEIGHT & 0xff;

  GT811_RD_Reg(0X717,&temp,1);
  version=(uint16_t)temp<<8;
  GT811_RD_Reg(0X718,&temp,1);
  version|=temp;
#ifdef TOUCH_DEBUG_ON
  printf("version:%x\r\n",version);
#endif

  if(version==0X2010)
  {
    temp=GT811_Send_Cfg((uint8_t*)GTP_CFG_DATA,sizeof(GTP_CFG_DATA));
  }else temp=2;

  GT811_Read_Cfg();
  return temp;
}

bool GT811_Scan(void)
{
  bool is_touch = false;

  uint8_t buf[34];
  uint8_t tmp,tmp2;
  uint16_t sum = 0;
  uint8_t count = 0;
  if(GT811_Dev.Flag==0) return is_touch;
  GT811_Dev.Flag = 0;
  is_touch = true;

  while(1)
  {
    count++;
    if (count == 5)
    {
      return;
    }
    GT811_RD_Reg(GT811_READ_XY_REG,buf,34);
    __DSB();

    GT811_Dev.TouchPointFlag = tmp2 = buf[0]&0X1F;
    sum = 0;
    for (tmp = 0; tmp<5; tmp++)
    {
      if (tmp2 & 0x01)
      {
        sum++;
      }
      tmp2 = tmp2 >> 1;
    }
    GT811_Dev.Count = sum;
    sum = 0;

    for (tmp = 0; tmp < 33; tmp++)
    {
      sum += buf[tmp];
    }

    if (sum == 0)
    {
      delay_us(5);
      continue;
    }
    __DSB();
    if (((sum&0xff) == buf[33])||(GT811_Dev.Count == 1)||(GT811_Dev.Count == 2))
    {
      GT811_Dev.Static = 1;
      break;
    }
    else
    {
      GT811_Dev.Static = 0;
      delay_us(5);
      continue;
    }
  }

#if (GT811_EXCHG_XY == 1)
  GT811_Dev.y[0]=GT811_MAX_HEIGHT-(((uint16_t)buf[2]<<8)+buf[3]);
  GT811_Dev.x[0]=((uint16_t)buf[4]<<8)+buf[5];
  GT811_Dev.ppr[0]=buf[6];
  GT811_Dev.y[1]=GT811_MAX_HEIGHT-(((uint16_t)buf[7]<<8)+buf[8]);
  GT811_Dev.x[1]=((uint16_t)buf[9]<<8)+buf[10];
  GT811_Dev.ppr[1]=buf[11];
  GT811_Dev.y[2]=GT811_MAX_HEIGHT-(((uint16_t)buf[12]<<8)+buf[13]);
  GT811_Dev.x[2]=((uint16_t)buf[14]<<8)+buf[15];
  GT811_Dev.ppr[2]=buf[16];
  GT811_Dev.y[3]=GT811_MAX_HEIGHT-(((uint16_t)buf[17]<<8)+buf[24]);
  GT811_Dev.x[3]=((uint16_t)buf[25]<<8)+buf[26];
  GT811_Dev.ppr[3]=buf[27];
  GT811_Dev.y[4]=GT811_MAX_HEIGHT-(((uint16_t)buf[28]<<8)+buf[29]);
  GT811_Dev.x[4]=((uint16_t)buf[30]<<8)+buf[31];
  GT811_Dev.ppr[4]=buf[32];
#else
  GT811_Dev.y[0]=((uint16_t)buf[2]<<8)+buf[3];
  GT811_Dev.x[0]=GT811_MAX_WIDTH-(((uint16_t)buf[4]<<8)+buf[5]);
  GT811_Dev.ppr[0]=buf[6];
  GT811_Dev.y[1]=((uint16_t)buf[7]<<8)+buf[8];
  GT811_Dev.x[1]=GT811_MAX_WIDTH-(((uint16_t)buf[9]<<8)+buf[10]);
  GT811_Dev.ppr[1]=buf[11];
  GT811_Dev.y[2]=((uint16_t)buf[12]<<8)+buf[13];
  GT811_Dev.x[2]=GT811_MAX_WIDTH-(((uint16_t)buf[14]<<8)+buf[15]);
  GT811_Dev.ppr[2]=buf[16];
  GT811_Dev.y[3]=((uint16_t)buf[17]<<8)+buf[24];
  GT811_Dev.x[3]=GT811_MAX_WIDTH-(((uint16_t)buf[25]<<8)+buf[26]);
  GT811_Dev.ppr[3]=buf[27];
  GT811_Dev.y[4]=((uint16_t)buf[28]<<8)+buf[29];
  GT811_Dev.x[4]=GT811_MAX_WIDTH-(((uint16_t)buf[30]<<8)+buf[31]);
  GT811_Dev.ppr[4]=buf[32];
#endif
  return is_touch;
}

HAL_StatusTypeDef Gt811_WS::Init() {
  if (GT811_Init() != 2) {
    return HAL_OK;
  }
  return HAL_ERROR;
}

bool Gt811_WS::CheckState(int32_t& x, int32_t& y) {
  if (GT811_Scan()) {
    x = GT811_Dev.x[0];
    y = GT811_Dev.y[0];
    return true;
  }
  return false;
}

void Gt811_WS::SetEvent() {
  GT811_Dev.Flag = 1;
}
