from collections import Counter

wordlist = [
    "Voyage", "Odyssey", "Expedition", "Quest", "Pilgrimage", "Trail", 
    "Journey", "Adventure", "Wanderlust", "Sojourn", "Crossing", "Trek", 
    "Passage", "Exploration", "Wander", "Traverse", "Discovery", "Pathway", 
    "Venture", "Escapade", "Migration", "Field", "Pursuit", "Roaming", 
    "Travel", "Survivor", "Bridge", "River", "Valley", "Ridge"
]

wordlist = [word.lower() for word in wordlist]

def analyze_frequency(filename):
    with open(filename, 'r', encoding='utf-8') as file: text = file.read().lower()
    words = text.split()
    word_count = Counter(word for word in words if word in wordlist)
    total_occurrences = sum(word_count.values())
    sorted_word_count = sorted(word_count.items(), key=lambda x: x[1], reverse=True)

    print("Frequency Analysis (Percentage):")
    for word, count in sorted_word_count:
        percentage = (count / total_occurrences) * 100 if total_occurrences > 0 else 0
        print(f"{word}: {count} ({percentage:.2f}%)")

# Specify the file to analyze
file_to_analyze = 'attachments/letter'

# Perform frequency analysis
analyze_frequency(file_to_analyze)

# Using the freq analysis we can narrow down the mappings