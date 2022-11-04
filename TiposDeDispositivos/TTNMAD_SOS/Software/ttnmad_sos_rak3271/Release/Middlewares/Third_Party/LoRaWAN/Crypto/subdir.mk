################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LoRaWAN/Crypto/cmac.c \
../Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.c \
../Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.c 

OBJS += \
./Middlewares/Third_Party/LoRaWAN/Crypto/cmac.o \
./Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.o \
./Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.o 

C_DEPS += \
./Middlewares/Third_Party/LoRaWAN/Crypto/cmac.d \
./Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.d \
./Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LoRaWAN/Crypto/%.o Middlewares/Third_Party/LoRaWAN/Crypto/%.su: ../Middlewares/Third_Party/LoRaWAN/Crypto/%.c Middlewares/Third_Party/LoRaWAN/Crypto/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I../LoRaWAN/App -I../LoRaWAN/Target -I../Utilities/trace/adv_trace -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Crypto

clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Crypto:
	-$(RM) ./Middlewares/Third_Party/LoRaWAN/Crypto/cmac.d ./Middlewares/Third_Party/LoRaWAN/Crypto/cmac.o ./Middlewares/Third_Party/LoRaWAN/Crypto/cmac.su ./Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.d ./Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.o ./Middlewares/Third_Party/LoRaWAN/Crypto/lorawan_aes.su ./Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.d ./Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.o ./Middlewares/Third_Party/LoRaWAN/Crypto/soft-se.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Crypto

