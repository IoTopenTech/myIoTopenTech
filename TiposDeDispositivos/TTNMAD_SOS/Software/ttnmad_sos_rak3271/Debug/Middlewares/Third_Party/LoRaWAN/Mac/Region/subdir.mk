################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.c \
../Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.c 

OBJS += \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.o \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.o 

C_DEPS += \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.d \
./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LoRaWAN/Mac/Region/%.o Middlewares/Third_Party/LoRaWAN/Mac/Region/%.su: ../Middlewares/Third_Party/LoRaWAN/Mac/Region/%.c Middlewares/Third_Party/LoRaWAN/Mac/Region/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I../LoRaWAN/App -I../LoRaWAN/Target -I../Utilities/trace/adv_trace -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Mac-2f-Region

clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Mac-2f-Region:
	-$(RM) ./Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/Region.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAS923.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionAU915.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionBaseUS.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A20.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470A26.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B20.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN470B26.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCN779.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionCommon.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU433.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionEU868.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionIN865.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionKR920.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionRU864.su ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.d ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.o ./Middlewares/Third_Party/LoRaWAN/Mac/Region/RegionUS915.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-LoRaWAN-2f-Mac-2f-Region

