#!/usr/bin/env bash

ORIGDIR=$(pwd)
FLAG=$(cat flag.txt)
MSG=$(base64 flag.txt)
TMPDIR=/tmp/ctf
mkdir -p $TMPDIR

# Generate an x.509 certificate authority (CA) and a server certificate signed by the CA.

cd $TMPDIR || exit

# CA
openssl genrsa -out ca.key 2048
openssl req -new -key ca.key -out ca.csr -subj "/C=US/ST=FL/L=Orlando/O=CTF/OU=CTF/CN=CTF"
openssl x509 -req -in ca.csr -signkey ca.key -out ca.crt -days 3650

# Server Certificate with the message "ctf{}" in the x509v3 extensions
openssl genrsa -out server.key 2048

echo -e "
[ req ]
default_bits = 2048
prompt = no
distinguished_name = req_distinguished_name
req_extensions = req_ext

[ req_distinguished_name ]
C = US
ST = FL
L = Orlando
O = CTF
OU = CTF
CN = CTF

[ custom_ext ]
subjectAltName = @alt_names

[ OIDs ]
FlagOID = 1.3.6.1.4.1.56337.1

[ req_ext ]
1.3.6.1.4.1.56337.1=ASN1:UTF8:$MSG

[ alt_names ]
DNS.1 = localhost

" >certconfig.cnf

openssl req -new -config certconfig.cnf -key server.key -out server.csr

# Print out the information in the certificate request
echo "<< Certificate Request >>"
openssl req -in server.csr -text -noout

openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.pem -copy_extensions=copyall

# Print out the information in the certificate including all the extensions
echo "<< Server Certificate >>"
TEXT=$(openssl x509 -in server.pem -text -noout)

echo "$TEXT"

echo "Checking solution..."

FLAGVAL=$(echo "$TEXT" | grep -a1 56337 | tr -d '[:space:]' | cut -d':' -f3)

# Remove the prefix of .0
CUTPREFIX=$(echo $FLAGVAL | cut -c 3-)

echo "Cleaned Base64: $CUTPREFIX"

RESFLAG=$(echo "$CUTPREFIX" | base64 -d)
echo "Flag: $RESFLAG"

if [ "$RESFLAG" == "$FLAG" ]; then
  echo "Flag is correct!"
else
  echo "Flag is incorrect!"
fi

cd "$ORIGDIR" || exit
mkdir -p attachments
cp $TMPDIR/server.pem ./attachments/server.pem

# Clean up
rm -rf $TMPDIR
