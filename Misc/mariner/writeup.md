# CTF Challenge Writeup: Exploiting K3s API to Retrieve Secrets

## Overview

In this challenge, we exploited a Kubernetes K3s API to retrieve a sensitive secret (ctfflag). This was done by generating a client certificate and private key, which we used to authenticate against the K3s server. The overall objective was to retrieve the flag from a Kubernetes secret named ctfflag.

## Steps to Solve the Challenge

## Setting Up Variables
    We start by defining several important file paths and API endpoint variables:
        CLIENT_CA_CERT_PATH and CLIENT_CA_KEY_PATH: Paths to the client CA certificate and key, used to sign the client certificate.
        CLIENT_CERT_PATH and CLIENT_KEY_PATH: Paths where the generated client certificate and private key will be stored.
        K3S_API: The endpoint of the K3s API server (in this case, https://127.0.0.1:6443).

### Step 1: Generate a Private Key for the Client

    We use OpenSSL to generate a 2048-bit RSA private key for the client. This key will be required to create a Certificate Signing Request (CSR).
    Command:

    openssl genrsa -out client.key 2048

### Step 2: Create a Certificate Signing Request (CSR)

    After generating the private key, the next step is to create a CSR. The CSR is used to request a certificate from the CA.
    The CSR subject field specifies the identity of the client as system:admin and assigns the client to the system:masters group, which grants administrative privileges in Kubernetes.
    Command:

    openssl req -new -key client.key -out client.csr -subj "/CN=system:admin/O=system:masters"

### Step 3: Sign the CSR with the Client CA Certificate

    Once the CSR is ready, it is signed by the Client CA's certificate and key. This process generates a valid client certificate, which can be used to authenticate against the K3s API.
    The certificate is signed to be valid for 365 days.
    Command:

    openssl x509 -req -in client.csr \
      -CA ${CLIENT_CA_CERT_PATH} \
      -CAkey ${CLIENT_CA_KEY_PATH} \
      -CAcreateserial \
      -out client.crt \
      -days 365 -sha256

### Step 4: Use the Client Certificate to Interact with the K3s API

    Once the client certificate is generated, it can be used to make authenticated requests to the K3s API.
    In this case, the script retrieves the Kubernetes secret containing the flag (ctfflag) by leveraging the client certificate.

## Retrieving the Kubernetes Secret (Flag)

    The script attempts to establish a connection to the K3s API (127.0.0.1:6444) using openssl s_client. This step retrieves the server CA certificate (server-ca.crt).

    With the server certificate retrieved, the kubectl command is used to query the ctfflag secret.

    Command:

    kubectl --kubeconfig=config get secret ctfflag -o jsonpath="{.data.flag}" | base64 -d

    This command retrieves the flag stored in the ctfflag secret by decoding the base64-encoded value.

## Cleanup

    After successfully retrieving the flag, the script performs cleanup by removing any generated files, including the client certificate, private key, and CSR.
    Commands:

    rm client.crt
    rm client.key
    rm *.srl
    rm *.csr
    rm server-ca.crt

## Lessons Learned

    Client Authentication: By generating and signing a certificate with a valid CA, we were able to authenticate as a privileged Kubernetes user (system:admin).
    Kubernetes Secrets: The ctfflag was stored in a Kubernetes secret, accessible only to users with the appropriate permissions.
    Certificates in Kubernetes: Understanding how Kubernetes uses client certificates for authentication is crucial for interacting with the API in secure environments.

## Conclusion

The challenge required exploiting the K3s API by generating a client certificate and using it to retrieve the secret ctfflag. Through careful use of OpenSSL and kubectl, we were able to achieve the goal and obtain the flag successfully.
