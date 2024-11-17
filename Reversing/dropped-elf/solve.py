
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

if __name__ == "__main__":
	assembled_binary = b""

	for i in NAME_ORDER:
		piece_file = open(f"dropped/{i}", "rb")
		assembled_binary += piece_file.read()
		piece_file.close()

	out_file = open("reconstructed", "wb")
	out_file.write(assembled_binary)
	out_file.close()