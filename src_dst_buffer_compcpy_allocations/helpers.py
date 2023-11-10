import csv
import os
import glob
from array import *
import sys
import random
import argparse


def reverse(arr):
    return arr[::-1]

def hex_to_bin(hex_address): 
    binary_address = bin(int(hex_address, 16))
    # print (reverse(binary_address))
    return reverse(binary_address[2:])
