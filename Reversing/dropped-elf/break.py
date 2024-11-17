import os

OUT_DIR = "dropped"

NAME_ORDER = [
				2,
				4,
				0,
				9,
				3,
				5,
				10,
				1,
				8,
				7,
				6,
				]

def split_chunk(file_contents: bytes, id: int, start: int, end: int) -> bytes:
	chunk = file_contents[start:end]
	out_file = open(f"{OUT_DIR}/{str(id)}", "wb")
	out_file.write(chunk)
	out_file.close()

if __name__ == "__main__":
	inp_file = open("dropped_elf", "rb")
	file_contents: bytes = inp_file.read()
	inp_file.close()

	print(len(file_contents))

	if not os.path.exists(OUT_DIR):
		os.makedirs(OUT_DIR)

	chunks = []

	split_chunk(file_contents, 2, 0x00, 0x300)
	split_chunk(file_contents, 4, 0x300, 0x440)
	split_chunk(file_contents, 0, 0x440, 0x510)
	split_chunk(file_contents, 9, 0x510, 0x610)
	split_chunk(file_contents, 3, 0x610, 0x800)
	split_chunk(file_contents, 5, 0x800, 0x1020)
	split_chunk(file_contents, 10, 0x1020, 0x2000)
	split_chunk(file_contents, 1, 0x2000, 0x4000)
	split_chunk(file_contents, 8, 0x4000, 0x7000)
	split_chunk(file_contents, 7, 0x7000, 0xcfb8)
	split_chunk(file_contents, 6, 0xcfb8, len(file_contents) - 1)


	print("What's up?")
