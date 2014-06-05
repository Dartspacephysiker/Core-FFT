################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../complex-1chan.o \
../core-fft.o \
../core-fft_wrap.o \
../real-1chan.o \
../real-nchan.o \
../uint8-1chan.o 

C_SRCS += \
../complex-1chan.c \
../core-fft.c \
../core-fft_wrap.c \
../real-1chan.c \
../real-nchan.c \
../uint8-1chan.c 

OBJS += \
./complex-1chan.o \
./core-fft.o \
./core-fft_wrap.o \
./real-1chan.o \
./real-nchan.o \
./uint8-1chan.o 

C_DEPS += \
./complex-1chan.d \
./core-fft.d \
./core-fft_wrap.d \
./real-1chan.d \
./real-nchan.d \
./uint8-1chan.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


