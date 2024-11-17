package main

import (
	"bytes"
	"encoding/base64"
	"fmt"
	"html/template"
	"io"
	"net/http"
	"os"
	"strings"

	"filippo.io/age"
	log "github.com/sirupsen/logrus"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

var ageKey *age.X25519Identity

func setupAgeKey() *age.X25519Identity {
	newKey, err := age.GenerateX25519Identity()
	if err != nil {
		log.Fatal(err)
	}
	return newKey
}

func main() {
	log.SetLevel(log.DebugLevel)
	ageKey = setupAgeKey()

	// http.HandleFunc("/hello", helloHandler)
	http.HandleFunc("/", indexHandler)
	http.HandleFunc("/ws", wsHandler)

	fmt.Println("Server is running on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func parseTemplate(path string) (*template.Template, error) {
	t, err := template.ParseFiles(path)
	if err != nil {
		return nil, err
	}
	return t, nil
}

func indexHandler(w http.ResponseWriter, r *http.Request) {
	// Template index.html with the public key
	t, err := parseTemplate("html/index.html")
	if err != nil {
		log.Fatal(err)
	}

	// Execute the template with the public key
	err = t.Execute(w, ageKey.Recipient().String())
	if err != nil {
		log.Fatal(err)
	}
}

func wsHandler(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}
	defer conn.Close()

	for {
		converse(conn)
	}
}

type websocketConn struct {
	*websocket.Conn
}

func (conn *websocketConn) ReadString() (int, string, error) {
	msgType, p, err := conn.ReadMessage()
	if err != nil {
		log.Println(err)
		return -1, "", err
	}
	log.Debugf("Received message: %s", p)
	return msgType, string(p), nil
}

func converse(conn *websocket.Conn) {

	wsc := &websocketConn{conn}
	msgType, msg, err := wsc.ReadString()
	if err != nil {
		log.Println(err)
		return
	}

	// Decrypt the message
	msgReader := bytes.NewReader([]byte(msg))
	plaintextR, err := age.Decrypt(msgReader, ageKey)
	if err != nil {
		log.Println(err)
		conn.WriteMessage(websocket.TextMessage, []byte("don't recognize that message"))
		return
	}

	buf, err := readerToString(plaintextR)
	if err != nil {
		log.Error(err)
		return
	}

	buf = strings.TrimSpace(buf)
	log.Debugf("Decrypted message: %s", buf)

	// Generate recipient with the public key from buf
	recipient, err := age.ParseX25519Recipient(buf)
	if err != nil {
		conn.WriteMessage(msgType, []byte("what kind of key is this?"))
		log.Errorf("Error parsing recipient: %s", err)
		return
	}

	// Generate encrypted flag message
	flag := getFlag()
	flagMsg := fmt.Sprintf("I told you I'd get it to you once we were on a secure line: %s", flag)
	out, err := encryptString(flagMsg, recipient)
	if err != nil {
		log.Errorf("Error encrypting flag: %s", err)
		return
	}

	flagEncryptedEncoded, err := bytesToBase64(out)
	if err != nil {
		log.Errorf("Error encoding encrypted flag: %s", err)
		return
	}

	log.Debugf("base64-encoded encryptedflag: %s", flagEncryptedEncoded)

	// Send the encrypted flag
	if err := conn.WriteMessage(msgType, []byte(flagEncryptedEncoded)); err != nil {
		log.Println(err)
		return
	}
}

func bytesToBase64(b []byte) (string, error) {
	// Type assert w to *bytes.Buffer if possible
	buf := bytes.NewBuffer(b)

	// Encode the buffer content to base64
	encoded := base64.StdEncoding.EncodeToString(buf.Bytes())
	return encoded, nil
}

func encryptString(flag string, recipient age.Recipient) ([]byte, error) {
	out := &bytes.Buffer{}
	w, err := age.Encrypt(out, recipient)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	_, err = w.Write([]byte(flag))
	if err != nil {
		log.Println(err)
		return nil, err
	}
	w.Close()

	return out.Bytes(), nil
}

func readFile(path string) ([]byte, error) {
	// read the file
	file, err := os.ReadFile(path)
	if err != nil {
		log.Fatal(err)
	}
	return file, nil
}

func getFlag() string {
	path := "flag.txt"
	flag, err := readFile(path)
	if err != nil {
		log.Fatal(err)
	}
	return string(flag)
}

func readerToString(plaintextR io.Reader) (string, error) {
	buf := new(strings.Builder)
	_, err := io.Copy(buf, plaintextR)
	if err != nil {
		log.Println(err)
		return "", err
	}
	return buf.String(), nil
}
