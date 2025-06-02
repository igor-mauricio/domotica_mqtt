#!/bin/bash

# Generate the .env file with the host IP
rm .env
echo "HOST_IP=$(hostname -I | awk '{print $1}')" > .env

# Run docker-compose
sudo docker-compose up