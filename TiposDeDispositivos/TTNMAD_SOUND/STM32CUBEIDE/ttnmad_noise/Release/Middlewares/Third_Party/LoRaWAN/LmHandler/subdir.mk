################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.c \
../Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.c 

OBJS += \
./Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.o \
./Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.o 

C_DEPS += \
./Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.d \
./Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LoRaWAN/LmHandler/%.o Middlewares/Third_Party/LoRaWAN/LmHandler/%.su: ../Middlewares/Third_Party/LoRaWAN/LmHandler/%.c Middlewares/Third_Party/LoRaWAN/LmHandler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../LoRaWAN/App -I../LoRaWAN/Target -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/trace/adv_trace -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-LmHandler

clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-LmHandler:
	-$(RM) ./Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.d ./Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.o ./Middlewares/Third_Party/LoRaWAN/LmHandler/LmHandler.su ./Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.d ./Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.o ./Middlewares/Third_Party/LoRaWAN/LmHandler/NvmDataMgmt.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-LmHandler

