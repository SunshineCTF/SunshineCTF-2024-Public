import string

alphabet = 'abcdefghijklmnopqrstuvwxyz_{} '

wordlist = [
    "Voyage",
    "Odyssey",
    "Expedition",
    "Quest",
    "Pilgrimage",
    "Trail",
    "Journey",
    "Adventure",
    "Wanderlust",
    "Sojourn",
    "Crossing",
    "Trek",
    "Passage",
    "Exploration",
    "Wander",
    "Traverse",
    "Discovery",
    "Pathway",
    "Venture",
    "Escapade",
    "Migration",
    "Field",
    "Pursuit",
    "Roaming",
    "Travel",
    "Survivor",
    "Bridge",
    "River",
    "Valley",
    "Ridge"
]

mapping = {alphabet[i]: wordlist[i] for i in range(len(alphabet))}
reverse_mapping = {v: k for k, v in mapping.items()}

def substitute_text(text):
    substituted_text = []
    for char in text:
        if char in mapping:
            substituted_text.append(mapping[char])
        elif char in string.punctuation:
            if char not in "{}":
                substituted_text.append(char)
        else:
            substituted_text.append(char)

    return ' '.join(substituted_text)

def reverse_substitute(text):
    words = text.split()
    decrypted_text = []
    for word in words:
        if word in reverse_mapping:
            decrypted_text.append(reverse_mapping[word])
        elif word in string.punctuation:
            decrypted_text.append(word)
        else:
            decrypted_text.append(word)

    return ''.join(decrypted_text)
    
with open('plaintext', 'r') as ptf: pt = ptf.read().lower()
with open('letter', 'w') as cpf: cpf.write(substitute_text(pt))