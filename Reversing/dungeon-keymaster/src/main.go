package main

import (
	"fmt"
	"html/template"
	"net/http"
	"os"
	"strings"
)

var tmpl = template.Must(template.New("index").Parse(`
<html>
<head>
	<title>Dungeon Challenge</title>
</head>
<style>
body {
  background-image: url('https://s3.amazonaws.com/dungeon-keymaster.2024.sunshinectf.games/dungeon_entrance.jpg');
}
h1,p,form,label {
	background-color: white
}
</style>
<body>
	<h1>Welcome to the Dungeon Challenge!</h1>
	<p>Only those with the correct key may enter and retrieve the treasure.</p>
	<form action="/" method="POST">
		<label for="key">Enter your key:</label><br>
		<input type="text" id="key" name="key" required><br><br>
		<input type="submit" value="Submit Key">
	</form>
	{{if .}}
	<p>{{.}}</p>
	{{end}}
</body>
</html>
`))

var dungeon_question_one = "/dungeon"
var dungeon_question_two = "8734"
var dungeon_question_three = "http://localhost:8734"

func main() {
	http.HandleFunc("/", handleForm)
	http.HandleFunc(dungeon_question_one, handleDungeon)

	fmt.Println("Server started at " + dungeon_question_three)
	http.ListenAndServe(":"+dungeon_question_two, nil)
}

func handleForm(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		key := r.FormValue("key")
		_, message := checkKey(strings.TrimSpace(key))
		tmpl.Execute(w, message)
	} else {
		tmpl.Execute(w, nil)
	}
}

func checkKey(input string) (bool, string) {
	// Validate the key format: it should be string-string-string
	parts := strings.Split(input, "-")
	if len(parts) != 3 {
		return false, "Invalid key format. It must be in the format: string-string-string."
	}

	// Check if the derived key matches the expected correctDerivedKey
	if input == dungeon_question_one+"-"+dungeon_question_two+"-"+dungeon_question_three {
		return true, "Correct key! You may enter the dungeon at /dungeon."
	} else {
		return false, "Incorrect key. You are not worthy to enter the dungeon."
	}
}

func handleDungeon(w http.ResponseWriter, r *http.Request) {
	if r.Method == "GET" {
		key := r.URL.Query().Get("key")
		// Validate the key format again for the dungeon route
		parts := strings.Split(key, "-")
		if len(parts) != 3 {
			fmt.Fprintf(w, `
<style>
body {
  background-image: url('https://s3.amazonaws.com/dungeon-keymaster.2024.sunshinectf.games/dungeon_failure.jpg');
}
h1,p,form,label {
	background-color: white
}
</style>
<h1>Access Denied!</h1>
<p>Invalid key format.</p>
`)
			return
		}

		var res, _ = checkKey(key)
		if res {
			flag, err := os.ReadFile("flag.txt")
			if err != nil {
				http.Error(w, "Flag not found!", http.StatusInternalServerError)
				return
			}
			fmt.Fprintf(w, "<h1>Dungeon Unlocked!</h1><pre>%s</pre>", flag)
		} else {
			fmt.Fprintf(w, `
<style>
body {
  background-image: url('https://s3.amazonaws.com/dungeon-keymaster.2024.sunshinectf.games/dungeon_failure.jpg');
}
h1,p,form,label {
	background-color: white
}
</style>
<h1>Access Denied!</h1>
<p>Incorrect key.</p>`)
		}
	}
}
