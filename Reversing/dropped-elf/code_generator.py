import random

hex_chars = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']

def randstring(size: int) -> str:
	return_string: str = ""
	for i in range(size):
		return_string += chr(random.randint(0x61, 0x7a))
	return return_string

def randhexstring(size: int) -> str:
	return_string: str = "0x"
	for i in range(size):
		return_string += random.choice(hex_chars)
	return return_string

def generate_callstack_sequence(depth: int):
	base = randstring(10)
	for i in range(depth):
		print(f"void {base}{str(i)}(int x)   {{ {base}{str(i+1)}(x); }} ")

def generate_xor_sequence(depth: int):
	base = randstring(10)
	print(f"int {base}(int x) {{\n")
	for i in range(depth):
		print(f"\tx ^= {randhexstring(8)};")
	print("\treturn x;\n\n}")


if __name__ == "__main__":
	print(f"{randhexstring(8)}")
	generate_callstack_sequence(100)
	generate_xor_sequence(100)
	generate_callstack_sequence(100)
	generate_xor_sequence(100)
	generate_callstack_sequence(100)
	generate_xor_sequence(100)
	generate_callstack_sequence(100)
	generate_xor_sequence(100)
	generate_callstack_sequence(100)
	generate_xor_sequence(100)