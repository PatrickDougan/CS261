#!/usr/bin/env python3

import binascii

from Crypto.Hash import SHA256
from Crypto.Random import get_random_bytes

from typing import Iterator

with open('flag', 'r') as f:
    flag = f.read().strip()

def sha(data: bytes) -> bytes:
    algo = SHA256.new(data)
    return algo.digest()

def xor(a: bytes, b: bytes) -> bytes:
    return bytes([x ^ y for x, y in zip(a, b)])

# Evenly split 'data' into 'n' parts
def split_str(data: bytes, n: int) -> Iterator[bytes]:
    if len(data) % n != 0:
        print(f"Length of 'data' must be a multiple of {n}")
        raise ValueError

    section_len = len(data) // n
    return (data[i * section_len : (i + 1) * section_len] for i in range(n))

def secure_hash_tm(data: bytes) -> bytes:
    a, b, c = split_str(data, 3)
    w = xor(b, xor(sha(a), sha(c)))
    return xor(sha(w), a) + sha(c)


def to_hex(data: bytes) -> str:
    return binascii.hexlify(data).decode()

rnd = get_random_bytes(32 * 3)
rnd_hash = secure_hash_tm(rnd)

print("Welcome to secure_hash_TM bug bounty program! If you can find a second preimage to my hash, you will get a reward!")
input("Press Enter to continue to your challenge\n")

print(f"I hashed the hexstring {to_hex(rnd)} using secure_hash_tm(), and my output was {to_hex(rnd_hash)} (feel free to check if you don't believe me)\n")
print(f"Please find me a value 'x' such that secure_hash_tm(x) == {to_hex(rnd_hash)} but x != {to_hex(rnd)} to demonstrate that you found a vulnerability in the hashing algorithm (pfft, I know it's fully secure so you'll never get it!)\n")

try:
    x = bytes.fromhex(input("Please provide your value for x (in hexstring form): "))
except ValueError as e:
    print(f"Invalid input: {e}")
    exit(1)

x_hash = secure_hash_tm(x)
if x != rnd:
    if  x_hash == rnd_hash:
        print(f"Alright, fine: you found a preimage. Have your flag, just don't tell my Cryptography professor about this: {flag}")
    else:
        print(f"When I ran secure_hash_tm(x), I got {to_hex(x_hash)}, which does not equal {to_hex(rnd_hash)}")
else:
    print("Haha, you copied the value of x from rnd! That's not a second preimage ;)")
