#!/bin/sh
set -e

TLS_DIR=/var/lib/rancher/k3s/server/tls
mkdir -p "$TLS_DIR"

# Pre-seed the (leaked) client CA so admin certs signed with it authenticate.
cp /seed/client-ca.crt "$TLS_DIR/client-ca.crt"
cp /seed/client-ca.key "$TLS_DIR/client-ca.key"

# Start the k3s API server in the background.
k3s server \
    --disable-agent \
    --write-kubeconfig-mode 644 \
    --tls-san "${TLS_SAN:-mariner.ctf.hackucf.org}" &
K3S_PID=$!

# Wait for the API to come up, then install the flag secret.
echo "[entrypoint] waiting for k3s API..."
i=0
until kubectl get --raw /readyz >/dev/null 2>&1; do
    i=$((i+1))
    if [ "$i" -gt 120 ]; then
        echo "[entrypoint] k3s API did not become ready in time" >&2
        break
    fi
    sleep 2
done

if kubectl get secret flag >/dev/null 2>&1; then
    echo "[entrypoint] flag secret already present"
else
    kubectl create secret generic flag \
        --from-literal=value="$(cat /flag.txt)" \
        && echo "[entrypoint] flag secret created"
fi

wait "$K3S_PID"
