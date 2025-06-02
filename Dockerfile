# Use a lightweight base image for Raspberry Pi
FROM golang:1.24.2-bullseye

# Set the working directory inside the container
WORKDIR /app

# Copy the Go module files and download dependencies
COPY go.mod go.sum ./
RUN go mod download

# Copy the rest of the application code
COPY . .

# Build the Go application for ARM architecture (Raspberry Pi)
RUN GOOS=linux GOARCH=arm GOARM=7 go build -o main .

# Expose the port (if needed for debugging or other purposes)
EXPOSE 1883

# Command to run the application
CMD ["./main"]