SHELL := /bin/bash

DIR_INPUT := files

CXX := g++

INCLUDES := \
	-I dotenv \
	-I dotjson

CXX_FLAGS := \
	-O3 \
	-Wall -Wextra -Wpedantic \
	-std=c++20

TARGET := ./prog

INPUTS := \
	$(DIR_INPUT)/.env \
	$(DIR_INPUT)/.json

default:
	@ $(CXX) $(INCLUDES) $(CXX_FLAGS) -o $(TARGET) main.cpp
	@ $(TARGET) $(INPUTS)
