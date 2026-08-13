/**
 * @file boot_hc32f460_ram_driver.c
 * @brief HC32F460-specific RAM-resident SPI / EFM / CRC / GPIO / WDT drivers for IAP.
 *        Selected only when BOOT_MCU_HC32F460 is defined.
 */
#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "main.h"
#include "BootRAM.h"
#include "boot_config.h"
#include "stdlib.h"
#include "string.h"

//extern IWDG_HandleTypeDef hiwdg; 

//////////	/* Select sFLASH: Chip Select pin low */
//////////	#define RAM_sFLASH_CS_LOW()       (RAM_GPIO_ResetPins(SF_CS_PORT, SF_CS_PIN))//LL_GPIO_ResetOutputPin(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
//////////	/* Deselect sFLASH: Chip Select pin high */
//////////	#define RAM_sFLASH_CS_HIGH()  		(RAM_GPIO_SetPins(SF_CS_PORT, SF_CS_PIN))//LL_GPIO_SetOutputPin(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)

void RAM_GPIO_SetPins(uint8_t u8Port, uint16_t u16Pin);
void RAM_GPIO_ResetPins(uint8_t u8Port, uint16_t u16Pin);

static uint8_t CacheToReactivate = FLASH_CACHE_DISABLED;

/**
 * @defgroup GPIO_Registers_Setting_definition GPIO Registers setting definition
 * @{
 */
#define GPIO_PSPCR_RST_VALUE            (0x001FU)
#define GPIO_PCCR_RST_VALUE             (0x4000U)
#define GPIO_PINAER_RST_VALUE           (0x0000U)
#define GPIO_PIN_NUM_MAX                (16U)
#define GPIO_PORT_OFFSET                (0x40UL)
#define GPIO_PIN_OFFSET                 (0x04UL)
#define GPIO_REG_OFFSET                 (0x10UL)
#define GPIO_REG_TYPE                   uint16_t
#define GPIO_PIDR_BASE                  ((uint32_t)(&CM_GPIO->PIDRA))
#define GPIO_PODR_BASE                  ((uint32_t)(&CM_GPIO->PODRA))
#define GPIO_POSR_BASE                  ((uint32_t)(&CM_GPIO->POSRA))
#define GPIO_PORR_BASE                  ((uint32_t)(&CM_GPIO->PORRA))
#define GPIO_POTR_BASE                  ((uint32_t)(&CM_GPIO->POTRA))
#define GPIO_POER_BASE                  ((uint32_t)(&CM_GPIO->POERA))
#define GPIO_PCR_BASE                   ((uint32_t)(&CM_GPIO->PCRA0))
#define GPIO_PFSR_BASE                  ((uint32_t)(&CM_GPIO->PFSRA0))

#define PIDR_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_PIDR_BASE + GPIO_REG_OFFSET * (x)))
#define PODR_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_PODR_BASE + GPIO_REG_OFFSET * (x)))
#define POSR_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_POSR_BASE + GPIO_REG_OFFSET * (x)))
#define PORR_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_PORR_BASE + GPIO_REG_OFFSET * (x)))
#define POTR_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_POTR_BASE + GPIO_REG_OFFSET * (x)))
#define POER_REG(x)     (*(__IO GPIO_REG_TYPE *)(GPIO_POER_BASE + GPIO_REG_OFFSET * (x)))
#define PCR_REG(x, y)   (*(__IO uint16_t *)(GPIO_PCR_BASE +  (uint32_t)((x) * GPIO_PORT_OFFSET) + (y) * GPIO_PIN_OFFSET))
#define PFSR_REG(x, y)  (*(__IO uint16_t *)(GPIO_PFSR_BASE + (uint32_t)((x) * GPIO_PORT_OFFSET) + (y) * GPIO_PIN_OFFSET))


/**
 * @defgroup GPIO_Check_Parameters_Validity GPIO Check Parameters Validity
 * @{
 */
/*! Parameter validity check for pin state. */
#define IS_GPIO_PIN_STATE(state)                                                \
(   ((state) == PIN_STAT_RST)                   ||                              \
    ((state) == PIN_STAT_SET))

/*! Parameter validity check for pin direction. */
#define IS_GPIO_DIR(dir)                                                        \
(   ((dir) == PIN_DIR_IN)                       ||                              \
    ((dir) == PIN_DIR_OUT))

/*! Parameter validity check for pin output type. */
#define IS_GPIO_OUT_TYPE(type)                                                  \
(   ((type) == PIN_OUT_TYPE_CMOS)               ||                              \
    ((type) == PIN_OUT_TYPE_NMOS))

/*! Parameter validity check for pin driver capacity. */
#define IS_GPIO_PIN_DRV(drv)                                                    \
(   ((drv) == PIN_LOW_DRV)                      ||                              \
    ((drv) == PIN_MID_DRV)                      ||                              \
    ((drv) == PIN_HIGH_DRV))

/*! Parameter validity check for pin attribute. */
#define IS_GPIO_ATTR(attr)                                                      \
(   ((attr) == PIN_ATTR_DIGITAL)                ||                              \
    ((attr) == PIN_ATTR_ANALOG))

/*! Parameter validity check for pin latch function. */
#define IS_GPIO_LATCH(latch)                                                    \
(   ((latch) == PIN_LATCH_OFF)                  ||                              \
    ((latch) == PIN_LATCH_ON))

/*! Parameter validity check for internal pull-up resistor. */
#define IS_GPIO_PIN_PU(pu)                                                      \
(   ((pu) == PIN_PU_OFF)                        ||                              \
    ((pu) == PIN_PU_ON))

/*! Parameter validity check for pin state invert. */
#define IS_GPIO_PIN_INVERT(invert)                                              \
(   ((invert) == PIN_INVT_OFF)                  ||                              \
    ((invert) == PIN_INVT_ON))

/*! Parameter validity check for external interrupt function. */
#define IS_GPIO_EXTINT(extint)                                                  \
(   ((extint) == PIN_EXTINT_OFF)                ||                              \
    ((extint) == PIN_EXTINT_ON))

/*! Parameter validity check for pin number. */
#define IS_GPIO_PIN(pin)                                                        \
(   ((pin) != 0U)                               &&                              \
    (((pin) & GPIO_PIN_ALL) != 0U))

/*! Parameter validity check for port source. */
#define IS_GPIO_PORT(port)                                                      \
(   ((port) == GPIO_PORT_A)                     ||                              \
    ((port) == GPIO_PORT_B)                     ||                              \
    ((port) == GPIO_PORT_C)                     ||                              \
    ((port) == GPIO_PORT_D)                     ||                              \
    ((port) == GPIO_PORT_E)                     ||                              \
    ((port) == GPIO_PORT_H))

/*! Parameter validity check for pin function. */
#define IS_GPIO_FUNC(func)                                                      \
(   ((func) <= GPIO_FUNC_15)                    ||                              \
    (((func) >= GPIO_FUNC_32) && ((func) <= GPIO_FUNC_59)))

/*! Parameter validity check for debug pin definition. */
#define IS_GPIO_DEBUG_PORT(port)                                                \
(   ((port) != 0U)                              &&                              \
    (((port) | GPIO_PIN_DEBUG) == GPIO_PIN_DEBUG))

/*! Parameter validity check for pin read wait cycle. */
#define IS_GPIO_READ_WAIT(wait)                                                 \
(   ((wait) == GPIO_RD_WAIT0)                   ||                              \
    ((wait) == GPIO_RD_WAIT1)                   ||                              \
    ((wait) == GPIO_RD_WAIT2)                   ||                              \
    ((wait) == GPIO_RD_WAIT3))

/*  Check GPIO register lock status. */
#define IS_GPIO_UNLOCK()      (GPIO_PWPR_WE == (CM_GPIO->PWPR & GPIO_PWPR_WE))



__RAM_FUNC void RAM_sFLASH_CS_LOW(uint8_t sFlashItem)
{
	switch (sFlashItem)
	{
		case 1:	
						RAM_GPIO_ResetPins(SF_CS_PORT, SF_CS_PIN);
						RAM_GPIO_SetPins(SF_CS_PORT_2, SF_CS_PIN_2);
						//sFLASH_lastCSLowItem=sFlashItem;
						break;
		case 2:	
						RAM_GPIO_ResetPins(SF_CS_PORT_2, SF_CS_PIN_2);
						RAM_GPIO_SetPins(SF_CS_PORT, SF_CS_PIN);
						//sFLASH_lastCSLowItem=sFlashItem;
						break;//ikinci CS port tanimi eklenecek
		default:
						//sFLASH_lastCSLowItem=0;
						break;
	}
}
__RAM_FUNC void RAM_sFLASH_CS_HIGH(void/*uint8_t sFlashItem*/)
{
	RAM_GPIO_SetPins(SF_CS_PORT, SF_CS_PIN);//birinci CS pini
	RAM_GPIO_SetPins(SF_CS_PORT_2, SF_CS_PIN_2);//ikinci CS pini
}


__RAM_FUNC uint8_t RAM_sFlashAddressPushToMessage(uint32_t u32Addr,uint8_t cmd,uint8_t sFlashItem,char sendBufData[])
{
	if((sFlashItem>0)&&(sFlashItem<=SPI_FLASH_CHIP_ITEM))
	{
		if(sFLASH_SPI_4BYTE_ADR_MODE[sFlashItem-1])
		{//sFlash 4 byte mode
				switch (cmd)
				{
					case sFLASH_CMD_ReadData: 		cmd=sFLASH_CMD_ReadData4ByteMode;			break;
					case sFLASH_CMD_PageProgram: 	cmd=sFLASH_CMD_PageProgram4ByteMode;	break;
					case sFLASH_CMD_SectorErase:  cmd=sFLASH_CMD_SectorErase4ByteMode;	break;
				}
				sendBufData[0]=cmd;
				sendBufData[1]=(uint8_t)((u32Addr & 0xFF000000ul) >> 24u);
				sendBufData[2]=(uint8_t)((u32Addr & 0x00FF0000ul) >> 16u);
				sendBufData[3]=(uint8_t)((u32Addr & 0x0000FF00ul) >> 8u);
				sendBufData[4]=(uint8_t) (u32Addr & 0x000000FFul);		
				return 5;
		}
		else
		{//sFlash 3 byte mode
				sendBufData[0]=cmd;
				sendBufData[1]=(uint8_t)((u32Addr & 0xFF0000ul) >> 16u);
				sendBufData[2]=(uint8_t)((u32Addr & 0xFF00u) >> 8u);
				sendBufData[3]=(uint8_t)(u32Addr & 0xFFu);	
				return 4;
		}
	}//elsi d?s?n?lecek!
	return 0;
}
__RAM_FUNC uint8_t RAM_AddressToChipOrder(uint32_t u32Addr,uint32_t* u32ChipAddress)
{
		uint8_t sFlashItem=0;
		//warning nedeni ile kapatildi//uint32_t u32ChipSizeTemp=0;
		if(u32Addr>=sFLASH_SPI_CHIPSIZE[0])
		{
			if(sFLASH_SPI_CHIPSIZE[1]!=0)//ikinci ?ipin b?y?kl?g? 0 degil ise buradayiz!
			{
				sFlashItem=2;
				if(u32ChipAddress!=NULL) *u32ChipAddress=u32Addr-sFLASH_SPI_CHIPSIZE[0];
			}
		}
		else
		{
			sFlashItem=1;
			if(u32ChipAddress!=NULL) *u32ChipAddress=u32Addr;
		}
	
		return sFlashItem;
}

__RAM_FUNC int32_t RAM_WDT_FeedDog(void)
{
    int32_t enRet = LL_OK;

    CM_WDT->RR = RAM_WDT_REFRESH_START_KEY;
    CM_WDT->RR = RAM_WDT_REFRESH_END_KEY;

    return enRet;
}

__RAM_FUNC void RAM_EFM_REG_Unlock(void)
{
    WRITE_REG32(CM_EFM->FAPRT, RAM_EFM_REG_UNLOCK_KEY1);
    WRITE_REG32(CM_EFM->FAPRT, RAM_EFM_REG_UNLOCK_KEY2);
}

/**
 * @brief  EFM Protect Lock.
 * @param  None
 * @retval None
 */
__RAM_FUNC void RAM_EFM_REG_Lock(void)
{
    WRITE_REG32(CM_EFM->FAPRT, RAM_EFM_REG_LOCK_KEY);
}
__RAM_FUNC ram_en_flag_status_t RAM_EFM_GetFlagStatus(uint32_t u32flag)
{
    RAM_DDL_ASSERT(RAM_IS_VALID_FLASH_FLAG(u32flag));

    return ((0ul == (CM_EFM->FSR & u32flag)) ? RAM_Reset : RAM_Set);
}
#define RAM_IS_EFM_REG_UNLOCK()             (CM_EFM->FAPRT == 0x00000001UL)
#define RAM_IS_EFM_CLRFLAG(x)               (((x) | EFM_FLAG_ALL) == EFM_FLAG_ALL)

__RAM_FUNC void RAM_EFM_ClearStatus(uint32_t u32Flag)
{
    RAM_DDL_ASSERT(RAM_IS_EFM_REG_UNLOCK());
    RAM_DDL_ASSERT(RAM_IS_EFM_CLRFLAG(u32Flag));

    SET_REG32_BIT(CM_EFM->FSCLR, u32Flag);
}	

#define RAM_IS_EFM_FLAG(x)                  (((x) | EFM_FLAG_ALL) == EFM_FLAG_ALL)

__RAM_FUNC en_flag_status_t RAM_EFM_GetStatus(uint32_t u32Flag)
{
    RAM_DDL_ASSERT(RAM_IS_EFM_FLAG(u32Flag));

    return ((u32Flag == READ_REG32_BIT(CM_EFM->FSR, u32Flag)) ? SET : RESET);
}
__RAM_FUNC int32_t RAM_EFM_WaitFlag(uint32_t u32Flag, uint32_t u32Time)
{
    __IO uint32_t u32Timeout = 0UL;
    int32_t i32Ret = LL_OK;

    while (SET != RAM_EFM_GetStatus(u32Flag)) {
        u32Timeout++;
        if (u32Timeout > u32Time) {
            i32Ret = LL_ERR_TIMEOUT;
            break;
        }
    }

    return i32Ret;
}

__RAM_FUNC int32_t RAM_EFM_ClearFlag(uint32_t u32Flag)
{
    uint32_t u32Time = RAM_EFM_TIMEOUT;
    __IO uint32_t u32Timeout = 0UL;
    int32_t i32Ret = LL_OK;

    while (RESET != RAM_EFM_GetStatus(u32Flag)) {
        RAM_EFM_ClearStatus(u32Flag);
        u32Timeout++;
        if (u32Timeout > u32Time) {
            i32Ret = LL_ERR_TIMEOUT;
            break;
        }
    }

    return i32Ret;
}
	


//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
__RAM_FUNC int32_t FLASH_WaitForLastOperation2(void)
{
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
       Even if the FLASH operation fails, the BUSY flag will be reset and an error
       flag will be set */
    while(RAM_EFM_GetFlagStatus(EFM_FLAG_RDY)==0)/*__HAL_FLASH_GET_FLAG(FLASH_SR_BSY)*/;  //0 ise bsy 1 ise idle

    /* Check FLASH End of Operation flag  */
    if(RAM_EFM_GetFlagStatus(EFM_FLAG_OPTEND)/*__HAL_FLASH_GET_FLAG(FLASH_SR_EOP)*/)
    {
        /* Clear FLASH End of Operation pending bit */
       // __HAL_FLASH_CLEAR_FLAG(FLASH_SR_EOP);
			RAM_EFM_REG_Unlock();//
			RAM_EFM_ClearFlag(EFM_FLAG_OPTEND);
			RAM_EFM_REG_Lock();//
    }
    return LL_OK;
}

__RAM_FUNC void RAM_EFM_DataCacheRstCmd(ram_en_functional_state_t enNewState)
{
    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));

		if(enNewState==RAM_ENABLE)
			CM_EFM->FRMC|=((uint32_t)0x00000001<<EFM_FRMC_CRST_POS);
		else
			CM_EFM->FRMC&=~((uint32_t)0x00000001<<EFM_FRMC_CRST_POS);
		//alternatif
		//CM_EFM->FRMC=((CM_EFM->FRMC & ~EFM_FRMC_CRST)|((enNewState<<EFM_FRMC_CRST_POS)&EFM_FRMC_CRST));
		//alternatif
    //CM_EFM->FRMC_f.CRST = enNewState;
}

__RAM_FUNC void RAM_EFM_InstructionCacheCmd(ram_en_functional_state_t enNewState)
{
    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));

		if(enNewState==RAM_ENABLE)
			CM_EFM->FRMC|=((uint32_t)0x00000001<<EFM_FRMC_CACHE_POS);
		else
			CM_EFM->FRMC&=~((uint32_t)0x00000001<<EFM_FRMC_CACHE_POS);
			
    //CM_EFM->FRMC_f.CACHE = enNewState;
}


//extern FLASH_ProcessTypeDef pFlash;

//__RAM_FUNC FLASH_WaitForLastOperation2(void)
//{
//  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
//     Even if the FLASH operation fails, the BUSY flag will be reset and an error
//     flag will be set */

//  uint32_t tickstart = 0;//HAL_GetTick();///////////////////////////////////////////////////////////////////
//	uint32_t Timeout = 0xFFFFFFFE;//HAL_GetTick();///////////////////////////////////////////////////////////////////
//  uint32_t error;
//	
//  while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
//  {
//		tickstart++;
//    if(Timeout != HAL_MAX_DELAY)
//    {
//      if(tickstart >= Timeout)
//      {
//        break;//return HAL_TIMEOUT;
//      }
//    }
//  }

//  error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);
//  error |= (FLASH->ECCR & FLASH_FLAG_ECCD);

//  if(error != 0u)
//  {
//    /*Save the error code*/
//    pFlash.ErrorCode |= error;

//    /* Clear error programming flags */
//    __HAL_FLASH_CLEAR_FLAG(error);

//    return HAL_ERROR;
//  }

//  /* Check FLASH End of Operation flag  */
//  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP))
//  {
//    /* Clear FLASH End of Operation pending bit */
//    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
//  }

//  /* If there is an error flag set */
//  return HAL_OK;
//}
//__RAM_FUNC FLASH_WaitForLastOperation2(void)
//{
//    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
//       Even if the FLASH operation fails, the BUSY flag will be reset and an error
//       flag will be set */
//    while(__HAL_FLASH_GET_FLAG(FLASH_SR_BSY));

//    /* Check FLASH End of Operation flag  */
//    if(__HAL_FLASH_GET_FLAG(FLASH_SR_EOP))
//    {
//        /* Clear FLASH End of Operation pending bit */
//        __HAL_FLASH_CLEAR_FLAG(FLASH_SR_EOP);
//    }
//    return HAL_OK;
//}
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

__RAM_FUNC void RAM_EFM_ErasePgmCmd(ram_en_functional_state_t enNewState)
{
    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));

	
		if(enNewState==RAM_ENABLE)
			CM_EFM->FWMC|=((uint32_t)0x00000001<<EFM_FWMC_PEMODE_POS);
		else
			CM_EFM->FWMC&=~((uint32_t)0x00000001<<EFM_FWMC_PEMODE_POS);
	
    //CM_EFM->FWMC_f.PEMODE = enNewState;
}



__RAM_FUNC int32_t FLASH_FlushCaches2(void)
{
    /* Flush instruction cache  */
    if((CacheToReactivate == FLASH_CACHE_ICACHE_ENABLED) ||
            (CacheToReactivate == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
    {
        /* Reset instruction cache */
       // __HAL_FLASH_INSTRUCTION_CACHE_RESET();
				RAM_EFM_DataCacheRstCmd(RAM_ENABLE);
			
        /* Enable instruction cache */
//        __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
				RAM_EFM_InstructionCacheCmd(RAM_ENABLE);
    }

    /* Flush data cache */
    if((CacheToReactivate == FLASH_CACHE_DCACHE_ENABLED) ||
            (CacheToReactivate == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
    {
        /* Reset data cache */
//        __HAL_FLASH_DATA_CACHE_RESET();
				RAM_EFM_DataCacheRstCmd(RAM_ENABLE);
        /* Enable data cache */
//        __HAL_FLASH_DATA_CACHE_ENABLE();
				RAM_EFM_InstructionCacheCmd(RAM_ENABLE);
    }

    /* Reset internal variable */
    CacheToReactivate = FLASH_CACHE_DISABLED;
    return LL_OK;
}
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
#define RAM_IS_EFM_FWMC_UNLOCK()            (bCM_EFM->FWMC_b.PEMODE == 1U)
#define RAM_IS_EFM_ADDR(x)                                                          \
(   ((x) <= EFM_END_ADDR)               ||                                      \
    (((x) >= EFM_OTP_START_ADDR) && ((x) <= EFM_OTP_END_ADDR)))
#define RAM_IS_ADDR_ALIGN_WORD(addr)        (0UL == (((uint32_t)(addr)) & 0x3UL))

__RAM_FUNC int32_t RAM_EFM_ProgramWord(uint32_t u32Addr, uint32_t u32Data)
{
    int32_t i32Ret = LL_OK;
    uint32_t u32Tmp;
    uint8_t u8Shift;

    RAM_DDL_ASSERT(RAM_IS_EFM_REG_UNLOCK());
    RAM_DDL_ASSERT(RAM_IS_EFM_FWMC_UNLOCK());
    RAM_DDL_ASSERT(RAM_IS_EFM_ADDR(u32Addr));
    RAM_DDL_ASSERT(RAM_IS_ADDR_ALIGN_WORD(u32Addr));

    /* Clear the error flag. */
    RAM_EFM_ClearStatus(EFM_FLAG_ALL);
	
    /* Get CACHE status */
    u32Tmp = READ_REG32_BIT(CM_EFM->FRMC, RAM_EFM_CACHE_ALL);
    /* Disable CACHE function */
    CLR_REG32_BIT(CM_EFM->FRMC, RAM_EFM_CACHE_ALL);
    u8Shift = 0U;
    /* Set single program mode. */
    MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_PGM_SINGLE);
    /* Program data. */
    RW_MEM32(u32Addr) = u32Data;

    /* Wait for ready flag. */
    if (LL_ERR_TIMEOUT == RAM_EFM_WaitFlag(EFM_FLAG_RDY << u8Shift, RAM_EFM_PGM_TIMEOUT)) {
        i32Ret = LL_ERR_NOT_RDY;
    }
    /* CLear the operation end flag. */
    RAM_EFM_ClearStatus(EFM_FLAG_OPTEND << u8Shift);

    /* Set read only mode. */
    MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_READONLY);

    /* Recover CACHE function */
    MODIFY_REG32(CM_EFM->FRMC, RAM_EFM_CACHE_ALL, u32Tmp);

    return i32Ret;
}
    

//void RAM_EFM_Lock(void)
//{
//    if(RAM_EFM_UNLOCK == CM_EFM->FAPRT)
//    {
//        CM_EFM->FAPRT = RAM_EFM_KEY2;
//        CM_EFM->FAPRT = RAM_EFM_KEY2;
//    }
//}

//void RAM_EFM_Unlock(void)
//{
//    CM_EFM->FAPRT = RAM_EFM_KEY1;
//    CM_EFM->FAPRT = RAM_EFM_KEY2;
//}



__RAM_FUNC void RAM_EFM_FWMC_Cmd(en_functional_state_t enNewState)
{
    RAM_DDL_ASSERT(IS_FUNCTIONAL_STATE(enNewState));

    WRITE_REG32(bCM_EFM->FWMC_b.PEMODE, enNewState);
}

//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
__RAM_FUNC int32_t FLASH_Program_Fast2(uint32_t Address, uint32_t DataAddress)
{
    //uint8_t row_index = (2*FLASH_NB_DOUBLE_WORDS_IN_ROW);
    uint16_t row_index = (FLASH_PAGE_SIZE / 4);
    __IO uint32_t *dest_addr = (__IO uint32_t *)Address;
    __IO uint32_t *src_addr = (__IO uint32_t *)DataAddress;
	
	RAM_EFM_REG_Unlock();//	RAM_EFM_Unlock();
	
	 RAM_EFM_ErasePgmCmd(RAM_ENABLE);
	 MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_PGM_SINGLE);//CM_EFM->FWMC_f.PEMOD = EFM_MD_PGM_SINGLE;
	
    /* Set FSTPG bit */
    //SET_BIT(FLASH->CR, FLASH_CR_FSTPG);
	//    SET_BIT(FLASH->CR, FLASH_CR_PG);  

//  /* Disable interrupts to avoid any interruption during the loop */
//  __disable_irq();

    /* Program the double word of the row */
    do
    {
//			RAM_EFM_SingleProgram(Address,DataAddress);
//					 Address=*dest_addr++;
//           DataAddress= *src_addr++;
      
        *dest_addr++ = *src_addr++;
				while (SET != RAM_EFM_GetStatus(EFM_FSR_RDY));//while(1ul != CM_EFM->FSR_f.RDY);
			
    }
    while(--row_index != 0); 
		
		
		
  //  CLEAR_BIT(FLASH->CR, FLASH_CR_PG);  
		
	RAM_EFM_ClearFlag(EFM_FLAG_OPTEND);
	MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_READONLY);//CM_EFM->FWMC_f.PEMOD = EFM_MD_READONLY;
	RAM_EFM_ErasePgmCmd(RAM_DISABLE);
		
		
		RAM_EFM_REG_Lock();//RAM_EFM_Lock();
//  /* Re-enable the interrupts */
//  __enable_irq();
    return LL_OK;
}




//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
__RAM_FUNC int32_t HAL_FLASH_Program2(uint32_t Address, uint32_t Data)
{
    //warning nedeni ile kapatildi//uint32_t prog_bit = 0;

    /* Wait for last operation to be completed */
    FLASH_WaitForLastOperation2();

    /* Deactivate the data cache if they are activated to avoid data misbehavior */
		RAM_EFM_GetFlagStatus(EFM_FRMC_CRST);
    if(/*READ_BIT(FLASH->ACR, FLASH_ACR_DCEN)*//*CM_EFM->FRMC_f.CRST*/RAM_EFM_GetFlagStatus(EFM_FRMC_CRST) == 1)
    {
        /* Disable data cache  */
//        __HAL_FLASH_DATA_CACHE_DISABLE();
			RAM_EFM_DataCacheRstCmd(RAM_DISABLE); //@INFO : huadada karsiligi
			  
			
        CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
    }
    else
    {
        CacheToReactivate = FLASH_CACHE_DISABLED;
    }

    /* Fast program a 32 row double-word (64-bit) at a specified address */
    FLASH_Program_Fast2(Address, Data);

    /* Wait for last operation to be completed */
    FLASH_WaitForLastOperation2();

    /* If the program operation is completed, disable the PG or FSTPG Bit */
//    if(prog_bit != 0)
//    {
////        CLEAR_BIT(FLASH->CR, prog_bit);
//    }

    /* Flush the caches to be sure of the data consistency */
    FLASH_FlushCaches2();
    return LL_OK;
}

#define RAM_IS_EFM_ERASE_ADDR(x)            ((x) <= EFM_END_ADDR)
#define IS_ADDR_ALIGN_WORD(addr)        (0UL == (((uint32_t)(addr)) & 0x3UL))

__RAM_FUNC int32_t RAM_EFM_SectorErase(uint32_t u32Addr)
{
    int32_t i32Ret = LL_OK;
    uint32_t u32Tmp;
    uint8_t u8Shift;

    RAM_DDL_ASSERT(RAM_IS_EFM_ERASE_ADDR(u32Addr));
    RAM_DDL_ASSERT(RAM_IS_ADDR_ALIGN_WORD(u32Addr));
    RAM_DDL_ASSERT(RAM_IS_EFM_REG_UNLOCK());
    RAM_DDL_ASSERT(RAM_IS_EFM_FWMC_UNLOCK());

    /* CLear the error flag. */
    RAM_EFM_ClearStatus(EFM_FLAG_ALL);
    /* Get CACHE status */
    u32Tmp = READ_REG32_BIT(CM_EFM->FRMC, RAM_EFM_CACHE_ALL);
    /* Disable CACHE */
    CLR_REG32_BIT(CM_EFM->FRMC, RAM_EFM_CACHE_ALL);
    u8Shift = 0U;
    /* Set sector erase mode. */
    MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_ERASE_SECTOR);

    /* Erase */
    RW_MEM32(u32Addr) = 0UL;

    /* Wait for ready flag. */
    if (LL_ERR_TIMEOUT == RAM_EFM_WaitFlag(EFM_FLAG_RDY << u8Shift, RAM_EFM_ERASE_TIMEOUT)) {
        i32Ret = LL_ERR_NOT_RDY;
    }
    /* Clear the operation end flag */
    RAM_EFM_ClearStatus(EFM_FLAG_OPTEND << u8Shift);

    /* Set read only mode. */
    MODIFY_REG32(CM_EFM->FWMC, EFM_FWMC_PEMOD, EFM_MD_READONLY);

    /* Recover CACHE */
    MODIFY_REG32(CM_EFM->FRMC, RAM_EFM_CACHE_ALL, u32Tmp);

    return i32Ret;
}

//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
__RAM_FUNC int32_t FLASH_PageErase2(uint32_t Page, uint32_t Banks)
{

////#if defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) || \
//    defined (STM32L496xx) || defined (STM32L4A6xx) || \
//    defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)
//#if defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)
//    if(READ_BIT(FLASH->OPTR, FLASH_OPTR_DBANK) == RESET)
//    {
//        CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
//    }
//    else
//#endif
//    {
//        assert_param(IS_FLASH_BANK_EXCLUSIVE(Banks));

//        if((Banks & FLASH_BANK_1) != RESET)
//        {
//            CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
//        }
//        else
//        {
//            SET_BIT(FLASH->CR, FLASH_CR_BKER);
//        }
//    }
//#endif

    /* Proceed to erase the page */
		RAM_EFM_REG_Unlock();
		RAM_EFM_FWMC_Cmd((en_functional_state_t)RAM_ENABLE);
		RAM_EFM_SectorErase(Page*RAM_F460_FLASH_PAGE_SIZE);
		RAM_EFM_FWMC_Cmd((en_functional_state_t)RAM_DISABLE);
		RAM_EFM_REG_Lock();
//    MODIFY_REG(FLASH->CR, FLASH_CR_PNB, (Page << POSITION_VAL(FLASH_CR_PNB)));  
//    SET_BIT(FLASH->CR, FLASH_CR_PER);
//    SET_BIT(FLASH->CR, FLASH_CR_STRT);
    return LL_OK;
}
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
__RAM_FUNC void RAM_EFM_CacheCmd(en_functional_state_t enNewState)
{
    DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));
    DDL_ASSERT(RAM_IS_EFM_REG_UNLOCK());

    WRITE_REG32(bCM_EFM->FRMC_b.CACHE, enNewState);
}
__RAM_FUNC void RAM_EFM_DataCacheResetCmd(en_functional_state_t enNewState)
{
    DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));
    DDL_ASSERT(RAM_IS_EFM_REG_UNLOCK());

    WRITE_REG32(bCM_EFM->FRMC_b.CRST, enNewState);
}

__RAM_FUNC int32_t HAL_FLASHEx_Erase2(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
{
    uint32_t page_index = 0;

		RAM_EFM_REG_Unlock();
    /* Wait for last operation to be completed */
    FLASH_WaitForLastOperation2();

    /* Deactivate the cache if they are activated to avoid data misbehavior */
    if(/*READ_BIT(FLASH->ACR, FLASH_ACR_ICEN)*/ /*M4_EFM->FRMC_f.CACHE*/READ_REG32_BIT(CM_EFM->FRMC, RAM_EFM_CACHE_ALL) == SET)
    {
        /* Disable instruction cache  */
//        __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
			RAM_EFM_CacheCmd((en_functional_state_t)RAM_DISABLE);          //@INFO : Huadada karsiligi
      
        if(/*READ_BIT(FLASH->ACR, FLASH_ACR_DCEN*/ /*M4_EFM->FRMC_f.CRST*/READ_REG32_BIT(CM_EFM->FRMC, EFM_FRMC_CRST)  == SET)
        {
            /* Disable data cache  */
//            __HAL_FLASH_DATA_CACHE_DISABLE();
					RAM_EFM_DataCacheResetCmd((en_functional_state_t)RAM_DISABLE);
            CacheToReactivate = FLASH_CACHE_ICACHE_DCACHE_ENABLED;
        }
        else
        {
            CacheToReactivate = FLASH_CACHE_ICACHE_ENABLED;
        }
    }
    else if(/*READ_BIT(FLASH->ACR, FLASH_ACR_DCEN)*//*M4_EFM->FRMC_f.CRST*/READ_REG32_BIT(CM_EFM->FRMC, EFM_FRMC_CRST) == SET)
    {

					RAM_EFM_DataCacheRstCmd(RAM_DISABLE);
        CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
    }
    else
    {
        CacheToReactivate = FLASH_CACHE_DISABLED;
    }

    if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {

    }
    else
    {
        /*Initialization of PageError variable*/
        *PageError = 0xFFFFFFFF;

        for(page_index = pEraseInit->Page; page_index < (pEraseInit->Page + pEraseInit->NbPages); page_index++)
        {
            FLASH_PageErase2(page_index, pEraseInit->Banks);

            /* Wait for last operation to be completed */
            FLASH_WaitForLastOperation2();
#ifdef IWDG_ENABLE
				RAM_WDT_FeedDog();
#endif

        }
    }

    /* Flush the caches to be sure of the data consistency */
    FLASH_FlushCaches2();
		RAM_EFM_REG_Lock();
    return LL_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////


__RAM_FUNC int32_t RAM_PORT_SetBits(uint8_t enPort, uint16_t u16Pin)
{
    uint16_t *POSRx;

    /* parameter check */
    RAM_DDL_ASSERT(RAM_IS_VALID_PORT(enPort));

    POSRx = (uint16_t *)((uint32_t)(&CM_GPIO->POSRA) + 0x10u * enPort);
    *POSRx |= u16Pin;
    return LL_OK;

}

__RAM_FUNC int32_t RAM_PORT_ResetBits(uint8_t enPort, uint16_t u16Pin)
{
    uint16_t *PORRx;

    /* parameter check */
    RAM_DDL_ASSERT(RAM_IS_VALID_PORT(enPort));

    PORRx = (uint16_t *)((uint32_t)(&CM_GPIO->PORRA) + 0x10u * enPort);
    *PORRx |= u16Pin;
    return LL_OK;
}


__RAM_FUNC static void RAM_GPIO_WritePin(uint8_t GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{  
  /* Check the parameters */
  

  if(PinState != GPIO_PIN_RESET)
  {
		RAM_PORT_SetBits(GPIOx, GPIO_Pin);
//    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
		RAM_PORT_ResetBits(GPIOx, GPIO_Pin);
//    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
}


///////////////////////////////////////////
///////////////////////////////////////////
__RAM_FUNC static GPIO_PinState RAM_LedPullUpInfo(uint8_t u8Port, uint16_t u16Pin)
{
	#ifndef DEBUG_ENABLE
		if((u8Port==RSSI_LED_4_Port)&&(u16Pin==RSSI_LED_4_Pin)) return GPIO_PIN_SET;//ters mantik s?r?len ledler i?in false d?ner!
		if((u8Port==RSSI_LED_5_Port)&&(u16Pin==RSSI_LED_5_Pin)) return GPIO_PIN_SET;
	#endif
	return GPIO_PIN_RESET;//standart ?alisan ledler i?in true d?ner
}
__RAM_FUNC void RAM_Board_LedOn(uint8_t u8Port, uint16_t u16Pin)
{
	GPIO_PinState pullUP=RAM_LedPullUpInfo(u8Port,u16Pin);
	RAM_GPIO_WritePin(u8Port,u16Pin,pullUP);

}

__RAM_FUNC void RAM_Board_LedOff(uint8_t u8Port, uint16_t u16Pin)
{
	GPIO_PinState pullUP=RAM_LedPullUpInfo(u8Port,u16Pin);
	//RAM_GPIO_WritePin(u8Port,u16Pin,(GPIO_PinState)(!pullUP));//warning nedeni ile alt satir ile g?ncellendi
	RAM_GPIO_WritePin(u8Port,u16Pin,((pullUP==GPIO_PIN_RESET)? GPIO_PIN_SET : GPIO_PIN_RESET));
}
///////////////////////////////////////////
///////////////////////////////////////////
//static HAL_StatusTypeDef RAM_SPI_WaitFifoStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Fifo, uint32_t State,
//                                                       uint32_t Timeout, uint32_t Tickstart)
//{
//  __IO uint8_t tmpreg;

//  while ((hspi->Instance->SR & Fifo) != State)
//  {
//    if ((Fifo == SPI_SR_FRLVL) && (State == SPI_FRLVL_EMPTY))
//    {
//      tmpreg = *((__IO uint8_t *)&hspi->Instance->DR);
//      /* To avoid GCC warning */
//      UNUSED(tmpreg);
//    }

//    if (Timeout != HAL_MAX_DELAY)
//    {
//      if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) >= Timeout))
//      {
//        /* Disable the SPI and reset the CRC: the CRC value should be cleared
//           on both master and slave sides in order to resynchronize the master
//           and slave for their respective CRC calculation */

//        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
//        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

//        if ((hspi->Init.Mode == SPI_MODE_MASTER) && ((hspi->Init.Direction == SPI_DIRECTION_1LINE)
//                                                     || (hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
//        {
//          /* Disable SPI peripheral */
//          __HAL_SPI_DISABLE(hspi);
//        }

//        /* Reset CRC Calculation */
//        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
//        {
//          SPI_RESET_CRC(hspi);
//        }

//        hspi->State = HAL_SPI_STATE_READY;

//        /* Process Unlocked */
//        __HAL_UNLOCK(hspi);

//        return HAL_TIMEOUT;
//      }
//    }
//  }

//  return LL_OK;
//}


//static HAL_StatusTypeDef RAM_SPI_WaitFlagStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, uint32_t State,
//                                                       uint32_t Timeout, uint32_t Tickstart)
//{
//  while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) != State)
//  {
//    if (Timeout != HAL_MAX_DELAY)
//    {
//      if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) >= Timeout))
//      {
//        /* Disable the SPI and reset the CRC: the CRC value should be cleared
//        on both master and slave sides in order to resynchronize the master
//        and slave for their respective CRC calculation */

//        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
//        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

//        if ((hspi->Init.Mode == SPI_MODE_MASTER) && ((hspi->Init.Direction == SPI_DIRECTION_1LINE)
//                                                     || (hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
//        {
//          /* Disable SPI peripheral */
//          __HAL_SPI_DISABLE(hspi);
//        }

//        /* Reset CRC Calculation */
//        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
//        {
//          SPI_RESET_CRC(hspi);
//        }

//        hspi->State = HAL_SPI_STATE_READY;

//        /* Process Unlocked */
//        __HAL_UNLOCK(hspi);

//        return HAL_TIMEOUT;
//      }
//    }
//  }

//  return LL_OK;
//}


//static HAL_StatusTypeDef RAM_SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout, uint32_t Tickstart)
//{
//  /* Control if the TX fifo is empty */
//  if (RAM_SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FTLVL, SPI_FTLVL_EMPTY, Timeout, Tickstart) != HAL_OK)
//  {
//    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
//    return HAL_TIMEOUT;
//  }

//  /* Control the BSY flag */
//  if (RAM_SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout, Tickstart) != HAL_OK)
//  {
//    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
//    return HAL_TIMEOUT;
//  }

//  /* Control if the RX fifo is empty */
//  if (RAM_SPI_WaitFifoStateUntilTimeout(hspi, SPI_FLAG_FRLVL, SPI_FRLVL_EMPTY, Timeout, Tickstart) != HAL_OK)
//  {
//    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
//    return HAL_TIMEOUT;
//  }

//  return HAL_OK;
//}

//static HAL_StatusTypeDef RAM_HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,
//                                          uint32_t Timeout)
//{
//  uint32_t tmp = 0U, tmp1 = 0U;
//  //uint32_t tickstart = 0U;
//  /* Variable used to alternate Rx and Tx during transfer */
//  uint32_t txallowed = 1U;
//  HAL_StatusTypeDef errorcode = HAL_OK;

//  /* Check Direction parameter */
//  

//  /* Process Locked */
//  __HAL_LOCK(hspi);

//  /* Init tickstart for timeout management*/
//  //tickstart = HAL_GetTick();//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  tmp  = hspi->State;
//  tmp1 = hspi->Init.Mode;

//  if (!((tmp == HAL_SPI_STATE_READY) || \
//        ((tmp1 == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmp == HAL_SPI_STATE_BUSY_RX))))
//  {
//    errorcode = HAL_BUSY;
//    goto error;
//  }

//  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
//  {
//    errorcode = HAL_ERROR;
//    goto error;
//  }

//  /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
//  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
//  {
//    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
//  }

//  /* Set the transaction information */
//  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
//  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
//  hspi->RxXferCount = Size;
//  hspi->RxXferSize  = Size;
//  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
//  hspi->TxXferCount = Size;
//  hspi->TxXferSize  = Size;

//  /*Init field not used in handle to zero */
//  hspi->RxISR       = NULL;
//  hspi->TxISR       = NULL;

//  /* Set the Rx Fifo threshold */
//  if ((hspi->Init.DataSize > SPI_DATASIZE_8BIT) || (hspi->RxXferCount > 1U))
//  {
//    /* Set fiforxthreshold according the reception data length: 16bit */
//    CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
//  }
//  else
//  {
//    /* Set fiforxthreshold according the reception data length: 8bit */
//    SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
//  }

//  /* Check if the SPI is already enabled */
//  if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
//  {
//    /* Enable SPI peripheral */
//    __HAL_SPI_ENABLE(hspi);
//  }

//  /* Transmit and Receive data in 16 Bit mode */
////  if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
////  {
////    if ((hspi->Init.Mode == SPI_MODE_SLAVE) || (hspi->TxXferCount == 0x01U))
////    {
////      hspi->Instance->DR = *((uint16_t *)pTxData);
////      pTxData += sizeof(uint16_t);
////      hspi->TxXferCount--;
////    }
////    while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
////    {
////      /* Check TXE flag */
////      if (txallowed && (hspi->TxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE)))
////      {
////        hspi->Instance->DR = *((uint16_t *)pTxData);
////        pTxData += sizeof(uint16_t);
////        hspi->TxXferCount--;
////        /* Next Data is a reception (Rx). Tx not allowed */
////        txallowed = 0U;

////      }

////      /* Check RXNE flag */
////      if ((hspi->RxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)))
////      {
////        *((uint16_t *)pRxData) = hspi->Instance->DR;
////        pRxData += sizeof(uint16_t);
////        hspi->RxXferCount--;
////        /* Next Data is a Transmission (Tx). Tx is allowed */
////        txallowed = 1U;
////      }
////      if ((Timeout != HAL_MAX_DELAY) && ((HAL_GetTick() - tickstart) >=  Timeout))
////      {
////        errorcode = HAL_TIMEOUT;
////        goto error;
////      }
////    }
////  }
////  /* Transmit and Receive data in 8 Bit mode */
////  else
////  {
//    if ((hspi->Init.Mode == SPI_MODE_SLAVE) || (hspi->TxXferCount == 0x01U))
//    {
//      if (hspi->TxXferCount > 1U)
//      {
//        hspi->Instance->DR = *((uint16_t *)pTxData);
//        pTxData += sizeof(uint16_t);
//        hspi->TxXferCount -= 2U;
//      }
//      else
//      {
//        *(__IO uint8_t *)&hspi->Instance->DR = (*pTxData++);
//        hspi->TxXferCount--;
//      }
//    }
//    while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
//    {
//      /* Check TXE flag */
//      if (txallowed && (hspi->TxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE)))
//      {
//        if (hspi->TxXferCount > 1U)
//        {
//          hspi->Instance->DR = *((uint16_t *)pTxData);
//          pTxData += sizeof(uint16_t);
//          hspi->TxXferCount -= 2U;
//        }
//        else
//        {
//          *(__IO uint8_t *)&hspi->Instance->DR = (*pTxData++);
//          hspi->TxXferCount--;
//        }
//        /* Next Data is a reception (Rx). Tx not allowed */
//        txallowed = 0U;

//      }

//      /* Wait until RXNE flag is reset */
//      if ((hspi->RxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)))
//      {
//        if (hspi->RxXferCount > 1U)
//        {
//          *((uint16_t *)pRxData) = hspi->Instance->DR;
//          pRxData += sizeof(uint16_t);
//          hspi->RxXferCount -= 2U;
//          if (hspi->RxXferCount <= 1U)
//          {
//            /* Set RX Fifo threshold before to switch on 8 bit data size */
//            SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
//          }
//        }
//        else
//        {
//          (*(uint8_t *)pRxData++) = *(__IO uint8_t *)&hspi->Instance->DR;
//          hspi->RxXferCount--;
//        }
//        /* Next Data is a Transmission (Tx). Tx is allowed */
//        txallowed = 1U;
//      }
////      if ((Timeout != HAL_MAX_DELAY) && ((HAL_GetTick() - tickstart) >=  Timeout))
////      {
////        errorcode = HAL_TIMEOUT;
////        goto error;
////      }
//    }
////  }

//  /* Check the end of the transaction */
//  if (RAM_SPI_EndRxTxTransaction(hspi, Timeout, 0/*tickstart*/) != HAL_OK)
//  {
//    errorcode = HAL_ERROR;
//    hspi->ErrorCode = HAL_SPI_ERROR_FLAG;
//  }

//error :
//  hspi->State = HAL_SPI_STATE_READY;
//  __HAL_UNLOCK(hspi);
//  return errorcode;
//}

	////////////////////////////
	/////////////////////////////
	////////////////////////////
	/////////////////////////////
	////////////////////////////
	/////////////////////////////
	////////////////////////////
	/////////////////////////////
	////////////////////////////
	/////////////////////////////
	////////////////////////////
	/////////////////////////////
	
//int32_t RAM_SPI_Init(M4_SPI_TypeDef *SPIx, const stc_spi_init_t *pstcSpiInitCfg)
//{
//    int32_t enRet = ErrorInvalidParameter;

//    /* Check parameters */
//    if((RAM_IS_VALID_SPI_UNIT(SPIx)) && (NULL != pstcSpiInitCfg))
//    {
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_SETUP_DELAY_OPTION(pstcSpiInitCfg->stcDelayConfig.enSsSetupDelayOption));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_SETUP_DELAY_TIME(pstcSpiInitCfg->stcDelayConfig.enSsSetupDelayTime));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_HOLD_DELAY_OPTION(pstcSpiInitCfg->stcDelayConfig.enSsHoldDelayOption));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_HOLD_DELAY_TIME(pstcSpiInitCfg->stcDelayConfig.enSsHoldDelayTime));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_INTERVAL_TIME_OPTION(pstcSpiInitCfg->stcDelayConfig.enSsIntervalTimeOption));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_INTERVAL_TIME(pstcSpiInitCfg->stcDelayConfig.enSsIntervalTime));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_VALID_CHANNEL(pstcSpiInitCfg->stcSsConfig.enSsValidBit));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_POLARITY(pstcSpiInitCfg->stcSsConfig.enSs0Polarity));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_POLARITY(pstcSpiInitCfg->stcSsConfig.enSs1Polarity));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_POLARITY(pstcSpiInitCfg->stcSsConfig.enSs2Polarity));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SS_POLARITY(pstcSpiInitCfg->stcSsConfig.enSs3Polarity));
//        RAM_DDL_ASSERT(RAM_IS_VALID_READ_DATA_REG_OBJECT(pstcSpiInitCfg->enReadBufferObject));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SCK_POLARITY(pstcSpiInitCfg->enSckPolarity));
//        RAM_DDL_ASSERT(RAM_IS_VALID_SCK_PHASE(pstcSpiInitCfg->enSckPhase));
//        RAM_DDL_ASSERT(RAM_IS_VALID_CLK_DIV(pstcSpiInitCfg->enClkDiv));
//        RAM_DDL_ASSERT(RAM_IS_VALID_DATA_LENGTH(pstcSpiInitCfg->enDataLength));
//        RAM_DDL_ASSERT(RAM_IS_VALID_FIRST_BIT_POSITION(pstcSpiInitCfg->enFirstBitPosition));
//        RAM_DDL_ASSERT(RAM_IS_VALID_FRAME_NUMBER(pstcSpiInitCfg->enFrameNumber));
//        RAM_DDL_ASSERT(RAM_IS_VALID_WORK_MODE(pstcSpiInitCfg->enWorkMode));
//        RAM_DDL_ASSERT(RAM_IS_VALID_COMM_MODE(pstcSpiInitCfg->enTransMode));
//        RAM_DDL_ASSERT(RAM_IS_VALID_MASTER_SLAVE_MODE(pstcSpiInitCfg->enMasterSlaveMode));
//        RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcSpiInitCfg->enCommAutoSuspendEn));
//        RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcSpiInitCfg->enModeFaultErrorDetectEn));
//        RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcSpiInitCfg->enParitySelfDetectEn));
//        RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcSpiInitCfg->enParityEn));
//        RAM_DDL_ASSERT(RAM_IS_VALID_PARITY_MODE(pstcSpiInitCfg->enParity));

//        /* Master mode */
//        if (SpiModeMaster == pstcSpiInitCfg->enMasterSlaveMode)
//        {
//            SPIx->CFG2_f.MSSIE = pstcSpiInitCfg->stcDelayConfig.enSsSetupDelayOption;
//            SPIx->CFG2_f.MSSDLE = pstcSpiInitCfg->stcDelayConfig.enSsHoldDelayOption;
//            SPIx->CFG2_f.MIDIE = pstcSpiInitCfg->stcDelayConfig.enSsIntervalTimeOption;
//            SPIx->CFG1_f.MSSI = pstcSpiInitCfg->stcDelayConfig.enSsSetupDelayTime;
//            SPIx->CFG1_f.MSSDL = pstcSpiInitCfg->stcDelayConfig.enSsHoldDelayTime;
//            SPIx->CFG1_f.MIDI = pstcSpiInitCfg->stcDelayConfig.enSsIntervalTime;
//        }
//        else
//        {
//            SPIx->CFG2_f.MSSIE = SpiSsSetupDelayTypicalSck1;
//            SPIx->CFG2_f.MSSDLE = SpiSsHoldDelayTypicalSck1;
//            SPIx->CFG2_f.MIDIE = SpiSsIntervalTypicalSck1PlusPck2;
//            SPIx->CFG1_f.MSSI = SpiSsSetupDelaySck1;
//            SPIx->CFG1_f.MSSDL = SpiSsHoldDelaySck1;
//            SPIx->CFG1_f.MIDI = SpiSsIntervalSck1PlusPck2;
//        }

//        /* 4 lines spi mode */
//        if (SpiWorkMode4Line == pstcSpiInitCfg->enWorkMode)
//        {
//            SPIx->CFG2_f.SSA = pstcSpiInitCfg->stcSsConfig.enSsValidBit;
//            SPIx->CFG1_f.SS0PV = pstcSpiInitCfg->stcSsConfig.enSs0Polarity;
//            SPIx->CFG1_f.SS1PV = pstcSpiInitCfg->stcSsConfig.enSs1Polarity;
//            SPIx->CFG1_f.SS2PV = pstcSpiInitCfg->stcSsConfig.enSs2Polarity;
//            SPIx->CFG1_f.SS3PV = pstcSpiInitCfg->stcSsConfig.enSs3Polarity;
//        }
//        else
//        {
//            SPIx->CFG2_f.SSA = SpiSsValidChannel0;
//            SPIx->CFG1_f.SS0PV = SpiSsLowValid;
//            SPIx->CFG1_f.SS1PV = SpiSsLowValid;
//            SPIx->CFG1_f.SS2PV = SpiSsLowValid;
//            SPIx->CFG1_f.SS3PV = SpiSsLowValid;
//        }

//        /* Configure communication config register 1 */
//        SPIx->CFG1_f.SPRDTD = pstcSpiInitCfg->enReadBufferObject;
//        SPIx->CFG1_f.FTHLV = pstcSpiInitCfg->enFrameNumber;

//        /* Configure communication config register 2 */
//        SPIx->CFG2_f.LSBF = pstcSpiInitCfg->enFirstBitPosition;
//        SPIx->CFG2_f.DSIZE = pstcSpiInitCfg->enDataLength;
//        SPIx->CFG2_f.MBR = pstcSpiInitCfg->enClkDiv;
//        SPIx->CFG2_f.CPOL = pstcSpiInitCfg->enSckPolarity;
//        SPIx->CFG2_f.CPHA = pstcSpiInitCfg->enSckPhase;

//        /* Configure control register */
//        SPIx->CR1_f.SPIMDS = pstcSpiInitCfg->enWorkMode;
//        SPIx->CR1_f.TXMDS = pstcSpiInitCfg->enTransMode;
//        SPIx->CR1_f.MSTR = pstcSpiInitCfg->enMasterSlaveMode;
//        SPIx->CR1_f.CSUSPE = pstcSpiInitCfg->enCommAutoSuspendEn;
//        SPIx->CR1_f.MODFE = pstcSpiInitCfg->enModeFaultErrorDetectEn;
//        SPIx->CR1_f.PATE = pstcSpiInitCfg->enParitySelfDetectEn;
//        SPIx->CR1_f.PAE = pstcSpiInitCfg->enParityEn;
//        SPIx->CR1_f.PAOE = pstcSpiInitCfg->enParity;
//        enRet = Ok;
//    }

//    return enRet;
//}	


//int32_t SPI_Initx(CM_SPI_TypeDef *SPIx, const stc_spi_init_t *pstcSpiInit)
//{
//    int32_t i32Ret = LL_ERR_INVD_PARAM;

//    DDL_ASSERT(IS_VALID_SPI_UNIT(SPIx));

//    if (NULL != pstcSpiInit) {
//        RAM_DDL_ASSERT(IS_SPI_WIRE_MD(pstcSpiInit->u32WireMode));
//        RAM_DDL_ASSERT(IS_SPI_TRANS_MD(pstcSpiInit->u32TransMode));
//        RAM_DDL_ASSERT(IS_SPI_MASTER_SLAVE(pstcSpiInit->u32MasterSlave));
//        RAM_DDL_ASSERT(IS_SPI_MD_FAULT_DETECT_CMD(pstcSpiInit->u32ModeFaultDetect));
//        RAM_DDL_ASSERT(IS_SPI_PARITY_CHECK(pstcSpiInit->u32Parity));
//        RAM_DDL_ASSERT(IS_SPI_SPI_MD(pstcSpiInit->u32SpiMode));
//        RAM_DDL_ASSERT(IS_SPI_BIT_RATE_DIV(pstcSpiInit->u32BaudRatePrescaler));
//        RAM_DDL_ASSERT(IS_SPI_DATA_SIZE(pstcSpiInit->u32DataBits));
//        RAM_DDL_ASSERT(IS_SPI_FIRST_BIT(pstcSpiInit->u32FirstBit));
//        RAM_DDL_ASSERT(IS_SPI_SUSPD_MD_STD(pstcSpiInit->u32SuspendMode));
//        RAM_DDL_ASSERT(IS_SPI_DATA_FRAME(pstcSpiInit->u32FrameLevel));

//        /* Configuration parameter check */
//        if ((SPI_MASTER == pstcSpiInit->u32MasterSlave) && (SPI_MD_FAULT_DETECT_ENABLE == pstcSpiInit->u32ModeFaultDetect)) {
//            /* pstcSpiInit->u32ModeFaultDetect can not be SPI_MD_FAULT_DETECT_ENABLE in master mode */
//        } else if ((SPI_3_WIRE == pstcSpiInit->u32WireMode) && (SPI_SLAVE == pstcSpiInit->u32MasterSlave)
//                   && ((SPI_MD_0 == pstcSpiInit->u32SpiMode) || (SPI_MD_2 == pstcSpiInit->u32SpiMode))) {
//            /* SPI_3_WIRE can not support SPI_MD_0 and SPI_MD_2 */
//        } else {
//            WRITE_REG32(SPIx->CR1, pstcSpiInit->u32WireMode | pstcSpiInit->u32TransMode | pstcSpiInit->u32MasterSlave
//                        | pstcSpiInit->u32SuspendMode | pstcSpiInit->u32ModeFaultDetect | pstcSpiInit->u32Parity);
//            MODIFY_REG32(SPIx->CFG1, SPI_CFG1_FTHLV, pstcSpiInit->u32FrameLevel);
//            WRITE_REG32(SPIx->CFG2, pstcSpiInit->u32SpiMode | pstcSpiInit->u32BaudRatePrescaler | pstcSpiInit->u32DataBits
//                        | pstcSpiInit->u32FirstBit);
//            i32Ret = LL_OK;
//        }
//    }
//    return i32Ret;
//}
	
__RAM_FUNC void RAM_PORT_Lock(void)
{
    CM_GPIO->PWPR = 0xA500u;
}

__RAM_FUNC void RAM_PORT_Unlock(void)
{
    CM_GPIO->PWPR = 0xA501u;
}


//int32_t RAM_GPIO_SetFunc(uint8_t enPort, uint16_t u16Pin, en_port_func_t enFuncSel,  \
//                  ram_en_functional_state_t enSubFunc)
//{
//    stc_port_pfsr_field_t *PFSRx;
//    uint8_t u8PinPos = 0u;

//    /* parameter check */
//    RAM_DDL_ASSERT(RAM_IS_VALID_PORT(enPort));
//    RAM_DDL_ASSERT(RAM_IS_VALID_FUNC(enFuncSel));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enSubFunc));

//    RAM_PORT_Unlock();

//    for (u8PinPos = 0u; u8PinPos < 16u; u8PinPos ++)
//    {
//        if (u16Pin & (uint16_t)(1ul<<u8PinPos))
//        {
//            PFSRx = (stc_port_pfsr_field_t *)((uint32_t)(&M4_PORT->PFSRA0) \
//                    + 0x40ul * enPort + 0x4ul * u8PinPos);

//            /* main function setting */
//            PFSRx->FSEL = enFuncSel;

//            /* sub function enable setting */
//            PFSRx->BFE = (RAM_ENABLE == enSubFunc ? RAM_ENABLE : RAM_DISABLE);
//        }
//    }

//    RAM_PORT_Lock();
//    return Ok;
//}

__RAM_FUNC int32_t RAM_GPIO_SetFunc(uint8_t u8Port, uint16_t u16Pin, uint16_t u16Func)
{
    uint8_t u8PinPos;
    __IO uint16_t *PFSRx;

    /* Parameter validity checking */
    DDL_ASSERT(IS_GPIO_PORT(u8Port));
    DDL_ASSERT(IS_GPIO_PIN(u16Pin));
    DDL_ASSERT(IS_GPIO_FUNC(u16Func));
    DDL_ASSERT(IS_GPIO_UNLOCK());

		
    for (u8PinPos = 0U; u8PinPos < GPIO_PIN_NUM_MAX; u8PinPos++) {
        if ((u16Pin & (uint16_t)(1UL << u8PinPos)) != 0U) {
            PFSRx = &PFSR_REG(u8Port, u8PinPos);
            WRITE_REG16(*PFSRx, u16Func);
        }
    }

    return LL_OK;
}

//int32_t RAM_PORT_Init(uint8_t enPort, uint16_t u16Pin, const stc_port_init_t *pstcPortInit)
//{
//    stc_port_pcr_field_t *PCRx;
//    stc_port_pfsr_field_t * PFSRx;
//    uint8_t u8PinPos = 0u;

//    /* parameter check */
//    RAM_DDL_ASSERT(RAM_IS_VALID_PORT(enPort));

//    RAM_DDL_ASSERT(RAM_IS_VALID_PINMODE(pstcPortInit->enPinMode));
//    RAM_DDL_ASSERT(RAM_IS_VALID_PINDRV(pstcPortInit->enPinDrv));
//    RAM_DDL_ASSERT(RAM_IS_VALID_PINTYPE(pstcPortInit->enPinOType));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcPortInit->enLatch));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcPortInit->enExInt));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcPortInit->enInvert));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcPortInit->enPullUp));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(pstcPortInit->enPinSubFunc));

//    RAM_PORT_Unlock();
//    for (u8PinPos = 0u; u8PinPos < 16u; u8PinPos ++)
//    {
//        if (u16Pin & (1ul<<u8PinPos))
//        {
//            PCRx = (stc_port_pcr_field_t *)((uint32_t)(&M4_PORT->PCRA0) +      \
//                                            enPort * 0x40ul + u8PinPos * 0x04ul);
//            PFSRx = (stc_port_pfsr_field_t *)((uint32_t)(&M4_PORT->PFSRA0) +   \
//                                              enPort * 0x40ul + u8PinPos * 0x04ul);

//            /* Input latch function setting */
//            PCRx->LTE = pstcPortInit->enLatch;

//            /* External interrupt input enable setting */
//            PCRx->INTE = pstcPortInit->enExInt;

//            /* In_Out invert setting */
//            PCRx->INVE = pstcPortInit->enInvert;

//            /* Pin pull-up setting */
//            PCRx->PUU = pstcPortInit->enPullUp;

//            /* CMOS/OD output setting */
//            PCRx->NOD = pstcPortInit->enPinOType;

//            /* Pin drive mode setting */
//            PCRx->DRV = pstcPortInit->enPinDrv;

//            /* Pin mode setting */
//            switch (pstcPortInit->enPinMode)
//            {
//                case Pin_Mode_In:
//                    PCRx->DDIS  = 0u;
//                    PCRx->POUTE = 0u;
//                break;
//                case Pin_Mode_Out:
//                    PCRx->DDIS  = 0u;
//                    PCRx->POUTE = 1u;
//                break;
//                case Pin_Mode_Ana:
//                    PCRx->DDIS  = 1u;
//                break;
//                default:
//                break;
//            }
//            /* Sub function enable setting */
//            PFSRx->BFE = pstcPortInit->enPinSubFunc;
//        }
//    }
//    RAM_PORT_Lock();
//    return Ok;
//}


__RAM_FUNC int32_t RAM_GPIO_Init(uint8_t u8Port, uint16_t u16Pin, const stc_gpio_init_t *pstcGpioInit)
{
    uint8_t u8PinPos;
    uint16_t u16PCRVal;
    uint16_t u16PCRMask;
    int32_t i32Ret = LL_OK;
    __IO uint16_t *PCRx;

    /* Check if pointer is NULL */
    if (NULL == pstcGpioInit) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        /* Parameter validity checking */
        RAM_DDL_ASSERT(IS_GPIO_UNLOCK());
        RAM_DDL_ASSERT(IS_GPIO_PORT(u8Port));
        RAM_DDL_ASSERT(IS_GPIO_PIN(u16Pin));
        RAM_DDL_ASSERT(IS_GPIO_PIN_STATE(pstcGpioInit->u16PinState));
        RAM_DDL_ASSERT(IS_GPIO_DIR(pstcGpioInit->u16PinDir));
        RAM_DDL_ASSERT(IS_GPIO_OUT_TYPE(pstcGpioInit->u16PinOutputType));
        RAM_DDL_ASSERT(IS_GPIO_PIN_DRV(pstcGpioInit->u16PinDrv));
        RAM_DDL_ASSERT(IS_GPIO_LATCH(pstcGpioInit->u16Latch));
        RAM_DDL_ASSERT(IS_GPIO_PIN_PU(pstcGpioInit->u16PullUp));
        RAM_DDL_ASSERT(IS_GPIO_PIN_INVERT(pstcGpioInit->u16Invert));
        RAM_DDL_ASSERT(IS_GPIO_EXTINT(pstcGpioInit->u16ExtInt));
        RAM_DDL_ASSERT(IS_GPIO_ATTR(pstcGpioInit->u16PinAttr));
        for (u8PinPos = 0U; u8PinPos < GPIO_PIN_NUM_MAX; u8PinPos++) {
            if ((u16Pin & (1UL << u8PinPos)) != 0U) {
                u16PCRVal = pstcGpioInit->u16PinState | pstcGpioInit->u16PinDir | pstcGpioInit->u16PinOutputType |  \
                            pstcGpioInit->u16PinDrv   | pstcGpioInit->u16PullUp | pstcGpioInit->u16Invert        |  \
                            pstcGpioInit->u16ExtInt   | pstcGpioInit->u16Latch;

                u16PCRMask = GPIO_PCR_POUT            | GPIO_PCR_POUTE          | GPIO_PCR_NOD                   |  \
                             GPIO_PCR_DRV             | GPIO_PCR_PUU            | GPIO_PCR_INVE                  |  \
                             GPIO_PCR_INTE            | GPIO_PCR_LTE ;
                u16PCRVal |= pstcGpioInit->u16PinAttr;
                u16PCRMask |= GPIO_PCR_DDIS;

                PCRx = &PCR_REG(u8Port, u8PinPos);
                MODIFY_REG16(*PCRx, u16PCRMask, u16PCRVal);
            }
        }
    }
    return i32Ret;
}

//ram_en_flag_status_t RAM_SPI_GetFlag(M4_SPI_TypeDef *SPIx, ram_en_spi_flag_type_t enFlag)
//{
//    ram_en_flag_status_t enFlagSta = RAM_Reset;

//    /* Check parameters */

//        RAM_DDL_ASSERT(RAM_IS_VALID_FLAG_TYPE(enFlag));

//        switch (enFlag)
//        {
//            case RAM_SpiFlagReceiveBufferFull:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.RDFF;
//                break;
//            case RAM_SpiFlagSendBufferEmpty:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.TDEF;
//                break;
//            case RAM_SpiFlagUnderloadError:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.UDRERF;
//                break;
//            case RAM_SpiFlagParityError:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.PERF;
//                break;
//            case RAM_SpiFlagModeFaultError:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.MODFERF;
//                break;
//            case RAM_SpiFlagSpiIdle:
//                enFlagSta = (ram_en_flag_status_t)(bool)(!SPIx->SR_f.IDLNF);
//                break;
//            case RAM_SpiFlagOverloadError:
//                enFlagSta = (ram_en_flag_status_t)SPIx->SR_f.OVRERF;
//                break;
//            default:
//                break;
//        }
//    

//    return enFlagSta;
//}

//int32_t RAM_SPI_ClearFlag(M4_SPI_TypeDef *SPIx, ram_en_spi_flag_type_t enFlag)
//{
//    int32_t enRet = ErrorInvalidParameter;

//    /* Check parameters */
//    if(RAM_IS_VALID_SPI_UNIT(SPIx))
//    {
//        RAM_DDL_ASSERT(RAM_IS_VALID_CLR_FLAG_TYPE(enFlag));

//        switch (enFlag)
//        {
//            case RAM_SpiFlagReceiveBufferFull:
//                SPIx->SR_f.RDFF = 0u;
//                break;
//            case RAM_SpiFlagSendBufferEmpty:
//                SPIx->SR_f.TDEF = 0u;
//                break;
//            case RAM_SpiFlagUnderloadError:
//                SPIx->SR_f.UDRERF = 0u;
//                break;
//            case RAM_SpiFlagParityError:
//                SPIx->SR_f.PERF = 0u;
//                break;
//            case RAM_SpiFlagModeFaultError:
//                SPIx->SR_f.MODFERF = 0u;
//                break;
//            case RAM_SpiFlagOverloadError:
//                SPIx->SR_f.OVRERF = 0u;
//                break;
//            default:
//                break;
//        }
//        enRet = Ok;
//    }

//    return enRet;
//}
#define RAM_IS_SPI_STD_FLAG(x)                                                     \
(   ((x) != 0UL)                            &&                                 \
    (((x) | SPI_FLAG_ALL) == SPI_FLAG_ALL))

__RAM_FUNC en_flag_status_t RAM_SPI_GetStatus(const CM_SPI_TypeDef *SPIx, uint32_t u32Flag)
{
    en_flag_status_t enFlag = RESET;
    RAM_DDL_ASSERT(RAM_IS_VALID_SPI_UNIT(SPIx));
    RAM_DDL_ASSERT(RAM_IS_SPI_STD_FLAG(u32Flag));

    if (0U != READ_REG32_BIT(SPIx->SR, u32Flag)) {
        enFlag = SET;
    }

    return enFlag;
}
#define RAM_IS_SPI_CLR_STD_FLAG(x)                                                 \
(   ((x) != 0UL)                            &&                                 \
    (((x) | SPI_FLAG_CLR_ALL) == SPI_FLAG_CLR_ALL))

__RAM_FUNC void RAM_SPI_ClearStatus(CM_SPI_TypeDef *SPIx, uint32_t u32Flag)
{
    RAM_DDL_ASSERT(RAM_IS_VALID_SPI_UNIT(SPIx));
    RAM_DDL_ASSERT(RAM_IS_SPI_CLR_STD_FLAG(u32Flag));

    CLR_REG32_BIT(SPIx->SR, u32Flag);
}

__RAM_FUNC void RAM_Spi_OverRun_Clear(void)
{
	if(SET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_OVERLOAD))RAM_SPI_ClearStatus(SPI_UNIT, SPI_FLAG_OVERLOAD);

//	if(RAM_SPI_GetFlag(RAM_SPI_UNIT, RAM_SpiFlagOverloadError))
//	{
//		RAM_SPI_ClearFlag(RAM_SPI_UNIT,RAM_SpiFlagOverloadError);
//	}

}



//int32_t RAM_SPI_Cmd(M4_SPI_TypeDef *SPIx, en_functional_state_t enNewSta)
//{
//    int32_t enRet = ErrorInvalidParameter;

//    /* Check parameters */
//    if(RAM_IS_VALID_SPI_UNIT(SPIx))
//    {
//        RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewSta));

//        SPIx->CR1_f.SPE = enNewSta;
//        enRet = Ok;
//    }

//    return enRet;
//}


//void RAM_PWC_Fcg1PeriphClockCmd(uint32_t u32Fcg1Periph, ram_en_functional_state_t enNewState)
//{
//    RAM_DDL_ASSERT(RAM_IS_PWC_FCG1_PERIPH(u32Fcg1Periph));
//    RAM_DDL_ASSERT(RAM_IS_FUNCTIONAL_STATE(enNewState));

//    if(RAM_ENABLE == enNewState)
//    {
//        CM_MSTP->FCG1 &= ~u32Fcg1Periph;
//    }
//    else
//    {
//        CM_MSTP->FCG1 |= u32Fcg1Periph;
//    }
//}
/* Parameter validity check for peripheral in fcg1. */
#define IS_FCG1_PERIPH(per)                                 \
(   ((per) != 0x00UL)                           &&          \
    (((per) | FCG_FCG1_PERIPH_MASK) == FCG_FCG1_PERIPH_MASK))
		
__RAM_FUNC void RAM_FCG_Fcg1PeriphClockCmd(uint32_t u32Fcg1Periph, en_functional_state_t enNewState)
{
    RAM_DDL_ASSERT(IS_FCG1_PERIPH(u32Fcg1Periph));
    RAM_DDL_ASSERT(IS_FUNCTIONAL_STATE(enNewState));

    if (ENABLE == enNewState) {
        CLR_REG32_BIT(CM_PWC->FCG1, u32Fcg1Periph);
    } else {
        SET_REG32_BIT(CM_PWC->FCG1, u32Fcg1Periph);
    }
}
	
__RAM_FUNC int32_t RAM_SPI_StructInit(stc_spi_init_t *pstcSpiInit)
{
    int32_t i32Ret = LL_ERR_INVD_PARAM;

    if (NULL != pstcSpiInit) {
        pstcSpiInit->u32WireMode          = SPI_4_WIRE;
        pstcSpiInit->u32TransMode         = SPI_FULL_DUPLEX;
        pstcSpiInit->u32MasterSlave       = SPI_MASTER;
        pstcSpiInit->u32ModeFaultDetect   = SPI_MD_FAULT_DETECT_DISABLE;
        pstcSpiInit->u32Parity            = SPI_PARITY_INVD;
        pstcSpiInit->u32SpiMode           = SPI_MD_0;
        pstcSpiInit->u32BaudRatePrescaler = SPI_BR_CLK_DIV8;
        pstcSpiInit->u32DataBits          = SPI_DATA_SIZE_8BIT;
        pstcSpiInit->u32FirstBit          = SPI_FIRST_MSB;
        pstcSpiInit->u32SuspendMode       = SPI_COM_SUSP_FUNC_OFF;
        pstcSpiInit->u32FrameLevel        = SPI_1_FRAME;
        i32Ret = LL_OK;
    }
    return i32Ret;
}
__RAM_FUNC void RAM_GPIO_SetPins(uint8_t u8Port, uint16_t u16Pin)
{
    __IO GPIO_REG_TYPE *POSRx;

    /* Parameter validity checking */
    RAM_DDL_ASSERT(IS_GPIO_PORT(u8Port));
    RAM_DDL_ASSERT(IS_GPIO_PIN(u16Pin));

    POSRx = &POSR_REG(u8Port);
    SET_REG_BIT(*POSRx, (GPIO_REG_TYPE)u16Pin);
}
__RAM_FUNC void RAM_GPIO_ResetPins(uint8_t u8Port, uint16_t u16Pin)
{
    __IO GPIO_REG_TYPE *PORRx;

    /* Parameter validity checking */
    RAM_DDL_ASSERT(IS_GPIO_PORT(u8Port));
    RAM_DDL_ASSERT(IS_GPIO_PIN(u16Pin));

    PORRx = &PORR_REG(u8Port);
    SET_REG_BIT(*PORRx, (GPIO_REG_TYPE)u16Pin);
}

/*! Parameter valid check for SPI command*/
#define IS_SPI_CMD_ALLOWED(x)                                                  \
(   (READ_REG32_BIT(SPIx->SR, SPI_FLAG_MD_FAULT) == 0UL)    ||                 \
    ((x) == DISABLE))

__RAM_FUNC void RAM_SPI_Cmd(CM_SPI_TypeDef *SPIx, en_functional_state_t enNewState)
{
    DDL_ASSERT(RAM_IS_VALID_SPI_UNIT(SPIx));
    DDL_ASSERT(IS_FUNCTIONAL_STATE(enNewState));
    DDL_ASSERT(IS_SPI_CMD_ALLOWED(enNewState));

    if (ENABLE == enNewState) {
        SET_REG32_BIT(SPIx->CR1, SPI_CR1_SPE);
    } else {
        CLR_REG32_BIT(SPIx->CR1, SPI_CR1_SPE);
    }
}
__RAM_FUNC void RAM_Spi_Config(void)
{
    stc_spi_init_t stcSpiInit;
		stc_gpio_init_t stcPortInit;
		//warning nedeni ile kapatildi//stc_irq_signin_config_t  stcIrqregiConf;
	
		SPI_DeInit(RAM_SPI_UNIT);

    /* configuration structure initialization */
//	RAM_MEM_ZERO_STRUCT(stcSpiInit);
//	RAM_MEM_ZERO_STRUCT(stcPortInit);

    /* Configuration peripheral clock */
						//FCG_Fcg1PeriphClockCmd(SPI_UNIT_CLOCK, ENABLE);
    RAM_FCG_Fcg1PeriphClockCmd(RAM_SPI_UNIT_CLOCK, (en_functional_state_t)RAM_ENABLE);

    /* Configuration SPI pin */
		RAM_PORT_Unlock();
    RAM_GPIO_SetFunc(SF_SCK_PORT, SF_SCK_PIN, SPI_SCK_FUNC);//, RAM_DISABLE);
    RAM_GPIO_SetFunc(SF_MOSI_PORT, SF_MOSI_PIN, SPI_MOSI_FUNC);//, RAM_DISABLE);
    RAM_GPIO_SetFunc(SF_MISO_PORT, SF_MISO_PIN, SPI_MISO_FUNC);//, RAM_DISABLE);
		RAM_PORT_Lock();
		RAM_SPI_StructInit(&stcSpiInit);
	
    /* Configuration SPI structure */
//    stcSpiInit.enClkDiv = SpiClkDiv2;   
//    stcSpiInit.enFrameNumber = SpiFrameNumber1;
//    stcSpiInit.enDataLength = SpiDataLengthBit8;
//    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
//		stcSpiInit.enWorkMode = SpiWorkMode3Line;
		
		stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV2;					//.enClkDiv = SpiClkDiv2;   
    stcSpiInit.u32FrameLevel        = SPI_1_FRAME;							//.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;				//.enDataLength = SpiDataLengthBit8;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;						//.enFirstBitPosition = SpiFirstBitPositionMSB;
		stcSpiInit.u32SpiMode           = SPI_MD_0;
																																//stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;
																																//stcSpiInit.enSckPhase = SpiSckOddSampleEvenChange;		
		//stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;			//in SPI_ReadBufConfig function
		stcSpiInit.u32WireMode          = SPI_3_WIRE;
		stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;					//enTransMode = SpiTransFullDuplex;
		//????!?!?!//stcSpiInit.enCommAutoSuspendEn = Disable;
    //????!?!?!//stcSpiInit.enModeFaultErrorDetectEn = Disable;
    //????!?!?!//stcSpiInit.enParitySelfDetectEn = Disable;
		//????!?!?!//stcSpiInit.enParityEn = Disable;
    stcSpiInit.u32Parity            = SPI_PARITY_INVD;					//.enParity = SpiParityEven;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;								//.enMasterSlaveMode = SpiModeMaster;
    
    //stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;//in SPI_DelayTimeConfig function
    ////stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;				//in SPI_DelayTimeConfig function
    //stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;	//in SPI_DelayTimeConfig function
    ////stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;					//in SPI_DelayTimeConfig function
    //stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;//in SPI_DelayTimeConfig function
    ////stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;		//in SPI_DelayTimeConfig function
		
		SPI_Init(RAM_SPI_UNIT, &stcSpiInit);
		////
		////
		SPI_ReadBufConfig(SPI_UNIT, SPI_RD_TARGET_RD_BUF);
		///
		stc_spi_delay_t	stc_SPI_Delay_Init;
		SPI_DelayStructInit(&stc_SPI_Delay_Init);
		stc_SPI_Delay_Init.u32SetupDelay		=SPI_SETUP_TIME_1SCK;
		stc_SPI_Delay_Init.u32IntervalDelay	=SPI_INTERVAL_TIME_6SCK;
		stc_SPI_Delay_Init.u32ReleaseDelay	=SPI_RELEASE_TIME_1SCK;
		
		SPI_DelayTimeConfig(SPI_UNIT, &stc_SPI_Delay_Init);
		////
		

		stcPortInit.u16PinAttr 				= PIN_ATTR_DIGITAL;// SPI Flash CS pin se?imi
		stcPortInit.u16PinDir  				= PIN_DIR_OUT;
		stcPortInit.u16PinDrv  				= PIN_HIGH_DRV;
		stcPortInit.u16PinOutputType 	= PIN_OUT_TYPE_CMOS;
		stcPortInit.u16PinState				= PIN_STAT_SET;
		
		RAM_PORT_Unlock();//WRITE_REG16(CM_GPIO->PWPR, GPIO_REG_UNLOCK_KEY);//LL_PERIPH_WE(LL_PERIPH_GPIO);
		RAM_GPIO_Init(SF_CS_PORT	,SF_CS_PIN	, &stcPortInit); // SPI	pinleri ilk etapta output
		RAM_GPIO_Init(SF_CS_PORT_2,SF_CS_PIN_2, &stcPortInit);		
		RAM_PORT_Lock();//WRITE_REG16(CM_GPIO->PWPR, GPIO_REG_LOCK_KEY);//LL_PERIPH_WP(LL_PERIPH_GPIO);
		RAM_sFLASH_CS_HIGH();
		
		RAM_SPI_Cmd(RAM_SPI_UNIT, ENABLE);
		
}


__RAM_FUNC void	RAM_sFlash_SPI_Busy_Wait(void)
{
	
			while (SET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_IDLE))//RAM_SpiFlagSpiIdle))//RAM_SPI_GetFlag
			{
				__NOP();
			}
}

__RAM_FUNC int32_t RAM_SPI_SendData8(CM_SPI_TypeDef *SPIx, uint8_t u8Data)
{
    int32_t enRet = LL_ERR_INVD_PARAM;

    /* Check parameters */
    if(RAM_IS_VALID_SPI_UNIT(SPIx))
    {
        SPIx->DR = u8Data;
        enRet = LL_OK;
    }

    return enRet;
}


__RAM_FUNC uint8_t RAM_SPI_ReceiveData8(const CM_SPI_TypeDef *SPIx)
{
    /* Check parameters */
    RAM_DDL_ASSERT(RAM_IS_VALID_SPI_UNIT(SPIx));

    return ((uint8_t)SPIx->DR);
}

__RAM_FUNC static uint8_t RAM_SpiFlash_WriteReadByte(uint8_t u8Data)
{
    uint8_t u8Byte;
	
		RAM_sFlash_SPI_Busy_Wait();

    /* Wait tx buffer empty */
    while (RESET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_TX_BUF_EMPTY))//RAM_SPI_GetFlag(RAM_SPI_UNIT, RAM_SpiFlagSendBufferEmpty))
    {
			RAM_Spi_OverRun_Clear();
    }
    /* Send data */
    RAM_SPI_SendData8(SPI_UNIT, u8Data);
    /* Wait rx buffer full */
//    while (RESET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_RX_BUF_FULL))//RAM_SPI_GetFlag(RAM_SPI_UNIT, RAM_SpiFlagReceiveBufferFull))
//    {
//			RAM_Spi_OverRun_Clear();
//    }
    /* Receive data */
    u8Byte = RAM_SPI_ReceiveData8(RAM_SPI_UNIT);

    return u8Byte;
}

//void RAM_SpiFlash_WriteEnable(uint8_t sFlashItem)
//{
//		RAM_sFlash_SPI_Busy_Wait();
//	
//    RAM_sFLASH_CS_LOW(sFlashItem);
//    RAM_SpiFlash_WriteReadByte(RAM_FLASH_INSTR_WRITE_ENABLE);
//    while (SET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_IDLE))//RAM_SPI_GetFlag(RAM_SPI_UNIT, RAM_SpiFlagSpiIdle))
//    {
//    }
//    RAM_sFLASH_CS_HIGH();
//}


//void RAM_CLK_GetClockFreq(stc_clk_freq_t *pstcClkFreq)
//{
//    uint32_t plln = 0u, pllp = 0u, pllm = 0u, pllsource = 0u;

//    if(NULL != pstcClkFreq)
//    {
//        /* Get system clock. */
//        switch(M4_SYSREG->CMU_CKSWR_f.CKSW)
//        {
//            case ClkSysSrcHRC:
//                /* HRC used as system clock. */
//                pstcClkFreq->sysclkFreq = HRC_VALUE;
//                break;
//            case ClkSysSrcMRC:
//                /* MRC used as system clock. */
//                pstcClkFreq->sysclkFreq = MRC_VALUE;
//                break;
//            case ClkSysSrcLRC:
//                /* LRC used as system clock. */
//                pstcClkFreq->sysclkFreq = LRC_VALUE;
//                break;
//            case ClkSysSrcXTAL:
//                /* XTAL used as system clock. */
//                pstcClkFreq->sysclkFreq = XTAL_VALUE;
//                break;
//            case ClkSysSrcXTAL32:
//                /* XTAL32 used as system clock. */
//                pstcClkFreq->sysclkFreq = XTAL32_VALUE;
//                break;
//            default:
//                /* MPLLP used as system clock. */
//                pllsource = M4_SYSREG->CMU_PLLCFGR_f.PLLSRC;
//                pllp = M4_SYSREG->CMU_PLLCFGR_f.MPLLP;
//                plln = M4_SYSREG->CMU_PLLCFGR_f.MPLLN;
//                pllm = M4_SYSREG->CMU_PLLCFGR_f.MPLLM;

//                /* PLLCLK = ((pllsrc / pllm) * plln) / pllp */
//                if (ClkPllSrcXTAL == pllsource)
//                {
//                    pstcClkFreq->sysclkFreq = (XTAL_VALUE)/(pllm+1u)*(plln+1u)/(pllp+1u);
//                }
//                else if (ClkPllSrcHRC == pllsource)
//                {
//                    pstcClkFreq->sysclkFreq = (HRC_VALUE)/(pllm+1u)*(plln+1u)/(pllp+1u);
//                }
//                else
//                {
//                    //else
//                }
//                break;
//        }

//        /* Get hclk. */
//        pstcClkFreq->hclkFreq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.HCLKS;

//        /* Get exck. */
//        pstcClkFreq->exckFreq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.EXCKS;

//        /* Get pclk0. */
//        pstcClkFreq->pclk0Freq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.PCLK0S;

//        /* Get pclk1. */
//        pstcClkFreq->pclk1Freq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.PCLK1S;

//        /* Get pclk2. */
//        pstcClkFreq->pclk2Freq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.PCLK2S;

//        /* Get pclk3. */
//        pstcClkFreq->pclk3Freq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.PCLK3S;

//        /* Get pclk4. */
//        pstcClkFreq->pclk4Freq = pstcClkFreq->sysclkFreq >> M4_SYSREG->CMU_SCFGR_f.PCLK4S;
//    }
//    else
//    {
//        /* code */
//    }
//}
#define PLL_SRC                         ((CM_CMU->PLLCFGR & CMU_PLLCFGR_PLLSRC) >> CMU_PLLCFGR_PLLSRC_POS)

__RAM_FUNC void RAM_GetClockFreq(stc_clock_freq_t *pstcClockFreq)
{
    stc_clock_scale_t *pstcClockScale;
    uint32_t u32HrcValue;
    uint32_t plln;
    uint32_t pllp;
    uint32_t pllm;

    switch (READ_REG8_BIT(CM_CMU->CKSWR, CMU_CKSWR_CKSW)) {
        case CLK_SYSCLK_SRC_HRC:
            /* HRC is used to system clock */
            pstcClockFreq->u32SysclkFreq = HRC_VALUE;
            break;
        case CLK_SYSCLK_SRC_MRC:
            /* MRC is used to system clock */
            pstcClockFreq->u32SysclkFreq = MRC_VALUE;
            break;
        case CLK_SYSCLK_SRC_LRC:
            /* LRC is used to system clock */
            pstcClockFreq->u32SysclkFreq = LRC_VALUE;
            break;
        case CLK_SYSCLK_SRC_XTAL:
            /* XTAL is used to system clock */
            pstcClockFreq->u32SysclkFreq = XTAL_VALUE;
            break;
        case CLK_SYSCLK_SRC_XTAL32:
            /* XTAL32 is used to system clock */
            pstcClockFreq->u32SysclkFreq = XTAL32_VALUE;
            break;
        case CLK_SYSCLK_SRC_PLL:
            /* PLLHP is used as system clock. */
            pllp = (uint32_t)((CM_CMU->PLLCFGR & CMU_PLLCFGR_MPLLP) >> CMU_PLLCFGR_MPLLP_POS);
            plln = (uint32_t)((CM_CMU->PLLCFGR & CMU_PLLCFGR_MPLLN) >> CMU_PLLCFGR_MPLLN_POS);
            pllm = (uint32_t)((CM_CMU->PLLCFGR & CMU_PLLCFGR_MPLLM) >> CMU_PLLCFGR_MPLLM_POS);
            /* pll = ((pllin / pllm) * plln) / pllp */
            if (CLK_PLL_SRC_XTAL == PLL_SRC) {
                pstcClockFreq->u32SysclkFreq = ((XTAL_VALUE / (pllm + 1UL)) * (plln + 1UL)) / (pllp + 1UL);
            } else {
                u32HrcValue = HRC_VALUE;
                pstcClockFreq->u32SysclkFreq = ((u32HrcValue / (pllm + 1UL)) * (plln + 1UL)) / (pllp + 1UL);
            }
            break;
        default:
            break;
    }

    pstcClockScale = (stc_clock_scale_t *)((uint32_t)&CM_CMU->SCFGR);
    pstcClockScale->SCFGR = READ_REG32(CM_CMU->SCFGR);
    /* Get hclk. */
    pstcClockFreq->u32HclkFreq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.HCLKS;
    /* Get pclk1. */
    pstcClockFreq->u32Pclk1Freq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.PCLK1S;
    /* Get pclk4. */
    pstcClockFreq->u32Pclk4Freq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.PCLK4S;
    /* Get pclk3. */
    pstcClockFreq->u32Pclk3Freq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.PCLK3S;
    /* Get exck. */
    pstcClockFreq->u32ExclkFreq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.EXCKS;
    /* Get pclk0. */
    pstcClockFreq->u32Pclk0Freq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.PCLK0S;
    /* Get pclk2. */
    pstcClockFreq->u32Pclk2Freq = pstcClockFreq->u32SysclkFreq >> pstcClockScale->SCFGR_f.PCLK2S;
}




__RAM_FUNC int32_t RAM_SpiFlash_WaitForWriteEnd(uint8_t sFlashItem)
{
    int32_t enRet = LL_OK;
    uint8_t u8Status = 0u;
    uint32_t u32Timeout;
    stc_clock_freq_t stcClkFreq;

	
    RAM_GetClockFreq(&stcClkFreq);//RAM_CLK_GetClockFreq(&stcClkFreq);
    u32Timeout = stcClkFreq.u32SysclkFreq / 1000u;
	
		RAM_sFlash_SPI_Busy_Wait();
	
    RAM_sFLASH_CS_LOW(sFlashItem);
    RAM_SpiFlash_WriteReadByte(RAM_FLASH_INSTR_READ_SR1);
    do
    {
        u8Status = RAM_SpiFlash_WriteReadByte(RAM_FLASH_DUMMY_BYTE_VALUE);
        u32Timeout--;
    } while ((u32Timeout != 0ul) &&
             ((u8Status & RAM_FLASH_BUSY_BIT_MASK) == RAM_FLASH_BUSY_BIT_MASK));

    if (RAM_FLASH_BUSY_BIT_MASK == u8Status)
    {
        enRet = LL_ERR_TIMEOUT;
    }
    while (SET == RAM_SPI_GetStatus(RAM_SPI_UNIT, SPI_FLAG_IDLE))//RAM_SPI_GetFlag(RAM_SPI_UNIT, RAM_SpiFlagSpiIdle))
    {
    }
    RAM_sFLASH_CS_HIGH();

    return enRet;
}


__RAM_FUNC int32_t RAM_SpiFlash_ReadData(uint32_t u32Addr,uint8_t cmd  /*uint8_t pData[], uint16_t len*/)
{
    int32_t enRet = LL_OK;
    //warning nedeni ile kapatildi//uint16_t u16Index = 0u;
		uint8_t adrlen=0;
		char sendBufData[5]={0,0,0,0,0};
	
		//////////////
		uint8_t sFlashItem=RAM_AddressToChipOrder(u32Addr,&u32Addr);//?ip sira numarasi alinir ve ?ip 2 ise adres verisi g?ncellenir
		if(sFlashItem==0)return LL_ERR;//non specific error
		//////////////
	
			RAM_sFlash_SPI_Busy_Wait();

			adrlen=RAM_sFlashAddressPushToMessage(u32Addr,cmd,sFlashItem,sendBufData);	

        /* Send data to flash */
        RAM_sFLASH_CS_LOW(sFlashItem);
        RAM_SpiFlash_WriteReadByte(sendBufData[0]);//(cmd/*RAM_FLASH_INSTR_STANDARD_READ*/);
        RAM_SpiFlash_WriteReadByte(sendBufData[1]);//((uint8_t)((u32Addr & 0xFF0000ul) >> 16u));
        RAM_SpiFlash_WriteReadByte(sendBufData[2]);//((uint8_t)((u32Addr & 0xFF00u) >> 8u));
        RAM_SpiFlash_WriteReadByte(sendBufData[3]);//((uint8_t)(u32Addr & 0xFFu));
		if(adrlen==5)
				RAM_SpiFlash_WriteReadByte(sendBufData[4]);//((uint8_t)(u32Addr & 0xFFu));
		
				RAM_sFlash_SPI_Busy_Wait();

    return enRet;
}

__RAM_FUNC static uint8_t RAM_sFLASH_SendByte(uint8_t byte)
{
    uint8_t rx;
//    RAM_HAL_SPI_TransmitReceive(&SPI_HandleStructure, &byte, &rx, 1, 1000);
		RAM_SPI_SendData8(RAM_SPI_UNIT, byte);
    return rx;
}	
	
__RAM_FUNC static void RAM_sFLASH_SendCmdAddress(uint8_t cmd, uint32_t flashAddress)
{//tekil komutlar i?in komut g?nderme kismi!
	
			RAM_SpiFlash_ReadData(flashAddress,cmd);
}
__RAM_FUNC void RAM_sFLASH_ReadBufferProc(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{	//spi kontroll? iken okuma yaptiran kisim. islem s?rerken veri okumayi saglar. sadece ?zel durumda kullanilir
	//kullanim esnasinda islem kontrol? ?st yordamdadir
			/*!< Select the FLASH: Chip Select low */
//			RAM_sFLASH_CS_LOW();
			RAM_Spi_OverRun_Clear();
			/*!< Send "Read from Memory " instruction & ReadAddr*/
			RAM_sFLASH_SendCmdAddress(sFLASH_CMD_ReadData,ReadAddr);
	
		

			while(NumByteToRead--)  /*!< while there is data to be read */
			{
					RAM_Spi_OverRun_Clear();
					/*!< Read a byte from the FLASH */
					*pBuffer = RAM_SpiFlash_WriteReadByte(RAM_FLASH_DUMMY_BYTE_VALUE);
					/*!< Point to the next location where the byte read will be saved */
					pBuffer++;
			}

			/*!< Deselect the FLASH: Chip Select high */
			RAM_sFLASH_CS_HIGH();
}	
	
__RAM_FUNC	bool RAM_sFLASH_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
	{
			//Mesgul degilse buradayiz
			RAM_sFLASH_ReadBufferProc(pBuffer, ReadAddr, NumByteToRead);
		
			
		return true;
	}
	
//bool RAM_sFLASH_WaitForWriteEnd(void)
//{
//// tekil komut //tekil komutlar DMA i?erikli iletisim kurmazlar!
//			uint8_t flashstatus = 0;
//			/*!< Loop as long as the memory is busy with a write cycle */
//			/*!< Select the FLASH: Chip Select low */
//			RAM_sFLASH_CS_LOW();
//			/*!< Send "Read Status Register" instruction */
//			RAM_sFLASH_SendByte(sFLASH_CMD_ReadStatusReg);
//			do
//			{
//					/*!< Send a dummy byte to generate the clock needed by the FLASH
//					and put the value of the status register in FLASH_Status variable */
//					flashstatus = RAM_sFLASH_SendByte(sFLASH_DUMMY_BYTE);
//			}
//			while((flashstatus & sFLASH_FLAG_WriteInProcess) == 1/*SET*/);  /* Write in progress */ //BUSY check//
//			/*!< Deselect the FLASH: Chip Select high */
//			RAM_sFLASH_CS_HIGH();

//	return true;
//}
/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
__RAM_FUNC void RAM_sFLASH_WriteByteZero(uint32_t WriteAddr)
{
	//	bool		writeComplete=false;
    //uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

//////////////
		uint8_t sFlashItem=RAM_AddressToChipOrder(WriteAddr,NULL);//?ip sira numarasi alinir ve ?ip 2 ise adres verisi g?ncellenir
		if(sFlashItem==0)return;//non specific error
		//////////////	
	
			RAM_sFLASH_CS_LOW(sFlashItem);

			/*!< Send "Write Enable" instruction */
			RAM_sFLASH_SendByte(sFLASH_CMD_WriteEnable);

    /*!< Deselect the FLASH: Chip Select high */
			RAM_sFLASH_CS_HIGH();
			/*!< Select the FLASH: Chip Select low */
			RAM_sFLASH_CS_LOW(sFlashItem);
			/*!< Send "Write to Memory " instruction & WriteAddr*/
			RAM_sFLASH_SendCmdAddress(sFLASH_CMD_PageProgram,WriteAddr);
		
			/*!< while there is data to be written on the FLASH */

			RAM_sFLASH_SendByte(0);
			/*!< Point on the next byte to be written */

			/*!< Deselect the FLASH: Chip Select high */
			RAM_sFLASH_CS_HIGH();
			/*!< Wait the end of Flash writing */
			RAM_SpiFlash_WaitForWriteEnd(sFlashItem);
}	
/* Stub-page0 flash budget: keep IAP LED animation out of permanent stub. */
__RAM_FUNC void RAM_Slider_Lights(void)
{
#ifdef IWDG_ENABLE
	(void)RAM_WDT_FeedDog();
#endif
}


/**
 * @brief  De-initialize the CRC.
 * @param  None
 * @retval None
 */
//void RAM_CRC_DeInit(void)
//{
//    WRITE_REG32(CM_CRC->CR, CRC_CR_RST_VALUE);
//}
#define CRC_CR_RST_VALUE                (0x001CUL)

__RAM_FUNC void RAM_CRC_Init(const stc_crc_init_t *pstcCrcInit)
{
//    int32_t i32Ret = LL_ERR_INVD_PARAM;

//    if (NULL != pstcCrcInit) {
//        DDL_ASSERT(IS_CRC_PROTOCOL(pstcCrcInit->u32Protocol));
        WRITE_REG32(CM_CRC->CR, CRC_CR_RST_VALUE);//RAM_CRC_DeInit();

//        DDL_ASSERT(IS_CRC_REFIN(pstcCrcInit->u32RefIn));
//        DDL_ASSERT(IS_CRC_REFOUT(pstcCrcInit->u32RefOut));
//        DDL_ASSERT(IS_CRC_XOROUT(pstcCrcInit->u32XorOut));

        WRITE_REG32(CM_CRC->CR, (pstcCrcInit->u32RefIn | pstcCrcInit->u32RefOut | pstcCrcInit->u32XorOut));

        MODIFY_REG32(CM_CRC->CR, CRC_CRC32, pstcCrcInit->u32Protocol);

        /* Set initial value */
//        if (CRC_CRC32 == pstcCrcInit->u32Protocol) {
            WRITE_REG32(CM_CRC->RESLT, pstcCrcInit->u32InitValue);
//        } else {
//            WRITE_REG16(CM_CRC->RESLT, pstcCrcInit->u32InitValue);
//        }

//        i32Ret = LL_OK;
//    }
//    return i32Ret;
}

#define IS_FCG0_PERIPH(per)                                 \
(   ((per) != 0x00UL)                           &&          \
    (((per) | FCG_FCG0_PERIPH_MASK) == FCG_FCG0_PERIPH_MASK))
#define IS_FCG0_UNLOCKED()      ((CM_PWC->FCG0PC & PWC_FCG0PC_PRT0) == PWC_FCG0PC_PRT0)

__RAM_FUNC void RAM_FCG_Fcg0PeriphClockCmd(uint32_t u32Fcg0Periph, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_FCG0_PERIPH(u32Fcg0Periph));
    DDL_ASSERT(IS_FUNCTIONAL_STATE(enNewState));
    DDL_ASSERT(IS_FCG0_UNLOCKED());

    if (ENABLE == enNewState) {
        CLR_REG32_BIT(CM_PWC->FCG0, u32Fcg0Periph);
    } else {
        SET_REG32_BIT(CM_PWC->FCG0, u32Fcg0Periph);
    }
}

__RAM_FUNC void RAM_Crc_Config(void)
{		
		WRITE_REG32(CM_PWC->FCG0PC, PWC_FCG0_REG_UNLOCK_KEY);//PWC_FCG0_REG_Unlock();//LL_PERIPH_WE(LL_PERIPH_FCG);////////////////Ram e uygun sekilde yeniden d?zenlenmeli
		SET_REG16_BIT(CM_PWC->FPRC, PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1);// PWC_REG_Unlock(PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1);
    RAM_FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_CRC, (en_functional_state_t)RAM_ENABLE);		
		CM_PWC->FPRC = (PWC_WRITE_ENABLE | (uint16_t)((uint16_t)(~(PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1)) & (CM_PWC->FPRC)));	//PWC_REG_Lock(PWC_UNLOCK_CODE0 | PWC_UNLOCK_CODE1);
		WRITE_REG32(CM_PWC->FCG0PC, PWC_FCG0_REG_LOCK_KEY);//PWC_FCG0_REG_Lock();//LL_PERIPH_WP(LL_PERIPH_FCG);////////////////
	
		stc_crc_init_t stcCrcInit;
		stcCrcInit.u32Protocol = CRC_CRC32;
		stcCrcInit.u32InitValue = CRC32_INIT_VALUE;
		stcCrcInit.u32RefIn = CRC_REFIN_ENABLE;
		stcCrcInit.u32RefOut = CRC_REFOUT_ENABLE;
		stcCrcInit.u32XorOut = CRC_XOROUT_DISABLE;
	
		RAM_CRC_Init(&stcCrcInit);
//		RAM_CRC_Init(CRC_CRC32 | CRC_REFIN_ENABLE | CRC_REFOUT_ENABLE | CRC_XOROUT_DISABLE);
//		M4_CRC->RESLT = 0xFFFFFFFF;
}

//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
/* Make STM32F CRC compatible with windows/winzip/winrar
Calculate CRC32 of DWORD data array.
Input:
u32 *dworddata -- the array point
u32 dwordcount -- the data len in DWORD
Output:
u32 CRC32 -- the result
**********************/
__RAM_FUNC void RAM_CalcCRC32Init(void)
{
		RAM_Crc_Config();
}


//void RAM_CRC_Calculate32BContinuous(const uint32_t *pu32Data, uint32_t u32Length)
//{
//    uint32_t u32Ret = 0u;
//    uint32_t u32Count;

//		if ((NULL != pu32Data) && (u32Length != 0UL))
//    {
//        for (u32Count = 0u; u32Count < u32Length; u32Count++)
//        {
//            RW_MEM32(CM_CRC->DAT0) = pu32Data[u32Count];
//        }

//        u32Ret = CM_CRC->RESLT;
//				//statik o nedenle ?agrilamaz//CRC_Accumulate(CRC_DATA_WIDTH_32BIT, pu32Data, u32Length);
//    }
//}
__RAM_FUNC int32_t RAM_CRC_WriteData32(const uint32_t au32Data[], uint32_t u32Len)
{
    uint32_t i;
    int32_t i32Ret = LL_ERR_INVD_PARAM;
    const uint32_t u32DataAddr = ((uint32_t)(&CM_CRC->DAT0));//CRC_DATA_ADDR;

    if ((au32Data != NULL) && (u32Len != 0UL)) {
        for (i = 0UL; i < u32Len; i++) {
            RW_MEM32(u32DataAddr) = au32Data[i];
        }
        i32Ret = LL_OK;
    }

    return i32Ret;
}
__RAM_FUNC uint32_t RAM_CRC_Accumulate32(const void *pvData, uint32_t u32Len)
{
    uint32_t u32CrcValue = 0UL;

    if ((pvData != NULL) && (u32Len != 0UL)) {

        /* Write data */        
        (void)RAM_CRC_WriteData32((const uint32_t *)pvData, u32Len);
        /* Get checksum */       
          u32CrcValue = READ_REG32(CM_CRC->RESLT);
       
    }

    return u32CrcValue;
}
__RAM_FUNC uint32_t RAM_CRC_AccumulateData32(const uint32_t au32Data[], uint32_t u32Len)
{
    uint32_t u32CrcValue = 0UL;

    if ((au32Data != NULL) && (u32Len != 0UL)) {
        u32CrcValue = RAM_CRC_Accumulate32(au32Data, u32Len);
    }

    return u32CrcValue;
}
__RAM_FUNC uint32_t RAM_CalcCRC32Get(void)
{
	uint32_t ui32=0;
	if((CM_CRC->FLG & 0x00000001)==0)//if(CM_CRC->FLG_f.FLAG==0)         //Huada 32bit CRC sonucu alma biti bit 0 ise hazir 1 ise hatali
	{
		ui32=CM_CRC->RESLT;
		ui32 ^= 0xffffffff; //xor with 0xffffffff
	}
  return ui32;
}

//void RAM_CalcCRC32Proc(uint32_t *dworddata, uint32_t dwordcount)
//{
//		//RAM_CRC_Calculate32BContinuous(dworddata,dwordcount);  // Huada 32 Bit CRC 
//		RAM_CRC_AccumulateData32(dworddata, dwordcount);
//}

/////////////////////////////////////////////////////
/* Product zone-FS CRC helpers removed from public reference. */






__RAM_FUNC void RAM_sFLASH_WipeFwSlot(boot_fw_slot_t slot)
{
#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)
    uint16_t sec_first;
    uint16_t sec_last;
    uint16_t sec;
    uint32_t addr;
    uint32_t tick = 0U;

    if(slot == BOOT_FW_SLOT_GOOD)
    {
        sec_first = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
        sec_last = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_LAST;
    }
    else
    {
        sec_first = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_FIRST;
        sec_last = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_LAST;
    }

    for(sec = sec_first; sec <= sec_last; sec++)
    {
#ifdef IWDG_ENABLE
        RAM_WDT_FeedDog();
#endif
        if((tick & 16U) != 0U)
        {
            RAM_GPIO_SetPins(WD_CTRL_GPIO_Port, WD_CTRL_Pin);
        }
        else
        {
            RAM_GPIO_ResetPins(WD_CTRL_GPIO_Port, WD_CTRL_Pin);
        }
        tick++;

        addr = ((uint32_t)(sec) * (uint32_t)BOOT_SPI_SECTOR_SIZE);
        RAM_sFLASH_WriteByteZero(addr);
        RAM_sFLASH_WriteByteZero(addr);
    }
#else
    (void)slot;
#endif
}

__RAM_FUNC void RAM_WipeFwSlotAndReset(boot_fw_slot_t slot)
{
    RAM_sFLASH_WipeFwSlot(slot);

    __DSB();
    SCB->AIRCR = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                            (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk);
    __DSB();
    for(;;)
    {
        __NOP();
    }
}





#endif /* BOOT_MCU_HC32F460 */
