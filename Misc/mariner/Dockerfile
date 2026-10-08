# K3s-based challenge. The player is given the cluster's client CA (an
# ECDSA key/cert pair, see attachments/) and must sign a client certificate
# with CN=system:admin / O=system:masters to authenticate to the Kubernetes
# API and read the `flag` secret.
#
# Note: the original deployment shipped a pre-built image whose k3s datastore
# already contained the flag secret. That cannot be reproduced at image-build
# time (k3s needs to run privileged, which buildkit disallows), so instead we
# pre-seed the leaked client CA and create the flag secret at runtime via the
# entrypoint below.
FROM rancher/k3s:v1.27.4-k3s1

ENV PRODUCT=k3s
ENV DATA_DIR=/var/lib/rancher/${PRODUCT}

# The leaked client CA that players use to forge an admin certificate.
COPY attachments/client-ca.crt /seed/client-ca.crt
COPY attachments/client-ca.key /seed/client-ca.key
COPY flag.txt /flag.txt
COPY docker-entrypoint.sh /docker-entrypoint.sh

EXPOSE 6443

ENTRYPOINT ["/bin/sh", "/docker-entrypoint.sh"]
