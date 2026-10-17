SHELL := /bin/bash

TARGET := ./prog

INCLUDES := \
	-I dotenv

CXX := g++
CXX_FLAGS := \
	-O3 \
	-Wall -Wextra -Wpedantic \
	-std=c++20

default:
	@ $(CXX) $(INCLUDES) $(CXX_FLAGS) -o $(TARGET) main.cpp
	@ $(TARGET)
