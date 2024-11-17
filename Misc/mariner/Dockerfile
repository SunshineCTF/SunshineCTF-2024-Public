# Stage 1: Use a base image to install and set up K3s
FROM rancher/k3s:v1.27.4-k3s1

# Set environment variables for the K3s cluster
ENV PRODUCT=k3s
ENV DATA_DIR=/var/lib/rancher/${PRODUCT}

# Expose K3s API server port
EXPOSE 6443

# Start the K3s server with the provided certificates and disable the agent
CMD ["server", "--tls-san", "your.server.domain.com", "--write-kubeconfig-mode", "644", "--disable-agent"]
RUN k3s server --tls-san your.server.domain.com \
    --write-kubeconfig-mode 644 \
    --disable-agent & \
    sleep 5 && \
    kubectl create secret generic flag \
    --from-literal=value='sun{YALLLLLAMANINERONTHEHIGHSEES}'
